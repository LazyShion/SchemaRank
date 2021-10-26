#ifndef SCHEMARANK_H
#define SCHEMARANK_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <cassert>

#include "graph.hpp"

using namespace std;

typedef vector<unsigned int> NodeV;
typedef vector<unsigned int> EdgeV;
typedef vector<float> WeightV;
typedef vector<float> V;
typedef pair<vector<unsigned int>::iterator, vector<float>::iterator > NeighborsItr;
typedef pair<float, unsigned int> Pair;

// Pair comparison rule
static bool pair_compare(const Pair &first_pair, const Pair &second_pair){
  return first_pair.first > second_pair.first;
}


class SchemaRank{
public:
  SchemaRank(Graph *g, float a, unsigned int t, unsigned int k, float s, char* query_file); 

  // -----------------------------
  // members
  // -----------------------------
  Graph *g;
  char *query_file;
  
  // vectors
  V vector_r; // score vector
  V tmp_vector_r; // temporary score vector
  V vector_Ar; // random_walk vector
  V tmp_vector_Ar; // temporary random_walk vector
  V vector_q; // query vector
  V vector_lower; // lower bound vector
  V tmp_vector_lower; // temporary lower bound vector
  V vector_upper; // upper bound vector
  V vector_maxA; // \overline{A}_i vector
  V vector_b; 
  vector<bool> pruned; // pruning flag vector
  list<Pair> results; // topk answer nodes
  NodeV hist_attr; // attribute histgram
  
  // parameter variables
  float alpha; // dumping factor alpha
  float reverse_alpha; // 1-alpha
  float div_alpha; // alpha^{t+1}/(1-alpha)
  float pow_alpha; // alpha^{t}
  unsigned int T; // maximum # of iterations
  unsigned int K; // # of answer nodes
  float l2norm; // l2norm
  float stop_condition; // stop_condition

  // inner state variables
  unsigned int t; // # of performed iterations
  unsigned int num_nonpruned_nodes; // # of candidate nodes
  unsigned int num_computed_nodes; // # of computed nodes
  float topk_lower;
  NodeV computed_nodes; // logs for # of computed nodes in each iteration
  V computed_norms; // logs for sizes of computed norms in each iteration


  // -----------------------------
  // functions
  // -----------------------------
  void init_pr_vectors();
  void init_query_vector();
  void init_ul_vectors();
  void one_iteration();
  void one_iteration_sg();
  void run();
  void get_topk_nodes();
  void set_values_to_dg(V *vector_b, NodeV *d2s);

  // -----------------------------
  // inline functions
  // -----------------------------

  // Return inner product Ar
  inline float inner_product(unsigned int u){
    assert(u>=0 && u<g->num_nodes);
    NeighborsItr Ni = g->neighbors(u);
    unsigned int degree = g->degree(u);
    float new_p = 0.0;
    
    for(unsigned int i=0; i<degree; ++i){
      new_p += vector_r[*(Ni.first+i)] * (*(Ni.second+i));
    }
    return new_p;
  }

  
};

#endif // SCHEMARANK_H
