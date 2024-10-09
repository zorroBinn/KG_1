#include <GL/freeglut.h>
#include <Windows.h>
#include <iostream>
#include <cmath>
using namespace std;

#define PI 3.1415

//��������� ��������
float animationStart[3]; //��������� �������
float animationEnd[3]; //�������� �������
float animationProgress; //������� �������� (0.0 �� 1.0)
bool isAnimating = false; //������ ��������
float animationDuration = 2.0; //������������ �������� � ��������
int frameRate = 60; //������� ������
float step; //��� ��� ��������

//���������� ��� ����������������� �����
float pointA[3]; //��������� ����� ������
float pointB[3]; //�������� ����� ������
float moveDistance; //���������� �����������

//������� ������������� (���������� �������������)
float projMatrix[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0.5 * (-cos(45)), 0.5 * (-sin(45)), 0, 0},
    {0, 0, 0, 1}
};

//������� ������� ��������������
float transformMatrix[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

//����������� ������� ��� ������ ���������
float initialMatrix[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

//����� "�"
float Cshape[16][4] = { 
    //������ ������
    {0, 0, 0, 1},
    {2, 0, 0, 1},
    {2, 0.5, 0, 1},
    {0.5, 0.5, 0, 1},
    {0.5, 2.5, 0, 1},
    {2, 2.5, 0, 1},
    {2, 3, 0, 1},
    {0, 3, 0, 1},
    //�������� ������
    {0, 0, 1, 1},
    {2, 0, 1, 1},
    {2, 0.5, 1, 1},
    {0.5, 0.5, 1, 1},
    {0.5, 2.5, 1, 1},
    {2, 2.5, 1, 1},
    {2, 3, 1, 1},
    {0, 3, 1, 1}
};

void multiplyMatrix(float a[4][4], float b[4][4], float result[4][4]) { //������� ��� ��������� ������
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void multiplyVector(float vec[4], float mat[4][4], float result[4]) { //������� ��� ��������� ������� �� �������
    for (int i = 0; i < 4; ++i) {
        result[i] = 0;
        for (int j = 0; j < 4; ++j) {
            result[i] += vec[j] * mat[j][i];
        }
    }
}

void printInstructions() { //������� ��� ������ ��������� � �������
    cout << "����������:" << endl;
    cout << "�����������:" << endl;
    cout << "  w - �����" << endl;
    cout << "  s - ����" << endl;
    cout << "  a - �����" << endl;
    cout << "  d - ������" << endl;
    cout << "  q - ����� (� ������)" << endl;
    cout << "  e - ����� (�� ������)" << endl;
    cout << "��������:" << endl;
    cout << "  i/k - �������� ������ ��� X" << endl;
    cout << "  j/l - �������� ������ ��� Y" << endl;
    cout << "  u/o - �������� ������ ��� Z" << endl;
    cout << "���������������:" << endl;
    cout << "  z - ���������" << endl;
    cout << "  x - ���������" << endl;
    cout << "��������������:" << endl;
    cout << "  1 - ��������� ������������ ��������� XY" << endl;
    cout << "  2 - ��������� ������������ ��������� YZ" << endl;
    cout << "  3 - ��������� ������������ ��������� XZ" << endl;
    cout << "1.����������� (�������������) ����� ������������ ������" << endl << "�� �������� ���������� � ����������� ����� ���������� : " << endl;
    cout << "  m - �������� �����������" << endl;
    cout << "����� ���������:" << endl;
    cout << "  r - ����� � ��������� ���������" << endl;
    cout << "ESC ��� ������." << endl;
}

void drawAxes() { //��������� ������������ ����
    glBegin(GL_LINES);

    //��� X (�������)
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);

    //��� Y (������)
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);

    //��� Z (�����)
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f((-cos(45))*10, (-sin(45)) * 10, 10);

    glEnd();
}

void drawCShape() { //��������� ����� "�"
    glColor3f(0, 0, 0);

    //������ ������
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 8; ++i) {
        float transformed[4], projected[4];
        multiplyVector(Cshape[i], transformMatrix, transformed);
        multiplyVector(transformed, projMatrix, projected);
        glVertex3f(projected[0], projected[1], projected[2]);
    }
    glEnd();

    //�������� ������
    glBegin(GL_LINE_LOOP);
    for (int i = 8; i < 16; ++i) {
        float transformed[4], projected[4];
        multiplyVector(Cshape[i], transformMatrix, transformed);
        multiplyVector(transformed, projMatrix, projected);
        glVertex3f(projected[0], projected[1], projected[2]);
    }
    glEnd();

    //���������� ������ � �������� ������
    glBegin(GL_LINES);
    for (int i = 0; i < 8; ++i) {
        float transformed1[4], projected1[4];
        float transformed2[4], projected2[4];
        multiplyVector(Cshape[i], transformMatrix, transformed1);
        multiplyVector(transformed1, projMatrix, projected1);
        multiplyVector(Cshape[i + 8], transformMatrix, transformed2);
        multiplyVector(transformed2, projMatrix, projected2);
        glVertex3f(projected1[0], projected1[1], projected1[2]);
        glVertex3f(projected2[0], projected2[1], projected2[2]);
    }
    glEnd();
}

void display() { //��������� �����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    drawAxes();
    drawCShape();
    glutSwapBuffers();
}

void translate(float tx, float ty, float tz) { //�����������
    float translationMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {tx, ty, tz, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, translationMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void rotateX(float angle) { //�������� ������ OX
    float rad = angle * PI / 180.0;
    float rotationMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, cos(rad), sin(rad), 0},
        {0, -sin(rad), cos(rad), 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, rotationMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void rotateY(float angle) { //�������� ������ OY
    float rad = angle * PI / 180.0;
    float rotationMatrix[4][4] = {
        {cos(rad), 0, -sin(rad), 0},
        {0, 1, 0, 0},
        {sin(rad), 0, cos(rad), 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, rotationMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void rotateZ(float angle) { //�������� ������ OZ
    float rad = angle * PI / 180.0;
    float rotationMatrix[4][4] = {
        {cos(rad), sin(rad), 0, 0},
        {-sin(rad), cos(rad), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, rotationMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void scale(float sx, float sy, float sz) { //����������
    float scaleMatrix[4][4] = {
        {sx, 0, 0, 0},
        {0, sy, 0, 0},
        {0, 0, sz, 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, scaleMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void mirrorXY() { //��������� ������������ ��������� XY
    float mirrorMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, -1, 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, mirrorMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void mirrorYZ() { //��������� ������������ ��������� YZ
    float mirrorMatrix[4][4] = {
        {-1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, mirrorMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void mirrorXZ() { //��������� ������������ ��������� XZ
    float mirrorMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, -1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float newMatrix[4][4];
    multiplyMatrix(transformMatrix, mirrorMatrix, newMatrix);
    memcpy(transformMatrix, newMatrix, sizeof(newMatrix));
    glutPostRedisplay();
}

void resetState() { //����� ���������
    memcpy(transformMatrix, initialMatrix, sizeof(initialMatrix));
    glutPostRedisplay();
}

void animate(int value) { //������� ��� ��������
    if (isAnimating) {
        animationProgress += step;
        if (animationProgress >= 1.0) {
            animationProgress = 1.0;
            isAnimating = false;
        }
        else {
            //���������� ��������
            float easingProgress = animationProgress * animationProgress * (3 - 2 * animationProgress);

            float currentX = animationStart[0] + (animationEnd[0] - animationStart[0]) * easingProgress;
            float currentY = animationStart[1] + (animationEnd[1] - animationStart[1]) * easingProgress;
            float currentZ = animationStart[2] + (animationEnd[2] - animationStart[2]) * easingProgress;

            transformMatrix[3][0] = currentX;
            transformMatrix[3][1] = currentY;
            transformMatrix[3][2] = currentZ;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(1000 / frameRate, animate, 0); //������ ��� ���������� ����� ��������
}

void keyboard(unsigned char key, int x, int y) { //��������� ������
    switch (key) {
    case 'w': translate(0, 0.25, 0); break;
    case 's': translate(0, -0.25, 0); break;
    case 'a': translate(-0.25, 0, 0); break;
    case 'd': translate(0.25, 0, 0); break;
    case 'q': translate(0, 0, 0.25); break;
    case 'e': translate(0, 0, -0.25); break;
    case 'i': rotateX(10); break;
    case 'k': rotateX(-10); break;
    case 'j': rotateY(10); break;
    case 'l': rotateY(-10); break;
    case 'u': rotateZ(10); break;
    case 'o': rotateZ(-10); break;
    case 'z': scale(1.1, 1.1, 1.1); break;
    case 'x': scale(0.9, 0.9, 0.9); break;
    case '1': mirrorXY(); break;
    case '2': mirrorYZ(); break;
    case '3': mirrorXZ(); break;
    case 'r': resetState(); break;
    case 'm': {
        animationStart[0] = transformMatrix[3][0];
        animationStart[1] = transformMatrix[3][1];
        animationStart[2] = transformMatrix[3][2];

        //���������� ������� �����������
        float direction[3] = {
            pointB[0] - pointA[0],
            pointB[1] - pointA[1],
            pointB[2] - pointA[2]
        };

        //������������ ������� �����������
        float length = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
        direction[0] /= length;
        direction[1] /= length;
        direction[2] /= length;

        //����������� �������� ������� �� ��������� ���������
        animationEnd[0] = animationStart[0] + direction[0] * moveDistance;
        animationEnd[1] = animationStart[1] + direction[1] * moveDistance;
        animationEnd[2] = animationStart[2] + direction[2] * moveDistance;

        animationProgress = 0.0;
        step = 1.0 / (frameRate * animationDuration); //��� ��������
        isAnimating = true;
        break;
    }
    case 27: exit(0);
    }
}

void init() { //������������� OpenGL
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    printInstructions();

}

int main(int argc, char** argv) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "������� ���������� 1 ����� ������ (����� ������, X Y Z): ";
    cin >> pointA[0] >> pointA[1] >> pointA[2];
    cout << "������� ���������� 2 ����� ������ (����� ������, X Y Z): ";
    cin >> pointB[0] >> pointB[1] >> pointB[2];
    cout << "������� ���������� ��� ����������� ����� ������: ";
    cin >> moveDistance;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("3D");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, animate, 0);
    glutMainLoop();
    return 0;
}
