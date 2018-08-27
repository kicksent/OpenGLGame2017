/* Project submission Nick Trierweiler */

/*
Key Bindings:
T       Textures on/off
L       Lighting on/off
Y/y     Move light up/down y axis
</>     Rotate light around scene clock/counterclock

U/u     ambience
I/i     diffuse
O/o     specular
P/p     emission

C       Faceculling
M       viewing modes(orthogonal/perspective)
F       toggle first person mode(can be used in orthogonal and perspective modes)
+/-     change view in/out in perspective mode
x       toggle axis viewing
w,a,s,d moving in first person mode
arrow keys for rotation around scene when not in first person

Magic:  1. Y/y to move light up/down, and < > keys to move light around the scene.
        2. Play with T/L, to see the change!
        3. Press M and then - to zoom in on the textures in the scene!
        4. Then press F and walk around with w,a,s,d to get a good look.
*/

/*
Cleanup inprovements for the future:
	change args in functions from (1,1,1,1,1) to arrays for pos, scale, rotations, etc.)
	
*/
 //SDL LIBS
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "CSCIx229.h" //See file
double t=.1;

/* Global Vars */
int face=0;       //  Face Culling
int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=6.0;   //  Size of world

/* Tree recursion parameters */
int max_levels=5;//  Depth of recursive trees
int depth_max=3;  //  Depth of recursive trees old!

/* Lighting  */
int local=0;
int light=1;      //  Enable light
int distance=10;  //  Set light distance
int ambient=15;    //  Set intensity 30
int diffuse=20;   //  Set intensity 100
int specular=85;  //  Set intensity
int emission=10;  //  Set intensity
int shininess=0;  //  Shininess (power of two)
float shiny=1;    //  Set Shininess
int zh=90;        //  Light azimuth
int ylight=1;     //  Light y-height

float X= 0;       // Light X position spotlight
float Y= 1;       // Light Y position
float Z= 5;       // Light Z position

float sco=20;     //  Spot cutoff angle
float Exp=1;      //  Spot exponent
//int th=0,ph=0;  //  View angles
int Th=0,Ph=30;   //  Light angles

/* Texture vars */
int texturemode=1;//  Toggle texture modes
int num=1;        //  Number of quads/cylinder auds slices(will need this for flashlight to look good later)
unsigned int treetex[2];
;
unsigned int 
texname[12];      //Texture name array
int blend=0;      //  Toggle blending

/* first person */

int fpview=1;     //  First person
int fpangle=0;    //  Angle for fp perspective
double Ex=0;      //  Eye coordinates first person
double Ey=1;
double Ez=5;
double Cx=0;      //  Camera coordinates first person
//double Cy=0;    
double Cz=10;
/* Map */
int map = 0;
float rgbcolor[3];

//skybox
int box=1;
unsigned int woods[3];



//collision vars
/* note: count number of hitboxes carefully or 
else a hitbox will be created at (0,0,0) */

int numObjects=3;  //number of objects in scene with hitboxes
int collision=0;   //collision return var(primitive, change to [0,0,0,0] for multiple case match
float playerbox[4];//player hitbox
float skyHbox[4]; //skybox hitbox
float objbox[5][6]; //house hitbox
#define xpos 0   //for readability of hitbox arrays
#define zpos 1
#define wbox  2 //width of hitbox
#define hbox  3 //height of hitbox
#define objCenterx 4
#define objCenterz 5



/* Pressing 'n' shows the map of the layout in real time and disallows movement */
void showMap(){
	if(map){
		map=0;
	}
	else{
		map=1;
	}
}
/*
 * Update the player position as we move around the eye position.
 * We only care about the x-z plane.
 * [x, z, width, height]
 *
 
 */
void updatePlayerPos(){ //playerhitbox position
	playerbox[xpos] = Ex-.1; 
	playerbox[zpos] = Ez-.1;
	playerbox[wbox] = .2;
	playerbox[hbox] = .2;
	
}
/* Created a square hitbox around object 
 note: from top left since x-z starts from top left!
 ex:    
		z-
		|
		|
x- -----|----- x+
        |
		|
		z+

*/
void setHitbox(double x, double z, double boxwidth, double boxheight, int number){
	objbox[number][objCenterx] = x; //centers, for menu teleport
	objbox[number][objCenterx] = z;
	x +=(-.5*boxwidth);
	z +=(-.5*boxheight);
	objbox[number][xpos] =x; 
	objbox[number][zpos] =z;
	objbox[number][wbox] =boxwidth;
	objbox[number][hbox] =boxheight;
}

/* AABB collision set for skybox */
void skyHitbox(double x, double z, double boxwidth, double boxheight){
	x +=(-.5*boxwidth);
	z +=(-.5*boxheight);
	skyHbox[xpos] =x; 
	skyHbox[zpos] =z;	 
	skyHbox[wbox] =boxwidth;   
	skyHbox[hbox] =boxheight;
}

