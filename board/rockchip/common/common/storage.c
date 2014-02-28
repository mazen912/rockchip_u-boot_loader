/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:  flash.C
Author:     XUESHAN LIN
Created:    1st Dec 2008
Modified:
Revision:   1.00
********************************************************************************
********************************************************************************/
#define     IN_STORAGE
#include    "../armlinux/config.h"
#include    "storage.h"


//TODO 
extern void FtlReIntForUpdate(void);
extern uint32 FTLLowFormat(void);

#ifdef RK_FLASH_BOOT_EN
MEM_FUN_T NandFunOp = 
{
    0,
    BOOT_FROM_FLASH,
    0,
    FTLInit,
    FlashReadID,
    FlashBootReadPBA,
    FlashBootWritePBA,
    FlashBootReadLBA,
    FlashBootWriteLBA,
    FlashBootErase,
    ReadFlashInfo,
    FtlReIntForUpdate,
    FTLLowFormat,
    FtlGetCurEraseBlock,
    FtlGetAllBlockNum,
    FlashBootGetCapacity,
    FlashBootSysDataLoad,
    FlashBootSysDataStore,
};
#endif

#ifdef RK_SDMMC_BOOT_EN
MEM_FUN_T emmcFunOp = 
{
    SDMMC_SDC_ID,
    BOOT_FROM_EMMC,
    0,
    SdmmcInit,
    SdmmcReadID,
    SdmmcBootReadPBA,
    SdmmcBootWritePBA,
    SdmmcBootReadLBA,
    SdmmcBootWriteLBA,
    SdmmcBootErase,
    SdmmcReadFlashInfo,
    SdmmcCheckIdBlock,
    NULL,
    NULL,
    NULL,
    SdmmcGetCapacity,
    SdmmcSysDataLoad,
    SdmmcSysDataStore,
};
#endif

#ifdef RK_SDCARD_BOOT_EN
MEM_FUN_T sd0FunOp = 
{
    0,
    BOOT_FROM_SD0,
    0,
    SdmmcInit,
    SdmmcReadID,
    SdmmcBootReadPBA,
    SdmmcBootWritePBA,
    SdmmcBootReadLBA,
    SdmmcBootWriteLBA,
    SdmmcBootErase,
    SdmmcReadFlashInfo,
    NULL,//SdmmcCheckIdBlock,
    NULL,
    NULL,
    NULL,
    SdmmcGetCapacity,
    SdmmcSysDataLoad,
    SdmmcSysDataStore,
};
#endif

