;=========================================
; NAME: LDK3.1 INIT
; DESC: start up codes
;       Configure memory, ISR ,stacks
;	Initialize C-variables
; Author: huangxinyu
; Date: 2007-04-11
;=========================================
;define the stack size
;�����ջ�Ĵ�С
FIQ_STACK_LEGTH         EQU         0				;not use FIQ 
IRQ_STACK_LEGTH         EQU         512             ;every layer need 9 bytes stack , permit 8 layer .ÿ��Ƕ����Ҫ9���ֶ�ջ������8��Ƕ��
ABT_STACK_LEGTH         EQU         0
UND_STACK_LEGTH         EQU         0

NoInt       EQU 0x80

USR32Mode   EQU 0x10
SVC32Mode   EQU 0x13
SYS32Mode   EQU 0x1f
IRQ32Mode   EQU 0x12
FIQ32Mode   EQU 0x11

PINSEL2     EQU 0xE002C014

BCFG0       EQU 0xFFE00000
BCFG1       EQU 0xFFE00004
BCFG2       EQU 0xFFE00008
BCFG3       EQU 0xFFE0000C

    
;������ⲿ�����������	
	IMPORT IrqHandler
    IMPORT  Main                        ;The entry point to the main function C������������� 
     
;���ⲿʹ�õı����������
   
    EXPORT  ResetHandler
	EXPORT  ResetCpu

	IMPORT		||Image$$CODE$$Base||
	IMPORT		||Image$$CODE$$Limit||
    IMPORT		||Image$$CODE2$$ZI$$Limit||
    
    CODE32

    AREA    vectors,CODE,READONLY
        ENTRY
;�ж�������
Reset
        LDR     PC, ResetAddr
        LDR     PC, UndefinedAddr
        LDR     PC, SWI_Addr
        LDR     PC, PrefetchAddr
        LDR     PC, DataAbortAddr
        DCD     0xb9205f80
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr
        
ResetAddr           DCD     ResetHandler
UndefinedAddr       DCD     Undefined
SWI_Addr            DCD     SoftwareInterrupt
PrefetchAddr        DCD     PrefetchAbort
DataAbortAddr       DCD     DataAbort
Nouse               DCD     0
IRQ_Addr            DCD     IrqExce
FIQ_Addr            DCD     FIQ_Handler

;δ����ָ��
Undefined
        B       Undefined

SoftwareInterrupt
        B       SoftwareInterrupt
        
;ȡָ����ֹ
PrefetchAbort
        SUBS	PC, R14, #4
        ;B       PrefetchAbort

;ȡ������ֹ
DataAbort
        SUBS	PC, R14, #4

;�����ж�
FIQ_Handler
        B       FIQ_Handler

;IRQ�ж�
IrqExce
        SUB	LR, LR, #4			;����ʵ�ʵķ��ص�ַ
        STMFD	SP!,	{R0-R12, LR}	;
        
        MRS		R1, SPSR
        STMFD	SP!, {R1}			;����IRQ ����ǰ CPSR ��ֵ����ջ
        PRESERVE8
        BL IrqHandler
;        LDR     LR, =ret
;        LDR     PC, =IrqHandler
;ret 
        ;BL		IrqHandler
        LDMFD	SP!, {R1}
        MSR		SPSR_cxsf,	R1
        LDMFD	SP!, {R0-R12, PC}^

	
;/*********************************************************************************************************
;** Initialize the stacks  ��ʼ����ջ
;********************************************************************************************************/
;		IMPORT		StackSvc

InitStack    
        MOV     R0, LR
;Build the SVC stack
; �����ж�ģʽ��ջ
        MSR     CPSR_c, #0xd2
        LDR     R1, =||Image$$CODE2$$ZI$$Limit||
        LDR		R2, =0x4000
        ADD		R1, R1, R2
        MOV     SP, R1

;Build the SVC stack
; �����ж�ģʽ��ջ
        MSR     CPSR_c, #0xd7
        LDR		R2, =0x4000
        ADD		R1, R1, R2
        MOV     SP, R1
        
;���ó����û�ģʽ��ջ
        MSR     CPSR_c, #0xd3	;SVC32Mode
        LDR		R2, =0x50000
        ADD		R1, R1, R2
        MOV     SP, R1
        MOV     PC, R0


	EXPORT		BootDisableInt	
BootDisableInt 
				MRS r0, cpsr 				;����������ں˶�������svcģʽ�£���˿ɷ���ز���cpsr 
				ORR r1, r0, #0xc0 		;����FIQ��IRQ�ж� 
				MSR cpsr_c, r1 			;��дcpsr��ֻ����IRQ�ж� 
				MOV pc, lr 					;���� 
	
;/********************************************************************************************************
;** RESET  ��λ���
;********************************************************************************************************/
ResetHandler
		bl  InitStack               	; Initialize the stack
        
    ldr	pc, initloops     			; Jump to the entry point of C program ��ת��c�������
initloops   DCD     Main

ResetCpu
    ;//cruReg->CRU_GLB_SRST_SND_VALUE = 0xeca8; //soft reset
	LDR		R2, =0x20020004
	LDR		R1, =0x00000003
 	STR		R1, [R2,#0]

	LDR		R1, =0x00001000
ResetCpuDelay0
    SUBS	R1, R1, #1
	BGE     ResetCpuDelay0
	
    ;//cruReg->CRU_MODE_CON = 0x33330000;    //cpu enter slow mode
	LDR		R2, =0x20000040
	LDR		R1, =0x33330000
 	STR		R1, [R2,#0]
 	
	LDR		R1, =0x00001000
ResetCpuDelay1
    SUBS	R1, R1, #1
	BGE     ResetCpuDelay1

 	LDR     R1, =0xeca8
	LDR     R2, =0x20000104
	LDR     R3, =0x10000000	
	
 	STR		R3, [R0,#0]  ;// remap
 	STR		R1, [R2,#0]

ResetCpu_loop
     B  ResetCpu_loop

     END
;/*********************************************************************************************************
;**                            End Of File
;********************************************************************************************************/
