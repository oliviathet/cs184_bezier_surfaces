#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <bitset>
#include <algorithm>


#include "Eigen/Geometry"

#include "CurveLocalGeometry.h"
#include "Camera.h"
#include "DifferentialGeometry.h"
#include "Triangle.h"
#include "BezierPatch.h"

inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
	int w, h; // width and height

};



//****************************************************
// Global Variables
//****************************************************
Camera camera;
Viewport viewport;
string filename;
string subdivisionMethod;
float subdivisionParameter;
int numberOfBezierPatches;
std::vector<BezierPatch> listOfBezierPatches;

std::vector<Eigen::Vector3f> objFileVertices;
std::vector<std::vector<DifferentialGeometry> > objFilePolygonList;
bool objMode;
string objFilenameOutput;
bool WRITE_OBJ;

// ***** Display-related global variables ***** //

// if false, then in flat shading mode
bool SMOOTH_SHADING;

// if false, then in filled mode
bool WIREFRAME_MODE;

bool HIDDEN_LINE_MODE;

bool debug;



//****************************************************
// Simple init function
//****************************************************
void initScene(){

	// Hard code various diffuse and specular constants
	// NOTE: Probably should change this, I copied it from online...

	SMOOTH_SHADING = true;
	WIREFRAME_MODE = true;
	HIDDEN_LINE_MODE = false;

	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color(0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	//Add directed light
	GLfloat lightColor1[] = {0.6f, 0.5f, 0.4f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = {-10.0f, 5.5f, 8.5f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	//Add positioned light
	GLfloat lightColor0[] = {0.6f, 0.55f, 0.55f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport (0,0,viewport.w,viewport.h);

	float aspect_ratio = ((float) viewport.w) / ((float) viewport.h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(camera.FIELD_OF_VIEW * camera.ZOOM_AMOUNT, aspect_ratio, camera.zNear, camera.zFar);

}




//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

	// clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	// make sure transformation is "zero'd"
	glLoadIdentity();

	float aspect_ratio = ((float) viewport.w) / ((float) viewport.h);

	// set OpenGL viewport
	glViewport(0, 0, viewport.w, viewport.h);

	gluPerspective(camera.FIELD_OF_VIEW * camera.ZOOM_AMOUNT, aspect_ratio, camera.zNear, camera.zFar);

	// set shading of model to smooth or flat, based on our global variable
	if (SMOOTH_SHADING) {
		glShadeModel(GL_SMOOTH);
	} else {
		glShadeModel(GL_FLAT);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set camera, via the following:
	// void gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY,
	//                GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
	gluLookAt(camera.position.x(), camera.position.y(), camera.position.z(), camera.lookAt.x(), camera.lookAt.y(),
			camera.lookAt.z(), camera.up.x(), camera.up.y(), camera.up.z());

	// Handle rotations
	glRotatef(camera.X_ROTATION_AMOUNT, 1, 0, 0);
	glRotatef(camera.Y_ROTATION_AMOUNT, 0, 1, 0);
	glRotatef(camera.Z_ROTATION_AMOUNT, 0, 0, 1);

	// Handle translations
	glTranslatef(camera.X_TRANSLATION_AMOUNT, camera.Y_TRANSLATION_AMOUNT, camera.Z_TRANSLATION_AMOUNT);

	if (objMode) {
		for (std::vector<std::vector<DifferentialGeometry> >::size_type j = 0; j < objFilePolygonList.size(); j++) {
			std::vector<DifferentialGeometry> currentPolygonToDraw = objFilePolygonList[j];

			if (WIREFRAME_MODE) {
				if (HIDDEN_LINE_MODE) {
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

					glDisable(GL_LIGHTING);
					glClearColor(0.0, 0.0, 0.0, 0.0);
					// Default the drawing color to white
					glColor3f(1.0f, 1.0f, 1.0f);

					glBegin(GL_POLYGON);

					for (std::vector<DifferentialGeometry>::size_type k = 0; k < currentPolygonToDraw.size(); k++) {
						glVertex3f(currentPolygonToDraw[k].position.x(),
								currentPolygonToDraw[k].position.y(),
								currentPolygonToDraw[k].position.z());
					}

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(1.0, 1.0);
					glClearColor(0.0, 0.0, 0.0, 0.0);
					glColor3f(0.0, 0.0, 0.0);

					glBegin(GL_POLYGON);
					for (std::vector<DifferentialGeometry>::size_type k = 0; k < currentPolygonToDraw.size(); k++) {
						glVertex3f(currentPolygonToDraw[k].position.x(),
								currentPolygonToDraw[k].position.y(),
								currentPolygonToDraw[k].position.z());
					}
					glEnd();
					glDisable(GL_POLYGON_OFFSET_FILL);


				} else {
					// Draw objects in wireframe mode
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

					glDisable(GL_LIGHTING);
					glClearColor(0.0, 0.0, 0.0, 0.0);
					// Default the drawing color to white
					glColor3f(1.0f, 1.0f, 1.0f);

					glBegin(GL_POLYGON);

					for (std::vector<DifferentialGeometry>::size_type k = 0; k < currentPolygonToDraw.size(); k++) {
						glVertex3f(currentPolygonToDraw[k].position.x(),
								currentPolygonToDraw[k].position.y(),
								currentPolygonToDraw[k].position.z());
					}

					glEnd();
				}


			} else {
				// Draw objects in filled mode
				glPolygonMode( GL_FRONT, GL_FILL);
				glPolygonMode( GL_BACK, GL_FILL);
				glClearColor(0.0, 0.0, 0.0, 0.0);
				glEnable(GL_LIGHTING);

				glBegin(GL_POLYGON);

				// TODO: Account for normals in non-wireframe mode
				for (std::vector<DifferentialGeometry>::size_type k = 0; k < currentPolygonToDraw.size(); k++) {
					glVertex3f(currentPolygonToDraw[k].position.x(),
							currentPolygonToDraw[k].position.y(),
							currentPolygonToDraw[k].position.z());
				}


				glEnd();
			}
		}


	} else {

		/*
		Begin drawing all of the triangles
		PSUEDOCODE:

		for each BezierPatch in the scene's list of Bezier patches:
			for each Triangle in the current Bezier patch's list of triangles
				Grab the three vertices of the triangle and render the triangle

		 */

		// Iterate through each of our BezierPatches...
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			BezierPatch currentBezierPatch = listOfBezierPatches[i];
			for (std::vector<Triangle>::size_type j = 0; j < currentBezierPatch.listOfTriangles.size(); j++) {
				Triangle currentTriangleToDraw = currentBezierPatch.listOfTriangles[j];

				DifferentialGeometry point1, point2, point3;
				point1 = currentTriangleToDraw.point1;
				point2 = currentTriangleToDraw.point2;
				point3 = currentTriangleToDraw.point3;

				if (WIREFRAME_MODE) {
					if (HIDDEN_LINE_MODE) {
						glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

						glDisable(GL_LIGHTING);
						glClearColor(0.0, 0.0, 0.0, 0.0);
						// Default the drawing color to white
						glColor3f(1.0f, 1.0f, 1.0f);

						glBegin(GL_POLYGON);

						// Set vertex and normals of all three points of the current triangle
						glNormal3f(point1.normal.x(), point1.normal.y(), point1.normal.z());
						glVertex3f(point1.position.x(), point1.position.y(), point1.position.z());
						glNormal3f(point2.normal.x(), point2.normal.y(), point2.normal.z());
						glVertex3f(point2.position.x(), point2.position.y(), point2.position.z());
						glNormal3f(point3.normal.x(), point3.normal.y(), point3.normal.z());
						glVertex3f(point3.position.x(), point3.position.y(), point3.position.z());

						glEnd();

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset(1.0, 1.0);
						glClearColor(0.0, 0.0, 0.0, 0.0);
						glColor3f(0.0, 0.0, 0.0);

						glBegin(GL_POLYGON);
						glNormal3f(point1.normal.x(), point1.normal.y(), point1.normal.z());
						glVertex3f(point1.position.x(), point1.position.y(), point1.position.z());
						glNormal3f(point2.normal.x(), point2.normal.y(), point2.normal.z());
						glVertex3f(point2.position.x(), point2.position.y(), point2.position.z());
						glNormal3f(point3.normal.x(), point3.normal.y(), point3.normal.z());
						glVertex3f(point3.position.x(), point3.position.y(), point3.position.z());

						glEnd();
						glDisable(GL_POLYGON_OFFSET_FILL);

					} else {
						// Draw objects in wireframe mode
						glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

						glDisable(GL_LIGHTING);
						glClearColor(0.0, 0.0, 0.0, 0.0);
						// Default the drawing color to white
						glColor3f(1.0f, 1.0f, 1.0f);

						glBegin(GL_POLYGON);

						// Set vertex and normals of all three points of the current triangle
						glNormal3f(point1.normal.x(), point1.normal.y(), point1.normal.z());
						glVertex3f(point1.position.x(), point1.position.y(), point1.position.z());
						glNormal3f(point2.normal.x(), point2.normal.y(), point2.normal.z());
						glVertex3f(point2.position.x(), point2.position.y(), point2.position.z());
						glNormal3f(point3.normal.x(), point3.normal.y(), point3.normal.z());
						glVertex3f(point3.position.x(), point3.position.y(), point3.position.z());

						glEnd();
					}


				} else {
					// Draw objects in filled mode
					glPolygonMode( GL_FRONT, GL_FILL);
					glPolygonMode( GL_BACK, GL_FILL);
					glClearColor(0.0, 0.0, 0.0, 0.0);
					glEnable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					// Set vertex and normals of all three points of the current triangle
					glNormal3f(point1.normal.x(), point1.normal.y(), point1.normal.z());
					glVertex3f(point1.position.x(), point1.position.y(), point1.position.z());
					glNormal3f(point2.normal.x(), point2.normal.y(), point2.normal.z());
					glVertex3f(point2.position.x(), point2.position.y(), point2.position.z());
					glNormal3f(point3.normal.x(), point3.normal.y(), point3.normal.z());
					glVertex3f(point3.position.x(), point3.position.y(), point3.position.z());

					glEnd();
				}
			}
		}
	}


	glPopMatrix();

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// function that assists with regular key presses
//***************************************************
void keyPressed( unsigned char key, int x, int y )
{
	switch ( key )
	{
	// Space bar: exit program
	case ' ':
		exit(1);

	case 's':
		// Toggle between flat and smooth shading, but only if we aren't in wireframe mode
		if (!WIREFRAME_MODE) {
			SMOOTH_SHADING = !SMOOTH_SHADING;
			if (debug) {
				if (SMOOTH_SHADING) {
					cout << "Turned smooth shading ON.\n";
				} else {
					cout << "Turned smooth shading OFF.\n";
				}
			}
		}
		break;

	case 'w':
		// Toggle between wireframe mode and filled mode
		WIREFRAME_MODE = !WIREFRAME_MODE;
		if (debug) {
			if (WIREFRAME_MODE) {
				cout << "Turned wireframe mode ON.\n";
			} else {
				cout << "Turned wireframe mode OFF.\n";
			}
		}
		break;

	case 'h':
		// Toggle between filled and hidden-line mode
		HIDDEN_LINE_MODE = !HIDDEN_LINE_MODE;
		if (debug) {
			if (HIDDEN_LINE_MODE) {
				cout << "Turned hidden line mode ON.\n";
			} else {
				cout << "Turned hidden line mode OFF.\n";
			}
		}
		break;

	case '+':
		// Zoom in
		camera.zoomIn();
		break;

	case '-':
		// Zoom out
		camera.zoomOut();
		break;

	case 'r':
		// Reset everything
		SMOOTH_SHADING = true;
		WIREFRAME_MODE = true;
		camera.resetCamera();
		break;

	case '.':
		// Rotate Z clockwise, looking from above
		camera.rotateZDown();
		break;

	case ',':
		// Rotate Z counterclockwise, looking from above
		camera.rotateZUp();
		break;
	}

}


//****************************************************
// function that assists with special (i.e. arrow key) key presses
//***************************************************
void handleSpecialKeypress(int key, int x, int y) {
	int mod_key = glutGetModifiers();
	switch (key) {
		case GLUT_KEY_LEFT:
			if (mod_key == GLUT_ACTIVE_SHIFT) {
				camera.translateLeft();
			} else {
				camera.rotateDown();
			}
			break;

		case GLUT_KEY_RIGHT:
			if (mod_key == GLUT_ACTIVE_SHIFT) {
				camera.translateRight();
			} else {
				camera.rotateUp();
			}
			break;

		case GLUT_KEY_UP:
			if (mod_key == GLUT_ACTIVE_SHIFT) {
				camera.translateUp();
			} else if (mod_key == GLUT_ACTIVE_ALT) {
				camera.translateZUp();
			} else {
				camera.rotateLeft();
			}
			break;

		case GLUT_KEY_DOWN:
			if (mod_key == GLUT_ACTIVE_SHIFT) {
				camera.translateDown();
			} else if (mod_key == GLUT_ACTIVE_ALT) {
				camera.translateZDown();
			} else {
				camera.rotateRight();
			}
			break;
		case GLUT_KEY_PAGE_UP:
			camera.translateZUp();
			break;
		case GLUT_KEY_PAGE_DOWN:
			camera.translateZDown();
			break;

	}
	glutPostRedisplay();

}


//****************************************************
// function that prints all of our command line option variables
//***************************************************
void printCommandLineOptionVariables( )
{
	if (debug)
	{
		cout << "\nBezier file: " << filename << "\n";
		cout << "Subdivision Parameter: " << subdivisionParameter << "\n";
		cout << "Subdivision Method: " << subdivisionMethod << "\n\n";

		cout << "We currently have " << listOfBezierPatches.size() << " Bezier patches.\n\n";
		// Iterate through Bezier Patches
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			cout << "  Bezier patch " << (i + 1) << ":\n\n";
			std::vector<std::vector <Eigen::Vector3f> > curves = listOfBezierPatches[i].listOfCurves;

			// Iterate through curves in each Bezier patch
			for (std::vector<std::vector <Eigen::Vector3f> >::size_type j = 0; j < curves.size(); j++) {
				std::vector<Eigen::Vector3f> listOfPointsForCurrentCurve = curves[j];

				cout << "    Curve " << (j + 1) << ":\n";

				// Iterate through points in current curve and print them
				for (std::vector<Eigen::Vector3f>::size_type k = 0; k < listOfPointsForCurrentCurve.size(); k++) {
					printf("    (%f, %f, %f)\n", listOfPointsForCurrentCurve[k].x(), listOfPointsForCurrentCurve[k].y(), listOfPointsForCurrentCurve[k].z());
				}
				cout << "\n\n";
			}
		}

		cout << "Display options:\n";
		if (SMOOTH_SHADING) {
			cout << "  Smooth shading is ON.\n";
		} else {
			cout << "  Flat shading is ON. (i.e. smooth shading is OFF)\n";
		}

		if (WIREFRAME_MODE) {
			cout << "  Wireframe mode is ON.\n";
		} else {
			cout << "  Filled mode is ON. (i.e. Wireframe mode is OFF)\n";
		}

	}
}


//****************************************************
// function that prints all triangles in every Bezier Patch
//***************************************************
void printTrianglesInBezierPatches() {
	if (debug) {
		// Iterate through Bezier Patches
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			cout << "  Bezier patch " << (i + 1) << ":\n\n";

			// Iterate through Triangles in the current Bezier patch
			for (std::vector<Triangle>::size_type j = 0; j < listOfBezierPatches[i].listOfTriangles.size(); j++) {
				Triangle currentTriangle = listOfBezierPatches[i].listOfTriangles[j];
				cout << "    Triangle " << (j + 1) << ":\n";
				cout << "      " << currentTriangle.printTriangleInformation();
			}

		}
	}
}


//****************************************************
// function that prints all differential geometries in every Bezier Patch
//***************************************************
void printDifferentialGeometriesInBezierPatches() {
	if (debug) {
		// Iterate through Bezier Patches
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			cout << "  Bezier patch " << (i + 1) << ":\n\n";

			// Iterate through Triangles in the current Bezier patch
			for (std::vector<DifferentialGeometry>::size_type j = 0; j < listOfBezierPatches[i].listOfDifferentialGeometries.size(); j++) {
				DifferentialGeometry currentDifferentialGeometry = listOfBezierPatches[i].listOfDifferentialGeometries[j];
				cout << "    DifferentialGeometry " << (j + 1) << ":\n";
				Eigen::Vector3f currentPosition = currentDifferentialGeometry.position;
				cout << "      (" << currentPosition.x() << " , " << currentPosition.y() << " , " << currentPosition.z() << ")\n";
			}

		}
	}
}

//****************************************************
// function that prints the number of triangles and differential geometries for each Bezier patch
//***************************************************
void printStatistics() {
	if (debug) {
		cout << "\n  Statistics:\n\n";
		// Iterate through Bezier Patches
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			cout << "    Bezier patch " << (i + 1) << " has " << listOfBezierPatches[i].listOfDifferentialGeometries.size()
					<< " differential geometries and " << listOfBezierPatches[i].listOfTriangles.size() << " triangles.\n";
		}
	}
}



//****************************************************
// function that prints the number of triangles and differential geometries for each Bezier patch
//***************************************************
void printCameraInformation() {
	if (debug) {
		cout << "\n  Camera information:\n\n";
		cout << "Position:\n" << camera.position << "\n\n";
		cout << "Up:\n" << camera.up << "\n\n";
		cout << "Look At:\n" << camera.lookAt << "\n\n";
		cout << "Z-Near: " << camera.zNear << "; Z-Far: " << camera.zFar << "\n";
		cout << "Field of view: " << camera.FIELD_OF_VIEW << "\n";
		cout << "Zoom amount: " << camera.ZOOM_AMOUNT << "\n";
	}
}


//****************************************************
// Method that populates each BezierPatch's list of DifferentialGeometries
// and list of Triangles, based on what kind of subdivision (i.e. adaptive or uniform)
// we are performing
//***************************************************
void perform_subdivision(bool adaptive_subdivision) {
	// Iterate through each of the Bezier patches...
	for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
		if (adaptive_subdivision) {
			listOfBezierPatches[i].performAdaptiveSubdivision(subdivisionParameter);
		} else {
			listOfBezierPatches[i].performUniformSubdivision(subdivisionParameter);

		}
	}

}




