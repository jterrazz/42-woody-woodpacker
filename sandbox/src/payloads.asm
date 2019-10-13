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

global _modification_try
_modification_try:
	push rbp
	mov rbp, rsp

	mov rax, [rel .value]
	pop rbp
	; rax is returned as u64
	ret
.value dq 0xdeadbeefdeadbeef
global _modification_try_size
_modification_try_size:    dq $-_modification_try

global _jump_base
_jump_base:
	push rbp
	mov rbp, rsp

	db 0xe9
.address: dd 0xdeadbeef

	pop rbp
	ret
global _jump_base_size
_jump_base_size:    dq $-_jump_base

global _writer
_writer:
	jmp .print_start_msg
.displayed_str db "AFTER RELATIVE JUMP", 0x0a, 0
.print_start_msg:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .displayed_str]
	mov rdx, 21
	syscall

	jmp $
global _writer_size
_writer_size:    dq $-_writer
