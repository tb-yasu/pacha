PACHA: Pairwise Chemical Aligner
====

## Overview
PACHA - a chemical graph alignment software. 

## Description
PACHA is a chemical graph alignment software. Given two chemical compounds represented as small graphs,
PACHA outputs an alignment, a.k.a atom-atom mapping, between two compounds. Graph alignements are useful
for building from a compound-compound pair a feature vector that can be used for an input of a machine learning,
e.g., support vector machine. Main applications are reconstructions of metabolic pathway and virtual screening, etc.
See [1] for detail. 

## Demo
cd pacha/src  
make  
./pacha --input_file=../dat/target_compounds/ --output_file=output.txt  
output.txt includes alignment results between all pairs of cheical graphs in ../dat/target_compounds/

## Requirement

GNU C++ compiler  
Higher version is preferable if you use multiple threads. 

## Usage
usage: ./pacha --input_file=string --output_file=string [options] ...  
options:  
  -i, --input_file     input file name (string)  
  -o, --output_file    output file name (string)  
  -d, --dist           maximum distance for computing vertex features (unsigned long long [=5])  
  -k, --topk           number of alignments starting from vertex pairs of topk matching scores (unsigned long long [=10])  
  -n, --num_threads    # of threads (unsigned long long [=1])  
  -t, --threshold      theshold (float [=0])  
  -c, --inter_cuts     maximum number of intermolecular cuts (unsigned long long [=100000000])  
  -a, --intra_cuts     maximum number of intramolecular cuts (unsigned long long [=100000000])  
  -?, --help           print this message  

## Install

cd pacha/src  
make

## Licence

New BSD Licence

## Author

Yasuo Tabei (yasuo.tabei@gmail.com)  
[Homepage](https://sites.google.com/site/yasuotabei/)  
[tb-yasu](https://github.com/tb-yasu)

## Reference
[1] Yoshihiro Yamanishi*, Yasuo Tabei*, Masaaki Kotera: Metabolome-scale de novo pathway reconstruction using regioisomer-sensitive graph alignments, In Proceedings of ISMB/ECCB, 2015. (*joint first author)
