#include "args.hpp"

args::args(int argc, const char** argv,const std::string& filename)
{
  std::vector<std::string> arg_vec;
  std::ofstream info(filename.c_str(),std::ios_base::app);
  for(int i=0;i<argc;i++){
    info << "arg[" << i << "] = " << argv[i] << '\n';
    arg_vec.push_back(argv[i]);
  }
  png_out = true;
  gen_only = false;
  short_data_filename = "";
  
  s.height = s.width = 0;
  
  unsigned index = 1;
  
  
  while(index<arg_vec.size()){
    if(arg_vec[index] == "-size"){
      argument_error_if(arg_vec.size() <= index+2);
      s.width = from_str<int>(arg_vec[index+1]);
      s.height = from_str<int>(arg_vec[index+2]);
      argument_error_if(s.width <= 0);
      argument_error_if(s.height <= 0);
      s.cells.assign(s.height*s.width,(cell_t)0);
      std::cout << "args: width,height = " << s.width;
      std::cout << ',' << s.height << std::endl;
      index += 3;
    }
    else if(arg_vec[index] == "-gen"){
      argument_error_if(arg_vec.size() <= index+1);
      std::string gen_algo = arg_vec[index+1];
      if(gen_algo == "uniform"){
        argument_error_if(arg_vec.size() <= index+2);
        double chance = from_str<double>(arg_vec[index+2]);
        argument_error_if(chance<0.0 || chance>1.0);
        std::cout << "args: generate algo = uniform, chance = " << chance << std::endl;
        s.gen_uniform(chance);
        index += 3;
      }
      else if(gen_algo == "pattern"){
        argument_error_if(arg_vec.size() <= index+1);
        int code = from_str<int>(arg_vec[index+2]);
        std::cout << "args generate algo = pattern, code = " << code << std::endl;
        argument_error_if(code < 0);
        s.gen_pattern(code);
        index += 3;
      }
      else if(gen_algo == "cluster"){
        argument_error_if(arg_vec.size() <= index+4);
        init_cluster_chance = from_str<double>(arg_vec[index+2]);
        argument_error_if(init_cluster_chance<0.0 || init_cluster_chance>1.0);
        cluster_steps = from_str<int>(arg_vec[index+3]);
        argument_error_if(cluster_steps<0);
        int k = from_str<int>(arg_vec[index+4]);
        argument_error_if(k<0);
        std::cout << "args: generate algo = cluster, init_chance,cluster_steps,k = ";
        std::cout << init_cluster_chance  << ',' << cluster_steps << ',' << k << std::endl;
        s.gen_cluster(init_cluster_chance,cluster_steps,k);
        index += 5;
      }
      else{
        argument_error_if(true);
      }
    }
    else if(arg_vec[index] == "-print-short-data"){
      argument_error_if(arg_vec.size() <= index+1);
      short_data_filename = arg_vec[index+1];
      std::cout << "args: short data filename = " << short_data_filename << std::endl;
      index += 2;      
    }
    else if(arg_vec[index] == "-no-png"){
      png_out = false;
      index++;
    }
    else if(arg_vec[index] == "-gen-only"){
      gen_only = true;
      index++;
    }
    else{
      std::cout << "args: unknown argument: " << arg_vec[index] << std::endl;
      std::exit(0);
    }
  }
}


void args::init_state(state* out) const
{
  *out = s;
}

void args::argument_error_if(bool b)
{
  if(b){
    std::cout << "args: argument error!" << std::endl;
    std::exit(0);
  }
}

void args::not_implemented()
{
  std::cout << "args: this is not implemented!" << std::endl;
  std::exit(0);
}
