// ConsoleApplication1.cpp : 定義主控台應用程式的進入點。
//

////////////////////////////////////////
// Include librarys
////////////////////////////////////////
#include "stdafx.h"
#include < vector >
#include "GL/freeglut.h"
#include "jsoncons/json.hpp"
#include "trackball.h"

////////////////////////////////////////
// Global variables
////////////////////////////////////////
std::vector<GLfloat> vertex_list;
std::vector<GLubyte> color_list;
std::vector<GLuint> index_list;
int num_v = 0;//number of vertex

float cent_x = 0.0, cent_y = 0.0, cent_z = 0.0; //center of bounding box
float box_size_x = 100.0, box_size_y = 100.0, box_size_z = 100.0;//bounding box size

////////////////////////////////////////
// Name space
////////////////////////////////////////
using namespace std;
using std::string;
using jsoncons::json;

////////////////////////////////////////
// Function 
////////////////////////////////////////
void Initial_Scene(void);
void Render_Scene(void);
void Reshape(int width, int high);

void InitialCube(void);
void Loadmodel(void);
void Draw_axes(void);
void Draw_model(void);
void perspectiveDrawCoordinateReference(void);


extern struct Rect perspectiveRect;
#define COORD_X_COLOR				255,0,0
#define COORD_Y_COLOR				0,200,0
#define COORD_Z_COLOR				20,20,255
#define VIEW_COORD_REF_LEN	60		// top, front, right views (but perspective view)
#define COORD_REF_LINE_WIDTH		3.0f

void mouse(int button, int state, int x, int y)
{
	tbMouse(button, state, x, y);
}
void motion(int x, int y)
{
	tbMotion(x, y);
}
void mousewheel(int wheel, int direction, int x, int y)
{
	mouse(GLUT_MIDDLE_BUTTON, GLUT_DOWN, x, y);
	motion(x + 3.0f*direction, y);
}
void timf( int value)
{
	glutPostRedisplay();
	glutTimerFunc(1, timf, 0);
}


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

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mousewheel);
	tbInit(GLUT_LEFT_BUTTON);
	tbAnimate(GL_TRUE);
	glutTimerFunc(40, timf, 0);

	//Send to GLUT main loops
	glutMainLoop();

	return 0;
}

void Initial_Scene(void){
	// 1.background color
	glClearColor(0.75f, 0.75f, 0.75f, 0.75f);

	// 2.load model
	//InitialCube(); //a sample cube for debug
	Loadmodel();

	// 3.camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	gluLookAt(150.0f, 150.0f, 150.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(0.0f, 0.0f, box_size_x*2.4f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-cent_x, -cent_y, -cent_z);
}

void Render_Scene(void){
	// 1.Open depth_test,then clear color buffer and depth buffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	glPushMatrix();

	glTranslatef(cent_x, cent_y, cent_z);
	tbMatrix();
	glTranslatef(-cent_x, -cent_y, -cent_z);

	// 2.Draw everything
	Draw_axes();
	Draw_model();

	perspectiveDrawCoordinateReference();

	glPopMatrix();


	// 3.Swap buffers
	glutSwapBuffers();
}

