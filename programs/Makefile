CC=gcc
LD=ld
DD=dd
#CFLAGS=-c -Wall -I ~/code/compiler-rt/lib -D _NON_BAREMETAL_
CFLAGS=-c -Wall -m64 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -I ~/code/bmos/newlib/newlib-2.0.0/newlib/libc/include/ -D _NON_BAREMETAL_
LDFLAGS=-T ~/code/bmos/newlib/app.ld
#LIBDEPS=~/code/bmos/programs/libBareMetal.o ~/code/bmos/newlib/crt0.o libcompiler_rt.a
LIBDEPS=~/code/bmos/programs/libBareMetal.o ~/code/bmos/newlib/crt0.o ~/code/bmos/newlib/libc.a 
SOURCES=testx.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(SOURCES:.c=.app)
BMFS=~/code/bmfs/bmfs
BMFSMBR=/home/a/code/pure64/bmfs_mbr.sys
VMHD=/home/a/VirtualBox\ VMs/BMOS/BMFS-256-flat.vmdk
#VMHD=/home/a/VirtualBox\ VMs/BMOS/BMOS-flat.vmdk
PXE=/home/a/code/pure64/pxestart.sys
PURE64D=/home/a/code/pure64
PURE64=/home/a/code/pure64/pure64.sys
BMOSD=/home/a/code/bmos/os
BMOS=/home/a/code/bmos/os/kernel64.sys
CAT=cat
TFTP=/var/lib/tftpboot/pxeboot.bin
VM=BMOS

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
	#$(DD) if=/dev/zero of=$(VMHD) bs=512 count=32
	$(DD) if=$(BMFSMBR) of=$(VMHD) bs=512 conv=notrunc
	$(CAT) $(PURE64) $(BMOS) > software.sys
	$(DD) if=software.sys of=$(VMHD) bs=512 seek=16 conv=notrunc
	#$(DD) if=$(PURE64) of=$(VMHD) bs=512 seek=16 conv=notrunc
	#$(DD) if=$(BMOS) of=$(VMHD) bs=1 seek=1048576 conv=notrunc
	#$(DD) if=$(BMOS) of=$(VMHD) bs=1 seek=6145 conv=notrunc
	#$(DD) if=$(BMOS) of=$(VMHD) bs=512 seek=32 conv=notrunc


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

remake: clean all install

create:
	cd $(PURE64D); $(PURE64D)/build.sh
	cd $(BMOSD);	nasm kernel64.asm -o kernel64.sys
