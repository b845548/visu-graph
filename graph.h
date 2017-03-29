#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/graphviz.hpp>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
using namespace boost;
using std::vector;

typedef adjacency_list<vecS, vecS, undirectedS, no_property> Graph;


