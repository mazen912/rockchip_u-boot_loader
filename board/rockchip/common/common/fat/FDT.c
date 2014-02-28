/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    FDT.C
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define     IN_FDT

#include    "../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�


/*********************************************************************************************************
** ��������	:
** ��������	:��ȡFDT��Ϣ
** �䡡��	:Rt���洢������Ϣ��ָ��
**        	 SecIndex��������
**         	 ByteIndex��ƫ����
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:ReadSecs
********************************************************************************************************/
uint8 ReadFDTInfo(FDT *Rt, uint8 Drive, uint32 SecIndex, uint16 ByteIndex)
{
	uint8 status;
    pCACHE cache;

    status=NOT_FIND_FDT;
    if (OK == ReadCache(Drive, SecIndex, &cache, CACHE_FDT))
	{
        ftl_memcpy((uint8*)Rt, cache->Buf+ByteIndex, sizeof(FDT));
	    status=RETURN_OK;
	}
	return (status);
}
#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:
** ��������	:дFDT��Ϣ
** �䡡��	:SecIndex��������
**        	 ByteIndex��ƫ����
**        	 FDT *FDTData:����
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:ReadSecs
********************************************************************************************************/
uint8 WriteFDTInfo(FDT *FDTData, uint8 Drive, uint32 SecIndex, uint16 ByteIndex)
{
	uint8 status;
    pCACHE cache;

    ReadCache(Drive, SecIndex, &cache, CACHE_FDT);
    cache->Flag=1;
    cache->Drive=Drive;
    ftl_memcpy(cache->Buf+ByteIndex, (uint8*)FDTData, sizeof(FDT));
    status=NOT_FIND_FDT;
    if (OK == CacheWriteBack(cache))
    {
        status=RETURN_OK;
    }
    return (status);
}
#endif

/*********************************************************************************************************
** ��������	:
** ��������	:��ȡ��Ŀ¼ָ���ļ�(Ŀ¼)��Ϣ
** �䡡��	:Rt���洢������Ϣ��ָ��
**        	 Index���ļ�(Ŀ¼)��FDT�е�λ��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 GetRootFDTInfo(FDT *Rt, uint8 Drive, uint32 Index)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8 temp;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
	temp = NOT_FAT_DISK;
	Index = Index << 5;
	if (pInfo->FATType == FAT12 || pInfo->FATType == FAT16)
	{
		temp = FDT_OVER;
		if (Index < (pInfo->RootDirSectors << 9))
		{
			ByteIndex = Index & (pInfo->BytsPerSec-1);
          	SecIndex = (Index >> 9)+(pInfo->FirstDataSector-pInfo->RootDirSectors);
          	temp = ReadFDTInfo(Rt, Drive, SecIndex, ByteIndex);
    	}
	}
	return (temp);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ȡָ��Ŀ¼ָ���ļ�(Ŀ¼)��Ϣ
** �䡡��	:Rt���洢������Ϣ��ָ��
**        	 ClusIndex��Ŀ¼�״غ�
**        	 Index���ļ�(Ŀ¼)��FDT�е�λ��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 GetFDTInfo(FDT *Rt, uint8 Drive, uint32 ClusIndex, uint32 Index)
{
	uint16 ByteIndex;
	uint16 ClusCnt;
	uint32 SecIndex, i;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (ClusIndex == EMPTY_CLUS)
	{
		if (pInfo->FATType == FAT32)
		{
			ClusIndex = pInfo->RootClus;
		}
		else
		{
			return (GetRootFDTInfo(Rt, Drive, Index));
		}
	}

	if (pInfo->FATType == FAT12 || pInfo->FATType == FAT16 || pInfo->FATType == FAT32)
	{
		if (ClusIndex != pInfo->FdtRef.DirClus)
		{
			pInfo->FdtRef.DirClus=ClusIndex;
			pInfo->FdtRef.CurClus=ClusIndex;
			pInfo->FdtRef.Cnt=0;
		}
		Index = Index << 5;
		ByteIndex = Index & (pInfo->BytsPerSec-1);
		SecIndex=Index >> 9;
		ClusCnt = SecIndex >> pInfo->LogSecPerClus;
		if (ClusCnt < pInfo->FdtRef.Cnt)
		{
			pInfo->FdtRef.Cnt=0;
			pInfo->FdtRef.CurClus=ClusIndex;
		}
		else
		{
			SecIndex-=pInfo->FdtRef.Cnt << pInfo->LogSecPerClus;
		}
		/* ����Ŀ¼���������� */
		i = pInfo->SecPerClus;
		while(SecIndex >= i)
		{
			pInfo->FdtRef.CurClus = FATGetNextClus(Drive, pInfo->FdtRef.CurClus, 1);
			pInfo->FdtRef.Cnt++;
			if (pInfo->FdtRef.CurClus <= EMPTY_CLUS_1 || pInfo->FdtRef.CurClus >= BAD_CLUS) 
			{
				return (FDT_OVER);
			}
			SecIndex -= i;
		}
		SecIndex = ((pInfo->FdtRef.CurClus - 2) << pInfo->LogSecPerClus) + SecIndex + pInfo->FirstDataSector;
		return (ReadFDTInfo(Rt, Drive, SecIndex, ByteIndex));
	}
	return (NOT_FAT_DISK);
}

