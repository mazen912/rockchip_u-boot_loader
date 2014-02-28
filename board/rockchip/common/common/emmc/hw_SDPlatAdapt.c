/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SDPlatAdapt.c
//����:RK28 SD/MMC driver Platform adaptation implement file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SDPlatAdapt.c,v $
Revision 1.5  2011/04/01 08:32:04  Administrator
���emmc ��ʼ���������

Revision 1.4  2011/03/29 09:24:55  Administrator
*** empty log message ***

Revision 1.3  2011/03/08 08:37:28  Administrator
��������֧��
���emmc ��ʼ���������
���delay��ʱ��׼������

Revision 1.2  2011/01/26 09:37:39  Administrator
*** empty log message ***

Revision 1.1  2011/01/18 07:20:31  Administrator
*** empty log message ***

Revision 1.1  2011/01/07 11:55:40  Administrator
*** empty log message ***

Revision 1.1.1.1  2010/05/17 03:44:52  hds
20100517 �Ƶ�ʤ�ύ��ʼ�汾

Revision 1.1.1.1  2010/03/06 05:22:59  zjd
2010.3.6�ɻƵ�ʤ�ύ��ʼ�汾

Revision 1.1.1.1  2009/12/15 01:46:31  zjd
20091215 �������ύ��ʼ�汾

Revision 1.2  2009/10/31 02:21:17  hcy
hcy 09-10-31 ����ÿ������������ʱ����֧���Ȱβ�

Revision 1.6  2009/10/13 08:08:28  hcy
hcy 09-10-13 SD���������£��ĳ�3�ֿ���ⷽʽ���Ż������������

Revision 1.2  2009/08/18 09:42:06  YYZ
no message

Revision 1.1.1.1  2009/08/18 06:43:27  Administrator
no message

Revision 1.1.1.1  2009/08/14 08:02:01  Administrator
no message

Revision 1.4  2009/05/07 12:12:07  hcy
hcy ʹ��С���������Ȱβ壬�����׳��ֶ������ļ�������

Revision 1.3  2009/04/29 10:35:48  fzf
ȥ�� SD POWER IO �����ã�

Revision 1.2  2009/04/02 03:18:08  hcy
�����͵�Դ��ĵ�Hw_define.h��

Revision 1.1.1.1  2009/03/16 01:34:07  zjd
20090316 ��ѵ���ṩ��ʼSDK�汾

Revision 1.4  2009/03/13 01:44:44  hcy
�����Ϳ���Դ����ĳ�GPIO��ʽ

Revision 1.3  2009/03/07 07:30:18  yk
(yk)����SCUģ���Ƶ�����ã�������к��������룬���³�ʼ�����ã�
����ң�������룬ɾ��FPGA_BOARD�ꡣ
(hcy)SDRAM�����ĳ�28��

Revision 1.2  2009/03/05 12:37:16  hxy
���CVS�汾�Զ�ע��

****************************************************************/
#include    "sdmmc_config.h"

#if (eMMC_PROJECT_LINUX) 
#include <linux/dma-mapping.h>
#include <asm/dma.h>
#endif

#ifdef DRIVERS_SDMMC

/****************************************************************/
//������:SDPAM_FlushCache
//����:���cache
//����˵��:adr      �������     ��Ҫ�������ʼ��ַ
//         size     �������     ��Ҫ����Ĵ�С����λ�ֽ�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_FlushCache(void *adr, uint32 size)
{
    //MMFlushCache(BOTHCACHE, CACREGION, adr, size);
}

/****************************************************************/
//������:SDPAM_CleanCache
//����:����cache
//����˵��:adr      �������     ��Ҫ�������ʼ��ַ
//         size     �������     ��Ҫ����Ĵ�С����λ�ֽ�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_CleanCache(void *adr, uint32 size)
{
    //MMCleanDCache(CACREGION, adr, size);
}

/****************************************************************/
//������:SDPAM_GetAHBFreq
//����:�õ���ǰAHB����Ƶ��
//����˵��:
//����ֵ:���ص�ǰAHB����Ƶ�ʣ���λKHz
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
uint32 SDPAM_GetAHBFreq(void)
{
#if SDMMC_NO_PLATFORM
    return 25000;
#else
    return GetMmcCLK();//PLLGetAHBFreq();
#endif
}

