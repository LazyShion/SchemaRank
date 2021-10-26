#include "schemarank.hpp"

using namespace std;

// -----------------------------------
// Constructor
// -----------------------------------
SchemaRank::SchemaRank(Graph *g, float a, unsigned int t, unsigned int k, float s, char* q_file){
  this->g = g;
  query_file = q_file;
  alpha = a;
  reverse_alpha = 1 - alpha;
  div_alpha = alpha/reverse_alpha;
  pow_alpha = 1;
  T = t;
  this->t = 0;
  K = k;
  stop_condition = s;
  num_nonpruned_nodes = g->num_nodes;

  computed_nodes.resize(T, 0);
  computed_norms.resize(T, 0);
  pruned.resize(g->num_nodes, false);

  init_query_vector();

  if(!g->is_DG){ 
    // For Schema Graph
    init_pr_vectors();
  }else{ 
    // For Data Graph
    vector_maxA.resize(g->num_nodes, 0.0);
    for(unsigned int i=0, end=g->num_nodes; i<end; ++i){
      NeighborsItr Ni = g->neighbors(i);
      unsigned int degree = g->degree(i);
      for(unsigned int j=0; j<degree; ++j){
	if(vector_maxA[i] < *(Ni.second)){
	  vector_maxA[i] = *(Ni.second);
	}
      }
    }
  }
  
}


// -----------------------------------
// Initialize Upper/Lower Vectors
// -----------------------------------
void
SchemaRank::init_ul_vectors(){
  vector_lower.resize(g->num_nodes, 0.0);
  tmp_vector_lower.resize(g->num_nodes, 0.0);
  vector_upper.resize(g->num_nodes, 0.0);

  for(unsigned int i=0, end=g->num_nodes; i<end; ++i){
    vector_lower[i] = reverse_alpha*vector_q[i];
    tmp_vector_lower[i] = vector_lower[i];
    vector_upper[i] = vector_Ar[i] + div_alpha*vector_maxA[i];
  }

}

// -----------------------------------
// Initialize Vector r and Ar
// -----------------------------------
void
SchemaRank::init_pr_vectors(){
  if(g->is_DG){
    // For Data Graph
    vector_r.resize(g->num_nodes, 0);
    tmp_vector_r.resize(g->num_nodes, 0);
    vector_Ar.resize(g->num_nodes, 0);
    tmp_vector_Ar.resize(g->num_nodes, 0);
    
    float sum_ri = 0;
    for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
      sum_ri += this->vector_b[g->d2s[u]];
    }

    for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
      float r_i = this->vector_b[g->d2s[u]]/sum_ri;
      vector_r[u] = r_i;
      tmp_vector_r[u] = r_i; 
      vector_Ar[u] = r_i;
      tmp_vector_Ar[u] = r_i;
    }
    
  }else{
    // For Schema Graph
    NodeV *d2s = &(g->d2s);
    hist_attr.resize(g->num_nodes, 0);
    for(unsigned int i=0, end=g->num_d_nodes; i<end; ++i){
      hist_attr[(*d2s)[i]] += 1;
    }
    
    vector_r.resize(g->num_nodes, 0);
    tmp_vector_r.resize(g->num_nodes, 0);
    vector_Ar.resize(g->num_nodes, 0);
    tmp_vector_Ar.resize(g->num_nodes, 0);

    for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
      float r_i = (float)hist_attr[u]/g->num_d_nodes;
      vector_r[u] = r_i;
      tmp_vector_r[u] = r_i;
      vector_Ar[u] = r_i;
      tmp_vector_Ar[u] = r_i;
    }
  }
  
}


// -----------------------------------
// Initialize Qeury Vector q
// -----------------------------------
void
SchemaRank::init_query_vector(){
  if(g->is_DG){
    ifstream finput;
    unsigned int node;
    unsigned int num_queries = 0;
    unordered_map<unsigned int, unsigned int> queries;
  
    finput.open(query_file);
    while(!finput.eof()){
      finput >> node;
      assert(node>=0 && node<g->num_nodes);
      queries[node] = 1;
    }
    finput.close();

    num_queries = queries.size();
    assert(num_queries>0 && num_queries<=g->num_nodes);
  
    float r_i = 1/(float)num_queries;
    vector_q.resize(g->num_nodes, 0);
    unordered_map<unsigned int, unsigned int>::iterator it = queries.begin(), end=queries.end();
    while(it!=end){
      vector_q[it->first] = r_i;
      ++it;
    }

  }else{
    ifstream finput;
    unsigned int node;
    unsigned int num_queries = 0;
    unordered_map<unsigned int, unsigned int> queries;
    NodeV *d2s = &(g->d2s);

    finput.open(query_file);
    while(!finput.eof()){
      finput >> node;
      assert(node>=0 && node<g->num_d_nodes);
      queries[node] = 1;
    }
    finput.close();

    num_queries = queries.size();
    vector_q.resize(g->num_nodes, 0);
    unordered_map<unsigned int, unsigned int>::iterator it = queries.begin(), end=queries.end();
    while(it!=end){
      unsigned int id = (*d2s)[it->first];
      vector_q[id] += 1;
      ++it;
    }

    for(unsigned int u=0; u<g->num_nodes; ++u){
      vector_q[u] = vector_q[u]/num_queries;
    }

  }
}


