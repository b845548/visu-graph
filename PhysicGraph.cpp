#include "PhysicGraph.h"

PhysicGraph::PhysicGraph(void)
{
    nb_node=50; // nb node
    max_adjacent=3;// max adjacent
    rayon=25;// point phisque
    rayon_visual=5; // point visible
    speed=3.0; // rapidite de deplacement vx initial
    center=0.01; // facteur pour centrer un sommet autours les adjacents, moyenne == 0 on est au centre 
    progress=1.00;// progresse de 1 vers 0
    solution=0.045;// solution opmtimale
    gravity=0.000007; // gravite vers centre, pour eviter que le sommet se deplace vers le frontier
    rebound=0.3; // facteur de rebondisement
    distance_min=80.; //  moyenne de la distance des adjacents
    intersection=0.3; // facteur intersection
    density_limite=5; 
    screenWidth=512;    
    screenHeight=512; 
    grid_length=8;  
//    densityGrid= new std::vector<int>[grid_length*grid_length]; 
//    assert(densityGrid);
    gridWidth=(int)(grid_length*(screenWidth/(double)screenHeight));    
    gridHeight=(int)(grid_length*(screenHeight/(double)screenWidth));
    initializeRandomNode();
    initializeVertexProperty();
}
PhysicGraph::~PhysicGraph(void)
{  
      delete[] densityGrid;  
}  
  
bool PhysicGraph::isFinish(void) 
{
    return progress > solution;
}


Graph & PhysicGraph::data(void) 
{
    return g;
}


void PhysicGraph::initializeRandomNode(void) 
{
    // Ajout de nb_vertex sommets
    for(int i = 0; i < nb_node; ++i)
        add_vertex(g);
    
    // Générateur de nombres pseudo-aléatoires
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> dis(0,nb_node - 1);
    std::uniform_int_distribution<unsigned long> dis2(1,nb_node * max_adjacent / 100);
    
    // Ajout d'arêtes
    for (auto pair_it = vertices(g);pair_it.first != pair_it.second;++pair_it.first)
    {
        int nb_edge = dis2(gen);
        for(int i = 0; i < nb_edge; ++i) 
        {
            auto vertex1 = *pair_it.first;
            auto vertex2 = dis(gen);

            if(vertex2 != vertex1)
            {
                add_edge(vertex1, vertex2, g);
    		}
        }
    }
}

void PhysicGraph::initializeVertexProperty(void)
{
    for (auto it = vertices(g); it.first != it.second; ++it.first)
    {
        g[*it.first].x = gl4dmURand() * screenWidth;
        g[*it.first].y = gl4dmURand() * screenHeight;
        g[*it.first].vy = gl4dmSURand() * screenHeight / speed;
        g[*it.first].gravityDirectionX = 0;
        g[*it.first].vx = gl4dmSURand() * screenWidth / speed;
        g[*it.first].gravityDirectionY = -980;
        g[*it.first].rayon = rayon;
        g[*it.first].rayon_visual = rayon_visual;
        g[*it.first].color = rand();
    }
}


void PhysicGraph::printEdges(void)
{
    for(std::pair<edgeIt,edgeIt> ei = boost::edges(g); ei.first != ei.second; ++ei.first)
    {
       std::cout << source(*ei.first, g) << " -> " << target(*ei.first, g) << std::endl;
    }
}



