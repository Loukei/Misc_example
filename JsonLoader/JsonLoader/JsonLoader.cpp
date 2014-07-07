// JsonLoader.cpp : 定義主控台應用程式的進入點。

//--------------------------------------
//	Include header
//	1.freeglut v2.8.1
//	2.jsoncons v0.93
//	3.other stander c++ lib for open .json file
//--------------------------------------
#include "stdafx.h"
#include <GL\freeglut.h>
#include <fstream>
#include <string>
#include <vector>
#include "jsoncons/json.hpp"

//----------------------------------------
//	Name space
//----------------------------------------
using namespace std;
using std::string;
using jsoncons::json;
using jsoncons::output_format;
using jsoncons::pretty_print;

//----------------------------------------
//	Function Prototypes
//	1.init(): compute bounding box
//	2.
//----------------------------------------
void init();
void display();
void specialKeys(int key, int x, int y);
void reshpae(int w, int h);

//------------------------------------------
//	Global Variables
//	1.rotate x,y control
//	2.json obj save the model
//------------------------------------------
double rotate_y=0; 
double rotate_x=0; 
json obj;

int _tmain(int argc,  char** argv)
{
	//  Inital glut window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("JsonLoader-demo");

	//  Enable Z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	//  Start loading json
	string fname = "input/voxdata32-earth.json";
	obj = json::parse_file(fname);
	init();

	// call back functions
	glutDisplayFunc(display);
	glutSpecialFunc(specialKeys);
	glutReshapeFunc(reshpae);

	//  Pass control to GLUT for events
	glutMainLoop();

	return 0;
}

void init(){
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	Caculate bounding box size
	int model_scale = ( obj["Modelscale"].as<std::vector<int>>() )[0];
	double voxel_scale = ( obj["Voxelscale"].as<std::vector<double>>() )[0];
	double box_scale = model_scale*voxel_scale*1.3; 

	glOrtho( 0.0, box_scale, 0.0, box_scale, 0.0, box_scale);
	gluLookAt(0.0, 0.0, box_scale, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void reshpae(int w, int h){
	glViewport(0, 0, w, h);
}

void display(){
 
  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  //  Reset transformations
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
 
  //  Rotate when user changes rotate_x and rotate_y
  glRotatef( rotate_x, 1.0, 0.0, 0.0 );
  glRotatef( rotate_y, 0.0, 1.0, 0.0 );
 
  //  Draw each voxel
  double vox_size = ( obj["Voxelscale"].as<std::vector<double>>() )[0];
  int vox_num = obj["Numofvoxel"].as<int>();

  //  For each voxel i
  for (int i = 0; i < vox_num; i++){
	  json& voxel = obj["Model"][i];
	  int tag = voxel["Tag"].as<int>();
	  //only draw border voxel
	  if (tag == 0){
		  GLfloat color_r, color_g, color_b;
		  color_r = voxel["RGBAcolor"].as<std::vector<int>>()[0];
		  color_g = voxel["RGBAcolor"].as<std::vector<int>>()[1];
		  color_b = voxel["RGBAcolor"].as<std::vector<int>>()[2];

		  GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
		  //voxel's center position
		  float pos_x = (voxel["Position"].as<std::vector<int>>()[0] + 1)*vox_size;
		  float pos_y = (voxel["Position"].as<std::vector<int>>()[1] + 1)*vox_size;
		  float pos_z = (voxel["Position"].as<std::vector<int>>()[2] + 1)*vox_size;
		  x_max = pos_x + 0.5*vox_size;
		  x_min = pos_x - 0.5*vox_size;
		  y_max = pos_y + 0.5*vox_size;
		  y_min = pos_y - 0.5*vox_size;
		  z_max = pos_z + 0.5*vox_size;
		  z_min = pos_z - 0.5*vox_size;

		  glBegin(GL_QUADS);
		  glColor3ub(color_r, color_g, color_b);
		  // bottom
		  glVertex3f(x_min, y_min, z_min);//v0
		  glVertex3f(x_max, y_min, z_min);//v1
		  glVertex3f(x_max, y_max, z_min);//v2
		  glVertex3f(x_min, y_max, z_min);//v3
		  //top
		  glVertex3f(x_min, y_max, z_max); //v4
		  glVertex3f(x_max, y_max, z_max); //v5
		  glVertex3f(x_max, y_min, z_max); //v6
		  glVertex3f(x_min, y_min, z_max); //v7
		  //front
		  glVertex3f(x_max, y_min, z_min); //v1
		  glVertex3f(x_max, y_max, z_min); //v2
		  glVertex3f(x_max, y_max, z_max); //v5
		  glVertex3f(x_max, y_min, z_max); //v6
		  //back
		  glVertex3f(x_min, y_min, z_min); //v0
		  glVertex3f(x_min, y_max, z_min); //v3
		  glVertex3f(x_min, y_max, z_max); //v4
		  glVertex3f(x_min, y_min, z_max); //v7
		  //left
		  glVertex3f(x_min, y_min, z_min); //v0
		  glVertex3f(x_max, y_min, z_min); //v1
		  glVertex3f(x_max, y_min, z_max); //v6
		  glVertex3f(x_min, y_min, z_max); //v7
		  //right
		  glVertex3f(x_max, y_max, z_min); //v2
		  glVertex3f(x_min, y_max, z_min); //v3
		  glVertex3f(x_min, y_max, z_max); //v4
		  glVertex3f(x_max, y_max, z_max); //v5
		  glEnd();
	  }
  }

  //glFlush();
  glutSwapBuffers();
}

void specialKeys( int key, int x, int y ) {
 
  //  Right arrow - increase rotation by 5 degree
  if (key == GLUT_KEY_RIGHT)
    rotate_y += 5;
 
  //  Left arrow - decrease rotation by 5 degree
  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;
 
  else if (key == GLUT_KEY_UP)
    rotate_x += 5;
 
  else if (key == GLUT_KEY_DOWN)
    rotate_x -= 5;
 
  //  Request display update
  glutPostRedisplay();
 
}