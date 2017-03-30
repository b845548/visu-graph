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
#include <assert.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/graphviz.hpp>
#include <random>
#include <cassert>
#include <algorithm>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, no_property> Graph;
typedef boost::graph_traits <Graph>::edge_iterator edgeIt;

typedef struct graph_parametre graph_parametre;
struct graph_parametre{
	int nb_node;
	int max_adjacent;
	int rayon;
	int rayon_visual;
	float speed;
	float gravity;
	float rebound;
	float distance_min;
	float center;
	float intersection;
	float progress;
	float solution;
};
typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, vx, vy, ax, ay;
  int r,r_vis;
  GLuint color;
};

static mobile_t * _mobiles = NULL;
static int _nbMobiles = 1;
static float _G[] = {0, -980};
static graph_parametre _gp;
static Graph _g;

void graph_random(Graph& g, int nb_vertex, int perc) {
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
		}
        }
    }
}
void graph_init(){
_gp.nb_node=50; // nb node
_gp.max_adjacent=3;// max adjacent
_gp.rayon=25;// point phisque
_gp.rayon_visual=5; // point visible
_gp.speed=3.0; // rapidite de deplacement vx initial
_gp.center=0.01; // facteur pour centrer un sommet autours les adjacents, moyenne == 0 on est au centre 
_gp.progress=1.00;// progresse de 1 vers 0
_gp.solution=0.045;// solution opmtimale
_gp.gravity=0.007; // gravite vers centre, pour eviter que le sommet se deplace vers le frontier
_gp.rebound=0.3; // facteur de rebondisement
_gp.distance_min=100.; //  moyenne de la distance des adjacents
_gp.intersection=0.3; // facteur intersection

graph_random(_g, _gp.nb_node, _gp.max_adjacent);
for(std::pair<edgeIt,edgeIt> ei = boost::edges(_g); ei.first != ei.second; ++ei.first) {
        std::cout << source(*ei.first, _g) << " -> " << target(*ei.first, _g) << std::endl;
    }
}
bool is_intersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    float denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
    float numerator1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
    float numerator2 = ((y1 - y3) * (x2 -  x1)) - ((x1 - x3) * (y2 - y1));

    // Detect coincident lines (has a problem, read below)
    if (denominator == 0) return numerator1 == 0 && numerator2 == 0;
    float r = numerator1 / denominator;
    float s = numerator2 / denominator;

    return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}


void graph_draw_intersection(){
	int s1,s2,s3,s4;
	float dist_x,dist_y;
int count=0,nb_eg=0;
for(std::pair<edgeIt,edgeIt> ei2 = boost::edges(_g); ei2.first != ei2.second; ++ei2.first) {
//    std::cout << source(*ei2.first, _g) << " -> " << target(*ei2.first, _g) << std::endl;
	nb_eg++;
	for(std::pair<edgeIt,edgeIt> ei = boost::edges(_g); ei.first != ei.second; ++ei.first) {
		s1=source(*ei.first, _g);
		s2=target(*ei.first, _g);
		s3=source(*ei2.first, _g);
		s4=target(*ei2.first, _g);
		if(s1!=s3&&s2!=s4)
		if(s1!=s2&&s3!=s4)
		if(s1!=s4&&s2!=s3){
			if(is_intersection(_mobiles[s1].x,_mobiles[s1].y,_mobiles[s2].x,_mobiles[s2].y
			,_mobiles[s3].x,_mobiles[s3].y,_mobiles[s4].x,_mobiles[s4].y)){
				gl4dpSetColor(RGB(255,0,0));
				gl4dpLine(_mobiles[s1].x, _mobiles[s1].y,_mobiles[s2].x, _mobiles[s2].y);
				gl4dpLine(_mobiles[s3].x, _mobiles[s3].y,_mobiles[s4].x, _mobiles[s4].y);
			count++;
			dist_x=_mobiles[s1].x -_mobiles[s2].x;
			dist_y=_mobiles[s1].y -_mobiles[s2].y;
				int rd=rand()%4;				
				if(rd==0){
				_mobiles[s1].x-=dist_x*_gp.intersection*_gp.progress;
				_mobiles[s1].y-=dist_y*_gp.intersection*_gp.progress;
				}else if(rd==1){
				_mobiles[s2].x+=dist_x*_gp.intersection*_gp.progress;
				_mobiles[s2].y+=dist_y*_gp.intersection*_gp.progress;				
				}else if(rd==2){
				_mobiles[s3].x-=dist_x*_gp.intersection*_gp.progress;
				_mobiles[s3].y-=dist_y*_gp.intersection*_gp.progress;				
				}else if(rd==3){
				_mobiles[s4].x+=dist_x*_gp.intersection*_gp.progress;
				_mobiles[s4].y+=dist_y*_gp.intersection*_gp.progress;				
				}


		
			}
		}

    }
}
    std::cout << _gp.progress<< std::endl;
_gp.progress=(count*2/8)/(float)nb_eg;
}