/****************************************************************/
//������:SDPAM_SDCClkEnable
//����:ѡ���Ƿ���SDMMC�������Ĺ���ʱ��
//����˵��:nSDCPort   �������   �˿ں�
//         enable     �������   �Ƿ�ʹ��
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_SDCClkEnable(SDMMC_PORT_E nSDCPort, bool enable)
{
/*    eCLK_GATE gate;

    //gate = (nSDCPort == SDC0) ? CLK_GATE_SDMMC0 : CLK_GATE_SDMMC1;
    if(nSDCPort == SDC0)
    {
        gate = CLK_GATE_SDMMC0;
    }
    else if(nSDCPort == SDC1)
    {
        gate = CLK_GATE_SDMMC1;
    }
    else
    {
        gate = CLK_GATE_EMMC; //eMMC controller
    }
    
    if(enable)
    {
        #if ((SDMMC0_DET_MODE == SD_GPIO_DET) || (SDMMC0_DET_MODE == SD_ALWAYS_PRESENT) )  ///eMMC ��ô������?
        SCUEnableClk(gate);
        #endif
    }
    else
    {
        #if ((SDMMC0_DET_MODE == SD_GPIO_DET) || (SDMMC0_DET_MODE == SD_ALWAYS_PRESENT))
        SCUDisableClk(gate);
        #endif
    }
*/
}

/****************************************************************/
//������:SDPAM_SDCReset
//����:��SCU�ϸ�λSDMMC������
//����˵��:nSDCPort   �������   �˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_SDCReset(SDMMC_PORT_E nSDCPort)
{
    if(nSDCPort == SDC0)
    {
        SDCReset(0);
    }
    else if (nSDCPort == SDC1)
    {
        SDCReset(1);
    }
    else
    {
        SDCReset(2);
    }
}

