

#ifndef _CHIP_DEPEND_H
#define _CHIP_DEPEND_H

#include "gpio_oper.h"

#define     RK3068_POP_CHIP_TAG      0xF
#define     RK3068_CHIP_TAG          0x5
#define     RK3066_CHIP_TAG          0x0
#define     RK3000_CHIP_TAG          0xC

#define         CHIP_RK3066     0
#define         CHIP_RK3066B    1
#define         CHIP_RK3168     2
#define         CHIP_RK3188     3
#define         CHIP_RK3188B    4


extern uint8    ChipType;

//typedef volatile unsigned int       REG32;
#define RKLD_APB_FREQ           (50*1000) //LOADER �� APBƵ�ʣ� khz 
#define APB0_TIMER_BASE         TIMER0_BASE_ADDR
#define RKLD_HWTM1_CON          ((REG32*)(APB0_TIMER_BASE+0X0008)) //config �Ĵ��� 
#define RKLD_HWTM1_LR           ((REG32*)(APB0_TIMER_BASE+0X0000)) // ��ʼ���� �Ĵ���.
#define RKLD_HWTM1_CVR          ((REG32*)(APB0_TIMER_BASE+0X0004)) // ��ʼ���� �Ĵ���.
#define KRTIMELoaderCount       (uint32)RKLD_APB_FREQ*80*1000      /* 0xee6b2800 */

//#define LOADER_FLAG_REG         ((REG32*)(GRF_BASE+0x1C8)) //GRF_OS_REG0
#define LOADER_FLAG_REG         ((REG32*)(PMU_BASE_ADDR+0x40)) //PMU_OS_REG0

//����Loader�����쳣����
#define SYS_LOADER_ERR_FLAG      0X1888AAFF 

//extern void ModifyUsbVidPid(USB_DEVICE_DESCRIPTOR * pDeviceDescr);
extern bool UsbPhyReset(void);
extern void FlashCsInit(void);

#endif
