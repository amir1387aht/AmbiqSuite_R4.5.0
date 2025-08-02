#include "./light_sensor.h"

am_hal_iom_config_t LightSens_sti2cSettings =
{
    .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
    .ui32ClockFreq = AM_HAL_IOM_400KHZ,
    .eSpiMode = AM_HAL_IOM_SPI_MODE_0,
    .ui32NBTxnBufLength = 0,
    .pNBTxnBuf = NULL,
};

uint8_t lightSens_getNormValue(void) {
    uint8_t dataOut[50];

    // Read sensor data
    lightSens_transceive(0x66, (uint32_t*)&dataOut, 4);
    lightSens_transmit(0x60, 0x22); // Clear FIFO

    // Extract light value from sensor data
    uint16_t light_value = ((uint16_t)dataOut[0] << 8) | dataOut[1];

    // If the raw light value hasn't changed significantly, return the current target brightness
    if (abs((int)light_value - (int)last_raw) < RAW_TOLERANCE)
        return last_output;

    // Update stored value
    last_raw = light_value;

    // Non-linear mapping using a logarithmic scale that centers around NORMAL_LIGHT
    float normalized;

    if (light_value <= 0)
        normalized = OUTPUT_MIN;
    else {
        // Use a non-linear formula to give better results in typical indoor lighting conditions
        // This formula ensures NORMAL_LIGHT maps to DEFAULT_TARGET
        float ratio = (float)light_value / NORMAL_LIGHT;

        // Apply sensitivity factor to the logarithmic curve
        float adjusted_ratio = powf(ratio, SENSITIVITY);

        // Map to output range, ensuring NORMAL_LIGHT maps to DEFAULT_TARGET
        normalized = OUTPUT_MIN + (DEFAULT_TARGET - OUTPUT_MIN) * adjusted_ratio;

        // Cap at OUTPUT_MAX
        if (normalized > OUTPUT_MAX) normalized = OUTPUT_MAX;
    }

    last_output = (uint8_t)(normalized + 0.5f); // round

    return last_output;
}

void lightSens_setTolerance(uint16_t tolerance) {
    RAW_TOLERANCE = tolerance;
}

void lightSens_setSensitivity(float sensitivity) {
    if (sensitivity > 0.1f && sensitivity < 10.0f)
        SENSITIVITY = sensitivity;
}

void lightSens_task(void* pvParameters) {
    // Delay between sensor readings (2 seconds)
    const TickType_t xLongDelay = pdMS_TO_TICKS(SENSOR_READ_TIME_MS);
    // Short delay for transitions (20ms)
    const TickType_t xShortDelay = pdMS_TO_TICKS(20);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // Get the target brightness value from sensor
        uint8_t target = lightSens_getNormValue();

        // If brightness needs to change significantly, perform smooth transition
        if (abs((int)target - (int)current_brightness) > 1) {

            // Calculate step size and direction
            int8_t direction = (target > current_brightness) ? 1 : -1;
            uint8_t total_change = abs(target - current_brightness);
            float step_size = (float)total_change / (TRANSITION_TIME_MS / 20); // 20ms intervals

            // Ensure minimum step size of 1 if there's any difference
            if (step_size < 1.0f) step_size = 1.0f;

            // Transition loop
            TickType_t transitionWakeTime = xTaskGetTickCount();
            while (current_brightness != target) {
                // Update brightness by calculated step size
                if (direction > 0) {
                    // Increasing brightness
                    uint8_t step = (uint8_t)(step_size + 0.5f);
                    if (current_brightness + step >= target)
                        current_brightness = target;
                    else current_brightness += step;
                }
                else {
                    // Decreasing brightness
                    uint8_t step = (uint8_t)(step_size + 0.5f);
                    if (current_brightness - step <= target)
                        current_brightness = target;
                    else current_brightness -= step;
                }

                // Apply the new brightness
                display_set_brightness(current_brightness);

                // Short delay for transition updates
                vTaskDelayUntil(&transitionWakeTime, xShortDelay);
            }
        }
        else if (current_brightness != target) {
            // For very small changes, update immediately
            current_brightness = target;
            display_set_brightness(current_brightness);
        }

        // Reset wake time for the long delay cycle
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, xLongDelay);
    }
}

