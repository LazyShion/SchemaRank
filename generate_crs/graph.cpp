#include "graph.hpp"

using namespace std;

/*******************************
         Schema Graph
*******************************/
SGraph::SGraph(char *sg_file){
  ifstream finput;  
  string src, dst;
  float weight;
  
  id = 0;
  num_edges = 0;

  finput.open(sg_file, fstream::in);
  while(!finput.eof()){    
    finput >> src >> dst >> weight;

    if(label2id.count(src) == 0){
      label2id[src] = id;
      ++id;
    }
    
    if(label2id.count(dst) == 0){
      label2id[dst] = id;
      ++id;
    }
    
    tuple<unsigned int, float, string> p = make_tuple(label2id[src], weight, dst+src);
    if(g.count(label2id[dst]) == 0){
      list<tuple<unsigned int, float, string> > l;
      g[label2id[dst]] = l;
    }
    
    if(find(g[label2id[dst]].begin(), g[label2id[dst]].end(), p) == g[label2id[dst]].end()){
      g[label2id[dst]].push_back(p);
      ++num_edges;
    }

    //cout << num_edges << endl;
    
    if(l2pair.count(dst+src) == 0){
      l2pair[dst+src] = make_pair(label2id[dst], label2id[src]);
    }

  }
  
  finput.close();

}

// output crs of schema graph in a binary format
void
SGraph::dump_sg(NodeV *d_nodes, EdgeV *d_edges, LabelV *d_labels){
  unsigned int num_d_nodes = d_nodes->size();
  nodes.resize(id);
  edges.resize(num_edges);
  weights.resize(num_edges);
  did2sid.resize(num_d_nodes, 0);
  
  for(unsigned int i=0, end=nodes.size(); i<end; ++i){
    unsigned int base = (i == 0)? 0 : nodes[i-1];
    unsigned int degree = (g.count(i) == 0)? 0 : g[i].size();
    nodes[i] = degree + base;

    if(degree > 0){
      Neighbors::iterator it = g[i].begin();
      for(unsigned int j=base; j<base+degree; ++j){
	edges[j] = get<0>(*it); 
	weights[j] = get<1>(*it); 
	++it;
      }
    }
  }

  unsigned int base = 0;
  for(unsigned int d_dst=0, end=num_d_nodes; d_dst<end; ++d_dst){
    unsigned int d_deg = (d_dst == 0)? d_nodes->at(d_dst) : d_nodes->at(d_dst) - d_nodes->at(d_dst-1);
    
    for(unsigned int i = 0; i<d_deg; ++i){
      string d_label = d_labels->at(base);
      unsigned int d_src = d_edges->at(base);
      pair<unsigned int, unsigned int> lp = l2pair[d_label];
      
      did2sid[d_src] = lp.first;
      did2sid[d_dst] = lp.second;
      ++base;
    }
  }

  //cout << id << "\t" << num_edges << "\t" << num_d_nodes << endl;
  
  ofstream foutput;
  foutput.open("output_sg.bin", fstream::out | fstream::binary);
  
  // output size of NodeV, and EdgeV
  foutput.write((char *)(&id), 4);
  foutput.write((char *)(&num_edges), 4);
  foutput.write((char *)(&num_d_nodes), 4);

  // output NodeV
  for(unsigned int i=0; i<id; ++i){
    foutput.write((char *)(&nodes[i]), 4);
  }

  // output EdgeV
  for(unsigned int i=0; i<num_edges; ++i){
    foutput.write((char *)(&edges[i]), 4);
  }

  // output WeightV
  for(unsigned int i=0; i<num_edges; ++i){
    foutput.write((char *)(&weights[i]), 4);
  }

  // output did2sid
  for(unsigned int i=0; i<num_d_nodes; ++i){
    foutput.write((char *)(&did2sid[i]), 4);
  }

  foutput.close(); 

}

/*******************************
          Data Graph
*******************************/
DGraph::DGraph(char *dg_file){
  ifstream finput;  
  unsigned int src, dst;
  float weight;
  string label;
  max_node=0, num_edges=0;

  finput.open(dg_file, fstream::in);
  
  while(!finput.eof()){    
    finput >> src >> dst >> weight >> label;

    if(dst > max_node){
      max_node = dst;
    }

    tuple<unsigned int, float, string> p = make_tuple(src, weight, label);
    if(g.count(dst) == 0){
      list<tuple<unsigned int, float, string> > l;
      g[dst] = l;
    }
    if(find(g[dst].begin(), g[dst].end(), p) == g[dst].end()){
      g[dst].push_back(p);
      ++num_edges;
    }
  }
  
  finput.close();  
  
  max_node += 1;
  nodes.resize(max_node);
  edges.resize(num_edges);
  weights.resize(num_edges);  
  labels.resize(num_edges);

  for(unsigned int i=0, end=max_node; i<end; ++i){
    unsigned int base = (i == 0) ? 0 : nodes[i-1];
    unsigned int degree = (g.count(i) == 0) ? 0 : g[i].size();
    nodes[i] = degree + base;
    
    if(degree > 0){
      Neighbors::iterator it = g[i].begin();
      for(unsigned int j=base; j<nodes[i]; ++j){
	edges[j] = get<0>(*it);
	weights[j] = get<1>(*it);
	labels[j] = get<2>(*it);
	++it;
      }
    }
    
  }

  ofstream foutput;
  foutput.open("output_dg.bin", fstream::out | fstream::binary);
  
  // output size of NodeV, and EdgeV
  foutput.write((char *)(&max_node), 4);
  foutput.write((char *)(&num_edges), 4);

  // output NodeV
  for(unsigned int i=0; i<max_node; ++i){
    foutput.write((char *)(&nodes[i]), 4);
  }

  // output EdgeV
  for(unsigned int i=0; i<num_edges; ++i){
    foutput.write((char *)(&edges[i]), 4);
  }

  // output WeightV
  for(unsigned int i=0; i<num_edges; ++i){
    foutput.write((char *)(&weights[i]), 4);
  }
  
  foutput.close();
  
}

