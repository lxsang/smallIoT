
ARMGNU ?= arm-none-eabi
BUILD_DIR=./build
ASFLAGS = --warn -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=softfp
FLAGS =  -O2 -ffreestanding \
		-nostartfiles \
		-marm -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=softfp \
		-std=c99 -DUSE_UART_AS_TTY\
		-DUSE_X_MODEM

FS_SRC = fs/fatfs/ff.c \
		fs/emmc.c \
		fs/fatfs/diskio.c \
		fs/fatfs/option/unicode.c\
		fs/fatfs/option/fs_syscall.c
		
IO_SRC = io/uart.c

PRIV_S = vm/primitives/primitive.c \
		vm/primitives/ioprimitive.c

VM_SRC = vm/gdef.c\
		vm/interp.c\
		vm/io.c\
		vm/free_list.c\
		vm/memory_v1.c\
		vm/names.c\
		vm/news.c\
		$(PRIV_S)
		
UTILS = utils/tty.c \
		utils/xmodem.c\
		utils/kilo.c
		
SRC = 	start.s \
		platform.c \
		syscalls.c \
		wiring.c\
		$(FS_SRC) \
		$(IO_SRC) \
		$(UTILS) \
		$(VM_SRC) \
		kernel.c 
OBJS = $(SRC:%=%.o)
PRJ_NAME = kernel
MAP_FILE = raspberry.ld
LIB = -L/Users/mrsang/Documents/ushare/cwp/ARM/std/arm-none-eabi/lib/ \
	 -L/Users/mrsang/private-app/gcc-arm-none-eabi-4_9/lib/gcc/arm-none-eabi/4.9.3/ \
	 -lc -lgcc -lm

system: vmimage all
	
vmimage:
	make clean
	cd vm/imgbuild/ && make clean && make

all : image

image: bin
	cp $(BUILD_DIR)/$(PRJ_NAME).bin $(BUILD_DIR)/$(PRJ_NAME).img
	cp $(BUILD_DIR)/$(PRJ_NAME).bin $(BUILD_DIR)/$(PRJ_NAME)7.img

%.s.o:%.s
	$(ARMGNU)-as $(ASFLAGS) $< -o $@

%.c.o:%.c
	$(ARMGNU)-gcc $(FLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)
	rm -f $(BUILD_DIR)/*.bin
	rm -f $(BUILD_DIR)/*.hex
	rm -f $(BUILD_DIR)/*.elf
	rm -f $(BUILD_DIR)/*.list
	rm -f $(BUILD_DIR)/*.img

elf: $(OBJS)
	$(ARMGNU)-ld $(OBJS) -T $(MAP_FILE) -o $(BUILD_DIR)/$(PRJ_NAME).elf $(LIB) 
	$(ARMGNU)-objdump -D $(BUILD_DIR)/$(PRJ_NAME).elf > $(BUILD_DIR)/$(PRJ_NAME).list
	rm -f *.o


bin : elf
	$(ARMGNU)-objcopy $(BUILD_DIR)/$(PRJ_NAME).elf -O binary $(BUILD_DIR)/$(PRJ_NAME).bin

hex : elf
	$(ARMGNU)-objcopy $(BUILD_DIR)/$(PRJ_NAME).elf -O ihex $(BUILD_DIR)/$(PRJ_NAME).hex
	




