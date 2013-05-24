CC=gcc
LD=ld
DD=dd
CFLAGS=-c -Wall -I ~/code/bmos/newlib/newlib-2.0.0/newlib/libc/include -D _NON_BAREMETAL_
LDFLAGS=-T ~/code/bmos/newlib/app.ld
LIBDEPS=~/code/bmos/programs/libBareMetal.o ~/code/bmos/newlib/libc.a ~/code/bmos/newlib/crt0.o
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=out.app
BMFS=~/code/bmfs/bmfs
VMHD=/home/a/VirtualBox\ VMs/BMOS/BMOS.vmdk
BMFSMBR=/home/a/code/pure64/bmfs_mbr.sys
PXE=/home/a/code/pure64/pxestart.sys
PURE64=/home/a/code/pure64/pure64.sys
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

install:
	$(BMFS) $(VMHD) list
	$(BMFS) $(VMHD) delete $(EXECUTABLE)
	# need to calculate file size
	$(BMFS) $(VMHD) create $(EXECUTABLE) 2
	$(BMFS) $(VMHD) write $(EXECUTABLE)
	$(BMFS) $(VMHD) list

vm: install 
	#$(DD) if=/dev/zero of=$(VMHD) bs=512 count=32
	$(DD) if=$(BMFSMBR) of=$(VMHD) bs=512 conv=notrunc
	$(DD) if=$(PURE64) of=$(VMHD) bs=512 seek=16 conv=notrunc
	$(DD) if=$(BMOS) of=$(VMHD) bs=512 seek=1024 conv=notrunc

format:
	$(BMFS) $(VMHD) format

start:
	vboxmanage startvm $(VM)

stop:
	vboxmanage controlvm $(VM) poweroff

dhcp:
	$(CAT) $(PXE) $(PURE64) $(KERNEL64) > $(TFTP)
	$(CAT) $(PXE) $(PURE64) $(KERNEL64) > ~/.VirtualBox/TFTP/$(VM).pxe

clean:
	rm -rf *.o *.app $(EXECUTABLE)
	
