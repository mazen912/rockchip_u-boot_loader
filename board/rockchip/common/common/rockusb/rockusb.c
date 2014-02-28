/********************************************************************************
*********************************************************************************
                        COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
                                --  ALL RIGHTS RESERVED  --

File Name:      FW_Upgrade.C
Author:         XUESHAN LIN
Created:        1st Dec 2008
Modified:
Revision:       1.00
********************************************************************************
********************************************************************************/
#define     IN_FW_Upgrade
#include    "../../armlinux/config.h"

//#pragma arm section code = "LOADER2"

extern uint32 CacheFlushDRegion(uint32 adr, uint32 size);
extern void boot_run( void );
extern void boot_session_handle( void );
extern void FW_CheckMultiBadBlock(uint8 ChipSel, uint32 blkNum , uint32 BlockCnt , uint64 * pResult);
extern void  CacheFlushBoth(void);
extern void   MMUDisable(void); 
extern uint32 RkldTimerGetTick( void );
extern void CacheDisableBoth(void);
extern void DRVDelayMs(uint32 ms);

uint32 * pimemBuf = (uint32 *)0;//0x10501000

void boot_session_dataout( uint16 len );
/***************************************************************************
16λ���ݴ�С��ת��
***************************************************************************/
uint16 Swap16(uint16 input)
{
    return ((input>>8)|(input<<8));
}

/***************************************************************************
32λ���ݴ�С��ת��
***************************************************************************/
uint32 Swap32(uint32 input)
{
    return ((input>>24)|(input<<24)|(input>>8&0xff00)|(input<<8&0xff0000));
}

/***************************************************************************
�������ݴ���OUT������
***************************************************************************/
void FWOutPacket(uint32 len)
{
	//cmdCount++;
	//RkPrintf("out packet:%x",cmdCount);
    if (FWCmdPhase == K_CommandPhase)
    {
        //cache flush
        CacheFlushDRegion((uint32)&gCBW,sizeof(gCBW));
        if(gCBW.Code == K_FW_SDRAM_EXECUTE)
        {
            uint8 *pchar = (uint8*)DataBuf;
            FW_SDRAM_Parameter = ((uint32)pchar[25])|((uint32)pchar[24]<<8)|((uint32)pchar[23]<<16)|((uint32)pchar[22]<<24);
        }
        gCBW.LBA=Swap32(gCBW.LBA);
        gCBW.Len=Swap16(gCBW.Len);

        if (CBWValidVerify() == OK)
        {
            FWCmd();
        }
        else
            CSWHandler(CSW_FAIL,0);
    }
	//��������
    else if (FWCmdPhase == K_OutDataPhase)
    {
        FW_DataLenCnt+=len;
        if( FW_WR_Mode == FW_WR_MODE_PBA )
        {
            if ((uint32)gCBW.Len*528 <= FW_DataLenCnt)
            {
                #if 0
                CacheFlushDRegion((uint32)DataBuf,(uint32)gCBW.Len*528 );
                FW_FlashWritePBA(gCBW.LUN,gCBW.LBA, DataBuf, gCBW.Len);
                SendCSW();
                #else
                usbCmd.cmd = K_FW_WRITE_10;
                usbCmd.xferLen = FW_DataLenCnt;
                #endif
                FW_DataLenCnt = 0;
            }
            else
            {
            }
        }
        else if( FW_WR_Mode == FW_WR_MODE_LBA )
        {
            if(1)//  (((uint32)gCBW.Len*512 <= FW_DataLenCnt) || (FW_DataLenCnt==0x10000))
            {
                usbCmd.cmd = K_FW_LBA_WRITE_10;
                usbCmd.xferLen = FW_DataLenCnt;
                FW_DataLenCnt = 0;
            }
            else
            {
            }
        }
        else  if( FW_WR_Mode == FW_WR_MODE_SDRAM )
        {
            if ((uint32)gCBW.Len <= FW_DataLenCnt)
            {
                uint8 * pSdram = (uint8*)(SDRAM_BASE_ADDRESS + gCBW.LBA);
                CacheFlushDRegion((uint32)DataBuf,(uint32)gCBW.Len );
                ftl_memcpy(pSdram,(uint8*)DataBuf,FW_DataLenCnt);
                memset(pSdram+FW_DataLenCnt,0,512);
                SendCSW();
                FW_DataLenCnt = 0;
            }
            else
            {
                ReadBulkEndpoint(len, &DataBuf[FW_DataLenCnt/4]);
            }
        }
    #if defined(DRIVERS_SPI) || defined(OTP_DATA_ENABLE)
        else  if( FW_WR_Mode == FW_WR_MODE_SPI )
        {
            if ((uint32)gCBW.Len <= FW_DataLenCnt)
            {
                #if 0
                CacheFlushDRegion((uint32)DataBuf,(uint32)gCBW.Len );
                SPIFlashWrite(gCBW.LBA, DataBuf, FW_DataLenCnt);
                SendCSW();
                #else
                usbCmd.cmd = K_FW_SPI_WRITE_10;
                usbCmd.xferLen = FW_DataLenCnt;
                #endif
                FW_DataLenCnt = 0;
            }
            else
            {
            }
        }
        #endif
	 #ifdef LINUX_BOOT
	 else if( FW_WR_MODE_SESSION == FW_WR_Mode )
	 	{
	 	    boot_session_dataout(len);
		    return ;	//set by session_dataout handler .
	 	}
	 #endif
	}	
}

