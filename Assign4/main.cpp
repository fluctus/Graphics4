#include <stdio.h>
#include <math.h>
#include <windows.h>
#include "glut.h"
#include "Vector3f.h"
#include <vector>
#include <utility>
GLfloat rot;

using namespace std;

vector<Vector3f*> vertices, normals;
vector<vector<vector<vector<pair<int,int>* >* >* >* > objs; //objects[groups[faces[vector<pair>]]]
int mouseX=0,mouseY=0;
enum modetype{CAMERA, GLOBAL};
int mode = CAMERA;
int pressedMouseButton;
int cameraX = 0;
int cameraY = 0;
int cameraZ = 0;
float zoom = 0;

void parseVertex(FILE *f){
	Vector3f *v = new Vector3f();

	fscanf(f, "%f", & v->x);
	fseek(f, 1, SEEK_CUR);
	fscanf(f, "%f", & v->y);
	fseek(f, 1, SEEK_CUR);
	fscanf(f, "%f", & v->z);
	fseek(f, 1, SEEK_CUR);

	vertices.push_back(v);
}

void parseNormal(FILE *f){
	Vector3f *v = new Vector3f();

	fseek(f, 1, SEEK_CUR);
	fscanf(f, "%f", & v->x);
	fseek(f, 1, SEEK_CUR);
	fscanf(f, "%f", & v->y);
	fseek(f, 1, SEEK_CUR);
	fscanf(f, "%f", & v->z);
	fseek(f, 1, SEEK_CUR);

	normals.push_back(v);
}

void parseFace(FILE *f){
	vector<pair<int,int>* > *face = new vector<pair<int,int>* >();
	char c;
	while(fscanf(f, "%c", &c) > 0){
		while(c == ' '){
			fscanf(f, "%c", &c);
		}
		if(c == '\n' || c == '\r')
			break;
		fseek(f, -1, SEEK_CUR);
		pair<int,int> *p = new pair<int,int>();
		fscanf(f, "%d", & p->first);
		fseek(f, 2, SEEK_CUR);
		fscanf(f, "%d", & p->second);
		face->push_back(p);
	}
	objs.back()->back()->push_back(face);
}

void parseGroup(FILE *f){
	vector<vector<pair<int,int>* >* > *group = new vector<vector<pair<int,int>* >* >();
	char c;
	while(fscanf(f, "%c", &c) > 0){
		if(c == '\n')
			break;
	}
	objs.back()->push_back(group);
}

void parseObject(FILE *f){
	vector<vector<vector<pair<int,int>* >* >* > *obj = new vector<vector<vector<pair<int,int>* >* >* >();
	char c;
	while(fscanf(f, "%c", &c) > 0){
		if(c == '\n')
			break;
	}
	objs.push_back(obj);
}

void readData(){
	FILE *f;
	char type = 0;
	vector<vector<vector<pair<int,int>* >* >* > *firstObj = new vector<vector<vector<pair<int,int>* >* >* >();
	objs.push_back(firstObj);

	f = fopen("doll.obj", "rb");
	while (fscanf(f, "%c", &type) > 0) {
		if (type == 'v') {
			fscanf(f, "%c", &type);
			if (type == 'n'){
				parseNormal(f);
			}else{
				parseVertex(f);
			}
		} else if (type == 'f') {
			parseFace(f);
		} else if (type == 'g') {
			parseGroup(f);
		} else if (type == 'o') {
			parseObject(f);
		} else if (type == '\n') {
		} else if (type == '\r') {
		} else if (type == '#') {
			while(fscanf(f, "%c", &type) > 0){
				if(type == '\n')
					break;
			}
		}
		else{
			printf("illegal tag\n");
		}
	}

	fclose(f);
}

void drawAxis() {
	GLfloat mat_x[] = {1, 0, 0, 1};
	GLfloat mat_y[] = {0, 0, 1, 1};
	GLfloat mat_z[] = {0, 1, 0, 1};
	// X
	glLineWidth(2);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_x);
	glBegin(GL_LINES);
		glVertex3f(-50.0, 0.0, 0.0);
		glVertex3f(50.0, 0.0, 0.0);
	glEnd();

	// Y
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_y);
	glBegin(GL_LINES);
		glVertex3f(0.0, -50.0, 0.0);
		glVertex3f(0.0, 50.0, 0.0);
	glEnd();

	// Z
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_z);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, -50.0);
		glVertex3f(0.0, 0.0, 50.0);
	glEnd();
}

