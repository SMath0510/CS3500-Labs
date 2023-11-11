#include<bits/stdc++.h>
using namespace std;

#define MAX_MEM 16777216 // 16MB
#define MAX_PROC 1000
#define MAX_PG 1000
#define DEBUG 0
#define TEST 0
#define ERROR_ENTRY -1
#define PASS_ENTRY 1

// Declaring the initial global variables
int MM_SZ = 32*1024; // Main Memory Size 
int VM_SZ = 32*1024; // Virtual Memory Size 
int PG_SZ = 512; // size of a Page
int PID_CNT = 0;  // tracking the pid count
string infile; // name of input file
string outfile; // name of output file

int MM_PG_CNT = MAX_PG; // pages in MM
int VM_PG_CNT = MAX_PG; // pages in VM

int MM[MAX_MEM];  // Representing the MM
int VM[MAX_MEM];  // Representing the VM
int ffMM[MAX_PG]; // Tracking the free frames of MM
int ffVM[MAX_PG]; // Tracking the free frames of VM
int VPID[MAX_PROC]; // Tracking valid processes

int MM_FF_CNT = MAX_PG; // num free frames in MM
int VM_FF_CNT = MAX_PG; // num free frames in VM

set<pair<int,int>> LRU_SET; // Maintaining the LRU Queue
int GLB_TIME; // Maintaining the global time

set<char> DELIM = {' ', '\n', '\t', ','};  // The character set of Delimiters

// Defining Base Utility Functions 
vector<string> split(string s){ 
    // splits a string based on delimiters
    if(DEBUG){
        cout << "Started Splitting " << s << endl;
    }
    s += " ";
    int sz = s.size();
    string cur_str = "";
    vector<string> sep_str;
    for(int i = 0; i < sz; i++){
        if(DELIM.find(s[i]) == DELIM.end()){
            cur_str.push_back(s[i]);
        }
        else{
            if(!cur_str.empty() && cur_str.size() > 0) sep_str.push_back(cur_str);
            cur_str = "";
        }
    }

    if(DEBUG){
        cout << "The splitted string " << endl;
        for(auto &ele: sep_str){
            cout << ele << " ";
        }
        cout << endl;
    }
    return sep_str;
}

int str_to_num(string s){
    // converts a string to an integer
    int num = 0;
    int sz = s.size();
    if(DEBUG){
        cout << "Initial string: " << s << endl;
    }
    for(int i = 0; i < sz; i++){
        if(s[i] <= '9' && s[i] >= '0')
        {
            num *= 10;
            num += (s[i] - '0');
        }
    }
    if(DEBUG){
        cout << "The number format: " << num << endl;
    }
    return num;
}

int ceil(int n, int d){
    // calculates the ceil of (n/d)
    return (n/d + (n%d != 0));
}

int get_pg_no(int addr){
    // calculates the page_number given an address
    return addr / PG_SZ;
}

void remove_set(int pid){
    // removes pid from the LRU SET
    vector<pair<int,int>> to_rem;
    for(auto &ele: LRU_SET){
        if(ele.first == pid){
            to_rem.push_back(ele);
        }
    }

    for(auto &ele: to_rem){
        LRU_SET.erase(ele);
    }
}

class PGEntry{
    public:

    int MMPFNum; // Main Memory Page Frame Number
    int VMPFNum; // Virtual Memory Page Frame Number

    PGEntry(){
        this->MMPFNum = ERROR_ENTRY; // initialized to ERROR ENTRY 
        this->VMPFNum = ERROR_ENTRY; // initialized to ERROR ENTRY
    }
};

class Instr{
    public:

    string instr_type; // type of ins
    int num_args;  // number of input arguments
    vector<string> args;  // vector of arguments

    Instr(){
        this->instr_type = ""; // initialized to empty string
        this->num_args = 0; // 0 args initially
    }

    void print_ins_cnt(){
        cout << "  Ins type: " << this->instr_type << endl;
        cout << "  Num Args: " << this->num_args << endl;
        cout << "  Arguments: ";
        for(int i = 0; i < num_args; i++){
            cout << args[i] << " ";
        }
        cout << endl;
    }
};

class Process{
    public:
    
    bool in_mm; // is the process in mm
    bool in_vm; // is the process in vm
    int num_pg; // number of pages occupied by the process
    int mem_sz; // size of the process (actual)
    int num_instr; // number of instructions to be executed
    int pid;  // pid of the process
    vector<Instr> instr_lst; // list of instructions to be executed
    vector<PGEntry> pageTable; // page table for the process

