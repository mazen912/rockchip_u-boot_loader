/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    dma.h
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/

#ifndef _DMA_H
#define _DMA_H
//1�����ò���
//#define 	MAX_DRIVE                   2   //���֧�ֵ���������

//1��������
//HDMA_CON0�Ĵ���λ����
#define 	DMA_EN                      0x200000
#define 	AUTO_RE                     0x100000
#define 	INT_EN                      0x080000
#define 	BURST16                     0x00e000
#define 	BURST8                      0x00a000
#define 	BURST4                      0x006000
#define 	SRC_FIX                     0x000080
#define 	SRC_INC                     0x000000
#define 	DST_FIX                     0x000020
#define 	DST_INC                     0x000000
#define 	WIDTH_X32                   0x000010
#define 	WIDTH_X16                   0x000008
#define 	WIDTH_X8                    0x000000
#define 	DMA_CANCE                   0x000006
#define 	DMA_PAUSE                   0x000004
#define 	DMA_START                   0x000002
#define 	HW_REQ_EN                   0x000001

//1�ṹ����


//1ȫ�ֱ���
#undef	EXT
#ifdef	IN_DMA
		#define	EXT
#else
		#define	EXT		extern
#endif		
//EXT		DRIVE_INFO     DriveInfo[MAX_DRIVE];

//1����ԭ������


//1�����
#ifdef IN_DMA
#else
#endif
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/


