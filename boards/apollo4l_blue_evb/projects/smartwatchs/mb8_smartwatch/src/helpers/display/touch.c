#include "./touch.h"

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

uint32_t ui32ModuleTouch = 2;
void *touch_pIomHandle;

am_hal_iom_config_t touch_i2cSettings = 
{
    .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
    .ui32ClockFreq = AM_HAL_IOM_400KHZ,
    .eSpiMode = AM_HAL_IOM_SPI_MODE_0,
    .ui32NBTxnBufLength = 0,
    .pNBTxnBuf = NULL,
};

void touch_transmit(uint16_t cmd, uint32_t *buffer, int sendLen)
{
    uint16_t cmdtemp = cmd >> 8 | ((cmd << 8) & 0xff);

    am_hal_iom_transfer_t Transaction;

    Transaction.ui32InstrLen = 2;
    Transaction.ui64Instr = cmdtemp;
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = sendLen;
    Transaction.pui32TxBuffer = buffer;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x48;
    Transaction.bContinue = 0;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if (am_hal_iom_blocking_transfer(touch_pIomHandle, &Transaction))
    {
        am_util_stdio_printf("Touch I2C_STATUS_ERROR");
        return;
    }
}

void touch_transceive(uint16_t cmd, uint32_t *buffer, int readLen)
{
    am_hal_iom_transfer_t Transaction;

    Transaction.ui32InstrLen = 0;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = 2;
    Transaction.pui32TxBuffer = (uint32_t *)&cmd;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x48;
    Transaction.bContinue = 1;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if (am_hal_iom_blocking_transfer(touch_pIomHandle, &Transaction))
    {
        am_util_stdio_printf("Touch I2C_STATUS_ERROR 1\r\n");
        return;
    }

    Transaction.ui32InstrLen = 0;
    Transaction.ui64Instr = 0;
    Transaction.eDirection = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes = readLen;
    Transaction.pui32RxBuffer = buffer;
    Transaction.uPeerInfo.ui32I2CDevAddr = 0x48;
    Transaction.bContinue = 0;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if (am_hal_iom_blocking_transfer(touch_pIomHandle, &Transaction))
    {
        am_util_stdio_printf("Touch I2C_STATUS_ERROR 2\r\n");
        return;
    }
}

uint8_t _touch_data[16] = {0};

void touch_read_data(uint16_t *touch_x, uint16_t *touch_y, bool *touch_available) {
    touch_transceive(0x0010, (uint32_t *)&_touch_data, 16);

    if((*touch_available = _touch_data[8] == 2)) {
        *touch_x = (_touch_data[4] << 8 | _touch_data[3]);
        *touch_y = (_touch_data[6] << 8 | _touch_data[5]);
    }
}

void touch_init()
{
    pinMode(87, 1);
    pinMode(104, 1);
    pinMode(27, 1);
    pinMode(24, 0);
    digitalWrite(87, 1);
    digitalWrite(104, 1);
    digitalWrite(27, 1);
    am_util_delay_ms(20);
    digitalWrite(27, 0);
    am_util_delay_ms(50);
    digitalWrite(27, 1);
    am_util_delay_ms(100);
    if (am_hal_iom_initialize(ui32ModuleTouch, &touch_pIomHandle) ||
        am_hal_iom_power_ctrl(touch_pIomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(touch_pIomHandle, &touch_i2cSettings) ||
        am_hal_iom_enable(touch_pIomHandle))
    {
        am_util_stdio_printf("Touch AM_DEVICES_I2C_STATUS_ERROR\r\n");
    }
    else
    {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM2_SCL, g_AM_BSP_GPIO_IOM2_SCL);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM2_SDA, g_AM_BSP_GPIO_IOM2_SDA);
    }
}