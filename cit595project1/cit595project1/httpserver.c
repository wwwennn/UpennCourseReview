/*
 This code primarily comes from
 http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
 and
 http://www.binarii.com/files/papers/c_sockets.txt
 */

// TODO: add a function to free linkedlist

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include "linkedlist.h"
#include "arraylist.h"
#include "functions.h"

typedef struct {
    int fd;
    char request[1024];
} server_info;

typedef struct {
    int port_number;
    char* input;
} server_params;

void* user_input(void* p) {
    char* input = (char*)p;
    printf("You can enter 'q' to terminate the sever\n");
    scanf("%s", input);
    return NULL;
}

void* handle_request(void* p) {
    server_info* info = (server_info*) p;
    /*********************************
     *   Prepare Prefix and Postfix
     *********************************/
    char* reply = malloc(105000);
    char* prefix = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Couese Evaluation Information</title></head>\n<body>\n<h3>Click the links below to get sorted results</h3>\n<ul>\n<li><a href=\"/by_course_num\">Sort By Course Number</a></li>\n<li><a href=\"/by_instructor_name\">Sort By Instructor Name</a></li>\n<li><a href=\"/by_enrollment\">Sort By Enrollment</a></li>\n<li><a href=\"/by_course_quality\">Sort By Course Quality</a></li>\n<li><a href=\"/by_course_difficulty\">Sort By Course Difficulty</a></li>\n<li><a href=\"/by_instructor_quality\">Sort By Instructor Quality</a></li>\n</ul>\n<h3>Average Instructor Quality: ";
    char* search_box = "</h3>\n<h3>Enter a Course Number or an Instructor Name below to search aggregated result</h3>\n<form>\n<input type=\"text\" name=\"search\" placeholder=\"Search...\">\n<input type=\"submit\" value=\"Submit\">\n</form>\n";
    char* postfix = "</body></html>\0";
    
    
    node* list = read_file();
    char* table = malloc(100000);
    table = get_table(list);
    
    
    char* link = malloc(25);
    link = get_sortkey(info->request);
    char* search = malloc(200);
    search = get_searchkey(info->request);
    
    if(strcmp(link, "by_course_num") == 0) {
//        printf("I'll be back in 3 seconds...\n\n");
//        sleep(3);
//        printf("I'm back!\n");
        list = sort(list, cmp_course_num);
        table = get_table(list);
    } else if(strcmp(link, "by_instructor_name") == 0) {
        list = sort(list, cmp_instructor_name);
        table = get_table(list);
    } else if(strcmp(link, "by_enrollment") == 0) {
        list = sort(list, compare_enrollment);
        table = get_table(list);
    } else if (strcmp(link, "by_course_quality") == 0) {
        list = sort(list, compare_course_quality);
        table = get_table(list);
    } else if (strcmp(link, "by_course_difficulty") == 0) {
        list = sort(list, compare_course_difficulty);
        table = get_table(list);
    } else if (strcmp(link, "by_instructor_quality") == 0) {
        list = sort(list, compare_instructor_quality);
        table = get_table(list);
    }
    

    if(strlen(search) > 0) {
        printf("The search key is %s\n", search);
        table = get_search_result(list, search);
    }
    
    
    double average_quality = average_instructor_quality(list);
    char number[100];
    sprintf(number, "%.2f", average_quality);
    strcpy(reply, prefix);
    strcat(reply, number);
    strcat(reply, search_box);
    strcat(reply, table);
    strcat(reply, postfix);
    
    
    send(info->fd, reply, strlen(reply), 0);
    free(link);
    free(table);
    free(search);
    free(reply);
    close(info->fd);
    printf("Server closed connection\n");
    return NULL;
}

