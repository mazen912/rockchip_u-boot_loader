
#include    "../../armlinux/config.h"

#define     DELAY_ARM_FREQ      50
#define     ASM_LOOP_INSTRUCTION_NUM     4
#define     ASM_LOOP_PER_US    (DELAY_ARM_FREQ/ASM_LOOP_INSTRUCTION_NUM) //
 
/***************************************************************************
��������:��ʱ
��ڲ���:us��
���ڲ���:
���ú���:
***************************************************************************/
extern uint32 Timer0Get100ns( void );
void DRVDelayUs(uint32 count)
{
#if 0
    uint32 tmp;
    uint32 TimerEnd = Timer0Get100ns() + count * 13;
    tmp =  ASM_LOOP_PER_US;//15;
    if(tmp)
        tmp *= count;
    else
        tmp = 1;

    while (--tmp) 
    {
        if(Timer0Get100ns() > TimerEnd)
            break;
    }
#else
	__udelay(count);
#endif
}
/***************************************************************************
��������:��ʱ
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
void Delay100cyc(uint16 count)
{
    uint16 i;

    while (count--)
        for (i=0; i<8; i++);
}


/***************************************************************************
��������:��ʱ
��ڲ���:ms��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayMs(uint32 count)
{
    DRVDelayUs(1000*count);
}


/***************************************************************************
��������:��ʱ
��ڲ���:s��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayS(uint32 count)
{
    while (count--)
        DRVDelayMs(1000);
}

uint8  ChipType;
uint32 Rk30ChipVerInfo[4];  
void ChipTypeCheck(void)
{
    Rk30ChipVerInfo[0] = 0;
#if(CONFIG_RKCHIPTYPE == CONFIG_RK3188)
    ftl_memcpy(Rk30ChipVerInfo, (uint8*)(BOOT_ROM_CHIP_VER_ADDR + 0x20000), 16);
#else
    ftl_memcpy(Rk30ChipVerInfo, (uint8*)(BOOT_ROM_CHIP_VER_ADDR), 16);
#endif
    
    ChipType = CHIP_RK3066;
    if(Rk30ChipVerInfo[0]== 0x33303042&&Rk30ChipVerInfo[3] == 0x56313030) 
    {
        ChipType = CHIP_RK3168;
    }
    
    if(Rk30ChipVerInfo[0]== 0x33303041&& Rk30ChipVerInfo[3] == 0x56313030) 
    {
        ChipType = CHIP_RK3066B;
        Rk30ChipVerInfo[0] =  0x33313041; // "310A"
    }

    if(Rk30ChipVerInfo[0]== 0x33313042&& Rk30ChipVerInfo[3] == 0x56313030) 
    {
        ChipType = CHIP_RK3188;
    }
#if(CONFIG_RKCHIPTYPE == CONFIG_RK3188)
    ChipType = CHIP_RK3188;

    if(Rk30ChipVerInfo[0]== 0x33313042&& Rk30ChipVerInfo[3] == 0x56313031) 
    {
        ChipType = CHIP_RK3188B;
    }
	
#endif
}

#include "../../common/rockusb/USB20.h"
void ModifyUsbVidPid(USB_DEVICE_DESCRIPTOR * pDeviceDescr)
{
    if(ChipType == CHIP_RK3066B) 
    {
        pDeviceDescr->idProduct = 0x310A;
        pDeviceDescr->idVendor  = 0x2207;
    }
    else if (ChipType == CHIP_RK3168)
    {
        pDeviceDescr->idProduct = 0x300B;
        pDeviceDescr->idVendor  = 0x2207;
    }
    else if (ChipType == CHIP_RK3188 || ChipType == CHIP_RK3188B)
    {
        pDeviceDescr->idProduct = 0x310B;
        pDeviceDescr->idVendor  = 0x2207;
    }
}

//����Loader�����쳣����
//ϵͳ������ָ����sdramֵΪ�ñ�־���������ɽ���rockusb
//ϵͳ����ʧ�ܱ�־
uint32 IReadLoaderFlag(void)
{
    return (*LOADER_FLAG_REG);
}

void ISetLoaderFlag(uint32 flag)
{
    if(*LOADER_FLAG_REG == flag)
        return;
    *LOADER_FLAG_REG = flag;
}

typedef enum PLL_ID_Tag
{
    APLL=0,
    DPLL,
    CPLL,
    GPLL,
    
    PLL_MAX
}PLL_ID;

#define PLL_RESET  (((0x1<<5)<<16) | (0x1<<5))
#define PLL_DE_RESET  (((0x1<<5)<<16) | (0x0<<5))
#define NR(n)      ((0x3F<<(8+16)) | ((n-1)<<8))
#define NO(n)      ((0x3F<<16) | (n-1))
#define NF(n)      ((0xFFFF<<16) | (n-1))
#define NB(n)      ((0xFFF<<16) | (n-1))

static void APLL_cb(void)
{
    if(ChipType == CHIP_RK3066)
    {
        g_cruReg->CRU_CLKSEL_CON[0] = ((0x1F | (0x3<<6) | (0x1<<8))<<16)
                                                      | (0x0<<8)     //core_clk_src = ARM PLL = 600MHz
                                                      | (0x1<<6)     //clk_core:clk_core_periph = 4:1 = 600MHz : 150MHz
                                                      | 0;           //clk_core = core_clk_src/1 = 600MHz
        g_cruReg->CRU_CLKSEL_CON[1] = (((0x3<<12) | (0x3<<8) | (0x3<<14) | 0x7)<<16)     //clk_core:aclk_cpu = 1:1 = 192MHz : 192 MHz
                                                      | (0x1<<14)    //hclk_cpu:pclken_ahb2apb = 2:1 = 150MHz : 75MHz
                                                      | (0x1<<12)    //aclk_cpu:pclk_cpu = 2:1 = 150MHz : 75MHz
                                                      | (0x0<<8)     //aclk_cpu:hclk_cpu = 1:1 = 150MHz : 150MHz
                                                      | 3;           //clk_core:aclk_cpu = 4:1 = 600MHz : 150MHz
    }
    else
    {
        g_cruReg->CRU_CLKSEL_CON[0] = (((0x1F<<9)|(1<<8)|(0x3<<6)|(1<<5)|(0x1F))<<16)
                                                      | (0x0<<9)     //core_clk : core_clk_src = APLL = 600MHz
                                                      | (0x0<<8)     //core_clk_src = APLL = 600MHz
                                                      | (0x1<<6)     //clk_cpu:clk_core_periph = 4:1 = 600MHz : 150MHz
                                                      | (0x0<<5)     //clk_cpu_src = APLL = 600MHz
                                                      | 1;           //aclk_cpu = core_clk_src/2 = 300MHz
        g_cruReg->CRU_CLKSEL_CON[1] = (((0x3<<14) | (0x3<<12) | (0x3<<8) | (0x7<<3)| 0x7)<<16)     //clk_core:aclk_cpu = 1:1 = 192MHz : 192 MHz
                                                      | (0x1<<14)    //hclk_cpu:pclken_ahb2apb = 2:1 = 150MHz : 75MHz
                                                      | (0x2<<12)    //aclk_cpu:pclk_cpu = 4:1 = 300MHz : 750MHz
                                                      | (0x1<<8)     //aclk_cpu:hclk_cpu = 2:1 = 300MHz : 150MHz
                                                      | (1<<3)       //clk_core:aclk_core = 2:1 = 600MHz : 300MHz
                                                      | 1;           //clk_cpu:aclk_cpu = 1:1 = 300MHz : 300MHz
    }
}

static void GPLL_cb(void)
{
    if(ChipType == CHIP_RK3066)
    {
    	g_cruReg->CRU_CLKSEL_CON[10] = (((0x1<<15)|(0x3<<12)|(0x3<<8)|0x1F)<<16) 
                                                    | (0x0<<15)     //aclk_periph = GPLL/1 = 144MHz
                                                    | (0x3<<12)     //aclk_periph:pclk_periph = 4:1 = 144MHz : 36MHz
                                                    | (0x1<<8)      //aclk_periph:hclk_periph = 1:1 = 144MHz : 144MHz
                                                    | 0x0;   
	
    }
    else if(ChipType == CHIP_RK3188)
	{
    	g_cruReg->CRU_CLKSEL_CON[10] = (((0x1<<15)|(0x3<<12)|(0x3<<8)|0x1F)<<16) 
                                                    | (0x1<<15)     //periph_clk_src = GPLL = 300MHz
                                                    | (0x2<<12)     //aclk_periph:pclk_periph = 4:1 = 300MHz : 75MHz
                                                    | (0x1<<8)      //aclk_periph:hclk_periph = 1:1 = 300MHz : 150MHz
                                                    | 0x0;          //aclk_periph=periph_clk_src/1 = 300Mhz
	}
	else // 3066B 3168
	{
    	g_cruReg->CRU_CLKSEL_CON[10] = (((0x1<<15)|(0x3<<12)|(0x3<<8)|0x1F)<<16) 
                                                    | (0x0<<15)     //periph_clk_src = GPLL = 300MHz
                                                    | (0x2<<12)     //aclk_periph:pclk_periph = 4:1 = 300MHz : 75MHz
                                                    | (0x1<<8)      //aclk_periph:hclk_periph = 2:1 = 300MHz : 150MHz
                                                    | 0x0;          //aclk_periph=periph_clk_src/1 = 300Mhz
	}
}

/*****************************************
NR   NO     NF             Fout(range)
3    8      37.5 - 187.5
4    6      50   - 250    100 - 150
6    4      75   - 375    150 - 250
12   2      150  - 750    250 - 500
24   1      300  - 1500   500 - 1000
******************************************/
//rk 3066b ����С��100Mhz
static void Set_PLL(PLL_ID pll_id, uint32 MHz, pFunc cb)
{
    uint32 nr,no,nf;
    
    MHz += (MHz & 0x1);   //change to even, for NB setting
    g_cruReg->CRU_MODE_CON = (0x3<<((pll_id*4) +  16)) | (0x0<<(pll_id*4));            //PLL slow-mode

    if(ChipType == CHIP_RK3066)
    {
        if(MHz > 500)
        {
            nr = 24;
            no = 1;
        }
        else if(MHz > 250)
        {
            nr = 12;
            no = 2;
        }
        else if(MHz > 150)
        {
            nr = 6;
            no = 4;
        }
        else if(MHz > 100)
        {
            nr = 4;
            no = 6;
        }
        else
        {
            nr = 3;
            no = 8;
        }

        g_cruReg->CRU_PLL_CON[pll_id][3] = PLL_RESET;
        g_cruReg->CRU_PLL_CON[pll_id][0] = NR(nr) | NO(no);
        g_cruReg->CRU_PLL_CON[pll_id][1] = NF(MHz);
        g_cruReg->CRU_PLL_CON[pll_id][2] = NB(MHz);
        DRVDelayUs(1);
        g_cruReg->CRU_PLL_CON[pll_id][3] = PLL_DE_RESET;
    }
    else
    {
        if(MHz >= 600)
        {
            nr = 1;
            no = 2;
            nf = MHz *2 / 24;
        }
        else if(MHz >= 400)
        {
            nr = 1;
            no = 3;
            nf = MHz * 3 / 24;
        }
        else if(MHz >= 250)
        {
            nr = 1;
            no = 5;
            nf = MHz * 5 / 24;
        }
        else if(MHz >= 140)
        {
            nr = 1;
            no = 8;
            nf = MHz * 8 / 24;
        }
        else    
        {
            nr = 1;
            no = 12;
            nf = MHz * 12 / 24;
        }
        g_cruReg->CRU_PLL_CON[pll_id][3] = (((0x1<<1)<<16) | (0x1<<1));
        g_cruReg->CRU_PLL_CON[pll_id][0] = NR(nr) | NO(no);
        g_cruReg->CRU_PLL_CON[pll_id][1] = NF(nf);
        DRVDelayUs(1);
        g_cruReg->CRU_PLL_CON[pll_id][3] = (((0x1<<1)<<16) | (0x0<<1));
    }

    DRVDelayUs(1000); // 1ms��
    if(cb)
    {
        cb();
    }
    g_cruReg->CRU_MODE_CON = (0x3<<((pll_id*4) +  16))  | (0x1<<(pll_id*4));            //PLL normal
}