/***************************************************************************
IN������
***************************************************************************/
void FWInPacket(void)
{
    if (FWCmdPhase == K_InDataPhase)
    {
        usbCmd.cmd = usbCmd.Precmd;
    }
    else if (FWCmdPhase == K_InCSWPhase)
    {
        SendCSW();
    }
    else
    {
    }
}


/***************************************************************************
�̼������������, ������ʽ�ο��ĵ�"FW_Upgrae.doc"
***************************************************************************/
void FWCmd(void)
{
    //RkPrintf("code:%x",gCBW.Code);
    extern  uint8   UsbBusReset;
    UsbBusReset = 4; //�ܹ�����ͨѶ�����reset����
    switch (gCBW.Code)
    {
        case K_FW_TEST_UNIT_READY:		//0x00
            FW_TestUnitReady();
            break;

        case K_FW_READ_FLASH_ID:		//0x01
            FW_ReadID();
            break;

        case K_FW_SET_DEVICE_ID:		//0x02
            FW_SetDeviceID();
            break;

        case K_FW_TEST_BAD_BLOCK:		//0x03
            FW_TestBadBlock();
            break;
			
        case K_FW_READ_10:				//0x04
            FW_Read10();
            break;

        case K_FW_WRITE_10:				//0x05
            FW_Write10();
            FWCmdPhase=K_OutDataPhase;
            break;

        case K_FW_READ_FLASH_INFO:        //0x1A
            FW_GetFlashInfo();
            break;
			
		case K_FW_LBA_READ_10:				//0x14
			FW_LBARead10();
			break;

		case K_FW_LBA_WRITE_10:				//0x15
			FW_LBAWrite10();
			FWCmdPhase =K_OutDataPhase;
			break;

		case K_FW_ERASE_SYS_DISK:			//0x16
            FW_LowFormatSysDisk();
			break;

		case K_FW_SDRAM_READ_10:			//0x17
			FW_SDRAMRead10();
			break;

		case K_FW_SDRAM_WRITE_10:			//0x18
			FW_SDRAMWrite10();
			FWCmdPhase =K_OutDataPhase;
			break;

		case K_FW_SDRAM_EXECUTE:			//0x19
		
			#ifdef LINUX_BOOT
			boot_run();
			#endif
			FW_SDRAMExecute();
			break;
 
		case K_FW_ERASE_10:				//0x06
			FW_Erase10();
			break;

		case K_FW_RESET:				//0xff
			FW_Reset();
			break;

		case K_FW_ERASE_10_FORCE:		//0x0b
			FW_Erase10Force();
			break;

		case K_FW_GET_VERSION:			//0x0C
			FW_GetVersion();
            break;
        case K_FW_GET_CHIP_VER:			//0x1B
            FW_GetChipVer();
			break;
            
        case K_FW_LOW_FORMAT:			//0x1C
            FW_LowFormat();
            break;

    	#ifdef LINUX_BOOT
            case K_FW_SESSION:				//0x30
    		boot_session_handle( );
    		break;
    	#endif 
#if defined(DRIVERS_SPI) || defined(OTP_DATA_ENABLE)
        case K_FW_SPI_READ_10:                //0x04
            FW_SPIRead10();
            break;

        case K_FW_SPI_WRITE_10:               //0x05
            FW_SPIWrite10();
            FWCmdPhase =K_OutDataPhase;
            break; 
	#endif
		case K_FW_SET_RESET_FLAG:
#if(PALTFORM==RK28XX)
            FileInit();
            SysSetUsbFlag();
#else
		    FWSetResetFlag = 1;
#endif		    
		    CSWHandler(CSW_GOOD,0);
            SendCSW();
			break;
	default:
			CSWHandler(CSW_FAIL,0);
            SendCSW();
			break;
      }                
}


