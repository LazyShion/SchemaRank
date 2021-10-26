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
#include "schemarank.hpp"

using namespace std;

char *dg_file = NULL;
char *sg_file = NULL;
char *query_file = NULL;
char *log_file = NULL;
unsigned int T = 1000;
unsigned int K = 10;
float dumping_factor = 0.5;
float stop_condition = 0.000000001; // 10^{-9}
bool results = false;
bool display = false;
bool log_flag = false;

void
usage(char *prog_name, const char *more){
  cerr << endl;
  cerr << more;
  cerr << "usage: " << prog_name << " -i sg_file dg_file -q query_file [-a dumping_factor] [-t num_of_iterations] [-k num_of_results] [-T threshold] [-l log_file] [-r] [-d] [-h]" << endl;
  cerr << " -i sg_file dg_file: binary files containing schema and data graph." << endl;
  cerr << " -q query_file: a query file." << endl;
  cerr << " -a dumping_factor: a dumping factor value between 0 and 1." << endl;
  cerr << " -t num_of_iterations: a number of maximum iterations (> 0)." << endl;
  cerr << " -k num_of_results: a number of top-K nodes (> 0)." << endl;
  cerr << " -l log_file: output log files." << endl;
  cerr << " -r: display top-k results." << endl;
  cerr << " -d: display running time." << endl;
  cerr << " -h: display help." << endl;
  cout << endl;
  exit(0);
}

void
parse_args(int argc, char **argv) {
  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
      case 'h':
	usage(argv[0], "Help menu\n");
	break;
      case 'r':
	results = true;
	break;
      case 'd':
	display = true;
	break;
      case 'i':
	if(i==argc-2)
	  usage(argv[0], "Input file is missing\n");
	sg_file = argv[i+1];
	dg_file = argv[i+2];
	i+=2;
	break;
      case 'q':
	if(i==argc-1)
	  usage(argv[0], "Query file is missing\n");
	query_file = argv[i+1];
	i++;
	break;
      case 'l':
	if(i==argc-1)
	  usage(argv[0], "Log file is missing\n");
	log_file = argv[i+1];
	log_flag = true;
	i++;
	break;
      case 't':
	if(i==argc-1)
	  usage(argv[0], "Input value is missing\n");
	T = atoi(argv[i+1]);
	i++;
	break;
      case 'a':
	if(i==argc-1)
	  usage(argv[0], "Input value is missing\n");
	dumping_factor = atof(argv[i+1]);
	i++;
	break;
      case 'k':
	if(i==argc-1)
	  usage(argv[0], "Input value is missing\n");
	K = atoi(argv[i+1]);
	i++;
	break;
      default:
	usage(argv[0], "Unknown option\n");
      }
    }
  }

  if(sg_file==NULL || dg_file==NULL || query_file==NULL){
    usage(argv[0], "Input files are missing\n ");
  }
}


int
main(int argc, char **argv){
  parse_args(argc, argv);
  assert(dumping_factor>=0 && dumping_factor<=1);

  // -----------------------------------------------------------------------------------
  // Main procedures
  // -----------------------------------------------------------------------------------
  Graph sg(sg_file, false); // generate a schema graph
  Graph dg(dg_file, true); // generate a data graph
  SchemaRank rank_sg(&sg, dumping_factor, T, K, stop_condition, query_file);
  SchemaRank rank(&dg, dumping_factor, T, K, stop_condition, query_file);

  Chrono_t start_time = get_time(); // start time
  rank_sg.run();
  rank.set_values_to_dg(&(rank_sg.vector_r), &(sg.d2s));
  rank.run();
  Chrono_t end_time = get_time(); // end time


  // -----------------------------------------------------------------------------------
  // Execute option commands
  // -----------------------------------------------------------------------------------
  // Display top-k results
  if(results){
    list<Pair>::iterator it = (rank.results).begin(), end=(rank.results).end();
    while(it!=end){
      cout << it->second << "\t" << it->first << endl;
      ++it;
    }
    cout << endl;
  }
  
  // Display some statistics
  if(display){
    cout << "# of nodes: " << dg.num_nodes << endl;
    cout << "# of edges: " << dg.num_edges << endl;
    cout << "# of iterations: " << rank.t+1 << endl;
    cout << "Time duration: " << get_time_sec(&start_time, &end_time) << " seconds" << endl << endl;
  }
  
  // Output a log file
  if(log_flag){
    assert(log_file != NULL);
    
    // generate log data
    string graph_name(dg_file);
    string query_name(query_file);
    string param = 
      "{alpha:"+to_string(dumping_factor)+", "+
      "K:"+to_string(K)+", "+
      "T:"+to_string(T)+"}";
    
    string topk_nodes = "{";
    list<Pair>::iterator it = (rank.results).begin(), end=(rank.results).end();
    unsigned int count = 0;
    while(it!=end){
      topk_nodes += to_string(it->second)+":"+to_string(it->first);
      ++it;
      ++count;
      if(count != (rank.results).size()){
	topk_nodes += ", ";
      }
    }
    topk_nodes += "}";

    string comp_nodes = "{";
    count = 0;
    for(unsigned int i=0; i<=rank.t; i++){
      comp_nodes += to_string(rank.computed_nodes[i]);
      if(i != rank.t){
	comp_nodes += ",";
      }
    }
    comp_nodes += "}";

    string comp_norms = "{";
    count = 0;
    for(unsigned int i=0; i<=rank.t; i++){
      comp_norms += to_string(rank.computed_norms[i]);
      if(i != (rank.t)){
	comp_norms += ",";
      }
    }
    comp_norms += "}";

    // add data to log
    add_logs("param", param);
    add_logs("topk_nodes", topk_nodes);
    add_logs("computed_nodes", comp_nodes);
    add_logs("computed_norms", comp_norms);
    add_logs("graph", "\""+graph_name+"\"");
    add_logs("query", "\""+query_name+"\"");
    add_logs("num_nodes", to_string(dg.num_nodes));
    add_logs("num_edges", to_string(dg.num_edges));
    add_logs("num_iterations", to_string(rank.t+1));
    add_logs("duration", to_string(get_time_sec(&start_time, &end_time)));

    // write a log file
    output_json_logs(log_file);
  }

}
