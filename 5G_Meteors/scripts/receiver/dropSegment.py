import os
import shutil
import xml.dom.minidom
import sys
from itertools import accumulate
import bisect
import time
import math


def main():
    # Packet size= Header (Ethernet (14) + IP (20) + UDP (8) + ALC Header (24) + payload (1428)
    packet_size = 1494

    file_segment_size = sys.argv[1]
    file_packet_counter = sys.argv[2]
    fec_ratio = int(sys.argv[3]) / 100

    if os.path.exists(file_packet_counter):
        os.remove(file_packet_counter)

    recv_folder = "DASH_Content1"
    lost_folder = "lost"
    if not os.path.exists(lost_folder):
        os.makedirs(lost_folder)

    # use the parse() function to load and parse an XML file
    doc = xml.dom.minidom.parse(file_segment_size);

    # get a list of XML tags from the document and print each one
    files = doc.getElementsByTagName("File")
    print("File segments to be received: %d" % files.length)

    segment_size = []
    number_repair_symbols = []
    packets_per_segment = []
    segment_file_names = []
    for segment in files:
        tmp_size = int(segment.getAttribute("Content-Length"))        
        #segment_size.append(tmp_size)
        source_symbols = math.ceil(tmp_size / packet_size)
        repair_symbols = math.floor(fec_ratio * source_symbols)
        
        number_repair_symbols.append(repair_symbols)        
        
        # Adding one for the transmission of E-FDT of the current segment, excluding repair_symbols as no FEC is actually being transmitted
        packets_per_segment.append(source_symbols + 1)
        
        segment_file_names.append(segment.getAttribute("Content-Location")[8:])

    accumulated_packets_per_segment = (list(accumulate(packets_per_segment)))
    #print(accumulated_packets_per_segment);

    prev_line = ''
    prev_error_packets = 0
    prev_segment = ''

    file_last_counter = None
    segment_lost = False
    while True:
        if os.path.exists(file_packet_counter):
            file_last_counter = open(file_packet_counter, "r")
            if file_last_counter is not None:
                # Normally we should get a single line
                # lines = [line.rstrip() for line in file_last_counter]
                line = file_last_counter.readline()

                # if new last line in the file has been updated
                if line != '' and prev_line != line:
                    # print('processing %s' % line)

                    counters = line.split()
                    ue_id = int(counters[0])
                    total_packets = int(counters[1])
                    error_packets = int(counters[2])

                    # find out the current segment file
                    index = bisect.bisect_left(accumulated_packets_per_segment, total_packets)
                    # file to be moved/deleted in case packet error counter exceed the number of repair symbols
                    current_segment = segment_file_names[index]
                    #print("Current segment: %s, Total packets %s" % (current_segment, total_packets))

                    if prev_segment == current_segment:
                        # update stats
                        lost_packets += error_packets - prev_error_packets
                        #print("    overall losses: %d, segment losses: %d ?> Limit %d" % (error_packets, lost_packets, number_repair_symbols[index]))
                    else:
                        # reset stat
                        prev_segment = current_segment
                        lost_packets = error_packets - prev_error_packets
                        segment_lost = False

                    prev_error_packets = error_packets

                    # simulate AL-FEC
                    if not segment_lost and lost_packets > number_repair_symbols[index]:
                        segment_lost = True
                        print("        Lost segment: %s, overall losses: %d, segment losses: %d > Limit %d" % (
                            current_segment, error_packets, lost_packets, number_repair_symbols[index]))

                        if os.path.exists(recv_folder + "/" + current_segment):
                            # (removing 'file:///')
                            dest = shutil.move(recv_folder + "/" + current_segment, lost_folder + "/" + current_segment)

                    prev_line = line

                file_last_counter.close()
        else:
            time.sleep(0.01)


if __name__ == "__main__":
    main();
