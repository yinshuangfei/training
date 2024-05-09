#!/bin/bash
FILE=$1
echo "
menuentry 'Cosmos' {
    insmod part_msdos
    insmod ext2
    set root='hd0'
    multiboot2 /boot/Cosmos.eki
    boot
}

set timeout_style=menu

if [ \"\${timeout}\" = 0 ]; then
    set timeout=10
fi
" > ${FILE}
exit 0
