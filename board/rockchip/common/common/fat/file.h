/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    file.h
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#ifndef _FILE_H
#define _FILE_H

//1�����ò���
#define 	MAX_OPEN_FILES		    4		/*����ͬʱ�򿪵��ļ���Ŀ*/
#define 	MAX_LFN_ENTRIES   		3		/*���3*13���ַ�*/


//1��������

#define 	NOT_OPEN_FILE			-1		//���ܴ��ļ�,�ļ������


/* �ļ�ϵͳ���ݿ��,�μ�����, ���ܸ��Ķ���ֵ*/
#define	X8		1
#define	X16		2
#define	X32		4



/* ���ļ���*/
#define 	MAX_FILENAME_LEN  	    ((MAX_LFN_ENTRIES * 13 * 2 + 2)/2)		//��β��NULռ��2B
#define 	LFN_SEQ_MASK			0x3f

/* �ļ��򿪷�ʽ */
#define 	FILE_FLAGS_READ          	1 << 0		//�ɶ�
#define 	FILE_FLAGS_WRITE        	1 << 1		//��д

/* �ļ�ָ�������ʽ */
#define 	SEEK_SET    				0           		//���ļ���ʼ���ƶ��ļ�ָ��
#define 	SEEK_CUR    				1           		//���ļ���ǰλ���ƶ��ļ�ָ��
#define 	SEEK_END    				2           		//���ļ�β�ƶ��ļ�ָ��
#define 	SEEK_REF    				3           		//���ļ��ο��㿪ʼ


//1�ṹ����
//���ҽṹ��
typedef struct _FIND_DATA
{
    uint8   Drive;
	uint32	Index;
	uint32	Clus;     			//��ǰ�غ�
} FIND_DATA;

//�ļ���Ϣ�ṹ��
typedef struct _FILE
{
	uint8	Flags;          	//һЩ��־
	char	Name[11];    		//�ļ���
	uint8   Drive;
	uint32	Sec;	            //��ǰ����
	uint32	DirClus;	        //����Ŀ¼��ʼ�غ�
	uint32	FileSize;		    //�ļ���С
	uint32	FstClus;			//��ʼ�غ�
	uint32	Clus;     			//��ǰ�غ�
	uint32	Offset;			    //�ļ�ָ��ƫ����
	uint32	RefClus;    		//��ǰ�غ�
	uint32	RefOffset;		    //�ļ�ָ��ƫ����
} MY_FILE, *pFILE;


typedef struct _recovery_info
{
	uint8 Drive;
	uint8 Flag;
	char FileName[11];
    uint32 FstClus;
	uint32 DirClus;
} RCV_INFO, *pRCV_INFO;



//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_FILE
		#define	EXT
#else
		#define	EXT		extern
#endif		
		
EXT		MY_FILE 	FileInfo[MAX_OPEN_FILES];		//ͬʱ���ļ���Ϣ��
EXT		uint16 		LongFileName[MAX_FILENAME_LEN];	//���ļ���


/*******************************************************************************************************/
//1����ԭ������
//#ifndef IN_FILE
extern 	void 	FileInit(void);
extern 	pFILE 	FileCreate(char *Path, char *DirFileName);
extern 	uint8 	FileDelete(char *Path, char *DirFileName);
extern 	pFILE 	FileOpen(uint8 Drive, uint32 DirClus, char *DirFileName, char *Type);
extern 	pFILE 	FileOpenExt(char *Path, char *DirFileName, char *Type);
extern 	uint8 	FileClose(pFILE fp);
extern 	uint32  FileRead(void *Buf, uint8 Size, uint32 count, pFILE fp);
extern 	uint32 	FileWrite(void *Buf, uint8 Size, uint32 count, pFILE fp);
extern 	uint8 	FileModify(void *buf, uint32 offset, uint32 Bytes, pFILE fp);
extern 	uint8 	FileCopy(char *SrcPath, char *DestPath, FDT *SrcFile);
extern 	bool 	FileEof(pFILE fp);
extern 	uint8 	FileSeek(pFILE fp, int32 offset, uint8 Whence);
extern 	uint8 	FindOpenFile(uint32 DirClus, char *FileName);
extern 	bool 	FileExtNameMatch(char *SrcExtName, char *Filter);
extern 	uint8 	FindFile(FDT *Rt, uint16 FileNum, char *Path, char *ExtName);
extern 	uint8 	FindFileBrowser(FDT *Rt, uint16 FileNum, char *Path, char *ExtName);
extern 	uint8 	FindFirst(FDT *Rt, FIND_DATA* FindData, char *Path, char *ExtName);
extern 	uint8 	FindNext(FDT *Rt, FIND_DATA* FindData, char *ExtName);
extern 	uint8 	FindDirFirst(FDT *Rt, FIND_DATA* FindData, char *Path);
extern 	uint8 	FindDirNext(FDT *Rt, FIND_DATA* FindData);
extern 	uint16 	GetTotalFiles(char *Path, char *ExtName);
extern 	void 	GetLongFileName(char *lfn);
extern 	uint16 	GetCurFileNum(uint8 Drive, uint16 FileNum, char *ExtName);
extern 	uint16 	GetGlobeFileNum(uint16 FileNum, char *Path, char *ExtName);
extern 	void 	StrUprCase(char *name);

extern 	uint8 	FileRefSet(pFILE fp);
extern 	uint8 	FileRefReset(pFILE fp);
extern 	void    VolumeCreate(uint8 Drive, char *DirFileName);
extern 	uint8	RecoveryEncInfo(RCV_INFO* RecoveryInfo);
extern 	void 	SaveEncInfo(RCV_INFO* RecoveryInfo);
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