/***************************************************************************
����:����׼��0x00
***************************************************************************/
void FW_TestUnitReady(void)
{
    if(FW_StorageGetValid() == 0)
    {//���ڵ͸�
        uint32 totleBlock = FW_GetTotleBlk();
        uint32 currEraseBlk = FW_GetCurEraseBlock();
        //gCSW.Signature=0x53425355;
        //gCSW.Tag=gCBW.Tag;
        CSWHandler(CSW_FAIL,0);
        gCSW.Residue=Swap32((totleBlock<<16)|currEraseBlk);
        //gCSW.Status=CSW_FAIL;
    }
    else if(gCBW.Reseved0 == 0xFD)
    {
        uint32 totleBlock = FW_GetTotleBlk();
        uint32 currEraseBlk = 0;
        //gCSW.Signature=0x53425355;
        //gCSW.Tag=gCBW.Tag;
        CSWHandler(CSW_FAIL,0);
        gCSW.Residue=Swap32((totleBlock<<16)|currEraseBlk);
        //gCSW.Status=CSW_FAIL;
        FWLowFormatEn = 1;
    }
    else if(SecureBootLock)
    {
        CSWHandler(CSW_FAIL,0);
        //gCSW.Residue=0;
    }
    else
    {
        CSWHandler(CSW_GOOD,0);
        gCSW.Residue=Swap32(6);
    }
    SendCSW();
}


/***************************************************************************
��������:�̼���������:��FLASH ID
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_ReadID(void)
{
    uint8 *FlashID=BulkInBuf;
    if(gpMemFun->ReadId)
        gpMemFun->ReadId(gCBW.LUN, FlashID);
    CSWHandler(CSW_GOOD,0);
    WriteBulkEndpoint(5, FlashID);
    FWCmdPhase=K_InCSWPhase;
}


/***************************************************************************
��������:�̼���������:����FLASH ����
			  Flash 1:
			  0:8bit small page;	1:8bit large page 4cyc;		2:8bit large page 5cyc
			  3:16bit small page;	4:16bit large page 4cyc;	5:16bit large page 5cyc
			  6:MLC 8bit large page 5cyc		7:MLC 8bit large page 5cyc, 4KB/page
��ڲ���:
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_SetDeviceID(void)
{
	CSWHandler(CSW_GOOD,0);
    SendCSW();
}

/***************************************************************************
��������:�̼���������:����FLASH ����
			  Flash 1:
			  0:8bit small page;	1:8bit large page 4cyc;		2:8bit large page 5cyc
			  3:16bit small page;	4:16bit large page 4cyc;	5:16bit large page 5cyc
			  6:MLC 8bit large page 5cyc		7:MLC 8bit large page 5cyc, 4KB/page
��ڲ���:
���ڲ���:��
���ú���:��
         1��2009-4-10 �����ӳ����أ�PC���߼�⵽����ϵͳ�̳����������б�����
            ����Ŀ飬������loader��͸����·���ռ䡣            
***************************************************************************/
void FW_LowFormatSysDisk(void)
{
	CSWHandler(CSW_GOOD,0);
    SendCSW();
}