#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:SetRootFDTInfo
** ��������	:���ø�Ŀ¼ָ���ļ�(Ŀ¼)��Ϣ
** �䡡��	:FDTData��Ҫд�����Ϣ
**        	 Index���ļ�(Ŀ¼)��FDT�е�λ��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 SetRootFDTInfo(uint8 Drive, uint32 Index, FDT *FDTData)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8 Rt;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
	Rt = NOT_FIND_DISK;
	Index = Index << 5;
	if (pInfo->FATType == FAT12 || pInfo->FATType == FAT16)
	{
		Rt = FDT_OVER;
		if (Index < (pInfo->RootDirSectors << 9))
		{
			ByteIndex = Index & (pInfo->BytsPerSec-1);
			SecIndex = (Index >> 9) + (pInfo->FirstDataSector-pInfo->RootDirSectors);
			Rt = WriteFDTInfo(FDTData, Drive, SecIndex, ByteIndex);
		}
	}
	return (Rt);
}
#endif
/*********************************************************************************************************
** ��������	:
** ��������	:��ָ��Ŀ¼����ָ���ļ�(Ŀ¼)
** �䡡��	:Rt���洢������Ϣ��ָ��
**        			 ClusIndex��Ŀ¼�״غ�
**        			 FileName���ļ�(Ŀ¼)��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:
********************************************************************************************************/
uint8 FindFDTInfo(FDT *Rt, uint8 Drive, uint32 ClusIndex, char *FileName)
{
	uint32 i;
	uint8 temp, j;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
	while (1)
	{
		temp = GetFDTInfo(Rt, Drive, ClusIndex, i);		//����RETURN_OK\NOT_FAT_DISK\FDT_OVER
		if (temp != RETURN_OK)
		{
			break;
		}
		if (Rt->Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
		if ((Rt->Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j=0; j<11; j++)
				if (FileName[j] != Rt->Name[j])
					break;
			if (j==11)
			{
				temp = RETURN_OK;
				break;
			}
		}
		i++;
	}
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
	return (temp);
}


#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:
** ��������	:����ָ��Ŀ¼ָ���ļ�(Ŀ¼)��Ϣ
** �䡡��	:FDTData��Ҫд�����Ϣ
**        	 ClusIndex��Ŀ¼�״غ�
**        	 Index���ļ�(Ŀ¼)��FDT�е�λ��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 SetFDTInfo(uint8 Drive, uint32 ClusIndex, uint32 Index, FDT *FDTData)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8 i;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
	if (ClusIndex == EMPTY_CLUS)
	{
		if (pInfo->FATType == FAT32)
		{
			ClusIndex = pInfo->RootClus;
		}
		else
		{
			return (SetRootFDTInfo(Drive, Index, FDTData));
		}
	}

	if (pInfo->FATType == FAT12 || pInfo->FATType == FAT16 || pInfo->FATType == FAT32)
	{
		Index = Index << 5;
		ByteIndex = Index & (pInfo->BytsPerSec-1);
		SecIndex = Index >> 9;	/* ����Ŀ¼������ƫ������ */
		i = pInfo->SecPerClus;
		while(SecIndex >= i)
		{
			ClusIndex = FATGetNextClus(Drive, ClusIndex, 1);
			if (ClusIndex <= EMPTY_CLUS_1 ||ClusIndex >= BAD_CLUS) 
			{
				return (FDT_OVER);
			}
			SecIndex -= i;
		}
		SecIndex = ((ClusIndex - 2) << pInfo->LogSecPerClus) + SecIndex + pInfo->FirstDataSector;
		return (WriteFDTInfo(FDTData, Drive, SecIndex, ByteIndex));
	}
	return (NOT_FAT_DISK);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ָ��Ŀ¼������ָ���ļ�(Ŀ¼)
** �䡡��	:ClusIndex��Ŀ¼�״غ�
**        	 FDTData���ļ�(Ŀ¼)��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 AddFDT(uint8 Drive, uint32 ClusIndex, FDT *FDTData)
{
	uint32 i;
	FDT TempFDT;
	uint8 temp;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (ClusIndex == EMPTY_CLUS)
	{
		if (pInfo->FATType == FAT32)
		{
			ClusIndex = pInfo->RootClus;
		}
	}

	temp = FindFDTInfo(&TempFDT, Drive, ClusIndex, FDTData->Name);		//NOT_FIND_FDT\RETURN_OK
	if (temp == RETURN_OK)
	{
		return (FDT_EXISTS);
	}

	if (temp != NOT_FIND_FDT && temp != FDT_OVER)		//NOT_FAT_DISK
	{
		return (temp);
	}

	if (FDTData->Name[0] == FILE_DELETED)
	{
		FDTData->Name[0] = ESC_FDT;
	}

	i = 0;
	temp = RETURN_OK;
	while (temp == RETURN_OK)
	{
		temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
		if (temp == RETURN_OK)
		{
			if (TempFDT.Name[0] == FILE_DELETED || TempFDT.Name[0] == FILE_NOT_EXIST)
			{
				temp = SetFDTInfo(Drive, ClusIndex, i, FDTData);
				break;
			}
		}
		i++;
	}
	
	if (temp == FDT_OVER && ClusIndex != EMPTY_CLUS)	//��ǰĿ¼�������,������һ����
	{
		i = FATAddClus(Drive, ClusIndex);
        CacheWriteBackAll();
		temp = DISK_FULL;
		if (i != BAD_CLUS)
		{
			ClearClus(Drive, i);
			temp = SetFDTInfo(Drive, i, 0, FDTData);
		}
	}
	
	if (FDTData->Name[0] == ESC_FDT)
	{
		FDTData->Name[0] = FILE_DELETED;
	}
	return (temp);
}



/*********************************************************************************************************
** ��������	:
** ��������	:��ָ��Ŀ¼ɾ��ָ���ļ�(Ŀ¼)
** �䡡��	:ClusIndex��Ŀ¼�״غ�
**        	 FileName���ļ�(Ŀ¼)��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:
********************************************************************************************************/
uint8 DelFDT(uint8 Drive, uint32 ClusIndex, char *FileName)
{
	uint32 i;
	FDT TempFDT;
	uint8 temp, j;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
	while (1)
	{
		temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
		if (temp != RETURN_OK)
		{
			break;
		}
            
		if (TempFDT.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
//		if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)		//��겻��ɾ��
		{
			for (j=0; j<11; j++)
				if (FileName[j] != TempFDT.Name[j])
					break;
			if (j==11)
			{
                //ɾ������Ŀ¼��
				do
				{
					TempFDT.Name[0] = FILE_DELETED;
					temp = SetFDTInfo(Drive, ClusIndex, i, &TempFDT);
					if (RETURN_OK != GetFDTInfo(&TempFDT, Drive, ClusIndex, --i))
						break;
				}while (TempFDT.Attr==ATTR_LFN_ENTRY);			//���ļ�����Ҫ�ҵ����ļ���
				break;
			}
		}
		i++;
	}
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
	return (temp);
}

/*********************************************************************************************************
** ��������	:
** ��������	:�ı�ָ��Ŀ¼ָ���ļ���Ŀ¼��������
** �䡡��	:ClusIndex��Ŀ¼�״غ�
**        	 FileName���ļ���Ŀ¼����
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:
********************************************************************************************************/
uint8 ChangeFDT(uint8 Drive, uint32 ClusIndex, FDT *FDTData)
{
	uint32 i;
	uint8 temp, j;
	FDT TempFDT;

	i = 0;
	if (FDTData->Name[0] == FILE_DELETED)
	{
		FDTData->Name[0] = ESC_FDT;
	}
	while (1)
	{
		temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
		if (temp != RETURN_OK)
		{
			break;
		}
            
		if (TempFDT.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
		if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j=0; j<11; j++)
				if (FDTData->Name[j] != TempFDT.Name[j])
					break;
			if (j==11)
			{
				temp = SetFDTInfo(Drive, ClusIndex, i, FDTData);
				break;
			}
		}
		i++;
	}
	if (FDTData->Name[0] == ESC_FDT)
	{
		FDTData->Name[0] = FILE_DELETED;
	}
	return (temp);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ָ��Ŀ¼�鿴ָ���ļ�(Ŀ¼)�Ƿ����
** �䡡��	:ClusIndex��Ŀ¼�״غ�
**        	 FileName���ļ�(Ŀ¼)��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:��
** ����ģ��	:
********************************************************************************************************/
uint8 FDTIsLie(uint8 Drive, uint32 ClusIndex, char *FileName)
{
	uint32 i;
	uint8 temp, j;
	FDT temp1;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
	while (1)
	{
		temp = GetFDTInfo(&temp1, Drive, ClusIndex, i);
		if (temp == FDT_OVER)
		{
			temp = NOT_FIND_FDT;
			break;
		}

		if (temp != RETURN_OK)
		{
			break;
		}

		if (temp1.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
		
		if ((temp1.Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j=0; j<11; j++)
				if (FileName[j] != temp1.Name[j])
					break;
			if (j==11)
			{
				temp = FDT_EXISTS;
				break;
			}
		}
		i++;
	}
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
	return (temp);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ָ����������������
** �䡡��	:Path:·��
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 ClearClus(uint8 Drive, uint32 Index)
{
    uint8 buf[512];
    uint32 i;
	uint32 SecIndex;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (Index < (pInfo->TotClus+2))
	{
        memset(buf, 0x00, 512);
		SecIndex = ((Index - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector;
        for (i=0; i<pInfo->SecPerClus; i++)
            WriteSecs(Drive, SecIndex+i, buf, 1);
		return (RETURN_OK);
	}
	else
	{
		return (CLUSTER_NOT_IN_DISK);
	}
}
#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

