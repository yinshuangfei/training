
CR0寄存器
=========
CR0寄存器的PE位置1，进入保护模式，CR0寄存器可以用mov指令操作。
mov eax, cr0
or eax, 1
mov cr0, eax