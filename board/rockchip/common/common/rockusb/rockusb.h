/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:      FW_Upgrade.h
Author:         XUESHAN LIN
Created:        1st Dec 2008
Modified:
Revision:       1.00
********************************************************************************
********************************************************************************/

#ifndef _FW_UPGRADE_H
#define _FW_UPGRADE_H

#ifndef __GNUC__
#define PACKED1 __packed
#define PACKED2
#define ALIGN(x) __align(x)
#else
#define PACKED1
#define PACKED2 __attribute__((packed))
#define ALIGN(x) __attribute__ ((aligned(x)))
#endif
//1�����ò���
#define 	MAX_LUN					    4			/*���֧�ֵ��߼��豸��*/
#define 	FW_VERSION				    1			/*�м����Ȩ��, 1��ʾv1.00*/

//1��������

#define	    MAX_CDBLEN      			0x10		//���CBW����

/*******************************************************************
CSW����״ֵ̬
*******************************************************************/
#define	    CSW_GOOD					0x00		//����ͨ��
#define	    CSW_FAIL					0x01		//����ʧ��

/*******************************************************************
����׶�
*******************************************************************/
#define	    K_CommandPhase			    0x00
#define	    K_OutDataPhase				0x01
#define 	K_InDataPhase				0x81
#define 	K_InCSWPhase				0x08

/*******************************************************************
�̼��������
*******************************************************************/
#define 	K_FW_TEST_UNIT_READY		0x00
#define 	K_FW_READ_FLASH_ID		    0x01
#define 	K_FW_SET_DEVICE_ID		    0x02
#define 	K_FW_TEST_BAD_BLOCK		    0x03
#define 	K_FW_READ_10				0x04
#define 	K_FW_WRITE_10				0x05
#define 	K_FW_ERASE_10				0x06
#define 	K_FW_WRITE_SPARE			0x07
#define 	K_FW_READ_SPARE			    0x08

#define 	K_FW_ERASE_10_FORCE		    0x0b
#define 	K_FW_GET_VERSION			0x0c

#define 	K_FW_LBA_READ_10            0x14
#define 	K_FW_LBA_WRITE_10           0x15
#define 	K_FW_ERASE_SYS_DISK         0x16
#define 	K_FW_SDRAM_READ_10          0x17
#define 	K_FW_SDRAM_WRITE_10         0x18
#define 	K_FW_SDRAM_EXECUTE          0x19
#define 	K_FW_READ_FLASH_INFO	    0x1A
#define     K_FW_GET_CHIP_VER           0x1B
#define     K_FW_LOW_FORMAT             0x1C
#define     K_FW_SET_RESET_FLAG         0x1E
#define     K_FW_SPI_READ_10            0x21  
#define     K_FW_SPI_WRITE_10           0x22  

#define     K_FW_SESSION				0X30 // ADD BY HSL.
#define 	K_FW_RESET				    0xff


//1����ԭ������
extern 	void 	FWCmd(void);
extern	void	FWInPacket(void);
extern	void    FWOutPacket(uint32 len);
extern	void    FWOutPacketTest(uint32 len);
extern 	void 	FW_TestUnitReady(void);
extern 	void 	FW_ReadID(void);
extern 	void 	FW_SetDeviceID(void);
extern 	void 	FW_TestBadBlock(void);
extern 	void 	FW_Read10(void);
extern 	void 	FW_Write10(void);
extern 	void 	FW_LBARead10(void);
extern 	void 	FW_LBAWrite10(void);
extern 	void 	FW_Erase10(void);
extern 	void    FW_GetFlashInfo(void);
extern 	void 	FW_Reset(void);
extern 	void 	FW_Erase10Force(void);
extern 	void 	FW_GetVersion(void);
extern 	void    FW_GetChipVer(void);
extern 	bool 	CBWValidVerify(void);
extern  void    FW_LowFormat(void);
extern 	void 	CSWHandler(uint8 HostDevCase, uint32 DeviceTrDataLen);
extern 	void    SendCSW(void);
extern    uint16 Swap16(uint16 input);
extern    uint32 Swap32(uint32 input);
extern    void FW_SDRAMRead10( void );
extern    void FW_SDRAMWrite10(void);
extern    void FW_LowFormatSysDisk(void);
extern    void FW_SDRAMExecute(void);
extern    void FW_ReadVersionAll(void);
extern    void FW_SetUsbFlag(void);
extern    void SoftReset(void);
extern    void FW_SPIRead10(void);
extern    void FW_SPIWrite10(void);




