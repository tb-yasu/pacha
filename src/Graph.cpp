/*
 * Graph.cpp
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

#include "Graph.hpp"

using namespace std;

//std::map<std::string, uint32_t> Graph::vl2c;
//uint32_t Graph::countLabel = 0ULL;

template <class T, class Iterator> 
void tokenize (const string &str, Iterator iterator) {
  istrstream is(str.c_str(), str.size());
  copy (std::istream_iterator <T> (is), std::istream_iterator <T> (), iterator);
}

void Graph::read(istream &is) {
  string line;
  while (getline(is, line)) {
    vector<string> result;
    tokenize<string>(line, back_inserter(result));
    if (result.size() > 1 && result[0] == "ENTRY") 
      name = result[1];
    else if (result.size() > 0 && result[0] == "ATOM")
      break;
  }
  
  clear();
  while (getline(is, line)) {
    vector<string> result;
    tokenize<string>(line, back_inserter(result));
    if (result[0] == "BOND")
      break;
    uint32_t id = atoi(result[0].c_str());
    this->resize(id + 1);
    (*this)[id].label = result[1];
  }

  while (getline(is, line)) {
    vector<string> result;
    tokenize<string>(line, back_inserter(result));
    if (result[0] == "///" || result[0] == "BRACKET")
      break;
    else if (result.size() >= 4) {
      uint32_t from   = atoi(result[1].c_str());
      uint32_t to     = atoi(result[2].c_str());
      uint32_t elabel = atoi(result[3].c_str());
      if (size() <= from || size() <= to) {
	cerr << "Format Error: define vertex lists before edges" << endl;
	exit(-1);
      }
      (*this)[from].push(from, to, elabel);
      (*this)[to].push(to, from, elabel);
    }
  }
}

