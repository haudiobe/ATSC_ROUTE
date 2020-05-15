This is to  work as repo for the additional script used to simulate the AL-FEC operation 

Content
scripts/RealNeS/
    packetCounterGenerator.c : a test utility to generate packet error counter in abscence of the Forwarder program attached to RealNeS simulator
    periodicCopy.py : tail and remote copy of packet error counter file. To be run at the same directory where Forwarder write the pktErrorCount.txt file
scripts/receiver/
    dropSegment.py: script that decide if a segment shall be drop if packet error counter excceed the amount of AL-FEC repair packets
                    usage: 
                    $> python3 dropSegment.py <EFDT XML file> <lastPktErrorCount file> <AL-FEC ratio>
                    $> python3 dropSegment.py efdt_Video.xml ~/lastPktErrorCount.txt 15
    efdt_Video.xml: sample of efdt_Video.xml, it shall be a copy of the file set with option -f at the ATSC 3.0 sender
    flute_checker.sh: a utility script to check if ATSC 3.0 receiver keep running.
