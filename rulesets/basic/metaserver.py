#!/usr/bin/python

import socket
import struct

SKEEP_ALIVE = socket.htonl(1)
CKEEP_ALIVE = socket.htonl(2)
HANDSHAKE = socket.htonl(3)
SERVERSHAKE = socket.htonl(4)
CLIENTSHAKE = socket.htonl(5)
TERMINATE = socket.htonl(6)
LIST_REQ = socket.htonl(7)
LIST_RESP = socket.htonl(8)
PROTO_ERANGE = socket.htonl(9)

def metaquery():
    ip = "80.68.90.68"
    port = 8453

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


    s.sendto(struct.pack('I', CKEEP_ALIVE), (ip, port))

    data,addr = s.recvfrom(8)

    res = struct.unpack('II', data)
    if res[0] != HANDSHAKE:
        print socket.ntohl(res[0]), HANDSHAKE
        print "no handshake"
        return

    shake = res[1]

    s.sendto(struct.pack('II', CLIENTSHAKE, shake), (ip, port))

    s.sendto(struct.pack('II', LIST_REQ, socket.htonl(0)), (ip, port))


metaquery()
