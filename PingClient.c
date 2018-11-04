// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <netdb.h>
#include <time.h>

#define TIMEOUT 1 
#define BUFSIZE 50
#define TIMES 10
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char **argv) {
	float min=10000;
	float max=0;
	float average=0;
	char host_name[100];
	int sockfd,port,packet_loss,i;	
	char buf[BUFSIZE]; 
	char timeString[10];
	struct hostent *hp;
	struct sockaddr_in server,from;
	struct timeval starttime, endtime;//init the clock
	struct timeval timeout={TIMEOUT,0};//set timeout	
	int times = TIMES;
	if (argc != 3) { 
		printf("Usage: server port\n");
		exit(1);
	}
   	sockfd= socket(AF_INET, SOCK_DGRAM, 0);
   	if (sockfd < 0) {
		error("socket");
	}
	memset((char *)&server,0,sizeof(server)); // zero the array
   	server.sin_family = AF_INET;
	strncpy(host_name,argv[1],sizeof(host_name));
   	hp = gethostbyname(argv[1]);
   	if (hp==0) error("Unknown host");

   	bcopy((char *)hp->h_addr, (char *)&server.sin_addr,hp->h_length);
	port = atoi(argv[2]);
	server.sin_port = htons(port);
	unsigned int length_sock = sizeof(struct sockaddr_in);
	//Set receive UDP message timeout
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
	//send the messages and calculate RTT
	packet_loss = 0;
	for(i=0;i<times;i++)
	{
		char send_buf[50];
		char a[3];
		send_buf[0]='P';
		send_buf[1]='I';
		send_buf[2]='N';
		send_buf[3]='G';
		sprintf(a, "%d", i+1);
		send_buf[4]=a[0];
		send_buf[5]=a[1];
		send_buf[6]=a[2];
	time_t secs = time(0); // get seconds from Epoch
	struct tm *local = localtime(&secs);
	sprintf(timeString, "%02d:%02d:%02d", local->tm_hour,local->tm_min,local->tm_sec);
	for(int i=0;i<strlen(timeString);i++)
	{
		send_buf[i+7] = timeString[i];
	}
		if(sendto(sockfd,send_buf,strlen(buf), 0, (struct sockaddr *)&server,length_sock)==-1)
		{
			perror("sendto");
			continue;
		}
		gettimeofday(&starttime,0);
		printf("Ping packet send to %s port %d\n",host_name,port);
		int recvlen = recvfrom(sockfd,buf,BUFSIZE,0,(struct sockaddr *)&server, &length_sock);
		if(recvlen>=0)
		{
            		gettimeofday(&endtime,0);
            		double timeuse = 1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
			if(min>timeuse) min = timeuse;
			if(max<timeuse) max = timeuse;
			average = average+timeuse;
            		printf("PING received from %s:seq#=%d time=%.3f ms\n",host_name,i+1,timeuse);
		}
		else
		{
			packet_loss++;
			printf("Request Time Out!\n");
			continue;
		}
	}
	float loss_rate = packet_loss/10.0*100;
	average = average/(10-packet_loss);
	printf("--- ping statistics --- %d packets transmitted, %d received, %.3f%%",i,10-packet_loss,loss_rate);
	printf(" packet loss rtt min/avg/max= %.3f %.3f %.3f ms",min,average,max);
	close(sockfd);
	return 0;
}