    Process(){
        this->num_instr = 0;
        this->mem_sz = 0;
        this->num_pg = 0;
        this->in_vm = false;
        this->in_mm = false;
        this->pid = ERROR_ENTRY;
    }

    void add_ins(Instr ins){
        // Adds an instruction to the instruction list
        this->instr_lst.push_back(ins);
    }

    void print_err(int inv_adr, ofstream &out){
        // Prints the error of invalid memory address
        out << "Invalid Memory Address " << inv_adr << " specified for process id " << pid << endl;
    }

    void print_prc_cnt(){
        cout << "Mem sz: " << this->mem_sz << endl;
        cout << "Num ins: " << this->num_instr << endl;
        cout << "PID: " << this->pid << endl;
        cout << "Location: " << this->in_mm << this->in_vm << endl;
        cout << "NumPages: " << this->num_pg << endl;
        cout << "Instructions: " << endl;
        for(int i = 0; i < num_instr; i++){
            instr_lst[i].print_ins_cnt();
        }
        cout << "Mapping: " << endl;
        for(int i = 0; i < num_pg; i++){
            cout << "  " << i << " := ";
            if(pageTable[i].MMPFNum != ERROR_ENTRY) cout << pageTable[i].MMPFNum << endl;
            else if(pageTable[i].VMPFNum != ERROR_ENTRY) cout << pageTable[i].VMPFNum << endl;
        }
    }

    bool check_validity(int adr){
        // Checks if address is valid
        return adr < mem_sz;
    }

    int get_real_addr(int adr, ofstream &out){
        // Locates the actual memory address from the process local address
        int pf_pgn = get_pg_no(adr);
        int offset = adr - (pf_pgn * PG_SZ);
        int r_adr = ERROR_ENTRY;
        if(DEBUG){
            cout << "Input Addr: " << adr << endl;
        }
        if(in_mm) {
            int mm_pgn = pageTable[pf_pgn].MMPFNum;
            r_adr = (mm_pgn * PG_SZ) + offset;
        }
        else if(in_vm){
            int vm_pgn = pageTable[pf_pgn].VMPFNum;
            r_adr = (vm_pgn * PG_SZ) + offset;
        } 
        if(DEBUG){
            cout << "Returning Addr: " << r_adr << endl;
        }

        if(r_adr == ERROR_ENTRY){
            out << "Unable to translate " << adr << " to valid memory location" << endl;
        }
        return r_adr;        
    }

    int add(int adr1, int adr2, int adr3, ofstream &out){
        // Adds the numbers 

        // Check validity
        if(!check_validity(adr1)) {
            print_err(adr1, out);
            return ERROR_ENTRY;
        }
        if(!check_validity(adr2)) {
            print_err(adr2, out);
            return ERROR_ENTRY;
        }
        if(!check_validity(adr3)) {
            print_err(adr3, out);
            return ERROR_ENTRY;
        }

        if(in_mm){
            // Get the real address and check bounds
            int radr1 = get_real_addr(adr1, out);
            if(radr1 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr1 >= MM_SZ){
                out << "Out of bounds access " << radr1 << endl;
                return ERROR_ENTRY;
            }
            
            int radr2 = get_real_addr(adr2, out);
            if(radr2 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr2 >= MM_SZ){
                out << "Out of bounds access " << radr2 << endl;
                return ERROR_ENTRY;
            }

            int radr3 = get_real_addr(adr3, out);
            if(radr3 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr3 >= MM_SZ){
                out << "Out of bounds access " << radr3 << endl;
                return ERROR_ENTRY;
            }
            
            // Perform the calculation
            int x = MM[radr1];
            int y = MM[radr2];
            int z = x + y;
            MM[radr3] = z;
            out << "Command: add " << adr1 << ", " << adr2 << ", " << adr3 << "; Result: Value in addr " << adr1 << " = " << x << ", addr " << adr2 << " = " << y << ", addr " << adr3 << " = " << z << endl;
        }
        else if(in_vm){
            // Get the real addresses and check bounds
            int radr1 = get_real_addr(adr1, out);
            if(radr1 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr1 >= VM_SZ){
                out << "Out of bounds access " << radr1 << endl;
                return ERROR_ENTRY;
            }
            
            int radr2 = get_real_addr(adr2, out);
            if(radr2 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr2 >= VM_SZ){
                out << "Out of bounds access " << radr2 << endl;
                return ERROR_ENTRY;
            }

            int radr3 = get_real_addr(adr3, out);
            if(radr3 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr3 >= VM_SZ){
                out << "Out of bounds access " << radr3 << endl;
                return ERROR_ENTRY;
            }
            
            // Perform the operation
            int x = VM[radr1];
            int y = VM[radr2];
            int z = x + y;
            VM[radr3] = z;
            out << "Command: add " << adr1 << ", " << adr2 << ", " << adr3 << "; Result: Value in addr " << adr1 << " = " << x << ", addr " << adr2 << " = " << y << ", addr " << adr3 << " = " << z << endl;
        }

        return PASS_ENTRY;
    }   

