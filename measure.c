/*
    TCP/IP-server - listener
*/

#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define SERVER_PORT 5060  //The port that the server listens
#define SIZE (1024*1024) - 1 
#define buf_size 256
#define buffer_size 1024
#define max_size 500

int main(){
    char buf[buf_size]; // type algorihm (cubic or reno)
    char buffer[buffer_size]; //save the data that recived
    socklen_t len;
    int message = 0; // for check data size that recived
    /* for calculate time: */
    int count = 0;
    double time = 0;
    bool toStart = true;
    double average_time_cubic = 0;
    double average_time_reno = 0;
    double average_time_for_all = 0;
    struct timeval start, end;
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket

    // Open the listening (server) socket
    int listeningSocket = -1;  
	 //AF_INET - for IPV4 
     // SOCK_STREAM  - for TCP Procotol
    if((listeningSocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1){
        printf("Could not create listening socket : %d" ,errno);
    }
      printf("Socket created succesfuly \n");

	// Reuse the address if the measure(server) socket on was closed
	// and remains for TIME-WAIT state till the final removal.
    //*Adapting the socket to my needs*:
	//SOL_SOCKET- is the socket layer itself. It is used for options that are protocol independent.
    //listeningSocket - the socket
    /*SO_REUSEADDR - For the purpose of performing the configuration change that we make during the sending 
    and receiving of the information (in order not to restart the program in order to change the configuration) */ 
    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse,sizeof(int)) < 0){
         printf("setsockopt() failed with error code : %d" , errno);
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication.
    
    struct sockaddr_in serverAddress;
    //init serverAddress struct
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET; // for ipv4 -my ip
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order - for port
      
    /*  the socket to the port with any IP at this port
    	bind - Link the address and port with the socket
    	listeningSocket - value that returned from create the socket erlier(its the socket)
    	sockaddr - pointer to the struct that contains the port and address of the server
    	sizeof(serverAddress) - length in bytes of the socket
    */
    if (bind(listeningSocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1){
        printf("Bind func failed with error code : %d" ,errno);
        return -1;
    }
      
    printf("The bind successfuly done \n");
  
    // Make the socket listening and wait for packet:
    ////max_size = 500 - The maximum size of queue connection requests
    if (listen(listeningSocket, max_size) == -1){
  	printf("listen func failed with error code : %d",errno);
        return -1;
    }
      
    //Accept and incoming connection
    printf("server ready: \n");
    printf("Waiting for incoming TCP-connections...\n");
      
    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1){
        //init clientAddress
    	memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);

        /*Accept incoming connection:
        sockaddr - the socket client that connect (sender)
          listeningSocket - server socket (measure)
        */
	int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    	if (clientSocket == -1){
           printf("listen func failed with error code : %d",errno);
           return -1;
    	}

    	printf("A new client connection accepted\n");
        len = sizeof(buf); 
        //print that the socket is currently on cubic 
        if (getsockopt(listeningSocket, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { 
        perror("error! ,getsockopt");
        return -1;
        } 
        //print mode algorithm - "cubic"
        printf("Current: %s\n", buf); 


        while (1){
         while(count != 5){//if not all messages have been received yet
         if(toStart == true){
            gettimeofday(&start, 0);//start the time
            toStart = false;
         }
          /*sockfd - Through it information is sent
            buffer - pointer to the buffer we read from him
            0 - deafult
          */
          recv(clientSocket, buffer, buffer_size, 0);//Some bits were absorbed
          message = message+recv(clientSocket, buffer, buffer_size, 0);
          
          //check the size of the message - to check its correctness
          if(message >= (SIZE + 1)){
              count++;
              toStart = false;
              gettimeofday(&end, 0); 
              time = (double)(end.tv_usec-start.tv_usec)/1000000 + (double)(end.tv_sec - start.tv_sec);
              average_time_cubic +=time; // add the time for the avarage calculation (cubic)
              printf("The message arrived successfully in: %lf seconds \n" , time);
              message = message-buffer_size*buffer_size;//next message
          }
          /* bzero() - erases the data in the n bytes of the memory
       		 starting at the location pointed
          */
         bzero(buffer, buffer_size);//clear the buffer
	}
        
        break;
        toStart = true ;
    }

	/* socket  reno */
        strcpy(buf, "reno"); //change mode (algorithm) to reno
        len = strlen(buf);
        if (setsockopt(listeningSocket, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("not succeed to set socket algorithm"); 
        return -1;
        }
        
       // len = sizeof(buf);
        // if (getsockopt(listeningSocket, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        // perror("getsockopt"); 
        // return -1; 
        // } 
        printf("change mode to : %s\n", buf); 
        
        while (1){
            if(toStart == true){
            toStart = false;
             gettimeofday(&start, 0);
         }
           /*sockfd - Through it information is sent
            buffer - pointer to the buffer we read from him
            0 - deafult
          */
          message = message + recv(clientSocket, buffer, buffer_size, 0);
          //check the size of the message - to check its correctness
          if(message >= (SIZE + 1)){
              toStart = true;
              gettimeofday(&end, 0); //get current time
              time = (double)(end.tv_usec-start.tv_usec) / 1000000 + (double)(end.tv_sec - start.tv_sec); // calculating time
              average_time_reno += time; // add the time for the avarage calculation (reno)
	      printf("The message arrived successfully in: %lf seconds \n", time);
              message = message-buffer_size*buffer_size;
          }
          
          bzero(buffer, buffer_size);
          if (recv(clientSocket, buffer, buffer_size, 0) <= 0){
             break;
          }   
        }
        break;
    }
    //calculating averages:
    average_time_cubic = average_time_cubic/5; 
    average_time_reno = average_time_reno/5;
    average_time_for_all = (average_time_cubic + average_time_reno)/2;
    printf("avarage time for cubic : %lf seconds \n", average_time_cubic);
    printf("avarage time for reno : %lf seconds \n", average_time_reno);
    printf("avarage time for all data : %lf seconds \n", average_time_for_all);
    
    // close the socket    
    close(listeningSocket);
      
    return 0;
}
    
