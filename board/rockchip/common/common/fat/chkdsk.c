/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    chkdsk.c
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define	    IN_CHKDSK
#include    "../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�
#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:FAT32UpdateBit
** ��������	:FAT32���´�����Ϣ
** �䡡��	:
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵     ��		:
********************************************************************************************************/
uint16 FAT32UpdateBit(uint8 Drive, uint32 LBA, uint8 ByteOffset, uint16 BitOffset, uint8 Bittype)
{
#if 1
	uint16 i, selection, Counter;

	//search matching sector
	for(i=0; i<MAX_CACHES; i++)
	{
		if (LBA == CacheChkdsk[i].Sector)
			break;
	}
	if (i >= MAX_CACHES)
	{
		// Cache Miss, so must read. Now find the Least recently used Buffer
		selection=0;
		Counter=0;
		for (i=0; i<MAX_CACHES; i++)
		{
			if (CacheChkdsk[i].Valid == 1)
			{
				if (CacheChkdsk[i].Cnt > Counter)
				{
					selection = i;
					Counter = CacheChkdsk[i].Cnt;
				}
			}
			else
			{
				selection = i;
				CacheChkdsk[selection].Valid = 1;
				break;
			}
		}

		if (CacheChkdsk[selection].Write == 1)
		{
            WriteSecs(Drive, CacheChkdsk[selection].Sector, CacheChkdsk[selection].buf, 1);
		}
		ReadSecs(Drive, LBA, CacheChkdsk[selection].buf, 1);
		CacheChkdsk[selection].Sector = LBA;

		//increment cache counter
		for(i=0; i<MAX_CACHES; i++)
			CacheChkdsk[i].Cnt++;

		i=selection;
	}

	switch(Bittype)
	{
		case GET_BIT:
    		CacheChkdsk[i].Cnt = READCOUNTER;
			return (CacheChkdsk[i].buf[ByteOffset] & BitMask[BitOffset]);
		case SET_BIT:
    		CacheChkdsk[i].Cnt = WRITECOUNTER;
			CacheChkdsk[i].Write = 1;
			selection = CacheChkdsk[i].buf[ByteOffset] & BitMask[BitOffset];
			if (selection==1)
			{
				FileCorrupted=0;
				return (1);
			}
       		CacheChkdsk[i].buf[ByteOffset] |= BitMask[BitOffset];                
			return (selection);
		case FREE_BIT:
    		CacheChkdsk[i].Cnt = WRITECOUNTER;
			CacheChkdsk[i].Write = 1;
			CacheChkdsk[i].buf[ByteOffset] &= ~BitMask[BitOffset];
		default:
			return (0);
	}
#endif	
}




/*********************************************************************************************************
** ��������	:UpdateBit
** ��������	:���´���
** �䡡��	:
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵     ��		:
********************************************************************************************************/
uint16 UpdateBit(uint8 Drive, uint32 wBitNumber, uint8 *pwBuffer, uint8 Bittype)
{
	uint8 ByteOffset;
	uint16 woffset;
	uint16 BitOffset;
	uint32 sector;
	uint16 OldBit;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);


	if (wBitNumber >= pInfo->TotClus+2)
	{
		return (0);
	}
	if(pInfo->FATType == FAT32)
	{   
		sector=wBitNumber >> 12;
		woffset=wBitNumber & 0x0fff;
		ByteOffset=woffset >> 3;
		BitOffset=woffset -(ByteOffset << 3);
		sector += pInfo->FirstDataSector - pInfo->FATSz;
		return (FAT32UpdateBit(Drive, sector,ByteOffset,BitOffset,Bittype));
	}          
	else
	{       
		ByteOffset = wBitNumber >> 3;
		BitOffset = wBitNumber - (ByteOffset << 3);

		switch(Bittype)
		{
			case GET_BIT:
				return (pwBuffer[ByteOffset] & BitMask[BitOffset]);
			case SET_BIT:
				OldBit = pwBuffer[ByteOffset] & BitMask[BitOffset];
				pwBuffer[ByteOffset] |= BitMask[BitOffset];
				return (OldBit);
			case FREE_BIT:
				pwBuffer[ByteOffset] &= ~BitMask[BitOffset];
			default:
 				return (0);
 		}	
 	 }							
}                                            