    int sub(int adr1, int adr2, int adr3, ofstream &out){
        // Check validity
        if(!check_validity(adr1)) {
            print_err(adr1, out);
            return ERROR_ENTRY;
        }
        if(!check_validity(adr2)) {
            print_err(adr2, out);
            return ERROR_ENTRY;
        }
        if(!check_validity(adr3)) {
            print_err(adr3, out);
            return ERROR_ENTRY;
        }

        if(in_mm){
            // Get the real address and check bounds
            int radr1 = get_real_addr(adr1, out);
            if(radr1 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr1 >= MM_SZ){
                out << "Out of bounds access " << radr1 << endl;
                return ERROR_ENTRY;
            }
            
            int radr2 = get_real_addr(adr2, out);
            if(radr2 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr2 >= MM_SZ){
                out << "Out of bounds access " << radr2 << endl;
                return ERROR_ENTRY;
            }

            int radr3 = get_real_addr(adr3, out);
            if(radr3 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr3 >= MM_SZ){
                out << "Out of bounds access " << radr3 << endl;
                return ERROR_ENTRY;
            }
            
            // Perform the actual operation
            int x = MM[radr1];
            int y = MM[radr2];
            int z = x - y;
            MM[radr3] = z;
            out << "Command: sub " << adr1 << ", " << adr2 << ", " << adr3 << "; Result: Value in addr " << adr1 << " = " << x << ", addr " << adr2 << " = " << y << ", addr " << adr3 << " = " << z << endl;
        }
        else if(in_vm){
            // Get the real address and check bounds
            int radr1 = get_real_addr(adr1, out);
            if(radr1 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr1 >= VM_SZ){
                out << "Out of bounds access " << radr1 << endl;
                return ERROR_ENTRY;
            }
            
            int radr2 = get_real_addr(adr2, out);
            if(radr2 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr2 >= VM_SZ){
                out << "Out of bounds access " << radr2 << endl;
                return ERROR_ENTRY;
            }
            
            int radr3 = get_real_addr(adr3, out);
            if(radr3 == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr3 >= VM_SZ){
                out << "Out of bounds access " << radr3 << endl;
                return ERROR_ENTRY;
            }

            // Perform the actual operation
            int x = VM[radr1];
            int y = VM[radr2];
            int z = x - y;
            VM[radr3] = z;
            out << "Command: sub " << adr1 << ", " << adr2 << ", " << adr3 << "; Result: Value in addr " << adr1 << " = " << x << ", addr " << adr2 << " = " << y << ", addr " << adr3 << " = " << z << endl;
        }

        return PASS_ENTRY;
    }   

    int load(int val, int adr, ofstream &out){
        // Check validity
        if(!check_validity(adr)) {
            print_err(adr, out);
            return ERROR_ENTRY;
        }
        
        if(in_mm){
            // Get the real address and check bounds
            int radr = get_real_addr(adr, out);
            if(radr == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr >= MM_SZ){
                out << "Out of bounds access " << radr << endl;
                return ERROR_ENTRY;
            }
            
            // Perform the operation
            MM[radr] = val;
            out << "Command: load " << val << ", " << adr << "; Result Value of " << val << " is now stored in addr " << adr << endl;
        }
        else if(in_vm){
            // Get the real address and check bounds
            int radr = get_real_addr(adr, out);
            if(radr == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr >= VM_SZ){
                out << "Out of bounds access " << radr << endl;
                return ERROR_ENTRY;
            }

            // Perform the operation
            VM[radr] = val;
            out << "Command: load " << val << ", " << adr << "; Result Value of " << val << " is now stored in addr " << adr << endl;
        }

        return PASS_ENTRY;
    }