/*
 *	The goal here is to check for AABB collisions since
 *	we are only working in the x-z plane.
 *  This was a journey of self discovery and stubbornness as I tried to make this work my own way.
 *  In the end, I followed the footsteps of my fellow code bretheren since this isn't exactly graphics
 *  rendering tasks. 
 *	Credit https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
 */
 int checkCollision(){
	int col = 0;
	int i = 0;
	
	for(i=0;i<numObjects;i++){
		if( playerbox[xpos] < objbox[i][xpos] + objbox[i][wbox] &&
			playerbox[xpos] + playerbox[wbox] > objbox[i][xpos] &&
			playerbox[zpos] < objbox[i][zpos] + objbox[i][hbox] &&
			playerbox[hbox] + playerbox[zpos] > objbox[i][zpos])
			col+=1;
	}
	for(i=0;i<numObjects;i++){
		if( playerbox[xpos] < skyHbox[xpos] + skyHbox[wbox] &&
			playerbox[xpos] + playerbox[wbox] > skyHbox[xpos] &&
			playerbox[zpos] < skyHbox[zpos] + skyHbox[hbox] &&
			playerbox[hbox] + playerbox[zpos] > skyHbox[zpos])
			col+=0;
		else
		{
			col=1;
		}
	}
	
	return col;
}

/*
 * Draw woods skybox
 */
static void sky(double D)
{
   skyHitbox(0,0,50,50);
   
   double xsign, zsign;
   if(Ex>0)
	   xsign=1;
   else
	   xsign=-1;
   if(Ez>0)
	   zsign=1;
   else
	   zsign=-1;
   double Vx = D+(xsign*.25*Ex);
   double Vz = D+(zsign*.25*Ez);
   /* translations help us here
      artificially makes the scene bigger than it is! :D without making it look
      unreal, the objects in the scene help so make this work too
	  This ended up actually working in my favor as well, as the translations help to accomodate for some skybox stretching making is hardly noticeable at higher dim values
	  */
   
   float i,j;
   float num = 100;
   float mul = 1/num;
   glPushMatrix();
   glTranslated(0,0,.25*Ez);
   glTranslated(.25*Ex,0,0);
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   

   //  Sides
   glBindTexture(GL_TEXTURE_2D,woods[0]);
   glBegin(GL_QUADS);
   glNormal3f(0,0,0); //left
   glTexCoord2f(0.00,0); glVertex3f(-Vx,-D,-Vz);
   glTexCoord2f(0.25,0); glVertex3f(+Vx,-D,-Vz);
   glTexCoord2f(0.25,1); glVertex3f(+Vx,+D,-Vz);
   glTexCoord2f(0.00,1); glVertex3f(-Vx,+D,-Vz);
   glNormal3f(0,0,0); //front
   glTexCoord2f(0.25,0); glVertex3f(+Vx,-D,-Vz);
   glTexCoord2f(0.50,0); glVertex3f(+Vx,-D,+Vz);
   glTexCoord2f(0.50,1); glVertex3f(+Vx,+D,+Vz);
   glTexCoord2f(0.25,1); glVertex3f(+Vx,+D,-Vz);
   glNormal3f(0,0,0); //right
   glTexCoord2f(0.50,0); glVertex3f(+Vx,-D,+Vz);
   glTexCoord2f(0.75,0); glVertex3f(-Vx,-D,+Vz);
   glTexCoord2f(0.75,1); glVertex3f(-Vx,+D,+Vz);
   glTexCoord2f(0.50,1); glVertex3f(+Vx,+D,+Vz);
   glNormal3f(0,0,0); //back
   glTexCoord2f(0.75,0); glVertex3f(-Vx,-D,+Vz);
   glTexCoord2f(1.00,0); glVertex3f(-Vx,-D,-Vz);
   glTexCoord2f(1.00,1); glVertex3f(-Vx,+D,-Vz);
   glTexCoord2f(0.75,1); glVertex3f(-Vx,+D,+Vz);
   glEnd();
   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,woods[1]);
   glBegin(GL_QUADS);
   glNormal3f(0,-1,0);
   glTexCoord2f(0.0,0); glVertex3f(+Vx,2*D,-Vz);
   glTexCoord2f(1,0); glVertex3f(+Vx,2*D,+Vz);
   glTexCoord2f(1,1); glVertex3f(-Vx,2*D,+Vz);
   glTexCoord2f(0.0,1); glVertex3f(-Vx,2*D,-Vz);
   glEnd();
   
   glBindTexture(GL_TEXTURE_2D,woods[2]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glBegin(GL_QUADS);
   glNormal3f(0,1,0);
   for (i=0;i<num;i++)
      for (j=0;j<num;j++){
		 glTexCoord2d(D*mul*(i+0),D*mul*(j+0)); glVertex3d(2*D*mul*(j+0)-D, 0, 2*D*mul*(i+0)-D);
		 glTexCoord2d(D*mul*(i+0),D*mul*(j+1)); glVertex3d(2*D*mul*(j+0)-D, 0, 2*D*mul*(i+1)-D);
		 glTexCoord2d(D*mul*(i+1),D*mul*(j+1)); glVertex3d(2*D*mul*(j+1)-D, 0, 2*D*mul*(i+1)-D);
		 glTexCoord2d(D*mul*(i+1),D*mul*(j+0)); glVertex3d(2*D*mul*(j+1)-D, 0, 2*D*mul*(i+0)-D);
		}
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*  Taken from ex13.c
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}
/*  Taken from ex13.c
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
void lightball(double x,double y,double z, double rad, float r,float g,float b)
{
   //Set ball increment
   int inc =10; 
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(rad,rad,rad);
   //  White ball
   glColor3f(r,g,b);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

   
/*
 *  Draw cylinder at pt
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 *     FIXED: scaling 1,1,1 now creates cylinder of radius 1 and height 1
 */
