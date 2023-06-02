 #include "ultrasonic.h"
#include "ThisThread.h"
#include <assert.h>

using namespace std;

#define timer_read_f(x)     chrono::duration<float>((x).elapsed_time()).count()
#define timer_read_s(x)     chrono::duration_cast<chrono::seconds>((x).elapsed_time()).count();
#define timer_read_ms(x)    chrono::duration_cast<chrono::milliseconds>((x).elapsed_time()).count()
#define timer_read_us(x)    (x).elapsed_time().count()

#define _updateSpeed 100ms
#define _timeout 100ms

int counter = 0;
    
    ultrasonic::ultrasonic(PinName trigPin, PinName echoPin):_trig(trigPin), _echo(echoPin)
    {
    }
    
    ultrasonic::ultrasonic(string name, PinName trigPin, PinName echoPin, void onUpdate(string, int, int, int))
    : _name(name), _trig(trigPin), _echo(echoPin)
    {
        _onUpdateMethod=onUpdate;
        _t.start ();
    }
    void ultrasonic::_startT()
    { 
        if(timer_read_us(_t)>1000000)
        {
            _t.reset ();
        }
        start = timer_read_us(_t);
    }
        
    void ultrasonic::_updateDist()
    {
        counter++;
        end = timer_read_us(_t);
        done = 1;
        _distance = (end - start)/6;
        queue.push(_distance);
    }
    void ultrasonic::_startTrig(void)
    {
            _tout.detach();
            _trig=1;             
            wait_us(10);    
            _echo.rise(callback(this, &ultrasonic::_startT));   
            _echo.fall(callback(this, &ultrasonic::_updateDist));
            _trig = 0;
            _echo.enable_irq();
            _tout.attach(callback(this, &ultrasonic::_startTrig) ,_timeout);
    }
    
    int ultrasonic::getCurrentDistance(void)
    {
        return _distance;
    }
    void ultrasonic::pauseUpdates(void)
    {
        _tout.detach();
        _echo.rise(NULL);
        _echo.fall(NULL);
    }
    void ultrasonic::startUpdates(void)
    {
        _startTrig();
    }
    void ultrasonic::attachOnUpdate(void method(string, int, int, int))
    {
        _onUpdateMethod = method;
    }
    int ultrasonic::isUpdated(void)
    {
        d = done;
        return d;
    }
    void ultrasonic::checkDistance(void)
    {
        if(isUpdated())
        {   
            clearDone();
            // queue.push(_distance);   
            (*_onUpdateMethod)(_name, start, end, _distance);
            // if(queue.ready()){
            //     auto a = getChanges();
            //     for(int i=0;i<queue.window;i++){
            //         printf("%d ", a[i]);
            //     }
            //     printf("\n");
            // }
        }
    }

    void ultrasonic::reset() {
        queue.reset();
    }
