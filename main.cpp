#include "mbed.h"
#include "ultrasonic.h"
#include "MyQueue.h"

#include <array>
#include <vector>
#include <string>
#include "wifi_helper.h"

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

/* Sockets Example
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET

int sample_num;
int16_t pDataXYZ[3];
float pfData[3];
const float Ratio = 180 / 3.141592653589;
int SCALE_MULTIPLIER = 1e6;
char acc_json[10000];
int response;

class SocketDemo {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 8080; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketDemo() : _net(NetworkInterface::get_default_instance())
    {
    }

    ~SocketDemo()
    {
        if (_net) {
            _net->disconnect();
        }
    }

    void run()
    {
        if (!_net) {
            printf("Error! No network interface found.\r\n");
            return;
        }

        /* if we're using a wifi interface run a quick scan */
        if (_net->wifiInterface()) {
            /* the scan is not required to connect and only serves to show visible access points */
            wifi_scan();

            /* in this example we use credentials configured at compile time which are used by
             * NetworkInterface::connect() but it's possible to do this at runtime by using the
             * WiFiInterface::connect() which takes these parameters as arguments */
        }

        /* connect will perform the action appropriate to the interface type to connect to the network */

        printf("Connecting to the network...\r\n");

        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }

        print_network_info();

        /* opening the socket only allocates resources */
        result = _socket.open(_net);
        if (result != 0) {
            printf("Error! _socket.open() returned: %d\r\n", result);
            return;
        }

#if MBED_CONF_APP_USE_TLS_SOCKET
        result = _socket.set_root_ca_cert(root_ca_cert);
        if (result != NSAPI_ERROR_OK) {
            printf("Error: _socket.set_root_ca_cert() returned %d\n", result);
            return;
        }
        _socket.set_hostname(MBED_CONF_APP_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

        /* now we have to find where to connect */

        SocketAddress address;

        if (!resolve_hostname(address)) {
            return;
        }

        address.set_port(REMOTE_PORT);

        /* we are connected to the network but since we're using a connection oriented
         * protocol we still need to open a connection on the socket */

        printf("Opening connection to remote port %d\r\n", REMOTE_PORT);

        result = _socket.connect(address);
        if (result != 0) {
            printf("Error! _socket.connect() returned: %d\r\n", result);
            return;
        }

        /* exchange an HTTP request and response */

        // if (!send_http_request()) {
        //     return;
        // }

        // if (!receive_http_response()) {
        //     return;
        // }

        printf("Demo concluded successfully \r\n");

        // int len = sprintf(acc_json, "1");
        // printf("%d\n", len);
        // _socket.send(acc_json, len);

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
        clk.start();

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
                        sendMessage("1");
                    } else if(state == 21){
                        printf("2 to 1\n");
                        sendMessage("0");
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
                            sendMessage("21");
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
                            sendMessage("20");
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

private:

    void sendMessage(const char *str) {
        int len = sprintf(acc_json, "%s", str);
        printf("%d\n", len);
        _socket.send(acc_json, len);
    }
    bool resolve_hostname(SocketAddress &address)
    {
        const char hostname[] = MBED_CONF_APP_HOSTNAME;

        /* get the host address */
        printf("\nResolve hostname %s\r\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
            return false;
        }

        printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

        return true;
    }

    bool send_http_request()
    {
        /* loop until whole request sent */
        const char buffer[] = "GET / HTTP/1.1\r\n"
                              "Host: ifconfig.io\r\n"
                              "Connection: close\r\n"
                              "\r\n";

        nsapi_size_t bytes_to_send = strlen(buffer);
        nsapi_size_or_error_t bytes_sent = 0;

        printf("\r\nSending message: \r\n%s", buffer);

        while (bytes_to_send) {
            bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
            if (bytes_sent < 0) {
                printf("Error! _socket.send() returned: %d\r\n", bytes_sent);
                return false;
            } else {
                printf("sent %d bytes\r\n", bytes_sent);
            }

            bytes_to_send -= bytes_sent;
        }

        printf("Complete message sent\r\n");

        return true;
    }

    bool receive_http_response()
    {
        char buffer[MAX_MESSAGE_RECEIVED_LENGTH];
        int remaining_bytes = MAX_MESSAGE_RECEIVED_LENGTH;
        int received_bytes = 0;

        /* loop until there is nothing received or we've ran out of buffer space */
        nsapi_size_or_error_t result = remaining_bytes;
        while (result > 0 && remaining_bytes > 0) {
            result = _socket.recv(buffer + received_bytes, remaining_bytes);
            if (result < 0) {
                printf("Error! _socket.recv() returned: %d\r\n", result);
                return false;
            }

            received_bytes += result;
            remaining_bytes -= result;
        }

        /* the message is likely larger but we only want the HTTP response code */

        printf("received %d bytes:\r\n%.*s\r\n\r\n", received_bytes, strstr(buffer, "\n") - buffer, buffer);

        return true;
    }

    void wifi_scan()
    {
        WiFiInterface *wifi = _net->wifiInterface();

        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
            return;
        }

        printf("%d networks available:\r\n", result);

        for (int i = 0; i < result; i++) {
            printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                   ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                   ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                   ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                   ap[i].get_rssi(), ap[i].get_channel());
        }
        printf("\r\n");
    }

    void print_network_info()
    {
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }

