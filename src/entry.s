global _main

_main:
entrypoint:
    ; put all used registers
    pushf
    push rax
    push rdi
    push rsi
    push rdx
    jmp print_start_msg

    displayed_str db "....WOODY.....", 0x0a, 0

print_start_msg:
    mov rax, 0x2000004 ; 1 for linux
    mov rdi, 1
    lea rsi, [rel displayed_str]
    mov rdx, 15
    syscall

decrypt:
    mov rax, [rel encrypted_entrypoint]
    mov rdi, [rel encrypted_length]
    mov rsi, [rel decryption_key]
    add rdi, rax ; rdi at the end of encrypted_data
    decrypt_loop:
        xor byte[rax], sil ; xor on byte with the key
        ror rsi, 8 ; rot of 8 bits
        inc rax
        cmp rax, rdi
        jne decrypt_loop
    jmp end

variables:
    encrypted_entrypoint dq 0x1111111111111111
    encrypted_length dq 0x2222222222222222
    decryption_key dq 0x3333333333333333

end:
    pop rdx
    pop rsi
    pop rdi
    pop rax
    popf
    ret