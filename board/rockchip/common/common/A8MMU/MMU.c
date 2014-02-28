
/******************************************************************/
/*	Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved.  	  */
/*******************************************************************
File 	 :	MMU.c
Desc   :	mmu�����ļ�
Author :  nizy
Date 	 :	2008-11-20
Notes  :
$Log: MMU.c,v $
Revision 1.1  2010/12/06 02:43:50  Administrator
*** empty log message ***

Revision 1.2  2009/03/05 12:37:14  hxy
���CVS�汾�Զ�ע��
	
Revision 1.00  2008/11/20 	nizy
********************************************************************/
#define     IN_MMU

#include "config.h"

#ifdef DRIVERS_MMU


/*----------------------------------------------------------------------
Name   : MMUFlushTLBRegion
Desc   : �������õ�ַ�����е�
Params : adr -> �׵�ַ
         ptBaseAddr -> ҳ���׵�ַ
Return : 0 -> �ɹ�
         1 -> ʧ��
Notes  : �������ҳ�����ڵ�APֵ��һ�������1�Σ���һ�������4��
----------------------------------------------------------------------*/
uint32 MMUFlushTLBRegion(uint32 *adr, uint32 *ptBaseAddr)
{ 
    uint32 i;
    uint32 j = 1;
    uint32 pageTableEntry1;
    uint32 pageTableEntry2;
    uint32 *adrTemp;
    uint32 addrOffset;
    
    if (((uint32)ptBaseAddr & 0x3fff) > 0)
    {
        return(1);
    }
    
    pageTableEntry1 = *(ptBaseAddr + ((uint32)adr >> 20));
    
    switch (pageTableEntry1 & 0x03)
    {
        case COARSE:
            {
                adrTemp = (uint32 *)(pageTableEntry1 & 0xfffffc00) + (((uint32)adr & 0x000ff000) >> 12);
                break;
            }
        case FINE:
            {
                adrTemp = (uint32 *)(pageTableEntry1 & 0xfffff000) + (((uint32)adr & 0x000ffc00) >> 10);
                break;
            }
        default:
            {
                //printf("MMUFlushTLBRegion: UNKNOWN pagetable type\n");
                return(1);
            }
    }

    pageTableEntry2 = *adrTemp;
    
    pageTableEntry1 = pageTableEntry2 & 0xff0; 

    if ((pageTableEntry1 != 0x000) && (pageTableEntry1 != 0x550) &&
        (pageTableEntry1 != 0xaa0) && (pageTableEntry1 != 0xff0))    
    {
    
        j = 4;
    }
    
    switch (pageTableEntry2 & 0x03)
    {
        case LARGEPAGE:  /* 16k = 64k/4 */
            {
                addrOffset = 14;
                break;
            }
        case SMALLPAGE:  /* 1k = 4k/4 */
            {
                addrOffset = 10;
                break;
            }
        default:
            {
                //printf("MMUFlushTLBRegion: UNKNOWN pagetable size\n");
                return(1);
            }
    }
        
    for (i = 0; i < j; i++)
    {
        adrTemp = (uint32 *)((uint32)adr + (i << addrOffset));
        __asm
        {
            MCR     p15, 0, adrTemp, c8, c7, 1
        }
    }
    
    return(0);

}

/*----------------------------------------------------------------------
Name   : MMUInitPageTable
Desc   : ��ʼ��ҳ��
Params : firPagtype -> һ��ҳ��������
         ptBaseAddr -> ҳ����Ļ���ַ
Return : 0 -> �ɹ�
         1 -> ʧ��
Notes  : ��ʼ��֮��ҳ����ȫΪ�������Ҫ����ʵ������ҳ����Ϣ
----------------------------------------------------------------------*/
uint32 MMUInitPageTable(eMMUPTE_TYPE firPagtype, uint32 *ptBaseAddr)
{
    uint32 index;
    uint32 pageTableEntry = 0;
    uint32 *ptemp;

   if (((uint32)ptBaseAddr & 0x3fff) > 0)
   {
       return(1);
   }
   
   ptemp = ptBaseAddr;
   
    switch (firPagtype)
    {
        case COARSE:
            {
                index =  256/32;
                break;
            }
        case SECTION:
            {
                index = 4096/32;
                break;
            }
        case FINE:
            {
                index = 1024/32;
                break;
            }
        default:
            {
                //printf("MMUInitPT: UNKNOWN pagetable type\n");
                return(1);
            }
    }
    __asm
    {
        MOV     r0, pageTableEntry
        MOV     r1, pageTableEntry
        MOV     r2, pageTableEntry
        MOV     r3, pageTableEntry
looppte:
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        STMIA   ptemp!, {r0-r3}
        SUBS    index, index, #1
        BNE     looppte
    }
    
    return(0);
}

