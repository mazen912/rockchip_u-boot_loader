/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hwapi_SDOsAdapt.h
//����:RK28 SD/MMC driver OS adaptation API file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hwapi_SDOsAdapt.h,v $
Revision 1.1  2011/03/29 09:20:48  Administrator
1��֧������8GB EMMC
2��֧��emmc boot 1��boot 2��д��IDBLOCK����

Revision 1.1  2011/01/18 07:20:31  Administrator
*** empty log message ***

Revision 1.1  2011/01/07 11:55:40  Administrator
*** empty log message ***

Revision 1.1.1.1  2009/08/18 06:43:27  Administrator
no message

Revision 1.1.1.1  2009/08/14 08:02:01  Administrator
no message

****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SDOAM_H_
#define _SDOAM_H_

typedef uint32*   pEVENT;
typedef uint32*   pMUTEX;

#define MSG_CARD_INSERT    (0x1)
#define MSG_CARD_REMOVE    (0x2)
#define MSG_CARD_USEABLE   (0x3)

/****************************************************************/
//���⺯������
/****************************************************************/
void  *SDOAM_Malloc(uint32 nSize);
void   SDOAM_Free(void *ptr);
//void   SDOAM_Printf(const char *fmt, ...);
void   SDOAM_PrintfAll(void);
void   SDOAM_SendMsg(uint32 code, uint32 param);
bool   SDOAM_GetMsg(uint32 *pCode, uint32 *pMsg);
pMUTEX SDOAM_CreateMutex(uint8 *name);
void   SDOAM_RequestMutex(pMUTEX handle);
void   SDOAM_ReleaseMutex(pMUTEX handle);
pEVENT SDOAM_CreateEvent(uint8 *name);
void   SDOAM_SetEvent(pEVENT handle, uint32 event);
void   SDOAM_GetEvent(pEVENT handle, uint32 event);
void   SDOAM_Delay(uint32 us);
void  *SDOAM_Memcpy(void *dest, void *src, uint32 count);
void  *SDOAM_Memset(void *dest, uint8 ch, uint32 count);
int32  SDOAM_Strlen(uint8 *s);
char  *SDOAM_Strcat(uint8 *dest, uint8 *src);
/* only used for SDT */
int32  SDOAM_Memcmp(void *dest, void *src, uint32 count);

#endif //end of #ifndef _SDOAM_H_

#endif //end of #ifdef DRIVERS_SDMMC
