/*
 * Graph.hpp
 * Copyright (c) 2016 Yasuo Tabei All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE and * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cstring>
#include <string>
#include <iterator>
#include <strstream>
#include <cmath>
#include <map>
#include "stdint.h"

struct Edge {
  uint32_t from;
  uint32_t to;
  uint32_t elabel;
  uint32_t id;
  Edge(): from(0), to(0), elabel(0), id(0) {};
};

class Vertex {
public:
  std::string label;
  std::vector<Edge> edge;

  std::vector<uint64_t> tmpLabels;
  
  bool neighbor(uint32_t to) {
    for (size_t i = 0; i < edge.size(); ++i) {
      if (edge[i].to == to)
	return true;
    }
    return false;
  }

  void push(uint32_t from, uint32_t to, uint32_t elabel) {
    edge.resize(edge.size() + 1);
    edge[edge.size()-1].from   = from;
    edge[edge.size()-1].to     = to;
    edge[edge.size()-1].elabel = elabel;
    return;
  }
};

class Graph: public std::vector<Vertex> {
private:
public:
  uint64_t vertexSize() { return (uint64_t)size(); }
  void read(std::istream &ifs);
  void write(std::ostream &ofs);
  //  string getName() {
  //    return gname;
  //  }
  std::string name;
private:

  //  static std::map<std::string, uint32_t> vl2c;
  //  static uint32_t countLabel;
};

#endif // _GRAPH_HPP_
