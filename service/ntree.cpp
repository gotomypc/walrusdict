#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>

#define MAX_DEPTH 8

/* The ntree is requesting all the necessary memory right at startup,
   hence no calls to free. */

using namespace std;

class Node {

public: 
  int depth;
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

private:
  Node * root;
  int insert(Node * node, string key, vector<string> data);
  string normalize(string val);
  int key_index(string s);
  
};

NTree::NTree() {
  root = new Node(0);
}

int NTree::key_index(string key) {
  int index = key[0];
  if (key[0] >= 'a' && key[0] <= 'z')
    index -= 'a';
  else
    index = 26;  
  return index;
}

vector<vector<string> > NTree::search(string val) {    
  int depth = 0;
  vector<vector<string> > results;
  Node * node = root;
  
  string key = normalize(val);
  //cout << " searching " << key << endl;

  while (key.size() > 0 && depth < MAX_DEPTH) {
    int index = key_index(key);
    
    if(node->next[index] != NULL)
      node = node->next[index];
    else 
      break;
    key = key.substr(1);
    depth++;
  }
  return node->vals;
}


int NTree::insert(Node * node, string key, vector<string> data) {
  int index = key_index(key);
  node->vals.push_back(data);

  string sub = key.substr(1);
  if (node->depth < MAX_DEPTH and sub.size() > 0) {
    if (node->next[index] == NULL)
      node->next[index] = new Node(node->depth + 1);        
    /* recursion */    
    return insert(node->next[index], sub, data);
  }
  return node->depth;
}

int NTree::add(string code, string val) {
  string final(val), norm, item;

  vector<string> entry;
  entry.push_back(code);


  stringstream ss(val);
  while(std::getline(ss, item, '\t')) {
    entry.push_back(item);
  }

  norm = normalize(entry[1]);
  
  insert(root, norm, entry);
  
  return 0;
}

string NTree::normalize(string val) {
  char from[] = { 0xe8, 0xf2, 0xf9, 0xe0, 0xcc,  /* lowercase accents */
		  0xf6, 0xfc, 0xe4, /* lowercase umlaute */
		  0xc1, 0xc2, 0xc8, 0xc9, 0xcc, 0xcd, 0xd2, 0xd3, 0xd9, 0xda, /* uppercase accents*/
		  0xd6, 0xdc, 0xc4, /* uppercase umlaute */
		  0 };
  char to[] = 
    "eouai"
    "oua"
    "AAEEIIOOUU"
    "OUA";
  int erase_start=-1, erase_end=-1;

  for(int i=0;i<val.size();i++) {        
    if(val[i] == '{')
      erase_start=i;

    if(val[i] == '}')
      erase_end=i;

    for(int k=0;from[k] != 0;k++) {
      if (val[i] == from[k])
	val[i] = to[k];
    }
  }

  if (erase_start >= 0 && erase_end >= 0) {
    val.erase(erase_start, 2 + erase_end-erase_start); 
  }

  transform(val.begin(), val.end(), val.begin(), ::tolower);


  return val;
}

void query(NTree * data, char * query, char * result) {
  cout << query;
  vector<vector<string> > results = data->search(query);
  string jout = "[";
  /* break after 20 results */
  int limit = 20;
  for(vector<vector<string> >::iterator it = results.begin(); it != results.end(); it++) {
    if(it != results.begin()) jout += ","; 
    jout += "[";
    for(vector<string>::iterator kt = (*it).begin(); kt != (*it).end(); kt++) {
      if(kt != (*it).begin()) jout += ","; 
      jout += '"' + (*kt) + '"';
    }
    jout += ']';
    if (--limit <= 0)
      break;
  }
  jout += "]";
  unsigned char *in = (unsigned char *) jout.c_str(), *out = (unsigned char *) result;
  while (*in)
    if (*in<128) *out++=*in++;
    else *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
  *out = 0;
}

NTree * init() {
  NTree * data = new NTree();
  return data;
}

void load(NTree * data, char * code, char * file) {
  ifstream input(file);
    cerr << "loading: " << file << endl;
    string line;
    while (getline(input, line)) {
      data->add(code, line);
    }
    input.close();  
}
