/********************************************************************************
		COPYRIGHT (c)   2013 BY ROCK-CHIP FUZHOU
			--  ALL RIGHTS RESERVED  --
File Name:	
Author:         
Created:        
Modified:
Revision:       1.00
********************************************************************************/

#include <common.h>
#include <fastboot.h>
#include "../common/armlinux/config.h"
#include <lcd.h>
#include "rkimage.h"
#include "rkloader.h"
#include "i2c.h"
#include <power/pmic.h>

//#include <asm/arch/rk30_drivers.h>
DECLARE_GLOBAL_DATA_PTR;

extern char PRODUCT_NAME[20] = FASTBOOT_PRODUCT_NAME;

int checkKey(uint32* boot_rockusb, uint32* boot_recovery, uint32* boot_fastboot)
{
    int i;
    int recovery_key = 0;
	*boot_rockusb = 0;
	*boot_recovery = 0;
	*boot_fastboot = 0;
	if(GetPortState(&key_rockusb))
	{
        *boot_rockusb = 1;
	    //printf("rockusb key is pressed\n");
	}
	if(GetPortState(&key_recovery))
	{
        *boot_recovery = 1;
	    //printf("recovery key is pressed\n");
	}
/*
	if(GetPortState(&key_fastboot))
	{
		*boot_fastboot = 1;
		//printf("fastboot key is pressed\n");
	}
*/
	return 0;
}

void RockusbKeyInit(key_config *key)
{
/*
    key->type = KEY_AD;
    key->key.adc.index = 1;
    key->key.adc.keyValueLow = 0;
    key->key.adc.keyValueHigh= 30;
    key->key.adc.data = SARADC_BASE;
    key->key.adc.stas = SARADC_BASE+4;
    key->key.adc.ctrl = SARADC_BASE+8;
*/
    key->type = KEY_GPIO;
    key->key.gpio.valid = 0;
    key->key.gpio.group = 0;
    key->key.gpio.index = 9;// gpio0B1
    setup_gpio(&key->key.gpio);
}

int power_hold() {
    return GetPortState(&key_powerHold);
}

void RecoveryKeyInit(key_config *key)
{
    key->type = KEY_AD;
    key->key.adc.index = 1;
    key->key.adc.keyValueLow = 0;
    key->key.adc.keyValueHigh= 30;
    key->key.adc.data = SARADC_BASE;
    key->key.adc.stas = SARADC_BASE+4;
    key->key.adc.ctrl = SARADC_BASE+8;
}

/*
void FastbootKeyInit(key_config *key)
{
    key->type = KEY_GPIO;
    key->key.gpio.valid = 0; 
	key->key.gpio.group = 4;
	key->key.gpio.index = 21;// gpio4C5
    setup_gpio(&key->key.gpio);
}*/

void PowerHoldKeyInit()
{
    key_powerHold.type = KEY_GPIO;
    key_powerHold.key.gpio.valid = 0; 
    if(ChipType == CHIP_RK3066)
    {
        key_powerHold.key.gpio.group = 6;
        key_powerHold.key.gpio.index = 8; // gpio6B0
    }
    else
    {
        key_powerHold.key.gpio.group = 0;
        key_powerHold.key.gpio.index = 4; // gpio0A4
        //rknand_print_hex("grf:", g_3188_grfReg,1,512);
    }

    setup_gpio(&key_powerHold.key.gpio);
}

void reset_cpu(ulong ignored)
{
	SoftReset();
}

#ifdef CONFIG_USE_RK30IRQ
static int rk30_interrupt_inited = 0;
void do_irq (struct pt_regs *pt_regs)
{
	//printf("do_irq\n");
	IrqHandler();
}

int arch_interrupt_init (void)
{
	if(!rk30_interrupt_inited)
	{
		printf("arch_interrupt_init\n");
		InterruptInit();
		//rk30_reg_irq(irq_init_reg);
		rk30_interrupt_inited = 1;
	}
	return 0;
}
#endif /* CONFIG_USE_IRQ */


#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	ChipTypeCheck();
	return 0;
}
#endif


#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
#ifdef CONFIG_RK30XX
     printf("CPU:\tRK30XX\n");
#endif
     return 0;
}
#endif

/*****************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************/
int board_init(void)
{
	/* Set Initial global variables */

#ifdef CONFIG_RK30XX
	gd->bd->bi_arch_number = MACH_TYPE_RK30XX;
#endif
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x88000;

	return 0;
}


