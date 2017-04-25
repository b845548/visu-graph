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
#include "PhysicGraph.h"

static PhysicGraph gp;
static int pauseMode = 0;
static int progressMode = 0;
static int gravityMode = 0;
static int intersectionMode = 0;
static int repulsionMode = 0;
static int attractionMode = 0;
static int collisionMode = 0;
static int densityMode = 0;

static void idle(void) 
{
	static GLuint t0 = 0;
    GLuint t;
	GLfloat dt;
	t = SDL_GetTicks();
	dt = (t - t0) / 1000.0;
	t0 = t;
    if(!pauseMode)
    {
        if(progressMode)
        {
            if(gp.isFinish())
            {
                gp.repulsionMove(dt);
                gp.intersectionMove(dt);
                gp.gravityMove(dt);
            }
                gp.attractionMove(dt);
            gp.collisionMove(dt);
            gp.densityMove();
        }
        else
        {
            if(gravityMode)
                gp.gravityMove(dt);
            if(repulsionMode)        
                gp.repulsionMove(dt);
            if(intersectionMode)        
                gp.intersectionMove(dt);
            if(attractionMode)
                gp.attractionMove(dt);
            if(collisionMode)        
                gp.collisionMove(dt);
            if(densityMode)
                gp.densityMove();
        

        }
    }
}
static void mouse(int button, int state, int x, int y) {

  if(button == GL4D_BUTTON_LEFT&&state) 
  {
        fprintf(stdin,"%d\n",gp.catchNode(x,y));
  }

}

static void printState(void){
    printf(" key 'q'  quit\n");
    printf(" key 's'  pause : %d\n",pauseMode);
    printf(" key 'p'  progressMode : %d\n",progressMode);
    printf(" key 'g'  gravityMode : %d\n",gravityMode);
    printf(" key 'i'  intersectionMode : %d\n",intersectionMode);
    printf(" key 'r'  repulsionMode : %d\n",repulsionMode);
    printf(" key 'a'  attractionMode : %d\n",attractionMode);
    printf(" key 'c'  collisionMode : %d\n",collisionMode);
    printf(" key 'd'  densityMode : %d\n",densityMode);
}
static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case 'q':
    exit(0);
    break;
  case 'p':
    progressMode = !progressMode;
    break;
  case 'g':
    gravityMode = !gravityMode;
    break;
  case 'i':
    intersectionMode = !intersectionMode;
    break;
  case 'r':
    repulsionMode = !repulsionMode;
    break;
  case 'a':
    attractionMode = !attractionMode;
    break;
  case 'c':
    collisionMode = !collisionMode;
    break;
  case 'd':
    densityMode = !densityMode;
  case 's':
    pauseMode = !pauseMode;
    break;
  default:
    break;
  }
}

static void draw(void) {
  gl4dpClearScreenWith (RGB(0, 0, 0));
  gp.drawNodes();
  system("clear");
  printState();
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
  if(!gl4duwCreateWindow(argc, argv, "Visualistation Graph - HBB", 10, 10, 512, 512, SDL_WINDOW_SHOWN))
    return 1;
  gl4dpInitScreen();
  atexit(quit);

  gl4duwKeyDownFunc(keydown);
  gl4duwMouseFunc(mouse);
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
