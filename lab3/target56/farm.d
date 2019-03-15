
farm.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <start_farm>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	b8 01 00 00 00       	mov    $0x1,%eax
   9:	5d                   	pop    %rbp
   a:	c3                   	retq   

000000000000000b <getval_433>:
   b:	55                   	push   %rbp
   c:	48 89 e5             	mov    %rsp,%rbp
   f:	b8 e1 50 c3 00       	mov    $0xc350e1,%eax
  14:	5d                   	pop    %rbp
  15:	c3                   	retq   

0000000000000016 <addval_391>:
  16:	55                   	push   %rbp
  17:	48 89 e5             	mov    %rsp,%rbp
  1a:	89 7d fc             	mov    %edi,-0x4(%rbp)
  1d:	8b 45 fc             	mov    -0x4(%rbp),%eax
  20:	2d b7 a7 6f 6f       	sub    $0x6f6fa7b7,%eax
  25:	5d                   	pop    %rbp
  26:	c3                   	retq   

0000000000000027 <getval_460>:
  27:	55                   	push   %rbp
  28:	48 89 e5             	mov    %rsp,%rbp
  2b:	b8 48 89 c7 c3       	mov    $0xc3c78948,%eax
  30:	5d                   	pop    %rbp
  31:	c3                   	retq   

0000000000000032 <getval_470>:
  32:	55                   	push   %rbp
  33:	48 89 e5             	mov    %rsp,%rbp
  36:	b8 f5 48 89 c7       	mov    $0xc78948f5,%eax
  3b:	5d                   	pop    %rbp
  3c:	c3                   	retq   

000000000000003d <getval_482>:
  3d:	55                   	push   %rbp
  3e:	48 89 e5             	mov    %rsp,%rbp
  41:	b8 b0 5f 58 92       	mov    $0x92585fb0,%eax
  46:	5d                   	pop    %rbp
  47:	c3                   	retq   

0000000000000048 <addval_299>:
  48:	55                   	push   %rbp
  49:	48 89 e5             	mov    %rsp,%rbp
  4c:	89 7d fc             	mov    %edi,-0x4(%rbp)
  4f:	8b 45 fc             	mov    -0x4(%rbp),%eax
  52:	2d a8 76 38 3c       	sub    $0x3c3876a8,%eax
  57:	5d                   	pop    %rbp
  58:	c3                   	retq   

0000000000000059 <getval_267>:
  59:	55                   	push   %rbp
  5a:	48 89 e5             	mov    %rsp,%rbp
  5d:	b8 48 09 c7 c3       	mov    $0xc3c70948,%eax
  62:	5d                   	pop    %rbp
  63:	c3                   	retq   

0000000000000064 <addval_179>:
  64:	55                   	push   %rbp
  65:	48 89 e5             	mov    %rsp,%rbp
  68:	89 7d fc             	mov    %edi,-0x4(%rbp)
  6b:	8b 45 fc             	mov    -0x4(%rbp),%eax
  6e:	05 58 90 c3 28       	add    $0x28c39058,%eax
  73:	5d                   	pop    %rbp
  74:	c3                   	retq   

0000000000000075 <mid_farm>:
  75:	55                   	push   %rbp
  76:	48 89 e5             	mov    %rsp,%rbp
  79:	b8 01 00 00 00       	mov    $0x1,%eax
  7e:	5d                   	pop    %rbp
  7f:	c3                   	retq   

0000000000000080 <add_xy>:
  80:	55                   	push   %rbp
  81:	48 89 e5             	mov    %rsp,%rbp
  84:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
  88:	48 89 75 f0          	mov    %rsi,-0x10(%rbp)
  8c:	48 8b 55 f8          	mov    -0x8(%rbp),%rdx
  90:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  94:	48 01 d0             	add    %rdx,%rax
  97:	5d                   	pop    %rbp
  98:	c3                   	retq   

