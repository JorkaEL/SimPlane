#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "glimage.h"
#include "SceneContext.h"

#define  GL_GLEXT_PROTOTYPES

#define XMAX 5000
#define ZMAX 10000
#define YMAX 5000
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define ESC 27
#define DEP 0.1
#define TBOUCLE 10000

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h> 
#else
#include <GL/glut.h>
#endif

#define INFO if(0) printf

void init(void);
void InitBufferObject(GLuint, int, GLuint*, GLvoid*);
void Sol(void);
void Ciel(void);
void Fond(void);
void Display(void);
void Reshape(int, int);
void back(void);
void coter(void);

SceneContext * gSceneContext;
SceneContext * iMeuble1;
/*SceneContext * iMeuble2;
SceneContext * iMeuble3;
SceneContext * iMeuble4;
*/
GLfloat rotation=0.0;

GLuint idvbosol = 0;
GLuint idindexsol = 0;

GLuint idvbociel = 0;
GLuint idindexciel = 0;

GLuint idvbofond = 0;
GLuint idindexfond = 0;

GLuint idvboback = 0;
GLuint idindexback = 0;

GLuint idvbogauche = 0;
GLuint idindexgauche = 0;

GLuint idvbodroit = 0;
GLuint idindexdroit = 0;

GLuint texsol = 0;
GLuint texciel = 0;
GLuint texfond = 0;
GLuint texcote = 0;

GLfloat liCouleur[4] = { 1.0, 1.0, 1.0, 0.0 };


GLfloat pas =1.0, pasModel=10.0, xcam = 0.0, ycam = 2.0, zcam = ZMAX, eyecamx=xcam, eyecamy=ycam, eyecamz=zcam-1, angle=0.0, ecart=0.0; // variables de caméra
GLfloat xmodelA=eyecamx,ymodelA=0.0,zmodelA=zcam-pasModel, angrotx=0.0, angroty=180.0,angrotz=0.0;//variables de déplacement de l'avion


GLint vitesse=0,t=0,vitesseDecrochage=6;
bool avance= false, envole=false;


void 
ExitFunction() {
  delete gSceneContext;
  delete iMeuble1;
}

void 
Timer(int) {
  // Ask to display the current frame only if necessary.
  if (gSceneContext->GetStatus() == SceneContext::MUST_BE_REFRESHED) {
    glutPostRedisplay();
  }

  gSceneContext->OnTimerClick();
  // Call the timer to display the next frame.
  glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), Timer, 0);
  
}


void 
TimerI(int) {
  // Ask to display the current frame only if necessary.
  if (iMeuble1->GetStatus() == SceneContext::MUST_BE_REFRESHED) {
    glutPostRedisplay();
  }

  iMeuble1->OnTimerClick();
  // Call the timer to display the next frame.
  glutTimerFunc((unsigned int)iMeuble1->GetFrameTime().GetMilliSeconds(), TimerI, 0);
}