void SetARMPLL(uint16 nMhz)
{
    if(ChipType != CHIP_RK3188 && ChipType != CHIP_RK3188B)
    {
        Set_PLL(APLL, 600, APLL_cb);
        Set_PLL(GPLL, 300, GPLL_cb); // 3188��Щ��Ƭ������ʹ��GPLL����DPLLʹ��
    }
}

uint32 GetGPLLCLK(void)
{
    uint32 NR,NF,NO;
    uint32 ArmPll;
    pCRU_REG ScuReg=(pCRU_REG)CRU_BASE_ADDR;

    ArmPll =  ScuReg->CRU_PLL_CON[3][0];
    //printf("GetGPLLCLK0 = %x\n",ArmPll);
    NO = (ArmPll&0x3Ful) + 1;
    NR = ((ArmPll >> 8)&0x3Ful) + 1;
    ArmPll =  ScuReg->CRU_PLL_CON[3][1];
    //printf("GetGPLLCLK1 = %x\n",ArmPll);
    NF = (ArmPll&0x1FFFul) + 1;
    //printf("GetGPLLCLK3 = %x\n",ArmPll);
    //ArmPll =  ScuReg->CRU_PLL_CON[3][2];
    ArmPll = 24*NF/(NR*NO);
    //printf("ArmPll = %d\n",ArmPll);
    return ArmPll;
}

