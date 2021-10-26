#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

#include "graph.hpp"

using namespace std;

char *sg_file = NULL;
char *dg_file = NULL;


void
usage(char *prog_name, const char *more){
  cerr << more;
  cerr << "usage: " << prog_name << "-s schema_graph_file -d data_graph_file" << endl << endl;
  exit(0);
}

void
parse_args(int argc, char **argv) {
  
  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
      case 's':
	if(i==argc-1)
	  usage(argv[0], "Schema graph is missing\n");
	sg_file = argv[i+1];
	i++;
	break;
      case 'd':
	if(i==argc-1)
	  usage(argv[0], "Data graph is missing\n");
	dg_file = argv[i+1];
	i++;
	break;
      default:
	usage(argv[0], "Unknown option\n");
      }
    }
  }

  if(sg_file==NULL && dg_file==NULL){
    usage(argv[0], "Input files are missing\n ");
  }
}



int
main(int argc, char **argv){
  parse_args(argc, argv);
  SGraph sg(sg_file);
  DGraph dg(dg_file);
  sg.dump_sg(&(dg.nodes), &(dg.edges), &(dg.labels));
  
  /*
  ifstream finput;
  finput.open("output_sg.bin", fstream::in | fstream::binary);

  unsigned int nb_nodes = 0;
  unsigned int nb_edges = 0;
  unsigned int nb_d_nodes = 0;
  vector<unsigned int> nodes;
  vector<unsigned int> edges;
  vector<float> weights;
  vector<unsigned int> d2s;
  
  finput.read((char *)(&nb_nodes), 4);
  finput.read((char *)(&nb_edges), 4);
  finput.read((char *)(&nb_d_nodes), 4);

  nodes.resize(nb_nodes);
  edges.resize(nb_edges);
  weights.resize(nb_edges);
  d2s.resize(nb_d_nodes);

  finput.read((char *)(&nodes[0]), nb_nodes*4);
  finput.read((char *)(&edges[0]), nb_edges*4);
  finput.read((char *)(&weights[0]), nb_edges*4);
  finput.read((char *)(&d2s[0]), nb_d_nodes*4);

  finput.close();

  debug(nb_nodes);
  debug(nb_edges);
  debug(nb_d_nodes);

  for(unsigned int i=0; i<nodes.size(); ++i){
    unsigned int base = 0;
    if(i!=0)
      base = nodes[i-1];

    for(unsigned int j=base; j<nodes[i]; ++j){
      cout << i << "\t" << edges[j] << "\t" << weights[j] << endl;
    }
  }

  debug(d2s);
  */
  
}
