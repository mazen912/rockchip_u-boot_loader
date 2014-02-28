/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    fat.C
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define	    IN_FAT
#include	"../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�

#ifndef BOOT_ONLY
/***************************************************************************
��������:�߼���ʽ��
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void Format(uint8 Drive, uint8 type)
{
    if (Drive < MAX_DRIVE)
    {
        DriveInfo[Drive].Valid=1;
    	WriteDBRSector(Drive, type);	//Write DBR
    	if (OK == CheckFileSystem(Drive))
    	{
        	WriteFAT(Drive, type);
        	WriteRootDir(Drive, type);		//Write Root Dir
    	}
       // FlashCacheCloseAll();
    }
}



/***************************************************************************
��������:д512�ֽ�DBR����
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void WriteDBRSector16(uint8 Drive)
{
	uint32 TotLogicSec;
	uint16 i;
	uint8 DbrBuf[512];
	uint32 TmpVal1;
	uint32 TmpVal2;

    memset(DbrBuf, 0x00, 512);
	//2 0x00-0x02:��תָ��,��������ϵͳ�����������
	DbrBuf[0]=0xeb;
	DbrBuf[1]=0x3e;
	DbrBuf[2]=0x90;
	
	//2 0x03-0x0a:OEM ID,����ϵͳ���ƺͰ汾
	DbrBuf[3]='M';
	DbrBuf[4]='S';
	DbrBuf[5]='D';
	DbrBuf[6]='O';
	DbrBuf[7]='S';
	DbrBuf[8]='5';
	DbrBuf[9]='.';
	DbrBuf[10]='0';
	//2 0x0b-0x23:BPB
	DbrBuf[12]=0x02;		//ÿ����512B
	DbrBuf[14]=0x01;		//����������
	DbrBuf[16]=0x02;		//FAT����
	DbrBuf[18]=0x02;		//��Ŀ¼����512
	DbrBuf[21]=0xf8;		//Ӳ��
	DbrBuf[0x16]=63;		//ÿFATռ�õ�������
	DbrBuf[0x18]=63;		//ÿ�ŵ�������
	DbrBuf[0x1a]=255;	    //��ͷ��
	DbrBuf[0x26]=0x29;	    //��չ������ǩ,������0x28��0x29

	DbrBuf[0x36]='F';		//�ļ�ϵͳ����(8B)
	DbrBuf[0x37]='A';
	DbrBuf[0x38]='T';
	DbrBuf[0x39]='1';
	DbrBuf[0x3a]='6';
	DbrBuf[0x3b]=' ';
	DbrBuf[0x3c]=' ';
	DbrBuf[0x3d]=' ';

	//2 0x01fe-0x01ff:signtrue
	*((uint16*)&DbrBuf[510])=0xAA55;
	
	TotLogicSec=GetTotalMem(Drive) << 1;
	for (i=0; i<8; i++)
	{
		if (TotLogicSec <= DskTableFAT16[i].DiskSize)
		{
			DbrBuf[13]=DskTableFAT16[i].SecPerClus;
			break;
		}
	}
	if (TotLogicSec > 65535)						//NOT 128M(256M~1G)
	{
	    *((uint32*)&DbrBuf[32])=TotLogicSec;		//��������
	}
	else
	{
	    DbrBuf[19]=TotLogicSec & 0xff;		        //С������
	    DbrBuf[20]=TotLogicSec >> 8;
	}
	TmpVal1 = TotLogicSec-(((DbrBuf[15]<<8)+DbrBuf[14]) + 32);	//������-��������-��Ŀ¼����
	TmpVal2 = (256 * DbrBuf[13]) + 2;
	DbrBuf[22]=(TmpVal1 + (TmpVal2 -1)) / TmpVal2;		//FAT��������
    WriteSecs(Drive, 0, DbrBuf, 1);                     //FSI_FREE_COUNT
}



/***************************************************************************
��������:д512�ֽ�DBR����
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void WriteDBRSector32(uint8 Drive)
{
	uint32 TotLogicSec;
	uint16 i;
    uint8 buf[512*6];
	uint32 TmpVal1;
	uint32 TmpVal2;

    memset(buf, 0x00, 512*6);
	//2 0x00-0x02:��תָ��,��������ϵͳ�����������
	buf[0]=0xeb;
	buf[1]=0x58;
	buf[2]=0x90;
	
	//2 0x03-0x0a:OEM ID,����ϵͳ���ƺͰ汾
	buf[3]='M';
	buf[4]='S';
	buf[5]='D';
	buf[6]='O';
	buf[7]='S';
	buf[8]='5';
	buf[9]='.';
	buf[10]='0';
	//2 0x0b-0x23:BPB
	buf[12]=0x02;		//ÿ����512B
	buf[14]=0x20;		//����������32
	buf[16]=0x02;		//FAT����
	buf[21]=0xf8;		//Ӳ��
	buf[0x18]=63;		//ÿ�ŵ�������
	buf[0x1a]=255;	    //��ͷ��

	buf[44]=0x02;		//BPB_RootClus
	buf[48]=0x01;		//BPB_FSInfo, fixed 1
	buf[50]=0x06;		//BPB_BkBootSec, fixed 6
	buf[66]=0x29;		//��չ������ǩ,������0x28��0x29

	buf[0x52]='F';		//�ļ�ϵͳ����(8B)
	buf[0x53]='A';
	buf[0x54]='T';
	buf[0x55]='3';
	buf[0x56]='2';
	buf[0x57]=' ';
	buf[0x58]=' ';
	buf[0x59]=' ';

	//2 0x01fe-0x01ff:signtrue
	*((uint16*)&buf[510])=0xAA55;
	
	TotLogicSec=GetTotalMem(Drive) << 1;
	for (i=0; i<8; i++)
	{
		if (TotLogicSec <= DskTableFAT32[i].DiskSize)
		{
			buf[13]=DskTableFAT32[i].SecPerClus;
			break;
		}
	}

	*((uint32*)&buf[32])=TotLogicSec;		//��������
	TmpVal1 = TotLogicSec-(((buf[15]<<8)+buf[14]) + 0);	//������-��������-��Ŀ¼����
	TmpVal2 = (256 * buf[13]) + 2;
    TmpVal2 >>= 1;
	TmpVal2=(TmpVal1 + (TmpVal2 -1)) / TmpVal2;		//FAT��������
	TmpVal2 = (TmpVal2 + 3) & (~0x03ul);            //���뵽2KB
	*((uint32*)&buf[36])=TmpVal2;            //FAT��������

	*((uint32*)&buf[512+0])=0x41615252;       //'RRaA'
	*((uint32*)&buf[512+484])=0x61417272;     //'rrAa'
	*((uint32*)&buf[512+488])=0xffffffff;     //total free clus
    *((uint32*)&buf[512+492])=0x02;           //next free clus
	*((uint16*)&buf[512+510])=0xAA55;
	*((uint16*)&buf[1024+510])=0xAA55;
    
    WriteSecs(Drive, 0, buf, 6);
    WriteSecs(Drive, 6, buf, 6);
}



/***************************************************************************
��������:д512�ֽ�DBR����
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void WriteDBRSector(uint8 Drive, uint8 type)
{
    if (type!=FAT32)
        WriteDBRSector16(Drive);
    else
        WriteDBRSector32(Drive);
}


/***************************************************************************
��������:�Ը�ʽ��,���ڵ�һ��ʹ��ʱ
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void WriteRootDir(uint8 Drive, uint8 type)
{
    uint8 buf[512];
    uint32 RootDirEndAddr;
	uint32 RootDirAddr;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

    memset(buf, 0x00, 512);
	RootDirAddr=(uint32)pInfo->ResvdSecCnt+(pInfo->NumFATs * pInfo->FATSz)+pInfo->PBRSector;
    if (type!=FAT32)
        RootDirEndAddr=RootDirAddr+pInfo->RootDirSectors;
    else
        RootDirEndAddr=RootDirAddr+pInfo->SecPerClus;
    for (; RootDirAddr<RootDirEndAddr; RootDirAddr++)
    {
        WriteSecs(Drive, RootDirAddr, buf, 1);
    }
}


/***************************************************************************
��������:дFAT��,���ڵ�һ��ʹ��ʱ
��ڲ���:type:FAT����
���ڲ���:��
���ú���:
***************************************************************************/
void WriteFAT(uint8 Drive, uint8 type)
{
    uint32 i;
	uint16 FATEntOffset;
	uint32 FATSecNum;
	uint8 FatBuf[512];
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	GetFATPosition(Drive, 2, &FATSecNum, &FATEntOffset);
    memset(FatBuf, 0x00, 512);
    if (type==FAT16)
    {
    	*((uint16*)&FatBuf[0])=0xfff8;
    	*((uint16*)&FatBuf[2])=0xffff;
    }
    else
    {
    	*((uint32*)&FatBuf[0])=0x0ffffff8;
    	*((uint32*)&FatBuf[4])=0x0fffffff;
    	*((uint32*)&FatBuf[8])=0x0fffffff;  //Rootdir reserved
    }

    WriteSecs(Drive, FATSecNum, FatBuf, 1);
    memset(FatBuf, 0x00, 512);
    for (i=1; i<pInfo->FATSz; i++)
        WriteSecs(Drive, FATSecNum+i, FatBuf, 1);
    
	//CopyFat(Drive, 0);
}
#endif

