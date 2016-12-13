/*
 * Pacha.cpp
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


#include "Pacha.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

double gettimeofday_sec()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

std::map<string, uint64_t> Pacha::p2c;
uint64_t Pacha::countPass = 0;

void Pacha::read(istream &is) {
  while (true) {
    Graph g;
    g.read(is);
    if (g.empty())
      break;
    TRANS.push_back(g);
  }
}

void Pacha::readPairFile(istream &is, set<pair<string, string> > &pairs) {
  string line;
  while (getline(is, line)) {
    stringstream ss(line);
    string name1, name2;
    ss >> name1;
    ss >> name2;
    pairs.insert(make_pair(name1, name2));
    pairs.insert(make_pair(name2, name1));
  }
}

void Pacha::initNodeLabels(Graph &g, uint64_t length, vector<uint64_t> &nodeLabels, vector<map<uint32_t, float> > &fvs) {
  nodeLabels.resize(g.size());
  for (size_t vid = 0; vid < nodeLabels.size(); ++vid) {
    stringstream ss;
    ss << g[vid].label.substr(0, length) << "_";
    if (p2c.find(ss.str()) == p2c.end()) {
      p2c[ss.str()] = countPass;
      #ifdef _PARALLEL_
      #pragma omp atomic
      #endif
      countPass++;
    }

    uint64_t label = p2c[ss.str()];
    nodeLabels[vid] = label;
    if (g[vid].tmpLabels.size() <= length)
      g[vid].tmpLabels.resize(length + 1);
    g[vid].tmpLabels[length] = label; 
    map<uint32_t, float> &fv = fvs[vid];
    map<uint32_t, float>::iterator it = fv.find(p2c[ss.str()]);
    if (it == fv.end()) 
      fv[p2c[ss.str()]] = 1;
    else 
      it->second += 1;
  }
}

void Pacha::compFeature(uint64_t vid, uint64_t length, Graph &g, vector<uint64_t> &nodeLabels, map<uint32_t, float> &fv) {
  stringstream ss;
  //  ss << nodeLabels[vid];
  ss << g[vid].tmpLabels[length];
  for (size_t i = 0; i < g[vid].edge.size(); ++i)  {
    uint64_t toid = g[vid].edge[i].to;
    //ss << "_" << g[toid].label.substr(0, length);
    ss << "_" << g[toid].tmpLabels[length];
  }

  if (p2c.find(ss.str()) == p2c.end()) {
    #ifdef _PARALLEL_
    #pragma omp critical
    #endif
    {
    p2c[ss.str()] = countPass++;
    }
  }

  uint64_t label  = p2c[ss.str()];
  nodeLabels[vid] = label;
  map<uint32_t, float>::iterator it = fv.find(label);
  if (it == fv.end()) 
    fv[p2c[ss.str()]] = 1;
  else 
    it->second += 1;
}

void Pacha::updateVertexLabels(uint64_t length, Graph &g, vector<uint64_t> &nodeLabels) {
  for (size_t vid = 0; vid < g.size(); ++vid) 
    g[vid].tmpLabels[length] = nodeLabels[vid];
}

void Pacha::compFeatureVec(Graph &g, uint64_t dist, vector<map<uint32_t, float> > &fvs) {
  fvs.resize(g.size());
  vector<uint64_t> nodeLabels1;
  vector<uint64_t> nodeLabels2;
  vector<uint64_t> nodeLabels3;
  initNodeLabels(g, 1, nodeLabels1, fvs);
  initNodeLabels(g, 2, nodeLabels2, fvs);
  initNodeLabels(g, 3, nodeLabels3, fvs);
  for (size_t iter = 0; iter < dist; ++iter)  {
    for (size_t vid = 0; vid < g.size(); ++vid) {
      map<uint32_t, float> &fv = fvs[vid];
      compFeature(vid, 1, g, nodeLabels1, fv);
      compFeature(vid, 2, g, nodeLabels2, fv);
      compFeature(vid, 3, g, nodeLabels3, fv);
    }
    for (size_t vid = 0; vid < g.size(); ++vid) {
      updateVertexLabels(1, g, nodeLabels1);
      updateVertexLabels(2, g, nodeLabels2);
      updateVertexLabels(3, g, nodeLabels3);
    }
  }
}

float Pacha::compCos(map<uint32_t, float> &fv1, map<uint32_t, float> &fv2) {
  map<uint32_t, float>::iterator it1 = fv1.begin();
  map<uint32_t, float>::iterator it2 = fv2.begin();

  float num = 0.f;
  float dev1 = 0.f, dev2 = 0.f;
  while (it1 != fv1.end() && it2 != fv2.end()) {
    if      (it1->first < it2->first) {
      dev1 += it1->second * it1->second;
      it1++;
    }
    else if (it1->first > it2->first) {
      dev2 += it2->second * it2->second;
      it2++;
    }
    else {
      num  += it1->second * it2->second;
      dev1 += it1->second * it1->second;
      dev2 += it2->second * it2->second;
      it1++; it2++;
    }
  }
  while (it1 != fv1.end()) {
    dev1 += it1->second * it1->second;
    it1++;
  }
  while (it2 != fv2.end()) {
    dev2 += it2->second * it2->second;
    it2++;
  }
  
  return num/(sqrt(dev1)*sqrt(dev2));
}

float Pacha::compJaccard(map<uint32_t, float> &fv1, map<uint32_t, float> &fv2) {
  map<uint32_t, float>::iterator it1 = fv1.begin();
  map<uint32_t, float>::iterator it2 = fv2.begin();

  float num = 0.f, den = 0.f;
  while (it1 != fv1.end() && it2 != fv2.end()) {
    if      (it1->first < it2->first) {
      den += it1->second;
      it1++;
    }
    else if (it1->first > it2->first) {
      den += it2->second;
      it2++;
    }
    else {
      den += max(it1->second, it2->second);
      num += min(it1->second, it2->second);
      it1++; it2++;
    }
  }
  while (it1 != fv1.end()) {
    den += it1->second;
    it1++;
  }
  while (it2 != fv2.end()) {
    den += it2->second;
    it2++;
  }
  return num/den;
}


void Pacha::compMatchingVertex(Graph &g1, Graph &g2, std::vector<std::map<uint32_t, float> > &fvs1, std::vector<std::map<uint32_t, float> > &fvs2, map<pair<uint32_t, uint32_t>, float> &simmat) {
  for (size_t i = 1; i < fvs1.size(); ++i) {
    map<uint32_t, float> &fv1 = fvs1[i];
    for (size_t j = 1; j < fvs2.size(); ++j) {
      if (g1[i].label[0] != g2[j].label[0])
	continue;
      map<uint32_t, float> &fv2 = fvs2[j];
      float sim = compJaccard(fv1, fv2);
      simmat[make_pair(i, j)] = sim;
    }
  }
}

void Pacha::compAlignment(Graph &g1, Graph &g2, map<pair<uint32_t, uint32_t>, float> &simmat, float &rscore, std::vector<std::pair<uint32_t, uint32_t> > &matchPair) {
  vector<pair<pair<uint32_t, uint32_t>, float> > tops;
  for (map<pair<uint32_t, uint32_t>, float>::iterator it = simmat.begin(); it != simmat.end(); ++it) {
    float score = it->second;
    uint32_t i = it->first.first;
    uint32_t j = it->first.second;
    tops.push_back(make_pair(make_pair(i, j), score));
  }
  sort(tops.begin(), tops.end(), Cmp3());

  rscore = 0.f;
  for (size_t k = 0; k < topk_ && k < tops.size(); ++k) {
    uint32_t i  = tops[k].first.first;
    uint32_t j  = tops[k].first.second;
    float score = tops[k].second;
    float currentScore = 0.f;
    vector<pair<uint32_t, uint32_t> > currentPair;
    set<uint32_t> checker1, checker2;
    priority_queue<pair<pair<uint32_t, uint32_t>, float>, vector<pair<pair<uint32_t, uint32_t>, float> >, Cmp1> stk;
    stk.push(make_pair(make_pair(i, j), score));
    while (!stk.empty()) {
      uint64_t i  = stk.top().first.first;
      uint64_t j  = stk.top().first.second;
      float score = stk.top().second;
      stk.pop();

      if (checker1.find(i) != checker1.end() || checker2.find(j) != checker2.end())
	continue;
      checker1.insert(i);
      checker2.insert(j);

      currentScore += score;
      currentPair.push_back(make_pair(i, j));
      for (size_t vid1 = 0; vid1 < g1[i].edge.size(); ++vid1) {
	uint32_t to1 = g1[i].edge[vid1].to;
	for (size_t vid2 = 0; vid2 < g2[j].edge.size(); ++vid2) {
	  uint32_t to2 = g2[j].edge[vid2].to;
	  map<pair<uint32_t, uint32_t>, float>::iterator it = simmat.find(make_pair(to1, to2));
	  if (it == simmat.end())
	    continue;
	  stk.push(make_pair(make_pair(to1, to2), it->second));
	}
      }
    }
    if (currentScore > rscore) {
      rscore = currentScore;
      matchPair = currentPair;
    }
  }
}

void Pacha::print(ostream &os, Graph &g1, Graph &g2, float score, vector<pair<uint32_t, uint32_t> > &matchPair) {
  os << "Graph name:" << g1.name << " " << g2.name << endl;
  os << "Score: " << score << " #matchingPairs: " << matchPair.size() << endl ;
  for (size_t i = 0; i < matchPair.size(); ++i)
    os << (i+1) << "\t" << matchPair[i].first << ":" << g1[matchPair[i].first].label << "<->" << matchPair[i].second << ":" << g2[matchPair[i].second].label << endl;
}

void Pacha::print(ostream &os, vector<pair<pair<string, pair<uint64_t, uint64_t> >, pair<string, pair<uint64_t, uint64_t> > > > &res) {
  for (size_t i = 0; i < res.size(); ++i) {
    string &name1 = res[i].first.first;
    uint64_t i1 = res[i].first.second.first;
    uint64_t i2 = res[i].first.second.first;
    string &name2 = res[i].second.first;
    uint64_t j1 = res[i].second.second.first;
    uint64_t j2 = res[i].second.second.second;
    os << "Edge " << "(" << i1 << "," << i2 << ") in " << name1 << " corresponding to atoms (" << j1 << "," << j2 << ") in " << name2 << " is eliminated." << endl;
  }
}

void Pacha::print(ostream &os, vector<pair<string, pair<uint64_t, uint64_t> > > &res) {
  for (size_t i = 0; i < res.size(); ++i) {
    string   &name = res[i].first;
    uint64_t from = res[i].second.first;
    uint64_t to   = res[i].second.second;
    os << "Edge " << "(" << from << "," << to << ") in "  << name << " is eliminated." << endl;
  }
}

bool Pacha::check1(vector<pair<uint32_t, string> > &tmp1, vector<pair<uint32_t, string> > &tmp2, uint64_t &counter) {
  counter = 0ULL;
  size_t i = 0ULL, j = 0ULL;
  while (i < tmp1.size() && j < tmp2.size()) {
    if      (tmp1[i].second[0] < tmp2[j].second[0]) {
      ++counter;
      ++i;
    }
    else if (tmp1[i].second[0] > tmp2[j].second[0]) {
      ++counter;
      ++j;
    }
    else {
      ++i; ++j;
    }
  }
  counter += (tmp1.size() - i);
  counter += (tmp2.size() - j);

  if (counter > 0)
    return true;
  return false;
}

void Pacha::postProcess1(Graph &g1, Graph &g2, std::vector<std::pair<uint32_t, uint32_t> > &matchPair, vector<pair<pair<string, pair<uint64_t, uint64_t> >, pair<string, pair<uint64_t, uint64_t> > > > &res) {
  for (size_t iter1 = 0; iter1 < matchPair.size(); ++iter1) {
    uint64_t i1 = matchPair[iter1].first;
    uint64_t j1 = matchPair[iter1].second;
    for (size_t iter2 = iter1 + 1; iter2 < matchPair.size(); ++iter2) {
      uint64_t i2 = matchPair[iter2].first;
      uint64_t j2 = matchPair[iter2].second;
      bool flag1 = g1[i1].neighbor(i2);
      bool flag2 = g2[j1].neighbor(j2);
      if (flag1 == true && flag2 == false) 
	res.push_back(make_pair(make_pair(g2.name, make_pair(j1, j2)), make_pair(g1.name, make_pair(i1, i2))));
      if (flag1 == false && flag2 == true)
	res.push_back(make_pair(make_pair(g1.name, make_pair(i1, i2)), make_pair(g2.name, make_pair(j1, j2))));
      
      /*
      if (flag1 == true && flag2 == false) 
	os << "Edge " << "(" << j1 << "," << j2 << ") in " << g2.name << " corresponding to atoms (" << i1 << "," << i2 << ") in " << g1.name << " is eliminated." << endl;
      if (flag1 == false && flag2 == true) 
	os << "Edge " << "(" << i1 << "," << i2 << ") in " << g1.name << " corresponding to atoms (" << j1 << "," << j2 << ") in " << g2.name << " is eliminated." << endl;
      */
    }
  }
}

