/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __ASM_ARCH_MX6_MX6DL_PINS_H__
#define __ASM_ARCH_MX6_MX6DL_PINS_H__

#include <asm/imx-common/iomux-v3.h>

/* Use to set PAD control */
#define PAD_CTL_HYS		(1 << 16)
#define PAD_CTL_PUS_100K_DOWN	(0 << 14)
#define PAD_CTL_PUS_47K_UP	(1 << 14)
#define PAD_CTL_PUS_100K_UP	(2 << 14)
#define PAD_CTL_PUS_22K_UP	(3 << 14)

#define PAD_CTL_PUE		(1 << 13)
#define PAD_CTL_PKE		(1 << 12)
#define PAD_CTL_ODE		(1 << 11)
#define PAD_CTL_SPEED_LOW	(1 << 6)
#define PAD_CTL_SPEED_MED	(2 << 6)
#define PAD_CTL_SPEED_HIGH	(3 << 6)
#define PAD_CTL_DSE_DISABLE	(0 << 3)
#define PAD_CTL_DSE_240ohm	(1 << 3)
#define PAD_CTL_DSE_120ohm	(2 << 3)
#define PAD_CTL_DSE_80ohm	(3 << 3)
#define PAD_CTL_DSE_60ohm	(4 << 3)
#define PAD_CTL_DSE_48ohm	(5 << 3)
#define PAD_CTL_DSE_40ohm	(6 << 3)
#define PAD_CTL_DSE_34ohm	(7 << 3)
#define PAD_CTL_SRE_FAST	(1 << 0)
#define PAD_CTL_SRE_SLOW	(0 << 0)