/*********************************************************************************************************
** ��������	:
** ��������	:
** �䡡��	:NONE
** �䡡��	:NONE
** �����ο�  
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
void CacheInit(void)
{
    uint32 i;

    for (i=0; i<FDT_CACHE_NUM; i++)
    {
        FdtCache[i].Valid=0;
        FdtCache[i].Flag=0;     //��ǲ�����0��ʾ��CACHE���д
        FdtCache[i].Drive=0;
        FdtCache[i].Count=i;
    }
    for (i=0; i<FAT_CACHE_NUM; i++)
    {
        FatCache[i].Valid=0;
        FdtCache[i].Flag=0;
        FatCache[i].Drive=0;
        FatCache[i].Count=i;
    }
}


/*********************************************************************************************************
** ��������	:ReadCache
** ��������	:��CACHE�ж�һ������
** �䡡��	:type:CACHE_FDT, CACHE_FAT, CACHE_DATA
** �䡡��	:NONE
** �����ο�  
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 ReadCache(uint8 Drive, uint32 Index, pCACHE *cache, uint8 type)
{
    uint8 i;
    uint8 j;
	uint8 status;
    uint8 MaxCache;
    pCACHE ptr;

    status=ERROR;
    if (type == CACHE_FDT)
    {
        MaxCache=FDT_CACHE_NUM;
        ptr=FdtCache;
    }
    else if (type == CACHE_FAT)
    {
        MaxCache=FAT_CACHE_NUM;
        ptr=FatCache;
    }
    //3����CACHE
    for (i=0; i<MaxCache; i++)
    {
        if (ptr[i].Valid!=0 && ptr[i].Drive==Drive && ptr[i].Sec==Index)
        {
            status=OK;
            break;
        }
    }

    if (i >= MaxCache)
    {
    	//3�滻һ��CACHE
    	for (i=0; i<MaxCache; i++)
    	{
    		if (ptr[i].Valid==0 || ptr[i].Count==0)
    		{
                #ifndef BOOT_ONLY
                CacheWriteBack(&ptr[i]);
                #endif
                ptr[i].Valid=0;
                if (OK==ReadSecs(Drive, Index, ptr[i].Buf, 1))
                {
                    status=OK;
                    ptr[i].Valid=1;
                    ptr[i].Drive=Drive;
                    ptr[i].Sec=Index;
                }
    			break;
    		}
    	}
    }
    
	//3��ӽ����ʴ���
	for (j=0; j<MaxCache; j++)
	{
		if (ptr[j].Count > ptr[i].Count)
		{
			ptr[j].Count--;
		}
	}
	ptr[i].Count=MaxCache-1;
    *cache = &ptr[i];
    return (status);
}

#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:CacheWriteBack
** ��������	:CACHE��д��������
** �䡡��	:NONE
** �䡡��	:NONE
** �����ο�  
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 CacheWriteBack(pCACHE cache)
{
    uint8 Rt=OK;

    if (cache->Valid!=0 && cache->Flag!=0)
    {
        Rt=WriteSecs(cache->Drive, cache->Sec, cache->Buf, 1);
        cache->Flag=0;
    }
    return (Rt);
}


/*********************************************************************************************************
** ��������	:CacheWriteBackAll
** ��������	:CACHE��д��������
** �䡡��	:NONE
** �䡡��	:NONE
** �����ο�  
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 CacheWriteBackAll(void)
{
    uint32 i;

    for (i=0; i<FDT_CACHE_NUM; i++)
    {
        CacheWriteBack(&FdtCache[i]);
    }
    for (i=0; i<FAT_CACHE_NUM; i++)
    {
        CacheWriteBack(&FatCache[i]);
    }
    //FlashCacheCloseAll();
    return (OK);
}
#endif

/***************************************************************************
��������:����ļ�ϵͳ����
��ڲ���:Drive
���ڲ���:OK=�ļ�ϵͳ��ȷ,ERROR=�ļ�ϵͳ����
���ú���:
***************************************************************************/
uint8 CheckFileSystem(uint8 Drive)
{
	uint32 PBRSector=0;
	uint8 DbrBuf[512];

    if (OK != ReadSecs(Drive, 0, DbrBuf, 1))   //��DBR����
        return (ERROR);
	if(OK != CheckFatBootSector(DbrBuf))	//not a FAT volume
	{
		PBRSector = CheckMbr(DbrBuf);
		if(PBRSector==0 || PBRSector==-1)		//not a MBR
		{
			return (ERROR);
		}
		else
		{
            if (OK != ReadSecs(Drive, PBRSector, DbrBuf, 1))   //��DBR����
				return (ERROR);
			if(OK != CheckFatBootSector(DbrBuf))
				return (ERROR);
		}
	}

	GetBootInfo(Drive, PBRSector, DbrBuf);
	return (OK);
}


