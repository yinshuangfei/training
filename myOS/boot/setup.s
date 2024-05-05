align 4
[bits 16]
    ;; first 1MB memory
    ;; 0x00000 ~ 0x003FF : interupt vector
    ;; 0x00400 ~ 0x004FF : BIOS data
    ;; 0x00500 ~ 0x07BFF : free
    ;; 0x07C00 ~ 0x07DFF : loader
    ;; 0x07E00 ~ 0x9FFFF : free
    ;; 0xA0000 ~ 0xBFFFF : vedio
    ;; 0xC0000 ~ 0xFFFFF : bios interupt precedure
    jmp setup

[global setup]
setup:
    mov ax, setupsegment
    mov ds, ax                      ; DS = 数据段寄存器（Data Segment）
    mov es, ax                      ; ES = 附加段寄存器（Extra Segment)
                                    ; 在使用内存数据拷贝（字符串指令）时，固定使用
                                    ; DS:[SI] 指向源地址, 固定使用 ES:[DI] 指向
                                    ; 目的地址
    mov ss, ax                      ; SS = 堆栈段寄存器（Stack Segment）
    mov sp, 0xffff                  ; SP = 堆栈指针寄存器（Stack Pointer）

    ;; loading message
    mov si, setup_msg
    call print_str

read_info:
    mov ax, 0x9000
    mov ds, ax

    ;; Now get memory size and save at 0x90002
    ;; This may just report up to 64M.
    ;; It only reports contiguous (usable) RAM.
    mov ah, 0x88                    ; EAX = 0xE820：遍历主机上全部内存
                                    ; AX  = 0xE801：分别检测低15MB和16MB～4GB内存，
                                    ; 最大支持4GB
                                    ; AH  = 0x88：最多检测64MB内存，实际内存超过此
                                    ; 容量也按照 64MB 返回，最开始的 1M 被占用
    int 0x15                        ; BIOS 中断, 探测内存
    mov [2], ax                     ; 存储中断的返回值: Memory Count
                                    ; （Number of contiguous KB above 1 MB）

    mov ax, setupsegment
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xffff

    ;; 将内核临时拷贝至 0x90100 + 512 起始的位置
    mov ax, setupsegment
    mov es, ax                      ; ES:BX = 缓冲区的地址
    mov bx, setupoffset+setupsize   ; put kernel at here now
    mov ah, 2                       ; AH = 读扇区
    mov dl, [bootdriver]            ; DL = 驱动器(00H~7FH: 软盘, 80H~0FFH: 硬盘)
    mov ch, 0                       ; CH = 柱面
    ;; in disk, 0,1 sector is for boot, setupsize/512 sectors for setup.bin
    mov cl, 3                       ; CL = 扇区
    mov al, systemsize/512          ; AL = 扇区数
.readfloppy:
    int 0x13                        ; 直接磁盘服务(Direct Disk Service)
    dec al                          ; al -= 1
    jnz .readfloppy

    ;; loading kernel done
    mov si, load_done_msg
    call print_str

    ;; move system to 0x00000
    ;; this is OK for our kernel.bin is small
    cld                             ; 将标志寄存器中的方向标志位清零
    mov si, setupoffset+setupsize
    mov ax, systemsegment
    mov es, ax
    mov di, systemoffset            ; DI = 寄存器（Destination Index）
                                    ; SI 指向源的起点，ES:[DI] 指向复制到的目标地点
    mov cx, systemsize
    rep movsd                       ; MOVSD(MOV Source Dword), 数据传送的指令

    ;; 由于 0x00000 地址的中断向量被覆盖，因此后续代码无法使用中断处理程序

    ;; 关闭中断
    cli
    lgdt [gdt_addr]                 ; 将 gdt_addr 地址指向的 GDT 表加载至 gdtr 中
                                    ; 该地址目前没有在内存的起始地址

    ;; enable A20, 使能 A20 包含多种模式, 现在 A20 gate 缺省为开的
    ;; 下面的流程是仿照 linux 内核制作的
    call empty_8042
    mov al, 0xd1                    ; command write
    out 0x64, al
    call empty_8042
    mov al, 0xdf                    ; A20 on
    out 0x60, al
    call empty_8042
    ;; enter pretect-mode
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ;; jump into head, which at 0x00000
    ;; jmp SELECTOR_CODE: OFFSET
    jmp dword 0x8:0x0               ; 0x8 表示第1个 GDT 记录，第 0 个无效
                                    ; 0x8 段选择子的基地址为 0

    ;; A20 键盘控制器芯片
