#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>

using namespace std;

struct PendingRequest {
    int pid;
    vector<int> req;
};

static void printAvailable(const vector<int>& available) {
    for (int j = 0; j < (int)available.size(); ++j) {
        if (j) cout << ' ';
        cout << available[j];
    }
    cout << '\n';
}

static bool isSafeState(
    const vector<vector<int>>& allocation,
    const vector<vector<int>>& need,
    const vector<int>& available,
    int n,
    int m
) {
    vector<int> work = available;
    vector<bool> finish(n, false);

    bool progress = true;
    while (progress) {
        progress = false;

        for (int i = 0; i < n; ++i) {
            if (finish[i]) continue;

            bool canFinish = true;
            for (int j = 0; j < m; ++j) {
                if (need[i][j] > work[j]) {
                    canFinish = false;
                    break;
                }
            }

            if (canFinish) {
                for (int j = 0; j < m; ++j) {
                    work[j] += allocation[i][j];
                }
                finish[i] = true;
                progress = true;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (!finish[i]) return false;
    }
    return true;
}

static bool tryGrantRequest(
    int pid,
    const vector<int>& req,
    vector<vector<int>>& allocation,
    vector<vector<int>>& need,
    vector<int>& available,
    int n,
    int m
) {
    for (int j = 0; j < m; ++j) {
        if (req[j] > available[j]) return false;
    }

    for (int j = 0; j < m; ++j) {
        available[j] -= req[j];
        allocation[pid][j] += req[j];
        need[pid][j] -= req[j];
    }

    if (isSafeState(allocation, need, available, n, m)) {
        return true;
    }

    for (int j = 0; j < m; ++j) {
        available[j] += req[j];
        allocation[pid][j] -= req[j];
        need[pid][j] += req[j];
    }
    return false;
}

static void processWaitingQueue(
    deque<PendingRequest>& waitQ,
    vector<vector<int>>& allocation,
    vector<vector<int>>& need,
    vector<int>& available,
    int n,
    int m
) {
    deque<PendingRequest> nextQ;

    
    while (!waitQ.empty()) {
        PendingRequest cur = waitQ.front();
        waitQ.pop_front();

        
        bool validNow = true;
        for (int j = 0; j < m; ++j) {
            if (cur.req[j] > need[cur.pid][j]) {
                validNow = false;
                break;
            }
        }
        if (!validNow) continue;

        if (!tryGrantRequest(cur.pid, cur.req, allocation, need, available, n, m)) {
            nextQ.push_back(cur);
        }
    }

    waitQ = nextQ;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<int> total(m);
    for (int j = 0; j < m; ++j) cin >> total[j];

    vector<vector<int>> maxNeed(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cin >> maxNeed[i][j];
        }
    }

    vector<vector<int>> allocation(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cin >> allocation[i][j];
        }
    }

    vector<vector<int>> need(n, vector<int>(m));
    vector<int> available = total;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            need[i][j] = maxNeed[i][j] - allocation[i][j];
            available[j] -= allocation[i][j];
        }
    }

    deque<PendingRequest> waitQ;
    string cmd;

    while (cin >> cmd) {
        if (cmd == "quit") break;

        if (cmd == "request") {
            int pid;
            cin >> pid;
            vector<int> req(m);
            for (int j = 0; j < m; ++j) cin >> req[j];

            bool valid = (pid >= 0 && pid < n);
            if (valid) {
                for (int j = 0; j < m; ++j) {
                    if (req[j] < 0 || req[j] > need[pid][j]) {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid) {
                if (!tryGrantRequest(pid, req, allocation, need, available, n, m)) {
                    waitQ.push_back({pid, req});
                }
            }

            printAvailable(available);
        } else if (cmd == "release") {
            int pid;
            cin >> pid;
            vector<int> rel(m);
            for (int j = 0; j < m; ++j) cin >> rel[j];

            bool valid = (pid >= 0 && pid < n);
            if (valid) {
                for (int j = 0; j < m; ++j) {
                    if (rel[j] < 0) {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid) {
                for (int j = 0; j < m; ++j) {
                    int giveBack = min(rel[j], allocation[pid][j]);
                    allocation[pid][j] -= giveBack;
                    need[pid][j] += giveBack;
                    available[j] += giveBack;
                }

                processWaitingQueue(waitQ, allocation, need, available, n, m);
            }

            printAvailable(available);
        }
    }

    return 0;
}
