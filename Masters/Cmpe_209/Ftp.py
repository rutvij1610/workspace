import sys
from scapy.all import *
import base64

def ftpSniff(pkts):
#	dest = pkt.getlayer(IP).dst
#	raw = pkt.sprintf('%Raw.load%')
	for  pkt in pkts:
	#  t = re.search(r"USER 	
		user = re.findall('(?i)USER (.*)', str(pkt[TCP].payload))
		pswd = re.findall('(?i)PASS (.*)', str(pkt[TCP].payload))
		if user:
			print '[*] Detected FTP Login to ' + pkt[IP].dst
			print '[+] User account: ' + str(user[0])
		elif pswd:
			print '[+] Password: ' + str(pswd[0])

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print "usage: write the name of the packet after the script like this-> Ftp.py pcapFileName"
    sys.exit()	 
  pcap= rdpcap(sys.argv[1])
  pcap = [pkt for pkt in pcap if TCP in pkt]
  ftpSniff(pcap) 
