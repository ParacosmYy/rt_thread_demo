;*************************************************************************
;                                 ȫ�ֱ���
;*************************************************************************

    IMPORT gs_thread_switch_interrupt_flag
    IMPORT gs_interrupt_from_thread
    IMPORT gs_interrupt_to_thread
		
;*************************************************************************
;                                 ����
;*************************************************************************
;-------------------------------------------------------------------------
;�й��ں�����Ĵ�������ɲο��ٷ��ĵ���STM32F10xxx Cortex-M3 programming manual
;ϵͳ���ƿ�����SCB��ַ��Χ��0xE000ED00-0xE000ED3F
;-------------------------------------------------------------------------
SCB_VTOR        EQU     0xE000ED08     ; ������ƫ�ƼĴ���
NVIC_INT_CTRL   EQU     0xE000ED04     ; �жϿ���״̬�Ĵ���
NVIC_SYSPRI2    EQU     0xE000ED20     ; ϵͳ���ȼ��Ĵ���(2)
NVIC_PENDSV_PRI EQU     0x00FF0000     ; PendSV ���ȼ�ֵ (lowest)
NVIC_PENDSVSET  EQU     0x10000000     ; ����PendSV exception��ֵ
	
;*************************************************************************
;                              �������ָ��
;*************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8
		
gs_hw_interrupt_disable PROC
    EXPORT gs_hw_interrupt_disable
        
    MRS r0, PRIMASK
    CPSID I
    BX lr
    ENDP
        
gs_hw_interrupt_enable PROC
    EXPORT gs_hw_interrupt_enable
        
    MSR PRIMASK , r0
    BX lr
    ENDP        
        
gs_hw_context_switch_first PROC
    EXPORT gs_hw_context_switch_first

    LDR r1 , =gs_interrupt_to_thread
    STR r0,  [r1]

    LDR r1 , =gs_interrupt_from_thread
    MOV r0 ,#0x0
    STR r0 , [r1]

    LDR r1 , =gs_thread_switch_interrupt_flag
    MOV r0 , #1
    STR r0 , [r1]

    LDR     r0, =NVIC_SYSPRI2
    LDR     r1, =NVIC_PENDSV_PRI
    LDR.W   r2, [r0,#0x00]       ; ��
    ORR     r1,r1,r2             ; ��
    STR     r1, [r0]             ; д

    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    CPSIE F
    CPSIE I

    ENDP
     
gs_hw_context_switch    PROC
    EXPORT gs_hw_context_switch
    
    LDR r2, =gs_thread_switch_interrupt_flag
    LDR r3,[r2]
    CMP r3,#1
    BEQ _reswitch
    MOV r3,#1
    STR r3,[r2]
    
    LDR     r2, =gs_interrupt_from_thread                
    STR     r0, [r2]                                     
     
_reswitch
   
	LDR     r2, =gs_interrupt_to_thread                  
    STR     r1, [r2]                                     

    ; ����PendSV�쳣��ʵ���������л�
	LDR     r0, =NVIC_INT_CTRL              
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
	
    ; �ӳ��򷵻�
	BX      LR
	
	; �ӳ������
    ENDP     

PendSV_Handler PROC
    EXPORT PendSV_Handler
    
    MRS     r2, PRIMASK
    CPSID   I
    
    LDR     r0, =gs_thread_switch_interrupt_flag     
    LDR     r1, [r0]                                 
    CBZ     r1, pendsv_exit     
    
    MOV     r1, #0x00
    STR     r1, [r0]  
    
    LDR     r0, =gs_interrupt_from_thread            
    LDR     r1, [r0]                                 
    CBZ     r1, switch_to_thread  
    
    MRS     r1, psp                        
    STMFD   r1!, {r4 - r11}                
    LDR     r0, [r0]                       
    STR     r1, [r0]     

switch_to_thread
    LDR     r1, =gs_interrupt_to_thread             
	                                                 
    LDR     r1, [r1]                                 
    LDR     r1, [r1]                                 

    LDMFD   r1!, {r4 - r11}                          
    MSR     psp, r1                                  
    
pendsv_exit
    ; �ָ��ж�
    MSR     PRIMASK, r2

    ORR     lr, lr, #0x04                            
    BX      lr                                       
	                                                 
    ; PendSV_Handler �ӳ������
	ENDP	
	
	
	ALIGN   4

    END    