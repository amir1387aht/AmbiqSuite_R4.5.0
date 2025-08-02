#include "./display.h"

#define XFER_MAX_WAIT_MS 100

static bool bXferDone = false;
static uint32_t g_dataSize = 0;
static bool isChangingBrightness = false;

typedef void (*am_devices_disp_handler_t)(void*);

typedef struct {
	// if frame transfer is pending for TE
	volatile bool bXferPending;

	// if frame trander in progress
	volatile bool bXferBusy;

	// frame transfer information
	uint16_t ui16XferResX;
	uint16_t ui16XferResY;
	uint32_t ui32XferAddress;

	// application callback when frame transfer completes
	am_devices_disp_handler_t fnXferDoneCb;
	void* pArgXferDone;

	// total stripe
	uint32_t total_stripe;

} am_devices_display_tranfer_t;

static const IRQn_Type mspi_display_interrupts[] = {
	MSPI0_IRQn,
	MSPI1_IRQn,
	MSPI2_IRQn,
};

static const IRQn_Type te_interrupts[] = {
	GPIO0_001F_IRQn, GPIO0_203F_IRQn,
	GPIO0_405F_IRQn, GPIO0_607F_IRQn
};

static void* g_MSPIDisplayHandle;
static void* g_DisplayHandle;

static uint32_t ui32MspiDisplayQBuffer[(AM_HAL_MSPI_CQ_ENTRY_SIZE / 4) * 12];

static am_devices_mspi_rm69330_config_t QuadDisplayMSPICfg = {
	.eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0_1_4_4,
	.eClockFreq = AM_HAL_MSPI_CLK_48MHZ,
	.pNBTxnBuf = ui32MspiDisplayQBuffer,
	.ui32NBTxnBufLength = sizeof(ui32MspiDisplayQBuffer) / sizeof(uint32_t),
};

am_devices_display_hw_config_t g_sDispCfg = {
	.eIC = DISP_IC_RM69330,
	.eInterface = DISP_IF_QSPI,
	.ui16TEpin = DISPLAY_TE_PIN,
	.ui16ResX = display_width,
	.ui16ResY = display_height,
	.bFlip = false,
	.ui32Module = DISPLAY_MSPI_INST,
	.eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0_1_4_4,
	.eClockFreq = AM_HAL_MSPI_CLK_48MHZ,
	.bClockonD4 = false,
	.ui8DispMspiSelect = 1,
	.eTEType = DISP_TE_GPIO,
	.ui16Offset = 0
};

am_hal_gpio_pincfg_t gpio_default = { .GP.cfg = 3 };

am_hal_gpio_pincfg_t g_AM_GPIO_TE_43 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_43_GPIO,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_ENABLE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P1X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_100K,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_D0 (37) - MSPI1 data 0.
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_D0 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_37_MSPI1_0,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_D1 (38) - MSPI1 data 1.
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_D1 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_38_MSPI1_1,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_D2 (39) - MSPI1 data 2.
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_D2 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_39_MSPI1_2,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_D3 (40) - MSPI1 data 3.
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_D3 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_40_MSPI1_3,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_CE0 (86) - MSPI1 chip select for device 0
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_CE0 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_86_NCE86,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = AM_HAL_GPIO_NCE_MSPI1CEN0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};
//*****************************************************************************
//
// MSPI1_CE1 (52) - MSPI1 chip select for device 1
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_CE1 = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_52_NCE52,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = AM_HAL_GPIO_NCE_MSPI1CEN1,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

//*****************************************************************************
//
// MSPI1_SCK (45) - MSPI1 clock.
//
//*****************************************************************************
am_hal_gpio_pincfg_t g_AM_GPIO_MSPI1_SCK = {
	.GP.cfg_b.uFuncSel = AM_HAL_PIN_45_MSPI1_8,
	.GP.cfg_b.eGPInput = AM_HAL_GPIO_PIN_INPUT_NONE,
	.GP.cfg_b.eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
	.GP.cfg_b.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
	.GP.cfg_b.eGPOutCfg = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
	.GP.cfg_b.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P5X,
	.GP.cfg_b.uSlewRate = 0,
	.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_NONE,
	.GP.cfg_b.uNCE = 0,
	.GP.cfg_b.eCEpol = AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW,
	.GP.cfg_b.uRsvd_0 = 0,
	.GP.cfg_b.ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
	.GP.cfg_b.eForceInputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.eForceOutputEn = AM_HAL_GPIO_PIN_FORCEEN_NONE,
	.GP.cfg_b.uRsvd_1 = 0,
};

