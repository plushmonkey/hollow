bits 64

global outb
global inb

outb:
  mov rax, rsi
  mov rdx, rdi
  out dx, al
  ret

inb:
  mov rdx, rdi
  xor rax, rax
  in al, dx
  ret