void lightSens_transmit(uint8_t cmd, uint8_t data) {
    uint8_t outBuff[2];
    outBuff[0] = cmd;
    outBuff[1] = data;
    am_hal_iom_transfer_t Transaction;

    Transaction.ui32InstrLen = 0;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = 2;
    Transaction.pui32TxBuffer = (uint32_t *)&outBuff;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x45;
    Transaction.bContinue = 0;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    int error_ret = am_hal_iom_blocking_transfer(LightSens_pIomHandle, &Transaction);
    if (error_ret) am_util_stdio_printf("Light Sensor I2C_STATUS_ERROR 1: %i\r\n", error_ret);
}

void lightSens_transceive(uint8_t cmd, uint32_t *buffer, int readLen) {
    am_hal_iom_transfer_t Transaction;

    Transaction.ui32InstrLen = 0;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = 1;
    Transaction.pui32TxBuffer = (uint32_t *)&cmd;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x45;
    Transaction.bContinue = 1;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if (am_hal_iom_blocking_transfer(LightSens_pIomHandle, &Transaction))
    {
        am_util_stdio_printf("Light Sensor I2C_STATUS_ERROR 1\r\n");
        return;
    }

    Transaction.ui32InstrLen = 0;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes = readLen;
    Transaction.pui32RxBuffer = buffer;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x45;
    Transaction.bContinue = 0;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if (am_hal_iom_blocking_transfer(LightSens_pIomHandle, &Transaction))
    {
        am_util_stdio_printf("Light Sensor I2C_STATUS_ERROR 2\r\n");
        return;
    }
}

void lightSens_init() {
    pinMode(80, 0);
    if (am_hal_iom_initialize(ui32ModuleLightSens, &LightSens_pIomHandle) ||
        am_hal_iom_power_ctrl(LightSens_pIomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(LightSens_pIomHandle, &LightSens_sti2cSettings) ||
        am_hal_iom_enable(LightSens_pIomHandle))
        am_util_stdio_printf("Light Sensor AM_DEVICES_I2C_STATUS_ERROR\r\n");
    else
    {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_SCL, g_AM_BSP_GPIO_IOM0_SCL);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_SDA, g_AM_BSP_GPIO_IOM0_SDA);
        uint8_t dataOut[50];
        lightSens_transceive(0x3E, (uint32_t *)&dataOut, 1);
        lightSens_transceive(0x3E, (uint32_t *)&dataOut, 1);
        lightSens_transmit(0x80, 0xFF);
        lightSens_transmit(0x02, 0x20);
        lightSens_transmit(0x04, 0x00);
        lightSens_transmit(0x05, 0x40);
        lightSens_transmit(0x10, 0x00);
        lightSens_transmit(0x4e, 0x26);
        lightSens_transmit(0x6f, 0x14);
        lightSens_transmit(0xa1, 0x03);
        lightSens_transmit(0xa5, 0x00);
        lightSens_transmit(0xdb, 0x00);
        lightSens_transmit(0x60, 0xa2);
        lightSens_transmit(0x61, 0x00);
        lightSens_transmit(0x62, 0x00);
        lightSens_transmit(0x63, 0x00);
        lightSens_transmit(0xf6, 0x09);
        lightSens_transmit(0xf1, 0x00);
        lightSens_transmit(0x60, 0x22); // Clear FIFO
        lightSens_transmit(0x00, 0x02); // Mode Enable
        lightSens_transceive(0x64, (uint32_t *)&dataOut, 1);// Read if fifo is full = 1
        lightSens_transceive(0x65, (uint32_t *)&dataOut, 1);// Read amount of data in fifo
    }
}