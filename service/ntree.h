#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

#define MAX_DEPTH 8
#define RESULTS_MINDEPTH 2
#define RESULTS_LIMIT 40


class Node {

public: 
  int depth;
  string key;
  Node(int d) : next(), depth(d) {}
  Node * next[27];
  vector<vector<string> > vals;
};


class NTree {
public:
  NTree();
  int add(string code, string val);
  int clear();
  vector<vector<string> > search(string val);
  void full_sort();

private:
  Node * root;
  int insert(Node * node, string key, vector<string> data);
  //  string normalize(string val);
  string artikel(string val);
  string articolo(string val);
  int key_index(string s);
  void deep_sort(Node * node);
  // bool sortstrcmp(vector<string> a, vector <string> b);
};
