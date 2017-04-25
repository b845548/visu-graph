#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>

#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#include <boost/graph/adjacency_list.hpp>


using namespace boost;

typedef struct vertexProperty vertexProperty;
struct vertexProperty
{
    float x, y, vx, vy, gravityDirectionX, gravityDirectionY;
    int rayon,rayon_visual;
    GLuint color;
};
typedef adjacency_list<vecS, vecS, undirectedS, vertexProperty> Graph;
typedef boost::graph_traits <Graph>::edge_iterator edgeIt;


class PhysicGraph{

    private:
        Graph g;
        std::vector<int> * densityGrid = new std::vector<int>[64];
	    int nb_node;
	    int max_adjacent;
	    int rayon;
	    int rayon_visual;
        int screenWidth;    
        int screenHeight;    
	    int gridWidth;    
        int gridHeight;
        int density_limite;    
	    int grid_length;
	    float speed;
	    float gravity;
	    float rebound;
	    float distance_min;
	    float center;
	    float intersection;
	    float progress;
	    float solution;

    public:
        PhysicGraph(void);
        ~PhysicGraph(void);
  
        void initializeRandomNode(void);
        void initializeVertexProperty(void);

        void printEdges(void);
        Graph & data(void);
        bool isFinish(void); 
        void drawNodes(void);
        int catchNode(int,int);

        void collisionMove(double);
        void attractionMove(double);
		void repulsionMove(double);
		void gravityMove(double);

        void intersectionMove(double);
        void densityMove(void);
};