/*********************************************************************************************************
** ��������	:CheckCrossLinkFile
** ��������	:ɨ��Ŀ¼�Ƿ���ȷ
** �䡡��	:Ŀ¼���ڴغ�
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵     ��		:
********************************************************************************************************/
uint32 CheckCrossLinkFile(uint8 Drive, uint32 wStartCluster)
{
	uint32 wCluster = wStartCluster;
	uint32 wClusterCount = 1;
	uint32 i;
    
	// This handles case of a 0 byte file
	if (wCluster == 0)
		return(0);
	if (FATGetNextClus(Drive, wStartCluster, 1) == EMPTY_CLUS)
		return(0);
		
	do
	{
		if (UpdateBit(Drive, wCluster, FATClusMap, SET_BIT))
		{
			wCluster = wStartCluster;
			for (i=0; i<(wClusterCount - 1); i++)
			{
				UpdateBit(Drive, wCluster, FATClusMap, FREE_BIT);
				wCluster = FATGetNextClus(Drive, wCluster, 1);
			}
			return (0);
		}
        
		wCluster = FATGetNextClus(Drive, wCluster, 1);
		if (wCluster <= 1)
		{
			wCluster = wStartCluster;
			for (i=0; i<wClusterCount; i++)
			{
				UpdateBit(Drive, wCluster, FATClusMap, FREE_BIT);
				wCluster = FATGetNextClus(Drive, wCluster, 1);
			}
			return (0);
		 }  
        
		if (wCluster == EOF_CLUS_END)
			return (wClusterCount);
            
		wClusterCount++;
	}while (wClusterCount < 0x100000);     // We should get out of this loop

	return (wClusterCount);
}


/*********************************************************************************************************
** ��������	:ScanDirectory
** ��������	:ɨ��Ŀ¼�Ƿ���ȷ
** �䡡��	:Ŀ¼���ڴغ�
** �䡡��	:Ŀ¼������
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵     ��		:
********************************************************************************************************/
uint32 ScanDirectory(uint8 Drive, uint32 clus)
{
	uint32 count;
	FDT tmp;
	
	GetFDTInfo(&tmp, Drive, clus, 0);
	if (tmp.Name[0] != '.')						//'.'Ŀ¼��
		return (0);
	if ((tmp.Attr & ATTR_DIRECTORY)  == 0)		//Ŀ¼����
		return (0);
	if (tmp.FileSize != 0)						//�ļ���С��Ҫ=0
		return (0);

	GetFDTInfo(&tmp, Drive, clus, 1);
	if (tmp.Name[0] != '.' || tmp.Name[1] != '.')	//'..'Ŀ¼��
		return (0);
	if ((tmp.Attr & ATTR_DIRECTORY)  == 0)		//Ŀ¼����
		return (0);
	if (tmp.FileSize != 0)						//�ļ���С��Ҫ=0
		return (0);


	count=CheckCrossLinkFile(Drive, clus);
	return (count);
}
          