void drawObjects() {
	// Go through all objects
	for (int objIndx = 0; objIndx < objs.size(); objIndx++) {

		vector<vector<vector<pair<int,int>* >* >* > *groups = objs.at(objIndx);
		// Go through all groups
		for (int grpIndx = 0; grpIndx < groups->size(); grpIndx++) {

			vector<vector<pair<int,int>* >* > *faces = groups->at(grpIndx);
			// Go thorugh all faces
			for (int fcIndx = 0; fcIndx < faces->size(); fcIndx++) {
				vector<pair<int,int>* > *pairs = faces->at(fcIndx);
				glBegin(GL_POLYGON);
				for (int prIndx = 0; prIndx < pairs->size(); prIndx++) {
					Vector3f *vertex = vertices.at(pairs->at(prIndx)->first - 1);
					Vector3f *normal = normals.at(pairs->at(prIndx)->second - 1);

					glNormal3f(normal->x ,normal->y, normal->z);
					glVertex3f(vertex->x ,vertex->y, vertex->z);
				}
				glEnd();
			}
		}
	}
}

void initMaterials() {
	GLfloat mat_a[] = {0.3, 0.4, 0.5, 1.0};
	GLfloat mat_d[] = {0.0, 0.6, 0.7, 1.0};
	GLfloat mat_s[] = {0.0, 0.0, 0.8, 1.0};
	GLfloat mat_e[] = {0, 0, 0, 1};
	GLfloat low_sh[] = {5.0};
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_e);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_sh);
}

void mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	//glRotatef(rot,0,1,0); //rotate scene

	//glMatrixMode(GL_PROJECTION); /* switch matrix mode */

	// defines view mode
//	glLoad

	//glMatrixMode(GL_MODELVIEW);

	drawAxis();

	initMaterials();

	drawObjects();

	glFlush(); //print to screen
}

void disp(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1,disp,0);
}

void initLight()
{
	//lightning
	GLfloat light_direction[]= {0,-1,-1,1.0};
	GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0}; //color
	GLfloat light_diffuse[] = {0.0, 0.5, 0.5, 1.0}; //color
	GLfloat light_specular[] = {0.0, 0.0, 0.5, 1.0};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

//	glEnable(GL_COLOR_MATERIAL);
//	glDisable(GL_LIGHTING);

}


void init()
{
	glClearColor(0,0,0,1);
	
	glMatrixMode(GL_PROJECTION); /* switch matrix mode */
	glLoadIdentity();		//load Identity matrix

	// defines view mode
	gluPerspective(60+zoom,1,2,200);
	glTranslatef(-cameraX,-cameraY,-(100+cameraZ));

	glEnable(GL_DEPTH_TEST);  //define in which order the scene will built
	 /* return to modelview mode */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	rot=0;
	initLight();
}



void mouse(int button, int state, int x, int y) 
{
	if(mode == CAMERA){
		switch (button) {
			case GLUT_LEFT_BUTTON:
				if (state == GLUT_UP){
					pressedMouseButton = 0;
					mouseX = 0;
					mouseY = 0;
				}
				else if(state == GLUT_DOWN){
					pressedMouseButton = GLUT_LEFT_BUTTON;
					if (mouseX == 0){
						mouseX = x;
					}
					if (mouseY == 0){
						mouseY = y;
					}
				}
			break;
			case GLUT_MIDDLE_BUTTON:
				if (state == GLUT_UP){
					pressedMouseButton = 0;
					mouseY = 0;
				}
				else if(state == GLUT_DOWN){
					pressedMouseButton = GLUT_MIDDLE_BUTTON;
					if (mouseY == 0){
						mouseY = y;
					}
				}
			break;
			case GLUT_RIGHT_BUTTON:
				if (state == GLUT_UP){
					pressedMouseButton = 0;
					mouseX = 0;
					mouseY = 0;
				}
				else if(state == GLUT_DOWN){
					pressedMouseButton = GLUT_RIGHT_BUTTON;
					if (mouseX == 0){
						mouseX = x;
					}
					if (mouseY == 0){
						mouseY = y;
					}
				}
			break;
		}
	}
	else if(mode == GLOBAL){
		switch (button) {
			case GLUT_LEFT_BUTTON:
				if (state == GLUT_UP){
					pressedMouseButton = 0;
					mouseX = 0;
					mouseY = 0;
				}
				else if(state == GLUT_DOWN){
					pressedMouseButton = GLUT_LEFT_BUTTON;
					if (mouseX == 0){
						mouseX = x;
					}
					if (mouseY == 0){
						mouseY = y;
					}
				}
			break;
		}
	}
}

