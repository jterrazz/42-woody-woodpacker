[BITS 64] ; TODO Doesnt recompile for now (Because its included in the other asm file)

;.encrypt:
;	mov rax, [rel .encrypted_data_start]
;	mov rdi, [rel .encrypted_data_len]
;	mov rsi, [rel .start_encode]
;	add rdi, rax
;.encode_loop:
;	xor byte[rax], sil
;	ror rsi, 1
;    sub byte[rax], 1
;	inc rax
;	cmp rdi, rax
;	jl .encode_loop ; TODO Or jle ?
