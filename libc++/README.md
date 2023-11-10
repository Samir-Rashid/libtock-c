C++ Libraries for libtock-c
===========================

Similarly to libc, we provide a pre-built version of the libc++ libraries so we
can compile with specific flags enabled.

Basic Instructions
------------------

You can create your own libc++ libraries by running `make` in this folder.

1. Build `newlib`.
2. Make sure the newlib version in `build.sh` matches.
3. Install dependencies:

        sudo apt install libmpc-dev

4. `make`.


Docker Instructions
-------------------

To help ensure reproducibility, we also include a Dockerfile which can be used
to create the libc++ libraries.

```bash
LIBCPP=11.2.0
cd libtock-c/libc++
docker build -t libtock-c-libcpp .
id=$(docker create libtock-c-libcpp)
docker cp $id:/libtock-c/libc++/libtock-libc++-$LIBCPP.zip libtock-libc++-$LIBCPP.zip
```