am_hal_gpio_pincfg_t g_AM_GPIO_RESET_11 = { .GP.cfg = 0x183 };
am_hal_gpio_pincfg_t g_AM_GPIO_RESET_41 = { .GP.cfg = 0x183 };

am_hal_gpio_pincfg_t g_AM_GPIO_RESET_11_d = { .GP.cfg = 0x2093 };

am_hal_gpio_pincfg_t g_AM_GPIO_RESET_41_d = { .GP.cfg = 0x2093 };

am_hal_gpio_pincfg_t g_AM_GPIO_RESET_42 = { .GP.cfg = 0xD03 };

#define TE_GPIO_IDX GPIO_NUM2IDX(DISPLAY_TE_PIN)

typedef struct {
	// the display resolution
	uint16_t ui16ResX;
	uint16_t ui16ResY;

	// Address of vertex
	uint16_t ui16MinX;
	uint16_t ui16MinY;

	// the color mode of application frame-buffer
	am_devices_disp_color_e eColorMode;

} am_devices_display_user_setting_t;

static am_devices_display_user_setting_t sDispUserSetting = { 0 };

static am_devices_display_tranfer_t sDispTransfer = {
	.bXferPending = false,
	.bXferBusy = false,
	.ui16XferResX = 0,
	.ui16XferResY = 0,
	.ui32XferAddress = 0,
	.fnXferDoneCb = NULL,
	.pArgXferDone = NULL,
};

bool display_is_busy() {
	return sDispTransfer.bXferBusy;
}

static void display_transfer_complete(void* pCallbackCtxt, uint32_t transactionStatus) {
	sDispTransfer.bXferBusy = false;

	if (sDispTransfer.fnXferDoneCb) {
		if (sDispTransfer.total_stripe == 0) {
			sDispTransfer.fnXferDoneCb(sDispTransfer.pArgXferDone);
		}
		else {
			sDispTransfer.fnXferDoneCb(pCallbackCtxt);
		}
	}

	bXferDone = true;
}

static uint32_t am_devices_display_launch_transfer() {
	return am_devices_mspi_rm69330_nonblocking_write_endian(
		g_DisplayHandle, sDispTransfer.ui32XferAddress, g_dataSize, 0, 0,
		display_transfer_complete, 0, false, 0);
}

uint32_t am_devices_display_set_scanline_recommended_parameter(
	uint8_t TETimesPerFrame) {
	return am_devices_mspi_rm69330_set_scanline_recommended_parameter(
		g_DisplayHandle, TETimesPerFrame);
}

static void am_devices_display_te_handler(void* pvUnused, uint32_t ui32Unused) {
	//
	// Transfer the frame when TE interrupt arrives.
	//
	if (sDispTransfer.bXferPending) {
		am_devices_display_launch_transfer();
		sDispTransfer.bXferPending = false;
	}
}

void am_bsp_disp_pins_enable1(void) {
	am_hal_gpio_pinconfig(86, g_AM_GPIO_MSPI1_CE0);
	am_hal_gpio_pinconfig(37, g_AM_GPIO_MSPI1_D0);
	am_hal_gpio_pinconfig(38, g_AM_GPIO_MSPI1_D1);
	am_hal_gpio_pinconfig(39, g_AM_GPIO_MSPI1_D2);
	am_hal_gpio_pinconfig(40, g_AM_GPIO_MSPI1_D3);
	am_hal_gpio_pinconfig(45, g_AM_GPIO_MSPI1_SCK);
	am_hal_gpio_pinconfig(43, g_AM_GPIO_TE_43);
}

void am_bsp_disp_pins_disable1(void) {
	am_hal_gpio_pinconfig(86, gpio_default);
	am_hal_gpio_pinconfig(37, gpio_default);
	am_hal_gpio_pinconfig(38, gpio_default);
	am_hal_gpio_pinconfig(39, gpio_default);
	am_hal_gpio_pinconfig(40, gpio_default);
	am_hal_gpio_pinconfig(45, gpio_default);
	am_hal_gpio_pinconfig(43, gpio_default);
}

