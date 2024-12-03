# Makefile for user application
STACK_SIZE = 16384
APP_HEAP_SIZE = 16384

# Specify this directory relative to the current application.
TOCK_USERLAND_BASE_DIR = ../..

# Which files to compile.
C_SRCS := $(wildcard *.c)

# Include userland master makefile. Contains rules and flags for actually
# building the application.

### See compilation.md for details ###
# add the headers for hardware registers (DWT) and max optimizations
# WRANGLING MAKEFILES IS SO INFURIATING
# I GIVE UP ON MAKING A PROPER DEPENDENCY
override CFLAGS += -I../timing/ -I./ -O0 
include $(TOCK_USERLAND_BASE_DIR)/AppMakefile.mk
