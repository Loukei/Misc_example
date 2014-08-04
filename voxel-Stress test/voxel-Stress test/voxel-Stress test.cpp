// voxel-Stress test.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include < vector >
#include <stdlib.h>
#include <time.h>
#include "GL/freeglut.h"

////////////////////////////////////////
// Global variables
////////////////////////////////////////
std::vector<GLfloat> vertex_list;
std::vector<GLubyte> color_list;
std::vector<GLuint> index_list;
int num_v = 0;//number of vertex

double rotate_y = 0;
double rotate_x = 0;

////////////////////////////////////////
// Name space
////////////////////////////////////////
using namespace std;
using std::string;

////////////////////////////////////////
// Function 
////////////////////////////////////////
void Initial_Scene(void);
void Render_Scene(void);
void Reshape(int width, int high);
void specialKeys(int key, int x, int y);

void InitialCube(void);
void Loadmodel(void);
void Draw_axes(void);
void Draw_model(void);

////////////////////////////////////////
// Main function
////////////////////////////////////////
int main(int argc, char* argv[])
{
	//Initial GLUT window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("DEMO");

	//Initial Scenes 
	Initial_Scene();

	//Register function call
	glutDisplayFunc(Render_Scene);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(specialKeys);

	//Send to GLUT main loops
	glutMainLoop();

	return 0;
}

void Initial_Scene(void){
	// 1.background color
	glClearColor(0.75f, 0.75f, 0.75f, 0.75f);

	// 2.camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(700.0f, 700.0f, 700.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// 3.load model
	//InitialCube();
	Loadmodel();
}

void Render_Scene(void){
	// 1.Open depth_test,then clear color buffer and depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);

	// 2.Draw everything
	Draw_axes();
	Draw_model();

	glPopMatrix();
	// 3.Swap buffers
	glutSwapBuffers();
}

void Reshape(int width, int high){
	glViewport(0, 0, width, high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, float(width) / high, 0.1f, 2000);
}

void specialKeys(int key, int x, int y){
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

void InitialCube(void){
	vertex_list = {
		-50.0f, -50.0f, -50.0f,	//v0
		50.0f, -50.0f, -50.0f,	//v1
		-50.0f, 50.0f, -50.0f,	//v2
		50.0f, 50.0f, -50.0f,	//v3
		-50.0f, -50.0f, 50.0f,	//v4
		50.0f, -50.0f, 50.0f,	//v5
		-50.0f, 50.0f, 50.0f,	//v6
		50.0f, 50.0f, 50.0f,	//v7
	};

	color_list = {
		0, 0, 0, 255,
		255, 0, 0, 255,
		0, 255, 0, 255,
		255, 255, 0, 255,
		0, 0, 255, 255,
		255, 0, 255, 255,
		0, 255, 255, 255,
		255, 255, 255, 255,
	};

	index_list = {
		6, 2, 3, 7,	//front
		4, 5, 1, 0, //end
		4, 0, 2, 6, //left
		7, 3, 1, 5, //right
		4, 6, 7, 5, //top
		0, 1, 3, 2, //down
	};
}

void Loadmodel(void){
	//    v4----- v5
	//   /|      /|
	//  v6------v7|
	//  | |     | |
	//  | |v0---|-|v1
	//  |/      |/
	//  v2------v3
	float vox_size = 4.92f;
	int resolution = 128;

	srand(time(NULL));

	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			for (int k = 0; k < resolution; k++)
			{
				GLubyte voxel_color[4] = {
					rand() % 256,
					rand() % 256,
					rand() % 256,
					0,
				};

				GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
				x_min = i * vox_size;
				x_max = (i + 1)*vox_size;
				y_min = j * vox_size;
				y_max = (j + 1)*vox_size;
				z_min = k * vox_size;
				z_max = (k + 1)*vox_size;

				GLfloat voxel_vertex_list[24] = {
					x_min, y_min, z_min,	//v0
					x_max, y_min, z_min,	//v1
					x_min, y_max, z_min,	//v2
					x_max, y_max, z_min,	//v3
					x_min, y_min, z_max,	//v4
					x_max, y_min, z_max,	//v5
					x_min, y_max, z_max,	//v6
					x_max, y_max, z_max,	//v7
				};

				GLubyte voxel_color_list[32] = {
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
					voxel_color[0], voxel_color[1], voxel_color[2], voxel_color[3],
				};

				GLuint voxel_index_list[24] = {
					num_v + 6, num_v + 7, num_v + 3, num_v + 2,	//front
					num_v + 4, num_v + 0, num_v + 1, num_v + 5, //end
					num_v + 4, num_v + 6, num_v + 2, num_v + 0, //left
					num_v + 7, num_v + 5, num_v + 1, num_v + 3, //right
					num_v + 4, num_v + 5, num_v + 7, num_v + 6, //top
					num_v + 0, num_v + 2, num_v + 3, num_v + 1, //down
				};

				vertex_list.insert(vertex_list.end(), voxel_vertex_list, voxel_vertex_list + 24);
				color_list.insert(color_list.end(), voxel_color_list, voxel_color_list + 32);
				index_list.insert(index_list.end(), voxel_index_list, voxel_index_list + 24);
				num_v = num_v + 8;
				//
			}
		}
	}

}

void Draw_axes(void){
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// x-axis :red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(150.0f, 0.0f, 0.0f);
	// y-axis :green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 150.0f, 0.0f);
	// z-axis :blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 150.0f);
	glEnd();
}

void Draw_model(void){
	glFrontFace(GL_CCW);

	glEnable(GL_BACK);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &vertex_list[0]);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &color_list[0]);
	// each face have 4 calls for glvertex
	glDrawElements(GL_QUADS, index_list.size(), GL_UNSIGNED_INT, &index_list[0]);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_BACK);
}