/**********************************************
 * Routine: dram_init
 * Description: sets uboots idea of sdram size
 **********************************************/
int dram_init(void)
{
	gd->ram_size = get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_SYS_SDRAM_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
/**
 * Print board information
 */
int checkboard(void)
{
	puts("Board:\tRK30xx platform Board\n");
	return 0;
}
#endif

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif

void startRockusb()
{
    printf("startRockusb,%d\n" , RkldTimerGetTick());
    rk_backlight_ctrl(0);
    FW_SDRAM_ExcuteAddr = 0;
    g_BootRockusb = 1;
    FWSetResetFlag = 0;
    FWLowFormatEn = 0;
    UsbBoot();
    UsbHook();
}
int board_fbt_key_pressed(void)
{
    int boot_rockusb = 0, boot_recovery = 0, boot_fastboot = 0; 
    enum fbt_reboot_type frt = FASTBOOT_REBOOT_NONE;
	int vbus = GetVbus();
    checkKey(&boot_rockusb, &boot_recovery, &boot_fastboot);
	printf("vbus = %d\n", vbus);
/*
    if(boot_recovery && (vbus==0)) {
        printf("%s: recovery key pressed.\n",__func__);
        frt = FASTBOOT_REBOOT_RECOVERY;
    } else if (boot_rockusb && (vbus!=0)) {
        printf("%s: rockusb key pressed.\n",__func__);
        startRockusb();
    } else if(boot_fastboot && (vbus!=0)){
        printf("%s: fastboot key pressed.\n",__func__);
        frt = FASTBOOT_REBOOT_FASTBOOT;
    }*/
    if(boot_recovery) {
        printf("%s: recovery key pressed.\n",__func__);
        frt = FASTBOOT_REBOOT_RECOVERY;
    } else if (boot_rockusb && (vbus!=0)) {
        printf("%s: rockusb key pressed.\n",__func__);
        startRockusb();
    }

    return frt;
}

struct fbt_partition fbt_partitions[FBT_PARTITION_MAX_NUM];

void board_fbt_finalize_bootargs(char* args, size_t buf_sz,
        size_t ramdisk_sz, int recovery)
{
    char recv_cmd[2]={0};
    ReSizeRamdisk(&gBootInfo, ramdisk_sz);
    if (recovery) {
        change_cmd_for_recovery(&gBootInfo, recv_cmd);
    }
    snprintf(args, buf_sz, "%s", gBootInfo.cmd_line);
//TODO:setup serial_no/device_id/mac here?
}
int board_fbt_handle_flash(char *name,
        struct cmd_fastboot_interface *priv)
{
    return handleRkFlash(name, priv);
}
int board_fbt_handle_download(unsigned char *buffer,
        int length, struct cmd_fastboot_interface *priv)
{
    return handleDownload(buffer, length, priv);
}
int board_fbt_check_misc()
{
    //return true if we got recovery cmd from misc.
    return checkMisc();
}
int board_fbt_set_bootloader_msg(struct bootloader_message* bmsg)
{
    return setBootloaderMsg(bmsg);
}
int board_fbt_boot_check(struct fastboot_boot_img_hdr *hdr, int unlocked)
{
    return secureCheck(hdr, unlocked);
}
void board_fbt_boot_failed(const char* boot)
{
    printf("Unable to boot:%s\n", boot);

    if (!memcmp(BOOT_NAME, boot, sizeof(BOOT_NAME))) {
        printf("try to start recovery\n");
        char *const boot_cmd[] = {"booti", RECOVERY_NAME};
        do_booti(NULL, 0, ARRAY_SIZE(boot_cmd), boot_cmd);
    } else if (!memcmp(RECOVERY_NAME, boot, sizeof(RECOVERY_NAME))) {
        printf("try to start backup\n");
        char *const boot_cmd[] = {"booti", BACKUP_NAME};
        do_booti(NULL, 0, ARRAY_SIZE(boot_cmd), boot_cmd);
    }
    printf("try to start rockusb\n");
    startRockusb();
}

extern char bootloader_ver[];

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
    printf("board_late_init\n");
	ChipTypeCheck();
    SecureBootCheck();
	get_bootloader_ver(NULL);
	printf("##################################################\n");
	printf("\n#Boot ver: %s\n\n", bootloader_ver);
	printf("##################################################\n");

    RockusbKeyInit(&key_rockusb);
//    FastbootKeyInit(&key_fastboot);
    RecoveryKeyInit(&key_recovery);
	PowerHoldKeyInit();
    ChargerStateInit();

    getParameter();

    //TODO:set those buffers in a better way, and use malloc?
    setup_space(gBootInfo.kernel_load_addr);

    char tmp_buf[30];
    if (getSn(tmp_buf)) {
        tmp_buf[sizeof(tmp_buf)-1] = 0;
        setenv("fbt_sn#", tmp_buf);
    }
    fbt_preboot();
	return 0;
}
#endif