void Pacha::postProcess2(Graph &g1, Graph &g2, vector<pair<uint32_t, uint32_t> > &matchPair, vector<pair<string, pair<uint64_t, uint64_t> > > &res) {
  set<uint32_t> tmp;
  for (size_t iter = 0; iter < matchPair.size(); ++iter) 
    tmp.insert(matchPair[iter].first);
  //  uint64_t counter = 0;
  for (size_t iter = 0; iter < matchPair.size(); ++iter) {
    uint64_t from = matchPair[iter].first;
    for (size_t j = 0; j < g1[from].edge.size(); ++j) {
      uint32_t to = g1[from].edge[j].to;
      if (tmp.find(to) != tmp.end())
	continue;
      res.push_back(make_pair(g1.name, make_pair(from, to)));
      //      os << "Edge " << "(" << from << "," << to << ") in "  << g1.name << " is eliminated." << endl;
      //      counter++;
    }
    
  }
  //  os << "Number of elimnated edges in " << g1.name << ": " << counter << endl;

  tmp.clear();
  for (size_t iter = 0; iter < matchPair.size(); ++iter) 
    tmp.insert(matchPair[iter].second);
  //  counter = 0;
  for (size_t iter = 0; iter < matchPair.size(); ++iter) {
    uint64_t from = matchPair[iter].second;
    for (size_t j = 0; j < g2[from].edge.size(); ++j) {
      uint32_t to = g2[from].edge[j].to;
      if (tmp.find(to) != tmp.end())
	continue;
      res.push_back(make_pair(g2.name, make_pair(from, to)));
      //      os << "Edge " << "(" << from << "," << to << ") in "  << g2.name << " is eliminated." << endl;
      //      counter++;
    }
  }
  //  os << "Number of elimnated edges in " << g2.name << ": " << counter << endl;
}

