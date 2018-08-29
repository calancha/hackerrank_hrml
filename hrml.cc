#include <fstream>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
/*
  Comment the following line when running in HackerRank.
  Otherwise read the input from a file 'hrml.data'.
*/
#define DEBUG 1

// 1 <= N <= 20
// 1 <= Q <= 20
// Each line in the source program contains, at max, 200 characters.
// Every reference to the attributes in the Q queries contains at max 200 characters.
// All tag names are unique and the HRML source program is logically correct.
// A tag can have no attributes as well.
  
vector<string> split(const string &s, char delim)
{
  // line: value = "BadVal" size = "10"
  // <b value = "BadVal" size = "10">
  vector<string> elems;
  string input = s;
  size_t its = 0;
  auto it = input.find(delim, its);
  if (it != string::npos) {
    size_t nattr = count(input.begin(), input.end(), delim);
    for (int i=0;i<nattr;++i) {
      string key = input.substr(its, it-1);
      elems.push_back(key); // key
      size_t val_beg = it+3;
      size_t val_end = input.find('"', val_beg);
      // value is between quotes
      string val = input.substr(val_beg, val_end - val_beg);
      elems.push_back(val);
      its = val_end;
      it = input.find(delim, its);
      if (it == string::npos) break;
      input = input.substr(its + 2);
      its = 0;
      it = input.find(delim, its);
    }
  }
  return elems;
}

vector<string> attributes(string line)
{
  size_t val_start = line.find(" ");
  if (line[1] == '/' || val_start == string::npos)
    return vector<string>();
  line.erase(0, val_start + 1);
  line.pop_back();
  vector<string> key_val = split(line, '=');
  return key_val;
}

struct Tag {
  string name;
  vector<string> keys;
  vector<string> values;
  string child;
  string call;
};

typedef struct Tag st_tag;

string get_name(string line)
{
  if (line[1] == '/') return "";
  size_t end_name = min(line.find(' '), line.find('>'));
  return line.substr(1, end_name - 1);
}

void get_key_vals(vector<string> &keys, vector<string> &values, string line)
{
  vector<string> key_val = attributes(line);
  for (size_t i=0;i<key_val.size();++i) {
    if (i%2==0)
      keys.push_back(key_val[i]);
    else
      values.push_back(key_val[i]);
  }
}

size_t get_val(string key, st_tag tag)
{
  vector<string> keys = tag.keys;
  vector<string> values = tag.values;
  auto it = find_if(keys.begin(),
                    keys.end(),
                    [&key](string &s) { return s.compare(key) == 0; });
  if (it == keys.end())
    return values.size();
  else
    return it - keys.begin();
}

string get_child_name(size_t idx, vector<string> lines)
{
  if (idx == lines.size() - 1) return "";
  if (lines[idx+1][1] == '/') return "";
  return get_name(lines[idx+1]);
}

string get_call(size_t idx, vector<string> lines)
{
  vector<string> parents;
  for (size_t i=0;i<idx;++i) {
    string line = lines[i];
    string tag_name = get_name(line);
    if (line[1] == '/') {
      parents.pop_back();
    } else {
      parents.push_back(tag_name);      
    }
  }
  string res;
  for (const auto &e : parents) res += e + ".";
  res += get_name(lines[idx]) + ".";
  if (!res.empty()) res.pop_back();

  return res;
}

void fill_tags(vector<st_tag> &tags, vector<string> lines)
{
  for (size_t i=0;i<lines.size();++i) {
    string line = lines[i];
    st_tag node;
    if (line[1] == '/') continue;
    string name = get_name(line);
    vector<string> keys;
    vector<string> values;
    get_key_vals(keys, values, line);
    
    node.name = name;
    node.keys = keys;
    node.values = values;
    node.child = get_child_name(i, lines);
    node.call = get_call(i, lines);
    tags.push_back(node);
  }
}

const st_tag &find_tag_by_name(string name, vector<st_tag> &tags)
{
  auto it = find_if(tags.begin(),
                    tags.end(),
                    [&name](st_tag &tag) { return name.compare(tag.name) == 0; });
  return *it;
}

int main() {
  /* Enter your code here. Read input from STDIN. Print output to STDOUT */
#ifdef DEBUG
  ifstream inFile;
  inFile.open("hrml.data");
#endif
  int t, q;
#ifdef DEBUG
#define INFILE inFile
#else
#define INFILE cin
#endif  
  INFILE >> t >> q;
  string line;
  getline(INFILE, line);
  vector<string> lines;
  vector<string> queries;
  while (t--) {
    getline(INFILE, line);
    lines.push_back(line);
  }
  while (q--) {
    getline(INFILE, line);
    queries.push_back(line);
  }
  vector<st_tag> doc_tags;
  fill_tags(doc_tags, lines);
  for (size_t i=0;i<queries.size();++i) {
    string query = queries.at(i);
    string call = query.substr(0, query.find("~"));
    string ctag = call;
    if (call.find(".") != string::npos)
      ctag = call.substr(call.find_last_of(".")+1, string::npos);
    const st_tag &tag = find_tag_by_name(ctag, doc_tags);
    if (call.compare(tag.call) != 0) {
      cout << "Not Found!" << endl;
      continue;
    }
    string key = query.substr(query.find("~") + 1);
    size_t val_pos = get_val(key, tag);
    if (val_pos != tag.values.size()) {
      string val = tag.values.at(val_pos);
      cout << val << endl;
    } else
      cout << "Not Found!" << endl;
  }

#ifdef DEBUG
  inFile.close();
#endif  
  return 0;
}