# Declare constants for the multiboot header.
.set MAGIC,    0xE85250D6       # 'magic number' lets bootloader find the header
.set ARCH,     0		# number determining the architecture for the os, 0 for i386 protected mode
.set HDR_LNGTH, multiboot_end - multiboot_begin		
				# total length of the header
.set CHECKSUM, -(MAGIC + ARCH + HDR_LNGTH) # checksum of above, to prove we are multiboot

# Tag to ask GRUB for a framebuffer
.set FB_TYPE, 5			# 5 is the tag type for framebuffer (see multiboot 1.6 spec for details)
.set FB_FLAGS, 0		# framebuffer setup is not optional
.set FB_SIZE, 20 		# size of the framebuffer tag in bytes
.set FB_WIDTH, 1280		# width (in pixels) of the wanted display
.set FB_HEIGHT, 720 		# height (in pixels) of the wanted display
.set FB_DEPTH, 32 		# requested nuber of bits per pixel

# Required end tag
.set ET_TYPE, 0			# type of the end tag
.set ET_FLAGS, 0
.set ET_SIZE, 8			# size of the end tag

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
multiboot_begin:
.long MAGIC
.long ARCH
.long HDR_LNGTH	#multiboot_end - multiboot_begin
.long CHECKSUM 	#-(MAGIC + ARCH + multiboot_end - multiboot_begin)
.align 8
.word FB_TYPE 			# start of the framebuffer tag
.word FB_FLAGS
.long FB_SIZE
.long FB_WIDTH
.long FB_HEIGHT
.long FB_DEPTH
.align 8
.word ET_TYPE 			# start of the end tag
.word ET_FLAGS
.long ET_SIZE
multiboot_end:

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
	push %ebx

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start
