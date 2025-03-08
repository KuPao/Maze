#pragma once
#include <QGLWidget>
#include <QString>
#include <QDir>
#include "Cell.h"
#include "Edge.h"
#include "Frustum.h"
#include "Maze.h"
struct Vector3 {
	Vector3();
	Vector3(float x, float y, float z);
	float x;
	float y;
	float z;

	void normalize();
	Vector3 operator+(const Vector3&);
	Vector3 operator-(const Vector3&);
	float operator*(const Vector3&);
	Vector3 operator*(const float&);
	bool operator!=(const Vector3&);
};

class OpenGLWidget :public QGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidget(QWidget *parent = 0);

	void initializeGL();
	void paintGL();
	void resizeGL(int, int);

	//Maze Setting
	void Mini_Map();
	//void Draw_Cell(Cell* nextCell, Vector3 gL, Vector3 gR);
	void Draw_Cell(Cell* nextCell, Frustum* f, float focal_dist, int iteration);
	void Map_3D();
	void My_Draw_Cell(Cell* currentCell, Frustum* f);
	void CellUndraw();
	void loadTexture2D(QString, GLuint &);
	float degree_change(float);
	float* NDC(float* Vec);
	void Print44Mat(float Mat[16]);
	void MyLookAt(Vector3 eye, Vector3 center, Vector3 up);
	void MyPerspective(double fovy, double aspect, double zNear, double zFar);
	void MyNDC(double fovy, double aspect, double zNear, double zFar);
	void DrawWall(float xDrawStart, float yDrawStart, float xDrawEnd, float yDrawEnd, float color[3]);
	
private:
	GLuint grass_ID;
	GLuint sky_ID;
	QString pic_path;
	float* mvpMat;
	float* pMat;
	float* mvMat;
	float* NDCMat;

	float top_z;
	float but_z;
	float wallH = 1;
	Vector3 g, w, t, eye, u;
};

Vector3 cross(Vector3 a, Vector3 b);
float* multMat(float* a, float* b, int m, int p, int n);
