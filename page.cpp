#include <iostream>
#include <limits>
#include <string>
#include <vector>
using namespace std;

struct Reference {
    int pid;
    int page;
};

struct Frame {
    int pid;
    int page;
    int loadedAt;
    int lastUsedAt;
};

struct Result {
    string algorithm;
    int pageFaults;
    vector<Frame> frames;
};

int findPage(const vector<Frame>& frames, const Reference& ref) {
    for (int i = 0; i < static_cast<int>(frames.size()); i++) {
        if (frames[i].pid == ref.pid && frames[i].page == ref.page) {
            return i;
        }
    }
    return -1;
}

int countProcessFrames(const vector<Frame>& frames, int pid) {
    int count = 0;
    for (const Frame& frame : frames) {
        if (frame.pid == pid) {
            count++;
        }
    }
    return count;
}

int findNextUse(const vector<Reference>& references, int currentIndex, const Frame& frame) {
    for (int i = currentIndex + 1; i < static_cast<int>(references.size()); i++) {
        if (references[i].pid == frame.pid && references[i].page == frame.page) {
            return i;
        }
    }
    return numeric_limits<int>::max();
}

int selectVictim(const vector<Frame>& frames,
                 const vector<Reference>& references,
                 int currentIndex,
                 int pid,
                 const string& algorithm) {
    int victim = -1;

    for (int i = 0; i < static_cast<int>(frames.size()); i++) {
        if (frames[i].pid != pid) {
            continue;
        }

        if (victim == -1) {
            victim = i;
            continue;
        }

        if (algorithm == "FIFO") {
            if (frames[i].loadedAt < frames[victim].loadedAt) {
                victim = i;
            }
        } else if (algorithm == "LRU") {
            if (frames[i].lastUsedAt < frames[victim].lastUsedAt) {
                victim = i;
            }
        } else {
            int nextUse = findNextUse(references, currentIndex, frames[i]);
            int victimNextUse = findNextUse(references, currentIndex, frames[victim]);

            if (nextUse > victimNextUse ||
                (nextUse == victimNextUse && frames[i].loadedAt < frames[victim].loadedAt)) {
                victim = i;
            }
        }
    }

    return victim;
}

Result simulate(const vector<Reference>& references, int frameLimit, const string& algorithm) {
    Result result = {algorithm, 0, {}};

    for (int time = 0; time < static_cast<int>(references.size()); time++) {
        const Reference& ref = references[time];
        int frameIndex = findPage(result.frames, ref);

        if (frameIndex != -1) {
            result.frames[frameIndex].lastUsedAt = time;
            continue;
        }

        result.pageFaults++;

        if (countProcessFrames(result.frames, ref.pid) < frameLimit) {
            result.frames.push_back({ref.pid, ref.page, time, time});
            continue;
        }

        int victim = selectVictim(result.frames, references, time, ref.pid, algorithm);
        result.frames[victim] = {ref.pid, ref.page, time, time};
    }

    return result;
}

void printResult(const Result& result) {
    cout << result.algorithm << ": " << result.pageFaults << '\n';

    for (int i = 0; i < static_cast<int>(result.frames.size()); i++) {
        cout << i << ' ' << result.frames[i].pid << ' ' << result.frames[i].page << '\n';
    }
}

int main() {
    int frameLimit;
    cin >> frameLimit;

    vector<Reference> references;
    while (true) {
        Reference ref;
        cin >> ref.pid >> ref.page;

        if (ref.pid == -1 && ref.page == -1) {
            break;
        }

        references.push_back(ref);
    }

    vector<string> algorithms = {"FIFO", "LRU", "OPT"};
    for (int i = 0; i < static_cast<int>(algorithms.size()); i++) {
        Result result = simulate(references, frameLimit, algorithms[i]);
        printResult(result);
    }

    return 0;
}