void* start_server(void* p)
{
    
//    int* PORT_NUMBER = (int*) p;
    server_params* params = (server_params*)p;
    /******************************************************************/
    struct sockaddr_in server_addr,client_addr;
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }
    int temp;
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
        perror("Setsockopt");
        exit(1);
    }
    server_addr.sin_port = htons(params->port_number);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero),8);
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Unable to bind");
        exit(1);
    }
    if (listen(sock, 1) == -1) {
        perror("Listen");
        exit(1);
    }
    printf("\nServer configured to listen on port %d\n", params->port_number);
    fflush(stdout);
    arraylist* thread_list = al_initialize(5);
    
    
    /*******************************************************************/
    
    while(1) {
        int sin_size = sizeof(struct sockaddr_in);
        int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
        if (fd != -1) {
            /********************************
             *        Getting Request
             ********************************/
            printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            
            server_info* info = malloc(sizeof(server_info));
            info->fd = fd;
            
            int bytes_received = recv(fd,info->request,1024,0);
            info->request[bytes_received] = '\0';
            printf("This is the incoming request:\n%s\n", info->request);
            
            
            pthread_t* t = malloc(sizeof(pthread_t));
            pthread_create(t, NULL, handle_request, info);
            al_add(thread_list, t);
            
//            /*********************************
//             *   Prepare Prefix and Postfix
//             *********************************/
//            char* reply = malloc(105000);
//            char* prefix = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Couese Evaluation Information</title></head>\n<body>\n<h3>Click the links below to get sorted results</h3>\n<ul>\n<li><a href=\"/by_course_num\">Sort By Course Number</a></li>\n<li><a href=\"/by_instructor_name\">Sort By Instructor Name</a></li>\n<li><a href=\"/by_enrollment\">Sort By Enrollment</a></li>\n<li><a href=\"/by_course_quality\">Sort By Course Quality</a></li>\n<li><a href=\"/by_course_difficulty\">Sort By Course Difficulty</a></li>\n<li><a href=\"/by_instructor_quality\">Sort By Instructor Quality</a></li>\n</ul>\n<h3>Enter a Course Number or an Instructor Name below to search aggregated result</h3>\n<form>\n<input type=\"text\" name=\"search\" placeholder=\"Search...\">\n<input type=\"submit\" value=\"Submit\">\n</form>\n";
//            char* postfix = "</body></html>\0";
            
            
            /*********************************
             *     Getting Table Contents
             *********************************/
//            node* list = read_file();
//            char* table = malloc(100000);
//            table = get_table(list);
//            
//            
//            char* link = malloc(25);
//            link = get_sortkey(request);
//            char* search = malloc(200);
//            search = get_searchkey(request);
//            
//            if(strcmp(link, "by_course_num") == 0) {
//                list = sort(list, cmp_course_num);
//                table = get_table(list);
//            } else if(strcmp(link, "by_instructor_name") == 0) {
//                list = sort(list, cmp_instructor_name);
//                table = get_table(list);
//            } else if(strcmp(link, "by_enrollment") == 0) {
//                list = sort(list, compare_enrollment);
//                table = get_table(list);
//            } else if (strcmp(link, "by_course_quality") == 0) {
//                list = sort(list, compare_course_quality);
//                table = get_table(list);
//            } else if (strcmp(link, "by_course_difficulty") == 0) {
//                list = sort(list, compare_course_difficulty);
//                table = get_table(list);
//            } else if (strcmp(link, "by_instructor_quality") == 0) {
//                list = sort(list, compare_instructor_quality);
//                table = get_table(list);
//            }
//            
////            table = get_search_result(list, search);
//            if(strlen(search) > 0) {
//                printf("The search key is %s\n", search);
//                table = get_search_result(list, search);
//            }
//
//            
////            printf("The search key is %s\n", search);
//            
//            strcpy(reply, prefix);
//            strcat(reply, table);
//            strcat(reply, postfix);
//            
//            /*******************************
//             *   FIND START OF HTTP/1.1
//             *******************************/
////            for (int k = 0; k < strlen(reply); k++) {
////                char substr[9];
////                strncpy(substr, reply, 8);
////                if (strcmp(substr, "HTTP/1.1") == 0) break;
////                reply++;
////            }
//            
//            send(fd, reply, strlen(reply), 0);
//            free(link);
//            free(reply);
//            close(fd);
//            printf("Server closed connection\n");
              

            if(strcmp(params->input, "q") == 0) break;
        }
    }
    
    close(sock);
    printf("Server shutting down\n");
    
    for(int i = 0; i < thread_list->size; i++) {
        void* r;
        pthread_join(*(*(thread_list->values) + i), &r);
    }
    
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
    
    char* input = malloc(sizeof(char) * 2);
    server_params* params = malloc(sizeof(server_params));
    params->port_number = port_number;
    params->input = input;
    pthread_t t1, t2;
    pthread_create(&t1, NULL, user_input, input);
    pthread_create(&t2, NULL, start_server, params);
    void* r1;
    void* r2;
    pthread_join(t1, &r1);
    pthread_join(t2, &r2);
    if(input != NULL) free(input);
    if(params != NULL) free(params);
//    start_server(&port_number);
}