static void cylinder(double x, double y, double z, 
   double dx, double dy, double dz, double r, double g, double b, 
   double xplane, double yplane, double zplane, double th, unsigned int texture1, unsigned int texture2)
{
   
   /*Set up material color*/
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

   double d = 10;
   double rad = .5;
   float angle = 0;
   float texinc = 360/d;

   //Save Transformation
   glPushMatrix();
   //Transformations
   glTranslated(x,y,z);
   glRotated(th, xplane, yplane, zplane);
   glScaled(dx,dy,dz);

   glColor3f(r,g,b);

   /* Set up textures */
   if(texturemode){
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture2); //top/bottom texture
   }
   else{
      glDisable(GL_TEXTURE_2D);
   }

   //Build cylinder
   glBegin(GL_TRIANGLES);
   
   //glVertex3d(Sin(0),1,cos(0));
   for (angle=0;angle<360;angle+=d){
      //bottom
      glNormal3f(0,-1,0);
      glTexCoord2f(.5, .5); glVertex3d(0,-.5*dy,0); //0,0 is the middle of the circle in x-z plane 
      glTexCoord2f(rad*Sin(angle+d)+.5, rad*Cos(angle+d)+.5); glVertex3d(rad*Sin(angle+d), -.5*dy, rad*Cos(angle+d));
      glTexCoord2f(rad*Sin(angle)+.5, rad*Cos(angle)+.5); glVertex3d(rad*Sin(angle),   -.5*dy, rad*Cos(angle));
      //top
      glNormal3f(0,1,0);
      glTexCoord2f(.5, .5); glVertex3d(0,.5*dy,0);
      glTexCoord2f(rad*Sin(angle)+.5, rad*Cos(angle)+.5);     glVertex3d(rad*Sin(angle),   .5*dy, rad*Cos(angle));
      glTexCoord2f(rad*Sin(angle+d)+.5, rad*Cos(angle+d)+.5); glVertex3d(rad*Sin(angle+d), .5*dy, rad*Cos(angle+d));
   }
   glEnd();

   //switch to side texture
   glBindTexture(GL_TEXTURE_2D, texture1);

   glBegin(GL_QUADS);
   for(angle=0;angle<360;angle+=d){
      //printf("increment: %f\n", angle/d/texinc);
      glNormal3f(rad*Sin(angle), 0, rad*Cos(angle));
      glTexCoord2f(angle/d/texinc, 0); glVertex3d(rad*Sin(angle), -.5*dy,rad*Cos(angle));//LL

      glNormal3f(rad*Sin(angle+d), 0, rad*Cos(angle+d));
      glTexCoord2f((angle+d)/d/texinc, 0); glVertex3d(rad*Sin(angle+d), -.5*dy,rad*Cos(angle+d));//LR

      glNormal3f(rad*Sin(angle+d), 0, rad*Cos(angle+d));
      glTexCoord2f((angle+d)/d/texinc ,1); glVertex3d(rad*Sin(angle+d), .5*dy,rad*Cos(angle+d));//UR

      glNormal3f(rad*Sin(angle), 0, rad*Cos(angle));
      glTexCoord2f(angle/d/texinc ,1); glVertex3d(rad*Sin(angle), .5*dy,rad*Cos(angle));//UL
   }
   glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
