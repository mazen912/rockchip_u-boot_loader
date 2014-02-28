
/******************************************************************/
/*  Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved.       */
/*******************************************************************
File     :  MM.c
Desc   :    �ڴ�����ļ�
Author :  nizy
Date     :  2008-11-20
Notes  :
$Log     :  Revision 1.00  2009/02/14   nizy
********************************************************************/

//#include "drivers.h"
#include "../../armlinux/config.h"
#include "hwapi_MMU.h"

#define DRIVERS_CACHE
#include "hwapi_Cache.h"

/*
    �Զ�����TLB��0x60000000~0x68000000Ϊcache able
*/
void MMUCreateTlb(INT32U * TlbAddr)
{
#ifdef DRIVERS_MMU
    int i;
//#if(PALTFORM==RK29XX)
    *TlbAddr++ = 0x00000c0E; // i == 0
    for(i=1;i<0x1000;i++)
    {
        if( i >= 0x600 && i < 0x604)  // loader ���ֲ�ʹ�� write back,usb�Ͳ��ô���clean
            *TlbAddr++ = 0x00000c0A+0x00100000*i;
        else if( i >= 0x600 && i <= 0x9F0 ) //�����ռ�ʹ��write back��crc32�����ٶȿ��Կ�4��
            *TlbAddr++ = 0x00000c0E + 0x00100000*i;
        else
            *TlbAddr++ = 0x00000c12+0x00100000*i;
    }
/*#elif(PALTFORM==RK30XX)
    *TlbAddr++ = 0x00000c0E; // i == 0 ʹ�� write back
    for(i=1;i<0x1000;i++)
    {
        if( i >= 0x600 && i <= 0x63F )
            *TlbAddr++ = 0x00000c0E+0x00100000*i;// kernel �Ĳ��ֿռ� ��ʹ�� write back
        else if( i >= 0x640 && i <= 0x9F0 )
            *TlbAddr++ = 0x00000c0A+0x00100000*i;// loader ��bss���ֲ�ʹ�� write back,usb�Ͳ��ô���clean
        else
            *TlbAddr++ = 0x00000c12+0x00100000*i; //c12
    }
#endif*/
#endif
}
/*----------------------------------------------------------------------
Name   : MMUInit
Desc   : MMU��س�ʼ��
Params :
Return :
Notes  :
----------------------------------------------------------------------*/
void MMUInit(uint32 adr)
{
#ifdef DRIVERS_MMU
    MMUDisable();              /*�ر�MMU*/
    CacheDisableBoth();        /*�ر�����cache*/
    MMUSetTTB((uint32*)adr);            /*����ҳ�����ڴ���׵�ַ*/
    MMUSetProcessID(0);        /*����FCSE��ת������(û��ת��)*/
    MMUSetDomain(0, CLIENT);   /*��������0�Ĺ���Ȩ��(�û�ģʽ)*/
    CacheInvBoth();           /*�������cache����������*/
    CacheEnableBoth();         /*������cache*/
    MMUEnable();               /*��MMU*/
    MMUFlushTLB();             /*���TLB�е�PTE*/
#ifdef L2CACHE_ENABLE  
    //L2x0Deinit();
    L2x0Init();
#endif
#endif
}

void MMUDeinit(uint32 adr)
{
#if 0

#ifdef DRIVERS_MMU
#ifdef L2CACHE_ENABLE    
    L2x0Deinit();
#endif
    CacheFlushBoth();
    MMUDisable();              /*�ر�MMU*/
    CacheDisableBoth();        /*�ر�����cache*/
#endif

#else

#ifndef CONFIG_SYS_L2CACHE_OFF
	v7_outer_cache_disable();
#endif
#ifndef CONFIG_SYS_DCACHE_OFF
	flush_dcache_all();
#endif
#ifndef CONFIG_SYS_ICACHE_OFF
	invalidate_icache_all();
#endif

#ifndef CONFIG_SYS_DCACHE_OFF
	dcache_disable();
#endif

#ifndef CONFIG_SYS_ICACHE_OFF
	icache_disable();
#endif

#endif
}

uint32 CacheFlushDRegion(uint32 adr, uint32 size)
{
#if 0

#ifdef DRIVERS_MMU
    __CacheFlushDRegion(adr,size);
#ifdef L2CACHE_ENABLE    
    l2x0_flush_range(adr, adr+size);
#endif
#endif

#else
#ifndef CONFIG_SYS_DCACHE_OFF
	flush_cache(adr, size);
#endif

#endif
}

/*uint32 CacheCleanDRegion(uint32 adr, uint32 size)
{
    __CacheCleanDRegion(adr,size);
#ifdef L2CACHE_ENABLE    
    l2x0_flush_range(adr, adr+size);
#endif
}*/


