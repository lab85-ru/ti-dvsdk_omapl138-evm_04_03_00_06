/*
 * Hawkboard.org based on TI's OMAP-L138 Platform
 *
 * Initial code: Syed Mohammed Khasim
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of
 * any kind, whether express or implied.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/cp_intc.h>
#include <mach/da8xx.h>
#include <mach/mux.h>

#include <mach/mux.h>
#include <mach/usb.h>
#include <mach/vpif.h>
//#include <media/davinci/videohd.h>

#include <media/tvp514x.h>

#define HAWKBOARD_PHY_ID		"0:07"

#define DA850_MMCSD_CD_PIN		GPIO_TO_PIN(4, 0)
#define DA850_LCD_PWR_PIN		GPIO_TO_PIN(2, 8)
#define DA850_LCD_BL_PIN		GPIO_TO_PIN(2, 15)

#define DA850_USB1_VBUS_PIN		GPIO_TO_PIN(2, 4)
#define DA850_USB1_OC_PIN		GPIO_TO_PIN(6, 13)

#define HAWKBOARD_SATA_REFCLKPN_RATE	(100 * 1000 * 1000)


#if defined(CONFIG_MMC_DAVINCI) || \
    defined(CONFIG_MMC_DAVINCI_MODULE)
#define HAS_MMC 1
#else
#define HAS_MMC 0
#endif

#define TVP5147_CH0		"tvp514x-0"
#define TVP5147_CH1		"tvp514x-1"

#define VPIF_STATUS	(0x002C)
#define VPIF_STATUS_CLR	(0x0030)

/* Retaining these APIs, since the VPIF drivers do not check NULL handlers */
static int da850_set_vpif_clock(int mux_mode, int hd)
{
	return 0;
}

static int da850_setup_vpif_input_channel_mode(int mux_mode)
{
	return 0;
}

int da850_vpif_setup_input_path(int ch, const char *name)
{
	int ret = 0;
	return ret;
}

static int da850_vpif_intr_status(void __iomem *vpif_base, int channel)
{
	int status = 0;
	int mask;

	if (channel < 0 || channel > 3)
		return 0;

	mask = 1 << channel;
	status = __raw_readl((vpif_base + VPIF_STATUS)) & mask;
	__raw_writel(status, (vpif_base + VPIF_STATUS_CLR));

	return status;
}

/* VPIF capture configuration */
static struct tvp514x_platform_data tvp5146_pdata = {
	.clk_polarity = 0,
	.hs_polarity = 1,
	.vs_polarity = 1
};

#define TVP514X_STD_ALL (V4L2_STD_NTSC | V4L2_STD_PAL)

static struct vpif_subdev_info da850_vpif_capture_sdev_info[] = {
	{
		.name	= TVP5147_CH0,
		.board_info = {
			I2C_BOARD_INFO("tvp5146", 0x5d),
			.platform_data = &tvp5146_pdata,
		},
		.input = INPUT_CVBS_VI2B,  // my VI2B->composite input
		.output = OUTPUT_10BIT_422_EMBEDDED_SYNC,
		.can_route = 1,
		.vpif_if = {
			.if_type = VPIF_IF_BT656,
			.hd_pol = 1,
			.vd_pol = 1,
			.fid_pol = 0,
		},
	},
/*	{
		.name	= TVP5147_CH1,
		.board_info = {
			I2C_BOARD_INFO("tvp5146", 0x5c),
			.platform_data = &tvp5146_pdata,
		},
		.input = INPUT_SVIDEO_VI2C_VI1C,
		.output = OUTPUT_10BIT_422_EMBEDDED_SYNC,
		.can_route = 1,
		.vpif_if = {
			.if_type = VPIF_IF_BT656,
			.hd_pol = 1,
			.vd_pol = 1,
			.fid_pol = 0,
		},
	},*/
};

