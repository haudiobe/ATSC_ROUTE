function [  ] = PlotDelays( VideoData, videoSegmentDuration )

%   This function takes the final video segments log file and generates
%   corresponding plot

format long

%Populate data into a matrix
video_times = importdata(VideoData,' ');

%Obtain matrix dimensions to determine range of operations
[rows,columns] = size(video_times.data);

%We assume that the segments have fixed durations. X contains segments'
%start times
X=[0:videoSegmentDuration:(rows-1)*videoSegmentDuration];

%Number of consecutive Segment PAIRS
X2=[1:rows-1];

%Number of segments
X3=[1:rows];

%video_times.data(1:rows,1)=video_times.data(1:rows,1)-2000000;

%Calculate delay at server side (i.e. Delay between start of fdt generation and
%end of file sending)
sender=video_times.data(1:rows,5) - floor(video_times.data(1:rows,1));

sendingTime=video_times.data(1:rows,1) - video_times.data(1,1);

figure
stem(X,sender,'fill')
ylabel('FLUTE Sender Delay (usec)')
xlabel('Media Presentation Time (sec)')

figure
stem(X,sendingTime,'fill')
ylabel('Absolute Sending Time')
xlabel('Media Presentation Time (sec)')

%Calculate delay at receiver side (i.e. Delay between start of fdt/file
%reception and end of file processing where segment is available for
%reference client
receiver=video_times.data(1:rows,8) - floor(video_times.data(1:rows,6));
receiver2=video_times.data(1:rows,8) - floor(video_times.data(1:rows,7));
receiver3=video_times.data(1:rows,7) - floor(video_times.data(1:rows,6));

receptionTime=video_times.data(1:rows,8)-video_times.data(1,8);

figure
stem(X,receptionTime,'fill')
ylabel('Reception Time)')
xlabel('Media Presentation Time (sec)')


%figure
%stem(X,receiver,'fill')
%ylabel('FLUTE Receiver Delay (usec)')
%xlabel('Media Presentation Time (sec)')

%figure
%stem(X,receiver2,'fill')
%ylabel('Receiver Processing Delay (usec)')
%xlabel('Media Presentation Time (sec)')

%figure
%stem(X,receiver3,'fill')
%ylabel('Delay Between Start of Reception and Start of Processing (usec)')
%xlabel('Media Presentation Time (sec)')

%Calculate delay between initiation of HTTP requests and complete delivery
%of segment to reference client
httpDelay=video_times.data(1:rows,11) - floor(video_times.data(1:rows,9));

figure
stem(X,httpDelay,'fill')
ylabel('HTTP Requests Delay (msec)')
xlabel('Media Presentation Time (sec)')

%Calculate delay start of segment appending to source buffer and start of
%segment rendering
rendering=video_times.data(1:rows,15) - floor(video_times.data(1:rows,12));

figure
stem(X,rendering,'fill')
ylabel('Delay of rendering after source buffer appending (msec)')
xlabel('Media Presentation Time (sec)')

%Times between HTTP requests
%betweenHttp=video_times.data(2:rows,9) - floor(video_times.data(1:rows-1,9));

%figure
%stem(X2,betweenHttp,'fill')
%ylabel('Time Between Consecutive HTTP Requests (msec)')
%xlabel('Number of Consecutive Pair')

%BufferLengths
bufLen=video_times.data(1:rows,14);

figure
stem(X3,bufLen,'fill')
ylabel('Buffer Length After Appending (sec)')
xlabel('Segment Number')

%Times between start of FDT receptions
betweenFdt=video_times.data(2:rows,6) - floor(video_times.data(1:rows-1,6));
%figure
%stem(X2,betweenFdt,'fill')
%ylabel('Time Between Consecutive FDT Receptions (usec)')
%xlabel('Number of Consecutive Pair')

%TransmissionDelay
transDelay=video_times.data(1:rows,6) - video_times.data(1:rows,4);

%figure
%stem(X,transDelay,'fill')
%ylabel('Delay From Start of Sending to Start of Reception(usec)')
%xlabel('Media Presentation Time (sec)')

%Delay between start of sending and end of reception
send2Recep=video_times.data(1:rows,8) - video_times.data(1:rows,1);

figure
stem(X,send2Recep,'fill')
ylabel('Delay From Start of Sending to End of Reception(usec)')
xlabel('Media Presentation Time (sec)')

%Delay between start of sending and end of segment fetching
send2HttpFetch=video_times.data(1:rows,11) - floor(video_times.data(1:rows,1)/1000);

%figure
%stem(X,send2HttpFetch,'fill')
%ylabel('Delay From Start of Sending to End of Segment Fetching by RC(msec)')
%xlabel('Media Presentation Time (sec)')

%Delay between start of sending and end of source buffer appending
send2SrcBuff=video_times.data(1:rows,13) - floor(video_times.data(1:rows,1)/1000);

%figure
%stem(X,send2SrcBuff,'fill')
%ylabel('Delay From Start of Sending to End of Source Buffer Appending(msec)')
%xlabel('Media Presentation Time (sec)')


%Division by 1000 is needed to convert the time from usec precision to msec
%precision. Time instants logged by javascript have msec precision
e2e=video_times.data(1:rows,columns) - floor(video_times.data(1:rows,1)/1000);

%figure
%stem(X,e2e,'fill')
%ylabel('End-to-End Delay (msec)')
%xlabel('Media Presentation Time (sec)')

figure
plot(X,floor(send2Recep/1000),X,send2HttpFetch,X,send2SrcBuff,X,e2e)
legend('Between Start of Sending and End of Reception','Between Start of Sending and End of Segment Fetching by RC','Between Start of Sending and End of Appending to Src Buffer','End-to-End Delay');
ylabel('Delay (msec)')
xlabel('Media Presentation Time (sec)')
end

