# Makefile for user application
STACK_SIZE = 16384
APP_HEAP_SIZE = 16384

# Specify this directory relative to the current application.
TOCK_USERLAND_BASE_DIR = ../..

# Specify this app depends on the MTD OpenThread library.
include $(TOCK_USERLAND_BASE_DIR)/libopenthread/libopenthread-mtd.mk

# Which files to compile.
C_SRCS := $(wildcard *.c)

# Include userland master makefile. Contains rules and flags for actually
# building the application.

### See compilation.md for details ###
# add the headers for hardware registers (DWT) and max optimizations
override CFLAGS += -I../timing/ -O0 
include $(TOCK_USERLAND_BASE_DIR)/AppMakefile.mk
