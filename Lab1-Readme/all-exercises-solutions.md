## Exercise 2

```
[f000:fff0]    0xffff0:	ljmp   $0xf000,$0xe05b
```
> 跳转到 0xfe05b。这是启动后BIOS执行的第一条指令，会跳转到0xfe05b地址处。而这个地址是根据0xf000<<4+0xe05b计算出的。原因是启动的时候是
处于实模式下，实模式被特殊定义为20位地址内存可访问空间上，8086有20根地址线，所以它们只能访问1MB的内存空间。但是寄存
器为16位的，所以由16位的段地址和16位的段内偏移地址组成了实际的物理地址。

```
[f000:e05b]    0xfe05b:	jmp    0xfc85e
```
> 跳转到 0xfc85e。不知道为什么这里直接就跳转了

```
[f000:c85e]    0xfc85e:	mov    %cr0,%eax
[f000:c861]    0xfc861:	and    $0x9fffffff,%eax
[f000:c867]    0xfc867:	mov    %eax,%cr0
```
> cr0寄存器是控制寄存器，上述三个语句的作用是将cr0的29位和30位设为0。29位为NW(Not Writethough)，设为0表示Writethough即写操作根本不使用缓存，数据总是直接写入磁盘。30位为CD(Cache Disabled)，设为0表示使用缓存。所以表示读有缓存，而写不用缓存

```
[f000:c86a]    0xfc86a:	cli
```
> 禁止中断，防止当前代码执行被CPU交换出去

```
[f000:c86b]    0xfc86b:	cld
```
> 设置方向标识位为0，表示后续的串操作如MOV操作，内存地址的变化方向，如果为0代表从低地址值变为高地址

```
[f000:c86c]    0xfc86c:	mov    $0x8f,%eax
[f000:c872]    0xfc872:	out    %al,$0x70
[f000:c874]    0xfc874:	in     $0x71,%al
```
> in和out用于操作io端口，0x70端口和0x71端口是用于控制系统中CMOS设备，它可以用于在计算机关闭时存储一些信息。还可包括时钟设备（Real Time Clock） ，还可以控制是否响应不可屏蔽中断NMI(Non-Maskable Interrupt)。操作CMOS存储器中的内容需要两个端口，一个是0x70另一个就是0x71。其中0x70可以叫做索引寄存器，这个8位寄存器的最高位是不可屏蔽中断(NMI)使能位。如果你把这个位置1，则NMI不会被响应。低7位用于指定CMOS存储器中的存储单元地址。这里即表示禁用NMI，并且使用第1-7号储存单元，并将值读取出来。参考http://bochs.sourceforge.net/techspec/PORTS.LST

```
[f000:c876]    0xfc876:	cmp    $0x0,%al
[f000:c878]    0xfc878:	jne    0xfc88d
```
> 这里将al中的值与0比较，并在不等的时候跳转。这里并没有跳转

```
[f000:c87a]    0xfc87a:	xor    %ax,%ax
[f000:c87c]    0xfc87c:	mov    %ax,%ss
```
> 将ax和ss置零

```
[f000:c87e]    0xfc87e:	mov    $0x7000,%esp
[f000:c884]    0xfc884:	mov    $0xf4b2c,%edx
```
> 给esp和edx赋值

```
[f000:c88a]    0xfc88a:	jmp    0xfc719
[f000:c719]    0xfc719:	mov    %eax,%ecx
```
> 跳转后将eax的值赋予ecx

```
[f000:c71c]    0xfc71c:	cli
[f000:c71d]    0xfc71d:	cld
```
> 这里同上的解释

```
[f000:c71e]    0xfc71e:	mov    $0x8f,%eax
[f000:c724]    0xfc724:	out    %al,$0x70
[f000:c726]    0xfc726:	in     $0x71,%al
[f000:c728]    0xfc728:	in     $0x92,%al
[f000:c72a]    0xfc72a:	or     $0x2,%al
[f000:c72c]    0xfc72c:	out    %al,$0x92
```
> 这里又操作了一次0x70端口，不知道为什么还需要一次，可能与前面两条指令有关。接下来是操作0x92端口，这个端口控制
的是 PS/2系统控制端口A，这里是将bit1置一，bit 1= 1 indicates A20 active，所以这里是打开了A20门，为进入保护模式
做好准备

