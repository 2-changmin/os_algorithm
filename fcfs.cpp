#include <iostream>
#include <vector>
#include <algorithm>
using namespace std; 

struct Process {
    int pid;
    int arrival;
    int burst;
};

int main() {
    int n;
    cin >> n;
    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        cin >> processes[i].pid >> processes[i].arrival >> processes[i].burst;
    }
    // Sort by arrival time, then by pid
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrival != b.arrival) return a.arrival < b.arrival;
        return a.pid < b.pid;
    });
    long long total_wait = 0;
    int current_time = 0;
    for (const auto& p : processes) {
        if (current_time < p.arrival) {
            current_time = p.arrival;
        }
        int wait = current_time - p.arrival;
        total_wait += wait;
        current_time += p.burst;
    }
    cout << total_wait << endl;
    return 0;
}