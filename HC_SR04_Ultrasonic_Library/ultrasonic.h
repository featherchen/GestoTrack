#ifndef MBED_ULTRASONIC_H
#define MBED_ULTRASONIC_H

#include "mbed.h"
#include <array>
#include <vector>
#include <string>
#include "gesture.h"
#include "MyQueue.h"

constexpr int sonicWindow = 8;
const int threshold = 50;
const int limit = 400;
class ultrasonic
{
    public:
        MyQueue<int, sonicWindow << 2, sonicWindow> queue; ultrasonic(PinName trigPin, PinName echoPin);
        /**iniates the class with the specified trigger pin, echo pin, update speed, timeout and method to call when the distance changes**/
        ultrasonic(std::string name, PinName trigPin, PinName echoPin, void onUpdate(std::string, int, int, int));
        /** returns the last measured distance**/
        int getCurrentDistance(void);
        /**pauses measuring the distance**/
        void pauseUpdates(void);
        /**starts mesuring the distance**/
        void startUpdates(void);
        /**attachs the method to be called when the distances changes**/
        void attachOnUpdate(void method(std::string, int, int, int));
        /**gets whether the distance has been changed since the last call of isUpdated() or checkDistance()**/
        int isUpdated(void);
        /**call this as often as possible in your code, eg. at the end of a while(1) loop,
        and it will check whether the method you have attached needs to be called**/
        void checkDistance(void);

        void clearDone(void){
            done = 0;
        }

        std::vector<int> getChanges(){
            vector<int> arr(queue.size());
            for(int i=0;i<queue.size();i++){
                if(i==0 || queue[i]>limit){
                    arr[i] = 0;
                } else if(arr[i-1]>=0 && queue[i-1]-queue[i] >= threshold){
                    arr[i] = -1;
                } else if(arr[i-1]<=0 && queue[i]-queue[i-1] >= threshold){
                    arr[i] = 1;
                } else{
                    arr[i] = 0;
                }
            }
            return arr;
        }
        void reset();
    private:

        DigitalOut _trig;
        InterruptIn _echo;
        Timer _t;
        Timeout _tout;
        int _distance;
        int start;
        int end;
        volatile int done;
        void (*_onUpdateMethod)(std::string, int, int, int);
        void _startT(void);
        void _updateDist(void);
        void _startTrig(void);
        int d;
        std::string _name;
};
#endif