/*
 *  Draw a house
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void house(double x,double y,double z,double dx,double dy,double dz,double th, double color, int housenum)
{
   
	float i,j; //for drawing loops
	float num = 40; //number of iterations
	float mul = 1/num;
   
   //setHitboxes
   setHitbox(x,z,2*dx,2*dz, housenum);
   
   float house[3] = {fmod(.15*color,1),fmod(.5*color,1),fmod(.1*color,1)};
   float roof[3] = {.5,.35,.05};

   /*Set up material color*/
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   
   //Save Transformation
   glPushMatrix();
   //Transformations - remember y is up! x-z plane for horiz. 
   glTranslated(x,y,z);
   glRotated(th, 0, 1, 0);
   glScaled(dx,dy,dz);

   /* Texture */
   if(texturemode)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texname[1]);
   }
   else{
      glDisable(GL_TEXTURE_2D);
   }

   //Enable Offset
   /* note: offset must be set before glBegin() calls */
   glEnable(GL_POLYGON_OFFSET_FILL);

   /******************** OFFSET 2 *******************/
   glPolygonOffset(2,2);
   /*  House Sides */
   glBegin(GL_QUADS);
   /*  Front wall x,y */
   glNormal3f(0,0,1);
   glColor3f(house[0],house[1],house[2]);
   
   for (i=0;i<num;i++)
      for (j=0;j<num;j++){
		 glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1, 1);
         glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1, 1);
         glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1, 1);
         glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1, 1);
		}
   
   /*  Back wall x,y */
   glNormal3f(0,0,-1);
   for (i=0;i<num;i++)
      for (j=0;j<num;j++){
		 glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1, -1);
		 glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1, -1);
         glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1, -1);
         glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1, -1);
         
		}
   
   /*  Right wall y,z */
   glNormal3f(1,0,0);
    for (i=0;i<num;i++)
      for (j=0;j<num;j++){
		 glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(1, 2*mul*(j+0)-1,2*mul*(i+1)-1);
		 glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(1, 2*mul*(j+0)-1, 2*mul*(i+0)-1);
		 glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(1, 2*mul*(j+1)-1, 2*mul*(i+0)-1);
         glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(1, 2*mul*(j+1)-1, 2*mul*(i+1)-1);
		}
   
   /*  Left wall y,z */
   glNormal3f(-1,0,0);
   for (i=0;i<num;i++)
      for (j=0;j<num;j++){
		 glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(-1, 2*mul*(j+0)-1, 2*mul*(i+0)-1);
		 glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(-1, 2*mul*(j+0)-1,2*mul*(i+1)-1);
		 glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(-1, 2*mul*(j+1)-1, 2*mul*(i+1)-1);
		 glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(-1, 2*mul*(j+1)-1, 2*mul*(i+0)-1);
		}
   
   /*  Floor  */
   glNormal3f(0,-1,0);
   
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   glEnd();

   //fill holes below roof
   glBegin(GL_TRIANGLES);
   glColor3f(house[0],house[1],house[2]);
   //right
   glNormal3f(1,0,0);
   glTexCoord2f(1,1); glVertex3f(1,1,-1);
   glTexCoord2f(.5,.5); glVertex3f(1,1.5,0);
   glTexCoord2f(.5,1); glVertex3f(1,1,0);
   
   glTexCoord2f(.5,1); glVertex3f(1,1,0);
   glTexCoord2f(.5,.5); glVertex3f(1,1.5,0);
   glTexCoord2f(0,1); glVertex3f(1,1,1);
   
  
   //left
   glNormal3f(-1,0,0);
   glTexCoord2f(1,1); glVertex3f(-1,1,1);
   glTexCoord2f(.5,.5); glVertex3f(-1,1.5,0);
   glTexCoord2f(.5,1); glVertex3f(-1,1,-1);
   
   glTexCoord2f(.5,1); glVertex3f(-1,1,1);
   glTexCoord2f(.5,.5); glVertex3f(-1,1.5,0);
   glTexCoord2f(0,1); glVertex3f(-1,1,-1);
   glEnd();
   
   /******************** OFFSET 1 Render *******************/
   glPolygonOffset(1,1);
   /* Door */
   if(texturemode){
      glBindTexture(GL_TEXTURE_2D, texname[5]);
      glColor3f(.4,.35,.1);
   }
   else{
      glColor3f(.3,.23,.1);
   }
   glBegin(GL_QUADS);
   glNormal3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(-.6,-1, 1);
   glTexCoord2f(1,0); glVertex3f(-.3,-1, 1);
   glTexCoord2f(1,1); glVertex3f(-.3,-.2, 1);
   glTexCoord2f(0,1); glVertex3f(-.6,-.2, 1);
   glEnd();
   /* Window Frame */
   if(texturemode)
   {
      glBindTexture(GL_TEXTURE_2D, texname[4]); //frame texture4
   }
   glBegin(GL_QUADS);
   //front
   glNormal3f(0,0,1);
   glColor3f(1,1,1);
   glTexCoord2f(1,0); glVertex3f(.7,-.7, 1);
   glTexCoord2f(1,1); glVertex3f(.7,0, 1);
   glTexCoord2f(0,1); glVertex3f(0,0, 1);
   glTexCoord2f(0,0); glVertex3f(0,-.7, 1);
   //back
   glNormal3f(0,0,-1);
   glTexCoord2f(1,0); glVertex3f(-.6,-.7, -1);
   glTexCoord2f(1,1); glVertex3f(-.6,0, -1);
   glTexCoord2f(0,1); glVertex3f(.1,0, -1);   
   glTexCoord2f(0,0); glVertex3f(.1,-.7, -1);
   glEnd();

   /*Roof*/
   if(texturemode)
   {
      glBindTexture(GL_TEXTURE_2D, texname[3]); //roof texture
   }

   glPushMatrix();
   glTranslated(0,1,0);
   // Translated to make normal calc easier
   glBegin(GL_QUADS);
   //Roof half front
   glNormal3f(0,1,.25);
   glColor3f(roof[0], roof[1], roof[2]);
   glTexCoord2f(0,0); glVertex3f(-1,+0,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+0,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+0.5,0);
   glTexCoord2f(0,1); glVertex3f(-1,+0.5,0);
   //Roof half back
   glNormal3f(0,1,-.25);
   glTexCoord2f(0,1); glVertex3f(-1,+0.5,0);
   glTexCoord2f(1,1); glVertex3f(+1,+0.5,0);
   glTexCoord2f(1,0); glVertex3f(+1,+0,-1);
   glTexCoord2f(0,0); glVertex3f(-1,+0,-1);
   glEnd();
   glPopMatrix();
   
 
   // Disable Offset! - objects can go here if they are the front layer
   glDisable(GL_POLYGON_OFFSET_FILL);

   /* Windows */
   if(!texturemode){
      //front
      glBegin(GL_QUADS);
      glNormal3f(0,0,1);
      glColor3f(0,0,1);
      //xs xy points UL, sq = length of sides
      float xs = .4;
      float ys = -.4;
      float sq = .2;
      glVertex3f(xs+sq,ys-sq, 1);
      glVertex3f(xs+sq,ys, 1);
      glVertex3f(xs,ys, 1);
      glVertex3f(xs,ys-sq, 1);
      xs-=.3;
      glVertex3f(xs+sq,ys-sq, 1);
      glVertex3f(xs+sq,ys, 1);
      glVertex3f(xs,ys, 1);
      glVertex3f(xs,ys-sq, 1);
      ys +=.3;
      glVertex3f(xs+sq,ys-sq, 1);
      glVertex3f(xs+sq,ys, 1);
      glVertex3f(xs,ys, 1);
      glVertex3f(xs,ys-sq, 1);
      xs +=.3;
      glVertex3f(xs+sq,ys-sq, 1);
      glVertex3f(xs+sq,ys, 1);
      glVertex3f(xs,ys, 1);
      glVertex3f(xs,ys-sq, 1);
      glEnd();
      glDisable(GL_BLEND);
      //Back
      glBegin(GL_QUADS);
      glNormal3f(0,0,-1);
      //xs xy points UL, sq = length of sides
      xs = -.2;
      ys = -.4;
      sq = .2;
      glVertex3f(xs,ys-sq, -1);
      glVertex3f(xs,ys, -1);
      glVertex3f(xs+sq,ys, -1);
      glVertex3f(xs+sq,ys-sq, -1);
      xs-=.3;
      glVertex3f(xs,ys-sq, -1);
      glVertex3f(xs,ys, -1);
      glVertex3f(xs+sq,ys, -1);
      glVertex3f(xs+sq,ys-sq, -1);
      ys +=.3;
      glVertex3f(xs,ys-sq, -1);
      glVertex3f(xs,ys, -1);
      glVertex3f(xs+sq,ys, -1);
      glVertex3f(xs+sq,ys-sq, -1);
      xs +=.3;
      glVertex3f(xs,ys-sq, -1);
      glVertex3f(xs,ys, -1);
      glVertex3f(xs+sq,ys, -1);
      glVertex3f(xs+sq,ys-sq, -1);
      glEnd();
   }
   //Chimney
   if(texturemode)
      cylinder(.6,1.5,0,.25,1,.25,1,1,1,0,0,0,0,texname[6], texname[7]);
   else
      cylinder(.6,1.5,0,.25,1,.25,.8,.23,0,0,0,0,0,texname[6], texname[7]);

   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
   
}


