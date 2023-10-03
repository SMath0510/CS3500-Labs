#include<bits/stdc++.h>
#include<fstream>
using namespace std;

#define debug false
#define final true 

class Process{
    private:
    int arrivalTime, scheduleTime, finishTime, remainingTime, currLevel, timeSinceLastRun, runningSince, initialQueue;
    int id;
    bool justin;

    public:

    Process(){}
    Process(int ID, int arrTime, int initLevel, int burstTime){
        id = ID;
        arrivalTime = arrTime;
        remainingTime = burstTime;
        initialQueue = initLevel;
        finishTime = -1;
        scheduleTime = -1;
        currLevel = initLevel;
        timeSinceLastRun = 0;
        runningSince = -1;
        justin = false;
    }

    bool isFinished(){
        return finishTime != -1;
    }

    bool isScheduled(){
        return scheduleTime != -1;
    }

    bool isRunning(){
        return runningSince != -1;
    }

    void markInserted(){
        justin = true;
    }

    void doneInserted(){
        justin = false;
    }

    int getID(){
        return id;
    }

    int getLevel(){
        return currLevel;
    }

    int getArrivalTime(){
        return arrivalTime;
    }

    int getRemainingTime(){
        return remainingTime;
    }

    int getStartOfRunTime(){
        return runningSince;
    }

    int getTimeSinceLastRun(){
        return timeSinceLastRun;
    }

    bool justIn(){
        return justin;
    }

    void printCheck(){
        cout << "ID: " << id << ", arrT = " << arrivalTime << ", remT = " << remainingTime << ", lev = " << currLevel << ", runT = " << runningSince << ", runSin = " << timeSinceLastRun << endl;
    }

    void printFinal(){
        cout << "ID: " << id << ", arrivalTime = " << arrivalTime << ", remTime = " << remainingTime << ", initLevel = " << currLevel << endl;
    }

    void setScheduleTime(int t){
        scheduleTime = t;
    }

    void setFinishTime(int t){
        finishTime = t;
    }

    void setRunningTime(int t){
        runningSince = t;
    }

    void setRemainingTime(int t){
        remainingTime = t;
    }

    void setTimeSinceLastRun(int t){
        timeSinceLastRun = t;
    }

    void setLevel(int l){
        currLevel = l;
    }

    void insertMessage(){
        cout << "Inserted " << id << " in " << currLevel << endl;
    }

    void updateLevelMessage(){
        cout << "Updated " << id << " from " << currLevel - 1 << " to " << currLevel << endl;
    }

    void finishedMessage(){
        cout << "Finished " << id << " at " << finishTime << endl;
    }

    void runningMessage(){
        cout << "Running " << id << " since " << runningSince << endl;
    }

    void switchMessage(){
        cout << "Switched " << id << " in the final level \n";
    }

    void finishProcessFinalMessage(ofstream &fout){
        fout << "ID: " << id << "; Orig. Level: " << initialQueue << "; Final Level: " << currLevel << "; Arr. Time(ms): " << arrivalTime << "; Comp. Time(ms): " << finishTime << ";TAT (ms): " << (finishTime - arrivalTime) << endl;
    }
    
    void incTimeSinceLastRun(){
        timeSinceLastRun ++;
    }
};

vector<string> splitLines(string s){
    s += " ";
    vector<string> seperated;
    string t = "";
    for(int i = 0; i < s.size(); i++){
        if(s[i] == ' ' || s[i] == '\n'){
            seperated.push_back(t);
            t = "";
        }
        else{
            t.push_back(s[i]);
        }
    }
    return seperated;
}

bool sortByArrival(Process &a, Process &b){
    return a.getArrivalTime() < b.getArrivalTime();
}

bool sortByID(Process &a, Process &b){
    return a.getID() < b.getID();
}

void printProcessList(vector<Process> &processList){
    for(auto &p: processList) p.printCheck();
    cout << endl;
}

void insertProcess(Process &p, vector<Process> &Q1, vector<Process> &Q2, vector<Process> &Q3, vector<Process> &Q4){
    if(p.getLevel() == 1) Q1.push_back(p);
    if(p.getLevel() == 2) Q2.push_back(p);
    if(p.getLevel() == 3) Q3.push_back(p);
    if(p.getLevel() == 4) Q4.push_back(p);
}

