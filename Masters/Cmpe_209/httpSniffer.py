from scapy.all import *
import sys
import base64

def parsePCAP(pkts):
  for pkt in pkts:
    t = re.search(r"Authorization:\sBasic\s(.*)\r", str(pkt[TCP].payload))
    if t:
        print '[*] Sniffed Credentials %s' % base64.b64decode(t.group(1))	
        print "Source IP: " + pkt[IP].src
        print "Destination IP: " + pkt[IP].dst	
        print "Source port: " + str(pkt[TCP].sport)	
        print "Destinations port: " + str(pkt[TCP].dport)

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print "usage: write the name of the packet after the script like this-> httpSniffer.py pcapFileName"
    sys.exit()	 
  pcap= rdpcap(sys.argv[1])
  pcap = [pkt for pkt in pcap if TCP in pkt]
  parsePCAP(pcap) 
