/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    FDT.h
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#ifndef _FDT_H
#define _FDT_H

//1�����ò���
#define 	FDT_CACHE_NUM               1

#define 	CACHE_FAT                   1
#define 	CACHE_FDT                   2
#define 	CACHE_DATA                  3

//1��������
/* FDT�ļ����� */
#define 	ATTR_READ_ONLY              0x01
#define 	ATTR_HIDDEN                 0x02
#define 	ATTR_SYSTEM                 0x04
#define 	ATTR_VOLUME_ID             	0x08
#define 	ATTR_LFN_ENTRY    		    0x0F      /* LFN entry attribute */
#define 	ATTR_DIRECTORY             	0x10
#define 	ATTR_ARCHIVE                0x20

/* FDT���� */
#define 	FILE_NOT_EXIST			    0
#define 	FILE_DELETED			    0xe5
#define 	ESC_FDT                    	0x05

/* ��������ֵ */
#define	    RETURN_OK                   0x00    	/* �����ɹ�*/
#define	    NOT_FIND_DISK               0x01    	/* �߼��̲�����*/
#define	    DISK_FULL                   0x02    	/* �߼�����*/
#define	    SECTOR_NOT_IN_CACHE         0x03    	/* ����û�б�cache  */
#define	    NOT_EMPTY_CACHE          	0x04    	/* û�п���cache*/
#define	    SECTOR_READ_ERR           	0x05    	/* ����������*/
#define	    CLUSTER_NOT_IN_DISK         0x06    	/* �߼�����û�д˴�  */
#define 	NOT_FIND_FDT                0x07    	/* û�з����ļ�(Ŀ¼)*/
#define 	NOT_FAT_DISK                0x08    	/* ��FAT�ļ�ϵͳ*/
#define 	FDT_OVER                    0x09    	/* FDT����������Χ*/
#define 	FDT_EXISTS                  0x0a    	/* �ļ�(Ŀ¼)�Ѿ�����*/
#define 	ROOT_FDT_FULL               0x0b    	/* ��Ŀ¼��*/
#define 	DIR_EMPTY                   0x0C    	/* Ŀ¼��*/
#define 	DIR_NOT_EMPTY               0x0d    	/* Ŀ¼����*/
#define 	PATH_NOT_FIND               0x0e    	/* ·��δ�ҵ�*/
#define 	FAT_ERR                     0x0f    	/* FAT�����*/
#define 	FILE_NAME_ERR               0x10    	/* �ļ�(Ŀ¼)������*/
#define 	FILE_EOF                    0x11    	/* �ļ�����*/
#define 	FILE_LOCK                   0x12    	/* �ļ�������*/
#define 	NOT_FIND_FILE               0x13    	/* û�з���ָ���ļ�*/
#define 	NOT_FIND_DIR                0x14    	/* û�з���ָ��Ŀ¼*/
#define 	NOT_RUN                     0xfd    	/* ����δִ��*/
#define 	BAD_COMMAND                 0xfe    	/* ��������*/
#define 	PARAMETER_ERR               0xff    	/* �Ƿ�����*/


//1�ṹ����

//Ŀ¼�����ӽṹ��
typedef struct _FDT_REF
{
	uint32	DirClus;			//Ŀ¼�״�
	uint32	CurClus;     		//��ǰ�غ�
	uint16	Cnt;				//��������
} FDT_REF;

typedef __packed struct _FDT
{
	char	Name[11];		    //���ļ������ļ���
	uint8	Attr;           	//�ļ�����
	uint8	NTRes;              //������NT
	uint8	CrtTimeTenth;  	    //����ʱ�䣨fat16������

	uint16	CrtTime;           	//����ʱ�䣨fat16������
	uint16	CrtDate;            //�������ڣ�fat16������
	uint16	LstAccDate;      	//���������ڣ�fat16������
	uint16	FstClusHI;         	//��ʼ�غŸ������ֽڣ�fat16������
	uint16	WrtTime;           	//���дʱ��
	uint16	WrtDate;            //���д����
	uint16	FstClusLO;          //��ʼ��(cluster)�ŵ������ֽ�
	uint32	FileSize;         	//�ļ���С
} FDT;


/***************************************************************************
��Ŀ¼��(32�ֽ�)
***************************************************************************/
typedef __packed struct _LONG_FDT
{
    uint8 	Order;
    char 	Name1[10];
    uint8 	Attr;
    uint8 	Type;
    uint8 	Chksum;
    char 	Name2[12];
    uint16 	FstClusLO;
    char 	Name3[4];
}LONG_FDT;

//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_FDT
		#define	EXT
#else
		#define	EXT		extern
#endif		
EXT		CACHE       FdtCache[FDT_CACHE_NUM];

//1����ԭ������
extern 	uint8 	ReadFDTInfo(FDT *Rt, uint8 Drive, uint32 SecIndex, uint16 ByteIndex);
extern 	uint8 	WriteFDTInfo(FDT *FDTData, uint8 Drive, uint32 SecIndex, uint16 ByteIndex);
extern 	uint8 	GetFDTInfo(FDT *Rt, uint8 Drive, uint32 ClusIndex, uint32 Index);
extern 	uint8   SetRootFDTInfo(uint8 Drive, uint32 Index, FDT *FDTData);
extern 	uint8 	SetFDTInfo(uint8 Drive, uint32 ClusIndex, uint32 Index, FDT *FDTData);
extern 	uint8 	FindFDTInfo(FDT *Rt, uint8 Drive, uint32 ClusIndex, char *FileName);
extern 	uint8 	AddFDT(uint8 Drive, uint32 ClusIndex, FDT *FDTData);
extern 	uint8 	ChangeFDT(uint8 Drive, uint32 ClusIndex, FDT *FDTData);
extern 	uint8 	DelFDT(uint8 Drive, uint32 ClusIndex, char *FileName);
extern 	uint8 	FDTIsLie(uint8 Drive, uint32 ClusIndex, char *FileName);
extern 	uint8   ClearClus(uint8 Drive, uint32 Index);
#endif