void removeProcess(Process &p, vector<Process> &Q1, vector<Process> &Q2, vector<Process> &Q3, vector<Process> &Q4){
    if(p.getLevel() == 1) {
        vector<Process> nQ;
        for(auto &p1: Q1){
            if(p1.getID() == p.getID()) continue;
            nQ.push_back(p1);
        }
        Q1 = nQ;
    }
    if(p.getLevel() == 2) {
        vector<Process> nQ;
        for(auto &p2: Q2){
            if(p2.getID() == p.getID()) continue;
            nQ.push_back(p2);
        }
        Q2 = nQ;
    }
    if(p.getLevel() == 3) {
        vector<Process> nQ;
        for(auto &p3: Q3){
            if(p3.getID() == p.getID()) continue;
            nQ.push_back(p3);
        }
        Q3 = nQ;
    }
    if(p.getLevel() == 4) {
        vector<Process> nQ;
        for(auto &p4: Q4){
            if(p4.getID() == p.getID()) continue;
            nQ.push_back(p4);
        }
        Q4 = nQ;
    }
}

void updateLevelOfProcess(Process &p, vector<Process> &Q1, vector<Process> &Q2, vector<Process> &Q3, vector<Process> &Q4){
    int currLevel = p.getLevel();
    removeProcess(p, Q1, Q2, Q3, Q4);
    p.setLevel(currLevel + 1);
    insertProcess(p, Q1, Q2, Q3, Q4);
}

void printQueueSize(vector<Process> &Q1, vector<Process> &Q2, vector<Process> &Q3, vector<Process> &Q4){
    cout << "( " << Q1.size() << ", " << Q2.size() << ", " << Q3.size() << ", " << Q4.size() << " )" << endl;
}

void startNextProcess(bool &isProcessRunning, vector<Process> &Q1, vector<Process> &Q2, vector<Process> &Q3, vector<Process> &Q4, 
    vector<Process> &processList, int &t){
    if(debug) cout << "No process is running\n";
    int runningID = -1;
    if(Q4.size() > 0){
        int workIdx = 0;
        if(Q4.size() > 1 && Q4[0].justIn()){
            workIdx ++;
        }
        Q4[workIdx].setRunningTime(t);
        runningID = Q4[workIdx].getID();
        isProcessRunning = true;
    }
    else if(Q3.size() > 0){
        int minID = Q3[0].getID();
        int minTime = Q3[0].getRemainingTime();
        for(auto &p: Q3){
            if(p.getRemainingTime() < minTime){
                minTime = p.getRemainingTime();
                minID = p.getID();
            }
        }
        for(auto &p: Q3){
            if(p.getID() == minID){
                p.setRunningTime(t);
                runningID = p.getID();
                isProcessRunning = true;
            }
        }
    }
    else if(Q2.size() > 0){
        int minID = Q2[0].getID();
        int minTime = Q2[0].getRemainingTime();
        for(auto &p: Q2){
            if(p.getRemainingTime() < minTime){
                minTime = p.getRemainingTime();
                minID = p.getID();
            }
        }
        for(auto &p: Q2){
            if(p.getID() == minID){
                p.setRunningTime(t);
                runningID = p.getID();
                isProcessRunning = true;
            }
        }
    }
    else if(Q1.size() > 0){
        Q1[0].setRunningTime(t);
        runningID = Q1[0].getID();
        isProcessRunning = true;
    }
    if(debug) cout << "The running ID is " << runningID << endl;
    if(debug) printQueueSize(Q1, Q2, Q3, Q4);
    for(auto &p: processList){
        if(p.getID() == runningID){
            p.setRunningTime(t); // updating in table
            if(debug) p.runningMessage();
        }
    }
}

void printFinalMessage(float totalTAT, int numProcess, int timeOfCompletion, ofstream &fout){
    float meanTAT = (totalTAT) / numProcess;
    float throughput = (1.0 * 1000 * numProcess) / (1.0 * timeOfCompletion);
    fout << "Mean Turnaround time: " << fixed << setprecision(2) << meanTAT << " (ms); Throughput: " << fixed << setprecision(2) << throughput << " processes/sec" << endl;
}

void checkOrdering(vector<Process> &Q4){
    int numProcess = Q4.size();
    vector<Process> tmpQ;
    for(int i = 0; i < numProcess; i++){
        if(Q4[i].justIn()) {
            if(debug) {cout << "JustIn : "; Q4[i].printCheck();}
            tmpQ.push_back(Q4[i]);
        }
    }
    for(auto &p: tmpQ) removeProcess(p, Q4, Q4, Q4, Q4);
    sort(tmpQ.begin(), tmpQ.end(), sortByID);
    for(auto &p: tmpQ) insertProcess(p, Q4, Q4, Q4, Q4);
}