int32_t am_devices_display_init(uint16_t ui16ResX, uint16_t ui16ResY,
	am_devices_disp_color_e eColorMode,
	bool bEnableTE) {
	//
	// store the user setting
	//
	if (ui16ResX < g_sDispCfg.ui16ResX) {
		sDispUserSetting.ui16ResX = ui16ResX;
	}
	else {
		sDispUserSetting.ui16ResX = g_sDispCfg.ui16ResX;
	}

	sDispUserSetting.ui16MinX =
		((g_sDispCfg.ui16ResX - sDispUserSetting.ui16ResX) >> 2) << 1;

	if (ui16ResY < g_sDispCfg.ui16ResY) {
		sDispUserSetting.ui16ResY = ui16ResY;
	}
	else {
		sDispUserSetting.ui16ResY = g_sDispCfg.ui16ResY;
	}

	sDispUserSetting.ui16MinY =
		((g_sDispCfg.ui16ResY - sDispUserSetting.ui16ResY) >> 2) << 1;

	sDispUserSetting.eColorMode = eColorMode;
	sDispTransfer.bXferPending = false;
	sDispTransfer.bXferBusy = false;

	//
	// check if the user would like to use TE
	//
	if (!bEnableTE) {
		g_sDispCfg.eTEType = DISP_TE_DISABLE;
	}

	//
	// Initialize the display specific GPIO signals.
	//
	am_bsp_disp_pins_enable1();

	uint32_t ui32Status = 0;
	uint8_t ui8Format = AM_DEVICES_MSPI_RM69330_COLOR_MODE_16BIT;

	if (sDispUserSetting.eColorMode == COLOR_FORMAT_RGB888) {
		ui8Format = AM_DEVICES_MSPI_RM69330_COLOR_MODE_24BIT;
	}
	else if (sDispUserSetting.eColorMode == COLOR_FORMAT_RGB565) {
		ui8Format = AM_DEVICES_MSPI_RM69330_COLOR_MODE_16BIT;
	}
	//
	// modified default row, column and format parameters.
	//
	am_devices_rm69330_set_parameters(
		sDispUserSetting.ui16MinX + g_sDispCfg.ui16Offset, ui16ResX,
		sDispUserSetting.ui16MinY, ui16ResY, ui8Format);
	//
	// Initialize the MSPI Display
	//
	QuadDisplayMSPICfg.eClockFreq = g_sDispCfg.eClockFreq;
	QuadDisplayMSPICfg.eDeviceConfig = g_sDispCfg.eDeviceConfig;

	ui32Status =
		am_devices_mspi_rm69330_init(g_sDispCfg.ui32Module, &QuadDisplayMSPICfg,
			&g_DisplayHandle, &g_MSPIDisplayHandle);
	if (AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS != ui32Status) {
		return AM_DEVICES_DISPLAY_STATUS_PANEL_ERR;
	}
	NVIC_SetPriority(mspi_display_interrupts[g_sDispCfg.ui32Module], 0x4);
	NVIC_EnableIRQ(mspi_display_interrupts[g_sDispCfg.ui32Module]);

	am_devices_mspi_rm69330_display_on(g_DisplayHandle);

	bXferDone = false;
	//
	// Setting default scanline
	//
	am_devices_display_set_scanline_recommended_parameter(1);
	//
	// Enable GPIO TE interrupt
	//
	if (g_sDispCfg.eTEType == DISP_TE_GPIO) {
		uint32_t IntNum = g_sDispCfg.ui16TEpin;
		am_hal_gpio_mask_t gpio_mask = AM_HAL_GPIO_MASK_DECLARE_ZERO;
		gpio_mask.U.Msk[GPIO_NUM2IDX(IntNum)] = GPIO_NUM2MSK(IntNum);
		am_hal_gpio_interrupt_clear(AM_HAL_GPIO_INT_CHANNEL_0, &gpio_mask);
		am_hal_gpio_interrupt_register(
			AM_HAL_GPIO_INT_CHANNEL_0, IntNum,
			(am_hal_gpio_handler_t)am_devices_display_te_handler, NULL);
		am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
			AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
			(void*)&IntNum);
		NVIC_SetPriority(te_interrupts[TE_GPIO_IDX], 0x4);
		NVIC_EnableIRQ(te_interrupts[TE_GPIO_IDX]);
	}

	return AM_DEVICES_DISPLAY_STATUS_SUCCESS;
}