/***************************************************************************
��������:��ȡBPB����
��ڲ���:DBR���ڵ�����
���ڲ���:�ļ�ϵͳ����
���ú���:
***************************************************************************/
void GetBootInfo(uint8 Drive, uint32 PBRSector, uint8 *DbrBuf)
{
	uint8 type;
	uint32 DataSec;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

    pInfo->BytsPerSec=(DbrBuf[12] << 8)+DbrBuf[11];
	pInfo->SecPerClus=DbrBuf[13];
	//pInfo->LogBytePerSec=lg2(pInfo->BytsPerSec);
	pInfo->LogSecPerClus=lg2(pInfo->SecPerClus);
	pInfo->NumFATs=DbrBuf[16];
	pInfo->RootEntCnt=(DbrBuf[18] << 8) + DbrBuf[17];
	pInfo->FATSz=*((uint16*)&DbrBuf[22]);
	if(pInfo->FATSz == 0)
	{
		pInfo->FATSz=*((uint32*)&DbrBuf[36]);
	}

	DataSec = (DbrBuf[20]<<8)+DbrBuf[19];
	if(DataSec == 0)
	{
		DataSec=*((uint32*)&DbrBuf[32]);
	}

	pInfo->ResvdSecCnt = *((uint16*)&DbrBuf[14]);
	pInfo->RootDirSectors = ((pInfo->RootEntCnt * 32) + (pInfo->BytsPerSec - 1)) >> 9;
	pInfo->PBRSector = PBRSector;
	pInfo->FirstDataSector=pInfo->ResvdSecCnt + (pInfo->NumFATs * pInfo->FATSz) + pInfo->RootDirSectors + PBRSector;

	DataSec = DataSec - pInfo->FirstDataSector + PBRSector;
	while((DataSec & (pInfo->SecPerClus-1)) != 0)	//Modify by lxs @2006.01.10 for No Standard Lib compiler
		DataSec--;

	pInfo->TotClus = DataSec >> pInfo->LogSecPerClus;
	if(pInfo->TotClus < 4085)
	{
		type = FAT12;		// Volume is FAT12
	}
	else if(pInfo->TotClus < 65525)
	{
		type = FAT16;		// Volume is FAT16
	}
	else
	{
		type = FAT32;		// Volume is FAT32
	}


	if(type == FAT32)
	{
		pInfo->RootClus=*((uint32*)&DbrBuf[44]);
        pInfo->FSInfo=*((uint16*)&DbrBuf[48]);
	}
	else
	{
		pInfo->RootClus=0;
        pInfo->FSInfo=0;
	}

    pInfo->TotSec=GetCapacity(Drive);
	pInfo->FATType=type;
    pInfo->PathClus=pInfo->RootClus;
    pInfo->Valid=1;
}


