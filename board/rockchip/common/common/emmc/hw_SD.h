/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SD.h
//����:SD protocol api file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SD.h,v $
Revision 1.1  2011/03/29 09:20:48  Administrator
1��֧������8GB EMMC
2��֧��emmc boot 1��boot 2��д��IDBLOCK����

Revision 1.1  2011/01/18 07:20:30  Administrator
*** empty log message ***

Revision 1.1  2011/01/07 11:55:40  Administrator
*** empty log message ***

Revision 1.1.1.1  2009/08/18 06:43:27  Administrator
no message

Revision 1.1.1.1  2009/08/14 08:02:01  Administrator
no message

****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SDP_API_H_
#define _SDP_API_H_

/****************************************************************/
//���⺯������
/****************************************************************/
void   SD1X_Init(void *pCardInfo);
void   SD20_Init(void *pCardInfo);

#endif //end of #ifndef _SDP_API_H

#endif //end of #ifdef DRIVERS_SDMMC
