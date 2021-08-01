
/*
	TCP/IP client
*/ 


#include <stdio.h>
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE ((1024*1024) - 1)
#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "10.0.2.15"
#define buf_size 256

/*auxiliary function to send the data to the server
  arguments:
  	File - file we want to read from 
  	sockfd - Through it information is sent
*/
void send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};
	//fgets - read from file
  while(fgets(data, SIZE, fp) != NULL) {
     
     /*sockfd - Through it information is sent
       data - that data we send
       sizeof(data) - data length
       0 - deafult 
       the function return the amount of bytes sent - if returned -1 its an error
     */
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }

    /* bzero() - erases the data in the n bytes of the memory
       		 starting at the location pointed
       */
    bzero(data, SIZE);
  }
   printf("File data sent successfully. \n");

}

int main(){
  struct sockaddr_in serverAddress; 
  char buf[buf_size];
  socklen_t len;
  FILE *fp;
  char *filename = "data.txt";
  /*flags explantion:
    AF_INET - for IPv4 - my ip
    SOCK_STREAM - for TCP
    0 - for deafult socket
  */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  /*setsockopt - should return 0 , if not - set was faild
   sizeof(serverAddress) - struct size
   SOL_SOCKET - to set options at other levels protocols
   buf - the package "window"
  */
  strcpy(buf,"cubic"); 
  len = strlen(buf);
  if (setsockopt(sockfd, SOL_SOCKET, TCP_CONGESTION, buf, sizeof(serverAddress)) != 0) {
    perror("setsockopt faild"); 
    return -1;
  }


     if(sockfd == -1){
        printf("Could not create socket : %d" ,errno);
     }

    /* "sockaddr_in" is the "derived" from sockaddr structure
     used for IPv4 communication. For IPv6 -  use sockaddr_in6
     memset - cleans n chars from the beginning of the string in their place write c
    */
    //init struct serverAddress
    memset(&serverAddress, 0, sizeof(serverAddress));
    /*sin_family -  set address family to IPv4 for stuct sockaddr
      sin_port -in charge of Port number: need to convert by htons() to network bytes order
     */
    serverAddress.sin_family = AF_INET;
    //convert for port:
    serverAddress.sin_port = htons(SERVER_PORT); //htons -Translate an unsigned short integer into network byte order
    /*sin_addr - for internet address
       convert address by inet_pton to binary network representation
    */
   //convert for ip_address server:
	int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	//check if convert succeed
	if (rval <= 0){
		printf("inet_pton() failed");
		return -1;
	}

     // Make a connection to the server with socket SendingSocket.
     /*flags explanation:
     	sockfd - value that return by create a new socket erlier
     	serverAddress - where to connect
     	sizeof(serverAddress) - size in bytes 
     	sockaddr - witch struct we need = which socket
     */
     if(connect(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
	//if connect faild the connect func will return -1 >> then print:
    	printf("connect() failed with error code : %d" ,errno);
     }

     printf("connected to server\n");
     printf("starting send data...\n");

     // Sends some data to server
     char message[] = "Hello from client \n";
     int messageLen = strlen(message) + 1;
     int bytesSent = send(sockfd, message, messageLen, 0); // how many bytes - for cheking we send currectly number of bytes 

      //send 5 times
     for(int i=0;i<5;i++){
       fp = fopen(filename, "r"); //open file: "r" - read only
       send_file(fp,sockfd); //my function that send the file
       if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
      }
    }
     printf("change mode to: reno \n");
     strcpy(buf, "reno"); 
    /*setsockopt - should return 0 if succeed
     IPPROTO_TCP - for TCP protocol
     sockfd - value that return by create the socket erlier - tell as which socket
     TCP_CONGESTION - method of operation
     buf - our "window" - algo
     len - size
    */
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
    perror("setsockopt"); 
    return -1;
    }

    //send the message 5 time
    for(int i = 0 ; i < 5 ; i++){
      fp = fopen(filename, "r"); //open file
      send_file(fp,sockfd);
      if(fp == NULL) {
        perror("Error in reading file. ");
        exit(1);
      }
    }

    if (bytesSent == -1){
	    printf("send() func failed with error code : %d" ,errno);
     }

    else if (0 == bytesSent){
	    printf("peer has closed the TCP connection prior to send().\n");
     }
     
     else if (messageLen > bytesSent){
	    printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
     }
     else {
	    printf("message was successfully sent .\n");
     }
      

	  sleep(3);
	 

// TODO: All open clientSocket descriptors should be kept
    // in some container and closed as well.
    close(sockfd);

     return 0;
}
