#include "ntree.h"

/* The ntree is requesting all the necessary memory right at startup,
   hence no calls to free. */

using namespace std;

NTree::NTree() {
  root = new Node(0);
  root->key = "";
}

void NTree::full_sort() {
  deep_sort(root);
}

bool charcmp(char a, char b) {
  return tolower(a) < tolower(b);
}
 
bool sortstrcmp(vector<string> a, vector <string> b)
{
  return lexicographical_compare(a[1].begin(),a[1].end(),b[1].begin(),b[1].end(), charcmp);
}

void NTree::deep_sort(Node * node){
  /* recursively traverse the tree to sort the vals */
  // cerr << node->key << endl;
  
  sort(node->vals.begin(),node->vals.end(),sortstrcmp);
  for (int i=0; i<27; i++) {
    Node * tgt = node->next[i];
    if (tgt != NULL)
      deep_sort(tgt);
  }
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

  if(node->depth >= RESULTS_MINDEPTH)
    node->vals.push_back(data);

  string sub = key.substr(1);
  if (node->depth < MAX_DEPTH && sub.size() > 0) {
    if (node->next[index] == NULL) {
      node->next[index] = new Node(node->depth + 1);
      node->next[index]->key = node->key + sub[0];
    }
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

  if(entry.size() < 3) {
    cerr << "skipping: " << val << endl;
    return 0;
  }

  /* DE fix */
  if(strncmp(entry[0].c_str(),"DE",2)==0) {
    entry[1] = artikel(entry[1]);
  } else if (strncmp(entry[0].c_str()+ 3,"DE",2)==0) {
    entry[2] = artikel(entry[2]);
  }

  /* IT fix */  
  if(strncmp(entry[0].c_str(),"IT",2)==0) {
    entry[1] = articolo(entry[1]);
  } else if (strncmp(entry[0].c_str()+ 3,"IT",2)==0) {
    entry[2] = articolo(entry[2]);
  }


  // cerr << "adding: " << entry[0] << " " << entry[1] << " - " << entry[2] << endl;

  norm = normalize(entry[1]);
  
  insert(root, norm, entry);
  
  return 1;
}

string NTree::articolo(string val) {
  int erase_start = -1, erase_end = -1;
  for(int i=0;i<val.size();i++) {
    if(val[i] == '{')
      erase_start=i;
    
    if(val[i] == '}')
      erase_end=i;
  }  

  if (erase_start >= 0 && erase_end >= 0) {
    val.erase(erase_start, 2 + erase_end-erase_start); 
  }
  return val;
}

string NTree::artikel(string val) {
  int erase_start = -1, erase_end = -1, del = 0;
  char gen = 0;

  for(int i=0;i<val.size();i++) {
    if (erase_start >= 0 && gen == 0 )
      gen=val[i];

    if(val[i] == '{')
      erase_start=i;
    
    if(val[i] == '}')
      erase_end=i;
  }
  
  switch(gen) {
  case 'm':
    val = "der " + val;
    del = 4;
    break;
  case 'f':
    val = "die " + val;
    del = 4;
    break;
  case 'n':
    val = "das " + val;
    del = 4;
    break;
  default:
    del = 0;
  }
  
  if (del > 0 && erase_start >= 0 && erase_end >= 0) {
    val.erase(4 + erase_start, 4 + 2 + erase_end-erase_start); 
  }

  return val;
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
