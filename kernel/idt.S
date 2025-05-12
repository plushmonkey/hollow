bits 64

extern interrupt_dispatch

section .text
global idt_load

idt_load:
  lidt [rdi]
  ret

%macro no_error_code_interrupt_handler 1
align 4
interrupt_handler_%1:
  push qword 0
  push qword %1
  jmp common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
align 4
interrupt_handler_%1:
  push qword %1
  jmp common_interrupt_handler
%endmacro

align 4
common_interrupt_handler:
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
  
  mov rdi, rsp
  call interrupt_dispatch
  
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax
  
  ; restore rsp by popping error code and interrupt number
  add rsp, 16
  
  iretq

; Intel Volume 4: 6.15 Exception and Interrupt Reference
align 64
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31

%assign i 32
%rep (256 - 32)
    no_error_code_interrupt_handler i
%assign i i+1
%endrep

section .data
global interrupt_vector

align 64
interrupt_vector:
%assign i 0
%rep 256
    dq interrupt_handler_ %+ i
%assign i i+1
%endrep