int send_mode7() {
	uint8_t pData[20];

	pData[0] = 0x70;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 0xC9;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0, pData, 1u);
	pData[0] = 0x80;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 8u, pData, 1u);
	pData[0] = 0xC9;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 9u, pData, 1u);
	pData[0] = 0x80;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x11u, pData, 1u);
	pData[0] = 0x82;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 3;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 9u, pData, 1u);
	pData[0] = 0x1A;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x34u, pData, 1u);
	pData[0] = 0x1B;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x40u, pData, 1u);
	pData[0] = 1;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x17u, pData, 1u);
	pData[0] = 2;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x43u, pData, 1u);
	pData[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u))
		return 1;
	pData[0] = 23;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x80u, pData, 1u))
		return 1;
	pData[0] = 32;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x53u, pData, 1u))
		return 1;
	uint32_t v4 = 0xD7001800;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Au,
		(uint8_t*)&v4, 4u))
		return 1;
	uint32_t v3 = 0xE9010000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Bu,
		(uint8_t*)&v3, 4u))
		return 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x12u, 0, 0))
		return 1;
	uint32_t v2 = 0xD6001900;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x31u,
		(uint8_t*)&v2, 4u))
		return 1;
	uint32_t v1 = 0xE8010100;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x30u,
		(uint8_t*)&v1, 4u))
		return 1;
	pData[0] = 117;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x3Au, pData, 1u))
		return 1;
	pData[0] = 2;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x35u, pData, 1u))
		return 1;
	pData[0] = -1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x51u, pData, 1u))
		return 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x11u, 0, 0))
		return 1;
	am_util_delay_ms(60);
	return am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x29u, 0, 0) !=
		0;
}

int send_mode4() {
	uint8_t pData[12];

	pData[0] = 0x20;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 0x5A;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xF4u, pData, 1u);
	pData[0] = 0x59;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xF5u, pData, 1u);
	pData[0] = 0xE0;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 0;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xBu, pData, 1u);
	pData[0] = 0x9B;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Du, pData, 1u);
	pData[0] = 0x1D;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x23u, pData, 1u);
	pData[0] = 0x81;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x24u, pData, 1u);
	pData[0] = 3;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x30u, pData, 1u);
	pData[0] = 0x40;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u))
		return 1;
	pData[0] = 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x76u, pData, 1u))
		return 1;
	pData[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u))
		return 1;
	pData[0] = 0x80;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xC4u, pData, 1u))
		return 1;
	pData[0] = 0x55;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x3Au, pData, 1u))
		return 1;
	pData[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x35u, pData, 1u))
		return 1;
	pData[0] = 0x20;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x53u, pData, 1u))
		return 1;
	pData[0] = 0xFF;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x51u, pData, 1u))
		return 1;
	pData[0] = 0xFF;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x63u, pData, 1u))
		return 1;
	uint32_t v2 = 0xBF000000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Au,
		(uint8_t*)&v2, 4u))
		return 1;
	uint32_t v1 = 0xE9010000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Bu,
		(uint8_t*)&v1, 4u))
		return 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x11u, 0, 0))
		return 1;
	am_util_delay_ms(60);
	return am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x29u, 0, 0) !=
		0;
}

int send_mode5() {
	uint8_t v4[12];

	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x11, 0, 0))
		return 1;
	am_util_delay_ms(5);
	uint32_t v3 = 0xBF000000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2A,
		(uint8_t*)&v3, 4))
		return 1;
	uint32_t v2 = 0xE9010000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2B,
		(uint8_t*)&v2, 4))
		return 1;
	uint16_t v1 = 0xE901;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x44,
		(uint8_t*)&v1, 2))
		return 1;
	v4[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x35, v4, 1))
		return 1;
	v4[0] = 0x53;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x4A, v4, 1))
		return 1;
	v4[0] = 0x17;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x80, v4, 1))
		return 1;
	v4[0] = 0x55;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x3A, v4, 1))
		return 1;
	v4[0] = 0x20;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x53, v4, 1))
		return 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x29, 0, 0))
		return 1;
	am_util_delay_ms(20);
	return 0;
}