private:
    NetworkInterface *_net;

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

int main() {
    printf("\r\nStarting socket demo\r\n\r\n");

#ifdef MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif

    SocketDemo *example = new SocketDemo();
    MBED_ASSERT(example);
    example->run();

    return 0;
}


// int main()
// {
//     // freopen("output.txt", "w", stdout);
//     // MyQueue<int, 1 << 5, 10> q;
//     // for (int i = 0; i < 100; i++) {
//     //     q.push(i);
//     //     int n = q.size();
//     //     for (int j = 0; j < n; j++) {
//     //         printf("%d ", q[j]);
//     //     }
//     //     printf("ready ? %d\n", q.ready());
//     // }
//     clk.start();

//     // sonic1.startUpdates();//start mesuring the distance
//     sonic1.startUpdates();
//     sonic2.startUpdates();
//     printf("========================================================start\n======================================================================================\n");
//     // while(!sonic2.queue.ready() || !sonic1.queue.ready()){
//     //     wait_us(1000);
//     // }
//     // std:array<int, sonic1.queue.window> peak{};
//     // MyQueue<int, sonicWindow << 2, sonicWindow> peak;
//     std::vector<int> peak;
//     peak.reserve(sonicWindow);

//     int state = 0;
//     int lastTime;
//     int lastPrint = getUsTime();
//     while(true) 
//     {
//         //Do something else here
//         if(sonic1.isUpdated() && sonic2.isUpdated()){
//             sonic1.checkDistance();
//             sonic2.checkDistance();

//             auto a1 = sonic1.getChanges();
//             auto a2 = sonic2.getChanges();
//             peak.clear();

//             for(int t=0; t<int(a1.size());t++){
//                 if(a1[t]==-1){
//                     peak.push_back(1);
//                 } else if(a2[t]==-1) {
//                     peak.push_back(2);            
//                 }
//             }

//             // if (state == 3) {
//             //     if (getUsTime() - lastTime > resetTime) {
//             //         state = 0;
//             //         lastTime = getUsTime();
//             //         printf("reset\n");
//             //         sonic1.reset();
//             //         sonic2.reset();
//             //     }
//             // } else if (getUsTime() - lastTime > idleTime) {
//             //     state = 0;
//             //     lastTime = getUsTime();
//             //     printf("idle\n");
//             // } else if (state == 0) {
//             //     if (!peak.empty()) {
//             //         state = peak.back();
//             //         lastTime = getUsTime();
//             //         printf("appeared at %d\n", state);
//             //     }
//             // } else if (state == 1) {
//             //     if (!peak.empty() && peak.back() == 2) {
//             //         printf("1 to 2\n");
//             //         state = 3;
//             //         lastTime = getUsTime();
//             //     }
//             // } else if (state == 2) {
//             //     if (!peak.empty() && peak.back() == 1) {
//             //         printf("2 to 1\n");
//             //         state = 3;
//             //         lastTime = getUsTime();
//             //     }
//             // }

//            if (state == 3) {
//                 if (getUsTime() - lastTime > resetTime) {
//                     state = 0;
//                     lastTime = getUsTime();
//                     printf("reset\n");
//                     sonic1.reset();
//                     sonic2.reset();
//                 }
//             } else if (getUsTime() - lastTime > idleTime) {
//                 if(state == 12){
//                     printf("1 to 2 \n");
//                 } else if(state == 21){
//                     printf("2 to 1\n");
//                 }
//                 state = 0;
//                 lastTime = getUsTime();
//                 printf("idle\n");
//             } else if (state == 0) {
//                 if (!peak.empty()) {
//                     state = peak.back();
//                     lastTime = getUsTime();
//                     printf("appeared at %d\n", state);
//                 }
//             } else if (state == 1 || state ==21) {
//                 if (!peak.empty() && peak.back() == 2 ) {
//                     if(state == 1){
//                         // printf("1 to 2\n");
//                         state = 12;
//                         lastTime = getUsTime();
//                     } else {
//                         state = 3;
//                         lastTime = getUsTime();
//                         printf("2 to 1 to 2\n");
//                     }
//                 } 
//             } else if (state == 2 || state == 12) {
//                 if (!peak.empty() && peak.back() == 1) {
//                     // printf("2 to 1\n");
//                     if(state == 2){
//                         state = 21;
//                         lastTime = getUsTime();
//                     } else {
//                         state = 3;
//                         lastTime = getUsTime();
//                         printf("1 to 2 to 1\n");
//                     }
//                 }
//             }
//         }

//         if (getUsTime() - lastPrint > 200000) {
//             printf("state: %d\n", state);
//             lastPrint = getUsTime();
//         }
//     }
// }