/*
 *  Draw leaf at pt
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 *  Texture mapping uses repeating texture because this is a depricated circle filled by openGL
 */
void leaf(double x, double y, double z, 
   double dx, double dy, double dz, double r, double g, double b, 
   double xplane, double yplane, double zplane, double th, unsigned int texture1, unsigned int texture2)
{
   // I need 2 sided lighting for this these drawings so I am going to disable the shader
   //glUseProgram(0);
   glDisable(GL_CULL_FACE);
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
   
   //Save Transformation
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(55,0,1,0);
   glRotated(th, xplane, yplane, zplane);
   glScaled(dx,dy,dz);

   glColor3f(r,g,b);

   // leaf is flat so two sided lighting
   // I tried to make the leaf have depth but that was a huge pain and wasted about 5 hours of work
   // because of the way I drew the leaf, I leanred a lot though!
   
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   
   /* Set up textures */
   if(texturemode){
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture2); //top/bottom texture
   }
   else{
      glDisable(GL_TEXTURE_2D);
   }
   double thick = .01; //thickness of leaf
   double d = 10;
   double rad = .4;
   float angle = 0;
   
   //draw leaf
   glBegin(GL_TRIANGLES);
   for (angle=90;angle<270;angle+=d){
	  //draw half circle but the middle is the tip of the leaf + rounding
      glNormal3f(0,0,1);
      glTexCoord2f(.5,1.5); glVertex3d(0,1,thick); //0,0 is the middle of the circle in x-z plane 
      glTexCoord2f(rad*Sin(angle+d)+.5, rad*Cos(angle+d)+.5); glVertex3d(rad*Sin(angle+d), rad*Cos(angle+d), thick);
      glTexCoord2f(rad*Sin(angle)+.5, rad*Cos(angle)+.5); glVertex3d(rad*Sin(angle),   rad*Cos(angle), thick);
   }
    glEnd();
   cylinder(0,-.75,0,.1,1,5*thick,r,.5,b,0,0,0,0, texture1, texture2);

   glPopMatrix();
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
   glDisable(GL_TEXTURE_2D);
   if(face)
      glEnable(GL_CULL_FACE);
   
}
/* 
 * Draws tree() using calls to cylinder
 *    at (x,y,z)
 *    dimentions (dx,dy,dz)
 *    of color(r,g,b)
 *    rotation (th)
 *    branch (angle)
 */