/***************************************************************************
��������:���Ի��顪��0:�ÿ�; 1:����
��ڲ���:������е�ָ��������ַ
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_TestBadBlock(void)
{
	uint16 i;
	uint8 status = CSW_GOOD;
	uint32 PageNum;
	uint16 BlockCnt;
	uint32 TestResult[16];

	for (i=0; i<16; i++)
		TestResult[i]=0;
	/*
	PageNum=gCBW.LBA;
	PageNum *= (uint32)g_id_block_size;
	BlockCnt=gCBW.Len;
	for (i=0; i<BlockCnt; i++)
	{
		if (FW_CheckBadBlock(gCBW.LUN, PageNum) != OK)
		{
			TestResult[i/64] |= 1 << (i % 64);
			status = CSW_FAIL;
	    }
		PageNum += g_id_block_size;
	}*/
	CSWHandler(status, 0);
    WriteBulkEndpoint(64, TestResult);
    FWCmdPhase=K_InCSWPhase;
}

/***************************************************************************
��������:��528 PAGE��С��
��ڲ���:������е������е�ַ������������
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_Read10(void)
{
    #if 0
    uint16 byte;
    CSWHandler(CSW_GOOD, gCBW.XferLen);

    FW_FlashReadPBA(gCBW.LUN,gCBW.LBA,DataBuf,gCBW.Len);

    dCSWDataResidueVal=(uint32)gCBW.Len * 528;
    byte=(dCSWDataResidueVal > BulkEpSize) ? BulkEpSize : dCSWDataResidueVal;
    FW_DataLenCnt=0;
    WriteBulkEndpoint(byte, &DataBuf[FW_DataLenCnt]);
    dCSWDataResidueVal-=byte; 
    FW_DataLenCnt+=byte;
    FWCmdPhase=(dCSWDataResidueVal>0) ? K_InDataPhase : K_InCSWPhase;
    #else
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    FWCmdPhase=K_InDataPhase;
    usbCmd.cmd = K_FW_READ_10;
    usbCmd.Precmd = K_FW_READ_10;
    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferLBA = gCBW.LBA;
    #endif
}

/***************************************************************************
��������:��528 PAGE��Сд
��ڲ���:������е������е�ַ������������
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_Write10(void)
{
    uint32 len;
    CSWHandler(CSW_GOOD,gCBW.XferLen);
    FW_DataLenCnt=0;
    FW_WR_Mode = FW_WR_MODE_PBA;
    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferBuf = bulkBuf[usbCmd.pppBufId&0x01];
    usbCmd.xferLBA = gCBW.LBA;
    usbCmd.preLen = 0;
    len = (usbCmd.len>=0x40)?0x40:usbCmd.len;
    ReadBulkEndpoint(len*528, usbCmd.xferBuf);
}

void FW_LBARead10( void )
{
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    FWCmdPhase=K_InDataPhase;
    usbCmd.cmd = K_FW_LBA_READ_10;
    usbCmd.Precmd = K_FW_LBA_READ_10;
    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferLBA = gCBW.LBA;
}

void FW_LBAWrite10(void)
{
    uint32 len;
    CSWHandler(CSW_GOOD,gCBW.XferLen);
    FW_DataLenCnt=0;
    FW_WR_Mode = FW_WR_MODE_LBA;
    FW_IMG_WR_Mode = gCBW.Reseved0;

    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferBuf = bulkBuf[usbCmd.pppBufId&0x01];
    usbCmd.xferLBA = gCBW.LBA;
    usbCmd.preLen = 0;
    len = (usbCmd.len>=0x80)?0x80:usbCmd.len;
    ReadBulkEndpoint(len*512, usbCmd.xferBuf);
}

void FW_SDRAMRead10( void )
{
    uint16 byte;
	uint8 *pBuf;			//IN��������ָ��
    pBuf =(uint8*)gCBW.LBA + SDRAM_BASE_ADDRESS;
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    //ftl_memcpy(DataBuf,pBuf,gCBW.Len);
    WriteBulkEndpoint(gCBW.Len,pBuf);
    FWCmdPhase= K_InCSWPhase;
}

void FW_SDRAMWrite10(void)
{
    CSWHandler(CSW_GOOD,gCBW.XferLen);
    FW_DataLenCnt=0;
    FW_WR_Mode = FW_WR_MODE_SDRAM;
    ReadBulkEndpoint(gCBW.Len, DataBuf);
}

#if defined(DRIVERS_SPI) || defined(OTP_DATA_ENABLE)
void FW_SPIRead10( void )
{
    #if 0
    uint16 byte;
    dCSWDataResidueVal=(uint32)gCBW.Len;
    SPIFlashRead(gCBW.LBA, DataBuf, gCBW.Len);
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    byte=(dCSWDataResidueVal > BulkEpSize) ? BulkEpSize : dCSWDataResidueVal;
    FW_DataLenCnt=0;
    WriteBulkEndpoint(byte, &DataBuf[FW_DataLenCnt]);
    dCSWDataResidueVal-=byte; 
    FW_DataLenCnt+=byte;
    FWCmdPhase=(dCSWDataResidueVal>0) ? K_InDataPhase : K_InCSWPhase;
    #else
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    FWCmdPhase=K_InDataPhase;
    usbCmd.cmd = K_FW_SPI_READ_10;
    usbCmd.Precmd = K_FW_SPI_READ_10;
    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferLBA = gCBW.LBA;
    #endif
}

void FW_SPIWrite10(void)
{
    uint32 len;
    CSWHandler(CSW_GOOD,gCBW.XferLen);
    FW_DataLenCnt=0;
    FW_WR_Mode = FW_WR_MODE_SPI;

    usbCmd.LUN = gCBW.LUN;
    usbCmd.LBA = gCBW.LBA;
    usbCmd.len = gCBW.Len;
    usbCmd.xferLen = 0;
    usbCmd.xferBuf = bulkBuf[usbCmd.pppBufId&0x01];
    usbCmd.xferLBA = gCBW.LBA;
    usbCmd.preLen = 0;
    len = (usbCmd.len>=0x10000)?0x10000:usbCmd.len;
    ReadBulkEndpoint(len, usbCmd.xferBuf);
}
#endif

typedef void(*pSdramFun)(void *p); 
extern void DisableRemap(void);
extern void DisableTcm(void);
void FW_SDRAMExecute(void)
{
    pSdramFun pfun;
    CSWHandler(CSW_GOOD,0);
    SendCSW();
    FW_SDRAM_Parameter =  SDRAM_BASE_ADDRESS + (FW_SDRAM_Parameter);//&0x0FFFFFFF
    FW_SDRAM_ExcuteAddr = SDRAM_BASE_ADDRESS + (gCBW.LBA);//&0x0FFFFFFF
}


/***************************************************************************
��������:������BLOCK��������0:�ÿ�; 1:����
��ڲ���:������е�������ַ
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_LowFormat(void)
{
    FWLowFormatEn = 1;
    CSWHandler(CSW_GOOD,0);
    SendCSW();
}

/***************************************************************************
��������:������BLOCK��������0:�ÿ�; 1:����
��ڲ���:������е�������ַ
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_Erase10(void)
{
	bool Status;
    if(gpMemFun->Erase && SecureBootLock == 0)
        Status = gpMemFun->Erase(gCBW.LUN, gCBW.LBA, gCBW.Len, 0);
	CSWHandler(Status, 0);
    SendCSW();
}

/***************************************************************************
��������:�̼���������:��FLASH ID
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_GetFlashInfo(void)
{
    extern void ReadFlashInfo(void *buf);
#if 0
    uint8 *FlashInfoBuf=BulkInBuf;
    ReadFlashInfo(FlashInfoBuf);
    CSWHandler(CSW_GOOD,0);
    WriteBulkEndpoint(512, FlashInfoBuf);
    FWCmdPhase=K_InCSWPhase;
#else
    uint16 byte;
    CSWHandler(CSW_GOOD, 0);
    if(gpMemFun->ReadInfo)
        gpMemFun->ReadInfo(DataBuf);
    FW_DataLenCnt=0;
    WriteBulkEndpoint(512, &DataBuf[FW_DataLenCnt]);
    FWCmdPhase= K_InCSWPhase;
#endif
}

/***************************************************************************
��������:������BLOCK��������0:�ÿ�; 1:����
��ڲ���:������е�������ַ
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_Erase10Force(void)
{
	bool Status;
    if(gpMemFun->Erase && SecureBootLock == 0)
        Status = gpMemFun->Erase(gCBW.LUN, gCBW.LBA, gCBW.Len, 1);
	CSWHandler(Status, 0);
    SendCSW();
}

/***************************************************************************
��������:ϵͳ��λ
��ڲ���:��
���ڲ���:��
���ú���:��
LOG:
        20100209,HSL@RK,ADD LUN for deffirent reboot.
        0: normal  reboot, 1: loader reboot.
***************************************************************************/
void FW_Reset(void)
{
    CSWHandler(CSW_GOOD, gCBW.XferLen);
    SendCSW();
    DRVDelayMs(10);
    FWSetResetFlag = gCBW.Reseved0;
    if(gCBW.Reseved0 == 0)
    {
        //SoftReset();
        FWSetResetFlag = 0xFF; // SoftReset ����UsbHook����ִ��
    }
}


