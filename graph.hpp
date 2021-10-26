#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <cassert>

#include "util.hpp"

using namespace std;

typedef vector<unsigned int> NodeV;
typedef vector<unsigned int> EdgeV;
typedef vector<float> WeightV;
typedef pair<vector<unsigned int>::iterator, vector<float>::iterator > NeighborsItr;


class Graph{
  public:
  Graph(char *sg_file, bool is_DG);

  // -----------------------------
  // members
  // -----------------------------
  bool is_DG;
  unsigned int num_nodes;
  unsigned int num_edges;
  NodeV nodes;
  EdgeV edges;
  WeightV weights;

  // members for schema graph
  NodeV d2s;
  unsigned int num_d_nodes;


  // -----------------------------
  // functions
  // -----------------------------
  void read_data_graph(char *input_file);
  void read_schema_graph(char *input_file);

  // -----------------------------
  // inline functions
  // -----------------------------
  
  // Return degree of node u
  inline unsigned int degree(unsigned int u){
    assert(u >= 0 && u < num_nodes);
    if(u == 0){
      return nodes[0];
    }else{
      return nodes[u] - nodes[u-1];
    }
  }
  
  // Return neighbor nodes and corresponding weights of node u
  inline NeighborsItr neighbors(unsigned int u){
    assert(u >= 0 && u < num_nodes);
    EdgeV::iterator it_e = edges.begin();
    WeightV::iterator it_w = weights.begin();
    
    if(u == 0){
      return make_pair(it_e, it_w);
    }else{
      return make_pair(it_e+nodes[u-1], it_w+nodes[u-1]);
    }
  }
  
};

#endif // GRAPH_H
