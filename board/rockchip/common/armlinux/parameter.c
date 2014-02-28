#include "config.h"

extern uint32 krnl_load_addr;
extern uint32 CRC_32CheckBuffer( unsigned char * aData, unsigned long aSize );

//uint8 g_mtdBuffer[sizeof(cmdline_mtd_partition)];
int KeyCombinationNum;
key_config		key_rockusb;
key_config		key_recovery;
key_config		key_fastboot;
key_config		key_powerHold;
key_config		key_combination[MAX_COMBINATION_KEY];

//uint8 gParamBuffer[MAX_LOADER_PARAM];	// ��������Parameters���ֽ���Ϊ:32*512
//suint8 *gParamBuffer;	// ��������Parameters���ֽ���Ϊ:32*512
BootInfo gBootInfo;
cmdline_mtd_partition *g_mymtd = &(gBootInfo.cmd_mtd);

uint32 parameter_lba;

//#pragma arm section code = "LOADER2"

uint32 str2hex(char *str)
{
	int32 i=0;
	uint32 value = 0;
	
	if(*str == '0' && ( *(str+1) == 'x' || *(str+1) == 'X' ) )
		str += 2;
	if( *str == 'x' || *str == 'X' )
		str += 1;

	for(i=0; *str!='\0'; i++,++str) 
	{ 
		if(*str>='0' && *str<='9') 
			value = value*16+*str-'0'; 
		else if(*str>='a' && *str<='f') 
			value = value*16+*str-'a'+10; 
		else if(*str>='A' && *str<='F') 
			value = value*16+*str-'A'+10;
		else// �����ַ�
			break;
	}
	return value;
}

int find_mtd_part(cmdline_mtd_partition* this_mtd, const char* part_name)
{
	int i=0;
	for(i=0; i<this_mtd->num_parts; i++)
	{
		if( !strcmp(part_name, this_mtd->parts[i].name) )
			return i;
	}
	
	return -1;
}

int get_rdInfo(PBootInfo pboot_info)
{
	char *token = NULL;
	char *s=NULL;
	char *p = NULL;
    char cmdline[MAX_LINE_CHAR];
    int len = strlen("initrd=");

	strcpy(cmdline, pboot_info->cmd_line);
    pboot_info->ramdisk_load_addr = 0;
    pboot_info->ramdisk_offset = 0;
    pboot_info->ramdisk_size = 0;
    
	token = strtok(cmdline, " ");
	while(token != NULL)
	{
		if( !strncmp(token, "initrd=", len) )
		{
            token+=len; //initrd=0x62000000,0x80000
            pboot_info->ramdisk_load_addr = simple_strtoull(token,&token,0);
            pboot_info->ramdisk_size = 0x80000;
            
            if(*token == ',')
            {
                token++;
                pboot_info->ramdisk_size = simple_strtoull(token,NULL,0);
            }
            
			break;
		}
		token = strtok(NULL, " ");
	}
	return 0;
}


int parse_cmdline(PBootInfo pboot_info)
{

// cmy: ���������У���ȡ������Ϣ
	pboot_info->cmd_mtd.num_parts = 0;
	pboot_info->cmd_mtd.mtd_id[0]='\0';
 
	if( mtdpart_parse(pboot_info->cmd_line, &pboot_info->cmd_mtd) )
	{
//		int i=0;
//		printf("NO\tOFFSET\t\tSIZE\t\tNAME\n");
//		for(i=0; i<pboot_info->cmd_mtd.num_parts; i++)
//		{
//			printf("%d\t0x%08X\t0x%08X\t%s\n", i, pboot_info->cmd_mtd.parts[i].offset, pboot_info->cmd_mtd.parts[i].size, pboot_info->cmd_mtd.parts[i].name);
//		}
		
		pboot_info->index_misc = find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_MISC);
		pboot_info->index_kernel = find_mtd_part(&pboot_info->cmd_mtd, 	PARTNAME_KERNEL);
		pboot_info->index_boot= find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_BOOT);
		pboot_info->index_recovery = find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_RECOVERY);
		pboot_info->index_system = find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_SYSTEM);
		pboot_info->index_backup = find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_BACKUP);
		pboot_info->index_snapshot = find_mtd_part(&pboot_info->cmd_mtd, PARTNAME_SNAPSHOT);

        if(pboot_info->index_backup > 0)
        {
            g_FwEndLba = pboot_info->cmd_mtd.parts[pboot_info->index_backup].offset + pboot_info->cmd_mtd.parts[pboot_info->index_backup].size;
        }

        get_rdInfo(pboot_info);
        
		if(pboot_info->index_misc < 0 ||
			pboot_info->index_kernel < 0 ||
			pboot_info->index_boot < 0 ||
			pboot_info->index_recovery < 0)
		{
			return -1;
		}
	}
	else
	{
		//RkPrintf("ERROR: parse cmdline failed!");
		return -2;
	}

	return 0;
}

 /* -1:error. */