#ifdef CONFIG_RK_FB
#define write_pwm_reg(id, addr, val)        (*(unsigned long *)(addr+(PWM01_BASE_ADDR+(id>>1)*0x20000)+id*0x10)=val)

void rk_backlight_ctrl(int brightness)
{
    int id =3;
    int total = 0x4b0;
    int pwm = total * (100 - brightness) / 100;
    int *addr =0;
    printf("backlight --- brightness:%d\n", brightness);
    
    SetPortOutput(3,30,1);   //gpio3_d6 0
    write_pwm_reg(id, 0x0c, 0x80);
    write_pwm_reg(id, 0x08, total);
    write_pwm_reg(id, 0x04, pwm);
    write_pwm_reg(id, 0x00, 0);
    write_pwm_reg(id, 0x0c, 0x09);  // PWM_DIV|PWM_ENABLE|PWM_TIME_EN

    if(ChipType == CHIP_RK3188 || ChipType == CHIP_RK3188B)
    {
        g_3188_grfReg->GRF_GPIO_IOMUX[3].GPIOD_IOMUX |= ((1<<12)<<16)|(1<<12);   // pwm3, gpio3_d6
    }else{
        g_grfReg->GRF_GPIO_IOMUX[3].GPIOD_IOMUX |= ((1<<12)<<16)|(1<<12);   // pwm3, gpio3_d6
    }
    if(pwm != total)
        mdelay(200);
    SetPortOutput(0,2, pwm != total);   //gpio0_a2 1 ,backlight enable
    if((pwm == total))
        mdelay(200);
}

void rk_fb_init(unsigned int onoff)
{
    printf("i2c init OVER in board! \n");
    pmic_init(0);  //enable lcdc power

    SetPortOutput(0,2, 0);   //gpio0_a2 1 ,backlight disable
    SetPortOutput(2,28,0);   //RK30_PIN2_PD4 0 reset 
    SetPortOutput(0,8,0);   //RK30_PIN0_PB0 0 en 
    mdelay(25);
    SetPortOutput(2,17,1);   //RK30_PIN2_PC1 1 cs
    SetPortOutput(2,18,1); //RK30_PIN2_PC2 en_bl
    SetPortOutput(2,19,1); // RK30_PIN2_PC3 stb
    SetPortOutput(2,28,1);   //RK30_PIN2_PD4 1 reset 
    printf("%s  ok!\n",__func__);
}

vidinfo_t panel_info = {
    .lcd_face    = OUT_P888,
	.vl_freq	= 48,  
	.vl_col		= 1024,
	.vl_row		= 600,
	.vl_width	= 1024,
	.vl_height	= 600,
	.vl_clkp	= 0,
	.vl_hsp		= 0,
	.vl_vsp		= 0,
	.vl_bpix	= 4,	/* Bits per pixel, 2^5 = 32 */
    .vl_swap_rb = 0,

	/* Panel infomation */
	.vl_hspw	= 10,
	.vl_hbpd	= 241,
	.vl_hfpd	= 20,

	.vl_vspw	= 2,
	.vl_vbpd	= 25,
	.vl_vfpd	= 10,

	.lcd_power_on = NULL,
	.mipi_power = NULL,

	.init_delay	= 0,
	.power_on_delay = 0,
	.reset_delay	= 0,
};


void init_panel_info(vidinfo_t *vid)
{
	vid->logo_on	= 1;
    vid->enable_ldo = rk_fb_init;
    vid->backlight_on = NULL;//rk_backlight_ctrl;   //move backlight enable to fbt_preboot, for don't show logo in rockusb
    vid->logo_rgb_mode = RGB565;
}

#endif


static key_config charger_state;
void ChargerStateInit()
{
    charger_state.type = KEY_GPIO;
    charger_state.key.gpio.valid = 1;
    charger_state.key.gpio.group = 0;
    charger_state.key.gpio.index = 10;

    setup_gpio(&charger_state.key.gpio);
}

int is_charging()
{
    return !GetPortState(&charger_state);  //gpio0_b2, charger in status
}


