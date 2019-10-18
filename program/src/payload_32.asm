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