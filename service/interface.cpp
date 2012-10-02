#include "ntree.h"

using namespace std;

void query(NTree * data, char * query, char * result) {
  cout << query;
  vector<vector<string> > results = data->search(query);
  string jout = "[";
  /* break after 20 results. TODO: optimize tree memory usage accordingly */
  int limit = RESULTS_LIMIT;
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
  /* iso-8859-1 to utf8 */
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

void full_sort(NTree * data) {
    cerr << "sorting..." << endl;
    data->full_sort();  
    cerr << "done!" << endl;
}

void load(NTree * data, char * code, char * file) {
  ifstream input(file);
    cerr << "loading: " << file << endl;
    string line;
    while (getline(input, line)) {
      if (line[0] != '#') {
	data->add(code, line);
      }
    }
    input.close();
    cerr << "done!" << endl;
}
