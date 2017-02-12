#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************************************

 This is an implementation a Linked List of "course_info"

***********************************************************/

typedef struct Node node;

struct Node {
    course_info* value;
    node* next;
};

int add_first (node** head, course_info* course) {
    if (head == NULL || course == NULL) return -1; /* ERROR */
    
    /* Linked List and course are both valid */
    node* new_node = malloc(sizeof(node));
    if (new_node == NULL) return 0; /* Failed to malloc space */
    
    /* node's value is a pointer that points to a course_info */
    new_node->value = malloc(sizeof(course_info*));
    if (new_node->value == NULL) return 0; /* Failed to malloc space */
    
    /* assign new head of the linked list */
    new_node->next = *head;
    *head = new_value;
    return 1;
}

int contains(node* head, char* target) {
    if (head == NULL || target == NULL) return -1; /* ERROR */
    
    /* Look into each node's value's course# & instructor_name */
    node* runner = head;
    while (runner != NULL) {
        char* number = runner->value->course_num;
        char* name = runner->value->instructor_name;
        if (strcmp(number, target) == 0 || strcmp(name, target)) return 0;
        runner = runner->next;
    }
    
    /* Not found */
    return 1;
}

int list_size(node* head) {
    int size = 0;
    node* runner = head;
    while (runner != NULL) {
        size++;
        runner = runner->next;
    }
    return size;
}

void sort_by_course_num(node* head) {
    
}

void sort_by_instructor_name(node* head) {
    
}

void sort_by_enrollment (node* head) {
    
}

void sort_by_course_quality (node* head) {
    
}

void sort_by_course_difficulty (node* head) {
    
}

void sort_by_instructor_quality (node* head) {
    
}
















int compare_int(void* a, void* b) {
    return *(int*)a - *(int*)b;
}

int compare_double(void* a, void* b) {
    return (int) *(double*)a - *(double*b);
}
