/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    dir.C
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define     IN_DIR
#include    "../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�
#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:
** ��������	:����Ŀ¼��Ϣ
** �䡡��	:��չ�������ļ�
** �䡡��	:�ļ���
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint16 BuildDirInfo(uint8 Drive, char* ExtName)
{
	uint16 TotalFiles=0;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
	
	GotoRootDir(Drive, ExtName);
	while (1)
	{
		TotalFiles += pInfo->DirInfo[pInfo->DirDeep].TotalFile;
		GotoNextDir(Drive, ExtName);			//����������Ŀ¼
		if (pInfo->DirDeep == 0)
			break;
	}
	return (TotalFiles);
}


/*********************************************************************************************************
** ��������	:
** ��������	:ת����һ��Ŀ¼
** �䡡��	:��չ�������ļ�
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
void GotoNextDir(uint8 Drive, char* ExtName)
{
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (pInfo->DirInfo[pInfo->DirDeep].TotalSubDir == 0)	//��Ŀ¼��û��Ŀ¼��ΪҶ���,Ҫ�Ҹ�Ŀ¼��ͬ��Ŀ¼
	{
		while (1)
		{
			if (pInfo->DirDeep == 0)					//�ҵ���Ŀ¼�˲�����������
				return;

			pInfo->PathClus=ChangeDir(Drive, 2);	//��ȡ��һ��Ŀ¼��ʼ�غ�(..Ŀ¼)
			pInfo->DirDeep--;						    //��ָ����һ��Ŀ¼
			if(pInfo->DirInfo[pInfo->DirDeep].CurDirNum < pInfo->DirInfo[pInfo->DirDeep].TotalSubDir)
			{
				pInfo->DirInfo[pInfo->DirDeep].CurDirNum++;
				pInfo->DirDeep++;
				pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep-1].CurDirNum);
				break;
			}
		}
	}
	else			//��Ŀ¼�»�����Ŀ¼,Ҫ�����ĵ�һ����Ŀ¼
	{
		if (pInfo->DirDeep == 0)
			pInfo->DirInfo[pInfo->DirDeep].CurDirNum=1;						//��Ŀ¼�ĵ�һ��Ŀ¼����1
		else
			pInfo->DirInfo[pInfo->DirDeep].CurDirNum=3;						//��Ŀ¼�ĵ�һ��Ŀ¼����3

		if (++pInfo->DirDeep < MAX_DIR_DEPTH-1)
			pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep-1].CurDirNum);
		else
		{
			pInfo->DirDeep=MAX_DIR_DEPTH-1;	//�߽�����
			pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep-1].CurDirNum);
		}
	}

	pInfo->DirInfo[pInfo->DirDeep].TotalFile=GetTotalFiles(".", ExtName);	//add by lxs @2005.02.24
	if (pInfo->DirDeep < MAX_DIR_DEPTH-1)
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=GetTotalSubDir(".");		//��ȡ��Ŀ¼�µ���Ŀ¼��
	else
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=0;						//���ΪҶ���
	if (pInfo->DirInfo[pInfo->DirDeep].TotalSubDir == 2 && pInfo->DirDeep>0)	//��Ŀ¼����2����Ŀ¼"."��".."
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=0;
	
	if (pInfo->DirInfo[pInfo->DirDeep].TotalFile == 0)						//��û���ļ��ٲ�����һ��Ŀ¼
		GotoNextDir(Drive, ExtName);
}


/*********************************************************************************************************
** ��������	:
** ��������	:ת��ǰһ��Ŀ¼
** �䡡��	:��չ�������ļ�
** �䡡��	:��
** ȫ�ֱ���  :
** ����ģ��	:��
********************************************************************************************************/
void GotoPrevDir(uint8 Drive, char* ExtName)
{
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (pInfo->DirDeep == 0)				//��ǰ�Ǹ�Ŀ¼Ҫת�����һ��Ŀ¼
		GotoLastDir(Drive, ExtName);
	else
	{
		pInfo->PathClus = ChangeDir(Drive, 2);	//��ȡ��һ��Ŀ¼��ʼ�غ�(..Ŀ¼)
		pInfo->DirDeep--;
		if (pInfo->DirDeep == 0)
		{
			if (pInfo->DirInfo[pInfo->DirDeep].CurDirNum != 1)
			{
				pInfo->DirInfo[pInfo->DirDeep].CurDirNum--;
				pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep].CurDirNum);
				pInfo->DirDeep++;
				GotoLastDir(Drive, ExtName);
			}
			else
				return;				//�Ѿ��ѵ���Ŀ¼��
		}
		else
		{
			if (pInfo->DirInfo[pInfo->DirDeep].CurDirNum != 3)
			{
				pInfo->DirInfo[pInfo->DirDeep].CurDirNum--;
				pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep].CurDirNum);
				pInfo->DirDeep++;
				GotoLastDir(Drive, ExtName);
			}
		}
	}

	pInfo->DirInfo[pInfo->DirDeep].TotalFile=GetTotalFiles(".", ExtName);	//add by lxs @2005.02.24
	if (pInfo->DirDeep < MAX_DIR_DEPTH-1)
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=GetTotalSubDir(".");		//��ȡ��Ŀ¼�µ���Ŀ¼��
	else
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=0;						//���ΪҶ���

	if (pInfo->DirInfo[pInfo->DirDeep].TotalSubDir == 2 && pInfo->DirDeep>0)	//��Ŀ¼����2����Ŀ¼"."��".."
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=0;
	if (pInfo->DirInfo[pInfo->DirDeep].TotalFile == 0)						//��û���ļ��ٲ�����һ��Ŀ¼
		GotoPrevDir(Drive, ExtName);
}


