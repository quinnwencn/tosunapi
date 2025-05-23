# tosunapi

`tosunapi` is a wrapper library for Tosun CAN device library.  In order to use this library, you must copy the library `libTSCANApiOnLinux.so`, `libTSH.so`provided by Tosun to `/usr/lib`, and copy the header `TSCANDef.h` to /usr/include

After compilation, you also need to copy the library provided by Tosun to the directory of your executable file. Otherwise, your executable won't work bacause Tosun dlopen `libTSH.so` in `libTSCANApiOnLinux.so`. This is a very bad practice but they are not willing to refine it.
