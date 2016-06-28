import socket
import struct
import sys

# Set multicast group address 
mcast_group = '224.0.23.60'
mcast_port = 4937
mcast_address = (mcast_group, mcast_port)

# Create the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
# Extra option, need to clarify for what is this
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) 

# Bind to the server address
sock.bind(mcast_address)     

# Tell the operating system to add the socket to the multicast group
# on all interfaces.
mreq = struct.pack("4sl", socket.inet_aton(mcast_group), socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

#while True:
print >>sys.stderr, '\nwaiting to receive message'
data, address = sock.recvfrom(1024)

print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
print >>sys.stderr, data
    
f = open('SLT.xml', 'w')
f.write(data)   
f.close()
    	
#If need be, use this to send acknowledgement back to the sender.
#print >>sys.stderr, 'sending acknowledgement to', address
#sock.sendto('ack', address)