int ParseAtoi( const char * line )
{
    int base = 10;
    char max = '9';
    int v = 0;

    EATCHAR(line, ' ');
    if(*line == 0) return 0;
    
    if( line[1] == 'x' || line[1] == 'X' ){
        base = 16;
        max = 'f';      /* F*/
        line += 2;
    }
    if( base == 10 ) {
            while( *line >= '0' && *line <= max ) {
                        v *= base ;
                        v += *line-'0';
                        line++;
            }
    } else {
            while( *line >= '0' && *line <= max ) {
                        v *= base ;
                        if( *line >= 'a' )
                                v += *line-'a'+10;
                        else if ( *line >= 'A' )
                                v += *line-'A'+10;
                        else
                                v += *line-'0';
                        line++;
                }
    }
    return v;
}

/*  �������line�л�ȡ����������Ϣ
    ����ֵ:
        0   - ��������
        -1  - ��������
        -2  - δʵ��
 */
int setup_key(char* line, key_config* key)
{
    key_config keyTemp;

	keyTemp.type = ParseAtoi(line++);
    if( keyTemp.type == KEY_GPIO )
    {
        int sub_group;
    	if(*(line++)!=',') return -1;
        keyTemp.key.gpio.group = ParseAtoi(line++);
    	if(*(line++)!=',') return -1;
        sub_group = *(line++);
    	if(*(line++)!=',') return -1;
        keyTemp.key.gpio.index = (sub_group-'A')*8+ParseAtoi(line++);
    	if(*(line++)!=',') return -1;
        keyTemp.key.gpio.valid = ParseAtoi(line++);
        *key = keyTemp;
        setup_gpio(&key->key.gpio);
#if 0 
        RkPrintf("%d,%d,%d,%d\n", key->type, key->key.gpio.group
                                        , key->key.gpio.index
                                        , key->key.gpio.valid);
        
        RkPrintf("%08X,%08X,%08X,%08X\n", key->key.gpio.io_read
                                        , key->key.gpio.io_write
                                        , key->key.gpio.io_dir_conf
                                        , key->key.gpio.io_debounce);
#endif
    }
    else if( keyTemp.type == KEY_AD)
    {// �������͵İ�������
    	if(*(line++)!=',') return -1;
        keyTemp.key.adc.index = ParseAtoi(line++);
    	if(*(line++)!=',') return -1;
        keyTemp.key.adc.keyValueLow = ParseAtoi(line++);
    	if(*(line++)!=',')
            if(*(line++)!=',')
    	        return -1;
        keyTemp.key.adc.keyValueHigh = ParseAtoi(line++);
    	if(*(line++)!=',')
    	     if(*(line++)!=',')
    	         return -1;
    	if(keyTemp.key.adc.keyValueLow >= keyTemp.key.adc.keyValueHigh)
    	    return -1;
        *key = keyTemp;
        setup_adckey(&key->key.adc);
    }
    return 0;
}

extern char PRODUCT_NAME[20];