/*******************************************************************/

//1�ṹ����
typedef PACKED1 struct _USBMSDC_CBW
{
    uint32 Signature;
    uint32 Tag;
    uint32 XferLen;
    uint8 Flags;
    uint8 LUN;
    uint8 CBWLen;
    uint8 Code;
    uint8 Reseved0;
    uint32 LBA;
    uint8 Reseved1;
    uint16 Len;
    uint8 Reseved2[7];
}PACKED2 CBW,*pCBW;

typedef PACKED1 struct _USBMSDC_CSW
{
    uint32	Signature;
    uint32	Tag;
    uint32	Residue;
    uint8	Status;
}PACKED2 CSW;

typedef volatile struct _USB_XFER
{
    uint8 cmd;
    uint8 Precmd;
    uint8 LUN;
    uint32 LBA;
    uint32 len;
    
    void *xferBuf;
    uint32 xferLBA;
    uint32 xferLen;

    uint32 preLen;
    uint32 preLBA;
    uint32 *preBuf;

    uint32 pppBufId;
} USB_XFER, *pUSB_XFER;


#define FW_WR_MODE_PBA       0
#define FW_WR_MODE_LBA       1
#define FW_WR_MODE_SDRAM     2
#define FW_WR_MODE_SPI       3
#define FW_WR_MODE_SESSION   4	//BY HSL


#define SDRAM_BASE_ADDRESS 0x60000000
#define FW_WR_BUF_SIZE   1024 //KB
#define DRAM_SIZE  	0x10000000

#define USB_XFER_BUF_SIZE (2048*512/4) //1MB

//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_FW_Upgrade
		#define	EXT
#else
		#define	EXT		extern
#endif		

	EXT		ALIGN(4) uint8 	FWCmdPhase;			//����׶�״̬��
	EXT     ALIGN(4) uint8  FWLowFormatEn;
	EXT     ALIGN(4) uint8  FWSetResetFlag;
	EXT		uint32 			FW_DataLenCnt;
	EXT		uint32 			FW_Write10PBA;
    EXT		int32           dCSWDataResidueVal;
    
	EXT		ALIGN(4) uint16 FWLBA_DataLenCnt;
	EXT		uint32 			FWLBA_Write10PBA;
    EXT		uint32 			FW_SDRAM_Parameter;
    EXT		uint32 			FW_SDRAM_ExcuteAddr;    
    EXT     uint32          FW_WR_Mode;
    EXT     uint32          FW_IMG_WR_Mode;//img д����lbaд��0Ϊimg��1Ϊlba
    EXT     USB_XFER        usbCmd;
    EXT     uint32 			*bulkBuf[2];


	EXT		ALIGN( 64 )CSW  	      gCSW;
    EXT		ALIGN( 64 )CBW           gCBW;
    EXT		ALIGN(64) uint8          BulkInBuf[512];
    EXT		ALIGN(64)uint32          DataBuf[528*128/4];
    EXT		ALIGN(64)uint32          Data[(1024*8*4/4)];
    EXT		ALIGN(64)uint32          SpareBuf[(32*8*4/4)];
    EXT    ALIGN(64) uint32          FWLBAWriteSrcBuf[512*32/4];
    EXT    ALIGN(64) uint32          FWLBAReadSrcBuf[512*32/4];
    EXT     ALIGN( 64 ) uint32 			usbXferBuf[2*USB_XFER_BUF_SIZE];

//1�����
#ifdef IN_FW_Upgrade
#ifdef DEBUG_FLASH
const uint8 USB_CMD[]=
{
	K_FW_TEST_UNIT_READY,
	K_FW_READ_FLASH_ID,
	K_FW_SET_DEVICE_ID,
	K_FW_TEST_BAD_BLOCK,
	K_FW_READ_10,
	K_FW_WRITE_10,
	K_FW_ERASE_10,
	K_FW_WRITE_SPARE,
	K_FW_RESET
};

const char* USB_CMD_Desr[]=
{
	"TEST_UNIT_READY",
	"READ_FLASH_ID",
	"SET_DEVICE_ID",
	"TEST_BAD_BLOCK",
	"READ_10",
	"WRITE_10",
	"ERASE_10",
	"WRITE_SPARE",
	"RESET",
};
#endif
#endif
#endif