/****************************************************************/
//������:SDPAM_SetMmcClkDiv
//����:����SCU��mmc_clk_div�ķ�Ƶֵ
//����˵��:nSDCPort   �������   �˿ں�
//         div        �������   ��Ƶֵ
//����ֵ:���ص�ǰAHB����Ƶ�ʣ���λKHz
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_SetMmcClkDiv(SDMMC_PORT_E nSDCPort, uint32 div)
{
#if SDMMC_NO_PLATFORM
    return;
#else
    if(nSDCPort == SDC0)
    {
        SCUSelSDClk(0, div);
    }
    else if (nSDCPort == SDC1)
    {
        SCUSelSDClk(1, div);
    }
    else
    {
        SCUSelSDClk(2, div);
    }
#endif
}
#if EN_SD_DMA
/****************************************************************/
//������:SDPAM_DMAStart
//����:����һ��DMA����
//����˵��:nSDCPort   �������   ��Ҫ���ݴ���Ķ˿ں�
//         dstAddr    �������   Ŀ���ַ
//         srcAddr    �������   Դ��ַ
//         size       �������   ���ݳ��ȣ���λ�ֽ�
//         rw         �������   ��ʾ������Ҫ�ӿ���������д������1:д������0:�ӿ�����
//         CallBack   �������   DMA������Ļص�����
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_DMAStart(SDMMC_PORT_E nSDCPort, uint32 dstAddr, uint32 srcAddr, uint32 size, bool rw, pFunc CallBack)
{
#if SDMMC_NO_PLATFORM
    return TRUE;
#else

#if eMMC_PROJECT_LINUX
    uint32 mode;
    int ret;
    
    uint8 *buf1;

    if(nSDCPort == SDC0)
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }
    else if(nSDCPort == SDC1)
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }
    else
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }

    eMMC_printk(5, "%s..%d   Before call dma_map_single.......\n",__FUNCTION__, __LINE__);
    if(rw)
    {
        eMMC_host->dmabuf= dma_map_single(NULL, (void*)srcAddr,size<<2, DMA_TO_DEVICE);
        if( eMMC_host->dmabuf <= 0)
        {
            printk("%s..%d   run dma_map_single fail!!!!\n",__FUNCTION__, __LINE__);
            BUG_ON(1);
            return FALSE;
        }
        
    }
    else
    {
         eMMC_host->dmabuf = dma_map_single(NULL, (void*)dstAddr, size<<2, DMA_FROM_DEVICE);
        if( eMMC_host->dmabuf <= 0)
        {
            printk("%s..%d   run dma_map_single fail!!!!\n",__FUNCTION__, __LINE__);
            BUG_ON(1);
            return FALSE;
        }
              
    }

    eMMC_host->dmalen = (size<<2);
    
    eMMC_printk(5,"%s..%d   After  call dma_map_single,  dmalen=%x, dma_addr=%x, dmabuf=%x.......\n",__FUNCTION__, __LINE__, eMMC_host->dmalen, eMMC_host->dma_addr, eMMC_host->dmabuf);
    rk29_dma_devconfig(eMMC_host->dma_chn, mode, (unsigned long )(eMMC_host->dma_addr));

    
    eMMC_printk(5,"%s..%d   Before  call rk29_dma_enqueue.......\n",__FUNCTION__, __LINE__);
    if(rw)
    {        
        
         ret = rk29_dma_enqueue(eMMC_host->dma_chn, (void *)eMMC_host,  eMMC_host->dmabuf,size<<2);
         buf1 = (uint8 *)srcAddr;

         //dma_unmap_single(NULL,   eMMC_host->dmabuf, size<<9, DMA_TO_DEVICE);

    }
    else
    {
        eMMC_printk(3,"%s...%d....=====  use DMA for read===========\n",__FUNCTION__,__LINE__, ret);
          
         ret = rk29_dma_enqueue(eMMC_host->dma_chn, (void *)eMMC_host,  eMMC_host->dmabuf, size<<2);
         buf1 = (uint8 *)dstAddr;
         
         //dma_unmap_single(NULL,   eMMC_host->dmabuf, size<<9, DMA_FROM_DEVICE);

    }
    
    eMMC_printk(3,"%s...%d.....After rk29_dma_enqueue, ret =%x \n",__FUNCTION__,__LINE__, ret);
    
    eMMC_printk(3,"%s...%d.....==============DMA config ================\n",__FUNCTION__,__LINE__);
    eMMC_printk(3, " use dma;  dma_chn=%d,  dma-addr=%x,  \n ",eMMC_host->dma_chn,eMMC_host->dma_addr);
    eMMC_printk(3, " Originbufaddr=%x, DMAbuf=%x,  size=%x, direction=%d (0--RK29_DMASRC_MEM;1--RK29_DMASRC_HW) \n ", buf1,  eMMC_host->dmabuf, size<<2, mode);
    eMMC_printk(3,"===========================================================================\n");

    if(ret<0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }


#else    
    eDMA_MODE       mode;

    if(nSDCPort == SDC0)
    {
        if(rw)
        {
            mode = DMA_PERI_SDMMC_TX;
        }
        else
        {
            mode = DMA_PERI_SDMMC_RX;
        }
    }
    else if(nSDCPort == SDC1)
    {
        if(rw)
        {
            mode = DMA_PERI_SDIO_TX;
        }
        else
        {
            mode = DMA_PERI_SDIO_RX;
        }
    }
    else
    {
        if(rw)
        {
            mode = DMA_PERI_EMMC_TX;
        }
        else
        {
            mode = DMA_PERI_EMMC_RX;
        }
    }

    if(DMAOK == DMAStart(dstAddr, srcAddr, size, mode, CallBack))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
#endif

#endif    
}

