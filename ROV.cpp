#include<iostream>
#include <GL/glut.h>
#include<algorithm>
#include<math.h>
#include<vector>
#include<random>
#include<functional>
#include<utility>
#define FPS 60.0
#define TO_RADIANS 3.1415926/180
using namespace std;
double width = GetSystemMetrics(SM_CXSCREEN)/*800.0*/;
double height = GetSystemMetrics(SM_CYSCREEN)/*450.0*/;
double subx = 0, suby = 0, subz = 0;
double speed = 0.2;
double subyaw = 0;
double pitch = -30, yaw = 0;
double cam_dis = 30;
double ppp = 0;
bool third_person_view = 1;
bool openlight = 1;
bool grab = 0;
double fishspeed = 0.4;
int kk = 0;
struct Fish {
	double x, y, z, angle;
};
struct Plants {
	double x, y, z;
};
vector<double> arr;
vector<pair<double, double> >robots;
vector<vector<Plants > >plants;
vector<struct Fish> fishes;
GLUquadricObj* sphere = NULL;
random_device rd;
default_random_engine gen = default_random_engine(rd());
uniform_real_distribution<float>dis(0, 250), fis(-500, 500);
auto randpos = bind(dis, gen), fishrandpos = bind(fis, gen);
struct Motions {
	bool left, right, front, back, up, down, rotate_left, rotate_right, accelerate, decelerate;
};
Motions motions = { 0,0,0,0,0,0,0,0 ,0,0 };
float persistence = 0.8;
int Number_of_Octaves = 8;
double Noise(int x, int y) {
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}
double SmoothedNoise(int x, int y) {
	double corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16;
	double sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8;
	double center = Noise(x, y) / 4;
	return corners + sides + center;
}
double Cosine_Interpolate(double a, double b, double x) {
	double ft = x * 3.1415926;
	double f = (1 - cosf(ft)) * 0.5;
	return a * (1 - f) + b * f;
}
double Linear_Interpolate(double a, double b, double x) {
	return a * (1 - x) + b * x;
}
double InterpolatedNoise(float x, float y) {
	int integer_X = int(x);
	float  fractional_X = x - integer_X;
	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;
	double v1 = SmoothedNoise(integer_X, integer_Y);
	double v2 = SmoothedNoise(integer_X + 1, integer_Y);
	double v3 = SmoothedNoise(integer_X, integer_Y + 1);
	double v4 = SmoothedNoise(integer_X + 1, integer_Y + 1);
	double i1 = Cosine_Interpolate(v1, v2, fractional_X);
	double i2 = Cosine_Interpolate(v3, v4, fractional_X);
	return Cosine_Interpolate(i1, i2, fractional_Y);
}
double PerlinNoise(float x, float y) {
	double noise = 0;
	double p = persistence;
	int n = Number_of_Octaves;
	for (int i = 0; i < n; i++) {
		double frequency = pow(2, i);
		double amplitude = pow(p, i);
		noise = noise + InterpolatedNoise(x * frequency, y * frequency) * amplitude;
	}
	return noise;
}
void init_robots() {
	robots.resize(4);
	for (int i = 0; i < 2; i++) {
		for (int k = 0; k < 2; k++) {
			double rx = i * 500 + fabs(fishrandpos()) - 500, rz = k * 500 + fabs(fishrandpos()) - 500;
			robots[i * 2 + k] = { rx,rz };
		}
	}
}
void init_floor() {
	arr.resize(101 * 101 + 1);
	for (double i = 0; i < 10; i += .1) {
		for (double k = 0; k < 10; k += .1) {
			arr[i * 1000 + k * 10] = PerlinNoise(i, k);
		}
	}
	suby = arr[50 * 100 + 50] + 5;
}
void init_plants() {
	plants.resize(4, vector<Plants>(4));
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
			double px = i * 250.0 + randpos() - 500, pz = k * 250.0 + randpos() - 500, py = (int(randpos() * 2) % 260 + 100) * 10;
			plants[i][k] = { px,py,pz };
		}
	}
}
void init_fish() {
	fishes.resize(8);
	for (int i = 0; i < 8; i++) {
		double fx = fishrandpos(), fy = randpos(), fz = fishrandpos(), angle = int(fishrandpos() + 1000) % 360;
		fishes[i] = { fx,fy + 10,fz,angle };
	}
}
void init() {
	glutSetCursor(GLUT_CURSOR_NONE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, float(width) / height, 1, 5000);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glutWarpPointer(width / 2, height / 2);
	init_floor();
	init_plants();
	init_fish();
	init_robots();
}
void light() {
	GLfloat ambient[] = { .1, .1, .1, .1 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[] = { 0.0, 300.0, 0.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1 };
	GLfloat local_view[] = { 0.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_CONSTANT_ATTENUATION, ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	if (openlight) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
}
double offset = 0;

void draw_fishes() {
	glColor3d(241 / 255.0, 71 / 255.0, 197 / 255.0);
	for (int i = 0; i < 8; i++) {
		glPushMatrix();
		glTranslatef(fishes[i].x, fishes[i].y, fishes[i].z);
		glRotatef(fishes[i].angle, 0, 1, 0);
		glScaled(3, 3, 3);
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glScalef(0.5, 0.3, 1);
		glRotatef(45, 0, 1, 0);
		glutSolidCube(3);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0.1, 1);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, -.7);
		glColor3d(0, 0, 0);
		gluCylinder(sphere, 0.3, 0.3, 1.4, 16, 16);
		glPopMatrix();
		glColor3d(241 / 255.0, 71 / 255.0, 197 / 255.0);
		glPushMatrix();
		glTranslatef(-cosf(offset * 2 * TO_RADIANS), 0, -2.15);
		for (int k = 0; k < 150; k++) {
			glBegin(GL_POLYGON);
			glVertex3d(.9 * cosf((k + offset) * 2 * TO_RADIANS), k / 150.0, -(k * 2) / 150.0);
			glVertex3d(.9 * cosf((k + offset) * 2 * TO_RADIANS), -k / 150.0, -(k * 2) / 150.0);
			glVertex3d(.9 * cosf((k + 1 + offset) * 2 * TO_RADIANS), (-k - 1) / 150.0, -((k + 1) * 2) / 150.0);
			glVertex3d(.9 * cosf((k + 1 + offset) * 2 * TO_RADIANS), (k + 1) / 150.0, -((k + 1) * 2) / 150.0);
			glEnd();
			offset += 0.005;
		}
		glPopMatrix();
		glPopMatrix();
		fishes[i].x += fishspeed * cosf((fishes[i].angle - 90) * TO_RADIANS);
		fishes[i].z -= fishspeed * sinf((fishes[i].angle - 90) * TO_RADIANS);
		if (fishes[i].x > 500)fishes[i].x = -500;
		else if (fishes[i].x < -500)fishes[i].x = 500;
		if (fishes[i].z > 500)fishes[i].z = -500;
		else if (fishes[i].z < -500)fishes[i].z = 500;
	}
}
void draw_plants() {
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
			glPushMatrix();
			glTranslatef(plants[i][k].x, 0, plants[i][k].z);
			glRotatef(int(plants[i][k].x + plants[i][k].z) % 360, 0, 1, 0);
			glColor3d(144 / 255.0, 230 / 255.0, 135 / 255.0);
			glPushMatrix();
			double gx, gy, gz, gxx = cosf(ppp / 3.0 * TO_RADIANS) * 4.0, gyy = 0, gzz = cosf(ppp / 3.0 * TO_RADIANS) * 4.0;
			glTranslated(-gxx, 0, -gzz);
			for (int j = 0; j < plants[i][k].y; j++) {
				glBegin(GL_QUADS);
				gx = gxx, gy = gyy;
				gz = gzz;
				glVertex3d(gx + 3, gy, gz);
				glVertex3d(gx - 3, gy, gz);
				gxx = cosf((j + ppp + 1) / 3.0 * TO_RADIANS) * 4.0, gyy = (j + 1.0) / 15.0, 0;
				gzz = cosf((j + ppp + 1) / 3.0 * TO_RADIANS) * 4.0;
				glVertex3d(gxx - 3, gyy, gzz);
				glVertex3d(gxx + 3, gyy, gzz);
				glVertex3d(gx, gy, gz + 3);
				glVertex3d(gx, gy, gz - 3);
				glVertex3d(gxx, gyy, gzz - 3);
				glVertex3d(gxx, gyy, gzz + 3);
				glEnd();
			}
			glPopMatrix();
			glPopMatrix();
		}
	}
	ppp += 3;
}
void draw_coord() {
	glPushMatrix();
	if (sphere == NULL) {
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricNormals(sphere, GLU_SMOOTH);
	}
	glPushMatrix();
	glTranslatef(0, 0, -500);
	glColor3d(0, 0, 0);
	gluCylinder(sphere, 0.05, 1, 1000, 16, 16);
	glPopMatrix();
	glRotated(90, 0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0, -500);
	glColor3d(0, 0, 0);
	gluCylinder(sphere, 0.05, 1, 1000, 16, 16);
	glPopMatrix();
	glRotated(90, 1, 0, 0);
	glPushMatrix();
	glTranslatef(0, 0, -500);
	glColor3d(0, 0, 0);
	gluCylinder(sphere, 0.05, 1, 1000, 16, 16);
	glPopMatrix();
	glPopMatrix();
}
vector<double> tmp;
void Cube() {
	glutSolidCube(1);
}
void Ball() {
	glutSolidSphere(1, 16, 16);
}
void Bar() {
	gluCylinder(sphere, 0.5, 0.5, 1, 16, 16);
}
void robot() {
	glPushMatrix();
	glTranslatef(0, 9, 0);
	glPushMatrix();
	glScalef(4, 6, 2);
	Cube();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 4, 0);
	Ball();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.3, 2, 0);
	glScalef(0.5, 0.5, 0.5);
	Ball();
	glScalef(0.5, 10, 0.5);
	glRotated(90, 1, 0, 0);
	Bar();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.3, 2, 0);
	glScalef(0.5, 0.5, 0.5);
	Ball();
	glScalef(0.5, 10, 0.5);
	glRotated(90, 1, 0, 0);
	Bar();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1, -3.3, 0);
	glScalef(0.5, 0.5, 0.5);
	Ball();
	glScalef(0.5, 10, 0.5);
	glRotated(90, 1, 0, 0);
	Bar();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1, -3.3, 0);
	glScalef(0.5, 0.5, 0.5);
	Ball();
	glScalef(0.5, 10, 0.5);
	glRotated(90, 1, 0, 0);
	Bar();
	glPopMatrix();
	glPopMatrix();
}
void draw_robots() {
	glColor3f(0.1, 0.1, 0.1);
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslatef(robots[i].first, -1, robots[i].second);
		robot();
		glPopMatrix();
	}
}
void floor() {
	glColor3d(0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glRotatef(-yaw, 0, 1, 0);
	glTranslated(subx, -suby, subz);
	draw_plants();
	draw_coord();
	draw_robots();
	draw_fishes();
	light();
	tmp.clear();
	for (int i = 0; i < 100; i++) {
		for (int k = 0; k < 100; k++) {
			double tl = arr[i * 100.0 + k], tr = arr[i * 100.0 + k + 1], bl = arr[(i + 1.0) * 100 + k], br = arr[(i + 1.0) * 100 + k + 1];
			glBegin(GL_POLYGON);
			if (abs(50 - int(subx) / 10 - i) <= 0.5 && abs(50 - int(subz) / 10 - k) <= 0.5)tmp.push_back(arr[i * 100.0 + k] * 5);
			glColor3d((1 + arr[i * 100.0 + k]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k]) / 2.0);
			glVertex3d(i * 10.0 - 500, tl * 5, k * 10.0 - 500);
			if (abs(50 - int(subx) / 10 - i - 1) <= 0.5 && abs(50 - int(subz) / 10 - k) <= 0.5)tmp.push_back(arr[i * 100.0 + 100 + k] * 5);
			glColor3d((1 + arr[(i + 1.0) * 100 + k]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k]) / 2.0);
			glVertex3d((i + 1.0) * 10 - 500, bl * 5, k * 10.0 - 500);
			if (abs(50 - int(subx) / 10 - i - 1) <= 0.5 && abs(50 - int(subz) / 10 - k - 1) <= 0.5)tmp.push_back(arr[i * 100.0 + 101 + k] * 5);
			glColor3d((1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0);
			glVertex3d((i + 1.0) * 10 - 500, br * 5, (k + 1.0) * 10 - 500);
			glEnd();
			glBegin(GL_POLYGON);
			if (abs(50 - int(subx) / 10 - i) <= 0.5 && abs(50 - int(subz) / 10 - k) <= 0.5)tmp.push_back(arr[i * 100.0 + k] * 5);
			glColor3d((1 + arr[i * 100.0 + k]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k]) / 2.0);
			glVertex3d(i * 10.0 - 500, tl * 5, k * 10.0 - 500);
			if (abs(50 - int(subx) / 10 - i - 1) <= 0.5 && abs(50 - int(subz) / 10 - k - 1) <= 0.5)tmp.push_back(arr[i * 100.0 + 101 + k] * 5);
			glColor3d((1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0 + 0.2, (1 + arr[(i + 1.0) * 100 + k + 1]) / 2.0);
			glVertex3d((i + 1.0) * 10.0 - 500, br * 5, (k + 1.0) * 10 - 500);
			if (abs(50 - int(subx) / 10 - i) <= 0.5 && abs(50 - int(subz) / 10 - k - 1) <= 0.5)tmp.push_back(arr[i * 100.0 + k + 1] * 5);
			glColor3d((1 + arr[i * 100.0 + k + 1]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k + 1]) / 2.0 + 0.2, (1 + arr[i * 100.0 + k + 1]) / 2.0);
			glVertex3d(i * 10.0 - 500, tr * 5, (k + 1.0) * 10 - 500);
			glEnd();
		}
	}
	glPopMatrix();
}
void timer(int) {
	glutPostRedisplay();
	glutWarpPointer(width / 2, height / 2);
	glutTimerFunc(1000 / FPS, timer, 0);
}
void draw_ROV() {
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glRotatef(-yaw - subyaw, 0, 1, 0);
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(-2, 0, 0);
	if (third_person_view) {
		if (motions.front || motions.back)
			glRotatef(10 * (kk++), 1, 0, 0);
		kk %= 13;
		for (int gg = 0; gg < 3; gg++) {
			glBegin(GL_POLYGON);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 1, 0);
			glVertex3f(0, 0.8, -0.2);
			glEnd();
			glRotatef(120, 1, 0, 0);
		}
	}
	glPopMatrix();
	glBegin(GL_LINES);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 0, 1.2);
	glVertex3f(0, 0, 1.2);
	glVertex3f(1.2, 0, 1.2);
	glVertex3f(0, 0, -1);
	glVertex3f(0, 0, -1.2);
	glVertex3f(0, 0, -1.2);
	glVertex3f(1.2, 0, -1.2);
	glEnd();
	//left hand
	glPushMatrix();
	glTranslatef(1.2, 0, -1.2);
	for (int i = 0; i < 3; i++) {
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.3, 0);
		if (!grab) {
			glVertex3f(0, 0.3, 0);
			glVertex3f(0.5, 0.3, 0);
		}
		else {
			glVertex3f(0, 0.3, 0);
			glVertex3f(0.5, 0, 0);
		}
		glEnd();
		glRotatef(120, 1, 0, 0);
	}
	glPopMatrix();
	//right hand
	glPushMatrix();
	glTranslatef(1, 0, 1.2);
	for (int i = 0; i < 3; i++) {
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.3, 0);
		if (!grab) {
			glVertex3f(0, 0.3, 0);
			glVertex3f(0.5, 0.3, 0);
		}
		else {
			glVertex3f(0, 0.3, 0);
			glVertex3f(0.5, 0, 0);
		}
		glEnd();
		glRotatef(120, 1, 0, 0);
	}
	glPopMatrix();
	if (third_person_view) {
		glColor3f(0.3, 0.3, 0.3);
		glScalef(2, 1, 1);
		GLUquadric* quadratic;
		quadratic = gluNewQuadric();
		gluSphere(quadratic, 1, 30, 30);
	}
	glPopMatrix();
}
void ROVpos() {
	if (motions.accelerate) {
		speed += 0.03;
	}
	else if (motions.decelerate) {
		speed -= 0.03;
		speed = max(0.03, speed);
	}
	if (motions.left) {
		subx += speed * cosf((subyaw + 90) * TO_RADIANS);
		subz += speed * sinf((subyaw + 90) * TO_RADIANS);
	}
	if (motions.right) {
		subx -= speed * cosf((subyaw + 90) * TO_RADIANS);
		subz -= speed * sinf((subyaw + 90) * TO_RADIANS);
	}
	if (motions.front) {
		subx -= speed * cosf((subyaw)* TO_RADIANS);
		subz -= speed * sinf((subyaw)* TO_RADIANS);
	}
	if (motions.back) {
		subx += speed * cosf((subyaw)* TO_RADIANS);
		subz += speed * sinf((subyaw)* TO_RADIANS);
	}
	if (motions.up) {
		suby += speed;
		suby = min(suby, 360.0);
	}
	if (motions.down) {
		suby -= speed;
	}
	if (motions.rotate_left) {
		subyaw -= 1;
		if (subyaw < 0)subyaw += 360;
		yaw += (!third_person_view);
		if (yaw >= 360)yaw -= 360;
	}
	if (motions.rotate_right) {
		subyaw += 1;
		if (subyaw >= 360) {
			subyaw -= 360;
		}
		yaw -= (!third_person_view);
		if (yaw < 0)yaw += 360;
	}
	for (int i = 0; i < tmp.size(); i++) {
		suby = max(suby, tmp[i] + 3);
	}
	if (subx > 500)subx = 500;
	else if (subx < -500)subx = -500;
	if (subz > 500)subz = 500;
	else if (subz < -500)subz = -500;
	glutPostRedisplay();
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4d(0.117, 0.352, 0.666, 0);
	glutSolidSphere(600, 16, 16);
	if (third_person_view)
		gluLookAt(-cam_dis * cosf(pitch * TO_RADIANS), cam_dis * sinf(-pitch * TO_RADIANS), 0, 0, 0, 0, 0, 1, 0);
	else
		gluLookAt(0, 0, 0, cam_dis * cosf(pitch * TO_RADIANS), -cam_dis * sinf(-pitch * TO_RADIANS), 0, 0, 1, 0);
	ROVpos();
	draw_ROV();
	floor();
	glutSwapBuffers();
	glFlush();
}
void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, float(width) / height, 1, 5000);
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
	else if (key == '=') {
		motions.accelerate = 1;
	}
	else if (key == '-') {
		motions.decelerate = 1;
	}
	else if (key == 'l' || key == 'L') {
		openlight ^= 1;
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
	else if (key == 'F' || key == 'f') {
		third_person_view ^= 1;
		if (!third_person_view) {
			yaw = 360 - subyaw;
		}
	}
	else if (key == '=') {
		motions.accelerate = 0;
	}
	else if (key == '-') {
		motions.decelerate = 0;
	}
}
void mouse(int button, int state, int x, int y) {
	if (button == 3) {
		cam_dis -= 0.5;
	}
	else if (button == 4) {
		cam_dis += 0.5;
	}
	if (button == GLUT_LEFT_BUTTON && state == 0) {
		grab = 1;
	}
	if (button == GLUT_LEFT_BUTTON && state == 1) {
		grab = 0;
	}
	cam_dis = max(min(80.0, cam_dis), 5.0);
}
void passive_motion(int x, int y) {
	double dev_x, dev_y;
	dev_x = (width / 2) - x;
	dev_y = (height / 2) - y;
	if (third_person_view) {
		yaw += dev_x / 10.0;
		if (yaw >= 360)yaw -= 360;
		else if (yaw < 0)yaw += 360;
	}
	pitch += dev_y / 8.0;
	pitch = max(min(pitch, (double)60.0), (double)-60.0);
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("ROV");
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
