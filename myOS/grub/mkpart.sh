#!/bin/bash
DISK=$1
fdisk ${DISK} 1>/dev/null 2>/dev/null <<EOF
n
p



w
EOF
exit 0