/****************************************************************/
//������:SDPAM_DMAStop
//����:ֹͣһ���Ѿ����ù���DMA����
//����˵��:nSDCPort   �������   ��Ҫֹͣ�Ķ˿ں�
//         rw         �������   ��ʾֹͣ��������Ҫ�ӿ������Ĳ�������д�����Ĳ�����1:д������0:�ӿ�����
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_DMAStop(SDMMC_PORT_E nSDCPort, bool rw)
{
#if SDMMC_NO_PLATFORM
    return TRUE;
#else
#if eMMC_PROJECT_LINUX
    uint32 mode;
    int ret;

    if(nSDCPort == SDC0)
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }
    else if(nSDCPort == SDC1)
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }
    else
    {
        if(rw)
        {
            mode = RK29_DMASRC_MEM;
        }
        else
        {
            mode = RK29_DMASRC_HW;
        }
    }
    
    //printk("%s..%d  ======= Stop the DMA; begin to call dma_unmap_single()=================\n",__FUNCTION__, __LINE__);
    if(rw)
    {
        
        dma_unmap_single(NULL,  eMMC_host->dmabuf, eMMC_host->dmalen, DMA_TO_DEVICE);
    }
    else
    {
        dma_unmap_single(NULL,  eMMC_host->dmabuf, eMMC_host->dmalen, DMA_FROM_DEVICE);
    }
   
    //printk("%s..%d  ======= Stop the DMA; begin to call rk29_dma_ctrl()=================\n",__FUNCTION__, __LINE__);
    ret = rk29_dma_ctrl(eMMC_host->dma_chn,RK29_DMAOP_STOP);
    if(ret<0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
       
    
#else
    eDMA_MODE       mode;
    if(nSDCPort == SDC0)
    {
        if(rw)
        {
            mode = DMA_PERI_SDMMC_TX;
        }
        else
        {
            mode = DMA_PERI_SDMMC_RX;
        }
    }
    else if(nSDCPort == SDC1)
    {
        if(rw)
        {
            mode = DMA_PERI_SDIO_TX;
        }
        else
        {
            mode = DMA_PERI_SDIO_RX;
        }
    }
    else
    {
        if(rw)
        {
            mode = DMA_PERI_EMMC_TX;
        }
        else
        {
            mode = DMA_PERI_EMMC_RX;
        }
    }

#endif

    return TRUE;
#endif    
}
#endif
/****************************************************************/
//������:SDPAM_INTCRegISR
//����:���жϿ�����ע��ĳ���˿ڵ��жϷ����߳�
//����˵��:nSDCPort   �������   ��Ҫע��Ķ˿ں�
//         Routine    �������   �����߳�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_INTCRegISR(SDMMC_PORT_E nSDCPort, pFunc Routine)
{
#if SDMMC_NO_PLATFORM
    return TRUE;
#else
    return TRUE;
#endif
}

/****************************************************************/
//������:SDPAM_INTCEnableIRQ
//����:ʹ���жϿ�������ĳ�˿ڵ��ж�
//����˵��:nSDCPort   �������   ��Ҫʹ�ܵĶ˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_INTCEnableIRQ(SDMMC_PORT_E nSDCPort)
{
#if eMMC_PROJECT_LINUX
    return TRUE;
#else
    uint32 ret = 0;
    
    if(nSDCPort == SDC0)
    {
        ret = IRQEnable(INT_SDMMC);//ret = IRQEnable(IRQ_SDMMC0);
    }
    else if(nSDCPort == SDC1)
    {
        ret = IRQEnable(INT_SDIO);//ret = IRQEnable(IRQ_SDMMC1);
    }
    else
    {
        ret = IRQEnable(INT_eMMC);
    }

    if(ret == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
}

#if 0
void IOMUXSetSDMMC2(eIOMUX_SDMMC type)
{    
    int value;
    pGRF_REG reg=(pGRF_REG)RK29_GRF_REG_BASE;
   
    switch(type)
    {
        case IOMUX_SDMMC_1BIT:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value &= ~(0x3fL<<22);   //data3/data2/data1            
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            value |= (0x15L<<16);
            reg->GRF_GPIO3L_IOMUX = value;
            
            value = reg->GRF_GPIO3H_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO3H_IOMUX = value;            
            break;
        case IOMUX_SDMMC_4BIT:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0xFFFL<<16);
            value |= (0x555L<<16);//clk cmd data0~3
            reg->GRF_GPIO3L_IOMUX = value;
            value = reg->GRF_GPIO3H_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO3H_IOMUX = value;   
            break;
        case IOMUX_SDMMC_8BIT:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0xFFFFL<<16);
            value |= (0x5555L<<16);//clk cmd data0~5
            reg->GRF_GPIO3L_IOMUX = value;
            value = reg->GRF_GPIO3H_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            value |= (0x5L<<0);
            reg->GRF_GPIO3H_IOMUX = value;  
            break;
        case IOMUX_SDMMC_OTHER:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            reg->GRF_GPIO3L_IOMUX = value;
            
            value = reg->GRF_GPIO3H_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO3H_IOMUX = value;  

            break;
        default:
            break;        
    }
}


