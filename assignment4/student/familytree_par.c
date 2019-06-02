#include "familytree.h"
#include <omp.h>

#define T 4

int par_traverse(tree *node, int numThreads) {
    if (node == NULL) return 0;
    
    numThreads /= 2;
    int father_iq, mother_iq;
    
#pragma omp task shared(father_iq) firstprivate(numThreads) final(numThreads < T)
    father_iq = par_traverse(node->father, numThreads);
//#pragma omp task shared(mother_iq) firstprivate(numThreads) final(numThreads < T)
    mother_iq = par_traverse(node->mother, numThreads);
    
#pragma omp taskwait
    node->IQ = compute_IQ(node->data, father_iq, mother_iq);
    genius[node->id] = node->IQ;

    return node->IQ;
}

int traverse(tree *node, int numThreads){
    omp_set_num_threads(numThreads);
#pragma omp parallel 
    {
#pragma omp single
        par_traverse(node, numThreads);
    }
    
    return node->IQ;
}
