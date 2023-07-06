Tock Userland LibC
==================

[Newlib](https://sourceware.org/newlib/) is a port of the C standard library
designed for embedded systems. Tock uses a version of Newlib compiled to support
the position independent code that Tock applications require.

Compiling a new version of Newlib
---------------------------------

In the `Makefile`, edit the variable `NEWLIB_VERSION` with the version
you want to compile. The releases are listed
[here](http://sourceware.org/pub/newlib/).

Then:

    $ make

When the build finishes (it takes a while), a zip folder named
`libtock-newlib-<version>.zip` will contain the built libraries. You can move
that folder to the `libtock-c/lib` directory to use the new version of newlib.
