/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_MMC.h
//����:MMC protocol api file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_MMC.h,v $
Revision 1.1  2011/03/29 09:20:48  Administrator
1��֧������8GB EMMC
2��֧��emmc boot 1��boot 2��д��IDBLOCK����

Revision 1.1  2011/01/18 07:20:30  Administrator
*** empty log message ***

Revision 1.1  2011/01/07 11:55:39  Administrator
*** empty log message ***

Revision 1.1.1.1  2009/08/18 06:43:27  Administrator
no message

Revision 1.1.1.1  2009/08/14 08:02:01  Administrator
no message

****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _MMCP_API_H_
#define _MMCP_API_H_

/****************************************************************/
//���⺯������
/****************************************************************/
void   MMC_Init(void *pCardInfo);
int32  MMC_SwitchBoot(void *pCardInfo, bool enable, uint32 partition);
int32  MMC_AccessBootPartition(void *pCardInfo, uint32 partition);

uint8 MMC_GetMID(void);

#endif //end of #ifndef _MMCP_API_H

#endif //end of #ifdef DRIVERS_SDMMC