#define IOMUX_CONFIG_SION 0x10
#define NO_MUX_I                0
#define NO_PAD_I                0
enum {
	MX6_PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK = IOMUX_PAD(0x03B0, 0x009C, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DI0_PIN15__IPU1_DI0_PIN15	= IOMUX_PAD(0x03B4, 0x00A0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DI0_PIN2__IPU1_DI0_PIN2	= IOMUX_PAD(0x03B8, 0x00A4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DI0_PIN3__IPU1_DI0_PIN3	= IOMUX_PAD(0x03BC, 0x00A8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DI0_PIN4__GPIO_4_20		= IOMUX_PAD(0x03C0, 0x00AC, 5, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT0__IPU1_DISP0_DAT_0	= IOMUX_PAD(0x03C4, 0x00B0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT1__IPU1_DISP0_DAT_1	= IOMUX_PAD(0x03C8, 0x00B4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT10__IPU1_DISP0_DAT_10 = IOMUX_PAD(0x03CC, 0x00B8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT11__IPU1_DISP0_DAT_11 = IOMUX_PAD(0x03D0, 0x00BC, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT12__IPU1_DISP0_DAT_12 = IOMUX_PAD(0x03D4, 0x00C0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT13__IPU1_DISP0_DAT_13 = IOMUX_PAD(0x03D8, 0x00C4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT14__IPU1_DISP0_DAT_14 = IOMUX_PAD(0x03DC, 0x00C8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT15__IPU1_DISP0_DAT_15 = IOMUX_PAD(0x03E0, 0x00CC, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT16__IPU1_DISP0_DAT_16 = IOMUX_PAD(0x03E4, 0x00D0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT17__IPU1_DISP0_DAT_17 = IOMUX_PAD(0x03E8, 0x00D4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT18__IPU1_DISP0_DAT_18 = IOMUX_PAD(0x03EC, 0x00D8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT19__IPU1_DISP0_DAT_19 = IOMUX_PAD(0x03F0, 0x00DC, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT2__IPU1_DISP0_DAT_2	 = IOMUX_PAD(0x03F4, 0x00E0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT20__IPU1_DISP0_DAT_20 = IOMUX_PAD(0x03F8, 0x00E4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT21__IPU1_DISP0_DAT_21 = IOMUX_PAD(0x03FC, 0x00E8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT22__IPU1_DISP0_DAT_22 = IOMUX_PAD(0x0400, 0x00EC, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT23__IPU1_DISP0_DAT_23 = IOMUX_PAD(0x0404, 0x00F0, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT3__IPU1_DISP0_DAT_3	= IOMUX_PAD(0x0408, 0x00F4, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT4__IPU1_DISP0_DAT_4	= IOMUX_PAD(0x040C, 0x00F8, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT5__IPU1_DISP0_DAT_5	= IOMUX_PAD(0x0410, 0x00FC, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT6__IPU1_DISP0_DAT_6	= IOMUX_PAD(0x0414, 0x0100, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT7__IPU1_DISP0_DAT_7	= IOMUX_PAD(0x0418, 0x0104, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT8__IPU1_DISP0_DAT_8	= IOMUX_PAD(0x041C, 0x0108, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_DISP0_DAT9__IPU1_DISP0_DAT_9	= IOMUX_PAD(0x0420, 0x010C, 0, 0x0000, 0, PAD_CTL_DSE_120ohm),
	MX6_PAD_EIM_D16__ECSPI1_SCLK		= IOMUX_PAD(0x0514, 0x0144, 1, 0x07D8, 2, 0),
	MX6_PAD_EIM_D17__ECSPI1_MISO		= IOMUX_PAD(0x0518, 0x0148, 1, 0x07DC, 2, 0),
	MX6_PAD_EIM_D18__ECSPI1_MOSI		= IOMUX_PAD(0x051C, 0x014C, 1, 0x07E0, 2, 0),
	MX6_PAD_EIM_D19__GPIO_3_19		= IOMUX_PAD(0x0520, 0x0150, 5, 0x0000, 0, 0),
	MX6_PAD_EIM_D21__GPIO_3_21		= IOMUX_PAD(0x0528, 0x0158, 5, 0x0000, 0, 0),
	MX6_PAD_EIM_D21__I2C1_SCL		= IOMUX_PAD(0x0528, 0x0158, 6 | IOMUX_CONFIG_SION, 0x0868, 1, 0),
	MX6_PAD_EIM_D23__GPIO_3_23		= IOMUX_PAD(0x0530, 0x0160, 5, 0x0000, 0, 0),
	MX6_PAD_EIM_D26__UART2_TXD		= IOMUX_PAD(0x053C, 0x016C, 4, 0x0000, 0, 0),
	MX6_PAD_EIM_D27__UART2_RXD		= IOMUX_PAD(0x0540, 0x0170, 4, 0x0904, 1, 0),
	MX6_PAD_EIM_D28__I2C1_SDA		= IOMUX_PAD(0x0544, 0x0174, 1 | IOMUX_CONFIG_SION, 0x086C, 1, 0),
	MX6_PAD_EIM_D28__GPIO_3_28		= IOMUX_PAD(0x0544, 0x0174, 5, 0x0000, 0, 0),
	MX6_PAD_ENET_MDC__ENET_MDC		= IOMUX_PAD(0x05B8, 0x01E8, 1, 0x0000, 0,  0),
	MX6_PAD_ENET_MDIO__ENET_MDIO		= IOMUX_PAD(0x05BC, 0x01EC, 1, 0x0810, 0, 0),
	MX6_PAD_ENET_REF_CLK__ENET_TX_CLK	= IOMUX_PAD(0x05C0, 0x01F0, 1, 0x0000, 0, 0),
	MX6_PAD_ENET_RXD0__GPIO_1_27		= IOMUX_PAD(0x05C8, 0x01F8, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_16__GPIO_7_11		= IOMUX_PAD(0x05E4, 0x0214, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_16__I2C3_SDA		= IOMUX_PAD(0x05E4, 0x0214, 6 | IOMUX_CONFIG_SION, 0x087C, 1, 0),
	MX6_PAD_GPIO_17__GPIO_7_12		= IOMUX_PAD(0x05E8, 0x0218, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_18__GPIO_7_13		= IOMUX_PAD(0x05EC, 0x021C, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_19__GPIO_4_5		= IOMUX_PAD(0x05F0, 0x0220, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_5__GPIO_1_5		= IOMUX_PAD(0x0600, 0x0230, 5, 0x0000, 0, 0),
	MX6_PAD_GPIO_5__I2C3_SCL		= IOMUX_PAD(0x0600, 0x0230, 6 | IOMUX_CONFIG_SION, 0x0878, 2, 0),
	MX6_PAD_KEY_COL3__I2C2_SCL		= IOMUX_PAD(0x0638, 0x0250, 4 | IOMUX_CONFIG_SION, 0x0870, 1, 0),
	MX6_PAD_KEY_COL3__GPIO_4_12		= IOMUX_PAD(0x0638, 0x0250, 5, 0x0000, 0, 0),
	MX6_PAD_KEY_ROW3__I2C2_SDA		= IOMUX_PAD(0x064C, 0x0264, 4 | IOMUX_CONFIG_SION, 0x0874, 1, 0),
	MX6_PAD_KEY_ROW3__GPIO_4_13		= IOMUX_PAD(0x064C, 0x0264, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_CS1__GPIO_6_14		= IOMUX_PAD(0x0660, 0x0278, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_CS2__GPIO_6_15		= IOMUX_PAD(0x0664, 0x027C, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_CS3__GPIO_6_16		= IOMUX_PAD(0x0668, 0x0280, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_D1__GPIO_2_1		= IOMUX_PAD(0x0670, 0x0288, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_D2__GPIO_2_2		= IOMUX_PAD(0x0674, 0x028C, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_D3__GPIO_2_3		= IOMUX_PAD(0x0678, 0x0290, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_D4__GPIO_2_4		= IOMUX_PAD(0x067C, 0x0294, 5, 0x0000, 0, 0),
	MX6_PAD_NANDF_D6__GPIO_2_6		= IOMUX_PAD(0x0684, 0x029C, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RD0__ENET_RGMII_RD0     = IOMUX_PAD(0x0694, 0x02AC, 1, 0x0818, 1, 0),
	MX6_PAD_RGMII_RD0__GPIO_6_25		= IOMUX_PAD(0x0694, 0x02AC, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RD1__ENET_RGMII_RD1	= IOMUX_PAD(0x0698, 0x02B0, 1, 0x081C, 1, 0),
	MX6_PAD_RGMII_RD1__GPIO_6_27		= IOMUX_PAD(0x0698, 0x02B0, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RD2__ENET_RGMII_RD2	= IOMUX_PAD(0x069C, 0x02B4, 1, 0x0820, 1, 0),
	MX6_PAD_RGMII_RD2__GPIO_6_28		= IOMUX_PAD(0x069C, 0x02B4, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RD3__ENET_RGMII_RD3	= IOMUX_PAD(0x06A0, 0x02B8, 1, 0x0824, 1, 0),
	MX6_PAD_RGMII_RD3__GPIO_6_29		= IOMUX_PAD(0x06A0, 0x02B8, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RX_CTL__RGMII_RX_CTL	= IOMUX_PAD(0x06A4, 0x02BC, 1, 0x0828, 1, 0),
	MX6_PAD_RGMII_RX_CTL__GPIO_6_24	= IOMUX_PAD(0x06A4, 0x02BC, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_RXC__ENET_RGMII_RXC	= IOMUX_PAD(0x06A8, 0x02C0, 1, 0x0814, 1, 0),
	MX6_PAD_RGMII_RXC__GPIO_6_30		= IOMUX_PAD(0x06A8, 0x02C0, 5, 0x0000, 0, 0),
	MX6_PAD_RGMII_TD0__ENET_RGMII_TD0	= IOMUX_PAD(0x06AC, 0x02C4, 1, 0x0000, 0, 0),
	MX6_PAD_RGMII_TD1__ENET_RGMII_TD1	= IOMUX_PAD(0x06B0, 0x02C8, 1, 0x0000, 0, 0),
	MX6_PAD_RGMII_TD2__ENET_RGMII_TD2	= IOMUX_PAD(0x06B4, 0x02CC, 1, 0x0000, 0, 0),
	MX6_PAD_RGMII_TD3__ENET_RGMII_TD3	= IOMUX_PAD(0x06B8, 0x02D0, 1, 0x0000, 0, 0),
	MX6_PAD_RGMII_TX_CTL__RGMII_TX_CTL	= IOMUX_PAD(0x06BC, 0x02D4, 1, 0x0000, 0, 0),
	MX6_PAD_RGMII_TXC__ENET_RGMII_TXC	= IOMUX_PAD(0x06C0, 0x02D8, 1, 0x0000, 0, 0),
	MX6_PAD_SD1_CMD__GPIO_1_18		= IOMUX_PAD(0x06C8, 0x02E0, 5, 0x0000, 0, 0),
	MX6_PAD_SD1_DAT3__GPIO_1_21		= IOMUX_PAD(0x06D8, 0x02F0, 5, 0x0000, 0, 0),
	MX6_PAD_SD3_CLK__USDHC3_CLK		= IOMUX_PAD(0x06F4, 0x030C, 0, 0x0934, 1, 0),
	MX6_PAD_SD3_CMD__USDHC3_CMD		= IOMUX_PAD(0x06F8, 0x0310, 0 | IOMUX_CONFIG_SION, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT0__USDHC3_DAT0		= IOMUX_PAD(0x06FC, 0x0314, 0, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT1__USDHC3_DAT1		= IOMUX_PAD(0x0700, 0x0318, 0, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT2__USDHC3_DAT2		= IOMUX_PAD(0x0704, 0x031C, 0, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT3__USDHC3_DAT3		= IOMUX_PAD(0x0708, 0x0320, 0, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT5__GPIO_7_0		= IOMUX_PAD(0x0710, 0x0328, 5, 0x0000, 0, 0),
	MX6_PAD_SD3_DAT6__UART1_RXD		= IOMUX_PAD(0x0714, 0x032C, 1, 0x08FC, 2, 0),
	MX6_PAD_SD3_DAT7__UART1_TXD		= IOMUX_PAD(0x0718, 0x0330, 1, 0x0000, 0, 0),
	MX6_PAD_SD4_CLK__USDHC4_CLK		= IOMUX_PAD(0x0720, 0x0338, 0, 0x0938, 1, 0),
	MX6_PAD_SD4_CMD__USDHC4_CMD		= IOMUX_PAD(0x0724, 0x033C, 0 | IOMUX_CONFIG_SION, 0x0000, 0, 0),
	MX6_PAD_SD4_DAT0__USDHC4_DAT0		= IOMUX_PAD(0x0728, 0x0340, 1, 0x0000, 0, 0),
	MX6_PAD_SD4_DAT1__USDHC4_DAT1		= IOMUX_PAD(0x072C, 0x0344, 1, 0x0000, 0, 0),
	MX6_PAD_SD4_DAT2__USDHC4_DAT2		= IOMUX_PAD(0x0730, 0x0348, 1, 0x0000, 0, 0),
	MX6_PAD_SD4_DAT3__USDHC4_DAT3		= IOMUX_PAD(0x0734, 0x034C, 1, 0x0000, 0, 0),
};
#endif	/* __ASM_ARCH_MX6_MX6DL_PINS_H__ */