//****************************************************
// Writes an .obj file that represents this BezierPatch
//***************************************************
void generateObjFile(std::string filename) {
	std::ofstream myfile;
	myfile.open(filename);

	std::vector<Triangle> aggregateTriangleList;
	for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
		for (std::vector<Triangle>::size_type j = 0; j < listOfBezierPatches[i].listOfTriangles.size(); j++) {
			aggregateTriangleList.push_back(listOfBezierPatches[i].listOfTriangles[j]);
		}
	}

	// Generate all vertex lines in file
	for (std::vector<Triangle>::size_type i = 0; i < aggregateTriangleList.size(); i++) {
		DifferentialGeometry point1 = aggregateTriangleList[i].point1;
		DifferentialGeometry point2 = aggregateTriangleList[i].point2;
		DifferentialGeometry point3 = aggregateTriangleList[i].point3;

		myfile << "v " << point1.position.x() << " " << point1.position.y() << " " << point1.position.z() << "\n";
		myfile << "v " << point2.position.x() << " " << point2.position.y() << " " << point2.position.z() << "\n";
		myfile << "v " << point3.position.x() << " " << point3.position.y() << " " << point3.position.z() << "\n";
	}


	for (std::vector<Triangle>::size_type i = 0; i < aggregateTriangleList.size(); i++) {
		myfile << "f " << (3 * i) + 1 << " " << (3 * i) + 2 << " " << (3 * i) + 3 << "\n";
	}
}



