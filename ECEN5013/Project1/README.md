#ECEN5013

##Project 1

###Makefile commands:

`make [cmd] [ARCH=[x86_64, host, bbb, frdm]]` - ARCH argument can be used to specify target architecture for any command

`build (default)` - compiles and links all files

`preprocess` - Proprocesses all files

`asm-file` - generates assembly output of all files and any single file

`%.o` - Individually compiles a single object file based on the name of the file

`compile-all` - Compiles all object files but does not link

`upload [IP_ADDR=XXX.XXX.XXX.XXX]` - Uploads the executable image to the BeagleBone Black at 192.168.1.111 (or IP_ADDR) to the debian user account.

`clean` -  cleans all generated files

`build-lib` - generates a library of memory and data functions to libproject1.a

