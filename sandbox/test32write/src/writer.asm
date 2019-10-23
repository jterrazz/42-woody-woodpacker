[BITS 32]
global _writer
global _string
global _string2
global _test_fn
global _function_return
_writer:
	push ebp
	mov ebp, esp

	push ebx

	mov eax, 4
	mov ebx, 1
	mov ecx, _string
	mov edx, 7

	int 80h

	jmp _next
_string: db "banane", 0x0a, 0
_next:
	mov eax, 4
	mov ebx, 1
	mov ecx, _string2
	mov edx, 5

	int 80h

	jmp _next2
_string2: db "toto", 0x0a, 0


_test_fn:
	push ebp
	mov ebp, esp
	mov eax, 4
	mov ebx, 1
	mov ecx, _string
	mov edx, 7

	int 80h
	pop ebp
	ret

_function_return: dd 0

_next2:
	mov eax, 4
	mov ebx, 1
	mov ecx, _string
	mov edx, 7

	int 80h

	call _test_fn

	mov eax, dword [_function_return]
	pop ebx

	pop ebp
	ret
global _writer_size
_writer_size: dd $-_writer
