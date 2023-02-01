# Simple TCP client
# Author: Carmelo Ballone

# This code is taken from the book
# "Black Hat Python: Python Programming for Hackers and Pentesters"
# by Justin Seitz and Tim Arnold.
# All rights reserved.

import socket

target_host = 'localhost'
target_port = 4444

# create socket object
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# connect to the server
client.connect((target_host, target_port))

# send data
client.send(b'ABCDE')

# receive response from server
response = client.recv(4096)

# decode and print on std out the response 
print(response.decode())

# close the socket
client.close()