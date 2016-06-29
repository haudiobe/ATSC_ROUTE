import xml.etree.ElementTree as ET
import lxml.etree as etree
import sys


SLT = ET.Element("SLT", bsid="1")

for channel in range(1,3):
	service = ET.SubElement(SLT, "service", majorChannelNo=str(channel), serviceCategory="1", hidden="true")
	portAddress = 4004 + channel
	tsi = 4 + channel
	broadcastSvcSignalling = ET.SubElement(service, "broadcastSvcSignalling", slsProtocol="1", slsDestinationIpAddress="224.1.1.1", slsDestinationUdpPort=str(portAddress), slsSourceIpAddress=sys.argv[1], tsi=str(tsi))
	# slsProtocol=1 refers to ROUTE protocol
	# This source IP address shall be later received as a run-time parameter.
	tree = ET.ElementTree(SLT)

tree.write("SLT_signalling/intermediate.xml")
x = etree.parse("SLT_signalling/intermediate.xml")
xml_string = etree.tostring(x, pretty_print = True)
#print xml_string
text_file = open("SLT_signalling/SLT.xml", "w")
text_file.write(xml_string)
text_file.close()
