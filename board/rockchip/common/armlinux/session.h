/********************************************************************************
*********************************************************************************
                        COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
                                --  ALL RIGHTS RESERVED  --

File Name:      session.C
Author:         HSL
Created:        2009.03.19
Modified:
Revision:       1.00
********************************************************************************
********************************************************************************/
#ifndef __SESSION_H_
#ifndef __GNUC__
#define PACKED1 __packed
#define PACKED2
#define ALIGN(x) __align(x)
#else
#define PACKED1
#define PACKED2 __attribute__((packed))
#define ALIGN(x) __attribute__ ((aligned(x)))
#endif
typedef PACKED1 struct session_head_tag
{
/*
    uint32 Signature;
    uint32 Tag;
    uint32 XferLen;
    uint8 Flags;
    uint8 LUN;
    uint8 CBWLen;
*/	
    uint8 Code;	//0x30 
    uint8 status;	// 1:start (out), 0: stop (int).
    uint16 function;	// 
    uint32 transLen;
    uint8  content[8];	// ��ͬ�Ĺ����в�ͬ�Ĳ���.
}PACKED2 session_head;

typedef PACKED1 struct session_stop_tag
{
/*
    uint32 Signature;
    uint32 Tag;
    uint32 XferLen;
    uint8 Flags;
    uint8 LUN;
    uint8 CBWLen;
*/	
    uint8 Code;	//0x30 
    uint8 status;	// 1:start (out), 0: stop (int).
    uint16 function;	// 
    //uint32 transLen;
    uint8  error;	// 0: OK , else error code.
}PACKED2 session_stop;


#define STRUCT_OFFSET( s , v )		(int32)( &(((s*)0)->v) )


// Ŀǰ SESSION���� ����������: 0x00 , 0x01 �����ļ�����.
#define SESSION_FUN_TRANSFILE			0	// �����ļ����ݡ�
#define SESSION_FUN_TRANSPARAM		1	// �����ļ���Ϣ��

typedef PACKED1 struct session_transfile_tag
{
	uint32 	sdram_lba;	//ֱ�ӵ������ַ������ 0x60008000
	uint8 	reserved[4];
}PACKED2 session_transfile;

typedef session_head	session_transparam;

typedef struct file_info_tag	//�ļ���Ϣ,Ŀǰֻ�����С���ļ���.
{
	uint32	fsize;
	char		fname[256];
} file_info;

void boot_session_handle( void );

#endif  // __SESSION_H_