float Pacha::calcTanimoto(Graph &g1, Graph &g2, vector<pair<uint32_t, uint32_t> > &matchPair) {
  return (float)matchPair.size()/((float)g1.size() + (float)g2.size() - 2 - (float)matchPair.size());
}

void Pacha::run(string input_file, string output_file, uint64_t dist, uint64_t topk, float threshold, uint64_t num_threads, uint64_t inter_cuts, uint64_t intra_cuts) {
  topk_ = topk;
  
  DIR* dir;
  struct dirent* dp;
  vector<string> names;
  if (NULL == (dir = opendir(input_file.c_str()))){
    cerr << "Directory Open Error." << endl;
    exit(1);
  }
  for(size_t i = 0; NULL != (dp = readdir(dir)); ++i) {
    printf("%ld:%s\n" , i , dp->d_name);
    if (!strcmp(dp->d_name, "."))  continue;
    if (!strcmp(dp->d_name, "..")) continue;
    string d_name(dp->d_name);
    string target = input_file + d_name;
    ifstream ifs(target.c_str());
    if (!ifs) {
      cerr << "cannot open: " << input_file << endl;
      exit(-1);
    }
    read(ifs);
    ifs.close();
  }

  ofstream ofs(output_file.c_str());
  if (!ofs) {
    cerr << "cannot open: " << output_file << endl;
    exit(-1);
  }

  double start = gettimeofday_sec();
  for (size_t i = 0; i < TRANS.size(); ++i) {
    Graph &g1 = TRANS[i];
    vector<map<uint32_t, float> > fvs1;
    compFeatureVec(g1, dist, fvs1);
    
    #ifdef _PARALLEL_
      omp_set_num_threads(num_threads);
    #endif
    #ifdef _PARALLEL_
      #pragma omp parallel for schedule(static)
    #endif
    for (size_t j = i + 1; j < TRANS.size(); ++j) {
      Graph &g2 = TRANS[j];
      vector<map<uint32_t, float> > fvs2;
      compFeatureVec(g2, dist, fvs2);
      
      map<pair<uint32_t, uint32_t>, float> simmat;
      compMatchingVertex(g1, g2, fvs1, fvs2, simmat);
      float rscore = 0.f;
      vector<pair<uint32_t, uint32_t> > matchPair;
      compAlignment(g1, g2, simmat, rscore, matchPair);

      float sim = calcTanimoto(g1, g2, matchPair);
      if (sim >= threshold) {
	vector<pair<pair<string, pair<uint64_t, uint64_t> >, pair<string, pair<uint64_t, uint64_t> > > > res1;
	postProcess1(g1, g2, matchPair, res1);
	if (res1.size() <= intra_cuts) {
	  vector<pair<string, pair<uint64_t, uint64_t> > > res2;
	  postProcess2(g1, g2, matchPair, res2);
	  if (res2.size() <= inter_cuts) {
            #ifdef _PARALLEL_
            #pragma omp critical
            #endif
            {
              print(ofs, g1, g2, rscore, matchPair);
              ofs << "Tanimoto similarity: " << sim << endl;
              print(ofs, res1);
              print(ofs, res2);
              ofs << endl;
            }
          }
	}
      }
    }
  }
  ofs.close();
  double end = gettimeofday_sec();
  cout << "CPU time = " << end - start << " sec." << endl;
}

