# Import necessary modules
# Referred from https://pymotw.com/2/socket/multicast.html
import socket

# Set multicast group address 
mcast_group = '224.1.1.1'
mcast_port = 5007
mcast_address = (mcast_group, mcast_port)

# Create the datagram socket with necessary details
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Set the time-to-live for messages to 1 so they do not go past the
# local network segment.
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)

# Read file data
f = open("SDP1.sdp","rb") 
file_data = f.read(1024)

# Send data to the multicast group
sock.sendto(file_data, mcast_address)