//****************************************************
// function that parses an input .bez file and initializes
// a list of Bezier patches
//
// psuedocode for parsing .bez file

/*

BezierPatch currentBezierPatch
for each collection of 4 rows that correspond to one Bezier patch:
    for i between 1 and 4 (ie. each of the 4 rows for the current patch):
        Vect3 point1 = first row
        Vect3 point2 = second row
        Vect3 point3 = third row
        Vect3 point4 = fourth row
        vector<Vect3> curveI;
        curveI.push_back(point1);
        curveI.push_back(point2);
        curveI.push_back(point3);
        curveI.push_back(point4);
        currentBezierPatch.addCurve(curveI);
    all_bezier_patches.addPatch(currentBezierPatch);

*/
//****************************************************
void parseBezierFile(string filename) {

	ifstream file(filename);
	// current line
	string str;

	// current word that we're parsing on a line
	string currentWord;

	// index of the line
	int i = 0;

	// index of a word on each specific line
	int j = 0;

	// coordinates for each set of four points (i.e. one line)
	float p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, p4x, p4y, p4z;

	// number of lines that have already been processed for the current Bezier patch
	int curvesParsedForCurrentPatch = 0;

	BezierPatch currentBezierPatch;

	bool readyForNextPatch = false;

	while (getline(file, str)) {
		// If we encounter a new line, then we know that the next 4 consecutive lines represent
		// 4 curves that will make up a Bezier patch, so we reset our current Bezier patch
		if (str.length() == 0) {
			curvesParsedForCurrentPatch = 0;
			currentBezierPatch = BezierPatch();
			i++;
			continue;
		}

		if (readyForNextPatch) {
			readyForNextPatch = false;
			curvesParsedForCurrentPatch = 0;
			currentBezierPatch = BezierPatch();
		}

		j = 0;
		if (i == 0) {
			numberOfBezierPatches = stoi(str);
			i++;
			continue;
		}

		istringstream iss(str);
		// Iterate through words on a single line...
		while (iss >> currentWord) {
			if (j == 0) { p1x = stof(currentWord); }
			if (j == 1) { p1y = stof(currentWord); }
			if (j == 2) { p1z = stof(currentWord); }
			if (j == 3) { p2x = stof(currentWord); }
			if (j == 4) { p2y = stof(currentWord); }
			if (j == 5) { p2z = stof(currentWord); }
			if (j == 6) { p3x = stof(currentWord); }
			if (j == 7) { p3y = stof(currentWord); }
			if (j == 8) { p3z = stof(currentWord); }
			if (j == 9) { p4x = stof(currentWord); }
			if (j == 10) { p4y = stof(currentWord); }
			if (j == 11) { p4z = stof(currentWord); }
			j++;
		}

		Eigen::Vector3f point1(p1x, p1y, p1z);
		Eigen::Vector3f point2(p2x, p2y, p2z);
		Eigen::Vector3f point3(p3x, p3y, p3z);
		Eigen::Vector3f point4(p4x, p4y, p4z);

		// 1 of the 4 curves for the currentBezierPatch
		vector<Eigen::Vector3f> currentCurve;
		currentCurve.push_back(point1);
		currentCurve.push_back(point2);
		currentCurve.push_back(point3);
		currentCurve.push_back(point4);

		currentBezierPatch.addCurve(currentCurve);
		curvesParsedForCurrentPatch++;

		// We have parsed all four curves for our current patch
		if (curvesParsedForCurrentPatch == 4) {

			listOfBezierPatches.push_back(currentBezierPatch);
			readyForNextPatch = true;
		}

		i++;
	}

	// Perform subdivision of BezierPatches, based on whether we want to adaptively or uniformly subdivide
	if (subdivisionMethod == "ADAPTIVE") {
		perform_subdivision(true);
	} else if (subdivisionMethod == "UNIFORM") {
		perform_subdivision(false);
	} else {
		cout << "Invalid subdivision method, terminating program.";
		exit(1);
	}

	// We want to write our Bezier patches to an .obj file
	if (WRITE_OBJ) {
		generateObjFile(objFilenameOutput);
	}

}

