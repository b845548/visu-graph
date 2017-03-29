#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/graphviz.hpp>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include <iostream>

using namespace boost;
using std::vector;

typedef adjacency_list<vecS, vecS, undirectedS, no_property> Graph;

template <class Palette, class Color>
class color_writer {
public:
    color_writer(Palette palette, Color& color):
    palette_(palette),
    color_(color)
    {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        assert(color_[v] < 6);
        out << "[color=\"" << palette_[color_[v]] << "\"]";
    }
private:
    Palette palette_;
    Color& color_;
};


void random_graph(Graph& g, int nb_vertex, int perc) {
    // Ajout de nb_vertex sommets
    for(int i = 0; i < nb_vertex; ++i)
        add_vertex(g);
    
    // Générateur de nombres pseudo-aléatoires
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> dis(0, nb_vertex - 1);
    std::uniform_int_distribution<unsigned long> dis2(1, nb_vertex * perc / 100);
    
    // Ajout d'arêtes
    for (auto pair_it = vertices(g);
         pair_it.first != pair_it.second;
         ++pair_it.first) {
        int nb_edge = dis2(gen);
        for(int i = 0; i < nb_edge; ++i) {
            auto vertex1 = *pair_it.first;
            auto vertex2 = dis(gen);
            if(vertex2 != vertex1)
                add_edge(vertex1, vertex2, g);
        }
    }
}

int coloration(const Graph& g, vector<int>& color) {
    vector<int> vertex(num_vertices(g));
    std::generate_n(vertex.begin(), vertex.size(), []() -> int { static int i = 0; return i++; });
    std::cerr << "sorting" << '\n';
    std::sort(vertex.begin(), vertex.end(),
              [&g](auto a, auto b) { return out_degree(a, g) > out_degree(b, g); }
              );
    std::cerr << "sorted" << '\n';
    color.resize(num_vertices(g), -1);
    int k;
    int colored = num_vertices(g);
    for(k = 0; colored != 0; ++k) {
        std::cerr << "color : " << k << '\n';
        for(auto x : vertex) {
            if(color[x] == -1) {
                bool same_color = false;
                for (auto pair_it = adjacent_vertices(x, g);
                     pair_it.first != pair_it.second;
                     ++pair_it.first) {
                    if(color[*pair_it.first] == k) {
                        same_color = true;
                        break;
                    }
                }
                if(!same_color) {
                    color[x] = k;
                    std::cerr << "vertex : " << x << '\n';
                    --colored;
                }
            }
        }
    }
    return k;
}

int main() {
    Graph g;
    random_graph(g, 100, 2);
    vector<int> color;
    coloration(g, color);
    
    const char* palette[] = { "#ff0000", "#00ff00", "#0000ff", "#ffff00", "#00ffff", "#ff00ff" };
    
    
    write_graphviz(std::cout, g, color_writer<const char**, vector<int>>(palette, color));
    
    return 0;
}

