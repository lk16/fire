#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "state.hpp"
#include "util.hpp"

struct args{
  // public members
  state s;
  
  // generate only initial state, do not run the algorithm
  bool gen_only;
  
  // output png files
  bool png_out;
  
  double init_cluster_chance;
  int cluster_steps;
  
  std::string short_data_filename;
  
  // ctor
  args(int argc,const char** argv,const std::string& filename);
  
  // parse args and initialize *out
  void init_state(state* out) const;
  
  
  static void argument_error_if(bool b);
  static void not_implemented();
};