[BITS 64] ; Doesnt recompile for now
	jmp .dd
.d:
	db "....WOODY.....", 0x0a, 0
.dd:
	mov rax, 0x1
	mov rdi, 1
	lea rsi, [rel .d]
	mov rdx, 15
	syscall
.encrypt:
	mov rax, [rel .encrypted_data_start]
	mov rdi, [rel .encrypted_data_len]
	mov rsi, [rel .start_encode]
	add rdi, rax
.encode_loop:
;	xor byte[rax], sil
;	ror rsi, 1
;    sub byte[rax], 1
	inc rax
	cmp rdi, rax
	jl .encode_loop ; TODO Or jle ?

	jmp .qq
    .q:
    	db "....WOODY.....", 0x0a, 0
    .qq:
    	mov rax, 0x1
    	mov rdi, 1
    	lea rsi, [rel .q]
    	mov rdx, 15
    	syscall