/*----------------------------------------------------------------------
Name   : MMUFillSectionPTE
Desc   : ����һ��ҳ����
Params : pfirstPTC -> ҳ������Ϣ������ҳ�����ͺ����
Return : 0 -> �ɹ�
         1 -> ʧ��
Notes  : ������������ҳ���ע�����ַ�����λ
----------------------------------------------------------------------*/
uint32 MMUFillSectionPTE(PCT *pfirstPTC)
{
    int32 i;
    uint32 pageTableEntry = 0x10;
    uint32 *pAddrtemp;
    uint32 numPageTemp;
    PTE    *pPTETemp;
    uint32 offsetAddr;

    pPTETemp    = pfirstPTC->pPagTblEntr;

    switch (pPTETemp->attribute.firPagtype)
    {
        case COARSE:  /* set coarse table base address */
            {
                offsetAddr      = 10;
                pageTableEntry |= ((uint32)pPTETemp->baseAddr.secPTBasAddr) & 0xfffffc00; /* set L2 coarse base address */
                pageTableEntry |= COARSE;
                break;
            }
        case SECTION: /* set physical address */
            {
                offsetAddr      = 20;
                pageTableEntry |= ((uint32)pPTETemp->baseAddr.perAddr) & 0xfff00000;      /* set physical address */
                pageTableEntry |= (pPTETemp->cacheWrbuf & 0x3) << 2;   /* set cache & WB attributes */
                pageTableEntry |= (pPTETemp->accessPop & 0x3) << 10;   /* set Access Permissions */
                pageTableEntry |= SECTION;
                break;
            }
        case FINE:    /* set fine table base address */
            {
                offsetAddr      = 12;
                pageTableEntry |= ((uint32)pPTETemp->baseAddr.secPTBasAddr) & 0xfffff000; /* set L2 fine base address */
                pageTableEntry |= FINE;
                break;
            }
        default:
            {
                //printf("FillSectionPTE: Incorrect type\n");
                return(1);
            }
    }
    pageTableEntry |= (pPTETemp->domain & 0xf) << 5;
    numPageTemp     = pfirstPTC->numPage;   
    pAddrtemp       = pfirstPTC->ptBaseAddr;                         /* get base address PT */   
    if(((uint32)pAddrtemp & 0x3fff) > 0);  /* get base address PT */
    {
        return(1);
    }
    pAddrtemp      += ((uint32)pfirstPTC->virBaseAddr) >> 20;                  /* index section PTE*/
    pAddrtemp      += numPageTemp - 1;

    for (i = numPageTemp - 1; i >= 0; i--)
    {
        *pAddrtemp-- = pageTableEntry + (i << offsetAddr);
    }

    return(0);
}

/*----------------------------------------------------------------------
Name   : MMUFillCoarsePTE
Desc   : ���ö�����ҳ��ҳ����
Params : pSecondPTC -> ҳ������Ϣ������ҳ�����ͺ�Ȩ�޵�
Return : 0 -> �ɹ�
         1 -> ʧ��
Notes  : ������������ҳ���ע�����ַ�����λ
----------------------------------------------------------------------*/
uint32 MMUFillCoarsePTE(PCT *pSecondPTC)
{
    int32 i,j;
    uint32 tempAP;
    uint32 pageTableEntry = 0x0;
    uint32 *pAddrtemp;
    PTE    *pPTETemp;
    uint32 offsetAddr;
    uint32 numPageTemp;
    uint32 numEntryTemp;
    uint32 bitCleanTemp;


    pPTETemp        = pSecondPTC->pPagTblEntr;
    numPageTemp     = pSecondPTC->numPage;
    pAddrtemp       = pSecondPTC->ptBaseAddr;                         /* get base address PT */   
    if(((uint32)pAddrtemp & 0x3fff) > 0);  /* get base address PT */
    {
        return(1);
    }    
    pAddrtemp      += ((uint32)pSecondPTC->virBaseAddr & 0x000ff000) >> 12;   /* 2nd pageTableEntry */
   
    switch (pPTETemp->attribute.secPagSize)
    {
        case LARGEPAGE:  /*64 KB, 4k per entry*/
            {
                bitCleanTemp    = 0xffff0000;                         /* clean index */
                pageTableEntry |= LARGEPAGE;                          /* set as large page */
                pAddrtemp      += (16 * numPageTemp) - 1;
                offsetAddr      = 16;
                numEntryTemp    = 16;                                 /* 64k = 16 * 4k*/
                break;
            }
        case SMALLPAGE: /*4 KB, 4k per entry*/
            {
                bitCleanTemp    = 0xfffff000;                         /* clean index */
                pageTableEntry |= SMALLPAGE;                          /* set as small page */
                pAddrtemp      += numPageTemp - 1;
                offsetAddr      = 12;
                numEntryTemp    = 1;                                  /* 4k = 1 * 4k*/
                break;
            }
        default:
            {
                //printf("MMUFillCoarsePTE: Incorrect page size\n");
                return(1);
            }
    }
    
    tempAP          = pPTETemp->accessPop & 0xff;
    pageTableEntry |= (tempAP & 0xc0) << 4;                          /* set Access Permissions sub page 3 */
    pageTableEntry |= (tempAP & 0x30) << 4;                           /* sub page 2 */
    pageTableEntry |= (tempAP & 0x0c) << 4;                           /* sub page 1 */
    pageTableEntry |= (tempAP & 0x03) << 4;                           /* sub page 0 */
    pageTableEntry |= (pPTETemp->cacheWrbuf & 0x3) << 2;              /* set cache & WB attributes */
    pageTableEntry |= ((uint32)pPTETemp->baseAddr.perAddr) & bitCleanTemp;    /* set physical base address */
    
    for (i = numPageTemp - 1; i >= 0; i--)
    {
        for (j = numEntryTemp - 1; j >= 0; j--)
        {
            *pAddrtemp-- = pageTableEntry + (i << offsetAddr);
        }
    }

    return(0);
}

