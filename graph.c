#include "graph.h"
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

            if(vertex2 != vertex1){
                add_edge(vertex1, vertex2, g);
//		std::cerr << vertex1 <<" : " << vertex2<<"\n";
		}
        }
    }
}

