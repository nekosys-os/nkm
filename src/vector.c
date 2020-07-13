//
// Created by Twometer on 13/07/2020.
//

#include <stdlib.h>

#include "vector.h"

vector *vector_create(int size) {
    vector *vec = malloc(sizeof(vector));
    vec->size = size;
    vec->offset = 0;
    vec->nodes = malloc(sizeof(vector_node*) * vec->size);
    return vec;
}

void vector_append(vector *vec, int node_type, void *data) {
    if (vec->offset >= vec->size - 1) {
        vec->size *= 2;
        vec->nodes = realloc(vec->nodes, sizeof(vector_node*) * vec->size);
    }
    vector_node *node = malloc(sizeof(vector_node));
    node->node_type = node_type;
    node->node_ptr = data;
    vec->nodes[vec->offset] = node;
}

void vector_destroy(vector *vec) {
    for (int i = 0; i < vec->size; i++)
        free(vec->nodes[i]);
    free(vec->nodes);
    free(vec);
}