    int print(int adr, ofstream &out){
        // Check validity
        if(!check_validity(adr)) {
            print_err(adr, out);
            return ERROR_ENTRY;
        }

        if(in_mm){
            // Get the real address and check bounds
            int radr = get_real_addr(adr, out);
            if(radr == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr >= MM_SZ){
                out << "Out of bounds access " << radr << endl;
                return ERROR_ENTRY;
            }

            // Perform the operation
            int val = MM[radr];
            out << "Command: print " << adr << "; Result: Value in addr " << adr << " = " << val << endl;
        }
        else if(in_vm){
            // Get the real address and check bounds
            int radr = get_real_addr(adr, out);
            if(radr == ERROR_ENTRY) return ERROR_ENTRY;
            if(radr >= VM_SZ){
                out << "Out of bounds access " << radr << endl;
                return ERROR_ENTRY;
            }

            // Perform the operation
            int val = VM[radr];
            out << "Command: print " << adr << "; Result: Value in addr " << adr << " = " << val << endl;
        }

        return PASS_ENTRY;
    }

};

// Global Process Storage
Process pid_to_prc[MAX_PROC];

// Additional Helper Functions

class PagingHelper{

    public: 
    bool check_space(int num_pg, bool &mm, bool &vm, ofstream &out){
        // checks for available space (if it exists or not)
        if(MM_FF_CNT >= num_pg) {
            mm = true;
            vm = false;
        }
        else if(VM_FF_CNT >= num_pg){
            mm = false;
            vm = true;
        }
        else{
            mm = false;
            vm = false;
        }
        return (mm ^ vm);
    }

    bool check_space_mm(int num_pg, ofstream &out){
        // Checks for available space in Main Memory
        return MM_FF_CNT >= num_pg;
    }

    bool check_space_vm(int num_pg, ofstream &out){
        // Checks for available space in Virtual Memory
        return VM_FF_CNT >= num_pg;
    }

    int get_freloc(Process &prc, ofstream &out, bool force_mm = false, bool force_vm = false){
        // Assigns the page frames to the process (if it exists)
        // Acts as a helper function to load into Main Memory / Virtual Memory (forcefully)

        int prc_sz = prc.mem_sz;
        int num_pg = prc.num_pg;
        int rem_pg = num_pg;
        int cur_pg = 0;
        bool mm = false, vm = false;

        bool can_insert = check_space(num_pg, mm, vm, out);
        if(!can_insert) {
            // Cannot load the process - insufficient memory
            return ERROR_ENTRY;
        }
        bool change_pid = !(force_mm || force_vm);

        // Forced into main memory
        if(force_mm){
            if(check_space_mm(num_pg, out)){
                mm = true;
                vm = false;
            }
            else {
                if(DEBUG){
                    cout << "Forced Main Memory" << endl;
                }
                out << "Process " << prc.pid << " couldn't be swapped in - insufficient memory" << endl;
                return ERROR_ENTRY;
            }
        }

        // Forced into virtual memory
        if(force_vm){
            if(check_space_vm(num_pg, out)){
                mm = false;
                vm = true;
            }
            else {
                if(DEBUG){
                    cout << "Forced Virtual Memory" << endl;
                }
                out << "Process " << prc.pid << " couldn't be swapped out - insufficient memory" << endl;
                return ERROR_ENTRY;
            }
        }

        prc.in_mm = mm;
        prc.in_vm = vm;
        if(mm){
            int cur_pg = 0;
            for(int i = 0; i < MM_PG_CNT; i++){
                if(ffMM[i] == PASS_ENTRY){
                    prc.pageTable[cur_pg].MMPFNum = i;
                    ffMM[i] = 0;
                    MM_FF_CNT --;
                    cur_pg ++;
                }
                if(cur_pg == num_pg) {
                    if(change_pid){
                        PID_CNT ++;
                        prc.pid = PID_CNT;
                        pid_to_prc[prc.pid] = prc;
                        VPID[PID_CNT] = PASS_ENTRY;
                        // Initializing into the Main Memory and adding to LRU Cache
                        remove_set(prc.pid);
                        GLB_TIME ++;
                        LRU_SET.insert({GLB_TIME, prc.pid});
                    }
                    return PASS_ENTRY;
                }
            }
        }
        else if(vm){
            int cur_pg = 0;
            for(int i = 0; i < VM_PG_CNT; i++){
                if(ffVM[i] == PASS_ENTRY){
                    prc.pageTable[cur_pg].VMPFNum = i;
                    ffVM[i] = 0;
                    VM_FF_CNT --;
                    cur_pg ++;
                }
                if(cur_pg == num_pg) {
                    if(change_pid){
                        PID_CNT ++;
                        prc.pid = PID_CNT;
                        pid_to_prc[prc.pid] = prc;
                        VPID[PID_CNT] = PASS_ENTRY;
                    }
                    return PASS_ENTRY;
                }
            }
        }
        out << "Invalid Process PID " << prc.pid << " provided " << endl;
        return ERROR_ENTRY;
    }

