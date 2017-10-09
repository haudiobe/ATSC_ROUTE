# Import necessary modules
# Referred from https://pymotw.com/2/socket/multicast.html
import socket
import time

# Set multicast group address 
mcast_group = '224.0.23.60'
mcast_port = 4937
mcast_address = (mcast_group, mcast_port)

# Create the datagram socket with necessary details
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Set the time-to-live for messages to 1 so they do not go past the
# local network segment.
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)

# Send file data. 
# Currently, the buffer size is set 1024 bytes.
# IMPORTANT! Change the buffer size to higher if xml file changes in size. 
f = open("SLT_signalling/SLT.xml","rb") 
file_data = f.read(1024)

# Send data to the multicast group
while 1:		
	sock.sendto(file_data, mcast_address)
	time.sleep(0.5)
