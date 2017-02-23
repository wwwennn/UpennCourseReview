#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct ArrayList {
    pthread_t** values;
    size_t capacity;
    size_t size;
} arraylist;

arraylist* al_initialize(size_t capacity) {
    arraylist* new_arraylist = (arraylist*) malloc(sizeof(arraylist));
    new_arraylist->values = malloc(sizeof(pthread_t*) * (capacity + 1));
    if (new_arraylist == NULL) return NULL;
    new_arraylist->capacity = capacity;
    new_arraylist->size = 0;
    return new_arraylist;
}

unsigned int al_add(arraylist* al, pthread_t* value) {
    if (al == NULL) return 0;
    if (al->size < al->capacity) {
        al->values[al->size] = value;
        al->size++;
    } else {
        pthread_t** new_values = malloc(sizeof(pthread_t*) * (al->capacity * 2 + 1));
        int i = 0;
        while (i < al->size) {
            new_values[i] = al->values[i];
            i++;
        }
        new_values[i] = value;
        al->size++;
        al->capacity = al->capacity * 2;
        int j = 0;
        while (j < al->size) {
            free(al->values[j]);
            j++;
        }
        free(al->values);
        al->size++;
        al->capacity = al->capacity * 2;
        al->values = new_values;
    }
    return 1;
}

void al_free(arraylist* al) {
    if (al != NULL) {
        int i = 0;
        while (i < al->size) {
            free(al->values[i]);
        }
        free(al->values);
        free(al);
    }
}
