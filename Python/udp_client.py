# Simple UDP client
# Author: Carmelo Ballone


import socket

target_host = '127.0.0.1'
target_port = 4444

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Since UDP is a connectionless protocol, we don't need to connect
client.sendto(b'ABCDEF', (target_host, target_port))

data, addr = client.recvfrom(4096)

print(data.decode())
client.close()