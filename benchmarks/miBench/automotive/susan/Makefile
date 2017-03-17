CC=gcc
susan: susan.c Makefile
	$(CC) -v -static -O4 -o susan susan.c -lm 

#TOOLCHAIN_PATH=/home/ramiro/Documents/Twelve_Semester/Research/gem5-pipe/alphaev67-unknown-linux-gnu
#PREFIX=alphaev67-unknown-linux-gnu
#CC=$(TOOLCHAIN_PATH)/bin/$(PREFIX)-gcc

# TOOLCHAIN_INCLUDE_DIR=$(TOOLCHAIN_PATH)/include
# TOOLCHAIN_LIB_DIR=$(TOOLCHAIN_PATH)/lib
# LIBRARY_PATH=$(TOOLCHAIN_PATH)/include
 FILENAME=susan

# CC=$(TOOLCHAIN_PATH)/bin/alphaev67-unknown-linux-gnu-gcc
# OBJCOPY=$(TOOLCHAIN_PATH)/bin/alphaev67-unknown-linux-gnu-objcopy
# OBJDUMP=$(TOOLCHAIN_PATH)/bin/alphaev67-unknown-linux-gnu-objdump
# LD=$(TOOLCHAIN_PATH)/bin/alphaev67-unknown-linux-gnu-ld

# LINKER_FLAGS= -Xlinker -o$(FILENAME).elf -Xlinker -Map=$(FILENAME).map
# # -mno-compact-eh

# DEBUG=-g
# STATIC=-static
# OPTIM=-O2
# VERBOSE=-v
# CFLAGS=$(VERBOSE) $(DEBUG) $(STATIC) $(OPTIM) -I. -I $(TOOLCHAIN_INCLUDE_DIR) -mcpu=ev67
# #CFLAGS=-mieee -mcpu=ev67 -Wa,-mev6 -O2 -pipe 
# #-mno-compact-eh -lm
# SOURCE=susan.c

# OBJS = $(SOURCE:.c=.o)

# all: $(FILENAME).elf $(FILENAME).lst $(FILENAME).bin
	
# $(FILENAME).lst : $(FILENAME).elf
# 	$(OBJDUMP) -S $(FILENAME).elf >  $(FILENAME).lst

# $(FILENAME).bin : $(FILENAME).elf
# 	$(OBJCOPY) $(FILENAME).elf -O binary $(FILENAME).bin

# $(FILENAME).elf : $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LINKER_FLAGS) -lm

# $(OBJS) : %.o : %.c 
# 	$(CC) -c $(CFLAGS) $< -o $@ -lm
	
 clean :
	rm -rf $(OBJS)	
	rm -rf $(FILENAME).o
	rm -rf $(FILENAME).elf
	rm -rf $(FILENAME).bin
	rm -rf $(FILENAME).lst	
	rm -rf $(FILENAME).map
	rm -rf $(FILENAME).txt
	rm -rf susan output*