void init(void){
    
	GLfloat sol[]={XMAX, 0.0, 0.0,
		0.0, 1.0, //blanc
		XMAX, 0.0, ZMAX,
		0.0, 0.0, //blanc
		-XMAX, 0.0, ZMAX,
		1.0, 0.0, //blanc
		-XMAX, 0.0, 0.0,
		1.0, 1.0 //blanc
	};

	GLuint indexsol[]={ 0, 1, 2, 3};
	
	
	GLfloat ciel[]={XMAX, YMAX, 0.0,
		0.0, 1.0, //blanc
		XMAX, YMAX, ZMAX,
		0.0, 0.0, //blanc
		-XMAX, YMAX, ZMAX,
		1.0, 0.0, //blanc
		-XMAX, YMAX, 0.0,
		1.0, 1.0 //blanc
	};

	GLuint indexciel[]={ 0, 1, 2, 3};
	

		GLfloat fond[]={XMAX, 0.0, 0.0,
		1.0,0.0,
		XMAX, YMAX, 0.0,
		1.0, 1.0, 
		-XMAX, YMAX, 0.0,
		0.0, 1.0, 
		-XMAX, 0.0, 0.0,
		0.0,0.0 
	};

	GLuint indexfond[]={ 0, 1, 2, 3};
	
	GLfloat back[]={XMAX, 0.0, ZMAX,
		1.0,0.0,
		XMAX, YMAX, ZMAX,
		1.0, 1.0, 
		-XMAX, YMAX, ZMAX,
		0.0, 1.0, 
		-XMAX, 0.0, ZMAX,
		0.0,0.0 
	};

	GLuint indexback[]={ 0, 1, 2, 3};

	GLfloat gauche[]={-XMAX, 0.0, ZMAX,
		1.0,0.0,
		-XMAX, YMAX, ZMAX,
		1.0, 1.0, 
		-XMAX, YMAX, 0.0,
		0.0, 1.0, 
		-XMAX, 0.0, 0.0,
		0.0,0.0 
	};

	GLuint indexgauche[]={ 0, 1, 2, 3};

	GLfloat droit[]={XMAX, 0.0, ZMAX,
		1.0, 0.0, //vert
		XMAX, YMAX, ZMAX,
		1.0, 1.0, //vert
		XMAX, YMAX, 0.0,
		0.0, 1.0, //vert
		XMAX, 0.0, 0.0,
		0.0, 0.0 //vert
	};

	GLuint indexdroit[]={ 0, 1, 2, 3};


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
 	glEnable(GL_LIGHT0);
  glClearColor(0.0, 0.0, 0.0, 0.0);

	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(sol), &idvbosol, sol);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexsol), &idindexsol, indexsol);
	
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(ciel), &idvbociel, ciel);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexciel), &idindexciel, indexciel);

		INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(fond), &idvbofond, fond);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexfond), &idindexfond, indexfond);
	
			INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(back), &idvboback, back);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexback), &idindexback, indexback);
	printf("%d\n", idindexback);

			INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(gauche), &idvbogauche, gauche);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexgauche), &idindexgauche, indexgauche);

				INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(droit), &idvbodroit, droit);
	INFO("%s %d\n", __func__, __LINE__);
	InitBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexdroit), &idindexdroit, indexdroit);

	//tex
	  if (!glimageLoadAndBind2 ("image/Herbe.bmp", &texsol)) exit(0);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

	if (!glimageLoadAndBind2 ("image/Paysageenneige.bmp", &texfond)) exit(0);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  if (!glimageLoadAndBind2 ("image/Paysageenneige.bmp", &texcote)) exit(0);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  if (!glimageLoadAndBind2 ("image/ciel.jpg", &texciel)) exit(0);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  
}

void InitBufferObject(GLuint TYPE, int size, GLuint *id, GLvoid *data){
  
  int sizeInByte;
  if (!glIsBuffer(*id)) glGenBuffers(1, id);

  glBindBuffer(TYPE, *id);
  glBufferData(TYPE, size, data, GL_STATIC_DRAW);  

  glGetBufferParameteriv(TYPE, GL_BUFFER_SIZE, &sizeInByte );
  printf("Taille du Buffer %s : \t %d octets\n", (TYPE==GL_ARRAY_BUFFER)?"Buffer Array":"Element Array", sizeInByte);
  glBindBuffer(TYPE, 0);
}



void Sol(void){

	glPushMatrix();

	INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, idvbosol);
	glBindTexture(GL_TEXTURE_2D, texsol);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexsol);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
  glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void Ciel(void){

	glPushMatrix();

	INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, idvbociel);
	glBindTexture(GL_TEXTURE_2D, texciel);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexciel);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
  glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void Fond(void){

	glPushMatrix();

	INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, idvbofond);
	glBindTexture(GL_TEXTURE_2D, texfond);
  glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);  
INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexfond);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
	//glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void back(void){

	glPushMatrix();

	INFO("%s %d\n", __func__, __LINE__);

  glBindBuffer(GL_ARRAY_BUFFER, idvboback);
	glBindTexture(GL_TEXTURE_2D, texfond);
  glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);  
INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexback);
INFO("%s %d\n", __func__, __LINE__);
  glDrawElements(GL_QUADS, 20, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
	//glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void coter(void){
	
	glPushMatrix();

	//gauche
	INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, idvbogauche);
  glBindTexture(GL_TEXTURE_2D, texcote);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexgauche);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
  //glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	//droit
	glPushMatrix();
	INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ARRAY_BUFFER, idvbodroit);
  glBindTexture(GL_TEXTURE_2D, texcote);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  INFO("%s %d\n", __func__, __LINE__);
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

INFO("%s %d\n", __func__, __LINE__);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idindexdroit);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, BUFFER_OFFSET(0));  

INFO("%s %d\n", __func__, __LINE__);
  //glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);


	glPopMatrix();


}