/*----------------------------------------------------------------------
Name	: IOMUXSetSDMMC1
Desc	: ����SDMMC1��عܽ�
Params  : type: IOMUX_SDMMC1 ���ó�SDMMC1�ź���
                IOMUX_SDMMC1_OTHER���óɷ�SDMMC1�ź���
Return  : 
Notes   : Ĭ��ʹ��4�ߣ���ʹ��pwr_en, write_prt, detect_n�ź�
----------------------------------------------------------------------*/
void IOMUXSetSDMMC1(eIOMUX_SDMMC type)
{
    int value;
    pGRF_REG reg=(pGRF_REG)RK29_GRF_REG_BASE;
   
    switch(type)
    {
        case IOMUX_SDMMC_1BIT:
            value = reg->GRF_GPIO1H_IOMUX;
            value &= ~(0x3fL<<8);   //data3/data2/data1            
            value &= ~(0xfL<<4);    //data0/cmd
            value |= (0x5L<<4);
            value &= ~(0x3L<<14);    //clk
            value |= (0x1L<<14);            
            reg->GRF_GPIO1H_IOMUX = value;        
            
            break;
        case IOMUX_SDMMC_4BIT:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value |= (0x15L<<22);
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            value |= (0x15L<<16);
            reg->GRF_GPIO3L_IOMUX = value;

            break;
        case IOMUX_SDMMC_8BIT:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            reg->GRF_GPIO3L_IOMUX = value;


            break;
        case IOMUX_SDMMC_OTHER:
            value = reg->GRF_GPIO3L_IOMUX;
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            reg->GRF_GPIO3L_IOMUX = value;


            break;
        default:
            break;        
    }

}

/*----------------------------------------------------------------------
Name	: IOMUXSetSDMMC0
Desc	: ����SDMMC0��عܽ�
Params  : type: IOMUX_SDMMC0 ���ó�SDMMC0�ź���
                IOMUX_SDMMC0_OTHER���óɷ�SDMMC0�ź���
Return  : 
Notes   : Ĭ��ʹ��4�ߣ���ʹ��pwr_en, write_prt, detect_n�ź�
----------------------------------------------------------------------*/
void IOMUXSetSDMMC0(eIOMUX_SDMMC type)
{    
    int value;
    pGRF_REG reg=(pGRF_REG)RK29_GRF_REG_BASE;
   
    switch(type)
    {
        case IOMUX_SDMMC_1BIT:
            value = reg->GRF_GPIO1H_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value &= ~(0x3fL<<22);   //data3/data2/data1            
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            value |= (0x15L<<16);
            reg->GRF_GPIO1H_IOMUX = value;
            
            value = reg->GRF_GPIO2L_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO2L_IOMUX = value;            
            
            break;
        case IOMUX_SDMMC_4BIT:
            value = reg->GRF_GPIO1H_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value |= (0x15L<<22);
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            value |= (0x15L<<16);
            reg->GRF_GPIO1H_IOMUX = value;
            
            value = reg->GRF_GPIO2L_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO2L_IOMUX = value;   

            break;
        case IOMUX_SDMMC_8BIT:
            value = reg->GRF_GPIO1H_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value |= (0x5L<<28);
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value |= (0x15L<<22);
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            value |= (0x15L<<16);
            reg->GRF_GPIO1H_IOMUX = value;
            
            value = reg->GRF_GPIO2L_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            value |= (0x5L<<28);
            reg->GRF_GPIO2L_IOMUX = value;  

            break;
        case IOMUX_SDMMC_OTHER:
            value = reg->GRF_GPIO1H_IOMUX;
            value &= ~(0x0fL<<28);   //data4/data5
            value &= ~(0x3fL<<22);   //data3/data2/data1
            value &= ~(0x3fL<<16);  //data0/cmd/clk
            reg->GRF_GPIO1H_IOMUX = value;
            
            value = reg->GRF_GPIO2L_IOMUX;
            value &= ~(0x0fL<<0);  //data6/data7
            reg->GRF_GPIO2L_IOMUX = value;  

            break;
        default:
            break;        
    }

}
#endif