/***************************************************************************
��������:����Ƿ���2���ݴ�
��ڲ���:�����Ĳ���
���ڲ���:-1--����,����ֵΪ�ݴ�ֵ
���ú���:
***************************************************************************/
int16 lg2(uint16 arg)
{
	uint16 log;

	for(log = 0; log < 16; log++)
	{
		if(arg & 1)
		{
			arg >>= 1;
			return ((arg != 0) ? -1 : log);
		}
		arg >>= 1;
	}
	return (-1);
}


/***************************************************************************
��������:����Ƿ�����������(DBR)
��ڲ���:��
���ڲ���:ERROR=��DBR����,OK=��DBR����
���ú���:
***************************************************************************/
bool CheckFatBootSector(uint8 *DbrBuf)
{
	uint16 temp;
	bool bad = OK;

	if(DbrBuf[0] == 0xE9);	// OK
	else if(DbrBuf[0] == 0xEB && DbrBuf[2] == 0x90);	// OK
	else
	{
		bad = ERROR;		// Missing JMP/NOP
	}

	// check other stuff
	temp = DbrBuf[13];
	if((lg2(temp)) < 0)
	{
		bad = ERROR;		//Sectors per cluster is not a power of 2
	}

	// very few disks have only 1 FAT, but it's valid
	temp = DbrBuf[16];
	if(temp != 1 && temp != 2)
	{
		bad = ERROR;		// Invalid number of FATs
	}

	// can't check against dev.sects because dev.sects may not yet be set
	temp = *((uint16*)&DbrBuf[24]);
	if(temp == 0 || temp > 63)
	{
		bad = ERROR;		// Invalid number of sectors
	}

	// can't check against dev.heads because dev.heads may not yet be set
	temp = *((uint16*)&DbrBuf[26]);
	if(temp == 0 || temp > 255)
	{
		bad = ERROR;		// Invalid number of heads
	}

	temp = *((uint16*)&DbrBuf[510]);
	if(temp != 0xAA55)
	{
		bad = ERROR;		// Invalid signature of FATs
	}

	return (bad);
}