```
[f000:c72e]    0xfc72e:	lidtw  %cs:-0x31cc
```
> 加载中断向量表寄存器IDTR

```
[f000:c734]    0xfc734:	lgdtw  %cs:-0x3188
```
> 加载全局描述符表格寄存器GDTR，这里在后面切换为保护模式的时候很关键

```
[f000:c73a]    0xfc73a:	mov    %cr0,%eax
[f000:c73d]    0xfc73d:	or     $0x1,%eax
[f000:c741]    0xfc741:	mov    %eax,%cr0
```
> 这里将cr0的第0位设置为1。第0位是PE位，启动保护位，当该位被置1，代表开启了保护模式


```
[f000:c744]    0xfc744:	ljmpl  $0x8,$0xfc74c

The target architecture is assumed to be i386
=> 0xfc74c:	mov    $0x10,%eax
0xfc751:	mov    %eax,%ds
0xfc753:	mov    %eax,%es
0xfc755:	mov    %eax,%ss
0xfc757:	mov    %eax,%fs
0xfc759:	mov    %eax,%gs
0xfc75b:	mov    %ecx,%eax
```
> 初始化段寄存器

```
0xfc75d:	jmp    *%edx
0xf4b2c:	push   %edi
0xf4b2d:	push   %esi
0xf4b2e:	sub    $0x4,%esp
0xf4b31:	xor    %edx,%edx
0xf4b33:	mov    %dl,%al
0xf4b35:	out    %al,$0xd
0xf4b37:	out    %al,$0xda
0xf4b39:	mov    $0xc0,%al
0xf4b3b:	out    %al,$0xd6
0xf4b3d:	mov    %dl,%al
0xf4b3f:	out    %al,$0xd4
0xf4b41:	push   $0xf5ebc
0xf4b46:	push   $0xf5639
0xf4b4b:	call   0xf3350l
```
> 开始调用函数,不知道在干嘛了。。

## Exercise 3

1. 从实模式切换到保护模式首先需要加载全局描述符表格寄存器GDTR并设置控制寄存器cr0的保护位,即下列四行指令
```
0x7c1e: lgdtw 0x7c64
0x7c23:      mov    %cr0,%eax
0x7c26:      or     $0x1,%eax
0x7c2a:      mov    %eax,%cr0
```
完成切换执行的指令是`0x7c2d: ljmp $0x8,$0x7c32`，这条指令后的指令就在32-bits模式下执行

2. boot loader执行的最后一条语句是
```
((void (*)(void)) (ELFHDR->e_entry))();
7d63:	ff 15 18 00 01 00    	call   *0x10018
```
这条指令后即跳转到内核的起始语句处，而内核的起始语句为`movw $0x1234, 0x472`

3. 在`kernel.asm`中可以发现第一条指令为`f010000c:	66 c7 05 72 04 00 00 	movw   $0x1234,0x472`

4. Boot loader通过ELF header中Program Header Table知道当前有多少个段以及每个段的具体信息。通过这些信息可以循环地将段读入内存中

## Exercise 4
```
1: a = 0xbfbb0df4, b = 0x953b008, c = 0x913324
2: a[0] = 200, a[1] = 101, a[2] = 102, a[3] = 103
3: a[0] = 200, a[1] = 300, a[2] = 301, a[3] = 302
4: a[0] = 200, a[1] = 400, a[2] = 301, a[3] = 302
5: a[0] = 200, a[1] = 128144, a[2] = 256, a[3] = 302
6: a = 0xbfbb0df4, b = 0xbfbb0df8, c = 0xbfbb0df5
```

## Exercise 5
将链接地址修改为0x7D00，在运行到0x0x7c1e时候，此时指令为`lgdtw 0x7d64`，这里是0x7d64后的六个单元的值拷贝入全局描述符表寄存器GDTR，
然而这里读取的值与正常0x7c64后的值不同，导致了从16到32bits的长跳转出错。

## Exercise 6
进入Boot loader之前，内存中值为空。而运行到内核开始的时候，值不为空，因为bootmain将内核中的某一段加载了进来，这里应该是指令段（.text）

## Exercise 7
```
0x100025:	mov    $0xf010002c,%eax
0x10002a:	jmp    *%eax
```
这里由于没有进行分页管理，所以没有进行从虚拟内存找物理内存的转换，所以出错

