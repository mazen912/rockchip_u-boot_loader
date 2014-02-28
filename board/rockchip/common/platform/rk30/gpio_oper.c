
#include    "../../armlinux/config.h"
uint32 g_Rk30xxChip;

extern void DRVDelayUs(uint32 us);

uint32 RK3066GpioBaseAddr[7] = 
{
    0x20034000, //GPIO0
    0x2003C000, //GPIO1
    0x2003E000, //GPIO2
    0x20080000, //GPIO3
    0x20084000, //GPIO4
    0x20088000, //GPIO5
    0x2000A000, //GPIO6
};

uint32 RK3188GpioBaseAddr[7] = 
{
    0x2000A000, //GPIO0
    0x2003C000, //GPIO1
    0x2003E000, //GPIO2
    0x20080000, //GPIO3
};
void setup_gpio(gpio_conf *key_gpio)
{
    uint32 base_addr = 0;
    ChipTypeCheck();

    if (ChipType == CHIP_RK3066)
    {
        if(key_gpio->group >= 7)
            return;
        base_addr = RK3066GpioBaseAddr[key_gpio->group];
    }
    else
    {
        if(key_gpio->group >= 4)
            return;
        base_addr = RK3188GpioBaseAddr[key_gpio->group];
    }

	key_gpio->io_read = base_addr+0x50;
	key_gpio->io_write = base_addr;
	key_gpio->io_dir_conf = base_addr+0x4;
	key_gpio->io_debounce = base_addr+0x48;
}

void setup_adckey(adc_conf *key_adc)
{       
//key  0    1      2       3       4       5
//vol  0mv  300mv  576mv   753mv   852mv   952mv
//ad   0    124    233     304     345     386
    key_adc->keyValueLow *= 10;
    key_adc->keyValueHigh *= 10;
    key_adc->data = SARADC_BASE;
    key_adc->stas = SARADC_BASE+4;
    key_adc->ctrl = SARADC_BASE+8;
    //PRINT_E("keyValueLow: %d\n", key_adc->keyValueLow );
    //PRINT_E("keyValueHigh: %d\n", key_adc->keyValueHigh);
}
#if 0
void dump_gpio(gpio_conf* gpio)
{
    RkPrintf("group: %d\n", gpio->group);
    RkPrintf("index: %d\n", gpio->index);
    RkPrintf("valid: %d\n", gpio->valid);
    RkPrintf("io_read: %08X\n", gpio->io_read);
    RkPrintf("io_write: %08X\n", gpio->io_write);
    RkPrintf("io_dir_conf: %08X\n", gpio->io_dir_conf);
    RkPrintf("io_debounce: %08X\n", gpio->io_debounce);
}
#endif
/*
    �̶�GPIOA_0����Ϊ��д����,ϵͳ���ֲ���ʹ�øÿ�
*/
int GetPortState(key_config *key)
{
#ifndef RK_LOADER_FOR_FT
    uint32 tt;
    uint32 hCnt = 0;
    gpio_conf* gpio = &key->key.gpio;
    adc_conf* adc = &key->key.adc; 
    if(key->type == KEY_GPIO)
    {
        // TODO: ����û�д���
        #if 1 
        // set direction as input 
        write_XDATA32( gpio->io_dir_conf, (read_XDATA32(gpio->io_dir_conf)&(~(1ul<<gpio->index))));
        // enable debounce
        write_XDATA32((gpio->io_debounce), (read_XDATA32(gpio->io_debounce)|((1ul<<gpio->index))));
        for(tt = 0; tt < 100; tt++)
    	{
    	    // read special gpio port value.
    		uint32 value = read_XDATA32(gpio->io_read);
    		if( ((value>>gpio->index)&0x01) == gpio->valid )
    	            hCnt++;
    		DRVDelayUs(1);
    	}
        return (hCnt>80);
        #endif
    }
    else
    {
        // TODO: clkû������
    	for(tt = 0; tt < 10; tt++)
    	{
    	    // read special gpio port value.
    	    uint32 value;
    	    uint32 timeout = 0;
            write_XDATA32( adc->ctrl, 0);
    	    DRVDelayUs(1);
            write_XDATA32( adc->ctrl, 0x0028|(adc->index));
    		DRVDelayUs(1);
    		do{
    		    value = read_XDATA32(adc->ctrl);
    		    timeout++;
            }while((value&0x40)==0);
    		value = read_XDATA32(adc->data);
            //PRINT_E("adc key = %d\n",value);
    		//DRVDelayUs(1000);
    		if( value<=adc->keyValueHigh && value>=adc->keyValueLow)
                hCnt++;
    	}
        write_XDATA32( adc->ctrl, 0);
        return (hCnt>8);
    }
#else
    return 0;
#endif
}

int SetPortOutput(int group, int index, int level)
{
    gpio_conf key_gpio;
    key_gpio.group = group;
    setup_gpio(&key_gpio);
    key_gpio.index = index;

    write_XDATA32( key_gpio.io_dir_conf, (read_XDATA32(key_gpio.io_dir_conf)|(1ul<<key_gpio.index)));
    if(level)
        write_XDATA32( key_gpio.io_write, (read_XDATA32(key_gpio.io_write)|(1ul<<key_gpio.index)));
    else write_XDATA32( key_gpio.io_write, (read_XDATA32(key_gpio.io_write)&(~(1ul<<key_gpio.index))));
    return 0 ;
}

