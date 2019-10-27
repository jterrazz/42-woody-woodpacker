global _payload_64
global _payload_size_64
extern puts

[BITS 64]

segment .text
_payload_64:
    pushf
	push rax
	push rdx
	push rsi
	push rdi
	jmp .print_start_msg
.displayed_str:
	db "....WOODY.....", 0x0a, 0
.print_start_msg:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .displayed_str]
	mov rdx, 15
	syscall
%include 'src/encryption/decrypt_64.asm'
	pop rdi
	pop rsi
	pop rdx
	pop rax
	popf
	jmp 0xFFFFFFFF

.encrypted_data_start: dq 0xFFFFFFFFFFFFFFFF
.encrypted_data_len: dq 0xFFFFFFFFFFFFFFFF
.start_encode: dq 0xFFFFFFFFFFFFFFFF

_payload_size_64: dq $-_payload_64