void graph_collision(){
	static GLuint t0 = 0;
	GLuint t;
	GLfloat dt;
	int i,j;
	t = SDL_GetTicks();
	dt = (t - t0) / 1000.0;
	t0 = t;

	for(i = 0; i < _nbMobiles; i++){ 
		// collision dans le cadre
		if(_mobiles[i].x + _mobiles[i].r >gl4dpGetWidth()) 
			 	_mobiles[i].x = gl4dpGetWidth()-_mobiles[i].r;
		if(_mobiles[i].x - _mobiles[i].r < 0) 
			 	_mobiles[i].x = _mobiles[i].r;
		if(_mobiles[i].y + _mobiles[i].r >gl4dpGetHeight()) 
			 	_mobiles[i].y = gl4dpGetHeight()-_mobiles[i].r;
		if(_mobiles[i].y - _mobiles[i].r < 0) 
			 	_mobiles[i].y = _mobiles[i].r;
		if((_mobiles[i].x - _mobiles[i].r < 0 && _mobiles[i].vx < 0) || 
		(_mobiles[i].x + _mobiles[i].r >= gl4dpGetWidth() && _mobiles[i].vx > 0))
			 	_mobiles[i].vx = -_mobiles[i].vx;
		if((_mobiles[i].y - _mobiles[i].r <= 0 && _mobiles[i].vy < 0) || 
		(_mobiles[i].y + _mobiles[i].r >= gl4dpGetHeight()&& _mobiles[i].vy > 0 ))
				_mobiles[i].vy = -_mobiles[i].vy;
		// collision dans le cadre
		for(j = 0; j < _nbMobiles; j++){ 
			if(i!=j&&pow(_mobiles[j].x-_mobiles[i].x,2)+pow(_mobiles[j].y-_mobiles[i].y,2)
			<=pow(_mobiles[j].r+_mobiles[i].r,2)){
				if(rand()%2==0){// eloigner i
					_mobiles[i].x -= (_mobiles[j].x - _mobiles[i].x) * _gp.rebound*_gp.progress;
					_mobiles[i].y -= (_mobiles[j].y - _mobiles[i].y) * _gp.rebound*_gp.progress;
				}else{// eloigner j
					_mobiles[j].x += (_mobiles[j].x - _mobiles[i].x) * _gp.rebound*_gp.progress;
					_mobiles[j].y += (_mobiles[j].y - _mobiles[i].y) * _gp.rebound*_gp.progress;
				}

				_mobiles[i].x -= (_mobiles[i].x - gl4dpGetWidth()/2) * _gp.gravity*_gp.progress;
				_mobiles[i].y -= (_mobiles[i].y - gl4dpGetHeight()/2) * _gp.gravity*_gp.progress;
				_mobiles[i].x += _mobiles[i].vx * dt*_gp.progress;
				_mobiles[i].y += _mobiles[i].vy * dt*_gp.progress;
	  		}
		}
	}
}
void graph_centrer(const Graph& g){
	float moyenne_x,moyenne_y,dist_x,dist_y;
	int nb_edge;
	
	for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first) { 
  		moyenne_x=moyenne_y=0.0;
		nb_edge=0;
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first){
			nb_edge++;
			dist_x=_mobiles[*it_a.first].x -_mobiles[*it_v.first].x;
			dist_y=_mobiles[*it_a.first].y -_mobiles[*it_v.first].y;
			moyenne_x+=dist_x;
			moyenne_y+=dist_y;
/*
			if(dist_x>_gp.distance_min){
				_mobiles[*it_a.first].x -= dist_x * 0.1*_gp.progress;
				_mobiles[*it_a.first].vx -= dist_x * _gp.step*_gp.progress;
			}
			if(dist_y>_gp.distance_min){
				_mobiles[*it_a.first].y -= dist_y * 0.1*_gp.progress;
				_mobiles[*it_a.first].vy -= dist_y * _gp.step*_gp.progress;
			}
*/  
  	}
		if(nb_edge){
			moyenne_x = moyenne_x / (float)nb_edge;	
			moyenne_y = moyenne_y / (float)nb_edge;
			if(moyenne_x>_gp.distance_min){
				_mobiles[*it_v.first].x += moyenne_x * _gp.center*_gp.progress;
				_mobiles[*it_v.first].vx += moyenne_x * _gp.center*_gp.progress;
			}
			if(moyenne_y>_gp.distance_min){
				_mobiles[*it_v.first].y += moyenne_y * _gp.center*_gp.progress;
				_mobiles[*it_v.first].vy += moyenne_y * _gp.center*_gp.progress;
			}
		}
    }
}

void graph_draw_node(const Graph& g){
    for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first){ 
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first){

				gl4dpSetColor(RGB(0,255,0));
				gl4dpLine(_mobiles[*it_v.first].x,_mobiles[*it_v.first].y,_mobiles[*it_a.first].x,_mobiles[*it_a.first].y);

			gl4dpSetColor(_mobiles[*it_a.first].color);
  			gl4dpFilledCircle(_mobiles[*it_a.first].x
			, _mobiles[*it_a.first].y, _mobiles[*it_a.first].r_vis);  
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
    _mobiles[i].vx = gl4dmSURand() * gl4dpGetWidth() / _gp.speed;
    _mobiles[i].vy = gl4dmSURand() * gl4dpGetHeight() / _gp.speed;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = _gp.rayon;
    _mobiles[i].r_vis = _gp.rayon_visual;
    _mobiles[i].color = rand();
  }
} 

static void idle(void) {
  if(_gp.progress>_gp.solution){
	graph_centrer(_g);
  }
	graph_collision();
}
static void mouse(int button, int state, int x, int y) {
  if(button == GL4D_BUTTON_LEFT&&state) {
    y = gl4dpGetHeight() - y;
	int i;
	for(i = 0; i < _nbMobiles; i++){ 
			if(pow(x-_mobiles[i].x,2)+pow(y-_mobiles[i].y,2)<=pow(_mobiles[i].r_vis,2)){
			fprintf(stderr,"%d ",i);
			break;	
	  		}
		}
  }
}
static void draw(void) {
  gl4dpClearScreenWith (RGB(0, 0, 0));
  graph_draw_node(_g);
  if(_gp.progress>_gp.solution)
     graph_draw_intersection();
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
  graph_init();
  mobileInit(_gp.nb_node);
  atexit(quit);
  gl4duwMouseFunc(mouse);
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