//****************************************************
// Parsing .OBJ file specified in scene file
//****************************************************
void parseObjFile(string filename) {

	string str;
	ifstream file(filename);

	// The identifier that we're currently parsing
	string currentlyParsing;

	// current word that we're parsing on a line
	string currentWord;

	bool validLine = true;

	while (getline(file, str)) {
		// str represents the current line of the file

		validLine = true;
		int i = 0;

		std::vector<DifferentialGeometry> currentPolygonPoints;

		istringstream iss(str);
		while (iss >> currentWord) {

			// ********** Figure out what the first word of each line is ********** //
			// We currently support:
			// (1) v ... (vertex definitions)
			// (2) f ... (face definitions)

			if ((i == 0) && (currentWord == "v")) {
				currentlyParsing = currentWord;

			} else if ((i == 0) && (currentWord == "f")) {
				currentlyParsing = currentWord;

			} else if (i == 0) {
				currentlyParsing = currentWord;
				validLine = false;
			}

			// If the current line is not valid, then just keep skipping every word in the line
			if (!validLine) {
				i++;
				continue;
			}

			// ********** After we've figured out the first word in each line, parse the rest of the line ********** //
			// If we've hit here, then we're NOT on the first word of the line anymore

			if (currentlyParsing == "v") {
				float xCoor, yCoor, zCoor;
				if (i == 0) { }
				else if (i == 1) { xCoor = stof(currentWord); }
				else if (i == 2) { yCoor = stof(currentWord); }
				else if (i == 3) { zCoor = stof(currentWord); }
				else if (i > 3) {
					cerr << "Extra parameters for " << currentlyParsing << ". Ignoring them.\n";
				}
				if (i == 3) {
					objFileVertices.push_back(Eigen::Vector3f(xCoor, yCoor, zCoor));
				}

			} else if (currentlyParsing == "f") {


				if (i == 0) {
					i++;
					continue;
				}
				int currentIndexOfVertex = stoi(currentWord) - 1;

				if (currentIndexOfVertex < objFileVertices.size()) {
					currentPolygonPoints.push_back(DifferentialGeometry(objFileVertices[currentIndexOfVertex]));
				}

			}

			i++;
		}

		// Finished parsing current line
		if (currentlyParsing == "f") {
			objFilePolygonList.push_back(currentPolygonPoints);
		}
	}
}
//****************************************************
// function that determines if full string ends with ending
//***************************************************

static bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

//****************************************************
// function that parses command line options,
// given number of command line arguments (argc)
// and the argument array (argv)
// Format:
// % as3 inputfile.bez 0.1 -a
//***************************************************
void parseCommandLineOptions(int argc, char *argv[])
{
	subdivisionMethod = "UNIFORM";
	string flag;

	int i = 1;
	while (i <= argc - 1) {
		flag = argv[i];

		if (i == 1) {
			filename = flag;
			if (hasEnding(flag, ".bez")) {
				objMode = false;
			} else if (hasEnding(flag, ".obj")) {
				objMode = true;
			} else {
				std::cout << "Unrecognized input file format.";
				exit(1);
			}
		} else if (i == 2) {
			subdivisionParameter = stof(flag);
		} else if (flag == "-o") {
			if ((i + 1) > (argc - 1))
			{
				std::cout << "Invalid number of parameters for -o.";
				exit(1);
			}
			if (!objMode) {
				WRITE_OBJ = true;
				objFilenameOutput = argv[i+1];
			} else {
				std::cout << "Error: cannot write to .obj file if in .obj mode.";
				exit(1);
			}
			i += 1;
		}

		if (i == 3 && flag == "-a") {
			subdivisionMethod = "ADAPTIVE";
		}

		i++;
	}

	if (hasEnding(filename, ".bez")) {
		parseBezierFile(filename);
	} else if (hasEnding(filename, ".obj")) {
		parseObjFile(filename);
	}
}



