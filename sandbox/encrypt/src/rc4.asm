[BITS 64]

global _ft_rc4

; rdi data
; rsi data_len
; rdx key
; rcx keu_len
section .data
S: times 256 db 0
K: times 256 db 0

; TODO Push / pop used registers
section .text
_ft_rc4:
	pushfq
	cmp rsi, 0
	je end

; rdi S
global _ft_rc4_set_s
_ft_rc4_set_s:
	xor rax, rax
	l1: ; Could probably replace by a processor opti
		mov BYTE[rdi + rax], al
		inc rax
		cmp rax, 256
		jl l1
	ret

; rdi K
; rsi key
; rdx key_len
global ft_rc4_set_k
ft_rc4_set_k:

state_permutation:

end:
	popfq
	ret
