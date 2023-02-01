# Simple UDP server
# Author: Carmelo Ballone

import socket
import threading

IP = "127.0.0.1"
PORT = 4444

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((IP, PORT))
    '''Since UDP is a connectionless protocol, we don't need to connect'''
    print(f"[*] Listening on {IP}:{PORT} ...")

    while True:
        data, client = server.recvfrom(4096)
        client_handler = threading.Thread(target=handle_client, args=(server,data,client))
        client_handler.start()

def handle_client(server,data, client):
    print(f"[*] Received from {client}: {data.decode('utf-8')}")
    server.sendto(b"ACK", client)


if __name__ == '__main__':
    main()
