## Q1 ##
是`uintptr_t`类型的，因为这里使用了`*`操作符进行解析，而`physaddr_t`类型不需要怎样

---

## Q2 ##

| Entry | Base Virtual Address | Points to|
|:---|:------:|:----:|
| 960 | KERNBASE  0xef000000  |   kernel       |
| 958 | KSTACKTOP - KSTKSIZE  0xefbf8000  |   kernel stack  |
| 956 | UPAGES  0xef000000 | pages |

---

## Q3 ##
- 因为用户程序修改内核可能破坏内核，使得操作系统崩溃
- 通过在页表项中设置一个bit代表是user还是supervisor，在JOS中，设置成PTE_U即可设置成用户模式

---

## Q4 ##
- 最多为2GB
- 因为这里`UPAGES`最多为`4MB`，而一个页结构是`8B`，所以最多有`4*1024/8=512`的页。一个物理页为`4KB`，所以最多
为`4KB*512K=2GB`

---

## Q5 ##
- 6MB+4KB,其中4MB是存512个page结构，2MB是页表，4KB是页目录表大小

## Q6 ##
- Where：在`entry.S`文件中指令 `jmp *%eax`，将EIP的值设置为寄存器eax中的值
- Possible：在`entry_pgdir.c`中，完成了虚拟地址空间[0, 4MB)到物理地址空间[0, 4MB)的映射
- Necessary：必要点在于kernel将会被映射到高地址