void tree2(double length, int level, int n, int angle, int type,double r,double g, double b){
   //glTranslated(x,y,z);
   //glRotated(0,0,1,0);
   glScaled(.8,.6,.8);
   if(level<max_levels){   //is the end of the recursion
      n = 3;
	  if(level == (max_levels-1)){
	   n=4;
	   angle = 75;
	  }
      int x=0;
      for(x=0; x<n; x++){   
         glPushMatrix();
         glRotated(((360 / n)*x), 0, 1, 0); //along y axis
         glRotated(angle, 0, 0, 1); //along z axis
         if(level == (max_levels-1)){
            g=.8;
			glScaled(2,.5,1);
			leaf(0,length,0,2*length,length,2*length,r,g,b,0,0,0,0, treetex[0+type], texname[8]);
         } 
         else
            cylinder(0,length,0,.3*length,length,.3*length,r,g,b,0,0,0,0, treetex[0+type], texname[9]);

         glTranslated(0,length*2,0); //length*2 = fractal tree, length = somewhat realistic tree
         

         tree2(length, level+1, n, angle, type, r, g, b);  //keep length the same
         glPopMatrix();
		 
      }

   }
}
void tree(double length, int angle, int type, double x, double z){
	glPushMatrix();
	double r=.8;
    double g=.5;
    double b=.5;
	glScaled(.2,.4,.2);
	if (type == 1)
	     r=.5;
	//glScaled(.8,.6,.8);
	glTranslated(x,0,z);
	cylinder(0,.5,0,.35*length,length*.9,.35*length,r,g,b,0,0,0,0, treetex[0+type], texname[9]);
	glTranslated(0,2,0);
	tree2(length, 1, 3, angle, type, r, g, b);
	glPopMatrix();
}

/* My attempt at a forest: I had to work pretty hard on this to make the trees not have a pattern which was discernable while also not hitting my houses, I wanted to make it a challenge to I spent way too much time on this and didn't search for much help on this idea */

void drawForest(){
   int i=0;
   int m;
   int t;
   for(i=7; i<28; i+=7){
	   m=i%19;
	   t=i%2;
	   glPushMatrix();
	   glTranslated(i,0,i);
	   glRotated(-15,0,1,0); 
	   tree(2, 35, t, -1.7+m,2+m);
	   glPopMatrix();
	   glPushMatrix();

	   glTranslated(0,0,i);
	   glRotated(-15,0,1,0); 
	   tree(2, 35, t, -1.7+m,2-m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(-i,0,i);
	   glRotated(15,0,1,0); 
	   tree(2, 35, t, 1+m, -5+m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(0,0,-i);
	   glRotated(15,0,1,0); 
	   tree(2, 35, t, 1-m, -5+m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(i,0,-i);
	   glRotated(30,0,1,0); 
	   tree(2, 35, t, -4+m,-7.5+m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(i,0,0);
	   glRotated(30,0,1,0); 
	   tree(2, 35, t, -4-m,-7.5-m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(-i,0,-i);
	   glRotated(35,0,1,0); 
	   tree(2, 35, t, -7+m,.5-m);
	   glPopMatrix();
	   
	   glPushMatrix();
	   glTranslated(-i,0,0);
	   glRotated(35,0,1,0); 
	   tree(2, 35, t, -7-m,.5+m);
	   glPopMatrix();
   }
   
   
}


/*
 *  OpenGL (SDL) calls this routine to display the scene
 */
void display()
{
   t+=.1;
   Th = fmod(90*t, 360);
   glShadeModel(GL_SMOOTH);//Enabled by default?
   const double len=4;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   //  enable face-culling
   if(face == 1)
      glEnable(GL_CULL_FACE);
   else
      glDisable(GL_CULL_FACE);
   //  Undo previous transformations
     glMatrixMode(GL_MODELVIEW);
   if(fpview) {
	   if(map == 0){
       Cx = +1.5*dim*Sin(fpangle);
       Cz = -1.5*dim*Cos(fpangle);
       gluLookAt(Ex,Ey,Ez,Cx+Ex,-.1+Ey,Cz+Ez, 0,1,0); //y is UP -.1+Ey allows ability to see floor in orthogonal first person
       glWindowPos2i(5,60);
       Print("First Person: %s Viewing angle: %d collision: %d",fpview?"On":"Off", fpangle, collision);
	   }
	   else //map is 0, because map is on, dont use Ex, Ez because it tilts the map... 
	   {
		   gluLookAt(0,45,0,0,0,0,0,0,-1);
	   }
   }
   else{
      //  Perspective - set eye position
      if (mode)
      {
         double Ex = -2*dim*Sin(th)*Cos(ph);
         double Ey = +2*dim        *Sin(ph);
         double Ez = +2*dim*Cos(th)*Cos(ph);
         gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      }
      //  Orthogonal - set world orientation
      else
      {
         glRotatef(ph,1,0,0);
         glRotatef(th,0,1,0);
      }
   }
   glShadeModel(GL_SMOOTH);//Enabled by default?

   /* Lighting for scene light0 */
   if(light)
   {
      float Emission[] = {0.01*emission, 0.01*emission, 0.01*emission, 1.0}; 
	  float Ambient[] = {0.01*ambient, 0.01*ambient, 0.01*ambient, 1.0};
      float Diffuse[] = {0.01*diffuse, 0.01*diffuse, 0.01*diffuse, 1.0};
      float Specular[] = {0.01*specular, 0.01*specular, 0.01*specular, 1.0};
      //Set light Position
      float Position[] = {Ex,ylight,Ez,1}; //pos of light
      float Direction[] = {.1*Cx,0,.1*Cz,0}; //dir of spotlight
      //float Position[] = {Ex,-.5+Ey+ylight,Ez, 1.0};
	  //float Position[] = {distance * Cos(zh), ylight, distance * Sin(zh), 1.0}; 

	  float Shinyness[] = {0};
      //Light represented as ball from ex13
      glColor3f(1,1,1);
	  
      lightball(Position[0], Position[1], Position[2], 0.1, 1,1,1);
      //Normalize Normal Vectors?
      glEnable(GL_NORMALIZE);
      //Enable Lighting
      glEnable(GL_LIGHTING);
      //Location of viewer for specular calc
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
      //glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //Enable light0
      glEnable(GL_LIGHT0);
      //Set Ambient diffuse and specular components and pos of light0
      glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);
	  // Set materials
	  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
	  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
      //  Set spotlight parameters
      glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,Direction);
      glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,sco);
      glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,Exp);
      //  Set attenuation
      glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION ,1);
      glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION   ,0);
      glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0);

   }
   else
      glDisable(GL_LIGHTING);
  
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

   /***  ~~~~~~~~~~~~~~~~~~~~~~~~~~Draw the scene~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ***/
   
   
   //  Draw houses (translate(,,), scale(,,), rotate angle, color offset, housenum)
   house( 0, 1,   0, 1, 1, 1, 0, 1, 0); // middle
   house(-20, 1, 15, 1, 1, 1, 0, 3, 1); //left
   house(15, 1, -10, 1, 1, 1, 270, 7, 2); //right
   //Draw Trees
   drawForest();
   
   //  Draw axes && disable lighting/texture/shader for axes
   if(box)
	   sky(4.5*dim);
   
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_LIGHTING);
   
   if(map){
	    lightball(Ex,.1,Ez,.15,1,0,0);
		glWindowPos2i(5,60);
		Print("Player=red");
    }
   
   
   
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
 
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s FaceCull=%s Light: %s Texture: %s",th,ph,dim,fov,mode?"Perpective":"Orthogonal", 
      face?"On":"Off", light?"On":"Off", texturemode?"On":"Off");
   glWindowPos2i(5,30);
   Print("Ambience: %d Diffuse: %d Specular: %d Emission: %d Shininess: %d Local:%s", ambient, diffuse, specular, emission, shininess, local?"On":"Off");
 
   //  SDL Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   SDL_GL_SwapBuffers();
}