int send_mode6() {
	uint8_t pData[20];  // [sp+Ch] [bp-14h] BYREF

	pData[0] = 0x20;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 0x5A;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xF4u, pData, 1u);
	pData[0] = 0x59;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xF5u, pData, 1u);
	pData[0] = 0x40;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 1;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x98u, pData, 1u);
	pData[0] = 0xE0;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u);
	pData[0] = 0x9E;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x23u, pData, 1u);
	pData[0] = 0x81;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x24u, pData, 1u);
	pData[0] = 3;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x30u, pData, 1u);
	pData[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xFEu, pData, 1u))
		return 1;
	pData[0] = 0x80;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0xC4u, pData, 1u))
		return 1;
	pData[0] = 0x55;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x3Au, pData, 1u))
		return 1;
	pData[0] = 0;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x35u, pData, 1u))
		return 1;
	pData[0] = 0x20;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x53u, pData, 1u))
		return 1;
	pData[0] = 0xFF;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x51u, pData, 1u))
		return 1;
	pData[0] = 0xFF;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x63u, pData, 1u))
		return 1;
	uint32_t v2 = 0xD7001800;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Au,
		(uint8_t*)&v2, 4u))
		return 1;
	uint32_t v1 = 0xE9010000;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x2Bu,
		(uint8_t*)&v1, 4u))
		return 1;
	if (am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x11u, 0, 0))
		return 1;
	am_util_delay_ms(60);
	return am_devices_mspi_rm69330_command_write(g_DisplayHandle, 0x29u, 0, 0) !=
		0;
}

int get_display_mode(uint32_t displayId) {
	displayId = displayId & 0xffffff;
	if (displayId == 0x200216) return 1;
	if (displayId == 0x200116) return 7;
	if (((displayId & 0xffff) == 0x0216) && ((displayId >> 16) - 0x30) <= 1)
		return 2;
	if (displayId == 0x300416) return 6;
	if (displayId == 0x400216) return 5;
	if (displayId == 0x050A1D) return 3;
	if (displayId == 0x050A5A) return 4;
	return 0;
}

int dword_100077AC = 0;

void display_pins_irq_something_enable() {
	uint32_t v0 = 0;
	am_hal_gpio_pinconfig(11, g_AM_GPIO_RESET_11);
	am_hal_gpio_pinconfig(41, g_AM_GPIO_RESET_41);
	v0 = am_hal_interrupt_master_disable();
	(*(volatile unsigned int*)0x40010224) = 2048;
	if (((*(volatile unsigned int*)0x40020000) & 0xFF00) == 0) {
		dword_100077AC = 0;
		while (((*(volatile unsigned int*)0x40010214) & 0x800) == 0) {
			(*(volatile unsigned int*)0x40010224) = 2048;
			dword_100077AC = 1;
		}
	}
	am_hal_interrupt_master_set(v0);
	am_util_delay_ms(5);
	v0 = am_hal_interrupt_master_disable();
	(*(volatile unsigned int*)0x40010228) = 512;
	if (((*(volatile unsigned int*)0x40020000) & 0xFF00) == 0) {
		dword_100077AC = 0;
		while (((*(volatile unsigned int*)0x40010218) & 0x200) == 0) {
			(*(volatile unsigned int*)0x40010228) = 512;
			dword_100077AC = 1;
		}
	}
	am_hal_interrupt_master_set(v0);
	am_util_delay_ms(15);
}

void display_pins_irq_something_disable() {
	uint32_t v0 = 0;
	am_hal_gpio_pinconfig(11, g_AM_GPIO_RESET_11_d);
	am_hal_gpio_pinconfig(41, g_AM_GPIO_RESET_41_d);
	v0 = am_hal_interrupt_master_disable();
	(*(volatile unsigned int*)0x40010238) = 512;
	if (((*(volatile unsigned int*)0x40020000) & 0xFF00) == 0) {
		dword_100077AC = 0;
		while (((*(volatile unsigned int*)0x40010218) & 0x200) != 0) {
			(*(volatile unsigned int*)0x40010238) = 512;
			dword_100077AC = 1;
		}
	}
	am_hal_interrupt_master_set(v0);
	am_util_delay_ms(10);
	v0 = am_hal_interrupt_master_disable();
	(*(volatile unsigned int*)0x40010234) = 2048;
	if (((*(volatile unsigned int*)0x40020000) & 0xFF00) == 0) {
		dword_100077AC = 0;
		while (((*(volatile unsigned int*)0x40010214) & 0x800) != 0) {
			(*(volatile unsigned int*)0x40010234) = 2048;
			dword_100077AC = 1;
		}
	}
	am_hal_interrupt_master_set(v0);
	am_util_delay_ms(1);
}