uint32 GetAHBCLK(void)
{
    uint32 Div1,Div2;
    uint32 ArmPll;
    uint32 AhbClk;
    pCRU_REG ScuReg=(pCRU_REG)CRU_BASE_ADDR;
    
    ArmPll = GetGPLLCLK();
    AhbClk = ScuReg->CRU_CLKSEL_CON[10];
    Div1 = (AhbClk&0x1F) + 1;
    Div2 = 1<<((AhbClk>>8)&0x3);
    AhbClk = ArmPll/(Div1*Div2);
    //printf("AhbClk = %d\n",AhbClk);
    return AhbClk*1000;
}

uint32 GetMmcCLK(void)
{
    return (GetAHBCLK());
}

void uart2UsbEn(uint8 en)
{
    if(en)
    {
        if((!(g_3066B_grfReg->GRF_SOC_STATUS0) & (1<<10)) && (g_BootRockusb == 0))
        {
            DRVDelayUs(1);
            if(!(g_3066B_grfReg->GRF_SOC_STATUS0) & (1<<10))
            {
                 //g_3066B_grfReg->GRF_UOC0_CON[2] = ((0x01 << 2) | ((0x01 << 2) << 16));  //software control usb phy enable
                 //g_3066B_grfReg->GRF_UOC0_CON[3] = (0x2A | (0x3F << 16));  //usb phy enter suspend
                 g_3066B_grfReg->GRF_UOC0_CON[0] = (0x0300 | (0x0300 << 16)); // uart enable
            }
        }
    }
    else
    {
        g_3066B_grfReg->GRF_UOC0_CON[0] = (0x0000 | (0x0300 << 16));
        g_3066B_grfReg->GRF_UOC0_CON[2] = (0x0000 | (0x0004 << 16));
    }
}