int main(){
    int quantumTime, thresholdTime;
    string inputFileName, outputFileName;
    cin >> quantumTime >> thresholdTime;
    cin >> inputFileName >> outputFileName;
    ifstream inputFile;
    inputFile.open(inputFileName);
    ofstream outFile(outputFileName);
    string line;
    vector<Process> processList;
    while(getline(inputFile, line)){
        vector<string> inputLine= splitLines(line);
        if(inputLine.size() < 4) continue;
        int ID = stoi(inputLine[0]);
        int initialQueue = stoi(inputLine[1]);
        int arrivalTime = stoi(inputLine[2]);
        int burstTime = stoi(inputLine[3]);
        Process p(ID, arrivalTime, initialQueue, burstTime);
        processList.push_back(p);
    }

    if(debug) printProcessList(processList);
    sort(processList.begin(), processList.end(), sortByArrival);
    if(debug) printProcessList(processList);

    int finishedProcess = 0;
    int numProcess = processList.size();
    vector<Process> Q1, Q2, Q3, Q4;
    int t = 0;
    float totalTAT = 0;
    while(finishedProcess < numProcess){
        // Insert the current processes
        bool isProcessRunning = false;
        for(auto &p: processList){
            if(p.isRunning()) isProcessRunning = true;
            if(p.isScheduled()) continue;
            if(p.getArrivalTime() == t){
                int level = p.getLevel();
                p.setScheduleTime(t);
                p.markInserted();
                insertProcess(p, Q1, Q2, Q3, Q4);
                if(debug) p.insertMessage();
            }
        }
        // Start the new process (if not there)
        if(!isProcessRunning){
            startNextProcess(isProcessRunning, Q1, Q2, Q3, Q4, processList, t);
        }

        t++;

        if(debug && (t%10 == 0)){
            cout << t << " -- \n";
            printProcessList(Q1);
            printProcessList(Q2);
            printProcessList(Q3);
            printProcessList(Q4);
        }

        // Remove the finished Process and check Round Robin
        if(isProcessRunning){
            for(auto &p: processList){
                if(p.isRunning()){
                    int currRemainingTime = p.getRemainingTime();
                    currRemainingTime --;
                    p.setRemainingTime(currRemainingTime);
                    if(currRemainingTime == 0){
                        p.setRunningTime(-1); // resetting its running value
                        p.setFinishTime(t);
                        finishedProcess ++;
                        removeProcess(p, Q1, Q2, Q3, Q4); // removing from queue
                        if(debug) p.finishedMessage();
                        if(final) p.finishProcessFinalMessage(outFile);
                        totalTAT += (t - p.getArrivalTime());
                    }
                    else if(p.getLevel() == 4){
                        int runningSince = t - p.getStartOfRunTime();
                        if(runningSince >= quantumTime){
                            p.setRunningTime(-1); // unset running time
                            isProcessRunning = false; // unschedule it
                            removeProcess(p, Q1, Q2, Q3, Q4); // remove from front
                            p.markInserted();
                            Q4.push_back(p); // at the back
                            if(debug) p.switchMessage();
                        }
                    }
                }
            }
        }
        

        // Increment the levels of the process (if to be done)
        for(auto &p: processList){
            if(p.isFinished()) continue;
            if(p.isRunning()) {
                p.setTimeSinceLastRun(0);
                continue;
            }
            if(!p.isScheduled()) continue;
            int lastRunTime = p.getTimeSinceLastRun() + 1;
            p.incTimeSinceLastRun();
            p.doneInserted();
            if(p.getLevel() == 4) continue;
            if(lastRunTime >= thresholdTime){
                p.setTimeSinceLastRun(0);
                p.markInserted();
                updateLevelOfProcess(p, Q1, Q2, Q3, Q4);
                if(debug) p.updateLevelMessage();
            }
        }

        checkOrdering(Q4);

        for(auto &p: Q4) p.doneInserted();
        for(auto &p: Q3) p.doneInserted();
        for(auto &p: Q2) p.doneInserted();
        for(auto &p: Q1) p.doneInserted();

    }
    if(debug) cout << numProcess << endl;
    printFinalMessage(totalTAT, numProcess, t, outFile);
}