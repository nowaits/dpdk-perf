# coding=utf-8

import socket, time, random, string

def rstring(length):
    return ''.join(random.choice(string.ascii_letters) for i in range(length))

s=socket.socket()
s.connect(('192.168.126.128', 5000))

try:
	while True:
		#w = rstring(50)
		#s.send(w.encode())
		#print("Sended: ", w)

		print(s.recv(1024))
		break
		time.sleep(0.5)
finally:
    s.close()