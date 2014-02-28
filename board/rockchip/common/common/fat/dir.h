/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    dir.h
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/

#ifndef _DIR_H
#define _DIR_H
//1�����ò���
#define	MAX_DIR_DEPTH		4				//Ŀ¼���4��


//1�ṹ����
typedef __packed struct _SubDir
{
	uint16 TotalFile;
	uint16 TotalSubDir;
	uint16 CurDirNum;
	char DirName[11];						//��Ŀ¼��ռ��11���ַ�
	char LongDirName[MAX_FILENAME_LEN];	    //��Ŀ¼��
}SubDir, *pSubDir;




//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_DIR
		#define	EXT
#else
		#define	EXT		extern
#endif		
//EXT		SubDir	    SubDirInfo[MAX_DRIVE][MAX_DIR_DEPTH];	//��Ŀ¼��Ϣ�б�

//1����ԭ������
extern	uint16 	BuildDirInfo(uint8 Drive, char* ExtName);
extern	void 	GotoNextDir(uint8 Drive, char* ExtName);
extern	void 	GotoPrevDir(uint8 Drive, char* ExtName);
extern	void 	GotoLastDir(uint8 Drive, char* ExtName);
extern	void 	GotoRootDir(uint8 Drive, char* ExtName);
extern	uint32 	ChangeDir(uint8 Drive, uint16 SubDirIndex);
extern	uint8 	MakeDir(char *Path, char *DirFileName);
extern	uint16 	GetTotalSubDir(char *Path);
extern	uint32 	GetDirClusIndex(char *Path);


//1�����
#ifdef IN_DIR
#else
#endif
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

