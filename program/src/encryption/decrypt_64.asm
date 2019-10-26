[BITS 64] ; Doesnt recompile for now
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
