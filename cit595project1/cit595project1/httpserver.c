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
#include <stdbool.h>
#include <ctype.h>

node* read_file();
char* get_table(node*);
int start_server(int);
char* get_search_result(node*, char*);

bool identify_request(char* request, char* target) {
    if (request == NULL || target == NULL) return false;
    int request_len = (int)strlen(request);
    int target_len = (int)strlen(target);
    
    for (int i = 0; i + target_len < request_len; i++) {
        char* substr = (char*) malloc(sizeof(char) * (target_len + 1));
        strncpy(substr, request + i, target_len);
        if (strcmp(substr, target) == 0) return true;
        free(substr);
    }
    return false;
}

char* get_search_target(char* request) {
    int request_len = (int)strlen(request);
    char* substr = (char*) malloc(sizeof(char) * 100);
    for (int i = 0; i < request_len; i++) {
        if (request[i] == '=') {
            strncpy(substr, request + i + 1, request_len - (i + 1));
            return substr;
        }
    }
    return NULL;
}

char* get_link_target(char* request) {
    int request_len = (int)strlen(request);
    char* substr = (char*) malloc(sizeof(char) * 100);
    for (int i = 0; i < request_len; i++) {
        if (request[i] == '/') {
            strncpy(substr, request + i + 1, request_len - (i + 1));
            return substr;
        }
    }
    return NULL;
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

node* read_file() {
    node* head = NULL;
    
    FILE* raw_data = fopen("course_evals.txt", "r");
    if(raw_data == NULL) {
        perror("Can't open the file");
        exit(-1);
    }
    char* line = malloc(sizeof(char) * 100);
    fgets(line, 100, raw_data);
    
    while(feof(raw_data) == 0) {
        course_info* info = malloc(sizeof(course_info));
        info->course_num = malloc(sizeof(char) * 15);
        info->instructor_name = malloc(sizeof(char) * 30);
        
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
            info->instructor_name[j] = line[i];
            j++;
            i++;
        }
        info->instructor_name[j] = '\0';
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
        
        
        add_first(&head, info);
        free(temp_for_enrollment);
        fgets(line, 100, raw_data);
    }
    
    return head;
}

