GCC_VERSION ?= 11.2.0
NEWLIB_VERSION ?= 4.2.0.20211231

all: rebuild-gcc

# Determine which RISC-V toolchain is installed.
ifneq (,$(shell which riscv64-none-elf-gcc 2>/dev/null))
  TOOLCHAIN_rv32i := riscv64-none-elf
else ifneq (,$(shell which riscv32-none-elf-gcc 2>/dev/null))
  TOOLCHAIN_rv32i := riscv32-none-elf
else ifneq (,$(shell which riscv64-elf-gcc 2>/dev/null))
  TOOLCHAIN_rv32i := riscv64-elf
else ifneq (,$(shell which riscv64-unknown-elf-clang 2>/dev/null))
  TOOLCHAIN_rv32i := riscv64-unknown-elf
else ifneq (,$(shell which riscv32-unknown-elf-clang 2>/dev/null))
  TOOLCHAIN_rv32i := riscv32-unknown-elf
else
  TOOLCHAIN_rv32i := riscv64-unknown-elf
endif

# We assume we are using newlib when creating libc++.
NEWLIB_INCLUDE_PATH ?= ../newlib-$(NEWLIB_VERSION)/newlib/libc/include

gcc-$(GCC_VERSION).tar.xz:
	@echo "Downloading gcc source $(@F)"
	@wget -q -O $@ 'https://bigsearcher.com/mirrors/gcc/releases/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz'

gcc-$(GCC_VERSION): gcc-$(GCC_VERSION).tar.xz | newlib-$(NEWLIB_VERSION)
	@echo "Extracting $(<F)"
	@tar -xf $<
	@touch $@ # Touch so directory appears newer than tarball

newlib-$(NEWLIB_VERSION).tar.gz:
	@echo "Downloading newlib source $(@F)"
	@wget -q -O $@ ftp://sourceware.org/pub/newlib/newlib-$(NEWLIB_VERSION).tar.gz

newlib-$(NEWLIB_VERSION): newlib-$(NEWLIB_VERSION).tar.gz newlib-$(NEWLIB_VERSION)
	@echo "Extracting $(<F)"
	@tar -xzf $<
	@touch $@ # Touch so directory appears newer than tarball

rebuild-gcc: gcc-$(GCC_VERSION)
	@echo ""
	@echo "=== BEGINNING ARM BUILD =========================="
	@echo ""
	@mkdir -p gcc-arm-$(GCC_VERSION)-out
	@echo "Entering directory gcc-arm-$(GCC_VERSION)-out"
	cd gcc-arm-$(GCC_VERSION)-out; ../build-arm.sh ../$< $(NEWLIB_INCLUDE_PATH)
	@echo ""
	@echo "=== BEGINNING RISC-V BUILD =========================="
	@echo ""
	@mkdir -p gcc-riscv-$(GCC_VERSION)-out
	@echo "Entering directory gcc-riscv-$(GCC_VERSION)-out"
	cd gcc-riscv-$(GCC_VERSION)-out; ../build-riscv.sh ../$< $(NEWLIB_INCLUDE_PATH) $(TOOLCHAIN_rv32i)
	@echo ""
	@echo "=== PACKAGING LIBC++ ARTIFACTS =========================="
	@echo ""
	@mkdir -p libtock-libc++-$(GCC_VERSION)/thumb
	@mkdir -p libtock-libc++-$(GCC_VERSION)/riscv
	@rsync -ram --include='libgcc.a' --include='*/' --exclude='*' gcc-arm-$(GCC_VERSION)-out/arm-none-eabi/thumb/ libtock-libc++-$(GCC_VERSION)/thumb/
	@rsync -ram --include='libstdc++.a' --include='libsupc++.a' --include='*/' --exclude='*' gcc-arm-$(GCC_VERSION)-out/arm-none-eabi/thumb/ libtock-libc++-$(GCC_VERSION)/thumb/
	@rsync -ram --include='libgcc.a' --include='*/' --exclude='*' gcc-riscv-$(GCC_VERSION)-out/$(TOOLCHAIN_rv32i)/ libtock-libc++-$(GCC_VERSION)/riscv/
	@rsync -ram --include='libstdc++.a' --include='libsupc++.a' --include='*/' --exclude='*' gcc-riscv-$(GCC_VERSION)-out/$(TOOLCHAIN_rv32i)/ libtock-libc++-$(GCC_VERSION)/riscv/
	@zip -r libtock-libc++-$(GCC_VERSION).zip libtock-libc++-$(GCC_VERSION)


clean:
	rm -f gcc-$(GCC_VERSION).tar.xz
	rm -rf gcc-$(GCC_VERSION)
	rm -rf gcc-$(GCC_VERSION)-out

.PHONY: clean rebuild-gcc

