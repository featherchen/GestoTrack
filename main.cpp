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

            // if (state == 3) {
            //     if (getUsTime() - lastTime > resetTime) {
            //         state = 0;
            //         lastTime = getUsTime();
            //         printf("reset\n");
            //         sonic1.reset();
            //         sonic2.reset();
            //     }
            // } else if (getUsTime() - lastTime > idleTime) {
            //     state = 0;
            //     lastTime = getUsTime();
            //     printf("idle\n");
            // } else if (state == 0) {
            //     if (!peak.empty()) {
            //         state = peak.back();
            //         lastTime = getUsTime();
            //         printf("appeared at %d\n", state);
            //     }
            // } else if (state == 1) {
            //     if (!peak.empty() && peak.back() == 2) {
            //         printf("1 to 2\n");
            //         state = 3;
            //         lastTime = getUsTime();
            //     }
            // } else if (state == 2) {
            //     if (!peak.empty() && peak.back() == 1) {
            //         printf("2 to 1\n");
            //         state = 3;
            //         lastTime = getUsTime();
            //     }
            // }

           if (state == 3) {
                if (getUsTime() - lastTime > resetTime) {
                    state = 0;
                    lastTime = getUsTime();
                    printf("reset\n");
                    sonic1.reset();
                    sonic2.reset();
                }
            } else if (getUsTime() - lastTime > idleTime) {
                if(state == 12){
                    printf("1 to 2 \n");
                } else if(state == 21){
                    printf("2 to 1\n");
                }
                state = 0;
                lastTime = getUsTime();
                printf("idle\n");
            } else if (state == 0) {
                if (!peak.empty()) {
                    state = peak.back();
                    lastTime = getUsTime();
                    printf("appeared at %d\n", state);
                }
            } else if (state == 1 || state ==21) {
                if (!peak.empty() && peak.back() == 2 ) {
                    if(state == 1){
                        // printf("1 to 2\n");
                        state = 12;
                        lastTime = getUsTime();
                    } else {
                        state = 3;
                        lastTime = getUsTime();
                        printf("2 to 1 to 2\n");
                    }
                } 
            } else if (state == 2 || state == 12) {
                if (!peak.empty() && peak.back() == 1) {
                    // printf("2 to 1\n");
                    if(state == 2){
                        state = 21;
                        lastTime = getUsTime();
                    } else {
                        state = 3;
                        lastTime = getUsTime();
                        printf("1 to 2 to 1\n");
                    }
                }
            }
        }

        if (getUsTime() - lastPrint > 200000) {
            printf("state: %d\n", state);
            lastPrint = getUsTime();
        }
    }
}

// #include "mbed.h"
// // #include "ISM43362Interface.h"
// // #include "HCSR04.h"
// #include "VL53L0X.h"
// #include <cstdint>
// #include <cstdio>

// static DevI2C devI2c(PB_11,PB_10);
// static DigitalOut shutdown_pin(PC_6);
// static VL53L0X laser(&devI2c, &shutdown_pin, PC_7);

// // ISM43362Interface wifi(false);
// // InterruptIn button(BUTTON1);
// // DigitalOut socket_led(LED1);
// DigitalOut car_led(LED2);
// DigitalOut obs_led(LED3);
// Timer sonic_timer;
// // HCSR04 sonic(D2, D4); // (echo, trigger)
// EventQueue queue(32*EVENTS_EVENT_SIZE);
// // Thread button_thread;
// TCPSocket socket;
// SocketAddress addr("192.168.0.15",9000);
// nsapi_error_t response;

// bool obsExistence, carExistence;
// float obsDistance;
// bool pause = 0;

// void sensor_init() {
//     laser.init_sensor(VL53L0X_DEFAULT_ADDRESS);
//     // sonic.setRanges(2, 70);
//     // printf("Obstacle detecting range: %g ~ %g cm\n\n", sonic.getMinRange(), sonic.getMaxRange());
// }

// void get_car_existence() {
//     uint32_t dist1, dist2, difference;
//     laser.get_distance(&dist1);
//     wait_us(25000);
//     laser.get_distance(&dist2);
//     if (dist1 > dist2) {
//         difference = dist1 - dist2;
//     }
//     else {
//         difference = dist2 - dist1;
//     }
//     if (difference>=100) {
//         carExistence = true;
//         car_led = 1;
//     }
//     else {
//         carExistence = false;
//         car_led = 0;
//     }
//     printf("distance %d\n", dist1);
//     printf("distance %d\n", dist2);
// }