/***************************************************************************
��������:����Ƿ�����������¼(MBR)
��ڲ���:��
���ڲ���:0--����,����ֵ��
���ú���:
***************************************************************************/
uint32 CheckMbr(uint8 *DbrBuf)
{
	bool bad = 0;
	uint32 MbrLba;

#if 1
	if(DbrBuf[0] != 0xFA)
	{
		bad = 1;		//Missing 0xFA, CLI instruction in X86
	}
	if(DbrBuf[446] != 0x80)
	{
		bad = 1;		//Missing 0x80, Action signture
	}
#endif
	MbrLba = *((uint16*)&DbrBuf[456]);
	MbrLba <<= 16;
	MbrLba |= *((uint16*)&DbrBuf[454]);
	return (bad? 0: MbrLba);		//DBR sector, [457][456][455][454]
}

#ifndef BOOT_ONLY
/***************************************************************************
��������:������һ��FAT���ڶ���FAT����෴
��ڲ���:����(0:fat1 to fat2, 1:fat2 to fat1)
���ڲ���:��
���ú���:
***************************************************************************/
void CopyFat(uint8 Drive, uint8 FatNum)
{
	uint16 i;
	uint32 FAT1LBA;
	uint32 FAT2LBA;
	uint32 EraseSectors;
	uint8 Buf[512];
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (FatNum >= 2)
		return;
	EraseSectors=pInfo->FATSz;	//�ܲ��������߼�������
	FAT1LBA=pInfo->ResvdSecCnt+pInfo->PBRSector+pInfo->FATSz*FatNum;
	FAT2LBA=pInfo->ResvdSecCnt+pInfo->PBRSector+pInfo->FATSz*(FatNum ^ 0x01);
	for (i=0; i<EraseSectors; i++)
	{
        ReadSecs(Drive, i+FAT1LBA, Buf, 1);
        WriteSecs(Drive, i+FAT2LBA, Buf, 1);
	}
}
#endif

/***************************************************************************
��������:��ȡһ������FAT���е�λ��
��ڲ���:�غ�
���ڲ���:�����ź�����ƫ��
���ú���:
***************************************************************************/
void GetFATPosition(uint8 Drive, uint32 cluster, uint32 *FATSecNum, uint16 *FATEntOffset)
{
	uint32 FATOffset;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if(pInfo->FATType == FAT16)
		FATOffset = cluster<<1;
	else if (pInfo->FATType == FAT32)
		FATOffset = cluster<<2;
    else if (pInfo->FATType == FAT12)
        FATOffset = (cluster*3)>>1;
    *FATSecNum = (FATOffset >> 9)+pInfo->ResvdSecCnt+pInfo->PBRSector;
    *FATEntOffset = (uint16)(FATOffset & (pInfo->BytsPerSec-1));
}