0000000000000099 <getval_404>:
  99:	55                   	push   %rbp
  9a:	48 89 e5             	mov    %rsp,%rbp
  9d:	b8 89 ce 78 d2       	mov    $0xd278ce89,%eax
  a2:	5d                   	pop    %rbp
  a3:	c3                   	retq   

00000000000000a4 <getval_265>:
  a4:	55                   	push   %rbp
  a5:	48 89 e5             	mov    %rsp,%rbp
  a8:	b8 88 d1 20 d2       	mov    $0xd220d188,%eax
  ad:	5d                   	pop    %rbp
  ae:	c3                   	retq   

00000000000000af <getval_363>:
  af:	55                   	push   %rbp
  b0:	48 89 e5             	mov    %rsp,%rbp
  b3:	b8 89 d1 90 c7       	mov    $0xc790d189,%eax
  b8:	5d                   	pop    %rbp
  b9:	c3                   	retq   

00000000000000ba <getval_407>:
  ba:	55                   	push   %rbp
  bb:	48 89 e5             	mov    %rsp,%rbp
  be:	b8 89 ce 28 c9       	mov    $0xc928ce89,%eax
  c3:	5d                   	pop    %rbp
  c4:	c3                   	retq   

00000000000000c5 <addval_445>:
  c5:	55                   	push   %rbp
  c6:	48 89 e5             	mov    %rsp,%rbp
  c9:	89 7d fc             	mov    %edi,-0x4(%rbp)
  cc:	8b 45 fc             	mov    -0x4(%rbp),%eax
  cf:	2d be b7 76 1f       	sub    $0x1f76b7be,%eax
  d4:	5d                   	pop    %rbp
  d5:	c3                   	retq   

00000000000000d6 <addval_245>:
  d6:	55                   	push   %rbp
  d7:	48 89 e5             	mov    %rsp,%rbp
  da:	89 7d fc             	mov    %edi,-0x4(%rbp)
  dd:	8b 45 fc             	mov    -0x4(%rbp),%eax
  e0:	2d 77 2e c7 3f       	sub    $0x3fc72e77,%eax
  e5:	5d                   	pop    %rbp
  e6:	c3                   	retq   

00000000000000e7 <setval_388>:
  e7:	55                   	push   %rbp
  e8:	48 89 e5             	mov    %rsp,%rbp
  eb:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
  ef:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  f3:	c7 00 68 89 e0 c3    	movl   $0xc3e08968,(%rax)
  f9:	90                   	nop
  fa:	5d                   	pop    %rbp
  fb:	c3                   	retq   

00000000000000fc <addval_308>:
  fc:	55                   	push   %rbp
  fd:	48 89 e5             	mov    %rsp,%rbp
 100:	89 7d fc             	mov    %edi,-0x4(%rbp)
 103:	8b 45 fc             	mov    -0x4(%rbp),%eax
 106:	2d 77 31 6f 3c       	sub    $0x3c6f3177,%eax
 10b:	5d                   	pop    %rbp
 10c:	c3                   	retq   

000000000000010d <setval_356>:
 10d:	55                   	push   %rbp
 10e:	48 89 e5             	mov    %rsp,%rbp
 111:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 115:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 119:	c7 00 89 d1 84 c9    	movl   $0xc984d189,(%rax)
 11f:	90                   	nop
 120:	5d                   	pop    %rbp
 121:	c3                   	retq   

0000000000000122 <addval_355>:
 122:	55                   	push   %rbp
 123:	48 89 e5             	mov    %rsp,%rbp
 126:	89 7d fc             	mov    %edi,-0x4(%rbp)
 129:	8b 45 fc             	mov    -0x4(%rbp),%eax
 12c:	2d b8 66 1f 6f       	sub    $0x6f1f66b8,%eax
 131:	5d                   	pop    %rbp
 132:	c3                   	retq   

0000000000000133 <getval_338>:
 133:	55                   	push   %rbp
 134:	48 89 e5             	mov    %rsp,%rbp
 137:	b8 ac 08 89 e0       	mov    $0xe08908ac,%eax
 13c:	5d                   	pop    %rbp
 13d:	c3                   	retq   