static const struct vpif_input da850_ch0_inputs[] = {
	{
		.input = {
			.index = 0,
			.name = "Composite",
			.type = V4L2_INPUT_TYPE_CAMERA,
			.std = TVP514X_STD_ALL,
		},
		.subdev_name = TVP5147_CH0,
	},
};
/*
static const struct vpif_input da850_ch1_inputs[] = {
       {
		.input = {
			.index = 0,
			.name = "S-Video",
			.type = V4L2_INPUT_TYPE_CAMERA,
			.std = TVP514X_STD_ALL,
		},
		.subdev_name = TVP5147_CH1,
	},
};
*/
static struct vpif_capture_config da850_vpif_capture_config = {
	.setup_input_channel_mode = da850_setup_vpif_input_channel_mode,
	.setup_input_path = da850_vpif_setup_input_path,
	.intr_status = da850_vpif_intr_status,
	.subdev_info = da850_vpif_capture_sdev_info,
	.subdev_count = ARRAY_SIZE(da850_vpif_capture_sdev_info),
	.chan_config[0] = {
		.inputs = da850_ch0_inputs,
		.input_count = ARRAY_SIZE(da850_ch0_inputs),
	},
/*
	.chan_config[1] = {
		.inputs = da850_ch1_inputs,
		.input_count = ARRAY_SIZE(da850_ch1_inputs),
	},
*/
	.card_name      = "DA850/OMAP-L138 Video Capture",
};


static irqreturn_t omapl138_hawk_usb_ocic_irq(int irq, void *dev_id);
static da8xx_ocic_handler_t hawk_usb_ocic_handler;

static const short da850_hawk_usb11_pins[] = {
	DA850_GPIO2_4, DA850_GPIO6_13,
	-1
};

static int hawk_usb_set_power(unsigned port, int on)
{
	gpio_set_value(DA850_USB1_VBUS_PIN, on);
	return 0;
}

static int hawk_usb_get_power(unsigned port)
{
	return gpio_get_value(DA850_USB1_VBUS_PIN);
}

static int hawk_usb_get_oci(unsigned port)
{
	return !gpio_get_value(DA850_USB1_OC_PIN);
}

static int hawk_usb_ocic_notify(da8xx_ocic_handler_t handler)
{
	int irq         = gpio_to_irq(DA850_USB1_OC_PIN);
	int error       = 0;

	if (handler != NULL) {
		hawk_usb_ocic_handler = handler;

		error = request_irq(irq, omapl138_hawk_usb_ocic_irq,
					IRQF_DISABLED | IRQF_TRIGGER_RISING |
					IRQF_TRIGGER_FALLING,
					"OHCI over-current indicator", NULL);
		if (error)
			pr_err("%s: could not request IRQ to watch "
				"over-current indicator changes\n", __func__);
	} else {
		free_irq(irq, NULL);
	}
	return error;
}

static struct da8xx_ohci_root_hub omapl138_hawk_usb11_pdata = {
	.set_power      = hawk_usb_set_power,
	.get_power      = hawk_usb_get_power,
	.get_oci        = hawk_usb_get_oci,
	.ocic_notify    = hawk_usb_ocic_notify,
	/* TPS2087 switch @ 5V */
	.potpgt         = (3 + 1) / 2,  /* 3 ms max */
};

static irqreturn_t omapl138_hawk_usb_ocic_irq(int irq, void *dev_id)
{
	hawk_usb_ocic_handler(&omapl138_hawk_usb11_pdata, 1);
	return IRQ_HANDLED;
}

static __init void omapl138_hawk_usb_init(void)
{
	int ret;
	u32 cfgchip2;

	ret = davinci_cfg_reg_list(da850_hawk_usb11_pins);
	if (ret) {
		pr_warning("%s: USB 1.1 PinMux setup failed: %d\n",
			__func__, ret);
		return;
	}

	/* Setup the Ref. clock frequency for the HAWK at 24 MHz. */

	cfgchip2 = __raw_readl(DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));
	cfgchip2 &= ~CFGCHIP2_REFFREQ;
	cfgchip2 |=  CFGCHIP2_REFFREQ_24MHZ;
	__raw_writel(cfgchip2, DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));

	ret = gpio_request_one(DA850_USB1_VBUS_PIN,
			GPIOF_DIR_OUT, "USB1 VBUS");
	if (ret < 0) {
		pr_err("%s: failed to request GPIO for USB 1.1 port "
			"power control: %d\n", __func__, ret);
		return;
	}

	ret = gpio_request_one(DA850_USB1_OC_PIN,
			GPIOF_DIR_IN, "USB1 OC");
	if (ret < 0) {
		pr_err("%s: failed to request GPIO for USB 1.1 port "
			"over-current indicator: %d\n", __func__, ret);
		goto usb11_setup_oc_fail;
	}

	ret = da8xx_register_usb11(&omapl138_hawk_usb11_pdata);
	if (ret) {
		pr_warning("%s: USB 1.1 registration failed: %d\n",
			__func__, ret);
		goto usb11_setup_fail;
	}

	return;