/**************************************************************************
USB PHY RESET
***************************************************************************/
bool UsbPhyReset(void)
{
    if(ChipType == CHIP_RK3188 || ChipType == CHIP_RK3188B)
    {
        uart2UsbEn(0);
        //g_3066B_grfReg->GRF_UOC0_CON[0] = (0x0000 | (0x0300 << 16));
        //g_3066B_grfReg->GRF_UOC0_CON[2] = (0x0000 | (0x0004 << 16));
        //3188 ����Ϊsoftware control usb phy��usbû�нӵ�ʱ�����DiEpDma��DopDma������
       // ddr ��ʼ��ʱ�������������д���
       // g_3066B_grfReg->GRF_UOC0_CON[2] = ((0x01 << 2) | ((0x01 << 2) << 16));  //software control usb phy enable
       // g_3066B_grfReg->GRF_UOC0_CON[3] = (0x2A | (0x3F << 16));  //usb phy enter suspend
       // g_3066B_grfReg->GRF_UOC0_CON[0] = (0x0300 | (0x0300 << 16)); // uart enable
    }
    
    if(ChipType == CHIP_RK3066)
    {
        g_grfReg->GRF_UOC0_CON[2] = (0x0000 | (0x0004 << 16)); //software control usb phy disable
    }
    else
    {
        g_3066B_grfReg->GRF_UOC0_CON[2] = (0x0000 | (0x0004 << 16)); //software control usb phy disable
    }

    DRVDelayUs(1100); //1.1ms
    g_cruReg->CRU_SOFTRST_CON[4] = ((7ul<<5)<<16)|(7<<5);
    DRVDelayUs(10*100);    //delay 10ms
    g_cruReg->CRU_SOFTRST_CON[4] = (uint32)((7ul<<5)<<16)|(0<<5);
    DRVDelayUs(1*100);     //delay 1ms
    return (TRUE);
}