000000000000013e <addval_247>:
 13e:	55                   	push   %rbp
 13f:	48 89 e5             	mov    %rsp,%rbp
 142:	89 7d fc             	mov    %edi,-0x4(%rbp)
 145:	8b 45 fc             	mov    -0x4(%rbp),%eax
 148:	2d d9 76 3d 3e       	sub    $0x3e3d76d9,%eax
 14d:	5d                   	pop    %rbp
 14e:	c3                   	retq   

000000000000014f <setval_281>:
 14f:	55                   	push   %rbp
 150:	48 89 e5             	mov    %rsp,%rbp
 153:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 157:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 15b:	c7 00 8b c2 90 90    	movl   $0x9090c28b,(%rax)
 161:	90                   	nop
 162:	5d                   	pop    %rbp
 163:	c3                   	retq   

0000000000000164 <getval_283>:
 164:	55                   	push   %rbp
 165:	48 89 e5             	mov    %rsp,%rbp
 168:	b8 a9 c2 20 c0       	mov    $0xc020c2a9,%eax
 16d:	5d                   	pop    %rbp
 16e:	c3                   	retq   

000000000000016f <setval_389>:
 16f:	55                   	push   %rbp
 170:	48 89 e5             	mov    %rsp,%rbp
 173:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 177:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 17b:	c7 00 89 c2 92 90    	movl   $0x9092c289,(%rax)
 181:	90                   	nop
 182:	5d                   	pop    %rbp
 183:	c3                   	retq   

0000000000000184 <getval_287>:
 184:	55                   	push   %rbp
 185:	48 89 e5             	mov    %rsp,%rbp
 188:	b8 48 89 e0 c2       	mov    $0xc2e08948,%eax
 18d:	5d                   	pop    %rbp
 18e:	c3                   	retq   

000000000000018f <getval_360>:
 18f:	55                   	push   %rbp
 190:	48 89 e5             	mov    %rsp,%rbp
 193:	b8 8b d1 38 c9       	mov    $0xc938d18b,%eax
 198:	5d                   	pop    %rbp
 199:	c3                   	retq   

000000000000019a <setval_457>:
 19a:	55                   	push   %rbp
 19b:	48 89 e5             	mov    %rsp,%rbp
 19e:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 1a2:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 1a6:	c7 00 89 ce 60 d2    	movl   $0xd260ce89,(%rax)
 1ac:	90                   	nop
 1ad:	5d                   	pop    %rbp
 1ae:	c3                   	retq   

00000000000001af <getval_358>:
 1af:	55                   	push   %rbp
 1b0:	48 89 e5             	mov    %rsp,%rbp
 1b3:	b8 89 ce a4 d2       	mov    $0xd2a4ce89,%eax
 1b8:	5d                   	pop    %rbp
 1b9:	c3                   	retq   

00000000000001ba <setval_248>:
 1ba:	55                   	push   %rbp
 1bb:	48 89 e5             	mov    %rsp,%rbp
 1be:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 1c2:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 1c6:	c7 00 eb 11 99 d1    	movl   $0xd19911eb,(%rax)
 1cc:	90                   	nop
 1cd:	5d                   	pop    %rbp
 1ce:	c3                   	retq   

00000000000001cf <getval_119>:
 1cf:	55                   	push   %rbp
 1d0:	48 89 e5             	mov    %rsp,%rbp
 1d3:	b8 48 89 e0 c1       	mov    $0xc1e08948,%eax
 1d8:	5d                   	pop    %rbp
 1d9:	c3                   	retq   

00000000000001da <setval_187>:
 1da:	55                   	push   %rbp
 1db:	48 89 e5             	mov    %rsp,%rbp
 1de:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 1e2:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 1e6:	c7 00 89 ce 90 c3    	movl   $0xc390ce89,(%rax)
 1ec:	90                   	nop
 1ed:	5d                   	pop    %rbp
 1ee:	c3                   	retq   