usb11_setup_fail:
	gpio_free(DA850_USB1_OC_PIN);
usb11_setup_oc_fail:
	gpio_free(DA850_USB1_VBUS_PIN);
}

/* I2C */
static struct i2c_board_info __initdata omapl138_hawk_i2c_devices[] = {
	{
		I2C_BOARD_INFO("tlv320aic3x", 0x18),
	},
};

static struct davinci_i2c_platform_data omapl138_hawk_i2c_0_pdata = {
        .sda_pin        = GPIO_TO_PIN(1, 4),
        .scl_pin        = GPIO_TO_PIN(1, 5),
	.bus_freq	= 100,	/* kHz */
	.bus_delay	= 0,	/* usec */
};

static struct i2c_gpio_platform_data da850_gpio_i2c_pdata = {
        .sda_pin        = GPIO_TO_PIN(1, 4),
        .scl_pin        = GPIO_TO_PIN(1, 5),
        .udelay         = 2,                    /* 250 KHz */
};

static struct platform_device da850_gpio_i2c = {
        .name           = "i2c-gpio",
        .id             = 1,
        .dev            = {
                .platform_data  = &da850_gpio_i2c_pdata,
        },
};

static void omapl138_hawk_i2c_init(void)
{
	int ret;

//	ret = davinci_cfg_reg_list(da850_i2c0_pins);
//	if (ret)
//		pr_warning("omapl138_hawk_init: i2c0 mux setup failed: %d\n", ret);

//	ret = davinci_cfg_reg_list(da850_i2c1_pins);
//	if (ret)
//		pr_warning("omapl138_hawk_init: i2c1 mux setup failed: %d\n", ret);

//	platform_device_register(&da850_gpio_i2c);

	ret = da8xx_register_i2c(0, &omapl138_hawk_i2c_0_pdata);
        if (ret)
                pr_warning("i2c0 registration failed: %d\n", ret);

//	ret = da8xx_register_i2c(1, &omapl138_hawk_i2c_0_pdata);
//        if (ret)
//                pr_warning("i2c1 registration failed: %d\n", ret);


//	i2c_register_board_info(1, omapl138_hawk_i2c_devices,
//			ARRAY_SIZE(omapl138_hawk_i2c_devices));
}

/* VGA */
static const short omapl138_hawk_lcdc_pins[] = {
	DA850_GPIO2_8, DA850_GPIO2_15,
 	-1
};

/* Backlight and power is for use with LCD expansion header only */
static void da850_panel_power_ctrl(int val)
{
	/* lcd backlight */
	gpio_set_value(DA850_LCD_BL_PIN, val);
	/* lcd power */
	gpio_set_value(DA850_LCD_PWR_PIN, val);
}

static int da850_lcd_hw_init(void)
{
	int status;

	status = gpio_request(DA850_LCD_BL_PIN, "lcd bl\n");
	if (status < 0)
		return status;

	status = gpio_request(DA850_LCD_PWR_PIN, "lcd pwr\n");
	if (status < 0) {
		gpio_free(DA850_LCD_BL_PIN);
		return status;
	}

	gpio_direction_output(DA850_LCD_BL_PIN, 0);
	gpio_direction_output(DA850_LCD_PWR_PIN, 0);

	/* Switch off panel power and backlight */
	da850_panel_power_ctrl(0);

	/* Switch on panel power and backlight */
	da850_panel_power_ctrl(1);

	return 0;
}

static void omapl138_hawk_display_init(void)
{
	int ret;

	ret = davinci_cfg_reg_list(da850_lcdcntl_pins);
	if (ret)
		pr_warning("omapl138_hawk_init: lcdcntl mux setup failed: %d\n",
				ret);
	
	ret = davinci_cfg_reg_list(omapl138_hawk_lcdc_pins);
	if (ret)
		pr_warning("omapl138_hawk_init: evm specific lcd mux setup "
				"failed: %d\n",	ret);

	da850_lcd_hw_init();

	ret = da8xx_register_lcdc(&vga_monitor_pdata);
	if (ret)
		pr_warning("omapl138_hawk_init: lcdc registration failed: %d\n",
				ret);
}

