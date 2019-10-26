[BITS 64]

global _ft_rc4

; rdi data
; rsi data_len
; rdx key
; rcx keu_len
_ft_rc4:
	jmp .init_k_and_s
.S: times 256 db 0
.K: times 256 db 0
.init_k_and_s:
	xor rsi, rsi

	ret
