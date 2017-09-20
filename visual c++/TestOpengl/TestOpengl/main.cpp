#include <stdio.h>
#include <gl/glut.h>
#include <windows.h>
#include <stdlib.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


void init(void);
void display(void);
void loop(void);
int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutCreateWindow("opengl");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(loop);
	glutMainLoop();
	return 0;
}
void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}
float angle = 0.0;
void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 0.0);

	glLoadIdentity();

	glPushMatrix();

	gluLookAt(0.0, 1.0, 5.0, 0.0, 0.0, -10.0, 0.0, 1.0, 0.0);
	glPushMatrix();

	glFlush();

}
void loop(){
	Sleep(10);
	angle += 1;
	display();
}
