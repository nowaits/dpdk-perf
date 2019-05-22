# coding=utf-8

import socket

s = socket.socket()
s.bind(('0.0.0.0',5000))
s.listen(5)

while True:
    cs, address = s.accept()
    print('===>>>', address)
    try:
        while True:
            data = cs.recv(1024)
            if not data:
                break
            cs.send(data)
    except Exception as e:
    	pass
    print('<<<===', address)
    cs.close()