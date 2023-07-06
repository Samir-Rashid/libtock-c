################################################################################
##
## libtock-c build system rules for pre-compiled libraries.
##
## This includes rules to require that pre-compiled libraries be present in the
## `/lib` folder.
##
## Supported pre-compiled libraries:
##
## - newlib
## - libc++
##
################################################################################

# Ensure that this file is only included once.
ifndef PRECOMPILED_MAKEFILE
PRECOMPILED_MAKEFILE = 1

# Target to download and extract newlib.
$(TOCK_USERLAND_BASE_DIR)/lib/libtock-newlib-$(NEWLIB_VERSION):
	cd $(TOCK_USERLAND_BASE_DIR)/lib; ./fetch-newlib.sh $(NEWLIB_VERSION)

# We use a custom newlib that is pre-compiled.
newlib: | $(TOCK_USERLAND_BASE_DIR)/lib/libtock-newlib-$(NEWLIB_VERSION)

# Target to download and extract the C++ libraries.
$(TOCK_USERLAND_BASE_DIR)/lib/libtock-libc++-$(LIBCPP_VERSION):
	cd $(TOCK_USERLAND_BASE_DIR)/lib; ./fetch-libc++.sh $(LIBCPP_VERSION)

# We use custom C++ libraries that are pre-compiled.
libc++: | $(TOCK_USERLAND_BASE_DIR)/lib/libtock-libc++-$(LIBCPP_VERSION)

endif