    int load_into_mm(int pid, ofstream &out){
        // Forcefully load into Main Memory
        return get_freloc(pid_to_prc[pid], out, true, false);
    }

    int load_into_vm(int pid, ofstream &out){
        // Forcefully load into Virtual Memory
        return get_freloc(pid_to_prc[pid], out, false, true);
    }


    int load_process(string fname, ofstream &out){
        // Load the file content into a Process
        if(fname.empty()){
            return PASS_ENTRY;
        }
        ifstream f1;
        f1.open(fname);
        if(!f1){
            out << fname << " could not be loaded - file does not exists" << endl;
            out << endl;
            return ERROR_ENTRY;
        }

        Process prc;

        string cur_line;
        int line_no = 0;

        // Parse the file
        while (getline (f1, cur_line)) {
            if(cur_line[cur_line.size() - 1] == '\r'){
                cur_line.pop_back();
            }
            if(line_no == 0){
                int mem_sz = str_to_num(cur_line); 
                mem_sz *= 1024;  // memory size in bytes
                prc.mem_sz = mem_sz; // the actual size of the process
                prc.num_pg = ceil(mem_sz,PG_SZ); // number of pages needed
                prc.pageTable.resize(prc.num_pg); // initializing the page_table
            }
            else{
                Instr ins;
                vector<string> split_args = split(cur_line);

                string ins_type = split_args[0];
                split_args.erase(split_args.begin());
                int num_args = split_args.size();
                
                ins.instr_type = ins_type;
                ins.num_args = num_args;
                ins.args = split_args;

                prc.add_ins(ins);
            }
            line_no ++;
        }
        f1.close();
        //  contents stored in the process

        int num_ins = line_no;
        num_ins --;

        prc.num_instr = num_ins;

        // Allocate space in the Physical Space
        int stat = get_freloc(prc, out);
        if(stat == ERROR_ENTRY) return ERROR_ENTRY;

        if(DEBUG){
            prc.print_prc_cnt();
        }

        if(prc.in_mm){
            out << fname << " is loaded in main memory and is assigned process id " << prc.pid << endl;
            out << endl;
            return PASS_ENTRY;
        }
        else if(prc.in_vm){
            out << fname << " is loaded in virtual memory and is assigned process id " << prc.pid << endl;
            out << endl;
            return PASS_ENTRY;
        }
        else{
            // Assuming the filename does not exist has been handled
            out << fname << " could not be loaded - memory is full" << endl;
            out << endl;
            return ERROR_ENTRY;
        }
    }


