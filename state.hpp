#pragma once

#include <cstdlib>
#include <climits>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <random>

#include <png++/png.hpp>

#include "util.hpp"

typedef int cell_t;

extern std::mt19937_64 twister;

struct state{

  static const int BURNING_STEPS = 5;
  static const int SPREAD_RADIUS = 4;
  static const int TREE = BURNING_STEPS + 1;
  static const int BURNING_TREE = BURNING_STEPS;
  static const int EMPTY = 0;
  
  inline static bool is_burning(cell_t c);
  inline static bool is_tree(cell_t c);
  inline static bool is_empty(cell_t c);
  inline static bool fire_spread(int dx,int dy);
  
  // default ctor
  state();
  
  // ctor with width and height
  state(int w,int h);
  
  // alternative to copy ctor
  void copy_from(const state* rhs);
  
  // generate uniformly distributed trees
  void gen_uniform(double chance);
  
  // generate pattern
  void gen_pattern(int code);
  
  // generates clusters
  void gen_cluster(double init_chance, int cluster_steps,int k);
  
  // do a step and put the result in *out
  void step(state* out) const;
  
  // count the occurrences of every cell state in *this
  std::vector<int> count() const;
  
  // count amount of burning cells
  int count_burning() const;
  
  
  // export this state to a png
  void to_png(const std::string& file) const;
  
  // sets the left top corner of this on fire
  void start_fire_in_corner();
  
  // sets a random tree of this on fire
  void start_fire_in_random_tree();
  
  
  // data members
  std::vector<cell_t> cells;
  std::set<int> burning;
  int height,width;
};

inline state::state(int w,int h){
  width = w;
  height = h;
}

inline bool state::is_burning(cell_t c){
  return c<TREE && c!=EMPTY;
}

inline bool state::is_tree(cell_t c){
  return c==TREE; 
}

inline bool state::is_empty(cell_t c){ 
  return c==EMPTY; 
}

inline bool state::fire_spread(int dx,int dy){
  if((dx*dx) + (dy*dy) > (SPREAD_RADIUS*SPREAD_RADIUS)){
    return false;
  }
  if(dx==0 && dy==0){
    return false;
  }
  return (twister() % (5*((dx*dx)+(dy*dy))) == 0);  
}

inline void state::start_fire_in_corner()
{
  assert(width>0 && height>0);
  cells[0] = BURNING_TREE;
  burning.insert(0);
}