/*
 *  SDL calls this routine when a key is pressed
 */
int key()
{
   Uint8 *keys = SDL_GetKeyState(NULL);
   
   //  Right arrow key - increase angle by 5 degrees
   if (keys[SDLK_RIGHT])
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (keys[SDLK_LEFT])
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (keys[SDLK_UP])
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (keys[SDLK_DOWN])
      ph -= 5;
   //  PageUp key - increase dim
   else if (keys[SDLK_PAGEUP])
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (keys[SDLK_PAGEDOWN] && dim>1)
      dim -= 0.1;
   //  Exit on ESC
   else if (keys[SDLK_ESCAPE])
      return 0;
   //  Reset view angle
   else if (keys[SDLK_0])
      th = ph = 0;
   //  Toggle axes
   else if (keys[SDLK_x])
      axes = 1-axes;
   //  Switch display mode
   else if (keys[SDLK_m])
      mode = 1-mode;
   //  Change field of view angle
   else if (keys[SDLK_KP_MINUS] || keys[SDLK_MINUS])
      fov--;
   else if (keys[SDLK_KP_PLUS] || keys[SDLK_EQUALS])
      fov++;
   //turn on face culling
   else if (keys[SDLK_c])
      face = (face+1)%2;
   //toggle first person view
   else if (keys[SDLK_f])
      fpview= (fpview+1)%2;
   // Move light
   else if(keys[SDLK_COMMA])
      zh+=4;
   else if(keys[SDLK_PERIOD])
      zh-=4;
   //Spotlight CutOff
   else if(keys[SDLK_p] && keys[SDLK_LSHIFT]){
	  if(sco<90)
		 sco+=5;
   }
   else if(keys[SDLK_p]){
	  if(sco>0)
	      sco-=5;  
   }
   //toggle light0
   else if (keys[SDLK_l])
      light=(light+1)%2;
   //change light position up/down
   else if ((keys[SDLK_y] && keys[SDLK_LSHIFT]) || (keys[SDLK_y] && keys[SDLK_RSHIFT]))
      ylight+=1;
   else if (keys[SDLK_y])
      ylight-=1;
   else if (keys[SDLK_RIGHTBRACKET])
	   distance +=1;
   else if (keys[SDLK_LEFTBRACKET])
	   distance -=1;
   else if (keys[SDLK_b])
      blend=(blend+1)%2;
   else if (keys[SDLK_t])
      texturemode = (texturemode+1)%2;
   else if (keys[SDLK_n])
	  showMap();
   else if ((keys[SDLK_u] && keys[SDLK_LSHIFT]) || (keys[SDLK_u] && keys[SDLK_RSHIFT]))
      ambient +=5;
   else if (keys[SDLK_u])
      ambient -=5;
   else if ((keys[SDLK_i] && keys[SDLK_LSHIFT]) || (keys[SDLK_i] && keys[SDLK_RSHIFT]))
      diffuse +=5;
   else if (keys[SDLK_i])
      diffuse -=5;
   else if ((keys[SDLK_o] && keys[SDLK_LSHIFT]) || (keys[SDLK_y] && keys[SDLK_RSHIFT]))
      specular +=5;
   else if (keys[SDLK_o])
      specular -=5;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   if(fpview){
      double dt = .03;
	  /* handle two directions which are not opposing */
	  if((keys[SDLK_w] && keys[SDLK_a]) && !map){
		 Ex += Cx*dt;
         Ez += Cz*dt;
		 fpangle -=5;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex -= Cx*dt;
			Ez -= Cz*dt;
		 }
	  }
	  else if((keys[SDLK_w] && keys[SDLK_d]) && !map){
		 Ex += Cx*dt;
         Ez += Cz*dt;
		 fpangle +=5;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex -= Cx*dt;
			Ez -= Cz*dt;
		 }
		  
	  }
	  else if((keys[SDLK_s] && keys[SDLK_a]) && !map){
		 Ex -= Cx*dt;
         Ez -= Cz*dt;
		 fpangle -=5;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex += Cx*dt;
			Ez += Cz*dt;
		 }
	  }
	  else if((keys[SDLK_s] && keys[SDLK_d]) && !map){
		 Ex -= Cx*dt;
         Ez -= Cz*dt;
		 fpangle +=5;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex += Cx*dt;
			Ez += Cz*dt;
		 }
	  }
      else if (keys[SDLK_w] && !map){
         Ex += Cx*dt;
         Ez += Cz*dt;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex -= Cx*dt;
			Ez -= Cz*dt;
		 }
      }
      else if (keys[SDLK_a] && !map){
         fpangle -=5;
		 updatePlayerPos();
		 collision = checkCollision();
	  }
      else if (keys[SDLK_s] && !map){
         Ex-=Cx*dt;
         Ez-=Cz*dt;
		 updatePlayerPos();
		 collision = checkCollision();
		 if(collision)
		 {
			Ex += Cx*dt;
			Ez += Cz*dt;
		 }
      }
      else if (keys[SDLK_d] && !map){
		  fpangle +=5;
		  updatePlayerPos();
		  collision = checkCollision();
		  
	  }
	  
	  
      fpangle=fpangle%360;
   }
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   Th = Th%360;
   Ph = Ph%360;
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  return success
   return 1;
}

