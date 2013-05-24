#!/bin/bash
nasm kernel64.asm -o kernel64.sys
cat pxestart.sys pure64.sys kernel64.sys > /var/lib/tftpboot/pxeboot.bin
vboxmanage controlvm BMOS poweroff
cat pxestart.sys pure64.sys kernel64.sys > ~/.VirtualBox/TFTP/BMOS.pxe
vboxmanage startvm BMOS 