/****************************************************************/
//������:SDPAM_IOMUX_SetSDPort
//����:��IO���õ�ĳ���˿ڣ����Ҹö˿ڵ������߿����widthָ��
//����˵��:nSDCPort   �������   �˿ں�
//         width      �������   �����߿��
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_IOMUX_SetSDPort(SDMMC_PORT_E nSDCPort, HOST_BUS_WIDTH_E width)
{
    uint32 Bits = 1;

    if (((nSDCPort == SDC1) && (width == BUS_WIDTH_8_BIT)) || (width == BUS_WIDTH_INVALID))
    {
        return FALSE;
    }
    
    switch(width)
    {
        case BUS_WIDTH_1_BIT:
            Bits = 1;
            break;
        case BUS_WIDTH_4_BIT:
            Bits = 4;
            break;
        case BUS_WIDTH_8_BIT:
            Bits = 8;
            break;
        default:
            return FALSE;
    }
    
    if(nSDCPort == SDC0)
    {
        IOMUXSetSDMMC(0,Bits);
    }
    else if (nSDCPort == SDC1)
    {
        IOMUXSetSDMMC(1,Bits);
    }
    else
    {
        IOMUXSetSDMMC(2,Bits);
    }

    return TRUE;
}

/****************************************************************/
//������:SDPAM_IOMUX_PwrEnGPIO
//����:���˿ڵĵ�Դ���Ƹ���ΪGPIO
//����˵��:nSDCPort   �������   �˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_IOMUX_PwrEnGPIO(SDMMC_PORT_E nSDCPort)
{
    if(nSDCPort == SDC0)
    {
        #if SDMMC0_EN_POWER_CTL
        IOMUXSetSMCS1(IOMUX_SMCS1_GPIO);
        GPIOSetPinDirection(SDMMC0_POWER_PIN, GPIO_OUT);
        #endif
    }
    else
    {
        #if SDMMC1_EN_POWER_CTL
        //IOMUX�Ȳ�����SDMMC1��
        #endif
    }
    return TRUE;
}