/*
 *   calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}




/*
 *  Start up SDL and start game loop!
 */
int main(int argc,char* argv[])
{
   // Set run loop var
   int run=1;
   // Delay for key inputs
   double t0=0; 
   Mix_Music* intromusic;
   SDL_Surface* screen;
   
   //  Initialize SDL
   SDL_Init(SDL_INIT_VIDEO);
   //  Request double buffered, true color window with Z buffering at 600x600
   screen = SDL_SetVideoMode(1000,600,0,SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
   if (!screen ) Fatal("Cannot set SDL video mode\n");
   //  Name the window
   SDL_WM_SetCaption("Nick Trierweiler SDL game Final Submission", "SDL_GAME");
   //  Set Screen Size
   reshape(screen->w,screen->h);
   // load tree textures 
   treetex[0] = LoadTexBMP("bark.bmp");
   treetex[1] = LoadTexBMP("bark2.bmp");
   // load other textures
   texname[1] = LoadTexBMP("siding.bmp");
   texname[2] = LoadTexBMP("test.bmp");
   texname[3] = LoadTexBMP("roof.bmp");
   texname[4] = LoadTexBMP("window.bmp");
   texname[5] = LoadTexBMP("door.bmp");
   texname[6] = LoadTexBMP("brick.bmp");
   texname[7] = LoadTexBMP("brickcircle.bmp");
   texname[8] = LoadTexBMP("leafy.bmp");
   texname[9] = LoadTexBMP("core.bmp");
   texname[10] = LoadTexBMP("grass.bmp");
   //load skybox textures
   woods[0] = LoadTexBMP("sky0.bmp");
   woods[1] = LoadTexBMP("sky1.bmp");
   woods[2] = LoadTexBMP("sky2.bmp");
   
   //  Initialize audio
   if (Mix_OpenAudio(44100,AUDIO_S16SYS,2,4096)) Fatal("Cannot initialize audio\n");
   //  Load "The Wall"
   intromusic = Mix_LoadMUS("breathing.ogg");
   if (!intromusic) Fatal("Cannot load breathing.ogg\n");
   //  Play (looping)
   Mix_PlayMusic(intromusic,-1);
   
   ErrCheck("init");
   
   // SDL Event Loop
   while (run)
   {
      //  Elapsed time in seconds
      double t = SDL_GetTicks()/1000.0;
      //  Process all pending events
      SDL_Event event;
      while (SDL_PollEvent(&event))
         switch (event.type)
         {
            case SDL_VIDEORESIZE:
               screen = SDL_SetVideoMode(event.resize.w,event.resize.h,0,SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
               reshape(screen->w,screen->h);
               break;
            case SDL_QUIT:
               run = 0;
               break;
            case SDL_KEYDOWN:
               run = key();
               t0 = t+0.5;  // Wait 1/2 s before repeating
               break;
            default:
               //  Do nothing
               break;
         }
      //  Repeat key every 50 ms
      if (t-t0>0.05)
      {
         run = key();
         t0 = t;
      }
      //  Display
      
      display();
      //  Slow down display rate to about 100 fps by sleeping 5ms
      SDL_Delay(5);
   }
   
   Mix_CloseAudio();
   SDL_Quit();
  
   return 0;
}
