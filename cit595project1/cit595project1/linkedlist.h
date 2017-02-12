#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************************************

 This is an implementation a Linked List of "course_info"

***********************************************************/


// define the struct of course info
typedef struct {
    char* course_num;
    char* instructor_name;
    int enrollment;
    double course_quality;
    double course_difficulty;
    double instructor_quality;
} course_info;

// define the node of linked list
typedef struct Node node;

struct Node {
    course_info* value;
    node* next;
};

// define the compare function
int cmp_course_num(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return strcmp(s_node->value->course_num, t_node->value->course_num);
}

int cmp_instructor_name(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return strcmp(s_node->value->instructor_name, t_node->value->instructor_name);
}

int compare_enrollment(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return s_node->value->enrollment - t_node->value->enrollment;
}

int compare_course_quality(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return (int)(s_node->value->course_quality - t_node->value->course_quality);
}

int compare_course_difficulty(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return (int)(s_node->value->course_difficulty - t_node->value->course_difficulty);
}

int compare_instructor_quality(void* s, void* t) {
    node* s_node = (node*)s;
    node* t_node = (node*)t;
    
    return (int)(s_node->value->instructor_quality - t_node->value->instructor_quality);
}


int add_first (node** head, course_info* course) {
    if (head == NULL || course == NULL) return -1; /* ERROR */
    
    /* Linked List and course are both valid */
    node* new_node = malloc(sizeof(node));
    if (new_node == NULL) return 0; /* Failed to malloc space */
//    new_node->next = NULL;
    /* node's value is a pointer that points to a course_info */
    new_node->value = course;
    
    /* assign new head of the linked list */
    new_node->next = *head;
    *head = new_node;
    return 1;
}

int contains(node* head, char* target) {
    if (head == NULL || target == NULL) return -1; /* ERROR */
    
    /* Look into each node's value's course# & instructor_name */
    node* runner = head;
    while (runner != NULL) {
        char* number = runner->value->course_num;
        char* name = runner->value->instructor_name;
        if (strcmp(number, target) == 0 || strcmp(name, target) == 0) return 0;
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

void print_list(node* head) {
    if (head == NULL) printf("The Linked List is empty.\n");
    else {
        printf("| Course # | Instructor Name | Enrollment | Course Quality | Course Difficulty | Instrucror Quality |\n");
        node* runner = head;
        //        int count = 0;
        while (runner != NULL) {
            //            count++;
            //            printf("The number is %d\n", count);
            printf("%s  %s  %d  %.2f  %.2f  %.2f\n",
                   runner->value->course_num,
                   runner->value->instructor_name,
                   runner->value->enrollment,
                   runner->value->course_quality,
                   runner->value->course_difficulty,
                   runner->value->instructor_quality);
            runner = runner->next;
        }
    }
}

node* sort(node* head, int (*compare)(void*, void*)) {
    if(head == NULL || head->next == NULL) return head;
    
    node* head1 = head;
    node* head2 = head->next;
    node* cur1 = head1;
    node* cur2 = head2;
    node* cur = head->next->next;
    
    while(cur != NULL && cur->next != NULL) {
        cur1->next = cur;
        cur2->next = cur->next;
        cur = cur->next->next;
        cur1 = cur1->next;
        cur2 = cur2->next;
    }
    
    if(cur != NULL){
        cur1->next = cur;
        cur1 = cur1->next;
    }
    
    cur1->next = NULL;
    cur2->next = NULL;
    
    head1 = sort(head1, compare);
    head2 = sort(head2, compare);
    
    cur1 = head1;
    cur2 = head2;
    
    if(compare(cur1, cur2) < 0) {
        head = cur1;
        cur1 = cur1->next;
    } else {
        head = cur2;
        cur2 = cur2->next;
    }
    
    cur = head;
    
    while(cur1 != NULL && cur2 != NULL) {
        if(compare(cur1, cur2) < 0) {
            cur->next = cur1;
            cur1 = cur1->next;
        } else {
            cur->next = cur2;
            cur2 = cur2->next;
        }
        cur = cur->next;
    }
    
    if(cur1 != NULL) cur->next = cur1;
    if(cur2 != NULL) cur->next = cur2;
    
    return head;
}



