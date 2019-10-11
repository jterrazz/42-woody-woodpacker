# 42-woody-woodpacker

Runtime packer for ELF binaries.

## Ressources

- [Understand ELF](https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html)

### Useful commands

Add a section to an ELF file
```
$ echo 'int main() { puts ("Hello world"); }' | gcc -x c - -c -o hello.o

$ echo "this is my special data" >mydata

$ objcopy --add-section .mydata=mydata \
          --set-section-flags .mydata=noload,readonly hello.o hello2.o

$ gcc hello2.o -o hello

$ ./hello
Hello world

$ objdump -sj .mydata hello
```

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

https://stackoverflow.com/questions/1647359/is-there-a-way-to-get-gcc-to-output-raw-binary

#### Get the bin file

```bash
cc -c test.c
objcopy -O binary test.o binfile
hexdump -C binfile 
```


