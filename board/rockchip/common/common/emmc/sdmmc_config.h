/******************************************************************/
/*	Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved.  	  */
/*******************************************************************
File    :   driver.h
Desc    :   �������������������ͷ�ļ���
            ����.c�ļ���Ӧֻ������һ��ͷ�ļ���
Author  :   rk28 driver
Date    :   2008-11-18
Notes   :   
$Log: drivers.h,v $
Revision 1.1  2011/03/29 09:20:48  Administrator
1��֧������8GB EMMC
2��֧��emmc boot 1��boot 2��д��IDBLOCK����

Revision 1.1  2011/01/07 03:28:04  Administrator
*** empty log message ***

********************************************************************/
#ifndef _EMMC_CONFIG_H
#define _EMMC_CONFIG_H

#include    "../../armlinux/config.h"

#define     DRIVER_ONLY

#include    "hw_SDConfig.h"
#include    "hw_SDCommon.h"
#include    "hwapi_SDOsAdapt.h"
#include    "hwapi_SDController.h"
#include    "hw_SDController.h"
#include    "hw_SDPlatAdapt.h"
#include    "hw_SD.h"
#include    "hw_MMC.h"
#include    "hw_SDM.h"
#include    "hwapi_SDM.h"

/********************************************************************
**                 api head files                                   *
********************************************************************/
//#include    "api_drivers.h"

#endif

