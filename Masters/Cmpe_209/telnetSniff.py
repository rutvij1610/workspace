from scapy.all import *
from sys import exit
import binascii
import struct
import base64
from subprocess import Popen, PIPE
from collections import OrderedDict
from StringIO import StringIO

telnet_stream = OrderedDict()

def pkt_parser(pkt):  
#Start parsing packets here
    
    if pkt.haslayer(Raw):
        load = pkt[Raw].load

    # TCP
    if pkt.haslayer(TCP) and pkt.haslayer(Raw) and pkt.haslayer(IP):
        src_ip_port = str(pkt[IP].src) + ':' + str(pkt[TCP].sport)
        dst_ip_port = str(pkt[IP].dst) + ':' + str(pkt[TCP].dport)
    # Telnet
        telnet_logins(src_ip_port, dst_ip_port, load)

def telnet_logins(src_ip_port, dst_ip_port, load):
#Catch telnet logins and passwords
    global telnet_stream
    msg = None
    if src_ip_port in telnet_stream:
        try: 
            telnet_stream[src_ip_port] += load.decode('utf8')
        except UnicodeDecodeError:
            pass 
  
        # \r or \r\n or \n terminate commands in telnet if my pcaps are to be believed
        if '\r' in telnet_stream[src_ip_port] or '\n' in telnet_stream[src_ip_port]:
            telnet_split = telnet_stream[src_ip_port].split(' ', 1)
            cred_type = telnet_split[0]
            value = telnet_split[1].replace('\r\n', '').replace('\r', '').replace('\n', '')
            # Create msg, the return variable
            msg = 'Telnet %s: %s' % (cred_type, value)
            print(src_ip_port, dst_ip_port, msg)
            del telnet_stream[src_ip_port]

    # This part relies on the telnet packet ending in
    # "login:", "password:", or "username:" 
    if len(telnet_stream) > 100:
        telnet_stream.popitem(last=False) 
    mod_load = load.lower().strip()
    if mod_load.endswith('username:') or mod_load.endswith('login:'):
        telnet_stream[dst_ip_port] = 'username '
    elif mod_load.endswith('password:'):
        telnet_stream[dst_ip_port] = 'password '
		
def main():
    pcapfile = raw_input("Enter the pcapfile name:")
    # Read packets from either pcap 
    for pkt in PcapReader(pcapfile):
        pkt_parser(pkt)
        
if __name__ == "__main__":
   main()