void ParseLine(PBootInfo pboot_info, char *line)
{
	if( !memcmp(line, "MAGIC:", strlen("MAGIC:")) )
	    pboot_info->magic_code = ParseAtoi(line+strlen("MAGIC:"));
	else if( !memcmp(line, "ATAG:", strlen("ATAG:")) )
	    pboot_info->atag_addr = ParseAtoi(line+strlen("ATAG:"));
	else if( !memcmp(line, "MACHINE:", strlen("MACHINE:")) )
	    pboot_info->machine_type = ParseAtoi(line+strlen("MACHINE:"));
	else if( !memcmp(line, "CHECK_MASK:", strlen("CHECK_MASK:")) )
	    pboot_info->check_mask = ParseAtoi(line+strlen("CHECK_MASK:"));
	else if( !memcmp(line, "KERNEL_IMG:", strlen("KERNEL_IMG:")) )
	    krnl_load_addr = pboot_info->kernel_load_addr = ParseAtoi(line+strlen("KERNEL_IMG:"));
	else if( !memcmp(line, "BOOT_IMG:", strlen("BOOT_IMG:")) )
	    pboot_info->boot_offset = ParseAtoi(line+strlen("BOOT_IMG:"));
	else if( !memcmp(line, "RECOVERY_IMG:", strlen("RECOVERY_IMG:")) )
	    pboot_info->recovery_offset = ParseAtoi(line+strlen("RECOVERY_IMG:"));
	else if( !memcmp(line, "MISC_IMG:", strlen("MISC_IMG:")) )
	    pboot_info->misc_offset = ParseAtoi(line+strlen("MISC_IMG:"));
	else if( !memcmp(line, "RECOVER_KEY:", strlen("RECOVER_KEY:")) )
	{//  RECOVER_KEY: 0,4,A,2,0  ==> GPIO4 PA2 ����Ч
    	line += strlen("RECOVER_KEY:");
    	EATCHAR(line, ' ');
        setup_key(line, &key_rockusb);
	}
    else if( !memcmp(line, "PRODUCT:", strlen("PRODUCT:")))
    {
        line += strlen("PRODUCT:");
        EATCHAR(line, ' ');
        strncpy(PRODUCT_NAME, line ,sizeof(PRODUCT_NAME));
    }
	else if( !memcmp(line, "PWR_HLD:", strlen("PWR_HLD:")) )
	{//  RECOVER_KEY: 0,4,A,2,0  ==> GPIO4 PA2 ����Ч
    	line += strlen("PWR_HLD:");
    	EATCHAR(line, ' ');
        setup_key(line, &key_powerHold);
        if(key_powerHold.key.gpio.valid)
            powerOn();
	}
	else if( !memcmp(line, "COMBINATION_KEY:", strlen("COMBINATION_KEY:")) )
	{
		line += strlen("COMBINATION_KEY:");
		EATCHAR(line, ' ');
        setup_key(line, &key_combination[KeyCombinationNum]);
        KeyCombinationNum++;
	}
#ifdef OTP_DATA_ENABLE
	else if( !memcmp(line, "WAV_ADDR:", strlen("WAV_ADDR:")) )
	{
	    uint32 wav_addr;
		line += strlen("WAV_ADDR:");
		EATCHAR(line, ' ');
	    wav_addr = simple_strtoull(line,&line,0);
        RkPrintf("wav_addr = %x\n",wav_addr);
        if(wav_addr >= 0x60000000 && wav_addr < 0x80000000)
        {
            OtpDataLoad(0, 0, (void*)wav_addr, 512);
            OtpDataLoad(1, 0, (void*)(wav_addr + (512ul*512)), 512);
            RkPrintf("wav = %x\n",*(uint32*)(wav_addr));
        }
	}
#endif	
	else if( !memcmp(line, "CMDLINE:", strlen("CMDLINE:")) )
	{
		line += strlen("CMDLINE:");
		EATCHAR(line, ' ');
		strcpy( pboot_info->cmd_line, line );

		PRINT_I("ORG CMDLINE: %s\n", pboot_info->cmd_line);
		parse_cmdline(pboot_info);
	}
	// cmy: get firmware version
	else if( !memcmp(line, "FIRMWARE_VER:", strlen("FIRMWARE_VER:")) )
	{
		line += strlen("FIRMWARE_VER:");
		EATCHAR(line, ' ');
		strcpy( pboot_info->fw_version, line );
		PRINT_I("FIRMWARE_VER: %s\n", pboot_info->fw_version);
	}
#if(PALTFORM==RK292X || PALTFORM==RK30XX )
	else if( !memcmp(line, "UART", strlen("UART")) )
	{
        uart2UsbEn(1);
	}
#endif	
	else
		PRINT_I("Unknow param: %s!\n", line);
}