int GetPortInput(int group, int index)
{
    gpio_conf key_gpio;
    key_gpio.group = group;
    setup_gpio(&key_gpio);
    key_gpio.index = index;

    // set direction as input 

    write_XDATA32( key_gpio.io_dir_conf, (read_XDATA32(key_gpio.io_dir_conf)&(~(1ul<<key_gpio.index))));
    return read_XDATA32(key_gpio.io_read);
}

#if 0
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
	if(GetPortState(&key_fastboot))
	{
		*boot_fastboot = 1;
		//printf("fastboot key is pressed\n");
	}
#if 0
{
        for(i=0;i<KeyCombinationNum;i++)
        {
    		//if(key_combination[i].type)
    		{
    		    // cmy@20100409: ���ж�combination����ʱ���ȳ�ʼ����IO��
    			if(GetPortState(&key_combination[i]))// ������ϼ�
    			{
    			    PRINT_E("COMBINATION key is pressed\n");
    				*boot_recovery = 1;
					*boot_rockusb = 0;
					*boot_fastboot = 0;
                    if(key_combination[i].type == KEY_GPIO) // TODO:����ֵ��Ҫ�޸�
                        recovery_key = key_combination[i].key.gpio.group*32+key_combination[i].key.gpio.index;
    				break; 
    			}	
    		}
	    }
	}
#endif
	return 0;
}

/*static int RKGetChipTag(void)
{
    uint32 i;
    uint32 hCnt = 0;
    uint32 valueL;
    uint32 valueH;
    uint32 value;
    write_XDATA32((GPIO6_BASE_ADDR+0x4), (read_XDATA32(GPIO6_BASE_ADDR+0x4)&(~(0x3ul<<14)))); // portD 4:6 input
    valueH = (read_XDATA32(GPIO6_BASE_ADDR+0x50)>>14)&0x03;
    //PRINT_E("valueH = %x\n",valueH);
    write_XDATA32((GRF_BASE+0x148), 0xC000C000); // GPIO 6 B6~7 disable pull up
    write_XDATA32((GPIO6_BASE_ADDR+0x4), (read_XDATA32(GPIO6_BASE_ADDR+0x4)|((0x3ul<<14)))); // portD 4:6 out
    write_XDATA32((GPIO6_BASE_ADDR+0), (read_XDATA32(GPIO6_BASE_ADDR+0)&(~(0x3ul<<14)))); //out put low
    write_XDATA32((GPIO6_BASE_ADDR+0x4), (read_XDATA32(GPIO6_BASE_ADDR+0x4)&(~(0x3ul<<14)))); // portD 4:6 input
    valueL = (read_XDATA32(GPIO6_BASE_ADDR+0x50)>>14)&0x03;
    //PRINT_E("valueL = %x\n",valueL);
    write_XDATA32((GRF_BASE+0x148), 0xC0000000); // GPIO 6 B6~7 enable pull up
    value = (valueH<<2)|valueL;
    //PRINT_E("value = 0x%x\n",value);
    return value;
}*/


void RockusbKeyInit(key_config *key)
{
    key->type = KEY_AD;
    key->key.adc.index = 1;
    key->key.adc.keyValueLow = 0;
    key->key.adc.keyValueHigh= 30;
    key->key.adc.data = SARADC_BASE;
    key->key.adc.stas = SARADC_BASE+4;
    key->key.adc.ctrl = SARADC_BASE+8;
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


void FastbootKeyInit(key_config *key)
{
    key->type = KEY_GPIO;
    key->key.gpio.valid = 0; 
	key->key.gpio.group = 4;
	key->key.gpio.index = 21;// gpio4C5
    setup_gpio(&key->key.gpio);
}

void PowerHoldKeyInit()
{
    key_powerHold.type = KEY_GPIO;
    key_powerHold.key.gpio.valid = 1; 
    if(ChipType == CHIP_RK3066)
    {
        key_powerHold.key.gpio.group = 6;
        key_powerHold.key.gpio.index = 8; // gpio6B0
    }
    else
    {
        key_powerHold.key.gpio.group = 0;
        key_powerHold.key.gpio.index = 0; // gpio0A0
        //rknand_print_hex("grf:", g_3066B_grfReg,1,512);
    }

    setup_gpio(&key_powerHold.key.gpio);
}

void test_port()
{
    key_config key;
    int i,j;
    uint32 value = 0;
    uint32 iovalue[7];

	memset(&key, 0, sizeof(key));
    key.type = KEY_GPIO;
    key.key.gpio.index = 0;
    key.key.gpio.valid = 0;
    memset(iovalue, 0, 7*4);

    while(1)
    {
        for(i=0; i<7; i++)
        {
            key.key.gpio.group = i;
            setup_gpio(&key.key.gpio);
            // set direction as input
            write_XDATA32( key.key.gpio.io_dir_conf, (read_XDATA32(key.key.gpio.io_dir_conf)&0x00000000));
            // enable debounce
            write_XDATA32((key.key.gpio.io_debounce), (read_XDATA32(key.key.gpio.io_debounce)|0xFFFFFFFF));
            value = read_XDATA32(key.key.gpio.io_read);
            if(iovalue[i] != value)
            {
                RkPrintf("GPIO%d: %08X\n", i, value);
                iovalue[i] = value;
            }
        }
//        RkPrintf("---------------\n", i, value);
		DRVDelayS(1);
    }
}
#endif

