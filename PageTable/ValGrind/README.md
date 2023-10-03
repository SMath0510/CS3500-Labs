### Program 1 Analysis:

```bash
root@DESKTOP-61I8PPU:/mnt/c/Users/Admin/Desktop/OS Assignment/ValGrind# ./prog1
Modified this value 0
```

**Valgrind Output:**
```bash
==2609== Memcheck, a memory error detector
==2609== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==2609== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==2609== Command: ./prog1
==2609==
Modified this value 0
==2609==
==2609== HEAP SUMMARY:
==2609==     in use at exit: 0 bytes in 0 blocks
==2609==   total heap usage: 1 allocs, 1 frees, 1,024 bytes allocated
==2609==
==2609== All heap blocks were freed -- no leaks are possible
==2609==
==2609== For lists of detected and suppressed errors, rerun with: -s
==2609== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

**Conclusion:**
The analysis of Program 1 using Valgrind reveals no memory leaks or errors. All allocated heap blocks were appropriately freed.

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
Program 2 analysis with Valgrind detects an invalid read of size 4 at a freed memory address (0x4a8e040). Despite proper deallocation, an attempt to access the freed memory occurred, resulting in one detected error. No memory leaks are reported in the overall heap analysis.