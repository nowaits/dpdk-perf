import socket, time, string, random

def rstring(length):
    return ''.join(random.choice(string.ascii_letters) for i in range(length))

address = ('192.168.100.128', 6000)

def test():
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	while True:
	    w = rstring(1000)
	    s.sendto(w.encode(), address)
	    #print("-->>: ", w, s.recv(1024))
	    #time.sleep(0.1)
	    break
	s.close()

while True:
	test()