### Program 1 Analysis:

```bash
root@DESKTOP-61I8PPU:/mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind# ./prog1
Modified this value 0
```

**Valgrind Output:**
```bash
root@DESKTOP-61I8PPU:/mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind# valgrind ./prog1
==466== Memcheck, a memory error detector
==466== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.   
==466== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==466== Command: ./prog1
==466==
==466== Invalid write of size 4
==466==    at 0x10918D: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog1)   
==466==  Address 0x4a8e1d0 is 0 bytes after a block of size 400 alloc'd
==466==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==466==    by 0x10917E: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog1)   
==466==
==466== Invalid read of size 4
==466==    at 0x10919D: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog1)   
==466==  Address 0x4a8e1f0 is 32 bytes before an unallocated block of size 4,193,744 in arena "client"
==466==
Modified this value 4193809
==466==
==466== HEAP SUMMARY:
==466==     in use at exit: 400 bytes in 1 blocks
==466==   total heap usage: 2 allocs, 1 frees, 1,424 bytes allocated
==466==
==466== LEAK SUMMARY:
==466==    definitely lost: 400 bytes in 1 blocks
==466==    indirectly lost: 0 bytes in 0 blocks
==466==      possibly lost: 0 bytes in 0 blocks
==466==    still reachable: 0 bytes in 0 blocks
==466==         suppressed: 0 bytes in 0 blocks
==466== Rerun with --leak-check=full to see details of leaked memory
==466==
==466== For lists of detected and suppressed errors, rerun with: -s
==466== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

**Conclusion:**
The code, when executed, has memory errors (invalid write and read) related to dynamic memory allocation. It allocates a block of 400 bytes, attempts invalid operations on that memory, and results in a memory leak of 400 bytes upon exit, as reported by Valgrind. The code is wrong "memory leak".
---

### Program 2 Analysis:

```bash
root@DESKTOP-61I8PPU:/mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind# ./prog2
Accessing the element 1551164832
```

**Valgrind Output:**
```bash
==2624== Memcheck, a memory error detector
==2624== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==2624== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==2624== Command: ./prog2
==2624==
==2624== Invalid read of size 4
==2624==    at 0x1091BD: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog2)
==2624==  Address 0x4a8e040 is 0 bytes inside a block of size 400 free'd
==2624==    at 0x484B27F: free (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==2624==    by 0x1091B8: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog2)
==2624==  Block was alloc'd at
==2624==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==2624==    by 0x10919E: main (in /mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind/prog2)
==2624==
Accessing the element 5
==2624==
==2624== HEAP SUMMARY:
==2624==     in use at exit: 0 bytes in 0 blocks
==2624==   total heap usage: 2 allocs, 2 frees, 1,424 bytes allocated
==2624==
==2624== All heap blocks were freed -- no leaks are possible
==2624==
==2624== For lists of detected and suppressed errors, rerun with: -s
==2624== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

**Conclusion:**
Program 2 analysis with Valgrind detects an invalid read of size 4 at a freed memory address (0x4a8e040). Despite proper deallocation, an attempt to access the freed memory occurred, resulting in one detected error. No memory leaks are reported in the overall heap analysis. The code is wrong "invalid read".