    int run_process(int pid, ofstream &out){
        // Run the process
        if(VPID[pid] == ERROR_ENTRY) {
            out << "Invalid Process PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        // Get it into main memory
        if(pid_to_prc[pid].in_vm){
            int stat = swap_in_process(pid, out);
            if(stat == ERROR_ENTRY){
                out << "Could not run the process " << pid << " - can't bring to Main Memory" << endl;
            }
            return ERROR_ENTRY;
        }

        // Update the LRU SET
        remove_set(pid);
        GLB_TIME ++;
        LRU_SET.insert({GLB_TIME, pid});

        out << "Running the Processs " << pid << endl;
        // Execute the instruction list
        vector<Instr> ins_lst = pid_to_prc[pid].instr_lst;
        int num_ins = pid_to_prc[pid].num_instr;
        for(int i = 0; i < num_ins; i++){
            Instr cur_ins = ins_lst[i];
            string ins_type = cur_ins.instr_type;
            vector<string> inp_args = cur_ins.args;
            int num_args = cur_ins.num_args;
            if(ins_type == "load"){
                if(inp_args.size() != 2){
                    out << "Invalid syntax. Expected Arguments: 2 - Received Arguments " << inp_args.size() << endl;
                    return ERROR_ENTRY;
                }
                int val = str_to_num(inp_args[0]);
                int adr = str_to_num(inp_args[1]);
                int stat = pid_to_prc[pid].load(val, adr, out);
                if(stat == ERROR_ENTRY){
                    out << "Failed to run the instruction " << ins_type << " in the process " << pid << endl;
                    continue;
                }
            }
            else if(ins_type == "add"){
                if(inp_args.size() != 3){
                    out << "Invalid syntax. Expected Arguments: 3 - Received Arguments " << inp_args.size() << endl;
                    return ERROR_ENTRY;
                }
                int adr1 = str_to_num(inp_args[0]);
                int adr2 = str_to_num(inp_args[1]);
                int adr3 = str_to_num(inp_args[2]);
                int stat = pid_to_prc[pid].add(adr1, adr2, adr3, out);
                if(stat == ERROR_ENTRY){
                    out << "Failed to run the instruction " << ins_type << " in the process " << pid << endl;
                    continue;
                }
            }
            else if(ins_type == "sub"){
                if(inp_args.size() != 3){
                    out << "Invalid syntax. Expected Arguments: 3 - Received Arguments " << inp_args.size() << endl;
                    return ERROR_ENTRY;
                }
                int adr1 = str_to_num(inp_args[0]);
                int adr2 = str_to_num(inp_args[1]);
                int adr3 = str_to_num(inp_args[2]);
                int stat = pid_to_prc[pid].sub(adr1, adr2, adr3, out);
                if(stat == ERROR_ENTRY){
                    out << "Failed to run the instruction " << ins_type << " in the process " << pid << endl;
                    continue;
                }
            }
            else if(ins_type == "print"){
                if(inp_args.size() != 1){
                    out << "Invalid syntax. Expected Arguments: 1 - Received Arguments " << inp_args.size() << endl;
                    return ERROR_ENTRY;
                }
                int adr = str_to_num(inp_args[0]);
                int stat = pid_to_prc[pid].print(adr, out);
                if(stat == ERROR_ENTRY){
                    out << "Failed to run the instruction " << ins_type << " in the process " << pid << endl;
                    continue;
                }
            }
            else{
                out << "Unknown command " << ins_type << endl;
                return ERROR_ENTRY;
            }
        }

        out << "Successfully ran the run command for the process " << pid << endl;
        out << endl;
        return PASS_ENTRY;
    }


    int kill_process(int pid, ofstream &out, bool change_pid = true){
        // Kill the process and reset its pid
        // Helper function to the free process
        if(VPID[pid] == ERROR_ENTRY) {
            out << "Invalid Process PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        // Remove from the LRU SET
        remove_set(pid);

        if(pid_to_prc[pid].in_mm){
            int num_pg = pid_to_prc[pid].num_pg;
            for(int i = 0; i < num_pg; i++){
                int mmpg_no = pid_to_prc[pid].pageTable[i].MMPFNum;
                pid_to_prc[pid].pageTable[i].MMPFNum = ERROR_ENTRY;
                ffMM[mmpg_no] = PASS_ENTRY;
                MM_FF_CNT ++;
                pid_to_prc[pid].in_mm = false;
            }
        }
        else if(pid_to_prc[pid].in_vm){
            int num_pg = pid_to_prc[pid].num_pg;
            for(int i = 0; i < num_pg; i++){
                int mmpg_no = pid_to_prc[pid].pageTable[i].VMPFNum;
                pid_to_prc[pid].pageTable[i].VMPFNum = ERROR_ENTRY;
                ffVM[mmpg_no] = PASS_ENTRY;
                VM_FF_CNT ++;
                pid_to_prc[pid].in_vm = false;
            }
        }
        else {
            out << "Invalid Process PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        // If Only Free Memory then no change PID
        if(change_pid) {
            pid_to_prc[pid].pid = ERROR_ENTRY;
            VPID[pid] = ERROR_ENTRY;
            out << "Successfully executed the kill command for the process " << pid << endl;
        }

        return PASS_ENTRY;
    }

    int free_process(int pid, ofstream &out){
        // Only free the memory
        if(VPID[pid] == ERROR_ENTRY) {
            cout << "Invalid Process PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        return kill_process(pid, out, false);
    }


    void print_process(ofstream &out){
        // Print the processes that are in the Physical System
        out << "The processes in the Main Memory are: " << endl;
        out << "  ";
        for(int i = 1; i <= PID_CNT; i++){
            if(VPID[i] == ERROR_ENTRY) continue;
            if(pid_to_prc[i].in_mm){
                out << i << "; ";
            }
        }
        out << endl;
        out << "The processes in the Virtual Memory are: " << endl;
        out << "  ";
        for(int i = 1; i <= PID_CNT; i++){
            if(VPID[i] == ERROR_ENTRY) continue;
            if(pid_to_prc[i].in_vm){
                out << i << "; ";
            }
        }
        out << endl;
    }

    int store_process(int pid, string fname, ofstream &out){
        // Store the Page Table Mapping of the Process into the file fname
        if(VPID[pid] == ERROR_ENTRY) {
            out << "Invalid Process PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }
        
        ofstream file;
        file.open(fname, ios::app);

        if(!file){
            out << "Could not store the page information - file " << fname << " does not exists" << endl;
            return ERROR_ENTRY;
        }
        out << "Process " << pid << " Page Table Information is being loaded into " << fname << endl;
        time_t currentTime = time(0);

        // Convert the current time to a string
        char* timeString = ctime(&currentTime);

        // Print the current time and date
        file << "Entry time and date: " << timeString << endl;

        file << "Process " << pid << " Page Table Information" << endl;

        int num_pg = pid_to_prc[pid].num_pg;
        if(pid_to_prc[pid].in_mm){
            for(int i = 0; i < num_pg; i++){
                file << "Logical Page " << i << " is mapped to " << " Physical (Main) Page Number " <<  pid_to_prc[pid].pageTable[i].MMPFNum << endl; 
            }
        }
        else if(pid_to_prc[pid].in_vm){
            for(int i = 0; i < num_pg; i++){
                file << "Logical Page " << i << " is mapped to " << " Physical (Virtual) Page Number " <<   pid_to_prc[pid].pageTable[i].VMPFNum << endl; 
            }
        }
        file << endl;
        out << "Sucessfully stored the PT contents of Process " << pid << " in " << fname << endl; 
        file.close();
        return PASS_ENTRY;
    }

    int print_mm_content(int memloc, int len, ofstream &out){
        // Print the memory content from memloc to memloc + len - 1
        if(len < 0) {
            out << "Invalid Traversing (len < 0)" << endl;
            return ERROR_ENTRY;
        }
        if(memloc + len >= MM_SZ){
            out << "Out of bounds access " << memloc + len << endl;
            return ERROR_ENTRY;
        }

        out << "Printing the Memory Content from " << memloc << " to " << memloc + len  - 1 << endl;
        for(int i = 0; i < len; i++){
            out << "  " << (memloc + i) << " := " << MM[memloc + i] << endl;
        }
        out << endl;
        out << "Successfully executed the print Memory command" << endl;
        return PASS_ENTRY;
    }

    int swap_out_process(int pid, ofstream &out){
        // Swap out the process from Main Memory to Virtual Memory
        if(VPID[pid] == ERROR_ENTRY) {
            out << "Invalid PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        // Bring from MM topid_to_prc[pid]
        if(pid_to_prc[pid].in_vm){
            out << "Process " << pid << " is already present in Virtual Memory" << endl;
            return PASS_ENTRY;
        }
        int sz = pid_to_prc[pid].mem_sz;
        int num_pg = pid_to_prc[pid].num_pg;

        // First freeing from the MM 
        if(pid_to_prc[pid].in_mm){
            free_process(pid, out); // just free the memory
        }

        if(check_space_vm(num_pg, out)){
            load_into_vm(pid, out);  // force to get free location in VM
            out << "Process " << pid << " has been sucessfully swapped out of the Main Memory into the Virtual Memory" << endl;
            return PASS_ENTRY;
        }
        load_into_mm(pid, out);
        // Reloading into Main Memory
        out << "Unable to swap out the process " << pid << " - insufficient memory" << endl;
        return ERROR_ENTRY;
    }

    int build_space_mm(int num_pg, ofstream &out){
        // Build space in Main memory (for Swap In)
        for(auto &ele: LRU_SET){
            if(check_space_mm(num_pg, out)){
                return PASS_ENTRY;
            }
            int pid = ele.second;
            if(VPID[pid] == ERROR_ENTRY) continue;
            int stat = swap_out_process(pid, out); // swap out the Least Recently Used Process
            if(stat == ERROR_ENTRY) return ERROR_ENTRY; 
        }
        return PASS_ENTRY;
    }

    int swap_in_process(int pid, ofstream &out){
        // Swap In the process
        if(VPID[pid] == ERROR_ENTRY) {
            out << "Invalid PID " << pid << " provided " << endl;
            return ERROR_ENTRY;
        }

        if(pid_to_prc[pid].in_mm){
            out << "Process " << pid << " is already present in Main Memory" << endl;
            return PASS_ENTRY;
        }

        int sz = pid_to_prc[pid].mem_sz;
        int num_pg = pid_to_prc[pid].num_pg;

        // First freeing the process from VM then checking for space
        if(pid_to_prc[pid].in_vm){
            free_process(pid, out); // just free the memory
        }

        // build_space for the process
        int stat = build_space_mm(num_pg, out);

        // Check if space exists
        if(check_space_mm(num_pg, out)){
            load_into_mm(pid, out);  // force to get free location in VM
            out << "Process " << pid << " has been sucessfully swapped into the Main Memory from the Virtual Memory" << endl;
            
            // Inserting into the LRU
            remove_set(pid);
            GLB_TIME ++;
            LRU_SET.insert({GLB_TIME, pid});
            return PASS_ENTRY;
        }
        load_into_vm(pid, out);
        // Reloading into the VM
        out << "Unable to swap out the process " << pid << " - insufficient memory" << endl;
        return ERROR_ENTRY;
    }

    void initialize(){
        // initialize the system
        for(int i = 0; i < MAX_PG; i++){
            ffMM[i] = ffVM[i] = PASS_ENTRY; // All are free
        }

        for(int i = 0; i < MAX_MEM; i++){
            MM[i] = VM[i] = ERROR_ENTRY; // initialized to ERROR_ENTRY
        }

        for(int i = 0; i < MAX_PROC; i++){
            VPID[i] = ERROR_ENTRY; // initialized to ERROR_ENTRY
        }
        LRU_SET.clear();
        GLB_TIME = 0;
    }

};

int main(int argc, char * argv[]){

    PagingHelper solver;
    solver.initialize(); // initializing the system

    if(argc != 11){
        cout << "11 Arguments required. Given " << argc << " arguments" << endl;
        return 1;
    }

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-M") == 0) {
            MM_SZ = stoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-V") == 0) {
            VM_SZ = stoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-P") == 0) {
            PG_SZ = stoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-i") == 0) {
            infile = argv[i + 1];
        } else if (strcmp(argv[i], "-o") == 0) {
            outfile = argv[i + 1];
        } else {
            cout << "Unknown option: " << argv[i] << endl;
            return 1; 
        }
    }

    // Declaring the sizes from the inputs
    MM_SZ *= 1024;
    VM_SZ *= 1024;

    MM_PG_CNT = MM_SZ / PG_SZ;
    VM_PG_CNT = VM_SZ / PG_SZ;

    MM_FF_CNT = MM_PG_CNT;
    VM_FF_CNT = VM_PG_CNT;
    
    ifstream inp (infile);
    ofstream out (outfile);

    Process inp_prc;
    string cur_line;

    // Parsing the input file
    while (getline (inp, cur_line)) {
        if(cur_line[cur_line.size() - 1] == '\r'){
            cur_line.pop_back();
        }
        cur_line += " ";
        Instr ins;
        vector<string> split_args = split(cur_line);
        string ins_type = split_args[0];
        split_args.erase(split_args.begin());
        int num_args = split_args.size();
        
        ins.instr_type = ins_type;
        ins.num_args = num_args;
        ins.args = split_args;

        inp_prc.add_ins(ins);
        inp_prc.num_instr ++;
    }

    // Executing the instructions
    for(int i = 0; i < inp_prc.num_instr; i++){
        Instr cur_ins = inp_prc.instr_lst[i];
        string ins_type = cur_ins.instr_type;

        if(ins_type == "load"){
            // load
            int num_args = cur_ins.num_args;
            for(int j = 0; j < num_args; j++){
                string fname = cur_ins.args[j];
                int stat = solver.load_process(fname, out);
            }
        }
        else if(ins_type == "run"){
            // run
            int pid = str_to_num(cur_ins.args[0]);
            int stat = solver.run_process(pid, out);
        }
        else if (ins_type == "kill"){
            // kill
            int pid = str_to_num(cur_ins.args[0]);
            int stat = solver.kill_process(pid, out);
        }
        else if(ins_type == "listpr"){
            solver.print_process(out);
        }
        else if(ins_type == "pte"){
            int pid = str_to_num(cur_ins.args[0]);
            string fname = cur_ins.args[1];
            int stat = solver.store_process(pid, fname, out);
        }
        else if(ins_type == "pteall"){
            string fname = cur_ins.args[0];
            for(int i = 1; i <= PID_CNT; i++){
                if(VPID[i] == 1) int stat = solver.store_process(i, fname, out);
            }
        }
        else if(ins_type == "swapin"){
            int pid = str_to_num(cur_ins.args[0]);
            int stat = solver.swap_in_process(pid, out);
        }
        else if(ins_type == "swapout"){
            int pid = str_to_num(cur_ins.args[0]);
            int stat = solver.swap_out_process(pid, out);
        }
        else if(ins_type == "print"){
            int memloc = str_to_num(cur_ins.args[0]);
            int len = str_to_num(cur_ins.args[1]);
            int stat = solver.print_mm_content(memloc, len, out);
        }
        else if(ins_type == "exit"){
            solver.initialize();
            MM_FF_CNT = MM_PG_CNT;
            VM_FF_CNT = VM_PG_CNT;
        }
        else{
            cout << "Unknown command " << ins_type << endl;
        }
    }
    return 0;
}