
**Command Used to Generate the Case:**

In this case, there are many processes, where one is just a little longer than others. The `%CPU` given to the program is set to 4.8% (an edge case) with the following parameters: Quantum = 10, Boost = 200, numProcess = 20.


```bash
> python3 .\mlfq.py -l 0,101,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0:0,100,0 -B 200 -q 10 -c
```

**Alternative Case:**

Another case worked out with different parameters:

```bash
> python3 .\mlfq.py -l 0,45,0:0,15,15 -c -q 10 -B 100 -i 5
```

**Case with 20 Processes:**

In this case, 20 processes with specific arrival and burst times, quantum = 10, and boost = 200:

```bash
> python3 .\mlfq.py -l 0,20,0:10,10,0:20,10,0:30,10,0:40,10,0:50,10,0:60,10,0:70,10,0:80,10,0:90,10,0:100,10,0:110,10,0:120,10,0:130,10,0:140,10,0:150,10,0:160,10,0:170,10,0:180,10,0:190,10,0 -q 10 -B 200 -c
```

**Extended Case with 40 Processes:**

An extension of the previous case with 40 processes:

```bash
> python3 .\mlfq.py -l 0,20,0:10,10,0:20,10,0:30,10,0:40,10,0:50,10,0:60,10,0:70,10,0:80,10,0:90,10,0:100,10,0:110,10,0:120,10,0:130,10,0:140,10,0:150,10,0:160,10,0:170,10,0:180,10,0:190,10,0:200,10,0:210,10,0:220,10,0:230,10,0:240,10,0:250,10,0:260,10,0:270,10,0:280,10,0:290,10,0:300,10,0:310,10,0:320,10,0:330,10,0:340,10,0:350,10,0:360,10,0:370,10,0:380,10,0:390,10,0 -B 200 -q 10 -c
```

**Edge Case with 39 Processes and 1 Process:**

An edge case with 39 processes (10 burst time, 0 io) and 1 process (11 burst time, 0 io), quantum = 10, and boost = 200:

```bash
> python3 .\mlfq.py -l 0,11,0:10,10,0:20,10,0:30,10,0:40,10,0:50,10,0:60,10,0:70,10,0:80,10,0:90,10,0:100,10,0:110,10,0:120,10,0:130,10,0:140,10,0:150,10,0:160,10,0:170,10,0:180,10,0:190,10,0:200,10,0:210,10,0:220,10,0:230,10,0:240,10,0:250,10,0:260,10,0:270,10,0:280,10,0:290,10,0:300,10,0:310,10,0:320,10,0:330,10,0:340,10,0:350,10,0:360,10,0:370,10,0:380,10,0:390,10,0 -B 200 -q 10 -c
```

**Assumptions:**

- The priority of boosted processes is determined by the order they left the CPU.
- Incoming processes are added after boosted processes to the topmost queue.

**Final Note:**

- Final answer for required boost time = 200 units.
- With boost time less than 200 units, more than 5% is assured in edge cases.
- With boost time greater than 200 units, less than 5% is assured in edge cases.
```