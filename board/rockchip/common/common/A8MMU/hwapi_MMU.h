/******************************************************************/
/*	Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved.  	  */
/*******************************************************************
File 	 :	hwapi_MMU.h
Desc   :	mmu���ͷ�ļ� 
Author :  nizy
Date 	 :	2008-11-20
Notes  : 
$Log: hwapi_MMU.h,v $
Revision 1.1  2010/12/06 02:43:50  Administrator
*** empty log message ***
	
Revision 1.00  2008/11/20 	nizy
********************************************************************/
//#ifdef DRIVERS_MMU

#ifndef __HWAPI_MMU_H__
#define __HWAPI_MMU_H__



/*����Ȩ��*/
typedef enum tagDOMAIN
{
    UNACCESS = 0x0,
    CLIENT,
    RESERVR,
    MANAGER,
    MAXACCESS
}eMMUDOMAIN;

/*system��ROMλ�ı���ģʽ*/
/*if AP is RSRS*/
/* NA = no access, RO = read only, RW = read/write, UN = unpredictable */
/* ROM & SYSYEM*/
typedef enum
{
    RS_NANA = 0x0,
    RS_RONA,
    RS_RORO,    
    RS_UNUN,
    MAXRS	
}eMMUPROTECTION;

/*����Ĺ���Ȩ��*/
/* RS = overridden by ROM&SYSTEM, RO = read only, RW = read/write */
/* privilege & user*/
typedef enum tageMMUPTE_ACCESSPOP
{
    PU_RSRS = 0x0,
    PU_RWNA,
    PU_RWRO,
    PU_RWRW,
    PU_MAXAP
}eMMUPTE_ACCESSPOP;

/*����ID*/
typedef enum tagMMUPTE_DOMAINNID
{
    D0 = 0x0,              
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,
    D14,
    D15,
    MAXDOMID
}eMMUPTE_DOMAINNID;

/*cache��wrtiebuffer������Ϣ*/
/* NC   = noncacheable,  CH   = cacheable,
   NB   = nonbufferable, BF   = bufferable,
   WRTG = write through, WRBK = write back */
typedef enum tageMMUPTE_CACBUF
{
    NCNB = 0x0,         
    NCBF,                 
    WRTH,                 
    WRBK,                 
    MAXCB
}eMMUPTE_CACBUF;

/*һ��ҳ��������*/
typedef enum tageMMUPTE_TYPE
{
    TYPEFAULT = 0x0,      /*��Ч��*/
    COARSE,               /*��ҳ��*/  
    SECTION,              /*��ҳ��*/  
    FINE,                 /*ϸҳ��*/  
    MAXTYPE
}eMMUPTE_TYPE;

/*����ҳ��������*/
typedef enum tageMMUPTE_PAGESIZE
{
    PAGEFAULT = 0x0,       /*��Ч��*/      
    LARGEPAGE,             /*��ҳ*/  
    SMALLPAGE,             /*Сҳ*/
    TINYPAGE,              /*΢ҳ*/
    MAXPAGESIZE           
}eMMUPTE_PAGESIZE;

/*һ��ҳ����Ļ���ַ����*/
typedef union tagBASADDR
{
    uint32 *perAddr;        /*�������ַ*/
    uint32 *secPTBasAddr;   /*����ҳ�����ַ*/
} uBASADDR;

/*ҳ��������*/
typedef union tagATRUTE
{
    uint32 firPagtype;    /*һ��ҳ������*/
    uint32 secPagSize;    /*����ҳ����ߴ�*/
} uATTRUTE;
 
/*ҳ������Ϣ*/ 
typedef struct tagPTE
{
    uBASADDR baseAddr;     /*�������ַ�����ҳ�����ַ*/
    uint32   accessPop;    /*�ô�Ȩ��*/
    uint32   domain;       /*������*/
    uint32   cacheWrbuf;   /*д����*/
    uATTRUTE attribute;    /*ҳ������*/
} PTE;

/*ҳ����ƿ���Ϣ*/
typedef struct tagPCT
{
    uint32 *virBaseAddr;   /*�����ַ*/
    uint32 *ptBaseAddr;    /*ҳ���׵�ַ*/
    uint32 numPage;       /*ҳ������*/
    PTE    *pPagTblEntr;  /*ҳ����*/
} PCT;






#define   APST(ap)                      ((uint32)(ap) & 0x3)
#define   APLS(ap3, ap2, ap1, ap0)      ((((uint32)(ap3)&0x3)<<6) | (((uint32)(ap2)&0x3)<<4) | (((uint32)(ap1)&0x3)<<2) | ((uint32)(ap0)&0x3))
 
 
 
 
 
 
extern void MMUFlushTLB(void); 
extern void MMUFlushSingleTLB(uint32 *adr);
extern void MMUEnable(void);
extern void MMUDisable(void);
extern void MMUProtection(uint32 romSystm); 
extern void MMUSetTTB(uint32 *ttb);    
extern uint32 MMULockTLB(uint32 *adr);
extern uint32 MMUSetProcessID(uint32 processID);
extern uint32 MMUSetDomain(uint32 id, uint32 domain);            
extern uint32 MMUFillCoarsePTE(PCT *pSecondPTC);
extern uint32 MMUFillSectionPTE(PCT *pfirstPTC);
extern uint32 MMUFillFinePTE(PCT *pSecondPTC);
extern uint32 MMUInitPageTable(eMMUPTE_TYPE firPagtype, uint32 *ptBaseAddr);
extern uint32 MMUFlushTLBRegion(uint32 *adr, uint32 *ptBaseAddr);





#endif

//#endif