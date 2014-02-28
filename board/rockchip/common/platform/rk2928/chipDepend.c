
#include    "config.h"

#define     DELAY_ARM_FREQ      50
#define     ASM_LOOP_INSTRUCTION_NUM     4
#define     ASM_LOOP_PER_US    (DELAY_ARM_FREQ/ASM_LOOP_INSTRUCTION_NUM) //

/***************************************************************************
��������:��ʱ
��ڲ���:cycle��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayCyc(uint32 count)
{
    count/=4;           //��Ϊÿ��whileѭ����Ҫ4��CYC, ����MEM��0�ȴ�
    while (count--)
        ;
}

/***************************************************************************
��������:��ʱ
��ڲ���:us��
���ڲ���:
���ú���:
***************************************************************************/
extern uint32 Timer0Get100ns( void );
void DRVDelayUs(uint32 count)
{
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
    while (count--)
        DRVDelayUs(1000);
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

//����Loader�����쳣����
//ϵͳ������ָ����sdramֵΪ�ñ�־���������ɽ���rockusb
//ϵͳ����ʧ�ܱ�־
uint32 IReadLoaderFlag(void)
{
    uint32 reg;
    reg = ((*LOADER_FLAG_REG_L) & 0xFFFFuL) | (((*LOADER_FLAG_REG_H) & 0xFFFFuL)<<16);
    return (reg);
}

void ISetLoaderFlag(uint32 flag)
{
    uint32 reg;
    reg = ((*LOADER_FLAG_REG_L) & 0xFFFFuL) | (((*LOADER_FLAG_REG_H) & 0xFFFFuL)<<16);
    if(reg == flag)
        return;
    (*LOADER_FLAG_REG_L) = 0xFFFF0000 | (flag & 0xFFFFuL);
    (*LOADER_FLAG_REG_H) = 0xFFFF0000 | ((flag >>16) & 0xFFFFuL);
}

uint32 cpuFreq;
void SetFreqFlag(uint32 freq)
{
    cpuFreq = freq;
}


typedef enum PLL_ID_Tag
{
    APLL=0,
    DPLL,
    CPLL,
    GPLL,
    
    PLL_MAX
}PLL_ID;

/*#define PLL_RESET  (((0x1<<5)<<16) | (0x1<<5))
#define PLL_DE_RESET  (((0x1<<5)<<16) | (0x0<<5))
#define NR(n)      ((0x3F<<(8+16)) | ((n-1)<<8))
#define NO(n)      ((0xF<<16) | (n-1))
#define NF(n)      ((0x1FFF<<16) | (n-1))
#define NB(n)      ((0xFFF<<16) | (n-1))
*/
#define PB(n)         ((0x1<<(15+16)) | ((n)<<15))
#define POSTDIV1(n)   ((0x7<<(12+16)) | ((n)<<12))
#define FBDIV(n)      ((0xFFF<<16) | (n))

#define RSTMODE(n)    ((0x1<<(15+16)) | ((n)<<15))
#define RST(n)        ((0x1<<(14+16)) | ((n)<<14))
#define PD(n)         ((0x1<<(13+16)) | ((n)<<13))
#define DSMPD(n)      ((0x1<<(12+16)) | ((n)<<12))
#define LOCK(n)       (((n)>>10)&0x1)
#define POSTDIV2(n)   ((0x7<<(6+16)) | ((n)<<6))
#define REFDIV(n)     ((0x3F<<16) | (n))

static void APLL_cb(void)
{
    g_cruReg->CRU_CLKSEL_CON[0] = (((0x1<<13)|(0x1f<<8)|(0x1<<7)|0x1f)<<16)
                                | (0x0<<13) // aclk cpu select ARM PLL
                                | (0x1<<8)  // APLL:aclk_cpu = 2:1
                                | (0x0<<7)  // core clock select ARM PLL
                                | (0x0);    // APLL:clk_core = 1:1
    g_cruReg->CRU_CLKSEL_CON[1] = (((0x1<<15)|(0x7<<12)|(0x3<<8)|(0x1<<4)|0xf)<<16)
                                | (0x0<<15)     // clk_core:clk_l2c = 1:1
                                | (0x3<<12)     // aclk_cpu:pclk_cpu = 4:1
                                | (0x1<<8)      // aclk_cpu:hclk_cpu = 2:1
                                | (0x1<<4)      // clk_l2c:aclk_core = 2:1
                                | (0x3);        // clk_core:clk_core_periph = 4:1 
}

static void GPLL_cb(void)
{
    g_cruReg->CRU_CLKSEL_CON[10] = (((0x1<<15)|(0x3<<12)|(0x3<<8)|0x1F)<<16) 
                                | (0x0<<15)     //aclk_periph = GPLL/1 = 300MHz
                                | (0x2<<12)     //aclk_periph:pclk_periph = 4:1 = 300MHz : 75MHz
                                | (0x1<<8)      //aclk_periph:hclk_periph = 2:1 = 300MHz : 150MHz
                                | (0x0);       // GPLL:aclk_periph = 1:1  

    g_cruReg->CRU_CLKSEL_CON[25] = ((0x7F)<<16) | (0x1);       // GPLL:spi = 2:1  
                                
}

/*static void DPLL_cb(void)
{ 
    g_cruReg->CRU_CLKSEL_CON[26] = ((0x3 | (0x1<<8))<<16)
                                                  | (0x0<<8)     //clk_ddr_src = DDR PLL
                                                  | 0;           //clk_ddr_src:clk_ddrphy = 1:1 
}*/

/*****************************************
REFDIV   FBDIV     POSTDIV1/POSTDIV2      FOUTPOSTDIV           freq Step     finally use
1        17 - 66   1-49                   8MHz  - 1600MHz                 
=========================================================================================
1        17 - 66   8                      50MHz  - 200MHz          3MHz          50MHz   <= 150MHz
1        17 - 66   7                      57MHz  - 228MHz          3.42MHz
1        17 - 66   6                      66MHz  - 266MHz          4MHz          150MHz  <= 200MHz
1        17 - 66   5                      80MHz  - 320MHz          4.8MHz
1        17 - 66   4                      100MHz - 400MHz          6MHz          200MHz  <= 300MHz
1        17 - 66   3                      133MHz - 533MHz          8MHz          
1        17 - 66   2                      200MHz - 800MHz          12MHz         300MHz  <= 600MHz
1        17 - 66   1                      400MHz - 1600MHz         24MHz         600MHz  <= 1200MHz
******************************************/
static void Set_PLL(PLL_ID pll_id, uint32 MHz, pFunc cb)
{
    uint32 refdiv,postdiv1,fbdiv;  //FREFԽ��VCOԽ��jitter�ͻ�С
    int delay = 1000;
    
    if(MHz <= 300)
    {
        postdiv1 = 2;
    }
    else if(MHz <= 600)
    {
        postdiv1 = 2;
    }
    else
    {
        postdiv1 = 1;
    }
    refdiv = 6;
    fbdiv=(MHz*refdiv*postdiv1)/24;
    
    g_cruReg->CRU_MODE_CON = (0x1<<((pll_id*4) +  16)) | (0x0<<(pll_id*4));            //PLL slow-mode
    
    g_cruReg->CRU_PLL_CON[pll_id][0] = POSTDIV1(postdiv1) | FBDIV(fbdiv);
    g_cruReg->CRU_PLL_CON[pll_id][1] = DSMPD(1) | POSTDIV2(1) | REFDIV(refdiv);

    while (delay > 0) 
    {
        DRVDelayUs(1);
		if (LOCK(g_cruReg->CRU_PLL_CON[pll_id][1]))
			break;
		delay--;
	}
	
    if(cb)
    {
        cb();
    }
    g_cruReg->CRU_MODE_CON = (0x1<<((pll_id*4) +  16))  | (0x1<<(pll_id*4));            //PLL normal
    return;
}

void SetARMPLL(uint16 nMhz)
{
    //Set_PLL(APLL, 600, APLL_cb);
    Set_PLL(GPLL, 300, GPLL_cb);
}

uint32 GetPLLCLK(PLL_ID pll_id)
{
    uint32 refdiv,postdiv1,fbdiv,postdiv2; 
    uint32 ArmPll;
    uint32 AhbClk; 
    ArmPll =  g_cruReg->CRU_PLL_CON[pll_id][0];
    fbdiv = (ArmPll&0xFFFul) ;
    postdiv1 = ((ArmPll >> 12)&0x7ul) ;
    
    ArmPll =  g_cruReg->CRU_PLL_CON[pll_id][1];
    refdiv =  (ArmPll&0x3Ful) ;
    postdiv2 = ((ArmPll >> 6)&0x7ul) ; 
    ArmPll = 24*fbdiv/(refdiv*postdiv1*postdiv2);
    //printf("GetPLLCLK  = %d\n",ArmPll);
    return ArmPll;
}

uint32 GetAHBCLK(void)
{
    uint32 Div1,Div2;
    uint32 ArmPll;
    uint32 AhbClk;
    
    ArmPll = GetPLLCLK(GPLL);
    AhbClk = g_cruReg->CRU_CLKSEL_CON[10];
    Div1 = (AhbClk&0x1F) + 1;
    Div2 = 1<<((AhbClk>>8)&0x3);
    AhbClk = ArmPll/(Div1*Div2);
    //printf("AhbClk  = %d\n",AhbClk);
    return AhbClk*1000;
}

uint32 GetMmcCLK(void)
{
    return (GetPLLCLK(GPLL) * 1000);
}

void uart2UsbEn(uint8 en)
{
    if(en)
    {
        if((!(g_grfReg->GRF_SOC_STATUS0) & (1<<7)) && (g_BootRockusb == 0))
        {
            DRVDelayUs(1);
            if(!(g_grfReg->GRF_SOC_STATUS0) & (1<<7))
            {
                g_grfReg->GRF_UOC0_CON[0] = 0x10001000;
                //g_grfReg->GRF_UOC0_CON[4] = 0x007f0055;
                g_grfReg->GRF_UOC1_CON[4] = 0x34003000;
            }
        }
    }
    else
    {
        g_grfReg->GRF_UOC1_CON[4] = 0x34000000; 
        //g_grfReg->GRF_UOC0_CON[4] = 0x00010000;
        g_grfReg->GRF_UOC0_CON[0] = 0x10000000;
    }
}


/**************************************************************************
USB PHY RESET
***************************************************************************/
bool UsbPhyReset(void)
{
#ifndef FPGA_EMU 
    uart2UsbEn(0);
    DRVDelayUs(1100);    //delay 1.1ms
    if (1)
    {
        g_cruReg->CRU_SOFTRST_CON[4] = ((0x7ul << 5)<<16)|(0x7 << 5);
        DRVDelayUs(10*100);    //delay 1ms
        g_cruReg->CRU_SOFTRST_CON[4] = ((0x7ul << 5)<<16)|(0x0 << 5);
        DRVDelayUs(1*100);     //delay 1ms
        //g_grfReg->GRF_UOC0_CON[0] = 0x07E02B59 ; //0x00F02C59 
    }
#endif
    return (TRUE);
}

/**************************************************************************
USB PHY RESET
***************************************************************************/
void FlashCsInit(void)
{ 
    g_grfReg->GRF_GPIO_IOMUX[1].GPIOD_IOMUX = (0xFFFFuL<<16)|0x5555;   // nand d0-d7
    g_grfReg->GRF_GPIO_PULL[1].GPIOH = 0xFF00FF00;                     //disable pull up d0~d7
    g_grfReg->GRF_GPIO_IOMUX[2].GPIOA_IOMUX = (0xFFFFuL<<16)|0x5555;   // nand dqs,cs0,wp,rdy,rdn,wrn,cle,ale 
    g_grfReg->GRF_GPIO_IOMUX[0].GPIOC_IOMUX = ((0x3uL<<14)<<16)|(0x1uL<<14);   // nand cs1
    g_grfReg->GRF_GPIO_IOMUX[1].GPIOC_IOMUX = ((0xFuL<<12)<<16)|(0x5uL<<12);   // nand cs2 cs3
}

/**************************************************************************
USB PHY RESET
***************************************************************************/
void SpiGpioInit(void)
{

}

void sdmmcGpioInit(uint32 ChipSel)
{
    if(ChipSel == 2)
    {
        g_grfReg->GRF_GPIO_IOMUX[1].GPIOC_IOMUX = ((0xFuL<<12)<<16)|(0xA<<12);        // emmc rstn,cmd
        g_grfReg->GRF_GPIO_IOMUX[1].GPIOD_IOMUX = (0xFFFFuL<<16)|0xAAAA;              // emmc d0-d7
        g_grfReg->GRF_GPIO_IOMUX[2].GPIOA_IOMUX = (((0x3uL<<14)|(0x3<<10))<<16) 
                                                  |(0x2uL<<14)|(0x2<<10);             // emmc_clk,pwren
        g_grfReg->GRF_GPIO_PULL[1].GPIOH = 0xFF000000;                                // pull up d0~d7
    }
    else if(ChipSel == 0)
    {
        g_grfReg->GRF_GPIO_IOMUX[1].GPIOB_IOMUX = (((0x1<<14)|(0x1<<12))<<16)|(0x1<<14)|(0x1<<12);  // mmc0_cmd mmc0_pwren
        g_grfReg->GRF_GPIO_IOMUX[1].GPIOC_IOMUX = (((0x1<<10)|(0x1<<8)|(0x1<<6)|(0x1<<4)|(0x1<<0))<<16)
                                                |(0x1<<10)|(0x1<<8)|(0x1<<6)|(0x1<<4)|(0x1<<0); //mmc0_clkout d0-d3
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
    //*(unsigned long volatile *)(GRF_BASE + GRF_SOC_CON0) 0x20008140  = 0x10001000; 
    g_grfReg->GRF_SOC_CON[0] = ((0x1uL<<12)<<16)|(0x0<<12); 
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
void SoftReset(void)
{
#if 0
    pFunc fp;
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG0_BASE_ADDR ;//USB_OTG_BASE_ADDR
    
    OtgReg->Device.dctl |= 0x02;          //soft disconnect
    DisableIRQ();
    UsbPhyReset();
    FW_NandDeInit();
    MMUDeinit();              /*�ر�MMU*/
    g_cruReg->CRU_MODE_CON = (0x1uL << 16) | (0); //arm enter slow mode 
    
    Delay100cyc(10);
    DisableRemap();
    g_giccReg->ICCEOIR=USB_OTG_INT_CH;
    Delay100cyc(10);
    fp=0x00;
    fp();
#else
    pFunc fp;
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG0_BASE_ADDR ;//USB_OTG_BASE_ADDR

    OtgReg->Device.dctl |= 0x02;          //soft disconnect
    DisableIRQ();
    UsbPhyReset();
    FW_NandDeInit();

    MMUDeinit();              /*�ر�MMU*/

    g_cruReg->CRU_MODE_CON = (0x1uL << 16) | (0); //arm enter slow mode 
    Delay100cyc(10);
    DisableRemap();
    g_giccReg->ICCEOIR=USB_OTG_INT_CH;
    //g_cruReg->CRU_GLB_SRST_FST_VALUE = 0xfdb9;
    g_cruReg->CRU_GLB_SRST_SND_VALUE = 0xeca8; //soft reset grf gpio no reset
    Delay100cyc(10);
    while(1);
#endif
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
#ifndef FPGA_EMU 
    uint32 data = g_cruReg->CRU_SOFTRST_CON[5];
    data = ((1<<16)|(1))<<(sdmmcId + 1);
    g_cruReg->CRU_SOFTRST_CON[5] = data;
    DRVDelayUs(100);
    data = ((1<<16)|(0))<<(sdmmcId + 1);
    g_cruReg->CRU_SOFTRST_CON[5] = data;
    DRVDelayUs(200);
    EmmcPowerEn(1);
#endif
}

int32 SCUSelSDClk(uint32 sdmmcId, uint32 div)
{
    if((div == 0))//||(sdmmcId > 1))
    {
        return (-1);
    }
    if(0 == sdmmcId)
    {
        g_cruReg->CRU_CLKSEL_CON[11] =  (((0x3Ful<<0) | (1<<6))<<16)|(((div-1)<<0) | (1<<6)); //general pll
    }
    else if(1 == sdmmcId)
    {
        g_cruReg->CRU_CLKSEL_CON[12] =  (((0x3Ful<<0) | (1<<6))<<16)|(((div-1)<<0) | (1<<6)); //general pll
    }
    else    //emmc
    {
        g_cruReg->CRU_CLKSEL_CON[12] =  (((0x3Ful<<8) | (1<<7))<<16)|(((div-1)<<8) | (1<<7)); //general pll
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

// pwr key gpio3 c5
/*void powerOn(void)
{
    uint32 chipTag = RKGetChipTag();
    if(chipTag == RK2928G_CHIP_TAG)
    {
        write_XDATA32((GPIO1_BASE_ADDR+0), ReadReg32(GPIO1_BASE_ADDR)|((1ul<<(1)))); //out put high
        write_XDATA32((GPIO1_BASE_ADDR+0x4), (read_XDATA32(GPIO1_BASE_ADDR+0x4)|((0x1ul<<(1))))); // port1 A1 out
    }
    else if(chipTag == RK2926_CHIP_TAG)
    {
        write_XDATA32((GPIO1_BASE_ADDR+0), ReadReg32(GPIO1_BASE_ADDR)|((1ul<<(2)))); //out put high
        write_XDATA32((GPIO1_BASE_ADDR+0x4), (read_XDATA32(GPIO1_BASE_ADDR+0x4)|((0x1ul<<(2))))); // port1 A2 out
    }
}

void powerOff(void)
{
    uint32 chipTag = RKGetChipTag();
    if(chipTag == RK2928G_CHIP_TAG)
    {
        write_XDATA32((GPIO1_BASE_ADDR+0), ReadReg32(GPIO1_BASE_ADDR)&(~(1ul<<(1)))); //out put low
        write_XDATA32((GPIO1_BASE_ADDR+0x4), (read_XDATA32(GPIO1_BASE_ADDR+0x4)|((0x1ul<<(1))))); // port1 A1 out
    }
    else if(chipTag == RK2926_CHIP_TAG)
    {
        write_XDATA32((GPIO1_BASE_ADDR+0), ReadReg32(GPIO1_BASE_ADDR)&(~(1ul<<(2)))); //out put low
        write_XDATA32((GPIO1_BASE_ADDR+0x4), (read_XDATA32(GPIO1_BASE_ADDR+0x4)|((0x1ul<<(2))))); // port1 A2 out
    }
}*/

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

int RKGetChipTag(void)
{
    uint32 i;
    uint32 hCnt = 0;
    uint32 valueL;
    uint32 valueH;
    uint32 value;
    WriteReg32((GPIO3_BASE_ADDR+0x4),  (ReadReg32(GPIO3_BASE_ADDR+0x4)&(~(0x7ul<<0)))); //gpio3   portA 0:2 input
    value = (ReadReg32(GPIO3_BASE_ADDR+0x50))&0x07;
    return value;
}

uint32 RKGetDDRTag(void)
{
    return 0x30334B52; // "RK30"
}

