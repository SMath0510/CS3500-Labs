
# Helgrind Analysis for  main_signal.c
Reporting the output, observation and conclusions obtained from running the program `main_signal.c` on valgrind (using the tool helgrind)

## Output
```bash
==72167== Helgrind, a thread error detector
==72167== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==72167== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==72167== Command: ./prog
==72167== 
this should print first
==72167== ---Thread-Announcement------------------------------------------
==72167== 
==72167== Thread #2 was created
==72167==    at 0x4991BE3: clone (clone.S:76)
==72167==    by 0x4992ADE: __clone_internal (clone-internal.c:83)
==72167==    by 0x49006D8: create_thread (pthread_create.c:295)
==72167==    by 0x49011FF: pthread_create@@GLIBC_2.34 (pthread_create.c:828)
==72167==    by 0x4853767: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x1091F7: main (main_signal.c:15)
==72167== 
==72167== ---Thread-Announcement------------------------------------------
==72167== 
==72167== Thread #1 is the program's root thread
==72167== 
==72167== ----------------------------------------------------------------
==72167== 
==72167== Possible data race during write of size 4 at 0x10C014 by thread #2
==72167== Locks held: none
==72167==    at 0x1091A8: worker (main_signal.c:9)
==72167==    by 0x485396A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x4900AC2: start_thread (pthread_create.c:442)
==72167==    by 0x4991BF3: clone (clone.S:100)
==72167== 
==72167== This conflicts with a previous read of size 4 by thread #1
==72167== Locks held: none
==72167==    at 0x1091F9: main (main_signal.c:16)
==72167==  Address 0x10c014 is 0 bytes inside data symbol "done"
==72167== 
==72167== ----------------------------------------------------------------
==72167== 
==72167== Possible data race during read of size 4 at 0x10C014 by thread #1
==72167== Locks held: none
==72167==    at 0x1091F9: main (main_signal.c:16)
==72167== 
==72167== This conflicts with a previous write of size 4 by thread #2
==72167== Locks held: none
==72167==    at 0x1091A8: worker (main_signal.c:9)
==72167==    by 0x485396A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x4900AC2: start_thread (pthread_create.c:442)
==72167==    by 0x4991BF3: clone (clone.S:100)
==72167==  Address 0x10c014 is 0 bytes inside data symbol "done"
==72167== 
==72167== ----------------------------------------------------------------
==72167== 
==72167== Possible data race during write of size 1 at 0x52981A5 by thread #1
==72167== Locks held: none
==72167==    at 0x4859796: mempcpy (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x48F7664: _IO_new_file_xsputn (fileops.c:1235)
==72167==    by 0x48F7664: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1196)
==72167==    by 0x48ECF1B: puts (ioputs.c:40)
==72167==    by 0x109211: main (main_signal.c:18)
==72167==  Address 0x52981a5 is 21 bytes inside a block of size 1,024 alloc'd
==72167==    at 0x484A919: malloc (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x48EABA3: _IO_file_doallocate (filedoalloc.c:101)
==72167==    by 0x48F9CDF: _IO_doallocbuf (genops.c:347)
==72167==    by 0x48F8F5F: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:744)
==72167==    by 0x48F76D4: _IO_new_file_xsputn (fileops.c:1243)
==72167==    by 0x48F76D4: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1196)
==72167==    by 0x48ECF1B: puts (ioputs.c:40)
==72167==    by 0x1091A7: worker (main_signal.c:8)
==72167==    by 0x485396A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==72167==    by 0x4900AC2: start_thread (pthread_create.c:442)
==72167==    by 0x4991BF3: clone (clone.S:100)
==72167==  Block was alloc'd by thread #2
==72167== 
this should print last
==72167== 
==72167== Use --history-level=approx or =none to gain increased speed, at
==72167== the cost of reduced accuracy of conflicting-access information
==72167== For lists of detected and suppressed errors, rerun with: -s
==72167== ERROR SUMMARY: 24 errors from 3 contexts (suppressed: 40 from 34)
```
## Observations
The Helgrind tool reports some errors in the given `main_signal.c` program. Here are some observations from the output:
1. Helgrind reports a data race during a write of size 4 by thread #2 at address 0x10C014.
2. The data race conflicts with a previous read of size 4 by the root thread (thread #1).
3. Another data race occurs during a read of size 4 by the root thread, conflicting with a previous write by thread #2.
4. A potential data race during a write of size 1 at address 0x52981A5 by the root thread is detected.

## Conclusions
1. The code lacks proper synchronization for the shared variable `done`.
2. Race conditions lead to conflicting accesses to shared data.

___
___
# Helgrind Analysis for main_signal_cv.c
Reporting the output, observation and conclusions obtained from running the program `main_signal_cv.c` on valgrind (using the tool helgrind)

## Output

```bash
==71671== Helgrind, a thread error detector
==71671== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==71671== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==71671== Command: ./prog
==71671== 
this should print first
this should print last
==71671== 
==71671== Use --history-level=approx or =none to gain increased speed, at
==71671== the cost of reduced accuracy of conflicting-access information
==71671== For lists of detected and suppressed errors, rerun with: -s
==71671== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 7 from 7)
```


## Observations

The Helgrind tool reports no errors in the given `main_signal_cv.c` program. Here are some observations from the output:

1. **No Data Races Detected:** Helgrind did not detect any data races in the program. This indicates that the synchronization mechanisms used in the code (mutex and condition variable) are effectively preventing concurrent access to shared data.

2. **No Deadlocks Detected:** The absence of deadlock errors suggests that the program is correctly using synchronization primitives, and there are no circular dependencies causing threads to wait indefinitely.

3. **Proper Synchronization:** The program uses a `synchronizer_t` structure with a mutex (`lock`) and a condition variable (`cond`) to coordinate between the main thread and the worker thread. This approach ensures that the main thread waits until the worker thread signals that it has completed its task.

## Conclusions

1. **Effective Synchronization:** The use of a mutex and condition variable in the `synchronizer_t` structure appears to be effective in coordinating the actions of the main thread and the worker thread. The program achieves its intended synchronization goals.