#ifdef RK_SPI_BOOT_EN
MEM_FUN_T SpiFunOp = 
{
    0,
    BOOT_FROM_SPI,
    SPI_MASTER_BASE_ADDR,
    SPIFlashInit,
    SPIFlashReadID,
    SpiBootReadPBA,
    SpiBootWritePBA,
    SpiBootReadLBA,
    SpiBootWriteLBA,
    SpiBootErase,
    SpiReadFlashInfo,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
#endif

#ifdef RK_SD_BOOT
uint32 ExtCardUserPartOffset;

uint32 RKUpdateFtlInit(void)
{
    uint32 ret = 0;
    if(gpMemFun->flag == BOOT_FROM_FLASH)
    {
        ret = FTLInit(NANDC_BASE_ADDR);
    }
    return ret;
}

uint16 SDBootVer = 0;
int32 StorageInit(void)
{
    uint32 ret;
    SdmmcSDMInit();
    gpSdBootMemFun = &sd0FunOp;
    ExtCardUserPartOffset = 0;
    if(gpSdBootMemFun->Init(gpSdBootMemFun->id) == 0) // ������SD���̼�
    {
        uint8 boot_ver[30];
        sdBootCheckSdCard(gpSdBootMemFun->id);
        ExtCardUserPartOffset = sdBootGetUserPartOffset(gpSdBootMemFun->id);
    }
#ifdef RK_FLASH_BOOT_EN
    gpMemFun = &NandFunOp;
    FlashCsInit();
    ret = FlashInit(NANDC_BASE_ADDR);
    if(ret != FTL_NO_FLASH) // ��flash����ʼ��flash
    {
        gpMemFun->Valid = 1;
        if(ret == FTL_NO_IDB)
        {
            RkPrintf("flash no idb\n");
            ;// û��idb����� 
        }
        else if(ret == FTL_OK)
        {
            RkPrintf("flash had idb\n");
            ;
        }
        return 0;
    }
#endif

#ifdef RK_SDMMC_BOOT_EN
    gpMemFun = &emmcFunOp;
    if(gpMemFun->Init(gpMemFun->id) == 0)
    {
        gpMemFun->Valid = 1;
        return 0;
    }
#endif
    return -1;
}
#else
MEM_FUN_T *memFunTab[] = 
{
#ifdef RK_SDCARD_BOOT_EN
    &sd0FunOp,
#endif

#ifdef RK_FLASH_BOOT_EN
    &NandFunOp,
#endif

#ifdef RK_SDMMC_BOOT_EN
    &emmcFunOp,
#endif

#ifdef RK_SPI_BOOT_EN
    &SpiFunOp, 
#endif
};

#define MAX_MEM_DEV (sizeof(memFunTab)/sizeof(MEM_FUN_T *))

int32 StorageInit(void)
{
    uint32 memdev;
    
    SdmmcSDMInit();
    for(memdev=0; memdev<MAX_MEM_DEV; memdev++)
    {
        gpMemFun = memFunTab[memdev];
        if(memFunTab[memdev]->Init(memFunTab[memdev]->id) == 0)
        {
            memFunTab[memdev]->Valid = 1;
            //gpMemFun = memFunTab[memdev];
#ifdef RK_SDCARD_BOOT_EN
            if(memFunTab[memdev]->flag == BOOT_FROM_SD0)
            {
                if(BootFromSdCard(memFunTab[memdev]->id) != 0)
                    continue;
            }
#endif
            return 0;
        }
    }
    gpMemFun = memFunTab[0];
    return -1;
}
#endif

void FW_ReIntForUpdate(void)
{
    gpMemFun->Valid = 0;
    if(gpMemFun->IntForUpdate)
        gpMemFun->IntForUpdate();
    gpMemFun->Valid = 1;
}

void FW_SorageLowFormat(void)
{
    if(gpMemFun->LowFormat)
    {
        gpMemFun->Valid = 0;
        gpMemFun->LowFormat();
        gpMemFun->Valid = 1;
    }
} 

uint32 FW_StorageGetValid(void)
{
    return gpMemFun->Valid;
}

/***************************************************************************
��������:��ȡ FTL ���������� block��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
uint32 FW_GetCurEraseBlock(void)
{
    uint32 data = 1;
    if(gpMemFun->GetCurEraseBlock)
       data = gpMemFun->GetCurEraseBlock();
     return data;
}
/***************************************************************************
��������:��ȡ FTL ��block��
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
uint32 FW_GetTotleBlk(void)
{
    uint32 totle = 1;
    if(gpMemFun->GetTotleBlk)
       totle = gpMemFun->GetTotleBlk();
    return totle;
}

int StorageReadPba(uint32 PBA , void *pbuf, uint16 nSec )
{
    int ret = FTL_ERROR;
    if(gpMemFun->ReadPba)
       ret = gpMemFun->ReadPba(gpMemFun->id, PBA, pbuf, nSec );
    return ret;
}

int StorageWritePba(uint32 PBA , void *pbuf, uint16 nSec )
{
    int ret = FTL_ERROR;
    if(gpMemFun->WritePba)
       ret = gpMemFun->WritePba(gpMemFun->id, PBA , pbuf, nSec);
    return ret;
}

int StorageReadLba( uint32 LBA ,void *pbuf  , uint16 nSec)
{
    int ret = FTL_ERROR;
    if(gpMemFun->ReadLba)
       ret = gpMemFun->ReadLba(gpMemFun->id, LBA , pbuf, nSec );
    return ret;
}

int StorageWriteLba( uint32 LBA, void *pbuf  , uint16 nSec  ,uint16 mode)
{
    int ret = FTL_ERROR;
    if(gpMemFun->WriteLba)
       ret = gpMemFun->WriteLba(gpMemFun->id, LBA , pbuf, nSec,mode);
    return ret;
}

uint32 StorageGetCapacity(void)
{
    uint32 ret = FTL_ERROR;
    if(gpMemFun->GetCapacity)
       ret = gpMemFun->GetCapacity(gpMemFun->id);
    return ret;
}


uint32 StorageSysDataLoad(uint32 Index,void *Buf)
{
    uint32 ret = FTL_ERROR;
    memset(Buf,0,512);
    if(gpMemFun->SysDataLoad)
       ret = gpMemFun->SysDataLoad(gpMemFun->id, Index,Buf);
    return ret;
}

uint32 StorageSysDataStore(uint32 Index,void *Buf)
{
    uint32 ret = FTL_ERROR;
    if(gpMemFun->SysDataStore)
       ret = gpMemFun->SysDataStore(gpMemFun->id, Index,Buf);
    return ret;
}

uint32 UsbStorageSysDataLoad(uint32 offset,uint32 len,uint32 *Buf)
{
    uint32 ret = FTL_ERROR;
    uint32 i;
    for(i=0;i<len;i++)
    {
        StorageSysDataLoad(offset + 2 + i,Buf);
        if(offset + i < 2) 
        {
            Buf[0] = 0x444E4556;
            Buf[1] = 504;
        }
        Buf += 128;
    }
    return ret;
}

uint32 UsbStorageSysDataStore(uint32 offset,uint32 len,uint32 *Buf)
{
    uint32 ret = FTL_ERROR;
    uint32 i;
    for(i=0;i<len;i++)
    {
        Buf[0] = 0x444E4556;
        Buf[1] = 504;
        StorageSysDataStore(offset + 2 + i,Buf);
        Buf += 128;
    }
    return ret;
}

int StorageReadFlashInfo( void *pbuf)
{
    int ret = FTL_ERROR;
    if(gpMemFun->ReadInfo)
    {
       gpMemFun->ReadInfo(pbuf);
       ret = FTL_OK;
    }
    return ret;
}

int StorageEraseBlock(uint32 blkIndex, uint32 nblk, uint8 mod)
{
    int Status = FTL_OK;
    if(gpMemFun->Erase)
        Status = gpMemFun->Erase(0, blkIndex, nblk, mod);
    return Status;
}

uint16 StorageGetBootMedia(void)
{
    return gpMemFun->flag;
}

uint32 StorageGetSDFwOffset(void)
{
    uint32 offset = 0;
    if(gpMemFun->flag != BOOT_FROM_FLASH)
    {
        offset = SdmmcGetFwOffset(gpMemFun->id);
    }
    return offset;
}

uint32 StorageGetSDSysOffset(void)
{
    uint32 offset = 0;
    if(gpMemFun->flag != BOOT_FROM_FLASH)
    {
        offset = SdmmcGetSysOffset(gpMemFun->id);
    }
    return offset;
}