//****************************************************
// Initializes the camera's vector instance variables,
// based on each BezierPatch's DifferentialGeometry objects
//
// NOTE: This method MUST be called AFTER all Bezier objects and their
//       DifferentialGeometry / Triangle lists are finished being initialized
//****************************************************
void initializeCamera() {
	// First, we iterate through all of the objects in our scene and we determine the minimum and maximum x, y, z values over all objects

	float xMin, xMax, yMin, yMax, zMin, zMax;

	xMin = yMin = zMin = numeric_limits<int>::max();
	xMax = yMax = zMax = numeric_limits<int>::min();


	if (objMode) {
		for (std::vector<std::vector<DifferentialGeometry> >::size_type i = 0; i < objFilePolygonList.size(); i++) {
			std::vector<DifferentialGeometry> currentPolygon = objFilePolygonList[i];

			for (std::vector<DifferentialGeometry>::size_type j = 0; j < currentPolygon.size(); j++) {
				Eigen::Vector3f currentDifferentialGeometryPosition = currentPolygon[j].position;

				// Update min's, if applicable
				if (currentDifferentialGeometryPosition.x() < xMin) {
					xMin = currentDifferentialGeometryPosition.x();
				}
				if (currentDifferentialGeometryPosition.y() < yMin) {
					yMin = currentDifferentialGeometryPosition.y();
				}
				if (currentDifferentialGeometryPosition.z() < zMin) {
					zMin = currentDifferentialGeometryPosition.z();
				}

				// Update max's, if applicable
				if (currentDifferentialGeometryPosition.x() > xMax) {
					xMax = currentDifferentialGeometryPosition.x();
				}
				if (currentDifferentialGeometryPosition.y() > yMax) {
					yMax = currentDifferentialGeometryPosition.y();
				}
				if (currentDifferentialGeometryPosition.z() > zMax) {
					zMax = currentDifferentialGeometryPosition.z();
				}
			}
		}

	} else {

		// Iterate through all BezierPatches...
		for (std::vector<BezierPatch>::size_type i = 0; i < listOfBezierPatches.size(); i++) {
			BezierPatch currentBezierPatch = listOfBezierPatches[i];

			// Iterate through each BezierPatch's DifferentialGeometries...
			for (std::vector<DifferentialGeometry>::size_type j = 0; j < currentBezierPatch.listOfDifferentialGeometries.size(); j++) {
				Eigen::Vector3f currentDifferentialGeometryPosition = currentBezierPatch.listOfDifferentialGeometries[j].position;

				// Update min's, if applicable
				if (currentDifferentialGeometryPosition.x() < xMin) {
					xMin = currentDifferentialGeometryPosition.x();
				}
				if (currentDifferentialGeometryPosition.y() < yMin) {
					yMin = currentDifferentialGeometryPosition.y();
				}
				if (currentDifferentialGeometryPosition.z() < zMin) {
					zMin = currentDifferentialGeometryPosition.z();
				}

				// Update max's, if applicable
				if (currentDifferentialGeometryPosition.x() > xMax) {
					xMax = currentDifferentialGeometryPosition.x();
				}
				if (currentDifferentialGeometryPosition.y() > yMax) {
					yMax = currentDifferentialGeometryPosition.y();
				}
				if (currentDifferentialGeometryPosition.z() > zMax) {
					zMax = currentDifferentialGeometryPosition.z();
				}

			}
		}
	}

	// At this point, xMin, xMax, yMin, yMax, zMin, zMax are initialized, and form a box that has dimensions
	// (xMax - xMin)  x  (yMax - yMin)  x  (zMax - zMin)
	float xLength = xMax - xMin;
	float yLength = yMax - yMin;
	float zLength = zMax - zMin;
	float largestLength = fmax(zLength, fmax(xLength, yLength));

	// First, we find the CENTER of our x/y/z min/max values, and this becomes our camera's lookAt vector
	Eigen::Vector3f center((xMin + xMax) / 2.0f, (yMin + yMax) / 2.0f, (zMin + zMax) / 2.0f);
	camera.lookAt = center;

	// Set the camera's position to (x, y) = (0, 0). The z-coordinate is the length of the largest coordinate
	// between xLength, yLength, zLength as defined above, so that the object will always be visible in the scope of the camera's lens
	camera.position = Eigen::Vector3f(0.0, 0.0, 2.0f * largestLength);

	// Hardcode camera up vector to (0, 1, 0)
	camera.up = Eigen::Vector3f(0, 1.0f, 0);

	camera.zNear = 1.0f;

	camera.zFar = camera.zNear + (10.0f * largestLength);
}




