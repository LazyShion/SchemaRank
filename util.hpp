#ifndef UTIL_H
#define UTIL_H

#include<stdlib.h>
#include<fstream>
#include<chrono>
#include<unordered_map>

using namespace std;

static string LOGS;
static unordered_map<string,string> LOG_MAP;

typedef chrono::system_clock::time_point Chrono_t;

// macro for debug
#define debug(var) do{cout << #var << ": "; view(var); }while(0)
template<typename T> void view(T e){cout << e << endl;}
template<typename T> void view(const vector<T>& v){for(const auto& e:v){cout << e << " ";} cout << endl;}
template<typename T> void view(const vector<vector<T>>& vv){for(const auto& v:vv){view(v);}}

// get a time point
static Chrono_t get_time(){
  return chrono::system_clock::now();
}

// get a running time in milliseconds
static double get_time_millisec(const Chrono_t *start, const Chrono_t *end){
  return static_cast<double>(chrono::duration_cast<chrono::microseconds>(*end - *start).count()/ 1000.0);
}

// get a running time in seconds
static double get_time_sec(const Chrono_t *start, const Chrono_t *end){
  return static_cast<double>(chrono::duration_cast<chrono::microseconds>(*end - *start).count()/ 1000000.0);
}

// output logs in JSON format
static void output_json_logs(const char *log_file_name){
  unordered_map<string, string>::iterator it = LOG_MAP.begin(), end = LOG_MAP.end();
  unsigned int log_size = LOG_MAP.size(), count=0;
  while(it!=end){
    LOGS += "\t"+it->first +":"+it->second+"";
    ++count;
    ++it;
    if(count<log_size){
      LOGS += ", \n";
    }
  }

  string log_name(log_file_name);
  LOGS = log_name+":{\n"+LOGS+"\n}";
  ofstream foutput(log_file_name);
  foutput << LOGS;
  foutput.close();
}

// add log data to LOGS in JSON format
static void add_logs(const string header, const string log){
  LOG_MAP[header]=log;
}

// return top-k largest value in a vector
static float get_topk_large(vector<float> &a, unsigned int k){
  for(int l=0, r=a.size()-1; l<=r;){
    float p = a[(l+r)/2];
    int i=l-1, j=r+1;
    while(1){
      while(a[++i]>p);
      while(a[--j]<p);
      if(i>=j) break;
      swap(a[i], a[j]);
    }
    if(k==i-l) return a[i-1];
    else if(k<i-l){r=i-1;}
    else if(k>i-l){k-=(i-l+1); l=j+1;}
  }
  return -1;
}



#endif // UTIL_H
