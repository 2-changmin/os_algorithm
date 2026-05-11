#include <iostream>
#include <vector>

using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int remaining;
};

int main() {
    int n;
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        cin >> p[i].pid >> p[i].arrival >> p[i].burst;
        p[i].remaining = p[i].burst;
    }

    vector<int> completion(n, -1);
    int current_time = 0;
    int finished = 0;

    while (finished < n) {
        int pick = -1;
        for (int i = 0; i < n; i++) {
            if (p[i].remaining == 0 || p[i].arrival > current_time) {
                continue;
            }

            if (pick == -1) {
                pick = i;
            } else if (p[i].remaining < p[pick].remaining) {
                pick = i;
            } else if (p[i].remaining == p[pick].remaining) {
                if (p[i].arrival < p[pick].arrival) {
                    pick = i;
                } else if (p[i].arrival == p[pick].arrival && p[i].pid < p[pick].pid) {
                    pick = i;
                }
            }
        }

        if (pick == -1) {
            current_time++;
            continue;
        }

        p[pick].remaining--;
        current_time++;
        if (p[pick].remaining == 0) {
            completion[pick] = current_time;
            finished++;
        }
    }

    long long total_wait = 0;
    for (int i = 0; i < n; i++) {
        int waiting = completion[i] - p[i].arrival - p[i].burst;
        total_wait += waiting;
    }

    cout << total_wait << "\n";
    return 0;
}
