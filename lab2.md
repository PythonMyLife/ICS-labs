# ICS lab2: Defusing  a Binary Bomb
# lab2的要求
   lab2的要求是拆除“炸弹”，也就是通过objdump 和 gdb 或者其他工具对可执行文件 **bomb** 进行分析，从而给出“炸弹”不会引爆的输入来“拆除炸弹”。（就是通过gdb等工具对程序进行分析从而给出不会发生爆炸的输入）
# 工具准备：GDB,objdump
## GDB：Gnu DeBugger
GDB的作用：
 - 开始和中止程序
 - 在特定条件或者指定地址暂停程序
 - 测试发生了什么，查看指定地址或寄存器中的值
 - 改变程序中的东西（寄存器中的值等）来测试程序 

命令：
~~~c
 - gdb <filename>           开始用gdb调试程序
 - break FUNC | *ADDR       在FUNC或ADDR地址处设置断点
 - run                      运行程序  
 - print</?>  $REG | ADDR   打印寄存器的值或者指定地址的值
 - continue | stepi | nexti 继续运行到下一个断点或者程序结束|下一步（不进入函数）| 下一步（进入程序）
 - quit                     退出调试
~~~
## objdump：OBJect-file DUMP
objdump用来显示来自对象文件的信息
命令： 
~~~c
objdump –d | -D <object-file> > <destination-file>
~~~
举例：
objdump可以将二进制文件转换成以下形式
![objdump举例](https://img-blog.csdnimg.cn/20181113212503500.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzNjY4NjMz,size_16,color_FFFFFF,t_70)
工具准备好了，就可以开始做lab啦！
# 开始做lab
得到 lab 文件之后，打开文件夹发现文件夹内有三个文件：
 - README：判断bomb是不是你的
 - bomb：可执行的bomb二进制文件
 - bomb.c：源文件与炸弹的主程序（但是没有关键代码T^T）
第一步阅读README文档，检验这个bomb真的是自己的。
第二步阅读bomb.c（如下，可跳过（但是写的很有意思））

```c
/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.0
 * Copyright 2002, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include "support.h"
#include "phases.h"

/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 */

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input. */
    if (argc == 1) {  
    infile = stdin;
    } 

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
    if (!(infile = fopen(argv[1], "r"))) {
        printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
        exit(8);
    }
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
    printf("Usage: %s [<input_file>]\n", argv[0]);
    exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
                      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");
    
    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */
    
    return 0;
}
```
看完bomb.c我们对bomb有了初步的了解，就是：读入一个文件，如果没有文件，就读入输入，然后根据输入一步一步地运行phase，如果正确，返回正确的信息并向服务器发送正确报告，如果错误返回错误信息并向服务器发送错误报告。

接下来通过

```c
objdump -d bomb > asm 
```
把二进制可执行文件bomb反汇编到asm里（我随便取的名字），这样我们就会得到一个包含汇编指令的文档。接下来就可以求解phase啦！
# 求解phase
## 运行bomb之前
在运行bomb之前一定要先设置断点，不然gdb就认为你直接run这个程序，那，，就炸了（在发现忘记输断点就run之后，不要万念俱焚，断网！这样服务器端检测不到错误，本地会因为网络连接失败而返回错误信息，重新开始，这次不要再忘记设断点了）
首先在main函数处设置断点，然后run检查main的地址和asm文件里地址是否一致，不一致的话后面asm文件里的地址都要保持一样的差值就好了。
```c
break main
```
为了使程序在爆炸前及时停止而不是向服务器发送错误信息（会扣分），我们采取在错误信息函数之前加断点的方式（嘻嘻）（引爆炸弹函数名可能会不同）。
```c
break explode_bomb
```
这样我们就可以放心大胆地开始我们的 defuse bomb 之旅啦！
## phase1
首先
```c
break phase_1
break phase_2
```
然后跑到asm文件里面看到phase1是这个样子的：
```c
0000000000001350 <phase_1>:
    1350:   48 83 ec 08             sub    $0x8,%rsp
    1354:   48 8d 35 95 10 00 00    lea    0x1095(%rip),%rsi        # 23f0 <_IO_stdin_used+0x150>
    135b:   e8 90 04 00 00          callq  17f0 <strings_not_equal>
    1360:   85 c0                   test   %eax,%eax
    1362:   75 05                   jne    1369 <phase_1+0x19>
    1364:   48 83 c4 08             add    $0x8,%rsp
    1368:   c3                      retq   
    1369:   e8 92 0c 00 00          callq  2000 <explode_bomb>
    136e:   eb f4                   jmp    1364 <phase_1+0x14>
```
就是简单的比较了一下输入的字符串（作为函数的参数通过%rdi传入函数）和标准字符串（0x1095(%rip)）如果equal就跳出，如果not equal就爆炸。
所以只需要输出标准字符串就可以了
可以使用以下指令输出标准字符串（当然也可以一个一个char的蹦（嘻））
```c
print (char *)($<寄存器> + <对应立即数>)
```
得到结果如下：![phase_1](https://img-blog.csdnimg.cn/20181114001521711.png)
所以 Why make trillions when we could make... billions? 就是我 phase1的答案啦！kill掉验证一下，对啦！开始做phase_2。
## phase2
照例break phase_3
然后跑到asm里看到phase2是这个样子的：
```c
0000000000001370 <phase_2>:
    1370:   55                      push   %rbp
    1371:   53                      push   %rbx
    1372:   48 83 ec 28             sub    $0x28,%rsp
    1376:   48 89 e5                mov    %rsp,%rbp
    1379:   48 89 e6                mov    %rsp,%rsi
    137c:   e8 bb 0c 00 00          callq  203c <read_six_numbers>
    1381:   48 89 e3                mov    %rsp,%rbx
    1384:   48 83 c5 14             add    $0x14,%rbp
    1388:   eb 09                   jmp    1393 <phase_2+0x23>
    138a:   48 83 c3 04             add    $0x4,%rbx
    138e:   48 39 eb                cmp    %rbp,%rbx
    1391:   74 11                   je     13a4 <phase_2+0x34>
    1393:   8b 03                   mov    (%rbx),%eax
    1395:   83 c0 05                add    $0x5,%eax
    1398:   39 43 04                cmp    %eax,0x4(%rbx)
    139b:   74 ed                   je     138a <phase_2+0x1a>
    139d:   e8 5e 0c 00 00          callq  2000 <explode_bomb>
    13a2:   eb e6                   jmp    138a <phase_2+0x1a>
    13a4:   48 83 c4 28             add    $0x28,%rsp
    13a8:   5b                      pop    %rbx
    13a9:   5d                      pop    %rbp
    13aa:   c3                      retq   
```
可以看到phase调用了另外一个函数read_six_numbers，所以找来这个函数看一看：

```c
000000000000203c <read_six_numbers>:
    203c:   48 83 ec 08             sub    $0x8,%rsp
    2040:   48 89 f2                mov    %rsi,%rdx
    2043:   48 8d 4e 04             lea    0x4(%rsi),%rcx
    2047:   48 8d 46 14             lea    0x14(%rsi),%rax
    204b:   50                      push   %rax
    204c:   48 8d 46 10             lea    0x10(%rsi),%rax
    2050:   50                      push   %rax
    2051:   4c 8d 4e 0c             lea    0xc(%rsi),%r9
    2055:   4c 8d 46 08             lea    0x8(%rsi),%r8
    2059:   48 8d 35 47 07 00 00    lea    0x747(%rip),%rsi        # 27a7 <array.3089+0x327>
    2060:   b8 00 00 00 00          mov    $0x0,%eax
    2065:   e8 96 ef ff ff          callq  1000 <__isoc99_sscanf@plt>
    206a:   48 83 c4 10             add    $0x10,%rsp
    206e:   83 f8 05                cmp    $0x5,%eax
    2071:   7e 05                   jle    2078 <read_six_numbers+0x3c>
    2073:   48 83 c4 08             add    $0x8,%rsp
    2077:   c3                      retq   
    2078:   e8 83 ff ff ff          callq  2000 <explode_bomb>
```
什么？里面有炸点！我的妈呀，幸好看了看，要不然就有可能炸了，分析得知，这个函数就是取6个数字，每个数值用空格隔开，**要是超过6个就会爆炸！**
所以我们得知了，phase2只能写入6个用空格隔开的数字。
回到phase2里，发现接下来是一个循环，从第一个数开始，每一个数+5都等于后一个数直到到达最后一个数，ok，这样的话，0 5 10 15 20 25
bingo！成功了，做下一个phase。
## phase3
照例break phase_4
asm中看到phase3:

```c
00000000000013ab <phase_3>:
    13ab:   48 83 ec 18             sub    $0x18,%rsp
    13af:   48 8d 4c 24 07          lea    0x7(%rsp),%rcx
    13b4:   48 8d 54 24 0c          lea    0xc(%rsp),%rdx
    13b9:   4c 8d 44 24 08          lea    0x8(%rsp),%r8
    13be:   48 8d 35 89 10 00 00    lea    0x1089(%rip),%rsi        # 244e <_IO_stdin_used+0x1ae>
    13c5:   b8 00 00 00 00          mov    $0x0,%eax
    13ca:   e8 31 fc ff ff          callq  1000 <__isoc99_sscanf@plt>
    13cf:   83 f8 02                cmp    $0x2,%eax
    13d2:   7e 1f                   jle    13f3 <phase_3+0x48>
    13d4:   83 7c 24 0c 07          cmpl   $0x7,0xc(%rsp)
    13d9:   0f 87 09 01 00 00       ja     14e8 <phase_3+0x13d>
    13df:   8b 44 24 0c             mov    0xc(%rsp),%eax
    13e3:   48 8d 15 76 10 00 00    lea    0x1076(%rip),%rdx        # 2460 <_IO_stdin_used+0x1c0>
    13ea:   48 63 04 82             movslq (%rdx,%rax,4),%rax
    13ee:   48 01 d0                add    %rdx,%rax
    13f1:   ff e0                   jmpq   *%rax
    13f3:   e8 08 0c 00 00          callq  2000 <explode_bomb>
    13f8:   eb da                   jmp    13d4 <phase_3+0x29>
    13fa:   b8 69 00 00 00          mov    $0x69,%eax
    13ff:   83 7c 24 08 62          cmpl   $0x62,0x8(%rsp)
    1404:   0f 84 e8 00 00 00       je     14f2 <phase_3+0x147>
    140a:   e8 f1 0b 00 00          callq  2000 <explode_bomb>
    140f:   b8 69 00 00 00          mov    $0x69,%eax
    1414:   e9 d9 00 00 00          jmpq   14f2 <phase_3+0x147>
    1419:   b8 64 00 00 00          mov    $0x64,%eax
    141e:   81 7c 24 08 50 03 00    cmpl   $0x350,0x8(%rsp)
    1425:   00 
    1426:   0f 84 c6 00 00 00       je     14f2 <phase_3+0x147>
    142c:   e8 cf 0b 00 00          callq  2000 <explode_bomb>
    1431:   b8 64 00 00 00          mov    $0x64,%eax
    1436:   e9 b7 00 00 00          jmpq   14f2 <phase_3+0x147>
    143b:   b8 79 00 00 00          mov    $0x79,%eax
    1440:   81 7c 24 08 86 03 00    cmpl   $0x386,0x8(%rsp)
    1447:   00 
    1448:   0f 84 a4 00 00 00       je     14f2 <phase_3+0x147>
    144e:   e8 ad 0b 00 00          callq  2000 <explode_bomb>
    1453:   b8 79 00 00 00          mov    $0x79,%eax
    1458:   e9 95 00 00 00          jmpq   14f2 <phase_3+0x147>
    145d:   b8 61 00 00 00          mov    $0x61,%eax
    1462:   81 7c 24 08 54 01 00    cmpl   $0x154,0x8(%rsp)
    1469:   00 
    146a:   0f 84 82 00 00 00       je     14f2 <phase_3+0x147>
    1470:   e8 8b 0b 00 00          callq  2000 <explode_bomb>
    1475:   b8 61 00 00 00          mov    $0x61,%eax
    147a:   eb 76                   jmp    14f2 <phase_3+0x147>
    147c:   b8 64 00 00 00          mov    $0x64,%eax
    1481:   81 7c 24 08 7f 01 00    cmpl   $0x17f,0x8(%rsp)
    1488:   00 
    1489:   74 67                   je     14f2 <phase_3+0x147>
    148b:   e8 70 0b 00 00          callq  2000 <explode_bomb>
    1490:   b8 64 00 00 00          mov    $0x64,%eax
    1495:   eb 5b                   jmp    14f2 <phase_3+0x147>
    1497:   b8 6a 00 00 00          mov    $0x6a,%eax
    149c:   81 7c 24 08 ea 00 00    cmpl   $0xea,0x8(%rsp)
    14a3:   00 
    14a4:   74 4c                   je     14f2 <phase_3+0x147>
    14a6:   e8 55 0b 00 00          callq  2000 <explode_bomb>
    14ab:   b8 6a 00 00 00          mov    $0x6a,%eax
    14b0:   eb 40                   jmp    14f2 <phase_3+0x147>
    14b2:   b8 6f 00 00 00          mov    $0x6f,%eax
    14b7:   81 7c 24 08 fb 02 00    cmpl   $0x2fb,0x8(%rsp)
    14be:   00 
    14bf:   74 31                   je     14f2 <phase_3+0x147>
    14c1:   e8 3a 0b 00 00          callq  2000 <explode_bomb>
    14c6:   b8 6f 00 00 00          mov    $0x6f,%eax
    14cb:   eb 25                   jmp    14f2 <phase_3+0x147>
    14cd:   b8 68 00 00 00          mov    $0x68,%eax
    14d2:   81 7c 24 08 64 01 00    cmpl   $0x164,0x8(%rsp)
    14d9:   00 
    14da:   74 16                   je     14f2 <phase_3+0x147>
    14dc:   e8 1f 0b 00 00          callq  2000 <explode_bomb>
    14e1:   b8 68 00 00 00          mov    $0x68,%eax
    14e6:   eb 0a                   jmp    14f2 <phase_3+0x147>
    14e8:   e8 13 0b 00 00          callq  2000 <explode_bomb>
    14ed:   b8 62 00 00 00          mov    $0x62,%eax
    14f2:   3a 44 24 07             cmp    0x7(%rsp),%al
    14f6:   74 05                   je     14fd <phase_3+0x152>
    14f8:   e8 03 0b 00 00          callq  2000 <explode_bomb>
    14fd:   48 83 c4 18             add    $0x18,%rsp
    1501:   c3                      retq   
```
真的长，首先拿小本本记下来爆炸的地址们，然后看看什么条件爆炸，记下来成功的位置，什么时候成功，然后发现这是一个跳转表，输入的第一个参数就是跳转的位置（因为我第一遍输入的3，我就沿用了3，当然也可以不一样，根据条件大于2小于7就行）然后跳转到指定的代码区，第二第三个参数保持和要求的一致就可以过关啦！
我的代码区：
```c
1475:   b8 61 00 00 00          mov    $0x61,%eax
147a:   eb 76                   jmp    14f2 <phase_3+0x147>

14f2:   3a 44 24 07             cmp    0x7(%rsp),%al
14f6:   74 05                   je     14fd <phase_3+0x152>
```
0x7(%rsp)的值我是直接打印出来哒，所以得到答案 3 a 340（a的ascii码是61）（为什么第二个值就是char是因为0x8(%rsp)和0x7(%rsp)之间只有一个byte）
下一个phase走起！
## phase4
照例break phase_5
asm里phase_4:

```c
0000000000001502 <func4>:
    1502:   b8 01 00 00 00          mov    $0x1,%eax
    1507:   83 ff 01                cmp    $0x1,%edi
    150a:   7e 22                   jle    152e <func4+0x2c>
    150c:   55                      push   %rbp
    150d:   53                      push   %rbx
    150e:   48 83 ec 08             sub    $0x8,%rsp
    1512:   89 fb                   mov    %edi,%ebx
    1514:   8d 7f ff                lea    -0x1(%rdi),%edi
    1517:   e8 e6 ff ff ff          callq  1502 <func4>
    151c:   89 c5                   mov    %eax,%ebp
    151e:   8d 7b fe                lea    -0x2(%rbx),%edi
    1521:   e8 dc ff ff ff          callq  1502 <func4>
    1526:   01 e8                   add    %ebp,%eax
    1528:   48 83 c4 08             add    $0x8,%rsp
    152c:   5b                      pop    %rbx
    152d:   5d                      pop    %rbp
    152e:   f3 c3                   repz retq 

0000000000001530 <phase_4>:
    1530:   48 83 ec 18             sub    $0x18,%rsp
    1534:   48 8d 54 24 0c          lea    0xc(%rsp),%rdx
    1539:   48 8d 35 14 0f 00 00    lea    0xf14(%rip),%rsi        # 2454 <_IO_stdin_used+0x1b4>
    1540:   b8 00 00 00 00          mov    $0x0,%eax
    1545:   e8 b6 fa ff ff          callq  1000 <__isoc99_sscanf@plt>
    154a:   83 f8 01                cmp    $0x1,%eax
    154d:   74 1f                   je     156e <phase_4+0x3e>
    154f:   e8 ac 0a 00 00          callq  2000 <explode_bomb>
    1554:   8b 7c 24 0c             mov    0xc(%rsp),%edi
    1558:   e8 a5 ff ff ff          callq  1502 <func4>
    155d:   3d 6d 1a 00 00          cmp    $0x1a6d,%eax
    1562:   74 05                   je     1569 <phase_4+0x39>
    1564:   e8 97 0a 00 00          callq  2000 <explode_bomb>
    1569:   48 83 c4 18             add    $0x18,%rsp
    156d:   c3                      retq   
    156e:   83 7c 24 0c 00          cmpl   $0x0,0xc(%rsp)
    1573:   7f df                   jg     1554 <phase_4+0x24>
    1575:   eb d8                   jmp    154f <phase_4+0x1f>
```
phase4里面用到了func4所以一并拷下来了。一步一步分析phase4看出phase4没有改变%rdi，把(%rip+0xf14)的值赋给%rsi调用了func4，之后将返回值和$0x1a6d比较，相同则成功，不同则爆炸。
所以问题的关键在于func4：
观察func4发现每一步是上两步的和，即斐波那契数列，所以返回值是0x1a6d的为第19项，输入19就对啦！
## phase5
照例break phase_6

```c
0000000000001577 <phase_5>:
    1577:   53                      push   %rbx
    1578:   48 83 ec 10             sub    $0x10,%rsp
    157c:   48 89 fb                mov    %rdi,%rbx
    157f:   e8 4e 02 00 00          callq  17d2 <string_length>
    1584:   83 f8 06                cmp    $0x6,%eax
    1587:   74 05                   je     158e <phase_5+0x17>
    1589:   e8 72 0a 00 00          callq  2000 <explode_bomb>
    158e:   b8 00 00 00 00          mov    $0x0,%eax
    1593:   48 8d 0d e6 0e 00 00    lea    0xee6(%rip),%rcx        # 2480 <array.3089>
    159a:   0f b6 14 03             movzbl (%rbx,%rax,1),%edx
    159e:   83 e2 0f                and    $0xf,%edx
    15a1:   0f b6 14 11             movzbl (%rcx,%rdx,1),%edx
    15a5:   88 54 04 09             mov    %dl,0x9(%rsp,%rax,1)
    15a9:   48 83 c0 01             add    $0x1,%rax
    15ad:   48 83 f8 06             cmp    $0x6,%rax
    15b1:   75 e7                   jne    159a <phase_5+0x23>
    15b3:   c6 44 24 0f 00          movb   $0x0,0xf(%rsp)
    15b8:   48 8d 7c 24 09          lea    0x9(%rsp),%rdi
    15bd:   48 8d 35 93 0e 00 00    lea    0xe93(%rip),%rsi        # 2457 <_IO_stdin_used+0x1b7>
    15c4:   e8 27 02 00 00          callq  17f0 <strings_not_equal>
    15c9:   85 c0                   test   %eax,%eax
    15cb:   75 06                   jne    15d3 <phase_5+0x5c>
    15cd:   48 83 c4 10             add    $0x10,%rsp
    15d1:   5b                      pop    %rbx
    15d2:   c3                      retq   
    15d3:   e8 28 0a 00 00          callq  2000 <explode_bomb>
    15d8:   eb f3                   jmp    15cd <phase_5+0x56>
```
首先

```c
    157f:   e8 4e 02 00 00          callq  17d2 <string_length>
    1584:   83 f8 06                cmp    $0x6,%eax
```
说明输入的为一个长为6的字符串，接下来是一个跳转表，将字符与0xf与之后得到对应的跳转表中的字符，

```c
    1593:   48 8d 0d e6 0e 00 00    lea    0xee6(%rip),%rcx        # 2480 <array.3089>
    159a:   0f b6 14 03             movzbl (%rbx,%rax,1),%edx
    159e:   83 e2 0f                and    $0xf,%edx
    15a1:   0f b6 14 11             movzbl (%rcx,%rdx,1),%edx
```

然后合起来和标准字符比较，用gdb得到标准字符和跳转表中的字符就可以了，我的答案是becdka
下一题走起！
## puzzle6
照例。。诶？不用了，，隐藏的puzzle写完puzzle6再找
asm里面puzzle6长这样

```c
00000000000015da <phase_6>:
    15da:   41 55                   push   %r13
    15dc:   41 54                   push   %r12
    15de:   55                      push   %rbp
    15df:   53                      push   %rbx
    15e0:   48 83 ec 58             sub    $0x58,%rsp
    15e4:   4c 8d 64 24 30          lea    0x30(%rsp),%r12
    15e9:   4c 89 e6                mov    %r12,%rsi
    15ec:   e8 4b 0a 00 00          callq  203c <read_six_numbers>
    15f1:   41 bd 00 00 00 00       mov    $0x0,%r13d
    15f7:   eb 25                   jmp    161e <phase_6+0x44>
    15f9:   41 83 c5 01             add    $0x1,%r13d
    15fd:   41 83 fd 06             cmp    $0x6,%r13d
    1601:   74 38                   je     163b <phase_6+0x61>
    1603:   44 89 eb                mov    %r13d,%ebx
    1606:   48 63 c3                movslq %ebx,%rax
    1609:   8b 44 84 30             mov    0x30(%rsp,%rax,4),%eax
    160d:   39 45 00                cmp    %eax,0x0(%rbp)
    1610:   74 22                   je     1634 <phase_6+0x5a>
    1612:   83 c3 01                add    $0x1,%ebx
    1615:   83 fb 05                cmp    $0x5,%ebx
    1618:   7e ec                   jle    1606 <phase_6+0x2c>
    161a:   49 83 c4 04             add    $0x4,%r12
    161e:   4c 89 e5                mov    %r12,%rbp
    1621:   41 8b 04 24             mov    (%r12),%eax
    1625:   83 e8 01                sub    $0x1,%eax
    1628:   83 f8 05                cmp    $0x5,%eax
    162b:   76 cc                   jbe    15f9 <phase_6+0x1f>
    162d:   e8 ce 09 00 00          callq  2000 <explode_bomb>
    1632:   eb c5                   jmp    15f9 <phase_6+0x1f>
    1634:   e8 c7 09 00 00          callq  2000 <explode_bomb>
    1639:   eb d7                   jmp    1612 <phase_6+0x38>
    163b:   be 00 00 00 00          mov    $0x0,%esi
    1640:   8b 4c 34 30             mov    0x30(%rsp,%rsi,1),%ecx
    1644:   b8 01 00 00 00          mov    $0x1,%eax
    1649:   48 8d 15 00 31 20 00    lea    0x203100(%rip),%rdx        # 204750 <node1>
    1650:   83 f9 01                cmp    $0x1,%ecx
    1653:   7e 0b                   jle    1660 <phase_6+0x86>
    1655:   48 8b 52 08             mov    0x8(%rdx),%rdx
    1659:   83 c0 01                add    $0x1,%eax
    165c:   39 c8                   cmp    %ecx,%eax
    165e:   75 f5                   jne    1655 <phase_6+0x7b>
    1660:   48 89 14 74             mov    %rdx,(%rsp,%rsi,2)
    1664:   48 83 c6 04             add    $0x4,%rsi
    1668:   48 83 fe 18             cmp    $0x18,%rsi
    166c:   75 d2                   jne    1640 <phase_6+0x66>
    166e:   48 8b 1c 24             mov    (%rsp),%rbx
    1672:   48 89 e0                mov    %rsp,%rax
    1675:   48 8d 74 24 28          lea    0x28(%rsp),%rsi
    167a:   48 89 d9                mov    %rbx,%rcx
    167d:   48 8b 50 08             mov    0x8(%rax),%rdx
    1681:   48 89 51 08             mov    %rdx,0x8(%rcx)
    1685:   48 83 c0 08             add    $0x8,%rax
    1689:   48 89 d1                mov    %rdx,%rcx
    168c:   48 39 c6                cmp    %rax,%rsi
    168f:   75 ec                   jne    167d <phase_6+0xa3>
    1691:   48 c7 42 08 00 00 00    movq   $0x0,0x8(%rdx)
    1698:   00 
    1699:   bd 05 00 00 00          mov    $0x5,%ebp
    169e:   eb 09                   jmp    16a9 <phase_6+0xcf>
    16a0:   48 8b 5b 08             mov    0x8(%rbx),%rbx
    16a4:   83 ed 01                sub    $0x1,%ebp
    16a7:   74 11                   je     16ba <phase_6+0xe0>
    16a9:   48 8b 43 08             mov    0x8(%rbx),%rax
    16ad:   8b 00                   mov    (%rax),%eax
    16af:   39 03                   cmp    %eax,(%rbx)
    16b1:   7d ed                   jge    16a0 <phase_6+0xc6>
    16b3:   e8 48 09 00 00          callq  2000 <explode_bomb>
    16b8:   eb e6                   jmp    16a0 <phase_6+0xc6>
    16ba:   48 83 c4 58             add    $0x58,%rsp
    16be:   5b                      pop    %rbx
    16bf:   5d                      pop    %rbp
    16c0:   41 5c                   pop    %r12
    16c2:   41 5d                   pop    %r13
    16c4:   c3                      retq   
```
可见这是一个令人发指的puzzle，可能会花费大量时间来摸清它的逻辑，那么话不多说开始吧
一眼看见老熟人read_six_numbers所以输入肯定就是6个数字
接下来是一个循环

```c
    15f9:   41 83 c5 01             add    $0x1,%r13d
    15fd:   41 83 fd 06             cmp    $0x6,%r13d
    1601:   74 38                   je     163b <phase_6+0x61>
    1603:   44 89 eb                mov    %r13d,%ebx
    1606:   48 63 c3                movslq %ebx,%rax
    1609:   8b 44 84 30             mov    0x30(%rsp,%rax,4),%eax
    160d:   39 45 00                cmp    %eax,0x0(%rbp)
    1610:   74 22                   je     1634 <phase_6+0x5a>
    1612:   83 c3 01                add    $0x1,%ebx
    1615:   83 fb 05                cmp    $0x5,%ebx
    1618:   7e ec                   jle    1606 <phase_6+0x2c>
    161a:   49 83 c4 04             add    $0x4,%r12
    161e:   4c 89 e5                mov    %r12,%rbp
    1621:   41 8b 04 24             mov    (%r12),%eax
    1625:   83 e8 01                sub    $0x1,%eax
    1628:   83 f8 05                cmp    $0x5,%eax
    162b:   76 cc                   jbe    15f9 <phase_6+0x1f>
```
其实意思很简单，就是设计者故意写难了，这个循环的意思就是这6个数字是1 2 3 4 5 6，只是顺序的差别罢了（于是全排列看看对不对？（6！好像还是太多了，算了还是接着看吧））
接下来又是一个跳转表，1-6每一个数字对应另外一个数字
接下来是一个循环，要求数字按照从大到小排列，所以得到数之后排列一下再把对应的数输进去就好啦！于是得到1 5 4 3 2 6
成功！
但是还差一个隐藏的phase，让我们找一找它藏在哪！

## secret phase
break secret_phase
然后在asm里面查找关键字secret，除了在函数secret_phase中找到了之外，在phase_defused函数里也发现了跳转往secret_phase的指令，嘿嘿这就是隐藏bomb的入口！
asm里面secret_phase如下：

```c
000000000000217e <phase_defused>:
    217e:   48 83 ec 68             sub    $0x68,%rsp
    2182:   bf 01 00 00 00          mov    $0x1,%edi
    2187:   e8 b1 f8 ff ff          callq  1a3d <send_msg>
    218c:   83 3d 29 26 20 00 06    cmpl   $0x6,0x202629(%rip)        # 2047bc <num_input_strings>
    2193:   74 05                   je     219a <phase_defused+0x1c>
    2195:   48 83 c4 68             add    $0x68,%rsp
    2199:   c3                      retq   
    219a:   48 8d 4c 24 10          lea    0x10(%rsp),%rcx
    219f:   48 8d 54 24 0c          lea    0xc(%rsp),%rdx
    21a4:   48 8d 35 52 06 00 00    lea    0x652(%rip),%rsi        # 27fd <array.3089+0x37d>
    21ab:   48 8d 3d 1e 27 20 00    lea    0x20271e(%rip),%rdi        # 2048d0 <input_strings+0xf0>
    21b2:   b8 00 00 00 00          mov    $0x0,%eax
    21b7:   e8 44 ee ff ff          callq  1000 <__isoc99_sscanf@plt>
    21bc:   83 f8 02                cmp    $0x2,%eax
    21bf:   74 1a                   je     21db <phase_defused+0x5d>
    21c1:   48 8d 3d 88 03 00 00    lea    0x388(%rip),%rdi        # 2550 <array.3089+0xd0>
    21c8:   e8 23 ed ff ff          callq  ef0 <puts@plt>
    21cd:   48 8d 3d ac 03 00 00    lea    0x3ac(%rip),%rdi        # 2580 <array.3089+0x100>
    21d4:   e8 17 ed ff ff          callq  ef0 <puts@plt>
    21d9:   eb ba                   jmp    2195 <phase_defused+0x17>
    21db:   48 8d 7c 24 10          lea    0x10(%rsp),%rdi
    21e0:   48 8d 35 1c 06 00 00    lea    0x61c(%rip),%rsi        # 2803 <array.3089+0x383>
    21e7:   e8 04 f6 ff ff          callq  17f0 <strings_not_equal>
    21ec:   85 c0                   test   %eax,%eax
    21ee:   75 d1                   jne    21c1 <phase_defused+0x43>
    21f0:   48 8d 3d f9 02 00 00    lea    0x2f9(%rip),%rdi        # 24f0 <array.3089+0x70>
    21f7:   e8 f4 ec ff ff          callq  ef0 <puts@plt>
    21fc:   48 8d 3d 15 03 00 00    lea    0x315(%rip),%rdi        # 2518 <array.3089+0x98>
    2203:   e8 e8 ec ff ff          callq  ef0 <puts@plt>
    2208:   b8 00 00 00 00          mov    $0x0,%eax
    220d:   e8 f0 f4 ff ff          callq  1702 <secret_phase>
    2212:   eb ad                   jmp    21c1 <phase_defused+0x43>
    2214:   66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
    221b:   00 00 00 
    221e:   66 90                   xchg   %ax,%ax
```
它在发送完成功信息之后进行了一个判断：
```c
    218c:   83 3d 29 26 20 00 06    cmpl   $0x6,0x202629(%rip)        # 2047bc <num_input_strings>
    2193:   74 05                   je     219a <phase_defused+0x1c>
```
也就是说，当发送完信息发现完成了6个phase之后 才继续运行之后的代码。
因为 __isoc99_sscanf@plt 的第二个参数表征接收的数据类型，查看接收什么数据类型：

```c
x /s ($rip +0x652)
```
得到"  %d  %s"，所以接受的为数字和字符串的数据类型，只有第四道题可以在后面加上字符串才满足这种数据类型，所以重开程序，在phase4后面输入一行字符串，得以进行下面的测试
接着往下看需要输入什么字符串，发现了几行非常明显的代码
```c
    21db:   48 8d 7c 24 10          lea    0x10(%rsp),%rdi
    21e0:   48 8d 35 1c 06 00 00    lea    0x61c(%rip),%rsi        # 2803 <array.3089+0x383>
    21e7:   e8 04 f6 ff ff          callq  17f0 <strings_not_equal>
    21ec:   85 c0                   test   %eax,%eax
    21ee:   75 d1                   jne    21c1 <phase_defused+0x43>
```
所以在运行完21e0之后查看%rsi指向位置的值就是想要得到的字符串 austinpowers 然后kill掉程序，重新再phase4的地方输入 19 austinpowers
最后一个phase6结束之后我们进入了secret_phase
在asm里查看secret_phase：

```c
00000000000016c5 <fun7>:
    16c5:   48 85 ff                test   %rdi,%rdi
    16c8:   74 32                   je     16fc <fun7+0x37>
    16ca:   48 83 ec 08             sub    $0x8,%rsp
    16ce:   8b 17                   mov    (%rdi),%edx
    16d0:   39 f2                   cmp    %esi,%edx
    16d2:   7f 1b                   jg     16ef <fun7+0x2a>
    16d4:   b8 00 00 00 00          mov    $0x0,%eax
    16d9:   39 f2                   cmp    %esi,%edx
    16db:   74 0d                   je     16ea <fun7+0x25>
    16dd:   48 8b 7f 10             mov    0x10(%rdi),%rdi
    16e1:   e8 df ff ff ff          callq  16c5 <fun7>
    16e6:   8d 44 00 01             lea    0x1(%rax,%rax,1),%eax
    16ea:   48 83 c4 08             add    $0x8,%rsp
    16ee:   c3                      retq   
    16ef:   48 8b 7f 08             mov    0x8(%rdi),%rdi
    16f3:   e8 cd ff ff ff          callq  16c5 <fun7>
    16f8:   01 c0                   add    %eax,%eax
    16fa:   eb ee                   jmp    16ea <fun7+0x25>
    16fc:   b8 ff ff ff ff          mov    $0xffffffff,%eax
    1701:   c3                      retq   

0000000000001702 <secret_phase>:
    1702:   53                      push   %rbx
    1703:   e8 75 09 00 00          callq  207d <read_line>
    1708:   ba 0a 00 00 00          mov    $0xa,%edx
    170d:   be 00 00 00 00          mov    $0x0,%esi
    1712:   48 89 c7                mov    %rax,%rdi
    1715:   e8 b6 f8 ff ff          callq  fd0 <strtol@plt>
    171a:   48 89 c3                mov    %rax,%rbx
    171d:   8d 40 ff                lea    -0x1(%rax),%eax
    1720:   3d e8 03 00 00          cmp    $0x3e8,%eax
    1725:   77 2b                   ja     1752 <secret_phase+0x50>
    1727:   89 de                   mov    %ebx,%esi
    1729:   48 8d 3d 40 2f 20 00    lea    0x202f40(%rip),%rdi        # 204670 <n1>
    1730:   e8 90 ff ff ff          callq  16c5 <fun7>
    1735:   83 f8 01                cmp    $0x1,%eax
    1738:   74 05                   je     173f <secret_phase+0x3d>
    173a:   e8 c1 08 00 00          callq  2000 <explode_bomb>
    173f:   48 8d 3d e2 0c 00 00    lea    0xce2(%rip),%rdi        # 2428 <_IO_stdin_used+0x188>
    1746:   e8 a5 f7 ff ff          callq  ef0 <puts@plt>
    174b:   e8 2e 0a 00 00          callq  217e <phase_defused>
    1750:   5b                      pop    %rbx
    1751:   c3                      retq   
    1752:   e8 a9 08 00 00          callq  2000 <explode_bomb>
    1757:   eb ce                   jmp    1727 <secret_phase+0x25>
```
看完代码我们可以知道输入值不能大于0x3e9

```c
    171a:   48 89 c3                mov    %rax,%rbx
    171d:   8d 40 ff                lea    -0x1(%rax),%eax
    1720:   3d e8 03 00 00          cmp    $0x3e8,%eax
    1725:   77 2b                   ja     1752 <secret_phase+0x50>
```
secret_phase调用了一个递归函数，函数的第一个参数是0x202f40(%rip)，第二个参数是输入的值

```c
    1727:   89 de                   mov    %ebx,%esi
    1729:   48 8d 3d 40 2f 20 00    lea    0x202f40(%rip),%rdi        # 204670 <n1>
    1730:   e8 90 ff ff ff          callq  16c5 <fun7>
```
查看fun7函数，发现这个函数是一个递归函数，小于第一个参数则2乘fun7（左儿子，输入的数），等于第一个参数则0，大于第一个参数则2乘fun7（右儿子，输入的数）+1，所以一步一步得出结果50
bingo！
成功完成lab2！