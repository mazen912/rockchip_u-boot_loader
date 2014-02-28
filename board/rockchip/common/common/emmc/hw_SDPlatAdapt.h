/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SDPlatAdapt.h
//����:RK28 SD/MMC driver Platform adaptation head file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SDPlatAdapt.h,v $
Revision 1.1  2011/03/29 09:20:48  Administrator
1��֧������8GB EMMC
2��֧��emmc boot 1��boot 2��д��IDBLOCK����

Revision 1.1  2011/01/18 07:20:31  Administrator
*** empty log message ***

Revision 1.1  2011/01/07 11:55:41  Administrator
*** empty log message ***

Revision 1.1.1.1  2009/08/18 06:43:27  Administrator
no message

Revision 1.1.1.1  2009/08/14 08:02:01  Administrator
no message

****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SDPAM_H_
#define _SDPAM_H_

#define SDMMC_NO_PLATFORM    0    //ָʾ��ǰ�ǲ���ֻ��SDMMC�Ĵ��룬����ģ��Ĵ��붼���ã�1:ֻ��SDMMCģ�飬0:�õ�����ģ��

#if SDMMC_NO_PLATFORM
#define SDPAM_MAX_AHB_FREQ   166   //MHz
#else
#define SDPAM_MAX_AHB_FREQ   200//FREQ_HCLK_MAX
#endif

/****************************************************************/
//���⺯������
/****************************************************************/
void   SDPAM_FlushCache(void *adr, uint32 size);
void   SDPAM_CleanCache(void *adr, uint32 size);
uint32 SDPAM_GetAHBFreq(void);
void   SDPAM_SDCClkEnable(SDMMC_PORT_E nSDCPort, bool enable);
void   SDPAM_SDCReset(SDMMC_PORT_E nSDCPort);
void   SDPAM_SetMmcClkDiv(SDMMC_PORT_E nSDCPort, uint32 div);
bool   SDPAM_DMAStart(SDMMC_PORT_E nSDCPort, uint32 dstAddr, uint32 srcAddr, uint32 size, bool rw, pFunc CallBack);
bool   SDPAM_DMAStop(SDMMC_PORT_E nSDCPort, bool rw);
bool   SDPAM_INTCRegISR(SDMMC_PORT_E nSDCPort, pFunc Routine);
bool   SDPAM_INTCEnableIRQ(SDMMC_PORT_E nSDCPort);
bool   SDPAM_IOMUX_SetSDPort(SDMMC_PORT_E nSDCPort, HOST_BUS_WIDTH_E width);
bool   SDPAM_IOMUX_PwrEnGPIO(SDMMC_PORT_E nSDCPort);
bool   SDPAM_IOMUX_DetGPIO(SDMMC_PORT_E nSDCPort);
void   SDPAM_ControlPower(SDMMC_PORT_E nSDCPort, bool enable);
bool   SDPAM_IsCardPresence(SDMMC_PORT_E nSDCPort);

int32 eMMC_changemode(uint8 mode);


#endif //end of #ifndef _SDPAM_H_

#endif //end of #ifdef DRIVERS_SDMMC