static void omapl138_hawk_vpif_init(void)
{
	int ret;
	
    ret = da850_register_vpif();
	if (ret)
	    pr_warning("da850_evm_init: VPIF registration failed: "
		    		"%d\n",	ret);	

    ret = davinci_cfg_reg_list(da850_vpif_capture_pins);
	if (ret)
		pr_warning("da850_evm_init: vpif capture mux failed: "
					"%d\n",	ret);

	ret = da850_register_vpif_capture(&da850_vpif_capture_config);
	if (ret)
		pr_warning("da850_evm_init: VPIF registration failed: "
					"%d\n",	ret);

}


/*
 * The following EDMA channels/slots are not being used by drivers (for
 * example: Timer, GPIO, UART events etc) on da850/omap-l138 EVM, hence
 * they are being reserved for codecs on the DSP side.
 */
static const s16 da850_dma0_rsv_chans[][2] = {
	/* (offset, number) */
	{ 8,  6},
	{24,  4},
	{30,  2},
	{-1, -1}
};

static const s16 da850_dma0_rsv_slots[][2] = {
	/* (offset, number) */
	{ 8,  6},
	{24,  4},
	{30, 50},
	{-1, -1}
};

static const s16 da850_dma1_rsv_chans[][2] = {
	/* (offset, number) */
	{ 0, 28},
	{30,  2},
	{-1, -1}
};

static const s16 da850_dma1_rsv_slots[][2] = {
	/* (offset, number) */
	{ 0, 28},
	{30, 90},
	{-1, -1}
};

static struct edma_rsv_info da850_edma_cc0_rsv = {
	.rsv_chans	= da850_dma0_rsv_chans,
	.rsv_slots	= da850_dma0_rsv_slots,
};

static struct edma_rsv_info da850_edma_cc1_rsv = {
	.rsv_chans	= da850_dma1_rsv_chans,
	.rsv_slots	= da850_dma1_rsv_slots,
};

static struct edma_rsv_info *da850_edma_rsv[2] = {
	&da850_edma_cc0_rsv,
	&da850_edma_cc1_rsv,
};


static struct davinci_uart_config omapl138_hawk_uart_config __initdata = {
	.enabled_uarts = 0x7,
};

static int da850_hawk_mmc_get_ro(int index)
{
	return 0;
}

static int da850_hawk_mmc_get_cd(int index)
{
	return !gpio_get_value(DA850_MMCSD_CD_PIN);
}

static struct davinci_mmc_config da850_mmc_config = {
	.get_ro		= da850_hawk_mmc_get_ro,
	.get_cd		= da850_hawk_mmc_get_cd,
	.wires		= 4,
	.max_freq	= 50000000,
	.caps		= MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED,
	.version	= MMC_CTLR_VERSION_2,
};

static short omapl138_hawk_mii_pins[] __initdata = {
	DA850_MII_TXEN, DA850_MII_TXCLK, DA850_MII_COL, DA850_MII_TXD_3,
	DA850_MII_TXD_2, DA850_MII_TXD_1, DA850_MII_TXD_0, DA850_MII_RXER,
	DA850_MII_CRS, DA850_MII_RXCLK, DA850_MII_RXDV, DA850_MII_RXD_3,
	DA850_MII_RXD_2, DA850_MII_RXD_1, DA850_MII_RXD_0, DA850_MDIO_CLK,
	DA850_MDIO_D,
	-1
};

static __init void omapl138_hawk_config_emac(void)
{
	void __iomem *cfgchip3 = DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP3_REG);
	int ret;
	u32 val;
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	val = __raw_readl(cfgchip3);
	val &= ~BIT(8);
	ret = davinci_cfg_reg_list(omapl138_hawk_mii_pins);
	if (ret) {
		pr_warning("%s: cpgmac/mii mux setup failed: %d\n",
			__func__, ret);
		return;
	}

	/* configure the CFGCHIP3 register for MII */
	__raw_writel(val, cfgchip3);
	pr_info("EMAC: MII PHY configured\n");

	soc_info->emac_pdata->phy_id = HAWKBOARD_PHY_ID;

	ret = da8xx_register_emac();
	if (ret)
		pr_warning("%s: emac registration failed: %d\n",
			__func__, ret);
}