/***************************************************************************
��������:��ȡ�м����Ȩ��Ϣ
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_GetVersion(void)
{
    CSWHandler(FW_VERSION,0);
    SendCSW();
}


/***************************************************************************
��������:��ȡ�м����Ȩ��Ϣ
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
void FW_GetChipVer(void)
{
    uint32 *chipInfo=(uint32*)BulkInBuf;
    #if((PALTFORM==RK30XX)||(PALTFORM==RK_ALL)) 
        extern uint32 Rk30ChipVerInfo[4];  
        ftl_memcpy(chipInfo, Rk30ChipVerInfo, 16);
    #else
        ftl_memcpy(chipInfo, (uint8*)(BOOT_ROM_CHIP_VER_ADDR), 16);
    #endif
    
#if(PALTFORM==RK292X)
    if(chipInfo[0]==0x32393241)//"292A"
    {
        chipInfo[0] = 0x32393258; // "292X"
    }
#endif

    CSWHandler(CSW_GOOD,0);
    WriteBulkEndpoint(16, chipInfo);
    FWCmdPhase=K_InCSWPhase;
}


/***************************************************************************
����:�������ЧУ��
���:��
����:OK=������ʽ��ȷ,ERROR=������ʽ����
***************************************************************************/
bool CBWValidVerify(void)
{
    if (gCBW.Signature==0x43425355 && gCBW.LUN < MAX_LUN && gCBW.CBWLen <= MAX_CDBLEN)//CBW_SIGNATUREУ��
		//LUN������鳤��У��
        return(OK);		//��Ч����
    else
        return(ERROR);	//��Ч����
}


/***************************************************************************
����:CSW����
���:HostDevCase=״̬,DeviceTrDataLen=�豸Ҫ���͵����ݳ���
����:��
***************************************************************************/
void CSWHandler(uint8 HostDevCase,uint32 DeviceTrDataLen)
{
    gCSW.Signature=0x53425355;
    gCSW.Tag=gCBW.Tag;
    gCSW.Residue=Swap32(gCBW.XferLen-DeviceTrDataLen);
    gCSW.Status=HostDevCase;
}


/***************************************************************************
����:����CSW
���:��
����:��
***************************************************************************/
void SendCSW(void)
{
    uint8 *tmp=BulkInBuf;
    ftl_memcpy(tmp, (uint8*)&gCSW, sizeof(gCSW));
    ReadBulkEndpoint(31, (uint8*)&gCBW);
    WriteBulkEndpoint(13, (uint8*)tmp);
    FWCmdPhase=K_CommandPhase;
}

//#pragma arm section code

