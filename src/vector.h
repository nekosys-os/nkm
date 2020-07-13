//
// Created by Twometer on 13/07/2020.
//

#ifndef NKM_VECTOR_H
#define NKM_VECTOR_H

typedef struct {
    int node_type;
    void *node_ptr;
} vector_node;

typedef struct {
    int size;
    int offset;
    vector_node** nodes;
} vector;

vector *vector_create(int size);

void vector_append(vector *vec, int node_type, void *data);

void vector_destroy(vector *vec);


#endif //NKM_VECTOR_H