char* get_table(node* list) {
    node* cur = list;
    char* table = malloc(sizeof(char) * 100000);
    strcat(table, "<table>");
    strcat(table, "<tr bgcolor = \"yellow\"><td>Course Number</td><td>Instructor</td><td width = \"80px\">Enrollment</td><td width = \"80px\">Course Quality</td><td width = \"80px\">Course Difficulty</td><td width = \"80px\">Instructor Quality</td></tr>");
    while(cur != NULL) {
        strcat(table, "<tr><td>");
        strcat(table, cur->value->course_num);
        strcat(table, "</td><td>");
        strcat(table, cur->value->instructor_name);
        strcat(table, "</td><td width = \"80px\">");
        char* buffer = malloc(sizeof(char) * 10);
        sprintf(buffer, "%d", cur->value->enrollment);
        strcat(table, buffer);
        strcat(table, "</td><td width = \"80px\">");
        sprintf(buffer, "%.2f", cur->value->course_quality);
        strcat(table, buffer);
        strcat(table, "</td><td width = \"80px\">");
        sprintf(buffer, "%.2f", cur->value->course_difficulty);
        strcat(table, buffer);
        strcat(table, "</td><td width = \"80px\">");
        sprintf(buffer, "%.2f", cur->value->instructor_quality);
        strcat(table, buffer);
        strcat(table, "</td></tr>");
        cur = cur->next;
    }
    strcat(table, "</table>\0");
    return table;
}

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
    
    /***********************
     Keep the server running
     ***********************/
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
            
            /******************************
             * extract request
             ******************************/
            
            
            // this is the message that we'll send back
            
            node* list = read_file();
            //            //    sort(list, cmp_instructor_name);
            //            list = sort(list, cmp_course_num);
            //    print_list(list);
            char* table = malloc(sizeof(char) * 100000);
            table = get_table(list);
            
            
            char* link = NULL;
            char* search = NULL;
            bool is_search = identify_request(request, "search=");
            if (is_search) {
                search = get_search_target(request);
                printf("search: %s\n", search);
                for(int i = 0; i < strlen(search); i++) {
                    toupper(search[i]);
                }
//                strupr(search);
                table = get_search_result(list, search);
            } else {
                link = get_link_target(request);
                printf("link: %s\n", link);
                if(strcmp(link, "by_course_num HTTP/1.1") == 0) {
                    list = sort(list, cmp_course_num);
                    table = get_table(list);
                } else if(strcmp(link, "by_instructor_name") == 0) {
                    list = sort(list, cmp_instructor_name);
                    table = get_table(list);
                } else if(strcmp(link, "by_enrollment") == 0) {
                    list = sort(list, compare_enrollment);
                    table = get_table(list);
                } else if(strcmp(link, "by_course_quality") == 0) {
                    list = sort(list, compare_course_quality);
                    table = get_table(list);
                } else if(strcmp(link, "by_course_difficulty") == 0) {
                    list = sort(list, compare_course_difficulty);
                    table = get_table(list);
                } else if(strcmp(link, "by_instructor_quality") == 0) {
                    list = sort(list, compare_instructor_quality);
                    table = get_table(list);
                }
            }
            
            
            
            char* reply = malloc(105000);
            strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Couese Evaluation Information</title></head>\n<body>\n<h3>Click the links below to get sorted results</h3>\n<ul>\n<li><a href=\"/by_course_num\">Sort By Course Number</a></li>\n<li><a href=\"/by_instructor_name\">Sort By Instructor Name</a></li>\n<li><a href=\"/by_enrollment\">Sort By Enrollment</a></li>\n<li><a href=\"/by_course_quality\">Sort By Course Quality</a></li>\n<li><a href=\"/by_course_difficulty\">Sort By Course Difficulty</a></li>\n<li><a href=\"/by_instructor_quality\">Sort By Instructor Quality</a></li>\n</ul>\n<h3>Enter a Course Number or an Instructor Name below to search aggregated result</h3>\n<form>\n<input type=\"text\" name=\"search\" placeholder=\"Search...\">\n<input type=\"submit\" value=\"Submit\">\n</form>\n");
        
            
            strcat(reply, table);
            strcat(reply, "</body></html>\0");
            
            
            
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

char* get_search_result(node* list, char* keyword) {
    char* table = malloc(sizeof(char) * 100000);
    
    node* cur = list;
    strcat(table, "<table>");
    strcat(table, "<tr bgcolor = \"yellow\"><td>Course Number</td><td>Instructor</td><td width = \"80px\">Enrollment</td><td width = \"80px\">Course Quality</td><td width = \"80px\">Course Difficulty</td><td width = \"80px\">Instructor Quality</td></tr>");
    
    // 要把keyword以大写的形式传进来
    while(cur != NULL) {
        if(strcmp(cur->value->course_num, keyword) == 0) {
            strcat(table, "<tr><td>");
            strcat(table, cur->value->course_num);
            strcat(table, "</td><td>");
            strcat(table, cur->value->instructor_name);
            strcat(table, "</td><td width = \"80px\">");
            char* buffer = malloc(sizeof(char) * 10);
            sprintf(buffer, "%d", cur->value->enrollment);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->course_quality);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->course_difficulty);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->instructor_quality);
            strcat(table, buffer);
            strcat(table, "</td></tr>");
        }
        if(strcmp(cur->value->instructor_name, keyword) == 0) {
            strcat(table, "<tr><td>");
            strcat(table, cur->value->course_num);
            strcat(table, "</td><td>");
            strcat(table, cur->value->instructor_name);
            strcat(table, "</td><td width = \"80px\">");
            char* buffer = malloc(sizeof(char) * 10);
            sprintf(buffer, "%d", cur->value->enrollment);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->course_quality);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->course_difficulty);
            strcat(table, buffer);
            strcat(table, "</td><td width = \"80px\">");
            sprintf(buffer, "%.2f", cur->value->instructor_quality);
            strcat(table, buffer);
            strcat(table, "</td></tr>");
        }
        cur = cur->next;
    }
    strcat(table, "</table>\0");
    return table;
}
