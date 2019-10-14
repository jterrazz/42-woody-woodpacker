# 42-woody-woodpacker

Runtime packer for ELF binaries.

## Ressources

- [ELF Header](https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html)
- [Add an ELF section in C (see last post)](https://stackoverflow.com/questions/1088128/adding-section-to-elf-file)

### Useful commands

#### Radare2

- [The base 1](https://medium.com/@jacob16682/reverse-engineering-using-radare2-588775ea38d5)
- [The base 2](https://medium.com/@jacob16682/reverse-engineering-with-radare2-part-2-83b71df7ffe4)

#### Compile a binary with nasm and dump his assembly code

```bash
$ nasm -f bin my_assembly_file.s -o my_binary
# Will compile the .s file into a raw binary

$ objdump -b binary -m i386:x86-64 -M intel -D my_binary
# Will dump the assembly code of the binary file for arch i386:x86-64 in intel syntax
```

#### Add an ELF section to an executable

*Doesn't modify the header*

```bash
$ echo "data to insert" > mydata
$ objcopy --add-section .mydata=mydata --set-section-flags .mydata=noload,readonly elf_exec new_exec
$ objdump -sj .mydata new_exec
```

#### Output raw binary from .c with gcc

```bash
$ cc -c test.c
$ objcopy -O binary test.o binfile

$ objdump -d test.o
# Will print binary with section

$ hexdump -C binfile 
# Will print the binary only
```

#### Other commands

```bash
lldb
radare2
readelf
strace
ltrace
objdump # -d test.o
objcopy # -O binary test.o binfile
strings
hexdump # -C
```


