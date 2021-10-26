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
#include <tuple>

using namespace std;

typedef vector<unsigned int> NodeV;
typedef vector<unsigned int> EdgeV;
typedef vector<float> WeightV;
typedef vector<string> LabelV;
typedef unordered_map<unsigned int, list<tuple<unsigned int, float, string> > > Graph;
typedef list<tuple<unsigned int, float, string> > Neighbors;

// macro for debug
#define debug(var) do{cout << #var << ": "; view(var); }while(0)
template<typename T> void view(T e){cout << e << endl;}
template<typename T> void view(const vector<T>& v){for(const auto& e:v){cout << e << " ";} cout << endl;}
template<typename T> void view(const vector<vector<T>>& vv){for(const auto& v:vv){view(v);}}

class SGraph {
 public:
  SGraph(char *sg_file);
  
  // members
  NodeV nodes;
  EdgeV edges;
  WeightV weights;
  Graph g;
  unordered_map<string, unsigned int> label2id;
  unordered_map<string, pair<unsigned int, unsigned int> > l2pair;
  NodeV did2sid;
  unsigned int num_edges;
  unsigned int id;

  // functions
  void dump_sg(NodeV *d_nodes, EdgeV *d_edges, LabelV *d_labels);
  
};

class DGraph{
 public:
  DGraph(char *dg_file);
  
  // members
  unsigned int max_node;
  unsigned int num_edges;
  NodeV nodes;
  EdgeV edges;
  WeightV weights;
  LabelV labels;
  Graph g;
  
};
