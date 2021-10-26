#include "graph.hpp"

using namespace std;

Graph::Graph(char *input_file, bool is_DG){
  this->is_DG = is_DG;
  
  if(is_DG){
    read_data_graph(input_file);
  }else{
    read_schema_graph(input_file);
  }
}

// -----------------------------
// Read Data Graph 
// -----------------------------
void
Graph::read_data_graph(char *input_file){
  ifstream finput;
  finput.open(input_file, fstream::in | fstream::binary);

  finput.read((char *)(&num_nodes), 4);
  finput.read((char *)(&num_edges), 4);

  nodes.resize(num_nodes);
  edges.resize(num_edges);
  weights.resize(num_edges);
  
  finput.read((char *)(&nodes[0]), num_nodes*4);
  finput.read((char *)(&edges[0]), num_edges*4);
  finput.read((char *)(&weights[0]), num_edges*4);

  finput.close();
}

// -----------------------------
// Read Schema Graph 
// -----------------------------
void
Graph::read_schema_graph(char *input_file){
  ifstream finput;
  finput.open(input_file, fstream::in | fstream::binary);

  finput.read((char *)(&num_nodes), 4);
  finput.read((char *)(&num_edges), 4);
  finput.read((char *)(&num_d_nodes), 4);

  nodes.resize(num_nodes);
  edges.resize(num_edges);
  weights.resize(num_edges);
  d2s.resize(num_d_nodes);
  
  finput.read((char *)(&nodes[0]), num_nodes*4);
  finput.read((char *)(&edges[0]), num_edges*4);
  finput.read((char *)(&weights[0]), num_edges*4);
  finput.read((char *)(&d2s[0]), num_d_nodes*4);

  finput.close();
}
