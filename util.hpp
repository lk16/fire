#pragma once

#include <sstream>
#include <string>

template<class T>
inline std::string to_str(T x){
  std::stringstream buff;
  buff << x;
  return buff.str();
}

template<class T>
inline T from_str(std::string x){
  std::stringstream buff(x);
  T temp;
  buff >> temp;
  return temp;
}
