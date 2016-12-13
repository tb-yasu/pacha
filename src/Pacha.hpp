/*
 * Pacha.hpp
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


#ifndef _PACHA_HPP_
#define _PACHA_HPP_

#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <sstream>
#include <set>
#include <map>
#include <algorithm>
#include "Graph.hpp"

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffff
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 0xffffffffffffffff
#endif

#ifdef _PARALLEL_
#include <omp.h>
#endif 

class Cmp1 {
public:
  bool operator()(std::pair<std::pair<uint32_t, uint32_t>, float> &a, std::pair<std::pair<uint32_t, uint32_t>, float> &b) {
    return a.second < b.second;
  }
};

class Cmp2 {
public:
  bool operator()(std::pair<uint32_t, std::string> &a, std::pair<uint32_t, std::string> &b) {
    return (a.second[0] < b.second[0]);
  }
};

class Cmp3 {
public:
  bool operator()(std::pair<std::pair<uint32_t, uint32_t>, float> &a, std::pair<std::pair<uint32_t, uint32_t>, float> &b) {
    return a.second > b.second;
  }
};

class Pacha {
private:
  void read(std::istream &is);
  void readPairFile(std::istream &is, std::set<std::pair<std::string, std::string> > &pairs);
  void compMatchingVertex(Graph &g1, Graph &g2, std::vector<std::map<uint32_t, float> > &fvs1, std::vector<std::map<uint32_t, float> > &fvs2, std::map<std::pair<uint32_t, uint32_t>, float> &simmat);
  void compFeatureVec(Graph &g, uint64_t dist, std::vector<std::map<uint32_t, float> > &fvs);
  void initNodeLabels(Graph &g, uint64_t length, std::vector<uint64_t> &nodeLabels, std::vector<std::map<uint32_t, float> > &fvs);
  void compFeature(uint64_t vid, uint64_t length, Graph &g, std::vector<uint64_t> &nodeLabels, std::map<uint32_t, float> &fv);
  float compCos(std::map<uint32_t, float> &fv1, std::map<uint32_t, float> &fv2);
  float compJaccard(std::map<uint32_t, float> &fv1, std::map<uint32_t, float> &fv2);
  void compAlignment(Graph &g1, Graph &g2, std::map<std::pair<uint32_t, uint32_t>, float> &simmat, float &rscore, std::vector<std::pair<uint32_t, uint32_t> > &matchPair);
  void print(std::ostream &os, Graph &g1, Graph &g2, float score, std::vector<std::pair<uint32_t, uint32_t> > &matchPair);
  void postProcess1(Graph &g1, Graph &g2, std::vector<std::pair<uint32_t, uint32_t> > &matchPair, std::vector<std::pair<std::pair<std::string, std::pair<uint64_t, uint64_t> >, std::pair<std::string, std::pair<uint64_t, uint64_t> > > > &res);
  void postProcess2(Graph &g1, Graph &g2, std::vector<std::pair<uint32_t, uint32_t> > &matchPair, std::vector<std::pair<std::string, std::pair<uint64_t, uint64_t> > > &res);
  bool check1(std::vector<std::pair<uint32_t, std::string> > &tmp1, std::vector<std::pair<uint32_t, std::string> > &tmp2, uint64_t &counter);
  bool check2(std::vector<std::pair<uint32_t, std::string> > &tmp1, std::vector<std::pair<uint32_t, std::string> > &tmp2, uint64_t &counter);
  float calcTanimoto(Graph &g1, Graph &g2, std::vector<std::pair<uint32_t, uint32_t> > &matchPair);
  void updateVertexLabels(uint64_t length, Graph &g, std::vector<uint64_t> &nodeLabels);
  void print(std::ostream &os, std::vector<std::pair<std::pair<std::string, std::pair<uint64_t, uint64_t> >, std::pair<std::string, std::pair<uint64_t, uint64_t> > > > &res);
  void print(std::ostream &os, std::vector<std::pair<std::string, std::pair<uint64_t, uint64_t> > > &res);
public:
  void run(std::string input_file, std::string output_file, uint64_t dist, uint64_t topk, float threshold, uint64_t num_threads, uint64_t inter_cuts, uint64_t intra_cuts);
  void run_use_pair(std::string input_file, std::string output_file, std::string pair_file, uint64_t dist, uint64_t topk, float threshold, uint64_t num_threads, uint64_t inter_cuts, uint64_t intra_cuts);
private:
  std::vector<Graph> TRANS;
  static std::map<std::string, uint64_t> p2c;
  static uint64_t countPass;
  uint64_t topk_;
};

#endif // _PACHA_HPP_