## Exercise 8
1. - `console.c`除了被static修饰的静态函数外的其他函数都可以被其他文件所用
    - `printf.c`利用了`console.c`的`cputchar()`函数，并用`putch()`对其进行了包装以记录输出的字符数。
2. 这里console的屏幕最大显示为80*25，也就是说`CRT_SIZE=80*25`，`crt_pos`是下一个要打印的字符的位置，
当前如果超过了最大的显示位置，就得把页面上移一行。`memcpy`就是进行依次复制操作，然后将最后一行除了第一位置零。
3. - fmt指向最终显示的字符串，这里是`x %d, y %x, z %d\n`。而ap是va_list类型的，指向所有输入参数的集合。
    - 执行顺序如下：
    ```
    vcprintf() *fmt = "x %d, y %x, z %d\n"   ap -> {1,2,3}
    cons_putc('x')
    cons_putc(' ')
    va_arg()  ap_before -> {1,2,3}  ap_after -> {2,3}
    cons_putc('1')
    cons_putc (' ')
    cons_putc ('y')
    cons_putc (' ')
    va_arg()  ap_before -> {2,3}  ap_after -> {3}
    cons_putc('3')
    cons_putc (' ')
    cons_putc ('z')
    cons_putc (' ')
    va_arg()  ap_before -> {3}  ap_after -> {}
    cons_putc ('4')
    cons_putc ('\n')
    ```
4. 输出结果为`He110 World`，执行顺序如下
    ```
    vcprintf() *fmt = "H%x Wo%s"   ap -> {57616, 0x72,0x6c,0x64,0x00}
    cons_putc('H')
    va_arg()  ap_before -> {57616, 0x72,0x6c,0x64,0x00}  ap_after -> {0x72,0x6c,0x64,0x00}
    cons_putc('e')
    cons_putc('1')
    cons_putc('1')
    cons_putc('0')
    cons_putc(' ')
    cons_putc('w')
    cons_putc('o')
    va_arg()  ap_before -> {0x72,0x6c,0x64,0x00}  ap_after -> {0x6c,0x64,0x00}
    cons_putc('r')
    va_arg()  ap_before -> {0x6c,0x64,0x00}  ap_after -> {0x64,0x00}
    cons_putc('l')
    va_arg()  ap_before -> {0x64,0x00}  ap_after -> {0x00}
    cons_putc('d')
    va_arg()  ap_before -> {0x00}  ap_after -> {}
    ```
    - 如果在大端机上，i应该变为`unsigned int i = 0x726c6400;`。而57616不需要改变
5. `x=3 y=-267380292`或`x=3 y=-267380292`或其他值。 会输出不确定是值，打印的4个字节是堆栈上的指令或者数据，但是由于格式化输出，所以只会是一个整数
6. 如果编译器改变了压栈的顺序，不需要改变cprintf函数。需要改变的是va_start, va_arg等宏的实现，使其按照先前相反的顺序得到新地址即可

## Exercise 9
栈初始化的指令在`entry.S`中
```
movl	$0x0,%ebp			# nuke frame pointer
movl	$(bootstacktop),%esp
```
这里`bootstacktop`在`kernel.asm`中可以发现值为`0xf0110000`，在`entry.S`末尾可看到栈的大小设为`KSTKSIZE`，在`memlayout.h`中知道值为`8*PGSIZE`
即`8*4096=32KB`。而栈向低地方方向增长，所以栈在内存中的位置为`0xf010c000--0xf0110000`。在entry.S中，数据段中保留了一段32KB的内存空间作为栈的空间，
而栈指针指向的是`bootstacktop`的值，即为栈的最高地址。

## Exercise 10
```
f0100040:	55                   	push   %ebp
f0100041:	89 e5                	mov    %esp,%ebp
f0100043:	53                   	push   %ebx
f0100044:	83 ec 14             	sub    $0x14,%esp
f0100047:	8b 5d 08             	mov    0x8(%ebp),%ebx
```
会有8个字的空间被压栈。其中call指令会将返回地址压栈，`push   %ebp`保存上一层的ebp，接着将ebx压栈，接着开辟了5个字大小的空间，
并将下一次调用的参数保存到开辟的空间中。这样就有`4+4+4+20=32bytes`。栈的形式如下，向下增长、
```
return address
saved ebp
saved ebx
local arg
local arg
local arg
local arg
arg x for next calling
```

