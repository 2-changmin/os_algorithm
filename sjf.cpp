#include <iostream>
#include <vector>

using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    bool done;
};

int main() {
    int n;
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        cin >> p[i].pid >> p[i].arrival >> p[i].burst;
        p[i].done = false;
    }

    long long total_wait = 0;
    int current_time = 0;
    int finished = 0;

    while (finished < n) {
        int pick = -1;

        // 현재 시각까지 도착한 프로세스 중 실행할 대상을 고른다.
        for (int i = 0; i < n; i++) {
            if (p[i].done || p[i].arrival > current_time) {
                continue;
            }

            if (pick == -1) {
                pick = i;
            } else if (p[i].burst < p[pick].burst) {
                pick = i;
            } else if (p[i].burst == p[pick].burst) {
                if (p[i].arrival < p[pick].arrival) {
                    pick = i;
                } else if (p[i].arrival == p[pick].arrival && p[i].pid < p[pick].pid) {
                    pick = i;
                }
            }
        }

        // 현재 실행 가능한 프로세스가 없으면, 가장 빠른 도착 시각으로 점프한다.
        if (pick == -1) {
            int next_arrival = -1;
            for (int i = 0; i < n; i++) {
                if (p[i].done) {
                    continue;
                }
                if (next_arrival == -1 || p[i].arrival < next_arrival) {
                    next_arrival = p[i].arrival;
                }
            }
            current_time = next_arrival;
            continue;
        }

        int wait = current_time - p[pick].arrival;
        total_wait += wait;
        current_time += p[pick].burst;
        p[pick].done = true;
        finished++;
    }

    cout << total_wait << "\n";
    return 0;
}
