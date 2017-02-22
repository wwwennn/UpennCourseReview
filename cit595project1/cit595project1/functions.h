#include <stdbool.h>

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
    /* change spaces into +s */
    for (int i = 0; i < strlen(substr); i++) {
        if (substr[i] == ' ') *(substr + i) = '+';
    }
    return NULL;
}

char* get_searchkey(char* request) {
    int len = strlen(request);
    int j = 0;
    char* search = malloc(200);
    for(int i = 0; i < len; i++) {
        if(request[i] == '?') {
            while(i < len && request[i] != '=') i++;
            i++;
            while(i < len && request[i] != ' ') {
                search[j] = request[i];
                j++; i++;
            }
            search[j] = '\0';
            break;
        }
    }
    for(int i = 0; i < strlen(search); i++) {
        if(search[i] == '+') search[i] = ' ';
        toupper(search[i]);
    }
    return search;
}

char* get_sortkey(char* request) {
    int len = strlen(request);
    int i = 0, j = 0;
    char* link = malloc(sizeof(char) * 25);
    while(i < len) {
        if(request[i] == '/') {
            i++;
            while(i < len && request[i] != ' ') {
                link[j] = request[i];
                j++; i++;
            }
            link[j] = '\0';
            return link;
        }
        i++;
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

/**
 * This method reads in a text file line by line
 * and store information from 6 different fields in a course_info struct
 * all course_info(s) are then stored into a singly linked list
 */
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
    strcpy(table, "<table>");
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

char* get_search_result(node* list, char* keyword) {
    char* table = malloc(sizeof(char) * 100000);
    
    node* cur = list;
    strcpy(table, "<table>");
    strcat(table, "<tr bgcolor = \"yellow\"><td>Course Number</td><td>Instructor</td><td width = \"80px\">Enrollment</td><td width = \"80px\">Course Quality</td><td width = \"80px\">Course Difficulty</td><td width = \"80px\">Instructor Quality</td></tr>");
    
//    printf("\nEnter\n");
    // 要把keyword以大写的形式传进来
    while(cur != NULL) {
//        printf("%s\n", cur->value->course_num);
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
//        printf("%s\n", cur->value->instructor_name);
        if(strcmp(cur->value->instructor_name + 1, keyword) == 0) {
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