/*********************************************************************************************************
** ��������	:GetTotalMem
** ��������	:����������
** ��ڲ���	:Drive=0ѡ��FLASH, Drive=1ѡ��SD��
** �䡡��	:��K�ֽ���
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint32 GetTotalMem(uint8 Drive)
{
    return (GetCapacity(Drive)>>1);
}


/*********************************************************************************************************
** ��������	:
** ��������	:����FAT16��ָ���صĺ��漸����
** �䡡��	:Index���غ�, Count:�ؼ���
** �䡡��	:��һ���غ�
** ȫ�ֱ���	:
** ����ģ��	:ReadSecs
********************************************************************************************************/
uint32 FATGetNextClus(uint8 Drive, uint32 Index, uint32 Count)
{
    uint32 Rt;
	uint16 ByteIndex;
	uint32 SecIndex;
    pCACHE cache;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (Index >= pInfo->TotClus+2)
	{
		return (BAD_CLUS);
	}
    Rt=Index;
	while (Count-- != 0)
	{
		/* ���������ź��ֽ����� */
        GetFATPosition(Drive, Rt, &SecIndex, &ByteIndex);

		/* ��ȡFAT������ */
        if (OK != ReadCache(Drive, SecIndex, &cache, CACHE_FAT))
		{
			Rt=BAD_CLUS;
			goto exit;
		}
        if (pInfo->FATType == FAT16)
    		Rt = *((uint16*)&cache->Buf[ByteIndex]);
        else if (pInfo->FATType == FAT32)
        {
    		Rt = *((uint32*)&cache->Buf[ByteIndex]);
        }
        else if (pInfo->FATType == FAT12)
        {
            Rt=cache->Buf[ByteIndex];
            if ((ByteIndex+1) >= 512)
            {
                ReadCache(Drive, SecIndex+1, &cache, CACHE_FAT);
                Rt |= cache->Buf[0] << 8;
            }
            else
                Rt |= cache->Buf[ByteIndex+1] << 8;
            if ((Index & 0x01) != 0)
                Rt >>= 4;
            else
                Rt &= 0x0fff;
        }
        else
        {
            Rt=BAD_CLUS;
            break;
        }
	}
	if (pInfo->FATType==FAT16 && Rt>=(BAD_CLUS & 0xffff))  /* �Ƿ����������� */
	{
		Rt |= 0x0ffful << 16;
	}
    else if (pInfo->FATType==FAT12 && Rt>=(BAD_CLUS & 0x0fff))
    {
		Rt |= 0x0fffful << 12;
    }
exit:	
	Rt &= 0x0fffffff;
	return (Rt);
}


