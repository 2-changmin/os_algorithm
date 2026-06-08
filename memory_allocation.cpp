#include <iostream>
#include <list>
#include <vector>

using namespace std;

const int MEMORY_SIZE = 1000;

struct Process {
    int requestTime;
    int runTime;
    int size;
};

struct Block {
    int start;
    int pid;
    int size;
};

enum FitType {
    FIRST,
    BEST,
    WORST
};

int findHole(const vector<Block>& memory, int need, FitType type) {
    int selected = -1;

    for (int i = 0; i < (int)memory.size(); ++i) {
        if (memory[i].pid != -1 || memory[i].size < need) {
            continue;
        }

        if (type == FIRST) {
            return i;
        }

        if (selected == -1) {
            selected = i;
        } else if (type == BEST && memory[i].size < memory[selected].size) {
            selected = i;
        } else if (type == WORST && memory[i].size > memory[selected].size) {
            selected = i;
        }
    }

    return selected;
}

int allocateProcess(vector<Block>& memory, const vector<Process>& processes, int pid, FitType type) {
    int index = findHole(memory, processes[pid].size, type);
    if (index == -1) {
        return -1;
    }

    int start = memory[index].start;
    int remain = memory[index].size - processes[pid].size;

    memory[index].pid = pid;
    memory[index].size = processes[pid].size;

    if (remain > 0) {
        Block hole = {start + processes[pid].size, -1, remain};
        memory.insert(memory.begin() + index + 1, hole);
    }

    return start;
}

void mergeHoles(vector<Block>& memory) {
    vector<Block> merged;

    for (const Block& block : memory) {
        if (!merged.empty() && merged.back().pid == -1 && block.pid == -1) {
            merged.back().size += block.size;
        } else {
            merged.push_back(block);
        }
    }

    memory = merged;
}

int simulate(const vector<Process>& processes, FitType type) {
    int n = (int)processes.size();
    int targetPid = n - 1;
    int nextRequest = 0;
    int targetAddress = -1;

    vector<Block> memory;
    memory.push_back({0, -1, MEMORY_SIZE});

    vector<int> finishTime(n, -1);
    list<int> waitQueue;

    while (targetAddress == -1) {
        int currentTime = -1;

        if (nextRequest < n) {
            currentTime = processes[nextRequest].requestTime;
        }

        for (int pid = 0; pid < n; ++pid) {
            if (finishTime[pid] != -1 && (currentTime == -1 || finishTime[pid] < currentTime)) {
                currentTime = finishTime[pid];
            }
        }

        if (currentTime == -1) {
            break;
        }

        for (Block& block : memory) {
            if (block.pid != -1 && finishTime[block.pid] == currentTime) {
                int finishedPid = block.pid;
                block.pid = -1;
                finishTime[finishedPid] = -1;
            }
        }
        mergeHoles(memory);

        while (nextRequest < n && processes[nextRequest].requestTime == currentTime) {
            waitQueue.push_back(nextRequest);
            ++nextRequest;
        }

        for (auto it = waitQueue.begin(); it != waitQueue.end(); ) {
            int pid = *it;
            int address = allocateProcess(memory, processes, pid, type);

            if (address == -1) {
                ++it;
                continue;
            }

            finishTime[pid] = currentTime + processes[pid].runTime;
            if (pid == targetPid) {
                targetAddress = address;
                break;
            }

            it = waitQueue.erase(it);
        }
    }

    return targetAddress;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; ++i) {
        cin >> processes[i].requestTime >> processes[i].runTime >> processes[i].size;
    }

    cout << simulate(processes, FIRST) << '\n';
    cout << simulate(processes, BEST) << '\n';
    cout << simulate(processes, WORST) << '\n';

    return 0;
}
