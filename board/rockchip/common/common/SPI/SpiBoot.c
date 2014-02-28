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
#include    "../../armlinux/config.h"
#include    "SpiBoot.h"

#ifdef      DRIVERS_SPI
#define     SPI_BLOCK_SIZE          512
#define     SPI_LBA_PART_OFFSET     1024

void SpiReadFlashInfo(void *buf)
{
    pFLASH_INFO pInfo=(pFLASH_INFO)buf;
    memset((uint8*)buf,0,512);
    pInfo->BlockSize = SPI_BLOCK_SIZE;
    pInfo->ECCBits = 0;
    pInfo->FlashSize = 0x200000; 
    pInfo->PageSize = 4;
    pInfo->AccessTime = 40;
    pInfo->ManufacturerName=0;
    pInfo->FlashMask=1;
}

/***************************************************************************
��������:������BLOCK��������0:�ÿ�; 1:����
��ڲ���:ChipSel, blkIndex=��ʼblock��,  nblk=blk�� ��mod: 0 Ϊ��ͨ������ 1Ϊǿ�Ʋ���
���ڲ���:0 Ϊû�л��飬1Ϊ�л���
���ú���:��
***************************************************************************/
uint32 SpiBootErase(uint8 ChipSel, uint32 blkIndex, uint32 nblk, uint8 mod)
{
	return OK;
}
#if 0
uint32 SpiBootWritePBA(uint8 ChipSel, uint32 PBA , void *pbuf, uint16 nSec )
{
    uint32 *pDataBuf = pbuf;
    uint16 len;
    if(PBA + nSec >= 256) //�����˲�д
        return;
    for (len=0; len<nSec; len++)
    {
        ftl_memcpy( Data,pDataBuf+(len)*132, 512);
        SPIFlashWrite(PBA*512, Data, 512);
    }
    return 0; 
}

uint32 SpiBootReadPBA(uint8 ChipSel, uint32 PBA , void *pbuf, uint16 nSec )
{
    uint16 len;
    uint32 *pDataBuf = pbuf;
    PBA &= 0xFF;


    memset(SpareBuf,0xFF,SPARE_LEN);
    for (len=0; len<nSec; len++)
    {
        SPIFlashRead(PBA*512, Data, 512);
        ftl_memcpy(pDataBuf+(len)*132, Data, 512);
        ftl_memcpy(pDataBuf+(len)*132+128, SpareBuf, 16);
    }
    return 0;
}
#else
uint32 SpiBootWritePBA(uint8 ChipSel, uint32 PBA , void *pbuf, uint16 nSec )
{
    uint32 *pDataBuf = pbuf;
    uint16 len,i;
    if(PBA + nSec >= SPI_BLOCK_SIZE*2) //�����˲�д
        return;
    //RkPrintf("SpiBootWritePBA(%x,%x,%x,%x)\n", ChipSel, PBA , pbuf, nSec );
    for (len=0; len<nSec; len+=4)
    {
        for (i=0; i<4; i++)
        {
            //RkPrintf("SPIFlashWrite len %x nSec %x  i %x \n", len,nSec,i);
        	SPIFlashWrite(((len+PBA)*2+i)*512, pDataBuf+(len+i)*132, 512);
        }
    }
    return 0; 
}


uint32 SpiBootReadPBA(uint8 ChipSel, uint32 PBA , void *pbuf, uint16 nSec )
{
    uint16 len,i;
    uint32 *pDataBuf = pbuf;
    PBA &= (SPI_BLOCK_SIZE*2-1);

    memset(SpareBuf,0xFF,SPARE_LEN);

    for (len=0; len<nSec; len+=4)
    {
        for (i=0; i<4; i++)
        {
        	SPIFlashRead(((len+PBA)*2+i)*512, pDataBuf+(len+i)*132, 512);//��2KҪ����2K
            ftl_memcpy(pDataBuf+(len+i)*132+128, SpareBuf+(i)*4, 16);
        }
    }
   
    return 0;
}

uint32 SpiBootReadLBA(uint8 ChipSel,  uint32 LBA ,void *pbuf, uint16 nSec)
{
    SPIFlashRead((LBA+SPI_LBA_PART_OFFSET)*512, pbuf,nSec*512);
    return(0);
}

uint32 SpiBootWriteLBA(uint8 ChipSel,  uint32 LBA, void *pbuf , uint16 nSec  ,uint16 mode)
{
    SPIFlashWrite((LBA+SPI_LBA_PART_OFFSET)*512, pbuf,nSec*512);
    return(0);
}
#endif
#endif
