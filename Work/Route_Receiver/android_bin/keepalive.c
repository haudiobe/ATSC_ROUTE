
/* Send small UDP packets to keep the transmission alive */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char**argv)
{
    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    char sendline[2];
    
    if (argc != 2)
    {
        printf("usage:  udpcli <IP address>\n");
        exit(1);
    }
    
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    servaddr.sin_port=htons(32000);
    
    //    struct timeval tval_before1, tval_after1, tval_before2, tval_after2;
    
    while (1)
    {
        sendline[0]='a';
        //     gettimeofday(&tval_before1, NULL);
        sendto(sockfd,sendline,1,0,(struct sockaddr *)&servaddr,sizeof(servaddr)); //only send one byte every time
        
        //     gettimeofday(&tval_after1, NULL);
        //     printf("sent at %lu.%lu, used %lu us\n", tval_before1.tv_sec, tval_before1.tv_usec, tval_after1.tv_usec-tval_before1.tv_usec);
        usleep(10000);
    }
}