#ifndef BOOT_ONLY
/*********************************************************************************************************
** ��������	:GetFreeMem
** ��������	:����ʣ��ռ�
** �䡡��	:��
** �䡡��	:�ܿ���K�ֽ���
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint32 GetFreeMem(uint8 Drive)
{
	uint32 TotalFree;
	uint32 Rt;
	uint32 clus;
	uint32 EndClus;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
    TotalFree=0;
	if (Drive<MAX_DRIVE)
	{
        if (pInfo->FreeClus>pInfo->TotClus)  //�Ƿ�ֵ���¶�ȡ
        {
            #if 0
        	if (pInfo->FATType == FAT32)
        	{
        	    uint8 buf[512];
        	    uint32 BPB_FsInfo;
                
                ReadSecs(Drive, pInfo->PBRSector, buf, 1);		//DBR
        		BPB_FsInfo=(buf[49] << 8) + buf[48];			//BPB_FSINFO
                ReadSecs(Drive, pInfo->PBRSector+BPB_FsInfo, buf, 1);	//FSI_FREE_COUNT
        		TotalFree=(buf[491] << 8) + buf[490];
        		TotalFree <<= 16;
        		TotalFree |= (buf[489] << 8) + buf[488]; 
        		if (TotalFree != 0xffffffff)
        			goto CaluFreeMem;
        	}
            #endif
        	EndClus=pInfo->TotClus+2;
        	#ifdef VIRTUAL_MEMORY
        	if (Drive == DISK_FLASH)
        	{
        		uint32 SecIndex;
        		uint32 MaxClus;

        		for (MaxClus=2; MaxClus<EndClus; MaxClus++)
        		{
        			SecIndex = ((MaxClus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector;
        			if (SecIndex > pInfo->TotSec)
        			{
        				EndClus=MaxClus-1;
        				break;
        			}
        		}
        	}
        	#endif
        	
        	for (clus=2; clus<EndClus; clus++)
        	{
        		Rt=FATGetNextClus(Drive, clus, 1);
        		if (Rt == EMPTY_CLUS)
        		{
        			TotalFree++;				//�ܿմ�
        			continue;
        		}
        		if (Rt == BAD_CLUS)
        			return (0);
        	}
        	pInfo->FreeClus=TotalFree;
        }
    	TotalFree = pInfo->FreeClus << (pInfo->LogSecPerClus-1);
	}
	return (TotalFree);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ɾ���ļ��������ļ������ʣ��ռ�
** �䡡��	:���µ��ֽ���
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:ReadSecs
** ˵		��	:��������(��ɾ���ļ�)Ϊ'+', ��С����Ϊ(�紴���ļ�)'-'
********************************************************************************************************/
void UpdataFreeMem(uint8 Drive, int32 size)
{
	uint32 TotalFree;
	uint32 clus;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

    if (Drive < MAX_DRIVE)
    {
    	TotalFree=GetFreeMem(Drive);
    	TotalFree <<= 1;
    	TotalFree >>= pInfo->LogSecPerClus;

    	if (size < 0)
    		clus=(~size) + 1;
    	else
    		clus=size;
    	clus += (1 << (pInfo->LogSecPerClus + 9))-1;
    	clus >>= pInfo->LogSecPerClus+9;
    	if (size >= 0)
    		TotalFree += clus;
    	else
    	{
    		if (TotalFree >= clus)
    			TotalFree -= clus;
    		else
    			TotalFree = 0;
    	}
        #if 0
    	if (pInfo->FATType == FAT32)
    	{
	        uint8 buf[512];
            ReadSecs(Drive, pInfo->PBRSector+pInfo->FSInfo, buf, 1);
    		buf[488]=(uint8)TotalFree;
    		buf[489]=(uint8)(TotalFree >> 8);
    		buf[490]=(uint8)(TotalFree >> 16);
    		buf[491]=(uint8)(TotalFree >> 24);
            WriteSecs(Drive, pInfo->PBRSector+pInfo->FSInfo, buf, 1); //FSI_FREE_COUNT
    	}
        #endif
        pInfo->FreeClus = TotalFree;
    }
}



/*****************************************************************************************
** ��������	:
** ��������	:ɾ��ָ������
** �䡡��	:Index���������״غ�
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:
*****************************************************************************************/
void FATDelClusChain(uint8 Drive, uint32 Index)
{
	uint16 FATEntOffset;
	uint32 ThisClus;
	uint32 NextClus;
	uint32 FATSecNum, pre_FATSecNum;
	uint8  Buf[512];
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
	if (Index <= EMPTY_CLUS_1 || Index >= BAD_CLUS)
	{
		return;
	}

	ThisClus=Index;
	pre_FATSecNum=0;
	FLASH_PROTECT_OFF();
	do
	{
		NextClus=FATGetNextClus(Drive, ThisClus, 1);					//��FAT1�л�ȡ��һ��
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

		switch (pInfo->FATType)		//���ô���
		{
			case FAT12:
                if ((ThisClus & 0x01) != 0)
                {
                    Buf[FATEntOffset] &= 0x0f;
                }
                else
                    Buf[FATEntOffset]=EMPTY_CLUS;
                if ((FATEntOffset+1) >= 512)
                {
                    WriteSecs(Drive, pre_FATSecNum, Buf, 1);
			        pre_FATSecNum = FATSecNum+1;
                    ReadSecs(Drive, pre_FATSecNum, Buf, 1);
                }
                FATEntOffset=(FATEntOffset+1) % 512;
                if ((ThisClus & 0x01) != 0)
                {
                    Buf[FATEntOffset]=EMPTY_CLUS;
                }
                else
                    Buf[FATEntOffset] &= 0xf0;
				break;
			case FAT16:
			    *((uint16*)&Buf[FATEntOffset])=EMPTY_CLUS;
				break;
			case FAT32:
			    *((uint32*)&Buf[FATEntOffset])=EMPTY_CLUS;
				break;
			default:
				break;
		}
		ThisClus=NextClus;
	}while (NextClus > EMPTY_CLUS_1 && NextClus < BAD_CLUS);
    WriteSecs(Drive, pre_FATSecNum, Buf, 1);
	FLASH_PROTECT_ON();
}


