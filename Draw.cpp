#include <GL/glut.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include<utility>
using namespace std;
int width = 800, height = 600, type = 3;
float line_width = 3.0;
float r = 0, g = 0, b = 0, br = 1, bg = 1, bb = 1;//RGB, background RGB
int cx = -1, cy = -1, ux = -1, uy = -1, px = -1, py = -1, fpx = -1, fpy = -1;//first coordinate
bool filled = 1,leftclick = 0;//leftclick down?
bool grid = 0;
struct Object {
    pair<int, int> f, l;//first point, last point
    float w; //width
    float R, G, B;//color
    int t;//type
    bool fill;//fill mode
};
vector<Object>lines;//Vector that stores the objects you've drawn for redrawing
vector<Object>saved;
void init() {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, (double)width, 0.0, (double)height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(br, bg, bb, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}
void draw_grid() {
    glColor3d(0,0, 0);
    glLineWidth(3);
    for (int i = 0; i <70; i++) {
        glBegin(GL_LINES);
        glVertex2d(i * 100.0, 0);
        glVertex2d(i * 100.0,height);
        glEnd();
        glBegin(GL_LINES);
        glVertex2d(0, i * 100.0);
        glVertex2d(width, i * 100.0);
        glEnd();
    }
}
void display() {
    glFlush();
}
void draw_line(struct Object L) {
    glBegin(GL_LINES);
    glVertex2d(L.f.first, L.f.second);
    glVertex2d(L.l.first, L.l.second);
    glEnd();
}
void draw_circle(struct Object L) {
    GLUquadricObj* mycircle = gluNewQuadric();
    gluQuadricDrawStyle(mycircle, GLU_SMOOTH);
    float xl = (L.f.first - L.l.first);
    float yl = L.f.second - L.l.second;
    float rad = sqrt(xl * xl + yl * yl) / 2;
    glPushMatrix();
    glTranslatef((L.f.first + L.l.first) / 2, (L.f.second + L.l.second) / 2, 0.0);
    gluDisk(mycircle, (L.fill ? rad - L.w : 0), rad, 720, 1);
    glPopMatrix();
}
void draw_rectangle(struct Object L) {
    glBegin((L.fill ? GL_LINE_LOOP : GL_QUADS));
    glVertex2d(L.f.first, L.f.second);//bottom left
    glVertex2d(L.f.first, L.l.second);//top left
    glVertex2d(L.l.first, L.l.second);//top right
    glVertex2d(L.l.first, L.f.second);//bottom right
    glEnd();
}
void redraw() {//redraw after clearing color buffer
    if (grid) draw_grid();
    for (int i = 0; i < lines.size(); i++) {
        glColor3d(lines[i].R, lines[i].G, lines[i].B);
        glLineWidth(lines[i].w);
        if (lines[i].t == 1 || lines[i].t == 3 || lines[i].t == 5) draw_line(lines[i]);//line, curve, polygon
        else if (lines[i].t == 4)draw_rectangle(lines[i]);//rectangle
        else if (lines[i].t == 2)draw_circle(lines[i]);//circle
    }
}
void menu1(const int s) {
    /*--------------------Color--------------------*/
    if (s == 1)r = 1, g = 1, b = 1;//white
    else if (s == 2)r = 0, g = 0, b = 0;//black
    else if (s == 3)r = 1, g = 0, b = 0;//red
    else if (s == 4)r = 0, g = 1, b = 0;//green
    else if (s == 5)r = 0, g = 0, b = 1;//blue
    /*-------------------Width---------------------*/
    else if (s == 6)glLineWidth(line_width = min(line_width + 2, (float)20));//increase line width
    else if (s == 7)glLineWidth(line_width = max(line_width - 2, (float)1));//decrease line width
    /*-------------------Clear---------------------*/
    else if (s == 9) {
        glClear(GL_COLOR_BUFFER_BIT);
        lines.clear();
        if (grid)draw_grid();
        glFlush();
        px = -1;
    }
    /*-------------------Fill Mode-----------------*/
    else if (s == 10)filled ^= 1;
    /*---------------Background Color--------------*/
    else if (s > 10 && s <= 15) {
        if (s == 11)br = 1, bg = 1, bb = 1;//while
        else if (s == 12)br = 0, bg = 0, bb = 0;//black
        else if (s == 13)br = 1, bg = 0, bb = 0;//red
        else if (s == 14)br = 0, bg = 1, bb = 0;//green
        else br = 0, bg = 1, bb = 1;//blue
        glClearColor(br, bg, bb, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        redraw();
        if (grid)draw_grid();
        glFlush();
    }
    else if (s == 16) {
        grid ^= 1;
        if (grid) draw_grid();
        else {
            glClear(GL_COLOR_BUFFER_BIT);
            redraw();
        }
        glFlush();
    }
    /*---------------------Eraser------------------*/
    else if (s == 17)r = br, g = bg, b = bb, type = 3;
    /*---------------------File--------------------*/
    else if (s == 18)saved = lines;
    else if (s == 19) {
        lines = saved;
        glClear(GL_COLOR_BUFFER_BIT);
        redraw();
        if (grid)draw_grid();
        glFlush();
    }
    /*---------------------Type--------------------*/
    else if (s >= 21 && s <= 25) {
        if (s == 21)type = 1, cx = -1;//line  
        else if (s == 22)type = 2;//circle
        else if (s == 23)type = 3;//curve
        else if (s == 24)type = 4;//rectangle
        else if (s == 25)type = 5;//polygon
        r = 0, g = 0, b = 0;
    }
}
void my_reshape(int new_w, int new_h) {
    height = new_h, width = new_w;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)width, 0.0, (double)height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    redraw();
    if (grid)draw_grid();
}
void Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        leftclick = 1;
        cx = x, cy = height - y;
        if (type == 3)ux = x, uy = height - y;//curve   set previous point
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (!leftclick)return;//if the leftclick hasn't pressed, return
        leftclick = 0;
        if (type == 1 || type == 2) {//line or circle
            lines.push_back({ {cx,cy},{x,height - y},line_width,r,g,b,type,filled });
        }
        else if (type == 3) {//curve
            lines.push_back({ {ux,uy},{x,height - y},line_width,r,g,b,type ,filled });
        }
        else if (type == 5) {//polygon
            if (px != -1) {//if the first point of the polygon exist
                if ((fpx - x) * (fpx - x) + (fpy - height + y) * (fpy - height + y) < 101) {   //if the current point touches the first point, stop it.
                    lines.push_back({ {fpx,fpy},{px,py},line_width,r,g,b,type ,filled });
                    px = -1, py = -1;//init
                    return;
                }
                else
                    lines.push_back({ {px,py},{x,height - y},line_width,r,g,b,type ,filled });
            }
            else {
                fpx = x, fpy = height - y;//first point of the polygon
            }
            px = x, py = height - y;//previous point
        }
        else {//rectangle
            lines.push_back({ {min(cx,x),min(height - y,cy)},{max(x,cx),max(cy,height - y)},line_width,r,g,b,type ,filled });
        }
    }
}
void MouseMotion(int x, int y) {
    if (!leftclick)return;
    glClear(GL_COLOR_BUFFER_BIT);
    redraw();
    if (grid)draw_grid();
    glColor3d(r, g, b);
    glLineWidth(line_width);
    if (type == 1) {//line
        draw_line({ {cx,cy},{x,height - y},line_width,r,g,b,type,filled });
    }
    else if (type == 2) {//circle
        draw_circle({ {cx,cy},{x,height - y},line_width,r,g,b,type,filled });
    }
    else if (type == 3) {//curve
        draw_line({ {ux,uy},{x,height - y},line_width,r,g,b,type,filled });
        lines.push_back({ {ux,uy},{x,height - y},line_width,r,g,b,type,filled });
        ux = x, uy = height - y;
    }
    else if (type == 4) {//rectangle
        pair<float, float>bl = { min(cx,x) ,min(cy,height - y) }, tr = { max(cx,x),max(cy,height - y) };
        draw_rectangle({ bl,tr,line_width,r,g,b,type,filled });
    }
    else if (type == 5) {//polygon
        if (px != -1) {
            draw_line({ {px,py},{x,height - y},line_width,r,g,b,type,filled });
        }
    }
    glFlush();
}
void PassiveMouseMotion(int x, int y) {
    if (type == 5) {//polygon
        glClear(GL_COLOR_BUFFER_BIT);
        redraw();
        if (grid)draw_grid();
        glColor3d(r, g, b);
        glLineWidth(line_width);
        if (px != -1) {//polygon exist
            draw_line({ {px,py},{x,height - y},line_width,r,g,b,type,filled });
        }
        glFlush();
    }
}
void keyboard(unsigned char key, int x, int y) {
    if (key == 'Q' || key == 'q')exit(0);
    else if (key == 'C' || key == 'c') {
        glClear(GL_COLOR_BUFFER_BIT);
        lines.clear();
        glFlush();
        px = -1;
    }
    else if (key == '=')glLineWidth(line_width = min(line_width + 2, (float)20));//increase line width
    else if (key == '-')glLineWidth(line_width = max(line_width - 2, (float)1));//decrease line width
}
void Menu() {
    int mainmenu = glutCreateMenu(menu1);
    int colors = glutCreateMenu(menu1);
    int line_width = glutCreateMenu(menu1);
    int draw_types = glutCreateMenu(menu1);
    int background_color = glutCreateMenu(menu1);
    int file = glutCreateMenu(menu1);
    glutSetMenu(mainmenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    /*-------------------Main----------------------*/
    glutAddSubMenu("color", colors);
    glutAddSubMenu("background color", background_color);
    glutAddSubMenu("line width", line_width);
    glutAddSubMenu("types", draw_types);
    glutAddSubMenu("file", file);
    glutAddMenuEntry("fill mode", 10);
    glutAddMenuEntry("grid", 16);
    glutAddMenuEntry("eraser", 17);
    glutAddMenuEntry("clear", 9);
    /*-------------------Type----------------------*/
    glutSetMenu(draw_types);
    glutAddMenuEntry("curve", 23);
    glutAddMenuEntry("line", 21);
    glutAddMenuEntry("rectangle", 24);
    glutAddMenuEntry("polygon", 25);
    glutAddMenuEntry("circle", 22);
    /*--------------------Color--------------------*/
    glutSetMenu(colors);
    glutAddMenuEntry("White", 1);
    glutAddMenuEntry("Black", 2);
    glutAddMenuEntry("Red", 3);
    glutAddMenuEntry("Green", 4);
    glutAddMenuEntry("Blue", 5);
    /*---------------Background Color--------------*/
    glutSetMenu(background_color);
    glutAddMenuEntry("White", 11);
    glutAddMenuEntry("Black", 12);
    glutAddMenuEntry("Red", 13);
    glutAddMenuEntry("Green", 14);
    glutAddMenuEntry("Blue", 15);
    /*--------------------Width--------------------*/
    glutSetMenu(line_width);    //line width menu
    glutAddMenuEntry("wider", 6);
    glutAddMenuEntry("thinner", 7);
    /*---------------------File--------------------*/
    glutSetMenu(file);
    glutAddMenuEntry("save",18);
    glutAddMenuEntry("load",19);
}
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("Computer Graphic");
    glutDisplayFunc(display);
    glutReshapeFunc(my_reshape);
    Menu();
    glutMotionFunc(MouseMotion);
    glutMouseFunc(Mouse);
    glutPassiveMotionFunc(PassiveMouseMotion);//mouse motion without holding button
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();
    return 0;
}