void PhysicGraph::collisionMove(double dt)
{

	int i,j;
	for(i = 0; i < nb_node; i++)
    { 
		// collision dans le cadre
		if(g[i].x + g[i].rayon > screenWidth) 
			 	g[i].x = screenHeight-g[i].rayon;
		if(g[i].x - g[i].rayon < 0) 
			 	g[i].x = g[i].rayon;
		if(g[i].y + g[i].rayon > screenHeight) 
			 	g[i].y = screenHeight-g[i].rayon;
		if(g[i].y - g[i].rayon < 0) 
			 	g[i].y = g[i].rayon;
		if((g[i].x - g[i].rayon < 0 && g[i].vx < 0) || (g[i].x + g[i].rayon >= screenWidth && g[i].vx > 0))
			 	g[i].vx = -g[i].vx;
		if((g[i].y - g[i].rayon <= 0 && g[i].vy < 0) ||	(g[i].y + g[i].rayon >= screenHeight&& g[i].vy > 0 ))
				g[i].vy = -g[i].vy;
		// collision dans le cadre
		for(j = 0; j < nb_node; j++)
        { 
			if(i!=j&&pow(g[j].x-g[i].x,2)+pow(g[j].y-g[i].y,2)<=pow(g[j].rayon+g[i].rayon,2))
            {
				if(rand()%2==0)
                {// eloigner i
					g[i].x -= (g[j].x - g[i].x) * rebound*progress;
					g[i].y -= (g[j].y - g[i].y) * rebound*progress;
				}
                else
                {// eloigner j
					g[j].x += (g[j].x - g[i].x) * rebound*progress;
					g[j].y += (g[j].y - g[i].y) * rebound*progress;
				}
	  		}
		}
	}
}
void PhysicGraph::gravityMove(double dt)
{
	int i,j;
	for(i = 0; i < nb_node; i++)
    { 
		for(j = 0; j < nb_node; j++)
        { 
			if(i!=j)
            {
				g[i].x -= (g[i].x - screenWidth/2) * gravity*progress;
				g[i].y -= (g[i].y - screenHeight/2) * gravity*progress;
            }
		}
	}
}
    
void PhysicGraph::repulsionMove(double dt)
{
	int i,j;
	for(i = 0; i < nb_node; i++)
    { 
		for(j = 0; j < nb_node; j++)
        { 
			if(i!=j)
            {
                if(edge(i,j,g).second==false)
                {
    				g[i].x += ((g[i].x-g[j].x)>0?1:-1)*0.01*progress;
    				g[i].y += ((g[i].y-g[j].y)>0?1:-1)*0.01*progress;
    	  		}
            }
		}
	}
}
void PhysicGraph::attractionMove(double dt)
{
	float moyenne_x,moyenne_y,dist_x,dist_y;
	int nb_edge;
	
	for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first) 
    { 
  		moyenne_x=moyenne_y=0.0;
		nb_edge=0;
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first)
        {
			nb_edge++;
			dist_x=g[*it_a.first].x -g[*it_v.first].x;
			dist_y=g[*it_a.first].y -g[*it_v.first].y;
			moyenne_x+=dist_x;
			moyenne_y+=dist_y;
      	}

		if(nb_edge)
        {
			moyenne_x = moyenne_x / (float)nb_edge;	
			moyenne_y = moyenne_y / (float)nb_edge;
			if(moyenne_x>distance_min)
            {
				g[*it_v.first].x += moyenne_x * center*progress;
				g[*it_v.first].vx += moyenne_x * center*progress;
			}
			if(moyenne_y>distance_min)
            {
				g[*it_v.first].y += moyenne_y * center*progress;
				g[*it_v.first].vy += moyenne_y * center*progress;
			}
		}
    }
}

bool isIntersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    float denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
    float numerator1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
    float numerator2 = ((y1 - y3) * (x2 -  x1)) - ((x1 - x3) * (y2 - y1));

    // Detect coincident lines (has a problem, read below)
    if (denominator == 0) return numerator1 == 0 && numerator2 == 0;
    float r = numerator1 / denominator;
    float s = numerator2 / denominator;

    return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