void Pacha::run_use_pair(string input_file, string output_file, string pair_file, uint64_t dist, uint64_t topk, float threshold, uint64_t num_threads, uint64_t inter_cuts, uint64_t intra_cuts) {
  topk_ = topk;

  set<pair<string, string> > pairs;
  {
    ifstream ifs(pair_file.c_str());
    if (!ifs) {
      cerr << "cannot open: " << pair_file << endl;
      exit(1);
    }
    readPairFile(ifs, pairs);
    ifs.close();
  }
  
  DIR* dir;
  struct dirent* dp;
  vector<string> names;
  if (NULL == (dir = opendir(input_file.c_str()))){
    cerr << "Directory Open Error." << endl;
    exit(1);
  }
  for(size_t i = 0; NULL != (dp = readdir(dir)); ++i) {
    printf("%ld:%s\n" , i , dp->d_name);
    if (!strcmp(dp->d_name, "."))  continue;
    if (!strcmp(dp->d_name, "..")) continue;
    string d_name(dp->d_name);
    string target = input_file + d_name;
    ifstream ifs(target.c_str());
    if (!ifs) {
      cerr << "cannot open: " << input_file << endl;
      exit(-1);
    }
    read(ifs);
    ifs.close();
  }

  ofstream ofs(output_file.c_str());
  if (!ofs) {
    cerr << "cannot open: " << output_file << endl;
    exit(-1);
  }

  double start = gettimeofday_sec();
  for (size_t i = 0; i < TRANS.size(); ++i) {
    Graph &g1 = TRANS[i];
    string &name1 = g1.name;
    vector<map<uint32_t, float> > fvs1;
    compFeatureVec(g1, dist, fvs1);
    
    #ifdef _PARALLEL_
      omp_set_num_threads(num_threads);
    #endif
    #ifdef _PARALLEL_
      #pragma omp parallel for schedule(static)
    #endif
    for (size_t j = i + 1; j < TRANS.size(); ++j) {
      Graph &g2 = TRANS[j];
      string &name2 = g2.name;
      if (pairs.find(make_pair(name1, name2)) == pairs.end())
	  continue;
      vector<map<uint32_t, float> > fvs2;
      compFeatureVec(g2, dist, fvs2);
      
      map<pair<uint32_t, uint32_t>, float> simmat;
      compMatchingVertex(g1, g2, fvs1, fvs2, simmat);
      float rscore = 0.f;
      vector<pair<uint32_t, uint32_t> > matchPair;
      compAlignment(g1, g2, simmat, rscore, matchPair);

      float sim = calcTanimoto(g1, g2, matchPair);
      if (sim >= threshold) {
	vector<pair<pair<string, pair<uint64_t, uint64_t> >, pair<string, pair<uint64_t, uint64_t> > > > res1;
	postProcess1(g1, g2, matchPair, res1);
	if (res1.size() <= intra_cuts) {
	  vector<pair<string, pair<uint64_t, uint64_t> > > res2;
	  postProcess2(g1, g2, matchPair, res2);
	  if (res2.size() <= inter_cuts) {
            #ifdef _PARALLEL_
            #pragma omp critical
            #endif
            {
              print(ofs, g1, g2, rscore, matchPair);
              ofs << "Tanimoto similarity: " << sim << endl;
              print(ofs, res1);
              print(ofs, res2);
              ofs << endl;
            }
          }
	}
      }
    }
  }
  ofs.close();
  double end = gettimeofday_sec();
  cout << "CPU time = " << end - start << " sec." << endl;
}
