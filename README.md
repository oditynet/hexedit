# hexedit
Hex edit elf,exe files

```
gcc elf_fix.c -o elfpatch
```
It is my real practic :)
```
./elfpatch execute.exe 0x000bbea2 0x33
Байт по смещению 0xbbea2 успешно изменен на 0x33
xxd execute.exe|grep "302c.3333"      
00095840: 3d40 0000 0000 302c 3333 0000 382c 3600  =@....0,33..8,6.
000bbea0: 302c 3333 33c2 b728 3630 2dd0 b029 2e08  0,333..(60-..)..
...
```
