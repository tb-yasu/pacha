/*
 * Main.cpp
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

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "Pacha.hpp"
#include "cmdline.h"

using namespace std;

void version(const string &ver) {
  cerr << "Pacha++ version: " << ver << endl;
  cerr << "Written by Yasuo Tabei(yasuo.tabei@gmail.com)" << endl;
  cerr << endl;
}

int main(int argc, char **argv) {
  cmdline::parser p;
  p.add<string>("input_file",  'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", true);
  p.add<string>("pair_file", 's', "pair file name", false, "");
  p.add<uint64_t>("dist", 'd', "maximum distance for computing vertex features", false, 5);
  p.add<uint64_t>("topk", 'k', "number of alignments starting from vertex pairs of topk matching scores", false, 10);
  p.add<uint64_t>("num_threads", 'n', "# of threads", false, 1); 
  p.add<float>("threshold", 't', "theshold", false, 0.f);
  p.add<uint64_t>("inter_cuts", 'c', "maximum number of intermolecular cuts", false, 100000000);
  p.add<uint64_t>("intra_cuts", 'a', "maximum number of intramolecular cuts", false, 100000000);
  p.parse_check(argc, argv);
  const string input_file  = p.get<string>("input_file");
  const string output_file = p.get<string>("output_file");
  const string pair_file   = p.get<string>("pair_file");
  uint64_t dist = p.get<uint64_t>("dist");
  uint64_t topk = p.get<uint64_t>("topk");
  uint64_t num_threads = p.get<uint64_t>("num_threads");
  float threshold = p.get<float>("threshold");
  uint64_t inter_cuts = p.get<uint64_t>("inter_cuts");
  uint64_t intra_cuts = p.get<uint64_t>("intra_cuts");

  version("3.0.0");

  Pacha pacha;
  if (pair_file == "") 
    pacha.run(input_file, output_file, dist, topk, threshold, num_threads, inter_cuts, intra_cuts);
  else
    pacha.run_use_pair(input_file, output_file, pair_file, dist, topk, threshold, num_threads, inter_cuts, intra_cuts);
}


