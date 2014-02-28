/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    drive.h
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/

#ifndef _DRIVE_H
#define _DRIVE_H
//1�����ò���
#define 	MAX_DRIVE                   1   //���֧�ֵ���������

//1��������
#define 	DISK_SYS                    0
#define 	DISK_FLASH                  1
#define 	DISK_SD                     2

//1�ṹ����

/***************************************************************************
������Ϣ�ṹ��
***************************************************************************/
typedef struct _DRIVE_INFO
{
    uint8   Valid;
    uint8   FATType;
    uint8 	LogSecPerClus;
	uint8	NumFATs;
	uint8	SecPerClus;
	uint16	BytsPerSec;
	uint16	ResvdSecCnt;
	uint16	RootEntCnt;
	uint32	RootClus;
	uint32	FSInfo;
	uint32  FATSz;
	uint32  RootDirSectors;
	uint32  FirstDataSector;
	uint32  PBRSector;
    uint32  PathClus;
	uint32  TotClus;
	uint32  TotSec;
    uint32  FreeClus;
    FDT_REF FdtRef;                 //FDTĿ¼�ο���, ���ڿ��ٶ�λ
    SubDir  DirInfo[MAX_DIR_DEPTH]; //Ŀ¼�ṹ����Ϣ
    uint8   DirDeep;
}DRIVE_INFO, *pDRIVE_INFO;


//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_DRIVE
		#define	EXT
#else
		#define	EXT		extern
#endif
EXT		uint8           CurDrive;
//EXT		uint8           DriveLowFormat;
EXT		DRIVE_INFO      DriveInfo[MAX_DRIVE];

//1����ԭ������
extern	pDRIVE_INFO Mount(uint8 Drive);
extern	void        Demount(uint8 Drive);
extern	pDRIVE_INFO GetDriveInfo(uint8 Drive);
extern	uint8       GetDrive(char *Path);
extern	uint32      GetCapacity(uint8 Drive);
extern    uint8 WriteSecs(uint8 LUN, uint32 Index, void *buf, uint32 nSec);
extern    uint8 ReadSecs(uint8 LUN, uint32 Index, void *buf, uint32 nSec);
extern    void DRVDelayUs(uint32 us);


//1�����
#ifdef IN_DRIVE
#else
#endif
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