/*********************************************************************************************************
** ��������	:
** ��������	:ת�����һ��Ŀ¼
** �䡡��	:��չ�������ļ�
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
void GotoLastDir(uint8 Drive, char* ExtName)
{
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if(pInfo->DirDeep == (MAX_DIR_DEPTH-1))
	{
		return;
	}
	//go last dir
	while(1)
	{
		pInfo->DirInfo[pInfo->DirDeep].TotalSubDir = GetTotalSubDir(".");
		if(pInfo->DirInfo[pInfo->DirDeep].TotalSubDir==0 || (pInfo->DirInfo[pInfo->DirDeep].TotalSubDir<3 && pInfo->DirDeep>0))
		{
			break;
		}
		else
		{
			pInfo->DirInfo[pInfo->DirDeep].CurDirNum = pInfo->DirInfo[pInfo->DirDeep].TotalSubDir;
			pInfo->PathClus = ChangeDir(Drive, pInfo->DirInfo[pInfo->DirDeep].CurDirNum);
			pInfo->DirDeep++;	
	
			if(pInfo->DirDeep == (MAX_DIR_DEPTH-1))
			{
				break;
			}
		}
	}
	pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=0;
	pInfo->DirInfo[pInfo->DirDeep].TotalFile=GetTotalFiles(".", ExtName);
}


/*********************************************************************************************************
** ��������	:
** ��������	:ת����Ŀ¼
** �䡡��	:��չ�������ļ�
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
void GotoRootDir(uint8 Drive, char* ExtName)
{
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	pInfo->DirDeep=0;						//Ŀ¼���0
	pInfo->PathClus=pInfo->RootClus;	//Ŀ¼�ظ�Ŀ¼
	pInfo->DirInfo[pInfo->DirDeep].TotalSubDir=GetTotalSubDir(".");
	pInfo->DirInfo[pInfo->DirDeep].TotalFile=GetTotalFiles(".", ExtName);
}


/*********************************************************************************************************
** ��������	:MakeDir
** ��������	:����Ŀ¼
** �䡡��	:Path:·��, DirFileName��8.3��ʽĿ¼��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 MakeDir(char *Path, char *DirFileName)
{
	uint8 Rt;
	uint8 i;
	uint32 ClusIndex, Temp1;
	FDT temp;
    uint8 Drive;
    pDRIVE_INFO pInfo;
    
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif

	StrUprCase(Path);
    Drive=GetDrive(Path);
    pInfo=GetDriveInfo(Drive);
	ClusIndex = GetDirClusIndex(Path);
	Rt=PATH_NOT_FIND;
	if (ClusIndex != BAD_CLUS)
	{
		for (i=0; i<11; i++)		//Ŀ¼����ո�
			temp.Name[i]=' ';
		for (i=0; i<11; i++)
		{
			if (*DirFileName == '\0')	//��·������
			{
				break;
			}
			temp.Name[i]=*DirFileName++;
		}

		/* FDT�Ƿ���� */
		Rt = FDTIsLie(Drive, ClusIndex, temp.Name);
		if (Rt == NOT_FIND_FDT)
		{
			/* ������ */
			Temp1 = FATAddClus(Drive, 0);               	/* ��ȡĿ¼������̿ռ� */
            CacheWriteBackAll();
			Rt=DISK_FULL;					/* û�п��пռ� */
			if ((Temp1 > EMPTY_CLUS_1) && (Temp1 < BAD_CLUS))
			{
				ClearClus(Drive, Temp1);            		/* ��մ� */
			        /* ����FDT���� */
				temp.Attr = ATTR_DIRECTORY;
				temp.FileSize = 0;
				temp.NTRes = 0;
				temp.CrtTimeTenth = 0;
				temp.CrtTime = 0;
				temp.CrtDate = 0;
				temp.LstAccDate = 0;
				temp.WrtTime = 0;
				temp.WrtDate = 0;
				temp.FstClusLO = Temp1 & 0xffff;
				temp.FstClusHI = Temp1 / 0x10000;
				Rt = AddFDT(Drive, ClusIndex, &temp);       /* ����Ŀ¼�� */
				if (Rt == RETURN_OK)
				{
					/* ����'.'Ŀ¼ */
					temp.Name[0] = '.';
					for (i=1; i < 11; i++)
					{
						temp.Name[i] = ' ';
					}
					AddFDT(Drive, Temp1, &temp);

					/* ����'..'Ŀ¼ */
					temp.Name[1] = '.';
					if (ClusIndex == pInfo->RootClus)
						ClusIndex=0;
					temp.FstClusLO = ClusIndex & 0xffff;
					temp.FstClusHI = ClusIndex / 0x10000;
					Rt = AddFDT(Drive, Temp1, &temp);
				}
				else
				{
					FATDelClusChain(Drive, Temp1);
				}
				Rt=RETURN_OK;
			}
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:
** ��������	:�ı�Ŀ¼��ת����ǰĿ¼�µ���Ŀ¼����
** �䡡��	:SubDirIndex
** �䡡��	:ָ����Ŀ¼�����Ĵغ�
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint32 ChangeDir(uint8 Drive, uint16 SubDirIndex)
{
	FDT Rt;
	uint32 cluster;
	uint32 index;
	uint8 *buf;
	uint8 offset;
	uint8 i;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	index=0;
	cluster=0;
	while (1)
	{
		if (RETURN_OK != GetFDTInfo(&Rt, Drive, pInfo->PathClus, index++))
			break;

		if (Rt.Name[0] == 0x00)			//�ҵ��յ���Ϊ����
			break;
		
		if (Rt.Name[0] == FILE_DELETED)	//��ɾ��
			continue;
		
		for (i=0; i<MAX_FILENAME_LEN; i++)
			pInfo->DirInfo[pInfo->DirDeep].LongDirName[i]='\0';

		while (Rt.Attr==ATTR_LFN_ENTRY)			//���ļ�����Ҫ�ҵ����ļ���
		{
			buf=(uint8 *)&Rt;
			offset=13 * ((buf[0] & LFN_SEQ_MASK) - 1);
			if ((buf[0] & LFN_SEQ_MASK) <= MAX_LFN_ENTRIES)   
			{/* ���ļ������Ŀ¼����*/
				for (i = 0; i < 10; i++)
					pInfo->DirInfo[pInfo->DirDeep].LongDirName[i/2+offset] |= buf[i+1] << (i % 2)*8;
				for (i = 0; i < 6; i++)
					pInfo->DirInfo[pInfo->DirDeep].LongDirName[i+5+offset]=buf[i+14];
				for (i = 0; i < 2; i++)
					pInfo->DirInfo[pInfo->DirDeep].LongDirName[i+11+offset]=buf[i+21];
			}
			
			GetFDTInfo(&Rt, Drive, pInfo->PathClus, index++);
		}
		if ((Rt.Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
		{
			if (--SubDirIndex == 0)
			{
				cluster=Rt.FstClusHI;
				cluster <<= 16;
				cluster |= Rt.FstClusLO;
				for (i=0; i<11; i++)
				{
					pInfo->DirInfo[pInfo->DirDeep].DirName[i]=Rt.Name[i];
				}
				if (pInfo->DirInfo[pInfo->DirDeep].LongDirName[0] == '\0')
				{
					for (i=0; i<8; i++)
					{
						pInfo->DirInfo[pInfo->DirDeep].LongDirName[i]=Rt.Name[i];
						if (pInfo->DirInfo[pInfo->DirDeep].LongDirName[i] == ' ')
							break;
					}
					if (Rt.Name[8] != ' ')
					{
						pInfo->DirInfo[pInfo->DirDeep].LongDirName[i++] = '.';				//׷����չ��
						pInfo->DirInfo[pInfo->DirDeep].LongDirName[i++] = Rt.Name[8];
						pInfo->DirInfo[pInfo->DirDeep].LongDirName[i++] = Rt.Name[9];
						pInfo->DirInfo[pInfo->DirDeep].LongDirName[i++] = Rt.Name[10];
					}
					pInfo->DirInfo[pInfo->DirDeep].LongDirName[i] = '\0';
				}
				break;
			}
		}
	}
	return (cluster);
}


/*********************************************************************************************************
** ��������	:GetTotalSubDir
** ��������	:��ȡ����Ŀ¼��
** �䡡��	:·��Path
** �䡡��	:��Ŀ¼��
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint16 GetTotalSubDir(char *Path)
{
	FDT Rt;
	uint32 index;
	uint32 DirClus;
	uint16 TotSubDir=0;

	DirClus=GetDirClusIndex(Path);
	if (DirClus == BAD_CLUS)
		return (0);
	
	for (index=0; ; index++)
	{
		if (RETURN_OK != GetFDTInfo(&Rt, GetDrive(Path), DirClus, index))
			break;
		
		if (Rt.Name[0] == FILE_NOT_EXIST)	//�ҵ��յ���Ϊ����
			break;
		
		if (Rt.Name[0] == FILE_DELETED)	//��ɾ��
			continue;
		
		if ((Rt.Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
			TotSubDir++;
	}
	return (TotSubDir);
}
#endif

/*********************************************************************************************************
** ��������	: GetDirClusIndex
** ��������	: ��ȡָ��Ŀ¼��ʼ�غ�
** �䡡��	: Path:·����(�������ļ���)
** �䡡��	: ��ʼ�غ�
** ȫ�ֱ���	: 
** ����ģ��	: 
** ˵	 ��		: ����·��������'\'����, ֧��1~11��·����
********************************************************************************************************/
uint32 GetDirClusIndex(char *Path)
{
	uint8 i;
	uint32 DirClusIndex;
	FDT temp;
	char PathName[12];
    uint8 Drive;
    pDRIVE_INFO pInfo;

	DirClusIndex = BAD_CLUS;
	if (Path != NULL)		//null pointer
	{
//***********************************************************************
//֧���̷���A:
//***********************************************************************
		StrUprCase(Path);
        Drive=GetDrive(Path);
        pInfo=GetDriveInfo(Drive);
		if (Path[1] == ':')
		{
			Path += 2;
		}
		
		DirClusIndex = pInfo->RootClus;	//��Ŀ¼
//***********************************************************************
//A:TEMP��TEMP��.\TEMP����ָ��ǰĿ¼�µ�TEMP��Ŀ¼
//***********************************************************************
		if (Path[0] != '\\')			//* ����Ŀ¼�ָ�����,��������ǵ�ǰ·��
		{
			DirClusIndex = pInfo->PathClus;
		}
		else
		{
			Path++;
		}
		
		if (Path[0] == '.')             // '\.'��������ǵ�ǰ·��
		{
			DirClusIndex = pInfo->PathClus;
			if (Path[1] == '\0' || Path[1] == '\\')		//case "." or ".\"
			{
				Path++;
			}
		}
#if 0	//remove by lxs for filename include '\'
		if (Path[0] == '\\')
		{
			Path++;
		}
#endif		
//***********************************************************************
//***********************************************************************
		while (Path[0] != '\0')
		{
			if (Path[0] == ' ')			//�׸��ַ�������Ϊ�ո�
			{
				DirClusIndex = BAD_CLUS;
				break;
			}

			for (i=0; i<11; i++)		//Ŀ¼����ո�
				PathName[i]=' ';
			for (i=0; i<12; i++)
			{
				#if 0	//remove by lxs for filename include '\'
				if (*Path == '\\')		//��Ŀ¼�ָ���
				{
					Path++;
					break;
				}
				#endif
				if (*Path == '\0')		//��·������
				{
					break;
				}
				PathName[i]=*Path++;
			}
			
			if (FindFDTInfo(&temp, Drive, DirClusIndex, PathName) != RETURN_OK)	//��ȡFDT��Ϣ
			{
				DirClusIndex = BAD_CLUS;
				break;
			}

			if ((temp.Attr & ATTR_DIRECTORY) == 0)	//FDT�Ƿ���Ŀ¼
			{
				DirClusIndex = BAD_CLUS;
				break;
			}
			
			DirClusIndex = ((uint32)(temp.FstClusHI) << 16) + temp.FstClusLO;
		}
	}
	return (DirClusIndex);
}


/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