void Immeuble(void){
			glPushMatrix();
	glTranslatef(0.0, 15.0, 100.0);

	iMeuble1->OnDisplay();
	glPopMatrix();
	
	if (iMeuble1->GetStatus() == SceneContext::MUST_BE_LOADED)  {
    // This function is only called in the first display callback
    // to make sure that the application window is opened and a 
    // status message is displayed before.

    iMeuble1->LoadFile();
    iMeuble1->SetCurrentAnimStack(0);

   glutTimerFunc((unsigned int)iMeuble1->GetFrameTime().GetMilliSeconds(), TimerI, 0);
  }

}

void Avion(void){
		glPushMatrix();
	glTranslatef(xmodelA, ymodelA, zmodelA);
	glRotatef(angroty,0.0,1.0,0.0);
	glRotatef(angrotx,1.0,0.0,0.0);
	glRotatef(angrotz,0.0,0.0,1.0);
	gSceneContext->OnDisplay();
	glPopMatrix();
	
	if (gSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)  {
    // This function is only called in the first display callback
    // to make sure that the application window is opened and a 
    // status message is displayed before.

    gSceneContext->LoadFile();
    gSceneContext->SetCurrentAnimStack(0);

    glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), Timer, 0);
  }
	
}


void Display (void){
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
	gluLookAt(xcam-rotation, ycam, zcam,eyecamx, eyecamy, eyecamz, 0.0, 1.0, 0.0);
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, liCouleur);
 	Sol();
 	Ciel();
	Fond();
	back();
	coter();
	Avion();
	//Immeuble();
  glutSwapBuffers();
    
}

void Reshape (int width, int height){
  
  glViewport(0,0,(GLsizei)(width),(GLsizei)(height));
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  gluPerspective(60.0f, (GLfloat)width/height, 0.01f, 20000.0);	

  glMatrixMode(GL_MODELVIEW);


}

void 
Keyboard (unsigned char key, int x, int y) { 
  switch(key){
  case ESC : 
    exit(0);
    break;
  case 'r':

    break;
  default:
    fprintf(stderr, "Unused Key\n");
    break;
  case 'p':
    //glutIdleFunc(NULL);
    break;
	case ' ':
					ecart=0.0;
					avance=true;
					//virage=false;
					if(vitesse < 60){
					vitesse+=8;
					}
    break;
	case 't':
			//if (ycam > 2.0){
    	ycam -= pas;
    	eyecamy=ycam;
    	ymodelA -= pas;
    	//}
    break;
  case 'P':
    //glutIdleFunc(Idle);
    break;
  }
  glutPostRedisplay();
}

