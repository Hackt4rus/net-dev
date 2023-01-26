/*
 * Simple TCP client with exception handling.
 *
 * Usage:
 * $ g++ -Wall -Wextra [std=c++11] tcp_client.cpp -o tcp_client
 * $ ./tcp_client
 * 
 * Author: Carmelo Ballone <carmelo.ballone@outlook.com>
 * Date: 01/2023
*/

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TARGET_HOST "127.0.0.1"
#define TARGET_PORT 4444

using std::cout;
using std::cerr;

int main() {

    int server_sock;
    char recv_buf[1024];

    // socket creation
    try {
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0) {
            throw -1;
        }
    } catch(int e){
        cerr << "[-] Error in socket fd creation.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    } 

    // server address initialization
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TARGET_PORT);
    server_addr.sin_addr.s_addr = inet_addr(TARGET_HOST);

    // connect to server
    try {
        if (connect(server_sock, (const sockaddr*) &server_addr, sizeof(server_addr)) < 0)
            throw -1;
    } catch(int e) {  
        if (close(server_sock) < 0) // if any error occurs, close the socket file descriptor
            cerr << "[x] Error while closing socket fd.\n";
        cerr << "[-] Connection error.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    } 

    cout << "[*] Connected to server " << TARGET_HOST << ':' << TARGET_PORT << "\n\n";


    // send data
    const char message[] = "Long Live Falcone & Borsellino!\r\n";
    try {
        if (send(server_sock, message, strlen(message), 0) < 0)
            throw -1;
    } catch(int e) {
        if (close(server_sock) < 0)
            cerr << "[x] Error while closing socket fd.\n";
        cerr << "[-] Error while sending data.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    } 

    // receive server response
    try {
        if (recv(server_sock, recv_buf, 1024, 0) < 0)
            throw -1;
    } catch(int e) {
        if (close(server_sock) < 0)
            cerr << "[x] Error while closing socket fd.\n";
        cerr << "[-] Error while receiving data.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    } 

    // print server response
    cout << recv_buf << '\n';

    // close socket file descriptor
    try {
        if (close(server_sock) < 0)
            throw -2;
    } catch(int e) {
        cerr << "[x] Error while closing socket fd.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    }
    
    return 0;

}