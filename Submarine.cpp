#include<bits/stdc++.h>
#include <GL/glut.h>
#define width 800.0
#define height 450.0
#define FPS 60.0
using namespace std;
double subx=0, suby=0, subz=0;
double speed = 0.05;
double subyaw = 0;
double pitch = -30, yaw = 0;
double cam_dis = 30;
bool third_person_view=1;
struct Motions {
	bool left, right, front, back, up, down,rotate_left,rotate_right;
};
Motions motions = { 0,0,0,0,0,0,0,0 };
void init() {
	glutSetCursor(GLUT_CURSOR_NONE);
	glMatrixMode(GL_PROJECTION);//�]�w��v�x�}
	glLoadIdentity();//���m�����x�}
	gluPerspective(60, float(width) / height, 0, 100);//����,���e��,�̪����,�̻�
	glEnable(GL_DEPTH_TEST);//�`�ױ���
	
}

void timer(int){
    glutPostRedisplay();
    glutWarpPointer(width / 2, height / 2);
    glutTimerFunc(1000 / FPS, timer, 0);
}
void draw_cube() {
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glRotatef( -yaw - subyaw, 0, 1, 0);
	glColor3f(1, 0, 0);
	{
		glBegin(GL_QUADS);
		glVertex3d(-1, -1, -1);
		glVertex3d(+1, -1, -1);
		glVertex3d(+1, -1, +1);
		glVertex3d(-1, -1, +1);


		glVertex3d(-1, -1, -1);
		glVertex3d(-1, +1, -1);
		glVertex3d(-1, +1, +1);
		glVertex3d(-1, -1, +1);

		glVertex3d(-1, -1, +1);
		glVertex3d(-1, +1, +1);
		glVertex3d(+1, +1, +1);
		glVertex3d(+1, -1, +1);


		glVertex3d(-1, -1, -1);
		glVertex3d(-1, +1, -1);
		glVertex3d(+1, +1, -1);
		glVertex3d(+1, -1, -1);
		glVertex3d(-1, +1, -1);
		glVertex3d(+1, +1, -1);
		glVertex3d(+1, +1, +1);
		glVertex3d(-1, +1, +1);
		glEnd();
	}
	{
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	//glVertex3d(-1, -1, -1);
	//glVertex3d(-1, +1, -1);
	glVertex3d(+1, -1, -1);
	glVertex3d(+1, +1, -1);
	glVertex3d(+1,  1,  1);
	glVertex3d(+1, +1, -1);
	glVertex3d(+1, -1, -1);
	glVertex3d(+1, -1,  1);
	glVertex3d(+1, -1, +1);
	glVertex3d(+1, +1, +1);
	//glVertex3d(-1, -1, +1);
	//glVertex3d(-1, +1, +1);
	glVertex3d(2, 0, 0);
	glVertex3d(+1, -1, -1);
	glVertex3d(2, 0, 0);
	glVertex3d(+1, +1, -1);
	glVertex3d(2, 0, 0);
	glVertex3d(+1, -1, +1);
	glVertex3d(2, 0, 0);
	glVertex3d(+1, +1, +1);
	glEnd();
	glPopMatrix();
	}
}
void draw_floor() {
	glColor3d(0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0,0);
	glRotatef(-yaw, 0, 1, 0);
	glTranslated(subx, -suby ,subz);
	glBegin(GL_QUADS);
	glVertex3f(-10,-2,-10);
	glVertex3f(-10,-2,10);
	glVertex3f(10,-2,10);
	glVertex3f(10,-2,-10);
	glEnd();
	glPopMatrix();
}
#define TO_RADIANS 3.1415926/180
void submarinepos() {
	if (motions.left) {
		subx += speed * cosf((subyaw + 90) * 3.1415926 / 180);
		subz += speed * sinf((subyaw + 90) * 3.1415926 / 180);
	}
	else if (motions.right) {
		subx -= speed * cosf((subyaw + 90) * 3.1415926 / 180);
		subz -= speed * sinf((subyaw + 90) * 3.1415926 / 180);
	}
	else if (motions.front) {
		subx -= speed * cosf((subyaw) * 3.1415926/180);
		subz -= speed * sinf((subyaw) * 3.1415926 / 180);
	}
	else if (motions.back) {
		subx += speed * cosf((subyaw) * 3.1415926 / 180);
		subz += speed * sinf((subyaw) * 3.1415926 / 180);
	}
	if (motions.up) {
		suby += speed;
	}
	if (motions.down) {
		suby -= speed;
	}
	if (motions.rotate_left) {
		subyaw -= 1;
		if(subyaw<0)subyaw+=360;
		yaw+=(!third_person_view);
		if(yaw>=360)yaw-=360;
	}
	if (motions.rotate_right) {
		subyaw += 1;
		if(subyaw>=360){
			subyaw-=360;
		}
		yaw-=(!third_person_view);
		if(yaw<0)yaw+=360;
	}
	glutPostRedisplay();
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(third_person_view)
		gluLookAt(-cam_dis*cosf(pitch*3.1415926/180), cam_dis*sinf(-pitch * 3.1415926 / 180),0, 0, 0, 0, 0, 1, 0);
	else
		gluLookAt(0,0,0,cam_dis*cosf(pitch*3.1415926/180), -cam_dis*sinf(-pitch * 3.1415926 / 180),0, 0, 1, 0);
	submarinepos();
	draw_cube();
	draw_floor();
	glutSwapBuffers();
}
void reshape(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(60, float(width) / height, 1, 100);//sight
    glMatrixMode(GL_MODELVIEW);
}
void keyboard(unsigned char key, int x, int y) {
	if (key == 'Q' || key == 'q') {
		motions.left = 1;
	}
	else if (key == 'S' || key == 's') {
		motions.back = 1;
	}
	else if (key == 'E' || key == 'e') {
		motions.right = 1;
	}
	else if (key == 'W' || key == 'w') {
		motions.front = 1;
	}
	else if (key == ' ') {
		motions.up = 1;
	}
	else if (key == 'Z' || key == 'z') {
		motions.down = 1;
	}
	else if (key == '0') {
		exit(0);
	}
	else if (key == 'A' || key == 'a') {
		motions.rotate_left = 1;
	}
	else if (key == 'D' || key == 'd') {
		motions.rotate_right = 1;
	}
}
void keyboardup(unsigned char key, int x, int y) {
	if (key == 'Q' || key == 'q') {
		motions.left = 0;
	}
	else if (key == 'S' || key == 's') {
		motions.back = 0;
	}
	else if (key == 'E' || key == 'e') {
		motions.right = 0;
	}
	else if (key == 'W' || key == 'w') {
		motions.front = 0;
	}
	else if (key == ' ') {
		motions.up = 0;
	}
	else if (key == 'Z' || key == 'z') {
		motions.down = 0;
	}
	else if (key == 'A' || key == 'a') {
		motions.rotate_left = 0;
	}
	else if (key == 'D' || key == 'd') {
		motions.rotate_right = 0;
	}
	else if(key=='F'||key=='f'){
		third_person_view^=1;
		if(!third_person_view){
			yaw=360-subyaw;
		}
	}
}
void mouse(int button, int state, int x, int y) {
	if (button == 3) {//front
		cam_dis -= 0.5;
	}
	else if(button==4){
		cam_dis += 0.5;
	}
	cam_dis = max(min(80.0,cam_dis ),5.0 );
}
void passive_motion(int x,int y) {
	double dev_x, dev_y;
    dev_x = (width / 2) - x;
    dev_y = (height / 2) - y;
    yaw += dev_x / 10.0;
    if(yaw>=360)yaw-=360;
    else if(yaw<0)yaw+=360;
    pitch += dev_y / 8.0;
	pitch = max(min(pitch, (double)60.0), (double)-60.0);
	if(!third_person_view){
		//
		//
		//
		//
		//
		//
	}
	cout << yaw << " " << subyaw<< endl;
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("subnarine");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passive_motion);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}