[BITS 32]
global _writer
_writer:
	push ebp
	mov ebp, esp

	push ebx

	mov eax, 4
	mov ebx, 1
	mov ecx, .string
	mov edx, 7

	int 80h

	jmp .next
.string db "banane", 0x0a, 0
.next:

	pop ebx

	pop ebp
	ret
global _writer_size
_writer_size: dd $-_writer