void PhysicGraph::intersectionMove(double dt)
{
	int s1,s2,s3,s4;
	float dist_x,dist_y;
    int count=0,nb_eg=0;
    for(std::pair<edgeIt,edgeIt> ei2 = boost::edges(g); ei2.first != ei2.second; ++ei2.first) 
    {
	    nb_eg++;
	    for(std::pair<edgeIt,edgeIt> ei = boost::edges(g); ei.first != ei.second; ++ei.first) 
        {
		    s1=source(*ei.first, g);
		    s2=target(*ei.first, g);
		    s3=source(*ei2.first, g);
		    s4=target(*ei2.first, g);
		    if(s1!=s3&&s2!=s4)
    		    if(s1!=s2&&s3!=s4)
        		    if(s1!=s4&&s2!=s3)
	        		    if(isIntersection(g[s1].x,g[s1].y,g[s2].x,g[s2].y,g[s3].x,g[s3].y,g[s4].x,g[s4].y))
                        {
        				    gl4dpSetColor(RGB(255,0,0));
	        			    gl4dpLine(g[s1].x, g[s1].y,g[s2].x, g[s2].y);
				            gl4dpLine(g[s3].x, g[s3].y,g[s4].x, g[s4].y);
			                count++;
			                dist_x=g[s1].x -g[s2].x;
			                dist_y=g[s1].y -g[s2].y;
				            int rd=rand()%4;				
				            if(rd==0)
                            {
            				    g[s1].x-=dist_x*intersection*progress;
			            	    g[s1].y-=dist_y*intersection*progress;
			        	    }
                            else if(rd==1)
                            {
				                g[s2].x+=dist_x*intersection*progress;
				                g[s2].y+=dist_y*intersection*progress;				
				            }
                            else if(rd==2)
                            {
	    			            g[s3].x-=dist_x*intersection*progress;
	    			            g[s3].y-=dist_y*intersection*progress;				
				            }
                            else if(rd==3)
                            {
            				    g[s4].x+=dist_x*intersection*progress;
            				    g[s4].y+=dist_y*intersection*progress;				
        				    }
        			    }

        }
    }
    progress=count*6/(float)(nb_eg*nb_eg);
}
void PhysicGraph::drawNodes(void
)
{
    for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first)
    { 
		for(auto it_a = adjacent_vertices(*it_v.first, g);it_a.first !=it_a.second; ++it_a.first)
        {

			gl4dpSetColor(RGB(0,255,0));
				gl4dpLine(g[*it_v.first].x,g[*it_v.first].y,g[*it_a.first].x,g[*it_a.first].y);
	
			gl4dpSetColor(g[*it_a.first].color);
  		//	gl4dpFilledCircle(g[*it_a.first].x, g[*it_a.first].y, g[*it_a.first].rayon);  

		//	gl4dpSetColor(RGB(0,0,0));
  			gl4dpFilledCircle(g[*it_a.first].x, g[*it_a.first].y, g[*it_a.first].rayon_visual);  

    	}
    }
}
int PhysicGraph::catchNode(int mouse_x,int mouse_y)
{
    for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first)
    {
        mouse_y = screenHeight - mouse_y;
    	if(pow(mouse_x-g[*it_v.first].x,2)+pow(mouse_y-g[*it_v.first].y,2)<=pow(g[*it_v.first].rayon_visual,2))
        {
      		fprintf(stdin,"id %d selected\n",(int)*it_v.first);
            return *it_v.first;
        }
    }
	fprintf(stdin,"any vertex selected\n");
    return -1;
}


void PhysicGraph::densityMove(void)
{
    for(int i=0;i<gridWidth*gridHeight;++i)
    {
        if(densityGrid[i].size())
            densityGrid[i].clear();
    }

    for(auto it_v = vertices(g);it_v.first != it_v.second;++it_v.first)
    {

        int newPosX=(int)g[*it_v.first].x/(double)screenWidth*gridWidth;
        int newPosY=(int)g[*it_v.first].y/(double)screenHeight*gridHeight;
        densityGrid[newPosX+newPosY*gridWidth].push_back(*it_v.first);
    }


    int min,max;
    int minS,maxS;
    min=max=0;
    maxS=minS=densityGrid[0].size();
    for(int i=1;i<gridWidth*gridHeight;++i)
    {
        if(densityGrid[i].size() < minS)
        {    
            minS=densityGrid[i].size();
            min=i;
        }
        if(densityGrid[i].size() > maxS)
        {
            maxS=densityGrid[i].size();        
            max=i;
        }
    }
    
    std::vector<int> minVec;
    for(int i=0;i<gridWidth*gridHeight;++i)
    {
        if(densityGrid[i].size() == minS)
            minVec.push_back(i);
    }

    std::vector<int> maxVec = densityGrid[max];
    if(maxS-minS < density_limite)
        return;
//    std::cout<< minVec.size()<<" "<< maxVec.size() <<std::endl;
    int randMax=maxVec[rand()%maxVec.size()];
    int randMin=minVec[rand()%minVec.size()];
    g[randMax].x=(randMin%gridWidth)/(double)gridWidth*screenWidth;
    g[randMax].y=(randMin/gridWidth)/(double)gridHeight*screenHeight;


}