//****************************************************
// psuedocode for... everything
//****************************************************

/*

- Parse command line arguments to get subdivision parameter and subdivision method (adaptive vs. uniform)

- Use parsed command line arguments to initialize a list of all Bezier patches (with psuedocode above)

- Iterate through list of patches and subdivide all of the patches in the list, either adapatively or uniformly

  - for uniform subdivision:
    - take given u | v step size, initialize a list of LocalGeometry objects, starting from (u, v) = (0, 0) and moving
      up by u = v = stepsize each time. NOTE: this list of LocalGeometry objects is specific for the current patch.
      That is, the list of LocalGeometry objects will be given by currentPatch.local_geo_list
    - take list of LocalGeometry objects and generate a list of triangles with it. NOTE: this list of triangles is specific
      for the current patch. That is, it is given by currentPatch.triangle_list

  - for adaptive subdivision:
    - split based on lecture slides
      (http://www.cs.berkeley.edu/~job/Classes/CS184/Spring-2015-Slides/14-Surfaces.pdf)

- At this point, each of the patches (ie. surfaces) has its own list of LocalGeometry objects and its own list of triangles

- Create an overarching list of LocalGeometry objects and an overarching list of Triangle objects, and aggregate
  all LocalGeometry and Triangle objects, respectively, into these two overarching lists

- Define myDisplay():
  - Set the gluLookAt, and other OpenGL variables (?)
  - for each BezierPatch in the Scene's BezierPatch list:
    - for each Triangle in the current BezierPatch's triangle list (ie. currentPatch.triangle_list):
      - set OpenGL Vertex3f/Normal3f and other things to draw the current triangle
      - NOTE: need to change this slightly when wireframe mode/other modes are active/non-active

- Call glutDisplayFunc(myDisplay)

 */


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

	// Turns debug mode ON or OFF
	debug = true;
	WRITE_OBJ = false;

	// This initializes glut
	glutInit(&argc, argv);

	// Parse command line options
	parseCommandLineOptions(argc, argv);
	printCommandLineOptionVariables();

	// At this point, all subdivision of Bezier Patches has been completed

	printStatistics();

//	printDifferentialGeometriesInBezierPatches();
//	printTrianglesInBezierPatches();

	// Initialize position, lookAt, and up vectors of camera so that we may feed them into OpenGL rendering system later
	initializeCamera();

	//This tells glut to use a double-buffered window with red, green, and blue channels
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	// Initalize theviewport size
	viewport.w = 1000;
	viewport.h = 1000;

	printCameraInformation();

	//The size and position of the window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();							// quick function to set up scene

	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutIdleFunc(myDisplay);

	// Handles key presses
	glutKeyboardFunc( keyPressed );
	glutSpecialFunc( handleSpecialKeypress );

	glutMainLoop();							// infinite loop that will keep drawing and resizing
	// and whatever else

	return 0;
}
