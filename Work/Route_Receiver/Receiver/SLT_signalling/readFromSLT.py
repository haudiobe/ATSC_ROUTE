import xml.etree.ElementTree as ET
import sys
import json

channelNumber = int(sys.argv[1]) - 1
tree = ET.parse('SLT.xml')
#tree = ET.parse('SLT_signalling/SLT.xml')
#tree = ET.parse('SLT.xml')
SLT  = tree.getroot()
destinationIP = SLT[channelNumber][0].get('slsDestinationIpAddress')
destinationPort = SLT[channelNumber][0].get('slsDestinationUdpPort')
sourceIP = SLT[channelNumber][0].get('slsSourceIpAddress')
TSI = SLT[channelNumber][0].get('tsi')
print json.dumps([destinationIP, sourceIP, destinationPort, TSI])
#print destinationIP, destinationPort, sourceIP, TSI
#print data
