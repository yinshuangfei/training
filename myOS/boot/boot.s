[bits 16]
    ; 作为单独的汇编程序时，不设置 org 0x7c00，程序运行将出错; 当该程序作为链接文件使
    ; 用 -f elf 参数编译时，不需要进行设置，因为程序在连接时会进行设置
    ; [0x7c00] will be seted in boot.ld

[global start]
start:
    ; clear screen
    mov ax, 0600h           ; AH = 屏幕初始化或上卷, AL = 上卷行数(0全屏幕为空白)
    mov bx, 0700h           ; BH = 卷入行属性
    mov cx, 0               ; CH = 左上角行号, CL = 左上角列号
    mov dx, 0184fh          ; DH = 右下角行号, DL = 右下角列号
    int 0x10                ; BIOS 对屏幕及显示器所提供的服务程序

    ; set focus
    mov ax, 0200h           ; AH = 置光标位置
    mov bx, 0000h           ; BH = 页号
    mov dx, 0000h           ; DH = 行, DL = 列
    int 0x10

    ; display loading on screen
    mov si, msg
    call print

    ; read the setup code from floppy
    ; begin with second, read setup to 0x90100
.readfloppy:
    mov ax, setupsegment
    mov es, ax
    mov bx, setupoffset     ; ES:BX = 缓冲区的地址
    mov ah, 2               ; AH = 读扇区
    mov dl, [bootdriver]    ; DL = 驱动器(00H~7FH: 软盘, 80H~0FFH: 硬盘)
                            ; DH = 磁头
    mov ch, 0               ; 柱面
    mov cl, 2               ; 扇区
    mov al, setupsize/512   ; 扇区数
    int 0x13                ; 直接磁盘服务(Direct Disk Service)
                            ; CF = 0, 操作成功
    jc  .readfloppy         ; Jump if Carry,

    ; ok, let finish boot and jump to setup
    jmp setupsegment:setupoffset

print:
    mov ah, 0x0E
.next:
    lodsb
    or  al, al
    jz  .done
    int 0x10
    jmp .next
.done:
	ret

    msg	db "Loading Panda OS @Alan Yin"
	db 13, 10, 0            ; 13: CR 回车, 10: LF 换行, 0: NUT 啥也不是

; some constants
setupsegment    equ 0x9000  ; setup address
setupoffset     equ 0x0100
setupsize       equ 512
bootdriver      db  0

; Magic number for sector
times 510 - ($-$$) db 0
dw 0xAA55