00000000000001ef <addval_321>:
 1ef:	55                   	push   %rbp
 1f0:	48 89 e5             	mov    %rsp,%rbp
 1f3:	89 7d fc             	mov    %edi,-0x4(%rbp)
 1f6:	8b 45 fc             	mov    -0x4(%rbp),%eax
 1f9:	2d b2 af f6 3d       	sub    $0x3df6afb2,%eax
 1fe:	5d                   	pop    %rbp
 1ff:	c3                   	retq   

0000000000000200 <getval_126>:
 200:	55                   	push   %rbp
 201:	48 89 e5             	mov    %rsp,%rbp
 204:	b8 48 89 e0 c3       	mov    $0xc3e08948,%eax
 209:	5d                   	pop    %rbp
 20a:	c3                   	retq   

000000000000020b <getval_487>:
 20b:	55                   	push   %rbp
 20c:	48 89 e5             	mov    %rsp,%rbp
 20f:	b8 89 c2 c3 ae       	mov    $0xaec3c289,%eax
 214:	5d                   	pop    %rbp
 215:	c3                   	retq   

0000000000000216 <getval_382>:
 216:	55                   	push   %rbp
 217:	48 89 e5             	mov    %rsp,%rbp
 21a:	b8 89 d1 90 c7       	mov    $0xc790d189,%eax
 21f:	5d                   	pop    %rbp
 220:	c3                   	retq   

0000000000000221 <addval_401>:
 221:	55                   	push   %rbp
 222:	48 89 e5             	mov    %rsp,%rbp
 225:	89 7d fc             	mov    %edi,-0x4(%rbp)
 228:	8b 45 fc             	mov    -0x4(%rbp),%eax
 22b:	2d b8 72 1f 3c       	sub    $0x3c1f72b8,%eax
 230:	5d                   	pop    %rbp
 231:	c3                   	retq   

0000000000000232 <setval_138>:
 232:	55                   	push   %rbp
 233:	48 89 e5             	mov    %rsp,%rbp
 236:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 23a:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 23e:	c7 00 89 c2 20 db    	movl   $0xdb20c289,(%rax)
 244:	90                   	nop
 245:	5d                   	pop    %rbp
 246:	c3                   	retq   

0000000000000247 <getval_396>:
 247:	55                   	push   %rbp
 248:	48 89 e5             	mov    %rsp,%rbp
 24b:	b8 c9 d1 20 c9       	mov    $0xc920d1c9,%eax
 250:	5d                   	pop    %rbp
 251:	c3                   	retq   

0000000000000252 <addval_225>:
 252:	55                   	push   %rbp
 253:	48 89 e5             	mov    %rsp,%rbp
 256:	89 7d fc             	mov    %edi,-0x4(%rbp)
 259:	8b 45 fc             	mov    -0x4(%rbp),%eax
 25c:	2d 77 31 cf 24       	sub    $0x24cf3177,%eax
 261:	5d                   	pop    %rbp
 262:	c3                   	retq   

0000000000000263 <setval_333>:
 263:	55                   	push   %rbp
 264:	48 89 e5             	mov    %rsp,%rbp
 267:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
 26b:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 26f:	c7 00 63 e1 81 ce    	movl   $0xce81e163,(%rax)
 275:	90                   	nop
 276:	5d                   	pop    %rbp
 277:	c3                   	retq   

0000000000000278 <addval_479>:
 278:	55                   	push   %rbp
 279:	48 89 e5             	mov    %rsp,%rbp
 27c:	89 7d fc             	mov    %edi,-0x4(%rbp)
 27f:	8b 45 fc             	mov    -0x4(%rbp),%eax
 282:	2d 77 3d d7 24       	sub    $0x24d73d77,%eax
 287:	5d                   	pop    %rbp
 288:	c3                   	retq   

0000000000000289 <end_farm>:
 289:	55                   	push   %rbp
 28a:	48 89 e5             	mov    %rsp,%rbp
 28d:	b8 01 00 00 00       	mov    $0x1,%eax
 292:	5d                   	pop    %rbp
 293:	c3                   	retq   
