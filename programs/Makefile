SOURCES=testx.c
BMFS=/home/a/code/bmfs/bmfs
BMOSD=/home/a/code/bmos/os
VMHD=/home/a/VirtualBox\ VMs/BMOS/BMFS-256-flat.vmdk
PURE64D=/home/a/code/pure64
NEWLIBD=/home/a/code/bmos/newlib
TFTP=/var/lib/tftpboot/pxeboot.bin
VM=BMOS

CAT=cat
CC=gcc
LD=ld
DD=dd
NASM=nasm
CFLAGS=-c -Wall -m64 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -I ~/code/bmos/newlib/newlib-2.0.0/newlib/libc/include/ -D _NON_BAREMETAL_
LDFLAGS=-T ~/code/bmos/newlib/app.ld
#LIBDEPS=~/code/bmos/programs/libBareMetal.o ~/code/bmos/newlib/crt0.o libcompiler_rt.a
LIBDEPS=./libBareMetal.o $(NEWLIBD)/crt0.o $(NEWLIBD)/build/x86_64-pc-baremetal/newlib/libc.a 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(SOURCES:.c=.app)
PXE=$(PURE64D)/pxestart.sys
PURE64=$(PURE64D)/pure64.sys
BMFSMBR=$(PURE64D)/bmfs_mbr.sys
BMOS=$(BMOSD)/kernel64.sys


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBDEPS) -o $@

.c.o:
	$(CC) -c -m64 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o libBareMetal.o libBareMetal.c
	$(CC) $(CFLAGS) $< -o $@
	#clang -c -mno-red-zone -integrated-as -target x86_64-multi-arch-m64 -o helloc.o helloc.c

install: all
	$(BMFS) $(VMHD) list
	$(BMFS) $(VMHD) delete $(EXECUTABLE)
	# need to calculate file size
	$(BMFS) $(VMHD) create $(EXECUTABLE) 2
	$(BMFS) $(VMHD) write $(EXECUTABLE)
	$(BMFS) $(VMHD) list

vm: install 
	$(DD) if=$(BMFSMBR) of=$(VMHD) bs=512 conv=notrunc
	$(CAT) $(PURE64) $(BMOS) > software.sys
	$(DD) if=software.sys of=$(VMHD) bs=512 seek=16 conv=notrunc


format:
	$(BMFS) $(VMHD) format

start:
	vboxmanage startvm $(VM)

stop:
	vboxmanage controlvm $(VM) poweroff

reset:
	cp -r ~/VirtualBox\ VMs/_backup/ ~/VirtualBox\ VMs/BMOS/

dhcp:
	$(CAT) $(PXE) $(PURE64) $(BMOS) > $(TFTP)
	$(CAT) $(PXE) $(PURE64) $(BMOS) > ~/.VirtualBox/TFTP/$(VM).pxe

clean:
	rm -rf *.o *.app $(EXECUTABLE) *.sys

create:
	cd $(NEWLIBD)/build; make
	cd $(NEWLIBD); $(CC) -I $(NEWLIBD)/newlib-2.0.0/newlib/libc/include/ -c crt0.c -o crt0.o
	cd $(PURE64D); $(PURE64D)/build.sh
	cd $(BMOSD);	$(NASM) kernel64.asm -o kernel64.sys

remake: clean create all vm

