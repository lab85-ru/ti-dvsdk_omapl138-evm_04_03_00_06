# Makefile for VPIF smaple applications
# Command to build sample applications is
# make KERNEL_PATH=<kernel directory path>

CROSS_COMPILE=arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc -I $(KERNEL_PATH)/arch/arm/include -I $(KERNEL_PATH)/include

all:vpif_test_mmap_loopback vpif_test_userptr_loopback vpif_test_display

vpif_test_mmap_loopback: vpif_test_mmap_loopback.o
	$(CC) -o  vpif_test_mmap_loopback $^

vpif_test_userptr_loopback: vpif_test_userptr_loopback.o
	$(CC) -o  vpif_test_userptr_loopback $^

vpif_test_display: vpif_test_display.o
	$(CC) -o vpif_test_display $^

%.o:%.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o *~ vpif_test_mmap_loopback vpif_test_userptr_loopback vpif_test_display
