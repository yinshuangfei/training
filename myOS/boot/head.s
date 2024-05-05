extern kmain

[bits 32]
    mov esp, stack_top
    mov ax, 0x10                ; 第2个 GDT 记录
    mov ds, ax
    mov cl, '2'
    mov [0xb8000], cl           ; '2' = 0x32
    mov cl, 0x04
    mov [0xb8001], cl

    ;; check A20 enable
    xor eax, eax                ; eax 清 0
err:
    inc eax                     ; eax += 1
    mov [0x000000], eax         ; eax 放到 0 地址处
    cmp [0x100000], eax         ; 比较 A20 地址线指向的值和 eax 的大小
                                ; 若没有开启 A20 则 0x100000 地址的指向将回滚到
                                ; 0x00000 处
    je err                      ; 相等表明地址回转, A20 地址线没有开启

    ;; Finally, we goto OS's c world
    push esp
    call kmain
    ;; jmp $

section .bss
    resb 8192               ; 80KB for stack
stack_top:                  ; top of our stack here