void 
Special (int key, int x, int y) { 
  switch(key){
  case GLUT_KEY_UP:
 INFO("Avant UP\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("zcam: %.1f \n",zcam);
					INFO("ecart: %.1f \n",ecart);
    			INFO("\n");
    			/*** indique que l'on avance et donc que l'on fait pas de virage, augmante la vitesse ***/
					
					if(angrotx <= 90){
					angrotx+=4.5;
  				}
    			INFO("Apres UP\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("zcam: %.1f \n",zcam);
					INFO("ecart: %.1f \n",ecart);
    			INFO("\n");
    			
    			INFO("......... \n");
    break;
  case GLUT_KEY_DOWN:

					INFO("Avant Down\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
					INFO("ecart: %.1f \n",ecart);
    			INFO("\n");
				
					if(angrotx >= -90){
					angrotx-=4.5;
					}
				
    			INFO("Apres Down\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
					INFO("ecart: %.1f \n",ecart);
    			INFO("\n");
    			
    			INFO("......... \n");


    break;
  case GLUT_KEY_RIGHT:
    		
    			INFO("Avant Right\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("angroty: %.1f \n",angroty);
    			INFO("\n");
    			
    			
    			if( angrotz< 30.0)
    					angrotz += 4.5;
				if(angrotz >= 0){
				
    			angle+=4.5;
				 if(fabs(angle)>=0 && fabs(angle) <45){
    						eyecamx=eyecamx+DEP;
						 		eyecamz=eyecamz;
						 		xmodelA += (DEP*pasModel);
						 		zmodelA=zmodelA;
						 		angroty = 180 - angle;
    			}else if(angle==45){
    						eyecamx = xcam + pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 - angle;
    			}else if(angle==-45){
    						eyecamx = xcam - pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 + angle;
    			}else if((angle >45 && angle <135) || (angle < -225 && angle > -315)){
    						eyecamx=eyecamx;
						 		eyecamz=eyecamz+DEP;
						 		zmodelA += (DEP*pasModel);
						 		angroty = 180 - angle;
    			}else if(angle==135 ){
    						eyecamx = xcam + pas;
    						eyecamz = zcam + pas;
    						zmodelA = zcam + pasModel;
    						xmodelA = xcam + pasModel;
    						angroty = 180 - angle;
    			}else if(angle==-135 ){
    						eyecamx = xcam - pas;
    						eyecamz = zcam + pas;
    						zmodelA = zcam + pasModel;
    						xmodelA = xcam - pasModel;
    						angroty = 180 + angle;
    			}else if(fabs(angle)>135 && fabs(angle) <225){
    						eyecamx = eyecamx-DEP;
						 		eyecamz = eyecamz;
						 		xmodelA -= (DEP*pasModel);
						 		angroty = 180 - angle;
    			}else if(angle == 255){
    						eyecamx = xcam - pas;
    						eyecamz = zcam + pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam + pasModel;
    						angroty = 180 - angle;
    			}else if(angle == -255){
    						eyecamx = xcam + pas;
    						eyecamz = zcam + pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 + angle;
    			}else if((angle >225 && angle <315) || (angle < -45 && angle > -135)){
    							eyecamx=eyecamx;
						 			eyecamz=eyecamz-DEP;
						 			zmodelA -= (DEP*pasModel);
						 			angroty = 180 - angle;
    			}else if(angle == -315){
    						eyecamx = xcam + pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 + angle;
    			}else if(angle == 315){
    						eyecamx = xcam - pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 - angle;
    			}else if(fabs(angle)>315 && fabs(angle) <=360){
    							eyecamx=eyecamx+DEP;
						 			eyecamz=eyecamz;
						 			xmodelA += (DEP*pasModel);
						 			angroty = 180 - angle;
						 			if(fabs(angle) == 360){
						 					angle = 0.0;
						 			}
    			}
				}
    			INFO("Apres Right\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("angroty: %.1f \n",angroty);
    			INFO("\n");
    			
    			INFO("......... \n");
    			
    break;
  case GLUT_KEY_LEFT:
					
    		INFO("Avant Left\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("angroty: %.1f \n",angroty);
    			INFO("\n");
    			
    			
    			if( angrotz > -30.0)
    					angrotz -= 4.5;
    			if(angrotz <= 0){
    					angle-=4.5;
				 if(fabs(angle)>=0 && fabs(angle) <45){
    						eyecamx=eyecamx-DEP;
						 		eyecamz=eyecamz;
						 		xmodelA -= (DEP*pasModel);
						 		angroty = 180 - angle;
    			}else if(angle==45){
    						eyecamx = xcam + pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 + angle;
    			}else if(angle==-45){
    						eyecamx = xcam - pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 - angle;
    			}else if((angle >45 && angle <135) || (angle < -225 && angle > -315)){
    						eyecamx=eyecamx;
					 			eyecamz=eyecamz-DEP;
					 			zmodelA -= (DEP*pasModel);
					 			angroty = 180 - angle;
    			}else if(angle==135 ){
    						eyecamx = xcam + pas;
    						eyecamz = zcam + pas;
    						zmodelA = zcam + pasModel;
    						xmodelA = xcam + pasModel;
    						angroty = 180 + angle;
    			}else if(angle == -135 ){
    						eyecamx = xcam - pas;
    						eyecamz = zcam + pas;
    						zmodelA = zcam + pasModel;
    						xmodelA = xcam - pasModel;
    						angroty = 180 - angle;
    			}else if(fabs(angle) > 135 && fabs(angle) < 225){
    							eyecamx=eyecamx+DEP;
						 			eyecamz=eyecamz;
						 			xmodelA += (DEP*pasModel);
						 			angroty = 180 - angle;
    			}else if(angle == 255){
    						eyecamx = xcam - pas;
    						eyecamz = zcam + pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam + pasModel;
    						angroty = 180 + angle;
    			}else if(angle == -255){
    						eyecamx = xcam + pas;
    						eyecamz = zcam + pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 - angle;
    			}else if((angle >225 && angle <315) || (angle < -45 && angle > -135)){
    							eyecamx=eyecamx;
						 			eyecamz=eyecamz+DEP;
						 			zmodelA +=(DEP*pasModel);
						 			angroty = 180 - angle;
    			}else if(angle == -315){
    						eyecamx = xcam + pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam + pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 - angle;
    			}else if(angle == 315){
    						eyecamx = xcam-pas;
    						eyecamz = zcam - pas;
    						xmodelA = xcam - pasModel;
    						zmodelA = zcam - pasModel;
    						angroty = 180 + angle;
    			}else if(fabs(angle)>315 && fabs(angle) <=360){
    							eyecamx=eyecamx-DEP;
						 			eyecamz=eyecamz;
						 			xmodelA -= (DEP*pasModel);
						 			angroty = 180 - angle;
						 			if(fabs(angle) == 360){
						 					angle = 0.0;
						 			}
    				}
					}
    			INFO("Apres Left\n");
    			INFO("angle: %.1f \n",angle);
    			INFO("eyecamx: %.1f \n",eyecamx);
    			INFO("eyecamz: %.1f \n",eyecamz);
    			INFO("xcam: %.1f \n",xcam);
    			INFO("zcam: %.1f \n",zcam);
    			INFO("xmodelA: %.1f \n",xmodelA);
    			INFO("angroty: %.1f \n",angroty);
    			INFO("\n");
    			
    			INFO("......... \n"); 		
    break;
  default:
    fprintf(stderr, "Unused Key\n");
    break;
  }
  glutPostRedisplay();
}

void idle(void){
	
	if(avance){
	t+=1;

	if(t == TBOUCLE){
		
		ecart=0.0;
					
					/*** fait avancer la camera vers la ou on regarde ainsi que le model ***/
					if(angrotx < 0){
						if(vitesse >= vitesseDecrochage){
						ycam += pas;
    				eyecamy=ycam;
    				ymodelA += pas;
    				}
					}else if(angrotx > 0){
						if(vitesse >= vitesseDecrochage){
								ycam -= pas;
    						eyecamy=ycam;
    						ymodelA -= pas;
    				}else{
    					ycam -= 5;
    					eyecamy=ycam;
    					ymodelA -= 5;
    				}
    				
					}else{
						ycam = ycam;
    				eyecamy = eyecamy;
    				ymodelA = ymodelA;
					}
				//printf("%.1f",angrotx);				
					
			if(fabs(angrotx) != 90){
  				if(angle == 0){
  						zcam -= pas;
  						eyecamz -= pas;
  						zmodelA-= pas;
  				}else if((angle>0 && angle <=45) || (angle < -315 && angle > -360)){
								ecart=fabs(xcam-eyecamx);
  							xcam = eyecamx;
    						zcam -= pas;
    						eyecamz -= pas;
    						eyecamx += ecart;
    						xmodelA += ecart;
    						zmodelA -= pas;
    			}else if((angle>45 && angle <=90) || (angle < -270 && angle >= -315)){
								ecart=fabs(zcam-eyecamz);
    						xcam =eyecamx ;
    						eyecamx+= pas;
    						zcam = eyecamz;
						 		eyecamz -= ecart;
						 		xmodelA += pas;
    						zmodelA -= ecart;
    			}else if((angle>90 && angle <=135) || (angle < -225 && angle >= -270)){
    						ecart=fabs(zcam-eyecamz);
    						xcam =eyecamx ;
    						eyecamx+= pas;
    						zcam = eyecamz;
						 		eyecamz+=ecart;
						 		xmodelA += pas;
    						zmodelA += ecart;
    			}else if((angle >135 && angle <=180) || (angle < -180 && angle >= -225)){
    						ecart=fabs(xcam-eyecamx);
  							xcam = eyecamx;
    						zcam += pas;
    						eyecamz += pas;
    						eyecamx += ecart;
    						xmodelA += ecart;
    						zmodelA += pas;
    			}else if((angle >180 && angle <=225) || (angle < -135 && angle >= -180)){
    						ecart=fabs(xcam-eyecamx);
  							xcam = eyecamx;
    						zcam += pas;
    						eyecamz += pas;
    						eyecamx -= ecart;
    						xmodelA -= ecart;
    						zmodelA += pas;
    			}else if((angle >225 && angle <=270) || (angle < -90 && angle >= -135)){
    						ecart=fabs(zcam-eyecamz);
    						xcam =eyecamx ;
    						eyecamx-= pas;
    						zcam = eyecamz;
						 		eyecamz+=ecart;
						 		xmodelA -= pas;
    						zmodelA += ecart;
    			}else if((angle >270 && angle <=315) || (angle < -45 && angle >= -90)){
    						ecart=fabs(zcam-eyecamz);
    						xcam =eyecamx ;
    						eyecamx-= pas;
    						zcam = eyecamz;
						 		eyecamz-=ecart;
						 		xmodelA -= pas;
    						zmodelA -= ecart;
    			}else if((angle >315 && angle <360) || (angle >= -45 && angle < 0)){
    						ecart=fabs(xcam-eyecamx);
  							xcam = eyecamx;
    						zcam -= pas;
    						eyecamz -= pas;
    						eyecamx -=ecart;
    						xmodelA -= ecart;
    						zmodelA -= pas;
    			}
    	}
		t=0;

	
		/*** si la vitesse depasse une certaine valeur l'avion s'envole ***/
		if(vitesse > 25 && angrotx < 0 ){
			envole=true;
			/*	if (ycam <YMAX/1.9){
    			ycam += pas;
    			eyecamy=ycam;
    			ymodelA += pas;
    		}*/
    	}
    	
    if(envole){
    	/*** si l'avion est envole et que la vitesse est trop faible, il perd de l'altitude et decroche ***/
    	  if(vitesse < vitesseDecrochage && (angrotx == 0 || angrotx == -4.5)){
    						if(ymodelA){
    							ycam -= pas;
    							eyecamy=ycam;
    							ymodelA -= pas;
    						}else{
    							angrotx = 0.0;
    							envole=false;
    						
    						}
    		}else if (vitesse < vitesseDecrochage && angrotx < 4.5){
    					angrotx = -angrotx;	
    		}else if(ymodelA == YMAX-pasModel){
    					angrotx = -angrotx;
    					vitesse = 0;
    		
    		}
    		
    		
    		if(vitesse > 0)
    			vitesse--;
			}
			printf("%d \n",vitesse);
	}	
		/*printf(" vitesse : %d", vitesse);
		printf(" angle rot x : %.1f",angrotx);
		printf(" ymodelA: %.1f",ymodelA);*/
	}
	
	if(zmodelA <= 0.0){
			printf("Perdu vous vous etes crashé !!!!\n");	
			exit(0);
	}else if(zmodelA >= ZMAX){
			printf("Perdu vous vous etes crashé !!!!\n");
			exit(0);
	}else if(xmodelA >= XMAX){
			printf("Perdu vous vous etes crashé !!!!\n");
			exit(0);
	}else if(xmodelA <= -XMAX){
			printf("Perdu vous vous etes crashé !!!!\n");
			exit(0);
	}else if(ymodelA < 0.0){
			printf("Perdu vous vous etes crashé !!!!\n");
			exit(0);
	}else if(angrotx > 0.0 && ymodelA == 0.0){
			printf("Perdu vous vous etes crashé !!!!\n");
			exit(0);
	}
	
}

int main(int argc, char **argv){

  glutInit(&argc, argv); 
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (500, 500); 
  glutInitWindowPosition (100, 100); 
  glutCreateWindow (argv[0]); 

  //srand(time(NULL));

  init();
  
  gSceneContext = new SceneContext("Models/avion/Cessna.fbx");//new SceneContext("Models/avion/SC_Private_001.fbx");

	iMeuble1 = new SceneContext("Models/Immeuble/Tower.FBX");//new SceneContext("Models/Immeuble/Dark_castle.fbx");//new SceneContext("Models/Immeuble/geisterhaus1_1.fbx");//new SceneContext("Models/Immeuble/Medieval_building.FBX");//new SceneContext("Models/Immeuble/fbx-EB-112.FBX");//new SceneContext("Models/Immeuble/sci_fi_towers_fbx.FBX");//new SceneContext("Models/Immeuble/High_Rise_Buildings_FBX/High_Rise_Buildings.FBX");//new SceneContext("Models/Immeuble/sci_fi_towers_fbx.FBX");//new SceneContext("Models/Immeube/Building_fbx/Building.FBX");//new SceneContext("Models/avion/skyscraper.fbx");//new SceneContext("Models/Immeube/KLtower.fbx");//new SceneContext("Models/Immeube/mrakodrap.fbx");


  glutDisplayFunc(Display);
  glutReshapeFunc(Reshape); 
  glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutIdleFunc(idle);


  glutMainLoop(); 
  return 0; 
}
