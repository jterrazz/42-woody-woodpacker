[BITS 32]
segment .text
global _payload32
_payload32:
	pushad

	jmp .next
.string: db "....WOODY.....", 0x0a, 0
.next:
	mov eax, 4
	mov ebx, 1
	mov ecx, .string
	mov edx, 15
	int 80h

.encrypt:
	mov eax, [rel .encrypted_data_start]
	mov edi, [rel .encrypted_data_len]
	mov esi, [rel .start_encode]
	add edi, eax
.encode_loop:
;	xor byte[eax], sil
;   ror esi, 1
    sub byte[eax], 1
	inc eax
	cmp edi, eax
	jne .encode_loop

	popad
	jmp [rel .encrypt] ; Replace by the start of the encrypted data
.encrypted_data_start: dd 0
.encrypted_data_len: dd 0
.start_encode: dd 0
global _payload32_size
_payload32_size:    dq $-_payload32

[BITS 64]
segment .text
global _payload64
_payload64:
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
.encrypt:
	mov rax, [rel .encrypted_data_start]
	mov rdi, [rel .encrypted_data_len]
	mov rsi, [rel .start_encode]
	add rdi, rax
.encode_loop:
;	xor byte[rax], sil
;   ror rsi, 1
    sub byte[rax], 1
	inc rax
	cmp rdi, rax
	jne .encode_loop

	pop rdi
	pop rsi
	pop rdx
	pop rax
	jmp [rel .encrypt] ; Replace by the start of the encrypted data
.encrypted_data_start: dq 0
.encrypted_data_len: dq 0
.start_encode: dq 0
global _payload64_size
_payload64_size: dq $-_payload64
