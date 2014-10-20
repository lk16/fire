#include "state.hpp"

extern std::mt19937_64 twister;

const int state::BURNING_STEPS;
const int state::SPREAD_RADIUS;
const int state::TREE;
const int state::BURNING_TREE;
const int state::EMPTY;


state::state()
{
  height = width = 0;
}

void state::copy_from(const state* rhs)
{
  width = rhs->width;
  height = rhs->height;
  burning = rhs->burning;
  cells = rhs->cells;
  // the previous line is OBVIOUSLY the speed bottleneck of the entire program  
}

void state::gen_uniform(double tree_chance)
{
  // ~0ull evaluates to (2^64)-1
  unsigned long long threshold;
  if(tree_chance == 1.0){
    // without this branch
    // due to some glitch we don't fully understand
    // threshold will be 0
    // We are ready to blame floating point rounding errors
    threshold = (~0ull);
  }
  else{
    threshold = ((~0ull)*tree_chance);
  }
  
  for(cell_t& c: cells){
    if(twister() < threshold){
      c = TREE;
    }
    else{
      c = EMPTY;
    }
  }
}

void state::gen_pattern(int code)
{
  int i = 0;
  for(cell_t& c: cells){
    int x = i%width;
    int y = i/width;
    switch(code){
      case 0:
        c = (((x+y) % 3 == 0) ? TREE : EMPTY);
        break;
      case 1:
        c = (((x%2==0) && (y%2==0)) ? TREE : EMPTY);
        break;
      case 2:
        c = (((x%3==0) && (y%3==0)) ? TREE : EMPTY);
        break;
      case 3:
        c = (((x%4<2) ^ (y%4<2)) ? TREE : EMPTY);
        break;
      default:
       std::cout << "gen_pattern: no code " << code << ". Exiting\n";
       std::exit(0);
    }
    i++;
  }
  
  
}



void state::start_fire_in_random_tree()
{
  std::vector<cell_t> trees;
  for(int i=0;i<width*height;i++){
    if(is_tree(cells[i])){
      trees.push_back(i);
    }
  }
  if(trees.empty()){
    std::cout << "warning: no trees, so none set on fire" << std::endl;
    return;
  }
  int fire = trees[twister() % trees.size()];
  cells[fire] = BURNING_TREE;
  burning.insert(fire);
}


void state::to_png(const std::string& file) const
{
  const png::rgb_pixel PIXEL_BLACK(0,0,0);
  const png::rgb_pixel PIXEL_RED(255,0,0);
  const png::rgb_pixel PIXEL_WHITE(255,255,255);
  const png::rgb_pixel PIXEL_GRAY(128,128,128);
  const png::rgb_pixel PIXEL_GREEN(30,255,30);
  
  
  std::map<cell_t,png::rgb_pixel> pix_map;
  
  pix_map[(cell_t)EMPTY] = PIXEL_GRAY;
  pix_map[(cell_t)TREE] = PIXEL_GREEN;
  for(int i=EMPTY+1;i<TREE;i++){
    double x = ((double)i)/(TREE-(EMPTY+1));
    pix_map[(cell_t)i] = png::rgb_pixel((1.0-x)*255,0,0);
  }
  
  int image_size = 800;
  png::image<png::rgb_pixel> im(image_size,image_size);
  double cell_height = (double)image_size/height;
  double cell_width = (double)image_size/width;
  
  if(height>image_size || width>image_size){
    cell_height = cell_width = 1;
    im.resize(width,height);
  }
  
  
  
  
    
  for(int y=0;y<height;y++){
    int y_start = (int)((cell_height*y)+0.5);
    int y_end = (int)((cell_height*(y+1))+0.5);
    for(int x=0;x<width;x++){
      int x_start = (int)((cell_width*x)+0.5);
      int x_end = (int)((cell_width*(x+1))+0.5);
      int i = y*width + x;
      assert(cells[i] <= TREE);
      png::rgb_pixel pix = pix_map[cells[i]];
      for(int a=y_start;a<y_end;a++){
        for(int b=x_start;b<x_end;b++){
          im.set_pixel(a,b,pix);
        }
      }
    }
  }
  
  im.write(file);
}

std::vector<int> state::count() const
{
  std::vector<int> res(256,0);
  assert(res[10] == 0);
  for(cell_t i: cells){
    res[i]++;
  }
  return res;
}


void state::step(state* out) const
{
  std::set<int> infected;
  
  // alias this constant for readability
  const int s = SPREAD_RADIUS;
  
  out->copy_from(this);
  
  // yes, this is ugly
  // no, I do not feel ashamed
  typedef std::set<int>::const_iterator citer_t;
  typedef std::set<int>::iterator iter_t;
  
  // for each cell within radius, spread the fire! 
  for(citer_t cit=out->burning.begin();cit!=out->burning.end();cit++){
    int x = *cit % width;
    int y = *cit / width;
    if(is_burning(cells[y*width + x])){
      for(int dy=-s;dy<=s;dy++){
        if(y+dy<0 || y+dy>=height){
          continue;
        }
        for(int dx=-s;dx<=s;dx++){
          if(x+dx<0 || x+dx>=width){
            continue;
          }
          if(is_tree(cells[(y+dy)*width + (x+dx)]) && fire_spread(dx,dy)){
            infected.insert((y+dy)*width + (x+dx));
          }
        }
      }
    }
  }
  
  { 
    // for each tree that stopped burning, remove it from the set
    iter_t next,it = out->burning.begin();
    while(it != out->burning.end()){
      out->cells[*it]--;
      if(out->cells[*it] == EMPTY){
        next = it;
        next++;
        out->burning.erase(it);
        it = next;
      }
      else{
        it++;
      }
    }
  }
  
  // for each tree that started burning: assign BURNING_TREE and add it to the set
  for(citer_t cit=infected.begin();cit!=infected.end();cit++){
    out->cells[*cit] = BURNING_TREE;
    out->burning.insert(*cit);
    
  }
  
}


int state::count_burning() const
{
  return burning.size();
}

void state::gen_cluster(double init_chance, int cluster_steps,int k)
{
    
  gen_uniform(init_chance);
  
  std::vector<cell_t> tmp;
  tmp.resize(cells.size());
  
  int trees = 0;
  int neighbours = 0;
  double chance;
  for(int c=0;c<cluster_steps;c++){
    for(int y=0;y<height;y++){
      for(int x=0;x<width;x++){
        int i = y*width + x;
        trees = neighbours = 0;
        if(y>0){
          neighbours++;
          if(is_tree(cells[i-width])){
            trees++; 
          }
        }
        if(y<height){
          neighbours++;
          if(is_tree(cells[i+width])){
            trees++; 
          }
        }
        if(x>0){
          neighbours++;
          if(is_tree(cells[i-1])){
            trees++; 
          }
        }
        if(x<width){
          neighbours++;
          if(is_tree(cells[i+1])){
            trees++;
          }
        }
        if(is_tree(cells[i])){
          trees += k;
        }
        chance = (double)trees / (k+neighbours);
        if(chance == 1.0){
          // same story as in gen uniform
          tmp[i] = TREE;
        }
        else{
          if(twister() < ((~0ull)*chance)){
            tmp[i] = TREE;
          }
          else{
            tmp[i] = EMPTY;
          }
        }
      }
    }
    cells = tmp;
  }
}