void Reshape(int width, int high){
	tbReshape(width, high);

	glViewport(0, 0, width, high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(45, float(width) / high, 0.1f, 1000);
	gluPerspective(45, float(width) / high, box_size_x*0.1f, box_size_x*10.0f);

	perspectiveRect.x1 = 0;
	perspectiveRect.x2 = width;
	perspectiveRect.y1 = 0;
	perspectiveRect.y2 = high;
	perspectiveRect.w = width;
	perspectiveRect.h = high;
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

	string fname = "input/voxdata32-earth.json";
	json obj = json::parse_file(fname);

	vector<double> vox_size = obj["Voxelscale"].as<std::vector<double>>();
	int vox_num = obj["Numofvoxel"].as<int>();

	//compute bounding box
	vector<int> model_scale = obj["Modelscale"].as<std::vector<int>>();
	box_size_x = model_scale[0] * vox_size[0];//32*4.92
	box_size_y = model_scale[1] * vox_size[1];//32*4.92
	box_size_z = model_scale[2] * vox_size[2];//32*4.92
	cent_x = box_size_x / 2.0;
	cent_y = box_size_y / 2.0;
	cent_z = box_size_z / 2.0;


	// for each voxels
	for (int i = 0; i < vox_num; i++){
	//int i = 0;
		json& voxel = obj["Model"][i];
		int tag = voxel["Tag"].as<int>();

		if (tag == 0){
			vector<int> voxel_color = voxel["RGBAcolor"].as<std::vector<int>>();
			vector<int> voxel_id = voxel["Position"].as<std::vector<int>>();

			GLfloat x_max, x_min, y_max, y_min, z_max, z_min;
			x_min = voxel_id[0] * vox_size[0];
			x_max = (voxel_id[0] + 1) * vox_size[0];
			y_min = voxel_id[1] * vox_size[1];
			y_max = (voxel_id[1] + 1) * vox_size[1];
			z_min = voxel_id[2] * vox_size[2];
			z_max = (voxel_id[2] + 1) * vox_size[2];

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
			num_v =  num_v + 8;
		}
	} //end for-voxel
}

void Draw_axes(void){
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// x-axis :red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x + 200.0f, cent_y, cent_z);
	// y-axis :green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x, cent_y + 200.0f, cent_z);
	// z-axis :blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(cent_x, cent_y, cent_z);
	glVertex3f(cent_x, cent_y, cent_z + 200.0f);
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

#define COORDINATE_LENGTH 2.5f
#define UNPROJECT_DEPTH 0.6
void perspectiveDrawCoordinateReference(void)
{
	GLdouble x_axis[3], y_axis[3], z_axis[3], reference[3];
	GLdouble mv[16], p[16];
	GLint vp[4];


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	GLdouble zoom = tb_getScale();
	glScaled(1.0 / zoom, 1.0 / zoom, 1.0 / zoom);


	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, p);
	glGetIntegerv(GL_VIEWPORT, vp);

	// use view center to unproject
	GLfloat viewportCenter[2];
	viewportCenter[0] = perspectiveRect.w / 2.0;
	viewportCenter[1] = perspectiveRect.h / 2.0;
	gluUnProject(viewportCenter[0], viewportCenter[1], UNPROJECT_DEPTH, mv, p, vp,
		&reference[0], &reference[1], &reference[2]);
	gluProject(reference[0] + COORDINATE_LENGTH, reference[1], reference[2], mv, p, vp, &x_axis[0], &x_axis[1], &x_axis[2]);
	gluProject(reference[0], reference[1] + COORDINATE_LENGTH, reference[2], mv, p, vp, &y_axis[0], &y_axis[1], &y_axis[2]);
	gluProject(reference[0], reference[1], reference[2] + COORDINATE_LENGTH, mv, p, vp, &z_axis[0], &z_axis[1], &z_axis[2]);


	GLdouble coordRefOffset;	// coordinate reference offset
	coordRefOffset = 0.1* glutGet(GLUT_WINDOW_HEIGHT);


	gluUnProject(coordRefOffset + (x_axis[0] - viewportCenter[0]), coordRefOffset + (x_axis[1] - viewportCenter[1]), UNPROJECT_DEPTH,
		mv, p, vp, &x_axis[0], &x_axis[1], &x_axis[2]);
	gluUnProject(coordRefOffset + (y_axis[0] - viewportCenter[0]), coordRefOffset + (y_axis[1] - viewportCenter[1]), UNPROJECT_DEPTH,
		mv, p, vp, &y_axis[0], &y_axis[1], &y_axis[2]);
	gluUnProject(coordRefOffset + (z_axis[0] - viewportCenter[0]), coordRefOffset + (z_axis[1] - viewportCenter[1]), UNPROJECT_DEPTH,
		mv, p, vp, &z_axis[0], &z_axis[1], &z_axis[2]);

	gluUnProject(coordRefOffset, coordRefOffset, UNPROJECT_DEPTH, mv, p, vp, &reference[0], &reference[1], &reference[2]);



	glLineWidth(1.0f);
	glBegin(GL_LINES);

	glColor3ub(COORD_X_COLOR);
	glVertex3dv(reference);//origin);
	glVertex3dv(x_axis);

	glColor3ub(COORD_Y_COLOR);
	glVertex3dv(reference);//origin);
	glVertex3dv(y_axis);

	glColor3ub(COORD_Z_COLOR);
	glVertex3dv(reference);//origin);
	glVertex3dv(z_axis);

	glEnd();

	glScaled(zoom, zoom, zoom);

	glPopMatrix();

	// draw center coordinate reference
	//	glMatrixMode(GL_MODELVIEW);
	//	glPushMatrix();
	//	glTranslatef(x, y , z);
	//	glEnable(GL_SMOOTH);
/*
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(COORD_REF_LINE_WIDTH*0.7);
	glBegin(GL_LINES);

	glColor3ub(COORD_X_COLOR);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(VIEW_COORD_REF_LEN, 0.0f, 0.0f);

	glColor3ub(COORD_Y_COLOR);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, VIEW_COORD_REF_LEN, 0.0f);

	glColor3ub(COORD_Z_COLOR);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, VIEW_COORD_REF_LEN);

	glEnd();

	glDisable(GL_LINE_SMOOTH);
*/	//	glPopMatrix();

	//	glDisable(GL_SMOOTH);
}