// -----------------------------------
// Perform One-level Iteration for DG
// -----------------------------------
void
SchemaRank::one_iteration(){
  // initialization
  num_computed_nodes = 0;
  l2norm = 0;

  // update alpha
  pow_alpha *= alpha;
  div_alpha *= alpha;

  // compute Delta_t and vector_r
  float Delta_t = 0.0;
  float sum_scores = 0.0;
  topk_lower = get_topk_large(tmp_vector_lower, K);

  for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
    // exact pruning
    if(!pruned[u]){
      if(vector_upper[u] < topk_lower){
	pruned[u] = true;
	num_nonpruned_nodes--;
      }
    }

    // add Delta_t value
    vector_Ar[u] = inner_product(u);
    if(vector_Ar[u] - tmp_vector_Ar[u] > 0){
      Delta_t += vector_Ar[u] - tmp_vector_Ar[u];
    }
    tmp_vector_Ar[u] = vector_Ar[u];
    
    // compute score r_u
    if(!pruned[u]){
      num_computed_nodes++;
      tmp_vector_r[u] = alpha*vector_Ar[u] + reverse_alpha*vector_q[u];
    }
    sum_scores += tmp_vector_r[u];
  }
  
  if(t==0){
    Delta_t = 1;
  }
  
  // update upper and lower bounds & compute l2norm
  float tmp_score = 0;
  for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
    if(!pruned[u]){
      // update upper bound
      vector_upper[u] = vector_lower[u] + pow_alpha*vector_Ar[u] + div_alpha*Delta_t*vector_maxA[u];
      // update lower bound
      vector_lower[u] = vector_lower[u] + reverse_alpha*pow_alpha*vector_Ar[u];
      tmp_vector_lower[u] = vector_lower[u];
      
    }

    tmp_score = tmp_vector_r[u]/sum_scores;
    l2norm += pow(tmp_score - vector_r[u], 2);
    vector_r[u] = tmp_score;
    
  }
  l2norm = sqrt(l2norm);

}

// -----------------------------------
// Perform One-level Iteration for SG
// -----------------------------------
void
SchemaRank::one_iteration_sg(){
  // initialization
  num_computed_nodes = 0;
  l2norm = 0;
  
  // compute Delta_t and vector_r
  float sum_scores = 0.0;
  for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
    num_computed_nodes++;
    tmp_vector_r[u] = alpha*vector_Ar[u] + reverse_alpha*vector_q[u];
    sum_scores += tmp_vector_r[u];
  }
    
  // compute l2norm
  float tmp_score = 0;
  for(unsigned int u=0, end=g->num_nodes; u<end; ++u){
    tmp_score = tmp_vector_r[u]/sum_scores;
    l2norm += pow(tmp_score - vector_r[u], 2);
    vector_r[u] = tmp_score;
  }
  l2norm = sqrt(l2norm);

}


// -----------------------------------
// Iterate Random-walk at most T Times
// -----------------------------------
void
SchemaRank::run(){
  for(unsigned int i = 0; i<T; ++i){
    if(g->is_DG){
      // For Data Graph
      this->t = i;
      one_iteration();
      computed_nodes[i] = num_computed_nodes;
      computed_norms[i] = l2norm;
    }else{
      // For Schema Graph
      one_iteration_sg();
    }

    if(l2norm < stop_condition){
      break;
    }

    if(g->is_DG && num_nonpruned_nodes <=K){
      break;
    }

  }
  
  if(g->is_DG){
    get_topk_nodes();
  }
}

// -----------------------------------
// Get top-K nodes 
// -----------------------------------
void
SchemaRank::get_topk_nodes(){
  vector<Pair> top_k(g->num_nodes);
  
  for(unsigned int i=0; i<g->num_nodes; ++i){
    top_k[i] = make_pair(vector_r[i], i);
  }

  sort(top_k.begin(), top_k.end(), pair_compare);
  
  float val = 0.0;
  for(unsigned int i=0; i<g->num_nodes; ++i){
    Pair p = top_k[i];
    if(results.size()<K){
      results.push_back(p);
      val = p.first;
    }else{
      if(val == p.first){
	results.push_back(p);
      }
    }
  }

}


// -----------------------------------
// Set vector_b & d2s from SG to DG,
// and Initialize vectors
// -----------------------------------
void
SchemaRank::set_values_to_dg(V *vector_b, NodeV *d2s){
  // Set vectors
  this->vector_b = *vector_b;
  g->d2s = *d2s;

  // Initialize vectors
  init_pr_vectors();
  init_ul_vectors();
}