/**************************************************************************
USB PHY RESET
***************************************************************************/
void FlashCsInit(void)
{
    if(ChipType == CHIP_RK3066)
    {
        g_grfReg->GRF_GPIO_IOMUX[3].GPIOD_IOMUX = ((0x3<<14)<<16)|(0x1<<14);  // dqs
        g_grfReg->GRF_GPIO_IOMUX[4].GPIOA_IOMUX = ((0xFFFF)<<16)|0x5555;      // data8-15
        g_grfReg->GRF_GPIO_IOMUX[4].GPIOB_IOMUX = ((0x3FFF)<<16)|0x1555;      // cs1-cs7
        g_grfReg->GRF_SOC_CON[0] = ((0x1<<11)<<16)|(0x0<<11);                 // flash data0-7,wp
    }
    else
    {
        //g_3066B_grfReg->GRF_GPIO_IOMUX[0].GPIOC_IOMUX = ((0xFFFF)<<16)|0x5555;      // data8-15
        g_3066B_grfReg->GRF_GPIO_IOMUX[0].GPIOD_IOMUX = ((0x00FF)<<16)|0x0055;      //dqs cs1-cs3 
        g_3066B_grfReg->GRF_SOC_CON[0] = ((0x1<<11)<<16)|(0x0<<11);                 // flash data0-7,wp
        g_3066B_grfReg->GRF_IO_CON[4] = 0x08000000;  // vcc flash 3.3V 
        g_3066B_grfReg->GRF_IO_CON[0] = 0x000C0008;  // drive_strength_ctrl_0  4ma
    }
}

/**************************************************************************
USB PHY RESET
***************************************************************************/
void SpiGpioInit(void)
{

}

void sdmmcGpioInit(uint32 ChipSel)
{
    if(ChipType == CHIP_RK3066)
    {
        g_grfReg->GRF_GPIO_IOMUX[3].GPIOD_IOMUX = ((0x3<<14)<<16)|(0x2<<14);  // dqs
        g_grfReg->GRF_GPIO_IOMUX[4].GPIOB_IOMUX = ((0xf<<2)<<16)|(0xa<<2);   // cmd,rstn
        g_grfReg->GRF_SOC_CON[0] = ((0x1<<11)<<16)|(0x1<<11);                 // emmc data0-7,wp
    }
    else
    {
        g_3066B_grfReg->GRF_GPIO_IOMUX[0].GPIOD_IOMUX = ((0x00F3)<<16)|0x00A2;      // clk cmd rstn 
        g_3066B_grfReg->GRF_SOC_CON[0] = ((0x1<<11)<<16)|(0x1<<11);                 // emmc data0-7,wp
        g_3066B_grfReg->GRF_IO_CON[4] = 0x08000800;  // vccio0 1.8V 3188����ط�������?????
        g_3066B_grfReg->GRF_IO_CON[0] = 0x000C0004;  // drive_strength_ctrl_0  4ma
    }
}

