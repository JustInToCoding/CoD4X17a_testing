
echo Compiling C-code...

cd bin
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -c ../unix/sys_unix.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -c ../unix/sys_linux.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -c ../unix/elf32_parser.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -c ../unix/sys_cod4linker_linux.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -c ../unix/sys_con_tty.c
gcc -m32 -Wall -O0 -g -fno-omit-frame-pointer -mtune=prescott -I../lib_tomcrypt/headers -I../lib_tomcrypt/math/tommath -D PUNKBUSTER -c ../src/*.c 
cd ../

echo Compiling NASM...

nasm -f elf src/qcommon_hooks.asm    -o bin/qcommon_hooks.o
nasm -f elf src/cmd_hooks.asm        -o bin/cmd_hooks.o
nasm -f elf src/filesystem_hooks.asm -o bin/filesystem_hooks.o
nasm -f elf src/xassets_hooks.asm    -o bin/xassets_hooks.o
nasm -f elf src/trace_hooks.asm      -o bin/trace_hooks.o
nasm -f elf src/misc_hooks.asm       -o bin/misc_hooks.o
nasm -f elf src/scr_vm_hooks.asm     -o bin/scr_vm_hooks.o
nasm -f elf src/g_sv_hooks.asm       -o bin/g_sv_hooks.o
nasm -f elf src/server_hooks.asm     -o bin/server_hooks.o
nasm -f elf src/msg_hooks.asm        -o bin/msg_hooks.o
nasm -f elf src/punkbuster_hooks.asm -o bin/punkbuster_hooks.o
nasm -f elf src/pluginexports.asm    -o bin/pluginexports.o

echo Linking...
gcc -m32 -rdynamic -Tlinkerscript.ld -o bin/cod4x17a_dedrun bin/*.o -Llib/ -ltomcrypt_linux -ltommath_linux -ldl -lpthread -lm -lstdc++

rm bin/*.o

./version_make_progress.sh
