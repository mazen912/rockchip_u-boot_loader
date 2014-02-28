/********************************************************************************
*********************************************************************************
                        COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
                                --  ALL RIGHTS RESERVED  --

File Name:      ISR.C
Author:         XUESHAN LIN
Created:        1st Dec 2008
Modified:
Revision:       1.00
********************************************************************************
********************************************************************************/
#include    "../../armlinux/config.h"

uint32 RockusbEn = 1;
/***************************************************************************
��������:ʹ��IRQ
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void EnableIRQ(void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}


/***************************************************************************
��������:ʹ��IRQ
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void DisableIRQ(void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return (old & 0x80) == 0;
}

uint32 IRQEnable(eINT_NUM intNum)
{
	uint32 M,N;
	uint32 shift = (intNum % 4) * 8;
	uint32 offset = (intNum /4);
    if (intNum >= INT_MAXNUM)
    {
        return(-1);
    }
    M=intNum/32;
    N=intNum%32;
    g_giccReg->ICCICR&=(~0x08);     //IntSetIRQ
    g_gicdReg->ICDISER[M]=(0x1<<(N));
    g_gicdReg->ITARGETSR[offset] |= (1 << shift);
    return(0);
}
uint32 IRQDisable(eINT_NUM intNum)
{
	uint32 M,N;
    if (intNum >= INT_MAXNUM)
    {
        return(-1);
    }
    M=intNum/32;
    N=intNum%32;
    g_gicdReg->ICDICER[M]=(0x1<<(N));
    return(0);
}



/***************************************************************************
��������:�жϼĴ�����ʼ��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void EnableOtgIntr(void)
{
    //g_giccReg->ICCICR&=(~0x08);     //IntSetIRQ
    //g_gicdReg->ICDISER[USB_OTG_INT_CH/32]=(0x1<<(USB_OTG_INT_CH % 32));
    IRQEnable(USB_OTG_INT_CH);
}

/***************************************************************************
��������:�жϼĴ�����ʼ��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void DisableOtgIntr(void)
{
    //g_gicdReg->ICDICER[USB_OTG_INT_CH/32]=(0x1<<(USB_OTG_INT_CH % 32));
    IRQDisable(USB_OTG_INT_CH);
}

/***************************************************************************
��������:�жϼĴ�����ʼ��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void InterruptInit(void)
{
    g_giccReg->ICCEOIR=USB_OTG_INT_CH;
    g_giccReg->ICCEOIR=INT_eMMC;
    g_giccReg->ICCICR=0x00;   //disable signalling the interrupt
    g_gicdReg->ICDDCR=0x00;  
                                              
    g_gicdReg->ICDICER[0]=0xFFFFFFFF;
    g_gicdReg->ICDICER[1]=0xFFFFFFFF;
    g_gicdReg->ICDICER[2]=0xFFFFFFFF;
    g_gicdReg->ICDICER[3]=0xFFFFFFFF;
    g_gicdReg->ICDICFR[3]&=(~(1<<1));
    EnableIRQ();   
	g_giccReg->ICCPMR=0xff;     //IntSetPrioFilt
	g_giccReg->ICCICR|=0x01;    //IntEnalbeSecureSignal
	g_giccReg->ICCICR|=0x02;    //IntEnalbeNoSecureSignal
	g_gicdReg->ICDDCR=0x01;     //IntEnalbeDistributor
}


/**************************************************************************
IRQ�жϷ����ӳ���
***************************************************************************/
void IrqHandler(void)
{
    uint32 intSrc;

	intSrc=g_giccReg->ICCIAR&0x3ff;     //IntGetIntID
	//g_giccReg->ICCEOIR=intSrc;
	//RkPrintf("Irq: %x\n", intSrc);
    if (intSrc == USB_OTG_INT_CH)
    {
        if(RockusbEn)
            UsbIsr();
        else if(RockusbEn == 0)
            udc_irq();
#ifdef DRIVERS_USB_APP
        else
            MscUsbIsr();
#endif
    }
#ifdef    RK_SDMMC_BOOT_EN
    else if(intSrc == INT_eMMC)
    {
        _SDC2IST();
    }
#endif    
    g_giccReg->ICCEOIR=intSrc;
}


