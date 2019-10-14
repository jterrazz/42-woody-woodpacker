[BITS 32]
global writer
writer:
	push ebp
	mov ebp, esp

	push ebx

	jmp .next
.string db "banane", 0x0a, 0
.next:
	mov eax, 4
	mov ebx, 1
	mov ecx, .string
	mov edx, 7

	int 80h

	pop ebx

	pop ebp
	ret
