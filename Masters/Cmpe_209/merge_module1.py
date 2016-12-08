#!/usr/bin/python

import dpkt
import socket
import optparse
import pygeoip
import re

def printPcap(pcap):
	d=['a']
        for(ts,buf) in pcap:
                try:
                        eth=dpkt.ethernet.Ethernet(buf)
                        ip=eth.data
                        src=socket.inet_ntoa(ip.src)
                        dst=socket.inet_ntoa(ip.dst)
                       # print '[+] Src: ' + src + '-->Dst:' +dst#
			d.append(dst)
			
		except:
                        pass
	
	#print d
	#list(set(d))#
	print '**************sorted IPs**********'
	dst_list= list(set(d))
	#print dst_list#
	return dst_list


gip=pygeoip.GeoIP('/opt/GeoIP/Geo.dat')
def retGeoStr(ip):
	try:
		rec=gip.record_by_name(ip)
		city=rec['city']
		country=rec['country_code3']
		if city !='':
			geoloc=city+'.'+country	
		else:
			geoloc=country
		return geoloc
	except Exception, e:
		return 'Unregistered'


def black_list_url(pcap,black_url):

        #f = open('/home/rutvij/Desktop/urls_test.pcap')
        #pcap1 = dpkt.pcap.Reader(f)
        http_ports = [80] # Add other ports if you website on non-standard port.
        urls = [ ]
        #black_url =['google.com']
        err=0

        x = 0
        dict ={}

        for timestamp, buf1 in pcap:
                eth = dpkt.ethernet.Ethernet(buf1)
                ip = eth.data
                tcp = ip.data
                if tcp.__class__.__name__ == 'TCP':
                        if tcp.dport in http_ports and len(tcp.data) > 0:
                                try:
                                        http = dpkt.http.Request(tcp.data)

                                        if ((http.headers['host'])!= ""):
                                                host = http.headers['host']
                                                src_ip = socket.inet_ntoa(ip.src)
                                                dst_ip = socket.inet_ntoa(ip.dst)


                                                dict[host] = [src_ip,dst_ip]
                                                x=x+1

                                                urls.append(http.headers['host'])
                                except Exception as e:
                                        # Just in case we come across some stubborn kid.
                                        #print "[-] Some error occured. - %s" % str(e)
                                        i =0

        print 'Total no of URLs found in PCAP are : %d'%x
        print 'Total no of URLs successfully captured : %d'%len((urls))


	#print "[+] URLs extracted from PCAP file are:\n"
        for url in urls:
                for b_url in black_url:
                        if(re.search(b_url,url)):
                                print 'Black listed url found...\n'
                                print 'URL : %s'%b_url
                                print 'Now looking for the Src and Dst IPs for this url ...'
                                #print 'URL Before'
                                #print url

                                url = url.replace('/','')
                                print dict[url]
                                print '=============================='





########main########

f=open('urls_test.pcap')
f1 = open('urls_test.pcap')
pcap=dpkt.pcap.Reader(f)
pcap1 = dpkt.pcap.Reader(f1)


#printPcap(pcap)#

dst_list=printPcap(pcap)
print dst_list

#retGeoStr(*dst_list)
#print retGeoStr(*dst_list)
loc=[]
for item in dst_list:
	retGeoStr(item)
	loc.append(retGeoStr(item))
#	print retGeoStr(item)
dict_loc = retGeoStr(dst_list)
print '================================================='
print 'Mapping of Physical locations to destination IPs'
print '================================================='
 	
#for (a,b) in zip(dst_list,loc):
#	print(a,"===>",b)
for ip,loc in dict_loc.items():
        print "IP : %s ====> LOC: %s" %(ip,loc)



print 'CALLING THE BLACK_LIST'
black_url =['google.com']

black_list_url(pcap1,black_url)


f.close()
f1.close()
                             
