//
// Created by Twometer on 13/07/2020.
//

#include <stdlib.h>

#include "vector.h"

vector *vector_create(int size) {
    vector *vec = malloc(sizeof(vector));
    vec->capacity = size;
    vec->size = 0;
    vec->nodes = malloc(sizeof(vector_node*) * vec->capacity);
    return vec;
}

void vector_append(vector *vec, int node_type, void *data) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->nodes = realloc(vec->nodes, sizeof(vector_node*) * vec->capacity);
    }
    vector_node *node = malloc(sizeof(vector_node));
    node->node_type = node_type;
    node->node_ptr = data;
    vec->nodes[vec->size] = node;
    vec->size++;
}

void vector_destroy(vector *vec) {
    for (int i = 0; i < vec->size; i++)
        free(vec->nodes[i]);
    free(vec->nodes);
    free(vec);
}