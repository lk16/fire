#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <sys/stat.h>
#include <sys/time.h>
#include <vector>
#include <random>

#include <png++/png.hpp>

#include "util.hpp"
#include "state.hpp"
#include "args.hpp"

std::mt19937_64 twister;


std::string to_three_digits(int x){
  if(x<10){
    return std::string("00") + to_str(x);
  }
  if(x<100){
    return std::string("0") + to_str(x);
  }
  return to_str(x);
}


std::string get_out_folder(){
  mkdir("out",0744);
  std::stringstream ss;
  std::string out_folder = (std::string("out/") + to_str(std::time(NULL)) + '/');
    mkdir(out_folder.c_str(),0744);
  return out_folder;
}

int main(int argc,char** argv){
  timeval tv;
  gettimeofday(&tv,nullptr);
  twister.seed(tv.tv_sec ^ tv.tv_usec);
  
  const std::string out_folder = get_out_folder();
  
  state states[2];

  const args arg_data(argc,(const char**)argv,out_folder);
  arg_data.init_state(&states[0]);
  
  
  const int initial_tree_count = states[0].count()[state::TREE];
  
  //states[0].start_fire_in_corner();
  states[0].start_fire_in_random_tree();
  
  int step = 0;
  if(arg_data.gen_only && arg_data.png_out){
    states[0].to_png(out_folder + "000.png");
    return 0;
  }
  
  while(states[step%2].count_burning() > 0){
    if(arg_data.png_out){
      states[step % 2].to_png(out_folder + to_three_digits(step) + ".png");
    }
    states[step % 2].step(&states[(step+1) % 2]);
    std::cout << "Calculated step " << step << ":\t";
    std::vector<int> count = states[step%2].count();
    std::cout << states[step%2].count_burning() << " burning\t";
    std::cout << count[state::TREE] << " non burning" << std::endl;
    step++;
  }
  
  if(arg_data.png_out){
    states[step % 2].to_png(out_folder + to_three_digits(step+1) + ".png");
  }
  
  double remaining_tree_percentage = 
    ((100.0*states[step%2].count()[state::TREE]) / initial_tree_count);
  
  if(!arg_data.gen_only){
    std::cout << "Remaining tree percentage: " << remaining_tree_percentage << std::endl;
  }
  
  if(arg_data.short_data_filename != ""){
    std::ofstream file(arg_data.short_data_filename,std::ios_base::app);
    file << states[0].width << ',';
    file << states[0].height << ',';
    file << arg_data.cluster_steps << ',';
    file << arg_data.init_cluster_chance << ',';
    file << remaining_tree_percentage << ',';
    file << step-1 << '\n';
  }
  
  
  return 0;  
}