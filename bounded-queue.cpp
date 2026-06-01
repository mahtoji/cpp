#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <atomic>
#include <chrono>

using namespace std;

#define MAXSIZE 5       // small size to easily trigger full/empty conditions
#define NUMPRODS 3
#define NUMCONSM 3
#define TOTALITEMS 20   // total items each producer will produce before stopping

vector<int> bq(MAXSIZE, -1);
int h = 0, t = 0;
mutex m;
condition_variable cv;
atomic<int> produced{0};
atomic<int> consumed{0};

auto AddToQueue = [](int x) {
    while (true) {
        // stop after producing TOTALITEMS across all producers
        if (produced >= TOTALITEMS) return;
        unique_lock<mutex> lock{m};
        cv.wait(lock, [](){ return ((t+1) % MAXSIZE) != h || produced >= TOTALITEMS; });
        if (produced >= TOTALITEMS) return;
        bq[t] = x;
        t = (t + 1) % MAXSIZE;
        produced++;
        cout << "PROD thread=" << this_thread::get_id() << " added=" << x
             << " produced=" << produced << " h=" << h << " t=" << t << "\n";
        cv.notify_all();
    }
};

auto RemoveFromQueue = [](int y) {
    while (true) {
        // stop once all produced items are consumed
        if (consumed >= TOTALITEMS) return;
        unique_lock<mutex> lock{m};
        cv.wait(lock, [](){ return h != t || consumed >= TOTALITEMS; });
        if (consumed >= TOTALITEMS) return;
        int val = bq[h];
        bq[h] = -1;
        h = (h + 1) % MAXSIZE;
        consumed++;
        cout << "CONS thread=" << this_thread::get_id() << " removed=" << val
             << " consumed=" << consumed << " h=" << h << " t=" << t << "\n";
        cv.notify_all();
    }
};

int main(int argc, char* argv[]) {
    vector<jthread> jtv;
    for (int i = 1; i <= NUMPRODS; i++)
        jtv.emplace_back(AddToQueue, i * 10);  // producers add 10, 20, 30
    for (int i = 1; i <= NUMCONSM; i++)
        jtv.emplace_back(RemoveFromQueue, i);
    // jthreads auto-join when jtv goes out of scope
    // wait for all to finish
    for (auto& jt : jtv) jt.join();
    cout << "Done. Total produced=" << produced << " consumed=" << consumed << "\n";
}
