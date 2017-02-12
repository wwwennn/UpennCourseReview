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
#include "linkedlist.h"

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
    server_addr.sin_port = htons(PORT_NUMBER); // specify port number
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero),8);
    
    // 2. bind: use the socket and associate it with the port number
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Unable to bind");
        exit(1);
    }
    
    // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
    if (listen(sock, 1) == -1) {
        perror("Listen");
        exit(1);
    }
    
    // once you get here, the server is set up and about to start listening
    printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
    fflush(stdout);
    
    while(1){
        // 4. accept: wait here until we get a connection on that port
        int sin_size = sizeof(struct sockaddr_in);
        int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
        if (fd != -1) {
            printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            
            // buffer to read data into
            char request[1024];
            
            // 5. recv: read incoming message (request) into buffer
            int bytes_received = recv(fd,request,1024,0);
            // null-terminate the string
            request[bytes_received] = '\0';
            // print it to standard out
            printf("This is the incoming request:\n%s\n", request);
            
            // this is the message that we'll send back
            char *reply = malloc(1024);
            strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>");
            char* temp = malloc(sizeof(char) * 11);
            FILE* file = fopen("course_evals.txt", "r");
            fgets(temp, 10, file);
            strcat(reply, temp);
            strcat(reply, "</html>\0");
            //	char *reply = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>Hello world!<p>This text is <b>bold</b>.</html>";
            
            // 6. send: send the outgoing message (response) over the socket
            // note that the second argument is a char*, and the third is the number of chars
            send(fd, reply, strlen(reply), 0);
            //      }
            
            // 7. close: close the connection
            close(fd);
            printf("Server closed connection\n");
        }
    }
    // 8. close: close the socket
    close(sock);
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
    
    FILE* raw_data = fopen("course_evals.txt", "r");
    char* line = malloc(sizeof(char) * 100);
    
    
    while((line = fgets(line, 100, raw_data)) != NULL) {
        course_info* info = malloc(sizeof(course_info));
        info->course_num = malloc(sizeof(char) * 15);
        info->instructor = malloc(sizeof(char) * 30);
        
        // get course_num
        int i = 0;
        while(line[i] != ',') {
            info->course_num[i] = line[i];
            i++;
        }
        info->course_num[i] = '\0';
        i++;
        
        // get instructor_name
        int j = 0;
        while(line[i] != ',') {
            info->instructor[j] = line[i];
            j++;
            i++;
        }
        info->instructor[j] = '\0';
        i++;
        
        // get enrollment
        char* temp_for_enrollment = malloc(sizeof(char) * 6);
        j = 0;
        while(line[i] != ',') {
            temp_for_enrollment[j] = line[i];
            j++;
            i++;
        }
        temp_for_enrollment[j] = '\0';
        i++;
        info->enrollment = atoi(temp_for_enrollment);
        
        // get course_quality
        j = 0;
        while(line[i] != ',') {
            temp_for_enrollment[j] = line[i];
            j++;
            i++;
        }
        temp_for_enrollment[j] = '\0';
        i++;
        info->course_quality = atof(temp_for_enrollment);
        
        // get course_difficulty
        j = 0;
        while (line[i] != ',') {
            temp_for_enrollment[j] = line[i];
            j++;
            i++;
        }
        temp_for_enrollment[j] = '\0';
        i++;
        info->course_difficulty = atof(temp_for_enrollment);
        
        // get instructor_quality
        j = 0;
        while (i < strlen(line)) {
            temp_for_enrollment[j] = line[i];
            j++;
            i++;
        }
        temp_for_enrollment[j] = '\0';
        info->instructor_quality = atof(temp_for_enrollment);
        
        
        
        free(temp_for_enrollment);
    }
    
    
    start_server(port_number);
}

