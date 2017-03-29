/*!\file window.c
 * \brief GL4D-Primitives et modèle simple de balle avec rebonds. Il
 * faut ajouter la prise en compte du temps, du rayon de la balle et
 * des bugs de type blocage de la balle (quand la gestion du temps sera
 * ajoutée).
 *
 * gestion de gravité, correction de bug et multi-balles ajoutée le 13/02/17
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 02 2017
 */
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/graphviz.hpp>
#include <assert.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
using namespace boost;
using std::vector;

typedef adjacency_list<vecS, vecS, undirectedS, no_property> Graph;


typedef struct mobile_t mobile_t;

struct mobile_t {
  float x, y, vx, vy, ax, ay;
  int r;
  GLuint color;
};
static mobile_t * _mobiles = NULL;
static int _nbMobiles = 1;
static float _G[] = {0, -980};

#define NB_MOBILE 30
static Graph g;
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
		std::cerr << vertex1 <<" : " << vertex2<<"\n";
		}
        }
    }
}

void parcours(const Graph& g){
 
    for (auto pair_it2 = vertices(g);
         pair_it2.first != pair_it2.second;
         ++pair_it2.first) { 
  	  for (auto pair_it = adjacent_vertices(*pair_it2.first, g);pair_it.first !=pair_it.second; ++pair_it.first) {
    gl4dpSetColor(_mobiles[0].color);
gl4dpLine(_mobiles[*pair_it.first].x, _mobiles[*pair_it.first].y,_mobiles[*pair_it2.first].x, _mobiles[*pair_it2.first].y);

    gl4dpSetColor(_mobiles[*pair_it.first].color);
  gl4dpFilledCircle(_mobiles[*pair_it.first].x, _mobiles[*pair_it.first].y, _mobiles[*pair_it.first].r/2);  
               
        }
    }

}







void mobileInit(int n) {
  int i;
  _nbMobiles = n;
  _mobiles = (mobile_t*)malloc(_nbMobiles * sizeof *_mobiles);
  assert(_mobiles);

  for(i = 0; i < _nbMobiles; i++) {
    _mobiles[i].x = gl4dmURand() * gl4dpGetWidth();
    _mobiles[i].y = gl4dmURand() * gl4dpGetHeight();
    _mobiles[i].vx = gl4dmSURand() * gl4dpGetWidth() / 3.0;
    _mobiles[i].vy = gl4dmURand() * gl4dpGetHeight() / 3.0;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = 15;//5.0 + gl4dmURand() * gl4dpGetWidth() / 20.0;
    _mobiles[i].color = rand();
  }

    random_graph(g, 50, 2);
} 

void mobileMove(void) {
  static GLuint t0 = 0;
  GLuint t;
  GLfloat dt;
  int i,j;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;
  for(j = 0; j < _nbMobiles; j++) 
  for(i = 0; i < _nbMobiles; i++) 
if(i!=j&&pow(_mobiles[j].x-_mobiles[i].x,2)+pow(_mobiles[j].y-_mobiles[i].y,2)<=pow(_mobiles[j].r+_mobiles[i].r,2)){
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
  //  _mobiles[i].vx += _mobiles[i].ax * dt;
  //  _mobiles[i].vy += _mobiles[i].ay * dt;
    if((_mobiles[i].x - _mobiles[i].r < 0 && _mobiles[i].vx < 0) || 
       (_mobiles[i].x + _mobiles[i].r >= gl4dpGetWidth() && _mobiles[i].vx > 0)) {
      _mobiles[i].vx = -_mobiles[i].vx;
      _mobiles[i].vx -= _mobiles[i].vx * 0.1;
      _mobiles[i].vy -= _mobiles[i].vy * 0.05;
    }
    if((_mobiles[i].y - _mobiles[i].r < 0 && _mobiles[i].vy < 0) || 
       (_mobiles[i].y + _mobiles[i].r >= gl4dpGetHeight() && _mobiles[i].vy > 0)) {
      _mobiles[i].vy = -_mobiles[i].vy;
      _mobiles[i].vy -= _mobiles[i].vy * 0.1;
      _mobiles[i].vx -= _mobiles[i].vx * 0.05;
    }
  }
}

void mobileDraw(void) {
//  int i;
parcours(g);
/*
  for(i = 0; i < _nbMobiles; i++) {
    gl4dpSetColor(_mobiles[i].color);
    gl4dpFilledCircle(_mobiles[i].x, _mobiles[i].y, _mobiles[i].r);
  }*/

}

static void idle(void) {
  mobileMove();
}

static void draw(void) {
  gl4dpClearScreenWith (RGB(0, 0, 0));
  mobileDraw();
  gl4dpUpdateScreen(NULL);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés.*/
static void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
/*!\brief créé la fenêtre, un screen 2D, place la fonction display et
 * appelle la boucle principale.*/









int main(int argc, char ** argv) {
  /* fenêtre positionnée en (10, 10), ayant une dimension de (512, 512) et un titre "GL4D-Primitives" */
  if(!gl4duwCreateWindow(argc, argv, "GL4D-BaBalle", 10, 10, 512, 512, SDL_WINDOW_SHOWN))
    return 1;
  gl4dpInitScreen();
  mobileInit(100);
  atexit(quit);
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
