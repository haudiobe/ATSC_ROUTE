import os.path
import sched, time
import shutil
import subprocess
import select
import sys

def tail_lines(filename,linesback=10,returnlist=0):
    """Does what "tail -10 filename" would have done
       Parameters:
            filename   file to read
            linesback  Number of lines to read from end of file
            returnlist Return a list containing the lines instead of a string

    """
    avgcharsperline=75

    file = open(filename,'r')
    while 1:
        try: file.seek(-1 * avgcharsperline * linesback,2)
        except IOError: file.seek(0)
        if file.tell() == 0: atstart=1
        else: atstart=0

        lines=file.read().split("\n")
        if (len(lines) > (linesback+1)) or atstart: break
        #The lines are bigger than we thought
        avgcharsperline=avgcharsperline * 1.3 #Inc avg for retry
    file.close()

    if len(lines) > linesback: start=len(lines)-linesback -1
    else: start=0
    if returnlist: return lines[start:len(lines)-1]

    out=""
    for l in lines[start:len(lines)-1]: out=out + l + "\n"
    return out

#file_path = './Debug/pktErrorCount.txt'
file_path = './pktErrorCount.txt'

s = sched.scheduler(time.time, time.sleep)

prev_line = ''
#text_file = open('lastPktErrorCount.txt', "w", 0)

def do_something(sc):
    global prev_line
    global text_file
    
    if os.path.isfile(file_path):        
        new_line = tail_lines(file_path, 1)
        
        if prev_line != new_line:
            text_file = open('lastPktErrorCount.txt', "w")
            #print new_line                
            n = text_file.write(new_line)
            text_file.close()        
        
            pSCP = subprocess.Popen(["scp", 'lastPktErrorCount.txt', "vorke@192.168.3.30:/home/vorke/pktCounter/"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            #pSCP = subprocess.Popen(["scp", 'lastPktErrorCount.txt', "frv@10.0.2.15:/home/frv/"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            sts = os.waitpid(pSCP.pid, 0) 
        
        prev_line = new_line
        s.enter(0.000001, 1, do_something, (sc,))
    else:
        s.enter(0.1, 1, do_something, (sc,))

s.enter(5, 1, do_something, (s,))
s.run()