/***************************************************************************
��������:�ر�TCM
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
void DisableRemap(void)
{
// TODO: Disable Remap
    //clean remap bit in grf enabled, remap 0x0000 to rom, 
    if(ChipType == CHIP_RK3066)
    {
        *(unsigned long volatile *)(GRF_BASE + 0x150) = 0x10000000;
    }
    else
    {
        *(unsigned long volatile *)(GRF_BASE + 0xA0)  = 0x10000000;
    }
}


void FW_NandDeInit(void)
{
#ifdef RK_FLASH_BOOT_EN 
    FlashDeInit();
    FlashTimingCfg(150*1000);
#endif
#ifdef RK_SDMMC_BOOT_EN
    SdmmcDeInit();
#endif
}


/***************************************************************************
��������:ϵͳ��λ
��ڲ���:��
���ڲ���:��
���ú���:��
***************************************************************************/
extern void ResetCpu(unsigned long remap_addr);

void SoftReset(void)
{
    pFunc fp;
    pCRU_REG cruReg=(pCRU_REG)CRU_BASE_ADDR;
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG0_BASE_ADDR ;//USB_OTG_BASE_ADDR

    DisableIRQ();
    UsbPhyReset();
    OtgReg->Device.dctl |= 0x02;          //soft disconnect
    FW_NandDeInit();

    MMUDeinit();              /*�ر�MMU*/
    //cruReg->CRU_MODE_CON = 0x33030000;    //cpu enter slow mode
    //Delay100cyc(10);
    g_giccReg->ICCEOIR=USB_OTG_INT_CH;
    //DisableRemap();
    if(ChipType == CHIP_RK3066)
    {
        ResetCpu((GRF_BASE + 0x150));
    }
    else
    {
        ResetCpu((GRF_BASE + 0xA0));
    }
    //cruReg->CRU_GLB_SRST_FST_VALUE = 0xfdb9; //kernel ʹ�� fst resetʱ��loader�����������⻹û�в飬����loader������snd reset
    cruReg->CRU_GLB_SRST_SND_VALUE = 0xeca8; //soft reset
    Delay100cyc(10);
    while(1);
}

void EmmcPowerEn(uint8 En)
{
// TODO: EMMC ��Դ����

    if(En)
    {
        g_EMMCReg->SDMMC_PWREN = 1;  // power enable
        g_EMMCReg->SDMMC_RST_n = 1;  // reset off
    }
    else
    {
        g_EMMCReg->SDMMC_PWREN = 0;  // power disable
        g_EMMCReg->SDMMC_RST_n = 0;  //reset on
    }
}

void SDCReset(uint32 sdmmcId)
{
    uint32 data = g_cruReg->CRU_SOFTRST_CON[5];
    data = ((1<<16)|(1))<<(sdmmcId + 1);
    g_cruReg->CRU_SOFTRST_CON[5] = data;
    DRVDelayUs(100);
    data = ((1<<16)|(0))<<(sdmmcId + 1);
    g_cruReg->CRU_SOFTRST_CON[5] = data;
    DRVDelayUs(200);
    EmmcPowerEn(1);
}

int32 SCUSelSDClk(uint32 sdmmcId, uint32 div)
{
    if((div == 0))//||(sdmmcId > 1))
    {
        return (-1);
    }
    if(0 == sdmmcId)
    {
        g_cruReg->CRU_CLKSEL_CON[11] = (0x3Ful<<16)|(div-1)<<0;
    }
    else if(1 == sdmmcId)
    {
        g_cruReg->CRU_CLKSEL_CON[12] = (0x3Ful<<16)|(div-1)<<0;
    }
    else    //emmc
    {
        //RkPrintf("SCUSelSDClk 2 %d\n",div);
        g_cruReg->CRU_CLKSEL_CON[12] = (0x3Ful<<24)|(div-1)<<8;
    }
    return(0);
}

//mode=1  changemode to normal mode;
//mode=0  changemode to boot mode
int32 eMMC_changemode(uint8 mode)
{ 
#ifdef RK_SDMMC_BOOT_EN    
    eMMC_SetDataHigh();
#endif
}

