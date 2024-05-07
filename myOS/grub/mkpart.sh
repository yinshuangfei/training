#!/bin/bash
DISK="/dev/loop520"
fdisk ${DISK} <<EOF
n
p



w
EOF