void display_pins_enable(bool state) {
	if (state) {
		am_hal_gpio_pinconfig(42, g_AM_GPIO_RESET_42);
		display_pins_irq_something_enable();
		digitalWrite(42, 1);
		am_util_delay_ms(25);
		am_bsp_disp_pins_enable1();
		am_util_delay_ms(1);
	}
	else {
		am_util_delay_ms(100);
		digitalWrite(42, 0);
		am_util_delay_ms(1);
		am_bsp_disp_pins_disable1();
		am_util_delay_ms(1);
		display_pins_irq_something_disable();
	}
}

static uint32_t am_devices_display_transfer_frame_inter(void) {
	sDispTransfer.bXferPending = true;
	return AM_DEVICES_DISPLAY_STATUS_SUCCESS;
}

uint32_t am_devices_display_transfer_frame(
	uint16_t ui16ResX, uint16_t ui16ResY, uint32_t ui32Address,
	am_devices_disp_handler_t fnXferDoneCb, void* pArgXferDone) {
	if (sDispTransfer.bXferBusy) {
		return AM_DEVICES_DISPLAY_STATUS_TRY_AGAIN;
	}

	//
	// Record the transfer setting
	//
	sDispTransfer.bXferBusy = true;
	sDispTransfer.bXferPending = false;
	sDispTransfer.ui16XferResX = ui16ResX;
	sDispTransfer.ui16XferResY = ui16ResY;
	sDispTransfer.ui32XferAddress = ui32Address;
	sDispTransfer.fnXferDoneCb = fnXferDoneCb;
	sDispTransfer.pArgXferDone = pArgXferDone;
	sDispTransfer.total_stripe = 0;

	return am_devices_display_transfer_frame_inter();
}

void display_set_brightness(uint8_t brightness) {
	while (display_is_busy());
	isChangingBrightness = true;
	am_devices_mspi_rm69330_command_write(g_DisplayHandle, AM_DEVICES_MSPI_RM69330_WRITE_DISPLAY_BRIGHTNESS, (uint8_t[]) { brightness }, 1);
	isChangingBrightness = false;
}

void display_update_area(const lv_area_t* area, uint8_t* px_map, void* pArgXferDone) {
	while(isChangingBrightness);

	uint16_t columnStart = area->x1 + 24;
	uint16_t columnSize = area->x2 - area->x1 + 1;
	uint16_t rowStart = area->y1;
	uint16_t rowSize = area->y2 - area->y1 + 1;
	g_dataSize = columnSize * rowSize * (LV_COLOR_DEPTH / 8);

	am_devices_mspi_rm69330_set_transfer_window(g_DisplayHandle, columnStart, columnSize, rowStart, rowSize);
	am_devices_display_transfer_frame(0, 0, px_map, pArgXferDone, NULL);
}

void display_init() {
	display_pins_enable(1);
	am_devices_display_init(display_width, display_height, LV_COLOR_DEPTH == 16 ? COLOR_FORMAT_RGB565 : COLOR_FORMAT_RGB888, true);

	uint32_t data;
	int result = am_devices_mspi_rm69330_read_id(g_DisplayHandle, (uint32_t*)&data);

	if (result) am_util_stdio_printf("Init display failed!!!\r\n");

	switch (get_display_mode(data)) {
	case 1:
	case 7:
		send_mode7();
		break;
	case 2:
	case 4:
		send_mode4();
		break;
	case 5:
		send_mode5();
		break;
	case 6:
		send_mode6();
		break;
	default:
		break;
	}

	display_set_brightness(175);
}

void display_end() {
	display_pins_enable(0);
	am_util_delay_ms(20);
	am_devices_mspi_rm69330_term(g_DisplayHandle);
}

void am_mspi1_isr(void) {
	uint32_t ui32Status;
	am_hal_mspi_interrupt_status_get(g_MSPIDisplayHandle, &ui32Status, false);
	am_hal_mspi_interrupt_clear(g_MSPIDisplayHandle, ui32Status);
	am_hal_mspi_interrupt_service(g_MSPIDisplayHandle, ui32Status);
}

void am_gpio0_001f_isr(void) {
	uint32_t ui32IntStatus;
	am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
	am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
	am_hal_gpio_interrupt_service(GPIO0_001F_IRQn, ui32IntStatus);
}

void am_gpio0_203f_isr(void) {
	uint32_t ui32IntStatus;
	am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, false, &ui32IntStatus);
	am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, ui32IntStatus);
	am_hal_gpio_interrupt_service(GPIO0_203F_IRQn, ui32IntStatus);
}