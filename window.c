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
#include "graph.h"


typedef struct mobile_t mobile_t;

struct mobile_t {
  float x, y, vx, vy, ax, ay;
  int r,r_vis;
  GLuint color;
};
static mobile_t * _mobiles = NULL;
static int _nbMobiles = 1;
static float _G[] = {0, -980};

static Graph g;
extern void random_graph(Graph& g, int nb_vertex, int perc);


void parcours(const Graph& g){
	float moyenne_x,moyenne_y,dist_x,dist_y;
	int nb_edge;
	float distance_mini=100; 
    
	for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first) { 
  		moyenne_x=moyenne_y=0.0;
		nb_edge=0;
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first){
			nb_edge++;
			dist_x=_mobiles[*it_a.first].x -_mobiles[*it_v.first].x;
			dist_y=_mobiles[*it_a.first].y -_mobiles[*it_v.first].y;
			moyenne_x+=dist_x;
			moyenne_y+=dist_y;
			if(dist_x>distance_mini){
				_mobiles[*it_a.first].vx -= dist_x * 0.01;
				_mobiles[*it_a.first].x -= dist_x * 0.1;
			}
			if(dist_y>distance_mini){
				_mobiles[*it_a.first].vy -= dist_y * 0.01;
				_mobiles[*it_a.first].y -= dist_y * 0.1;
			}
    	}
		if(nb_edge){
			moyenne_x = moyenne_x / (float)nb_edge;	
			moyenne_y = moyenne_y / (float)nb_edge;
			if(moyenne_x>distance_mini){
				_mobiles[*it_v.first].x += moyenne_x * 0.01;
				_mobiles[*it_v.first].vx += moyenne_x * 0.1;
			}
			if(moyenne_y>distance_mini){
				_mobiles[*it_v.first].y += moyenne_y * 0.01;
				_mobiles[*it_v.first].vy += moyenne_y * 0.1;
			}
		}
    }

    for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first){ 
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first){
			gl4dpSetColor(_mobiles[0].color);
			gl4dpLine(_mobiles[*it_a.first].x, _mobiles[*it_a.first].y
			,_mobiles[*it_v.first].x, _mobiles[*it_v.first].y);
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
    _mobiles[i].vx = gl4dmSURand() * gl4dpGetWidth() / 3.0;
    _mobiles[i].vy = gl4dmSURand() * gl4dpGetHeight() / 3.0;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = 20;
    _mobiles[i].r_vis = 5;
    _mobiles[i].color = rand();
  }

    random_graph(g, 50, 2);
} 

void mobileMove(void) {
	static GLuint t0 = 0;
	GLuint t;
	GLfloat dt;
	int i,j;
	float rebondisement=0.3;
	float gravity_center=0.009;
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
				_mobiles[i].x -= (_mobiles[j].x - _mobiles[i].x) * rebondisement;
				_mobiles[i].y -= (_mobiles[j].y - _mobiles[i].y) * rebondisement;
				_mobiles[i].x -= (_mobiles[i].x - gl4dpGetWidth()) * gravity_center;
				_mobiles[i].y -= (_mobiles[i].y - gl4dpGetHeight()) * gravity_center;
				_mobiles[i].x += _mobiles[i].vx * dt;
				_mobiles[i].y += _mobiles[i].vy * dt;
	  		}
		}
	}
}

void mobileDraw(void) {
parcours(g);
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