/*----------------------------------------------------------------------
Name	: IOMUXSetSDMMC1
Desc	: ����SDMMC1��عܽ�
Params  : type: IOMUX_SDMMC1 ���ó�SDMMC1�ź���
                IOMUX_SDMMC1_OTHER���óɷ�SDMMC1�ź���
Return  : 
Notes   : Ĭ��ʹ��4�ߣ���ʹ��pwr_en, write_prt, detect_n�ź�
----------------------------------------------------------------------*/
void IOMUXSetSDMMC(uint32 sdmmcId,uint32 Bits)
{
// TODO:SDMMC IOMUX 
}


void power_io_ctrl(uint8 mode)
{
    gpio_conf *key_gpio = &key_powerHold.key.gpio;
    if(mode)         // ����ߵ�ƽ
    {
        write_XDATA32((key_gpio->io_write), ReadReg32((key_gpio->io_write))|((1ul<<key_gpio->index))); //out put high
        write_XDATA32(key_gpio->io_dir_conf, (read_XDATA32(key_gpio->io_dir_conf)|((1ul<<key_gpio->index)))); // port6 B0 out
    }
    else                //�����������͵�ƽ
    {
        write_XDATA32((key_gpio->io_write), ReadReg32((key_gpio->io_write))&(~(1ul<<key_gpio->index))); //out put high
        write_XDATA32(key_gpio->io_dir_conf, (read_XDATA32(key_gpio->io_dir_conf)|((1ul<<key_gpio->index)))); // port6 B0 out
    }
}

void powerOn(void)
{
    gpio_conf *key_gpio = &key_powerHold.key.gpio;
    power_io_ctrl((key_gpio->valid)&0x01);
}

void powerOff(void)
{
    gpio_conf *key_gpio = &key_powerHold.key.gpio;
    power_io_ctrl((key_gpio->valid&0x01)==0);
}


#if 0
int test_stuck_address(uint32 *bufa, uint32 count) {
    uint32 *p1 = bufa;
    unsigned int j;
    size_t i;
	RkPrintf("test_stuck_address = %x , %x \n",bufa,count);
    for (j = 0; j < 2; j++) {
        RkPrintf("write data %d\n",j);
        p1 = (uint32 *) bufa;
        for (i = 0; i < count; i++) {
            *p1 = ((j + i) % 2) == 0 ? (uint32) p1 : ~((uint32) p1);
            *p1++;
        }
        RkPrintf("check data %d\n",j);
        p1 = (uint32 *) bufa;
        for (i = 0; i < count; i++, p1++) {
            if (*p1 != (((j + i) % 2) == 0 ? (uint32) p1 : ~((uint32) p1))) {
                {
                    RkPrintf("FAILURE: possible bad address line at offset 0x%x\n",j);
                    while(1);
                }
                RkPrintf("Skipping to next test...\n");
                return -1;
            }
        }
    }
    RkPrintf("Test end");
    return 0;
}

void loader_reset(void)
{
    uint32 loader_flag = IReadLoaderFlag();
    if(loader_flag!=BOOT_LOADER)
    {
        pCRU_REG cruReg=(pCRU_REG)CRU_BASE_ADDR;
        ISetLoaderFlag(BOOT_LOADER);
        DisableRemap();
        DRVDelayMs(10);
        cruReg->CRU_GLB_SRST_SND_VALUE = 0xeca8; //soft reset
    }
}
void loader_reset2(void)
{
    uint32 loader_flag = *(uint32*)0x1008FFF0;//IReadLoaderFlag();
    if(loader_flag!=BOOT_LOADER)
    {
        pCRU_REG cruReg=(pCRU_REG)CRU_BASE_ADDR;
        //ISetLoaderFlag(BOOT_LOADER);
        *(uint32*)0x1008FFF0 = BOOT_LOADER;
        DisableRemap();
        DRVDelayMs(10);
        while(1);
        //cruReg->CRU_GLB_SRST_SND_VALUE = 0xeca8; //soft reset
    }
}
#endif


