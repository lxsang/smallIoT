CC=gcc
UNAME_S := $(shell uname -s)
ADV_INPUT := True
PF_FLAG=-D_GNU_SOURCE -DIMG_BUILD
BUILDIRD= ../../build
VMLIBS = -lm

SYS_PRIV =
VMDEP = ../free_list.o\
		../memory_v1.o \
		../names.o \
		../news.o \
		../interp.o \
		../primitives/primitive.o \
		../primitives/ioprimitive.o \
		../io.o \
		../gdef.o \
		#$(SYS_PRIV) 
IMG_BUILD_DEP = fileio.o \
				initial.o \
				lex.o \
				parser.o
			
ST_BOOTSTRAP =  basic.st \
				Block.st \
				Context.st \
				Process.st \
				Smalltalk.st \
				UndefinedObject.st \
				mag.st \
				Boolean.st \
				Method.st \
				collect.st \
				Class.st \
				Object.st \
				Symbol.st \
				Scheduler.st \
				../optional/parser.st\
				../optional/Serial.st\
				../optional/GPIO.st\
				../optional/File.st #\
				tty.st \
				../optional/Date.st \
				../optional/JSONParser.st
				
CFLAGS=-W -Wall -g -std=c99 -D DEBUG  $(PF_FLAG)
main:  $(VMDEP) $(IMG_BUILD_DEP) buildimage

%.o: %.c
		$(CC) $(CFLAGS) -I$(PLUGINS_BASE) -I$(STROS_BASE) -c $< -o $@
clean:
		-rm -f *.o ../*.o *.o  $(BUILDIRD)/image.im


.PRECIOUS: %.o

buildimage:
	-mkdir $(BUILDIRD)
	$(CC) $(CFLAGS) $(VMDEP) $(IMG_BUILD_DEP) -o build_image $(VMLIBS)
	@cd bootstrap && ../build_image $(ST_BOOTSTRAP)
	@mv bootstrap/systemImage $(BUILDIRD)/image.im
	