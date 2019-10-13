[BITS 32]
segment .text
global _payload32
_payload32:
	push ebp
	mov ebp, esp

	pop ebp
	ret
global _payload32_size
_payload32_size:    dq $-_payload32

[BITS 64]
segment .text
global _payload64
_payload64:
	push rbp
	mov rbp, rsp

	push rax
	push rdx
	push rsi
	push rdi

	jmp .print_start_msg
.displayed_str db "....WOODY.....", 0x0a, 0
.print_start_msg:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .displayed_str]
	mov rdx, 15
	syscall

	pop rdi
	pop rsi
	pop rdx
	pop rax

	pop rbp
	ret
global _payload64_size
_payload64_size:    dq $-_payload64
