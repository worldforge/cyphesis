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

    print "handshook"

    shake = res[1]

    s.sendto(struct.pack('II', CLIENTSHAKE, shake), (ip, port))

    print "listing"
    segment=0
    while True:
        s.sendto(struct.pack('II', LIST_REQ, socket.htonl(segment)), (ip, port))

        data,addr = s.recvfrom(4096)
        print len(data)
        res = struct.unpack('I', data[:4])
        print socket.ntohl(res[0])
        if res[0] == LIST_RESP:
            payload = struct.unpack('II', data[4:12])
            count = socket.ntohl(payload[1])
            if len(data) != (12 + count * 4):
                print "Error: ", len(data), count * 4
                return
            for i in range(0, count):
                entry = struct.unpack('BBBB', data[12+i*4:16+i*4])
                print "%d.%d.%d.%d" % entry
            segment += count
            print segment, " got."
            if segment == socket.ntohl(payload[0]):
                print "Done."
                return


metaquery()
