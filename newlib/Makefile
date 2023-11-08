NEWLIB_VERSION ?= 4.2.0.20211231

all: rebuild-newlib

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

newlib-$(NEWLIB_VERSION).tar.gz:
	@echo "Downloading newlib source $(@F)"
	@wget -q -O $@ ftp://sourceware.org/pub/newlib/newlib-$(NEWLIB_VERSION).tar.gz

newlib-$(NEWLIB_VERSION): newlib-$(NEWLIB_VERSION).tar.gz
	@echo "Extracting $(<F)"
	@tar -xzf $<
	@touch $@ # Touch so directory appears newer than tarball

rebuild-newlib: newlib-$(NEWLIB_VERSION)
	@echo ""
	@echo "=== BEGINNING ARM BUILD =========================="
	@echo ""
	@rm -rf newlib-arm-$(NEWLIB_VERSION)-out
	@mkdir -p newlib-arm-$(NEWLIB_VERSION)-out
	@echo "Entering directory newlib-arm-$(NEWLIB_VERSION)-out"
	cd newlib-arm-$(NEWLIB_VERSION)-out; ../build-arm.sh ../$<
	@echo ""
	@echo "=== BEGINNING RISC-V BUILD ======================="
	@echo ""
	@rm -rf newlib-riscv-$(NEWLIB_VERSION)-out
	@mkdir -p newlib-riscv-$(NEWLIB_VERSION)-out
	@echo "Entering directory newlib-riscv-$(NEWLIB_VERSION)-out"
	cd newlib-riscv-$(NEWLIB_VERSION)-out; ../build-riscv.sh ../$< $(TOOLCHAIN_rv32i)
	@echo ""
	@echo "=== PACKAGING NEWLIB ARTIFACTS ==================="
	@echo ""
	@mkdir -p libtock-newlib-$(NEWLIB_VERSION)/thumb
	@mkdir -p libtock-newlib-$(NEWLIB_VERSION)/riscv
	@cp -r newlib-$(NEWLIB_VERSION)/newlib/libc/include libtock-newlib-$(NEWLIB_VERSION)/headers
	@rsync -ram --exclude='/*/*/*/*/' --include='libm.a' --include='libc.a' --include='*/' --exclude='*' newlib-arm-$(NEWLIB_VERSION)-out/arm-none-eabi/thumb/ libtock-newlib-$(NEWLIB_VERSION)/thumb/
	@rsync -ram --exclude='/*/*/*/*/' --include='libm.a' --include='libc.a' --include='*/' --exclude='*' newlib-riscv-$(NEWLIB_VERSION)-out/$(TOOLCHAIN_rv32i)/ libtock-newlib-$(NEWLIB_VERSION)/riscv/
	@patch -p1 < newlib-$(NEWLIB_VERSION).patch
	@zip -r libtock-newlib-$(NEWLIB_VERSION).zip libtock-newlib-$(NEWLIB_VERSION)
	@echo ""
	@echo "=== FINISHED NEWLIB ZIP ==================="
	@echo ""
	@sha256sum libtock-newlib-$(NEWLIB_VERSION).zip