/* Set up OMAP-L138 Hawkboard/ LCDK low-level McASP driver */
/*
static u8 da850_iis_serializer_direction[] = {
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	TX_MODE,	RX_MODE,	INACTIVE_MODE,
};
*/
static u8 da850_iis_serializer_direction[] = {
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	TX_MODE,
	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};


static struct snd_platform_data omapl138_hawk_snd_data = {
	.tx_dma_offset	= 0x2000,
	.rx_dma_offset	= 0x2000,
	.op_mode	= DAVINCI_MCASP_IIS_MODE,
	.num_serializer	= ARRAY_SIZE(da850_iis_serializer_direction),
	.tdm_slots	= 2,
	.serial_dir	= da850_iis_serializer_direction,
	.asp_chan_q	= EVENTQ_0,
	.version	= MCASP_VERSION_2,
	.txnumevt	= 1,
	.rxnumevt	= 1,
};

static const short omapl138_hawk_mcasp_pins[] __initconst = {
	DA850_AHCLKX, DA850_ACLKX, DA850_AFSX,
	DA850_AHCLKR, DA850_ACLKR, DA850_AFSR, DA850_AMUTE,
	DA850_AXR_13, DA850_AXR_14,
	-1
};

static void omapl138_hawk_sound_init(void)
{
	int ret;
	ret = davinci_cfg_reg_list(omapl138_hawk_mcasp_pins);
	if (ret)
		pr_warning("omapl138_hawk_init: mcasp mux setup failed: %d\n",
				ret);

	da8xx_register_mcasp(0, &omapl138_hawk_snd_data);
}


static __init void omapl138_hawk_init(void)
{
	int ret;
	
	ret = da850_register_edma(da850_edma_rsv);
	if (ret)
		pr_warning("omapl138_lcdk_init: edma registration failed: %d\n",
				ret);

	omapl138_hawk_config_emac();

    if (HAS_MMC) {
        ret = davinci_cfg_reg_list(da850_mmcsd0_pins);
		if (ret)
			pr_warning("omapl138_lcdk_init: mmcsd0 mux setup failed:"
					" %d\n", ret);

		ret = gpio_request(DA850_MMCSD_CD_PIN, "MMC CD\n");
		if (ret)
			pr_warning("omapl138_lcdk_init: can not open GPIO %d\n",
					DA850_MMCSD_CD_PIN);
		gpio_direction_input(DA850_MMCSD_CD_PIN);

		ret = da8xx_register_mmcsd0(&da850_mmc_config);
		if (ret)
			pr_warning("omapl138_lcdk_init: mmcsd0 registration failed:"
					" %d\n", ret);
	}
	
	davinci_serial_init(&omapl138_hawk_uart_config);

	ret = da8xx_register_watchdog();
	if (ret)
		pr_warning("omapl138_lcdk_init: watchdog registration failed: %d\n", ret);

	ret = da8xx_register_rtc();
	if (ret)
		pr_warning("omapl138_lcdk_init: rts setup failed: %d\n", ret);

	omapl138_hawk_usb_init();
	omapl138_hawk_i2c_init();
	omapl138_hawk_display_init();
	omapl138_hawk_sound_init();
	omapl138_hawk_vpif_init();


	ret = da850_register_sata(HAWKBOARD_SATA_REFCLKPN_RATE);
	if (ret)
		pr_warning("omapl138_lcdk_init: sata registration failed: %d\n", ret);

	i2c_register_board_info(1, omapl138_hawk_i2c_devices,
			ARRAY_SIZE(omapl138_hawk_i2c_devices));

}

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init omapl138_hawk_console_init(void)
{
	if (!machine_is_omapl138_hawkboard())
		return 0;

	return add_preferred_console("ttyS", 2, "115200");
}
console_initcall(omapl138_hawk_console_init);
#endif

static void __init omapl138_hawk_map_io(void)
{
	da850_init();
}

MACHINE_START(OMAPL138_HAWKBOARD, "AM18x/OMAP-L138 Hawkboard")
	.boot_params	= (DA8XX_DDR_BASE + 0x100),
	.map_io		= omapl138_hawk_map_io,
	.init_irq	= cp_intc_init,
	.timer		= &davinci_timer,
	.init_machine	= omapl138_hawk_init,
MACHINE_END