/****************************************************************************************
** ��������	:
** ��������	:������һ����
** �䡡��	:Index���غ�
**        	 Next����һ���غ�
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:
*****************************************************************************************/
void FATSetNextClus(uint8 Drive, uint32 Index, uint32 Next)
{
    uint16 temp;
	uint16 ByteIndex;
	uint32 SecIndex;
    pCACHE cache;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);
    
	if (Index <= EMPTY_CLUS_1 || Index >= pInfo->TotClus+2)
	{
		return;
	}
    
	/* ���������ź��ֽ����� */
	switch (pInfo->FATType)
	{
		case FAT12:
		case FAT16:
		case FAT32:
			GetFATPosition(Drive, Index, &SecIndex, &ByteIndex);
			break;
		default:
			return;
	}

    ReadCache(Drive, SecIndex, &cache, CACHE_FAT);
    cache->Flag=1;  //��Ҫ��д
    cache->Drive=Drive;
	switch (pInfo->FATType)
	{
		case FAT12:
			temp=Next & 0x0fff;
            if ((Index & 0x01) != 0)
            {
                temp <<= 4;
                temp |= cache->Buf[ByteIndex] & 0x0f;
                cache->Buf[ByteIndex]=temp;
            }
            else
                cache->Buf[ByteIndex]=temp;
            temp >>= 8;
            if ((ByteIndex+1) >= 512)
            {
                ReadCache(Drive, SecIndex+1, &cache, CACHE_FAT);
            }
            ByteIndex=(ByteIndex+1) % 512;
            if ((Index & 0x01) != 0)
            {
                cache->Buf[ByteIndex]=temp;
            }
            else
                cache->Buf[ByteIndex]=(cache->Buf[ByteIndex] & 0xf0) | temp;
			break;
		case FAT16:
			*((uint16*)&cache->Buf[ByteIndex])=Next & 0xffff;
			break;
		case FAT32:
			*((uint32*)&cache->Buf[ByteIndex])=Next & 0x0fffffff;
			break;
		default:
			break;
	}
}


/****************************************************************************************
** ��������	:
** ��������	:Ϊָ����������һ����
** �䡡��	:Index������������һ���غ�,���Ϊ0,��Ϊһ����������һ����
** �䡡��	:���ӵĴغ�
** ȫ�ֱ���	:
** ����ģ��	:
*****************************************************************************************/
uint32 FATAddClus(uint8 Drive, uint32 Index)
{
	uint32 NextClus,ThisClus,MaxClus;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

	if (Index >= BAD_CLUS)
	{
		return (BAD_CLUS);
	}
    
	MaxClus=pInfo->TotClus+2;

	//���Ҵ��������һ����
	ThisClus = Index;
	if (ThisClus > EMPTY_CLUS_1)
	{
		while (1)
		{
			NextClus = FATGetNextClus(Drive, ThisClus, 1);
			if (NextClus >= EOF_CLUS_1 || NextClus <= EMPTY_CLUS_1)
			{
				break;		//���ҵ��մػ������
			}
			if (NextClus == BAD_CLUS)
			{
				return (BAD_CLUS);
			}
			ThisClus = NextClus;
		}
	}
	else
	{
		ThisClus = EMPTY_CLUS_1;
	}

	//�Ӵ�����β����ʼ����һ���մ�
	for (NextClus = ThisClus + 1; NextClus < MaxClus; NextClus++)
	{
		if (FATGetNextClus(Drive, NextClus, 1) == EMPTY_CLUS)
		{
			break;
		}
	}

	//�յ�β��û�յ����ٴ�ͷ����
	if (NextClus >= MaxClus)
	{
		for (NextClus = EMPTY_CLUS_1 + 1; NextClus < ThisClus; NextClus++)
		{
			if (FATGetNextClus(Drive, NextClus, 1) == EMPTY_CLUS)
			{
				break;
			}
		}
	}

	//���յ�Ϊ��β������һ����
	if (FATGetNextClus(Drive, NextClus, 1) == EMPTY_CLUS)
	{
		if (ThisClus > EMPTY_CLUS_1)
		{
			FATSetNextClus(Drive, ThisClus, NextClus);
		}
		FATSetNextClus(Drive, NextClus, EOF_CLUS_END);
        UpdataFreeMem(Drive, -(1 << (pInfo->LogSecPerClus+9)));
		return (NextClus);
	}
	else
	{
		return (BAD_CLUS);
	}
}
#endif

/****************************************************************************************
**                            End Of File
****************************************************************************************/