void mouseMove(int x, int y){
	if(mode == CAMERA){
		if(pressedMouseButton == GLUT_LEFT_BUTTON){
			if(mouseX != 0){
				int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
				rot=180*(mouseX - x)/windowWidth;
				glTranslatef(cameraX,cameraY,100+cameraZ);
				glRotatef(rot,0,1,0);
				glTranslatef(-cameraX,-cameraY,-(100+cameraZ));
				mouseX = x;
			}
			if(mouseY != 0){
				int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
				rot=180*(mouseY - y)/windowHeight;
				glTranslatef(cameraX,cameraY,100+cameraZ);
				glRotatef(rot,1,0,0);
				glTranslatef(-cameraX,-cameraY,-(100+cameraZ));
				mouseY = y;
			}
		}
		else if(pressedMouseButton == GLUT_MIDDLE_BUTTON){
			if(mouseY != 0){
				int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
				cameraX = 50*(mouseY - y)/windowHeight;
				cameraY = 50*(mouseY - y)/windowHeight;
				cameraZ = 50*(mouseY - y)/windowHeight;
				glTranslatef(cameraX,cameraY,cameraZ);
				mouseY = y;
			}
		}
		else if(pressedMouseButton == GLUT_RIGHT_BUTTON){
			if(mouseY != 0){
				int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
				cameraY = 50*(mouseY - y)/windowHeight;
				glTranslatef(0,cameraY,0);
				mouseY = y;
			}
			if(mouseX != 0){
				int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
				cameraX = 50*(mouseX - x)/windowWidth;
				glTranslatef(cameraX,0,0);
				mouseX = x;
			}
		}
	}
	else if(mode == GLOBAL){
		if(mouseX != 0){
			int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
			rot=180*(mouseX - x)/windowWidth;
			glRotatef(rot,0,1,0);
			mouseX = x;
		}
		if(mouseY != 0){
			int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
			rot=180*(mouseY - y)/windowHeight;
			glRotatef(rot,1,0,0);
			mouseY = y;
		}
	}
}

void keyboard(unsigned char key, int x, int y){
	if(key == ' '){
		mode = 1 - mode;
	}
}

void special(int key, int x, int y){
	if(key == GLUT_KEY_F2){
		zoom -= 0.05;
		//float modelMatrix[16],projectionMatrix[16];
		//glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		//glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60+zoom,1,2,200);
		glTranslatef(0,0,-100);
		glMatrixMode(GL_MODELVIEW);
		//glMultMatrixf(modelMatrix);
	}
	else if(key == GLUT_KEY_F3){
		zoom += 0.05;
		//float modelMatrix[16],projectionMatrix[16];
		//glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		//glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60+zoom,1,2,200);
		glTranslatef(0,0,-100);
		glMatrixMode(GL_MODELVIEW);
		//glMultMatrixf(modelMatrix);
	}
}

int main(int  argc,  char** argv) 
{
	readData();
	glutInit (& argc, argv) ;
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB) ;
	glutInitWindowSize (1024,1024) ;
	glutCreateWindow("Ass4") ;

	init();
	glutDisplayFunc(mydisplay);

	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glutTimerFunc(2,disp,0);
	glutMainLoop ();
}
