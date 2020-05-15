#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int dropPacket(int errorRate )
{
  int result = 0;

  if (errorRate > 0) {
    double prob = (double) errorRate / 100;
    int draw = rand();
    int threshold = RAND_MAX * prob;
	  if (draw < threshold) {
		  result = 1;
	  }
  }

  return result;
}

int rates[11] = {0, 13, 14, 15, 16, 15, 14, 13, 12, 11, 10};
int switch_times[11] = {20000, 25000, 30000, 35000, 40000, 42000, 45000, 55000, 65000, 75000};

int main(int argc, char **argv) {
	FILE *pfOutFile = NULL;

	unsigned long cnt1 = 0;
	unsigned long cnt2 = 0;

	srand(time(0));

	//int errorPacketRate = atoi(argv[1]);

	int index = 0;
	for (cnt1 = 0; cnt1 < 300000; cnt1++)
	{
	  if (cnt1 > switch_times[index] && index < 9 )
	  {
	    index += 1;
	  }

	  // if (dropPacket(errorPacketRate))
	  if (dropPacket(rates[index]))
	  {
		  if ( (cnt2 & (1024-1)) != 0 ){
			  pfOutFile = fopen ("pktErrorCount.txt", "a+");
		  }
		  else {
			  pfOutFile = fopen ("pktErrorCount.txt", "w+");
		  }
		  cnt2 ++;
		  fprintf(pfOutFile, "0 %lu %lu\n", cnt1, cnt2);
		  //printf("0 %lu %lu\n", cnt1, cnt2);
		  fclose(pfOutFile);
	  }
	  usleep(800); // Adjust to match the Tx rate of ATSC 3.0 sender
	}

	return 0;
}