//
// param  �ַ���
// line   ��ȡ����һ�����ݴ���ڸñ�����
// ����ֵ ƫ�Ƶ�λ��
//
char* getline(char* param, int32 len, char *line)
{
	int i=0;
//	char *string=param;
	
	for(i=0; i<len; i++)
	{
		if(param[i]=='\n' || param[i]=='\0')
			break;
		
		if(param[i] != '\r')
			*(line++) = param[i];
	}
	
	*line='\0';
	
	return param+i+1;
}

int CheckParam(PLoaderParam pParam)
{
	uint32 crc = 0;
//	PRINT_D("Enter\n");
	
	//if( pParam->tag != 0xFFFFFFFF && pParam->tag != 0 )
	{
		if(pParam->tag != PARM_TAG)
		{
			RkPrintf("W: Invalid Parameter's tag (0x%08X)!\n", pParam->tag);
			return -2;
		}
		
		if( pParam->length > (MAX_LOADER_PARAM-12) )
		{
			RkPrintf("E: Invalid parameter length(%d)!\n", pParam->length);
			return -3;
		}

		crc = CRC_32CheckBuffer((unsigned char*)pParam->parameter, pParam->length+4);
		if(!crc)
		{
			RkPrintf("E:Para CRC failed!\n","");
			return -4;
		}
	}
	/*else
	{
		RkPrintf("Warning: No parameter or Parameter's tag not match!\n");
		return -1;
	}*/

//	pParam->crc = crc;

//	PRINT_D("Leave!\n");
//	RkPrintf("Check ok!\n");
	return 0;
}

int32 GetParam(uint32 param_addr, void *buf)
{
	PLoaderParam param = (PLoaderParam)buf;
	int iResult = 0;
	int i=0;
	int iRet = 0;
	int read_sec = MAX_LOADER_PARAM>>9;
	RkPrintf("GetParam\n","");

	for(i=0; i<PARAMETER_NUM; i++)
	{
		if(StorageReadLba( param_addr+i*PARAMETER_OFFSET, buf, read_sec)==0 )//read_sec
		{
			iResult = CheckParam(param);
			if(iResult >= 0)
			{
				RkPrintf("check parameter success\n","");
				return 0;
			}
			else
			{
				RkPrintf("Invalid parameter\n","");
				iRet = -1;
			}
		}
		else
		{
			RkPrintf("read parameter fail\n","");
			iRet = -2;
		}
	}
	return iRet;
}

int32 GetBackupParam(void *buf)
{
	int iResult = -1;
    return iResult;
}

int32 BackupParam(void *buf)
{
    return 0;
}

void ParseParam(PBootInfo pboot_info, char *param, uint32 len)
{
// һ�����1024Bytes
	char *prev_param=NULL;
	char line[MAX_LINE_CHAR];
	int32 remain_len = (int32)len;
	
//	RkPrintf("Enter\n");
//	RkPrintf("--------------- PARAM ---------------\n");
	while(remain_len>0)
	{
		// ��ȡһ������(�����س����з�������߲����ո�)
		prev_param = param;
		param = getline(param, remain_len, line);
		remain_len -= (param-prev_param);

//		RkPrintf("%s\n", line);
		// ȥ�����м�ע����
		if( line[0] != 0 && line[0] != '#' )
		{// ���в��ǿ��У����Ҳ���ע����
			ParseLine(pboot_info, line);
		}
	}
//	RkPrintf("--------------------------------------\n");
//	RkPrintf("Leave\n");
}

//#pragma arm section code
