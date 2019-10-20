[BITS 64]
segment .text
global _payload64
_payload64:
    mov eax, 60     ; x86_64 opcode for sys_exit
    mov edi, 0     ; first argument
    syscall
	push rax
	push rdx
	push rsi
	push rdi

	jmp .print_start_msg
.displayed_str: db "....WOODY.....", 0x0a, 0
.print_start_msg:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .displayed_str]
	mov rdx, 15
	syscall
;.encrypt:
;	mov rax, [rel .encrypted_data_start]
;	mov rdi, [rel .encrypted_data_len]
;	mov rsi, [rel .start_encode]
;	add rdi, rax
;.encode_loop:
;	xor byte[rax], sil
;   ror rsi, 1
;    sub byte[rax], 1
;	inc rax
;	cmp rdi, rax
;	jne .encode_loop

	pop rdi
	pop rsi
	pop rdx
	pop rax
	ret ; rm
;	jmp [rel .encrypt] ; Replace by the start of the encrypted data
.encrypted_data_start: dq 0
.encrypted_data_len: dq 0
.start_encode: dq 0
global _payload64_size
_payload64_size: dq $-_payload64
