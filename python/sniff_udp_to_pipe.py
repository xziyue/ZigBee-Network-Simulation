'''
Run wireshark afterwards:
    sudo wireshark -k -i /tmp/udp2pipe
'''

import os
from scapy.all import *
import time
import struct

outputFile = '/tmp/udp2pipe'
if os.path.exists(outputFile):
    os.remove(outputFile)

# create PIPE
os.mkfifo(outputFile)

outfile = open(outputFile, 'wb')

pcapGlobHeader = b'\xd4\xc3\xb2\xa1' \
                 b'\x02\x00\x04\x00' \
                 b'\x00\x00\x00\x00' \
                 b'\x00\x00\x00\x00' \
                 b'\x00\x01\x00\x00' \
                 b'\xc3\x00\x00\x00'

outfile.write(pcapGlobHeader)

def get_packet_bin(duration, load):
    sec = int(duration)
    ms = int((duration - sec) * 1000.0)
    hdr = struct.pack('IIII', sec, ms, len(load), len(load))
    return hdr + load

packetCounter = 0
startTime = time.time()

def write_to_pipe(pkt):
    global header, packetCounter

    packetCounter += 1
    if packetCounter % 2 == 0:
        # skip every second packet
        return

    print('dumping one packet...')
    nowTime = time.time()

    load = pkt[Raw].load
    outData = get_packet_bin(nowTime - startTime, load)
    outfile.write(outData)
    outfile.flush()

# begin sniffing
sniff(iface='lo',
      filter='udp src port 60009 or udp src port 60010',
      prn=write_to_pipe)


