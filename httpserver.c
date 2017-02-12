/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef struct {
    char* course_num;
    char* instructor;
    int enrollment;
    double course_quality;
    double course_difficulty;
    double instructor_quality;
} course_info;

int start_server(int PORT_NUMBER)
{

      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;    
      
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Socket");
	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
	perror("Setsockopt");
	exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number: PORT_NUMBER
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	perror("Unable to bind");
	exit(1);
      }

      // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 1) == -1) { /* (sock, 1) allows for only 1 connections */
	perror("Listen");
	exit(1);
      }
          
      // once you get here, the server is set up and about to start listening
      printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
      fflush(stdout);
     
     while(1) {

        // 4. accept: wait here until we get a connection on that port
        int sin_size = sizeof(struct sockaddr_in);
        int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size); /* accept, wait here until got a file descriptor */
        if (fd != -1) {
        printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        
      	// buffer to read data into
      	char request[1024];
      	
      	// 5. recv: read incoming message (request) into buffer
      	int bytes_received = recv(fd,request,1024,0); /* 1024: max # of bytes */
      	// null-terminate the string
      	request[bytes_received] = '\0';
      	// print it to standard out
      	printf("This is the incoming request:\n%s\n", request);

        /*********************************
         *           READ FILE
         *********************************/
  
         // char* reply = malloc(1024);
         // strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>");
         // char* content = malloc(sizeof(char) * 11);
         // FILE* file = fopen("course_eval.txt", "r");
         // fgets(content, 10, file);
         // strcat(reply, content);
         // strcat(reply, "</html>\0");

          char *reply = malloc(1024);
          strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>");
          char* temp = malloc(sizeof(char) * 11);
          FILE* file = fopen("course_evals.txt", "r");
          fgets(temp, 10, file);
          strcat(reply, temp);
          strcat(reply, "</html>\0");

      	// 6. send: send the outgoing message (response) over the socket
      	// note that the second argument is a char*, and the third is the number of chars	
      	send(fd, reply, strlen(reply), 0); /* server send bytes back to browser */
      	
      	// 7. close: close the connection
      	close(fd);
      	printf("Server closed connection\n");
      }
  }

      // 8. close: close the socket
      close(sock); /* shut down the server */
      printf("Server shutting down\n");
  
      return 0;
} 



int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 2) {
      printf("\nUsage: %s [port_number]\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }

  start_server(port_number);
}