/*----------------------------------------------------------------------
Name   : MMUFillFinePTE
Desc   : ���ö���ϸҳ��ҳ����
Params : pSecondPTC -> ҳ������Ϣ������ҳ�����ͺ�Ȩ�޵�
Return : 0 -> �ɹ�
         1 -> ʧ��
Notes  : ������������ҳ���ע�����ַ�����λ
----------------------------------------------------------------------*/
uint32 MMUFillFinePTE(PCT *pSecondPTC)
{
    int32 i,j;
    uint32 tempAP;
    uint32 pageTableEntry = 0x0;
    uint32 *pAddrtemp;
    PTE    *pPTETemp;
    uint32 offsetAddr;
    uint32 numPageTemp;
    uint32 numEntryTemp;
    uint32 bitCleanTemp;


    pPTETemp        = pSecondPTC->pPagTblEntr;
    numPageTemp     = pSecondPTC->numPage;
    
    pAddrtemp       = pSecondPTC->ptBaseAddr;                         /* get base address PT */   
    if(((uint32)pAddrtemp & 0x3fff) > 0);  /* get base address PT */
    {
        return(1);
    }    
    
    pAddrtemp      += ((uint32)pSecondPTC->virBaseAddr & 0x000ffc00) >> 10;   /* 2nd pageTableEntry */
   
    switch (pPTETemp->attribute.secPagSize)
    {
        case LARGEPAGE:  /*64 KB, 1k per entry*/
            {
                bitCleanTemp    = 0xffff0000;                         /* clean index */
                pageTableEntry |= LARGEPAGE;                          /* set as large page */
                pAddrtemp      += (64 * numPageTemp) - 1;
                offsetAddr      = 16;
                numEntryTemp    = 64;                                 /* 64k = 64 * 1k*/
                break;
            }
        case SMALLPAGE:  /*4 KB, 1k per entry*/
            {
                bitCleanTemp    = 0xfffff000;                         /* clean index */
                pageTableEntry |= SMALLPAGE;                          /* set as small page */                                      /* set as LARGE PAGE */
                pAddrtemp      += (4 * numPageTemp) - 1;
                offsetAddr      = 12;
                numEntryTemp    = 4;                                  /* 4k = 4 * 1k*/
                break;
            }
        case TINYPAGE:   /*1 KB, 1k per entry*/
            {
                bitCleanTemp    = 0xfffffc00;                         /* clean index */
                pageTableEntry |= TINYPAGE;                           /* set as tiny page */
                pAddrtemp      += numPageTemp - 1;
                offsetAddr      = 10;
                numEntryTemp    = 1;                                  /* 1k = 1 * 1k*/
                break;
            }
        default:
            {
                //printf("MMUFillFinePTE: Incorrect page size\n");
                return(1);
            }
    }
    
    tempAP          = pPTETemp->accessPop & 0xff;
    pageTableEntry |= (tempAP & 0xc0) << 4;                          /* set Access Permissions sub page 3 */
    pageTableEntry |= (tempAP & 0x30) << 4;                           /* sub page 2 */
    pageTableEntry |= (tempAP & 0x0c) << 4;                           /* sub page 1 */
    pageTableEntry |= (tempAP & 0x03) << 4;                           /* sub page 0 */
    pageTableEntry |= (pPTETemp->cacheWrbuf & 0x3) << 2;              /* set cache & WB attributes */
    pageTableEntry |= ((uint32)pPTETemp->baseAddr.perAddr) & bitCleanTemp;    /* set physical base address */

    for (i = numPageTemp - 1; i >= 0; i--)
    {
        for (j = numEntryTemp - 1; j >= 0; j--)
        {
            *pAddrtemp-- = pageTableEntry + (i << offsetAddr);
        }
    }

    return(0);
}


#endif