empty_8042:
    in al, 0x64                     ; 读取输入端口, 取得控制器状态, 8042 status port
    test al, 0x2                    ; is input buffer full
    jnz  empty_8042
    ret

;; interupt show message
print_str:
    mov ah, 0x0E
.next:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

setupsegment    equ 0x9000
setupoffset     equ 0x0100
setupsize       equ 512

systemsegment   equ 0x0000
systemoffset    equ 0x0000
systemsize      equ 1024*36 ; this will bigger than kernel.bin, 36K, 72 扇区
                ;; bochs seems has error when read more than 1024*36
bootdriver      db  80h     ; 0: 表示软盘；80h: 表示硬盘

setup_msg db "Setuping Glue OS"
    db 13, 10, 0            ; 13: CR 回车, 10: LF 换行, 0: NUT 啥也不是

load_done_msg db "Finish Loading kernel"
    db 13, 10, 0            ; 13: CR 回车, 10: LF 换行, 0: NUT 啥也不是

gdt_addr:                   ; dw 表示 16 字节
    ; dw 0x7fff               ; GDT  0-15 位，全局描述符表边界，32767
    ; dw 0x07ff               ; GDT  0-15 位，全局描述符表边界，32767
    dw 0x007f               ; GDT  0-15 位，全局描述符表边界，128 / 8 = 16 条
    dw gdt                  ; GDT 16-31 位
    dw 0x0009               ; GDT 32-47 位，当前的段基地址为 0x90000，因此该值为 9

gdt:
    ;; 第一个段描述符占位（NULL）
gdt_null:
    dw 0x0000
    dw 0x0000
    dw 0x0000
    dw 0x0000
gdt_system_code:
    ;; 64 MB
    ;; 下面内容作为低 32 位进行描述
    dw 0x3fff               ; 段界限: 0-15, 实际效果 0x3fff fff
    dw 0x0000               ; 段基址: 0-15
    ;; 下面内容作为高 32 位进行描述
    dw 0x9a00               ; 见下述描述, 1|00|1|1010,00000000，type 位保留
    ;; 高8位: (15)      P, 是否位于内存中
    ;;        (14-13)   DPL, 访问段所需要的特权级，特权级范围为0～3，越小特权级越高
    ;;        (12)      S, =1 表示非系统段（代码段或数据段, 栈段也是一种特殊的数据段）
                         ; =0 表示系统段
    ;;        (11-8)    TYPE, S=1，第三位为0时为数据段，为1时为代码段
    ;; 低8位: 段基址 23-16
    dw 0x00c0               ; 见下述描述, 00000000,1|1|0|0|0000
    ;; 高8位: 段基址 31-24
    ;; 低8位: (23)      G, 段界限粒度, 为0时表示粒度为1字节，为1时表示粒度为4KB
    ;;        (22)      D/B, 代码段此位是D位, 栈段此为是B位, 为0时表示使用的16位
    ;;        (21)      L, 设置是否为64位代码段，L属性为1，则必须保证D属性为0
    ;;        (20)      AVL, 可由操作系统、应用程序自行定义
    ;;        (19-16)   段界限 19-16
gdt_syste_data:
    dw 0x3fff
    dw 0x0000
    dw 0x9200               ; 1|00|1|0010,00000000, type 为 LDT
    dw 0x00c0               ; 00000000,1|1|0|0|0000

; DA_DR     EQU 90h         ; 存在的只读数据段类型值
; DA_DRW    EQU 92h         ; 存在的可读写数据段属性值
; DA_DRWA   EQU 93h         ; 存在的已访问可读写数据段类型值
; DA_C      EQU 98h         ; 存在的只执行代码段属性值
; DA_CR     EQU 9Ah         ; 存在的可执行可读代码段属性值
; DA_CCO    EQU 9Ch         ; 存在的只执行一致代码段属性值
; DA_CCOR   EQU 9Eh         ; 存在的可执行可读一致代码段属性值

;; Magic number for sector
times 512 - ($ - $$) db 0