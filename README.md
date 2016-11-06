PACHA: Pairwise Chemical Aligner
====

## Overview
PACHA - a chemical graph alignment software. 

## Description
PACHA is a chemical graph alignment software. Given two chemical compounds represented as small graphs,
PACHA outputs an alignment, a.k.a atom-atom mapping, between two compounds. Graph alignements are useful
for building from a compound-compound pair a feature vector that can be used for an input of a machine learning,
e.g., support vector machine.
Main applications are reconstructions of metabolic pathway and virtual screening, etc.
See [1] for detail. 

## Demo
cd pacha/src
make
./pacha --input_file=../dat/target_compounds/ --output_file=output.txt

--input_file=[DIR] : [DIR] is a directory including chemical graph files in the mol format. 
--input_file=[FILE] : [FILE] is an output FILE
## VS. 

## Requirement
GNU C++ compiler

## Usage

## Install

cd pacha/src
make

## Licence


## Author

Yasuo Tabei (yasuo.tabei@gmail.com)
[tb-yasu](https://github.com/tb-yasu)

## Reference
[1] Yoshihiro Yamanishi*, Yasuo Tabei*, Masaaki Kotera: Metabolome-scale de novo pathway reconstruction using regioisomer-sensitive graph alignments, In Proceedings of ISMB/ECCB, 2015. (*joint first author)