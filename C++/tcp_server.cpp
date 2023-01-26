/*
 * Simple TCP server with exception handling.
 *
 * Usage:
 * $ g++ -Wall -Wextra [std=c++11] tcp_server.cpp -o tcp_server
 * $ ./tcp_server
 * 
 * Author: Carmelo Ballone <carmelo.ballone@outlook.com>
 * Date: 01/2023
*/

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <pthread.h>

#define IP "127.0.0.1"
#define PORT 4444

using std::cout;
using std::cerr;

void* client_handler(void *);

int main() {
    int server_sock, client_sock;

    // socket server file descriptor creation
    try {
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0) {
            throw -1;
        }
    } catch(int e){
        cerr << "[-] An error in socket fd creation occurred.\n";
        cerr << "Exit status: " << e << '\n';
        return -1;
    } 

    sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // binding port 
    try {
        if (bind(server_sock, (sockaddr*) &server_addr,
            sizeof(server_addr)))
            throw -1;
    } catch(int e) {
        if (close(server_sock) < 0)         // close socket fd to avoid resource leakage
            cerr << "[x] Error while closing socket fd.\n";
        cerr << "[-] An error in server socket binding occurred.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    } 

    // server listening for connections
    try {
        if (listen(server_sock, 5) < 0)
            throw -1; 
    } catch(int e) {
        if (close(server_sock) < 0) 
            cerr << "[x] Error while closing server socket fd.\n";
        cerr << "[-] An error while listening for new connections occurred.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    }

    cout << "[*] Listening on " << IP << ':' << PORT << '\n';

    do {
        try {
            int c = sizeof(sockaddr_in);
            client_sock = accept(server_sock, (sockaddr *) &client_addr, (socklen_t *) &c);
            if (client_sock < 0)
                throw -1;

            char ip[INET_ADDRSTRLEN];
            
            cout << "[*] Accepted connection from " 
                 << inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN) << ':' 
                 << htons(client_addr.sin_port) << '\n';
            
            // create the thread handling clients
            pthread_t tid;              // thread ID

            int err_code = pthread_create(&tid, nullptr, client_handler, &client_sock);
            if (err_code != 0)
                throw err_code;
            
            err_code = pthread_join(tid, nullptr);
            if (err_code != 0)
                throw err_code;

        } catch(int e){
            if (close(client_sock) < 0) 
                cerr << "[x] Error while closing client socket fd.\n";
            if (close(server_sock) < 0) 
                cerr << "[x] Error while closing server socket fd.\n";
            cerr << "[-] An error while accepting connection occurred.\n";
            cerr << "Exit status: " << e << '\n';
            return e;
        } catch(...){
                if (close(client_sock) < 0) 
                    cerr << "[x] Error while closing client socket fd.\n";
                if (close(server_sock) < 0) 
                    cerr << "[x] Error while closing server socket fd.\n";
                cerr << "[-] An unspecified error occured.\n";
                return -1;
        }

    } while(true);

    try {
        if (close(server_sock) < 0)
            throw -1;
    } catch(int e) {
        cerr << "[x] Error while closing socket fd.\n";
        cerr << "Exit status: " << e << '\n';
        return e;
    }

    return 0;

}

void* client_handler(void* client_sock) {
    const char reply[] = "ACK";
    char recv_buf[1024];
    int client_socket = *(int*)client_sock;
    try {
        // receive data from client
        if (recv(client_socket, recv_buf, 1024, 0) < 0)
            throw -2;

        // print client data
        cout << recv_buf << '\n';

        // send response to client
        if (send(client_socket, reply, 1024, 0) < 0)
            throw -3;

        // close the client socket file descriptor
        if (close(client_socket) < 0)
            throw -4;
            
    } catch(int e) {
        cerr << "[x] An error in client_handler function occurred.\n";
        cerr << "Error code: " << e << '\n';
        // re-throw the exception to the caller
        throw;
    }

    pthread_exit(0);

}