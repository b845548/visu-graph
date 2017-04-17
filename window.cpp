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

static void idle(void) 
{
	static GLuint t0 = 0;
    GLuint t;
	GLfloat dt;
	t = SDL_GetTicks();
	dt = (t - t0) / 1000.0;
	t0 = t;
    if(gp.isFinish())
    {
        gp.centerMove(dt);
        gp.intersectionMove(dt);
    }
    gp.collisionMove(dt);
    gp.densityMove();
}
static void mouse(int button, int state, int x, int y) {

  if(button == GL4D_BUTTON_LEFT&&state) 
  {
        fprintf(stdin,"%d\n",gp.catchNode(x,y));
  }

}
static void draw(void) {
  gl4dpClearScreenWith (RGB(0, 0, 0));
  gp.drawNodes();
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
  atexit(quit);
  gl4duwMouseFunc(mouse);
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