/*********************************************************************************************************
** ��������	:ScanFilesAndSubDirs
** ��������	:ɨ��Ŀ¼�µ��ļ����ļ���
** �䡡��	:
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵     ��		:
********************************************************************************************************/
bool ScanFilesAndSubDirs(uint8 Drive, uint32 DirClus)
{
	uint16 i;
	uint16 index;
	uint32 TmpClus;
	uint32 FstClus;
	uint32 wClusterCount;
	uint32 dwFileSizeDiskBytes;
	FDT tmp;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

   	if(Nestinglevel >= MAX_NESTING_LEVEL)
   	{
   		Nestinglevel--;
	 	return(OK);
	}
	Nestinglevel++;
	index=0;
	while (1)
	{
		if (RETURN_OK!=GetFDTInfo(&tmp, Drive, DirClus, index++))
		{
			Nestinglevel--;
			return (OK);
		}
		
		if (tmp.Name[0]==FILE_NOT_EXIST)				//��Ŀ¼��,���治�����ļ�
		{
			Nestinglevel--;
			wClusterCount=((index-1) >> 4) >> pInfo->LogSecPerClus;
			wClusterCount++;
			if (CacheChkdsk[Nestinglevel].DirClusCnt > wClusterCount)
			{
				TmpClus = DirClus;
				for (index=0; index<CacheChkdsk[Nestinglevel].DirClusCnt; index++)
				{
					UpdateBit(Drive, TmpClus, FATClusMap, FREE_BIT);
					TmpClus=FATGetNextClus(Drive, TmpClus, 1);
				}
				return (ERROR);
			}
			return (OK);
		}
		
		if (tmp.Name[0]==FILE_DELETED || tmp.Name[0]==0x2e)
			continue;
		
		if (tmp.Attr==ATTR_LFN_ENTRY && tmp.FstClusLO==0)
			continue;
		
		FstClus=((uint32)tmp.FstClusHI << 16) | tmp.FstClusLO;
		if ((tmp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0x00)
		{
			if ((wClusterCount=CheckCrossLinkFile(Drive, FstClus)) == 0)
			{
				FileCorrupted++;
				DelFDT(Drive, DirClus, tmp.Name);	//��������ɾ��
				continue;
        	}
			else
			{
				dwFileSizeDiskBytes=wClusterCount << (pInfo->LogSecPerClus+9);
				if (dwFileSizeDiskBytes != tmp.FileSize)
				{
					if ((dwFileSizeDiskBytes < tmp.FileSize) || (dwFileSizeDiskBytes-(1 << (pInfo->LogSecPerClus+9)) > tmp.FileSize))
					{
						TmpClus = FstClus;
						for(i=0; i<wClusterCount; i++)
						{
							UpdateBit(Drive, TmpClus, FATClusMap, FREE_BIT);
							TmpClus = FATGetNextClus(Drive, TmpClus, 1);
						}
						FileCorrupted++;
						DelFDT(Drive, DirClus, tmp.Name);		//�ļ���С����ɾ��
						continue;
			        }
				}
			}
		}
		else if ((tmp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY)
		{
			CacheChkdsk[Nestinglevel].DirClusCnt=ScanDirectory(Drive, FstClus);
			if(CacheChkdsk[Nestinglevel].DirClusCnt == 0)
			{
				FileCorrupted++;
				DelFDT(Drive, DirClus, tmp.Name);	//Ŀ¼��ʽ����ȷɾ��
				continue;
	        }
			if (OK != ScanFilesAndSubDirs(Drive, FstClus))
			{
				FileCorrupted++;
				DelFDT(Drive, DirClus, tmp.Name);	//Ŀ¼��ʽ����ȷɾ��
//				UpdateBit(Drive, DirClus, FATClusMap, FREE_BIT);
				continue;
	        }
		}
		else if ((tmp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID)
		{
		}
		else
		{
			FileCorrupted++;
			DelFDT(Drive, DirClus, tmp.Name);		//��ЧĿ¼��ɾ��
			continue;
		}
	}
} 
    


/*********************************************************************************************************
** ��������	:ScanAndUpdateFat
** ��������	:�ͷ���Ч�Ĵ�����
** �䡡��	:
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵   ��	:
********************************************************************************************************/
bool ScanAndUpdateFat(uint8 Drive)
{
	uint32 ThisClus;
	uint32 FATSecNum, pre_FATSecNum;
	uint16 FATEntOffset;
	uint8 i;
	uint8 Buf[512];
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);


	if (pInfo->FATType == FAT32)
	{
		for(i=0; i<MAX_CACHES; i++)
		{
			if (CacheChkdsk[i].Write == 1)
			{
                WriteSecs(Drive, CacheChkdsk[i].Sector, CacheChkdsk[i].buf, 1);
			}
			CacheChkdsk[i].Write=0;
		}
	}
	pre_FATSecNum=0;
	for (ThisClus=2 ; ThisClus<pInfo->TotClus; ThisClus++)
	{
		GetFATPosition(Drive, ThisClus, &FATSecNum, &FATEntOffset);		//��ȡ�ô���FAT���ƫ��
		if (pre_FATSecNum != FATSecNum)
		{
			if (pre_FATSecNum != 0)
			{
                WriteSecs(Drive, pre_FATSecNum, Buf, 1);
			}
			pre_FATSecNum = FATSecNum;
			ReadSecs(Drive, FATSecNum, Buf, 1);
		}
		if (UpdateBit(Drive, ThisClus, FATClusMap, GET_BIT)==0)
		{
			switch (pInfo->FATType)		//���ô���
			{
				case FAT16:
					Buf[FATEntOffset+0] = EMPTY_CLUS;
					Buf[FATEntOffset+1] = EMPTY_CLUS;
					break;
				case FAT32:
					Buf[FATEntOffset+0] = EMPTY_CLUS;
					Buf[FATEntOffset+1] = EMPTY_CLUS;
					Buf[FATEntOffset+2] = EMPTY_CLUS;
					Buf[FATEntOffset+3] = EMPTY_CLUS;
					break;
				default:
					break;
			}
		}
	}
    WriteSecs(Drive, pre_FATSecNum, Buf, 1);
	return(OK);
}
    


/*********************************************************************************************************
** ��������	:Chkdsk
** ��������	:���ָ��������
** �䡡��	:
** �䡡��	:OK,ERROR
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵    ��	:
********************************************************************************************************/
uint16 Chkdsk(uint8 Drive)
{
    uint8 buf[512];
	uint16 i;
	uint32 Fat2Sec;
	uint32 TotalFatsectors;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

    if (pInfo->Valid == 0)
        return (ERROR);
    Nestinglevel=0;
	FileCorrupted=0;
    memset(FATClusMap, 0x00, sizeof(FATClusMap));
	CacheChkdsk[0].DirClusCnt=1;
	if(pInfo->FATType == FAT32) 
	{
		CacheChkdsk[0].DirClusCnt=0;
		for(i=0; i<MAX_CACHES; i++)
		{
			CacheChkdsk[i].buf=&FATClusMap[i << 9];
	        CacheChkdsk[i].Valid=0;
	        CacheChkdsk[i].Write=0;
		}
		TotalFatsectors=pInfo->TotClus >> 12;
		TotalFatsectors++;
		Fat2Sec=pInfo->FirstDataSector - pInfo->FATSz;
        memset(buf, 0x00, 512);
        for (i=0; i<TotalFatsectors; i++)
        {
            WriteSecs(Drive, Fat2Sec+i, buf, 1);
        }
		Fat2Sec=pInfo->RootClus;
		do
		{
			CacheChkdsk[0].DirClusCnt++;
			UpdateBit(Drive, Fat2Sec, FATClusMap, SET_BIT);
			Fat2Sec = FATGetNextClus(Drive, Fat2Sec, 1);
			if (Fat2Sec == EOF_CLUS_END)
				break;
		}while(1);
	}
   
#if 0
	if (FDTIsLie(pInfo->RootClus, "RECYCLED   ") != NOT_FIND_FDT)
	{
		FileCorrupted++;
		DelFDT(Drive, pInfo->RootClus, "RECYCLED   ");	//Ŀ¼��ʽ����ȷɾ��
	}
#endif

	if(ScanFilesAndSubDirs(Drive, pInfo->RootClus) != OK)
	{
		//return (ERROR);
	}

	if (FileCorrupted != 0)
	{
		ScanAndUpdateFat(Drive);
	}
    //FlashCacheCloseAll();
	return (FileCorrupted);
}    
#endif