#if (SDMMC0_DET_MODE == SD_GPIO_DET)
static void sdmmc0_det_Handler(void)
{
    eGPIOIntType_t intType;
    
    if(SDMMC0_DETECT_ACTIVE_LEVEL == GPIOGetPinLevel(SDMMC0_DETECT_PIN))
    {
        SDOAM_SendMsg(MSG_CARD_INSERT, SDC0);
        intType = ((SDMMC0_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelHigh : GPIOLevelLow);
        GPIOIRQRegISR(SDMMC0_DETECT_PIN, sdmmc0_det_Handler, intType);
    }
    else
    {
        SDOAM_SendMsg(MSG_CARD_REMOVE, SDC0);
        intType = ((SDMMC0_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelLow : GPIOLevelHigh);
        GPIOIRQRegISR(SDMMC0_DETECT_PIN, sdmmc0_det_Handler, intType);
    }
}
#endif

#if (SDMMC1_DET_MODE == SD_GPIO_DET)
#if 0
static void sdmmc1_det_Handler(void)
{
    eGPIOIntType_t intType;
    
    if(SDMMC1_DETECT_ACTIVE_LEVEL == GPIOGetPinLevel(SDMMC1_DETECT_PIN))
    {
        SDOAM_SendMsg(MSG_CARD_INSERT, SDC1);
        intType = ((SDMMC1_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelHigh : GPIOLevelLow);
        GPIOIRQRegISR(SDMMC1_DETECT_PIN, sdmmc1_det_Handler, intType);
    }
    else
    {
        SDOAM_SendMsg(MSG_CARD_REMOVE, SDC1);
        intType = ((SDMMC1_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelLow : GPIOLevelHigh);
        GPIOIRQRegISR(SDMMC1_DETECT_PIN, sdmmc1_det_Handler, intType);
    }
 
}
#endif  
#endif

/****************************************************************/
//������:SDPAM_IOMUX_DetGPIO
//����:���˿ڵĿ���⸴��ΪGPIO
//����˵��:nSDCPort   �������   �˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool   SDPAM_IOMUX_DetGPIO(SDMMC_PORT_E nSDCPort)
{  
    #if 0
    eGPIOIntType_t intType;
    
    if(nSDCPort == SDC0)
    {
        #if (SDMMC0_DET_MODE == SD_GPIO_DET)
        //detect GPIO��IOMUX�����������������������
        GPIOSetPinDirection(SDMMC0_DETECT_PIN, GPIO_IN);
        if(SDMMC0_DETECT_ACTIVE_LEVEL == GPIOGetPinLevel(SDMMC0_DETECT_PIN))
        {
            intType = ((SDMMC0_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelHigh : GPIOLevelLow);
        }
        else
        {
            intType = ((SDMMC0_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelLow : GPIOLevelHigh);
        }
        GPIOIRQRegISR(SDMMC0_DETECT_PIN, sdmmc0_det_Handler, intType);
        GPIOEnableIntr(SDMMC0_DETECT_PIN);
        #endif
    }
    else
    {
        #if (SDMMC1_DET_MODE == SD_GPIO_DET)
        //detect GPIO��IOMUX�����������������������
        GPIOSetPinDirection(SDMMC1_DETECT_PIN, GPIO_IN);
        if(SDMMC1_DETECT_ACTIVE_LEVEL == GPIOGetPinLevel(SDMMC1_DETECT_PIN))
        {
            intType = ((SDMMC1_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelHigh : GPIOLevelLow);
        }
        else
        {
            intType = ((SDMMC1_DETECT_ACTIVE_LEVEL == GPIO_LOW) ? GPIOLevelLow : GPIOLevelHigh);
        }
        GPIOIRQRegISR(SDMMC1_DETECT_PIN, sdmmc1_det_Handler, intType);
        GPIOEnableIntr(SDMMC1_DETECT_PIN);
        #endif
    }
    #endif
    
    return TRUE;
}

/****************************************************************/
//������:SDPAM_ControlPower
//����:����ָ���˿ڵ�card��Դ������ر�
//����˵��:nSDCPort �������   �˿ں�
//         enable   �������   1:������Դ��0:�رյ�Դ
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_ControlPower(SDMMC_PORT_E nSDCPort, bool enable)
{
#if SDMMC0_EN_POWER_CTL
    eGPIOPinLevel_t  level;
#endif
    if(nSDCPort == SDC0)
    {
        #if SDMMC0_EN_POWER_CTL
        level = enable ? SDMMC0_POWER_ACTIVE_LEVEL : ((SDMMC0_POWER_ACTIVE_LEVEL == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW);
        GPIOSetPinLevel(SDMMC0_POWER_PIN, level);
        #endif
    }
    else if(nSDCPort == SDC1)
    {   
        #if SDMMC1_EN_POWER_CTL
        level = enable ? SDMMC0_POWER_ACTIVE_LEVEL : ((SDMMC0_POWER_ACTIVE_LEVEL == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW);
        //IOMUX�Ȳ�����SDMMC1��
        #endif
    }
    else
    {
        #if SDMMC2_EN_POWER_CTL
        //level = enable ? SDMMC0_POWER_ACTIVE_LEVEL : ((SDMMC0_POWER_ACTIVE_LEVEL == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW);
        //GPIOSetPinLevel(SDMMC0_POWER_PIN, level);
        #endif
    }
    
}

/****************************************************************/
//������:SDPAM_IsCardPresence
//����:���˿��ϵĿ��Ƿ��ڿ�����
//����˵��:nSDCPort �������   �˿ں�
//����ֵ: TRUE       ���ڿ�����
//        FALSE      �����ڿ�����
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool SDPAM_IsCardPresence(SDMMC_PORT_E nSDCPort)
{
    if(nSDCPort == SDC0)
    {
        #if (SDMMC0_DET_MODE == SD_GPIO_DET)
        if(SDMMC0_DETECT_ACTIVE_LEVEL == GPIOGetPinLevel(SDMMC0_DETECT_PIN))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
        #else
        return TRUE;
        #endif
    }
    else
    {
        #if (SDMMC1_DET_MODE == SD_GPIO_DET)
        //IOMUX�Ȳ�����SDMMC1��
        return TRUE;
        #else
        return TRUE;
        #endif
    }
}

#endif //end of #ifdef DRIVERS_SDMMC
