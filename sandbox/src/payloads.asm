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

	pop rbp
	ret
	jmp [rel .encrypt] ; Replace by the start of the encrypted data
.encrypted_data_start: dq 0
.encrypted_data_len: dq 0
.start_encode: dq 0
global _payload64_size
_payload64_size: dq $-_payload64

; MODIFICATION TRY

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

; put opcode of relative jump REL32
	db 0xe9
.address: dd 0xdeadbeef

; that code is normally unreachable
	pop rbp
	ret
global _jump_base_size
_jump_base_size:    dq $-_jump_base

global _writer
_writer:
	push rdx
	push rsi
	push rdi

	jmp .print_start_msg
.displayed_str db "AFTER RELATIVE JUMP", 0x0a, 0
.print_start_msg:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .displayed_str]
	mov rdx, 21
	syscall

	pop rdi
	pop rsi
	pop rdx

	pop rbp
	ret

global _writer_size
_writer_size:    dq $-_writer
