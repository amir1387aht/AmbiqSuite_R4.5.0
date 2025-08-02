#include "./accl_sensor.h"

// SPI pins
#define ACCL_CS     12
#define ACCL_MOSI   48
#define ACCL_MISO   49
#define ACCL_SCK    47
#define ACCL_INT1   61
#define ACCL_INT2   62

// SPI configuration: 8 MHz, Mode 3 (CPOL=1, CPHA=1)
am_hal_iom_config_t AcclSens_spiSettings = {
    .eInterfaceMode = AM_HAL_IOM_SPI_MODE,
    .ui32ClockFreq = AM_HAL_IOM_8MHZ,
    .eSpiMode = AM_HAL_IOM_SPI_MODE_3,
    .ui32NBTxnBufLength = 0,
    .pNBTxnBuf = NULL,
};

// Function to write a single byte to a register
void acclSens_write(uint8_t reg, uint8_t value)
{
    am_hal_iom_transfer_t trx = { 0 };
    uint8_t tx_data[2] = { reg & 0x7F, value }; // MSB=0 for write

    digitalWrite(ACCL_CS, 0);

    trx.eDirection = AM_HAL_IOM_TX;
    trx.ui32InstrLen = 0;
    trx.ui64Instr = 0;
    trx.ui32NumBytes = 2;
    trx.pui32TxBuffer = (uint32_t*)tx_data;
    trx.bContinue = false;

    am_hal_iom_blocking_transfer(AcclSens_pIomHandle, &trx);

    digitalWrite(ACCL_CS, 1);
}

// Function to read multiple bytes from a register
void acclSens_read(uint8_t reg, uint8_t* rxBuf, uint32_t len)
{
    am_hal_iom_transfer_t trx = { 0 };
    uint8_t cmd = 0x80 | reg;  // Set MSB for read operation

    digitalWrite(ACCL_CS, 0);

    // PHASE 1: Send register address
    trx.eDirection = AM_HAL_IOM_TX;
    trx.ui32InstrLen = 0;
    trx.ui64Instr = 0;
    trx.ui32NumBytes = 1;
    trx.pui32TxBuffer = (uint32_t*)&cmd;
    trx.bContinue = true;    // Keep CS low
    int err = am_hal_iom_blocking_transfer(AcclSens_pIomHandle, &trx);
    if (err)
    {
        am_util_stdio_printf("[AcclSens ERROR] Write reg 0x%02X failed: %d\r\n", reg, err);
        digitalWrite(ACCL_CS, 1);
        return;
    }

    // PHASE 2: Read data
    trx.eDirection = AM_HAL_IOM_RX;
    trx.ui32NumBytes = len;
    trx.pui32RxBuffer = (uint32_t*)rxBuf;
    trx.bContinue = false;   // Release CS at end

    err = am_hal_iom_blocking_transfer(AcclSens_pIomHandle, &trx);
    if (err) am_util_stdio_printf("[AcclSens ERROR] Read reg 0x%02X failed: %d\r\n", reg, err);

    digitalWrite(ACCL_CS, 1);
}

// Function to initialize the accelerometer sensor
void acclSens_init()
{
    am_hal_gpio_pinconfig(ACCL_MISO, g_AM_BSP_GPIO_IOM5_MISO);
    am_hal_gpio_pinconfig(ACCL_MOSI, g_AM_BSP_GPIO_IOM5_MOSI);
    am_hal_gpio_pinconfig(ACCL_SCK, g_AM_BSP_GPIO_IOM5_SCK);

    pinMode(ACCL_CS, 1);
    digitalWrite(ACCL_CS, 1);
    pinMode(ACCL_INT1, 0);
    pinMode(ACCL_INT2, 0);

    if (am_hal_iom_initialize(ui32ModuleAcclSens, &AcclSens_pIomHandle) ||
        am_hal_iom_power_ctrl(AcclSens_pIomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(AcclSens_pIomHandle, &AcclSens_spiSettings) ||
        am_hal_iom_enable(AcclSens_pIomHandle))
    {
        am_util_stdio_printf("[AcclSens ERROR] SPI init failed\r\n");
        return;
    }

    uint8_t device_id = 0;
    acclSens_read(0x0F, &device_id, 1);

    if (!(IsAcclSensorInitSuccessfully = device_id == 0x6C)) {
        am_util_stdio_printf("[AcclSens ERROR] Couldn't identify the AcclSens. RI: 0x%02X\r\n", device_id);
        return;
    }

    // Configure accelerometer and gyroscope
    // CTRL1_XL (0x10): Accelerometer settings
    // ODR_XL = 104 Hz (0x40), FS_XL = ±2g (0x00)
    acclSens_write(0x10, 0x40);

    // CTRL2_G (0x11): Gyroscope settings
    // ODR_G = 104 Hz (0x40), FS_G = ±250 dps (0x00)
    acclSens_write(0x11, 0x40);

    am_util_stdio_printf("[AcclSens INFO] LSM6DSO initialized successfully\r\n");
}

// Function to read accelerometer and gyroscope data
void acclSens_task(void* pvParameters) {
    if (!IsAcclSensorInitSuccessfully) {
        vTaskDelay(NULL);
        return;
    }

    uint8_t data[12];
    int16_t ax, ay, az, gx, gy, gz;

    while (1) {
        // Read 12 bytes starting from OUTX_L_G (0x22)
        acclSens_read(0x22, data, 12);

        // Gyroscope data
        gx = (int16_t)(data[1] << 8 | data[0]);
        gy = (int16_t)(data[3] << 8 | data[2]);
        gz = (int16_t)(data[5] << 8 | data[4]);

        // Accelerometer data
        ax = (int16_t)(data[7] << 8 | data[6]);
        ay = (int16_t)(data[9] << 8 | data[8]);
        az = (int16_t)(data[11] << 8 | data[10]);

        // Convert raw data to physical units
        // Accelerometer: ±2g => 0.061 mg/LSB
        float ax_g = ax * 0.000061f;
        float ay_g = ay * 0.000061f;
        float az_g = az * 0.000061f;

        // Gyroscope: ±250 dps => 8.75 mdps/LSB
        float gx_dps = gx * 0.00875f;
        float gy_dps = gy * 0.00875f;
        float gz_dps = gz * 0.00875f;

        // Print the results
        am_util_stdio_printf("Accel [g]: X=%.3f Y=%.3f Z=%.3f | Gyro [dps]: X=%.3f Y=%.3f Z=%.3f\r\n",
            ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps);

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}