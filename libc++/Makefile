GCC_VERSION ?= 12.2.0

all: rebuild-gcc

gcc-$(GCC_VERSION).tar.xz:
	@echo "Downloading gcc source $(@F)"
	@wget -q -O $@ 'https://bigsearcher.com/mirrors/gcc/releases/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz'

gcc-$(GCC_VERSION): gcc-$(GCC_VERSION).tar.xz
	@echo "Extracting $(<F)"
	@tar -xf $<
	@touch $@ # Touch so directory appears newer than tarball

rebuild-gcc: gcc-$(GCC_VERSION)
	@echo ""
	@echo "=== BEGINNING ARM BUILD =========================="
	@echo ""
	@mkdir -p gcc-arm-$(GCC_VERSION)-out
	@echo "Entering directory gcc-arm-$(GCC_VERSION)-out"
	cd gcc-arm-$(GCC_VERSION)-out; sh ../build-arm.sh ../$<
	@echo ""
	@echo "=== BEGINNING RISC-V BUILD =========================="
	@echo ""
	@mkdir -p gcc-riscv-$(GCC_VERSION)-out
	@echo "Entering directory gcc-riscv-$(GCC_VERSION)-out"
	cd gcc-rv32i-$(GCC_VERSION)-out; sh ../build-riscv.sh ../$<
	@echo ""
	@echo "=== PACKAGING LIBC++ ARTIFACTS =========================="
	@echo ""
	@mkdir -p libtock-libc++-$(GCC_VERSION)/thumb
	@mkdir -p libtock-libc++-$(GCC_VERSION)/riscv
	@rsync -ram --include='libgcc.a' --include='*/' --exclude='*' gcc-arm-$(GCC_VERSION)-out/arm-none-eabi/thumb/ libtock-libc++-$(GCC_VERSION)/thumb/
	@rsync -ram --include='libstdc++.a' --include='libsupc++.a' --include='*/' --exclude='*' gcc-arm-$(GCC_VERSION)-out/arm-none-eabi/thumb/ libtock-libc++-$(GCC_VERSION)/thumb/
	@rsync -ram --include='libgcc.a' --include='*/' --exclude='*' gcc-riscv-$(GCC_VERSION)-out/riscv64-elf/ libtock-libc++-$(GCC_VERSION)/riscv/
	@rsync -ram --include='libstdc++.a' --include='libsupc++.a' --include='*/' --exclude='*' gcc-riscv-$(GCC_VERSION)-out/riscv64-elf/ libtock-libc++-$(GCC_VERSION)/riscv/
	@zip -r libtock-libc++-$(GCC_VERSION).zip libtock-libc++-$(GCC_VERSION)


clean:
	rm -f gcc-$(GCC_VERSION).tar.xz
	rm -rf gcc-$(GCC_VERSION)
	rm -rf gcc-$(GCC_VERSION)-out

.PHONY: clean rebuild-gcc

