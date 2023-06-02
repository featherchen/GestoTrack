#include "mbed.h"
#include "ultrasonic.h"
#include "MyQueue.h"

#include <array>
#include <string>

using namespace std;

void dist(string name, int start, int end, int distance)
{
    printf("---------------------\n");
    //put code here to happen when the distance is changed
    printf("(Name, Start, End, Dist: %s, %d, %d, %d\n", name.c_str(), start, end, distance);
}

ultrasonic sonic1("Sonic1", D8, D9, &dist);    //Set the trigger pin to D8 and the echo pin to D9
                                        //have updates every .1 seconds and a timeout after 1
                                        //second, and call dist when the distance changes

ultrasonic sonic2("Sonic2", D4, D5, &dist);

int main()
{
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
    while(!sonic2.queue.ready() || !sonic1.queue.ready()){
        wait_us(1000);
    }
    std:array<int, sonic1.queue.window> peak;
    int state = 0;
    while(true) 
    {
        //Do something else here
        if(sonic1.isUpdated() && sonic2.isUpdated()){
            sonic1.checkDistance();
            sonic2.checkDistance();
            auto a1 = sonic1.getChanges();
            auto a2 = sonic2.getChanges();
            for(int i=0;i<sonic1.queue.window;i++){
                printf("%d ", a1[i]);
            }
            printf("\n");
            for(int i=0;i<sonic2.queue.window;i++){
                printf("%d ", a2[i]);
            }
            printf("\n");
            for(int t=0; t<sonic1.queue.window;t++){
                if(a1[t]==-1){
                    peak[t] = 1;
                } else if(a2[t]==-1) {
                    peak[t] = 2;               
                } else{
                    peak[t] = 0;
                }
                printf("%d ", peak[t]);
            }
            printf("\n");
            
            // for(int i=0;i<sonic2.queue.window;i++){
                if(peak.back()==1 && state == 2){
                    printf("2 to 1\n");
                    state = 1;
                } else if(peak.back()==2 && state == 1){
                    state = 2;
                    printf("1 to 2\n");
                } else if(state==0){
                    state = peak.back();
                }
                // printf("%d ", peak[i]);
                // state = peak.back();
            // }
            // printf("\n");
        }
        
        // sonic1.checkDistance();     //call checkDistance() as much as possible, as this is where
                                //the class checks if dist needs to be called.

        // sonic2.checkDistance();
    }
}

     