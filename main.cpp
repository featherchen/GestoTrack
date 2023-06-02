#include "mbed.h"
#include "ultrasonic.h"
#include "MyQueue.h"

#include <array>
#include <vector>
#include <string>

using namespace std;

void dist(string name, int start, int end, int distance)
{
    // printf("---------------------\n");
    //put code here to happen when the distance is changed
    // printf("(Name, Dist: %s, %d\n", name.c_str(), distance);
}

ultrasonic sonic1("Sonic1", D8, D9, &dist);    //Set the trigger pin to D8 and the echo pin to D9
                                        //have updates every .1 seconds and a timeout after 1
                                        //second, and call dist when the distance changes

ultrasonic sonic2("Sonic2", D4, D5, &dist);

Timer clk;

#define resetTime 200000
#define idleTime 1000000
#define getUsTime()  clk.elapsed_time().count()

int main()
{
    clk.start();
    // freopen("output.txt", "w", stdout);
    // MyQueue<int, 1 << 5, 10> q;
    // for (int i = 0; i < 100; i++) {
    //     q.push(i);
    //     int n = q.size();
    //     for (int j = 0; j < n; j++) {
    //         printf("%d ", q[j]);
    //     }
    //     printf("ready ? %d\n", q.ready());
    // }

    // sonic1.startUpdates();//start mesuring the distance
    sonic1.startUpdates();
    sonic2.startUpdates();
    printf("========================================================start\n======================================================================================\n");
    // while(!sonic2.queue.ready() || !sonic1.queue.ready()){
    //     wait_us(1000);
    // }
    // std:array<int, sonic1.queue.window> peak{};
    // MyQueue<int, sonicWindow << 2, sonicWindow> peak;
    std::vector<int> peak;
    peak.reserve(sonicWindow);

    int state = 0;
    int lastTime;
    int lastPrint = getUsTime();
    while(true) 
    {
        //Do something else here
        if(sonic1.isUpdated() && sonic2.isUpdated()){
            sonic1.checkDistance();
            sonic2.checkDistance();

            auto a1 = sonic1.getChanges();
            auto a2 = sonic2.getChanges();
            peak.clear();

            for(int t=0; t<int(a1.size());t++){
                if(a1[t]==-1){
                    peak.push_back(1);
                } else if(a2[t]==-1) {
                    peak.push_back(2);            
                }
            }

            if (state == 3) {
                if (getUsTime() - lastTime > resetTime) {
                    state = 0;
                    lastTime = getUsTime();
                    printf("reset\n");
                    sonic1.reset();
                    sonic2.reset();
                }
            } else if (getUsTime() - lastTime > idleTime) {
                state = 0;
                lastTime = getUsTime();
                printf("idle\n");
            } else if (state == 0) {
                if (!peak.empty()) {
                    state = peak.back();
                    lastTime = getUsTime();
                    printf("appeared at %d\n", state);
                }
            } else if (state == 1) {
                if (!peak.empty() && peak.back() == 2) {
                    printf("1 to 2\n");
                    state = 3;
                    lastTime = getUsTime();
                }
            } else if (state == 2) {
                if (!peak.empty() && peak.back() == 1) {
                    printf("2 to 1\n");
                    state = 3;
                    lastTime = getUsTime();
                }
            }
        }

        if (getUsTime() - lastPrint > 200000) {
            printf("state: %d\n", state);
            lastPrint = getUsTime();
        }
    }
}

     