// void get_obs_existence() {
//     if (obsDistance>=450) {
//         obsExistence = false;
//         obs_led = 0;
//     }
//     else {
//         obsExistence = true;
//         obs_led = 1;
//     }
// }

// void button_pressed()
// {
//     pause = !pause;
//     // carExistence = !carExistence;
//     // car_led = !car_led;
//     // obs_led = !obs_led;
//     // obsExistence = !obsExistence;
//     // printf("\nObstacle: %d\n", obsExistence);
// }

// void get_obs_distance() {
//     sonic_timer.reset();
//     sonic_timer.start();
//     // sonic.startMeasurement();
//     // while(!sonic.isNewDataReady()) {
//     //     // wait for new data
//     // }
//     // obsDistance = sonic.getDistance_mm();
//     sonic_timer.stop();
//     // wait_us(500000 - sonic_timer.elapsed_time().count());
// }

// void wifi_connect() {
//     // printf("Connecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
//     // int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
//     // if (ret != 0) {
//     //     printf("Connection error\n");
//     // }

//     // printf("Success\n\n");
//     // printf("MAC: %s\n", wifi.get_mac_address());
//     // printf("IP: %s\n", wifi.get_ip_address());
//     // printf("Netmask: %s\n", wifi.get_netmask());
//     // printf("Gateway: %s\n", wifi.get_gateway());
//     // printf("RSSI: %d\n\n", wifi.get_rssi());
// }

// int socket_test(NetworkInterface *net) {
//     response = socket.open(net);
//     if (0 != response){
//         printf("Error opening: %d\n", response);
//         return 0;
//     }
//     response = socket.connect(addr);
//     if (0 != response){
//         printf("Error connecting: %d\n", response);
//         return 0;
//     }
//     return 1;
// }

// void send_road_state(NetworkInterface *net) {    
//     // socket_led = 1;
//     char obs_info[64];

//     get_obs_distance();
//     get_obs_existence();

//     int len = sprintf(obs_info,
//         "{\"car\":%d,\"obs\":%d,\"dist\":%3.1f}",
//         carExistence, obsExistence, obsDistance);
//     response = socket.send(obs_info,len);
//     if (response > 0) {
//         printf("sent %s\n",obs_info);
//     }
//     else {
//         printf("Error sending: %d\n", response);
//     }
//     // socket_led = 0;
//     // wait_us(2000000);
// }

// int main()
// {
//     // wifi_connect();
//     // bool isServerAvailable = socket_test(&wifi);
//     // if (!isServerAvailable)
//     //     return -1;

//     // button_thread.start(callback(&queue,&EventQueue::dispatch_forever));
//     // button.fall(queue.event(&button_pressed));

//     printf("\nSensor init...\n");
//     sensor_init();
//     printf("\nSensor init completed\n");
//     bool lastCarExistence = carExistence;
//     // send_road_state(&wifi);

//     while (1) {
//         if (!pause){
//             get_car_existence();
//             if (lastCarExistence != carExistence) {
//                 // send_road_state(&wifi);
//             }
//             if (carExistence) {
//                 ThisThread::sleep_for(5s);
//             }
//             lastCarExistence = carExistence;
//         }
//     }
// }

/*
 * Copyright (c) 2014-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

// #include "mbed.h"

// // Read temperature from LM75BD

// I2C i2c(I2C_SDA, I2C_SCL);

// const int addr7bit = 0x48;      // 7 bit I2C address
// const int addr8bit = 0x48 << 1; // 8bit I2C address, 0x90

// int main()
// {
//     char cmd[2];
//     while (1) {
//         cmd[0] = 0x01;
//         cmd[1] = 0x00;
//         i2c.write(addr8bit, cmd, 2);

//         ThisThread::sleep_for(500);

//         cmd[0] = 0x00;
//         i2c.write(addr8bit, cmd, 1);
//         i2c.read(addr8bit, cmd, 2);

//         float tmp = (float((cmd[0] << 8) | cmd[1]) / 256.0);
//         printf("Temp = %.2f\n", tmp);
//     }
// }