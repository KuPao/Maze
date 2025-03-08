#include "OpenGLWidget.h"
#include <iostream>
#include <algorithm>
#include "MazeWidget.h"
#include <gl\gl.h>
#include <gl\GLU.h>

OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{
	top_z = 1.5f;
	but_z = -1;

	QDir dir("Pic");
	if (dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0, 0, 0, 1);
	glEnable(GL_TEXTURE_2D);
	loadTexture2D(pic_path + "grass.png", grass_ID);
	loadTexture2D(pic_path + "sky.png", sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (MazeWidget::maze != NULL)
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, MazeWidget::w / 2, MazeWidget::h);
		glOrtho(-0.1, MazeWidget::maze->max_xp + 0.1, -0.1, MazeWidget::maze->max_yp + 0.1, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w / 2, 0, MazeWidget::w / 2, MazeWidget::h);
		/*gluPerspective 定義透視
		//視野大小, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		/*gluLookAt(viewerPosX, viewerPosZ, viewerPosY,
			viewerPosX + cos(degree_change(MazeWidget::maze->viewer_dir)), viewerPosZ, viewerPosY + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, -1.0, 0.0);*/
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		/*float m[16];
		gluLookAt(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[2], MazeWidget::maze->viewer_posn[1],
			MazeWidget::maze->viewer_posn[0] + cos(degree_change(MazeWidget::maze->viewer_dir)), MazeWidget::maze->viewer_posn[2], MazeWidget::maze->viewer_posn[1] + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, m);*/
		/*Print44Mat(m);*/
		Map_3D();
	}
}
void OpenGLWidget::resizeGL(int w, int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()
{
	glBegin(GL_LINES);

	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

	for (int i = 0; i < (int)MazeWidget::maze->num_edges; i++)
	{
		float edgeStartX = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X];
		float edgeStartY = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y];
		float edgeEndX = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X];
		float edgeEndY = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y];

		glColor3f(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
		if (MazeWidget::maze->edges[i]->opaque)
		{
			glVertex2f(edgeStartX, edgeStartY);
			glVertex2f(edgeEndX, edgeEndY);
		}
	}

	//draw frustum
	float len = 0.1;
	glColor3f(1, 1, 1);
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));

	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	glEnd();
}

//**********************************************************************
//
// * Draws the first-person view of the maze.
//   THIS IS THE FUINCTION YOU SHOULD MODIFY.
//
//Note: You must not use any openGL build-in function to set model matrix, view matrix and projection matrix.
//		ex: gluPerspective, gluLookAt, glTraslatef, glRotatef... etc.
//		Otherwise, You will get 0 !
//======================================================================

void OpenGLWidget::Map_3D()
{
	MazeWidget::maze->AddFrame();
	glLoadIdentity();

	// 畫右邊區塊的所有東西

	MyPerspective(MazeWidget::maze->viewer_fov, MazeWidget::w / 2 / MazeWidget::h, 0.01, 200);
	Vector3 viewer_pos = Vector3(MazeWidget::maze->viewer_posn[1], 0.0f, MazeWidget::maze->viewer_posn[0]);
	MyLookAt(viewer_pos,
		Vector3(viewer_pos.x + sin(degree_change(MazeWidget::maze->viewer_dir)), viewer_pos.y, viewer_pos.z + cos(degree_change(MazeWidget::maze->viewer_dir))),
		Vector3(0.0, 1.0, 0.0));

	//fov左右計算
	Frustum *f = new Frustum(MazeWidget::maze->viewer_dir, MazeWidget::maze->viewer_fov, MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1]);

	//Word-Screen Matrix
	//std::cout << "projection" << std::endl;
	//Print44Mat(pMat);
	//std::cout << "model_view" << std::endl;
	//Print44Mat(mvMat);
	mvpMat = multMat(pMat, mvMat, 4, 4, 4);
	//std::cout << "MVP" << std::endl;
	//Print44Mat(mvpMat);

	MazeWidget::maze->Find_View_Cell(MazeWidget::maze->cells[0]);
	My_Draw_Cell(MazeWidget::maze->GetViewCell(), f);
	CellUndraw();
	

	/*若有興趣的話, 可以為地板或迷宮上貼圖, 此項目不影響評分*/
	glBindTexture(GL_TEXTURE_2D, sky_ID);

	// 畫貼圖 & 算 UV

	glDisable(GL_TEXTURE_2D);
}

void OpenGLWidget::My_Draw_Cell(Cell * currentCell, Frustum * f)
{
	currentCell->isDrawn = true;
	LineSeg* edgeline[4];
	int edgecount = 0;
	char side;
	double FcrossE_R, FcrossE_L, EcrossF_R, EcrossF_L;
	bool draw = false;
	LineSeg newfrustumR;
	LineSeg newfrustumL;
	Cell* newCell;

	float drawStart[2], drawEnd[2];
	for (int i = 0; i < 4; i++) {
		edgeline[i] = new LineSeg(currentCell->edges[i]);
	}
	for (edgecount = 0; edgecount < 4; edgecount++) {
		FcrossE_R = f->edgelines[0].Cross_Param(*edgeline[edgecount]);
		EcrossF_R = edgeline[edgecount]->Cross_Param(f->edgelines[0]);
		FcrossE_L = f->edgelines[1].Cross_Param(*edgeline[edgecount]);
		EcrossF_L = edgeline[edgecount]->Cross_Param(f->edgelines[1]);

		side = currentCell->edges[edgecount]->Point_Side(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y]);
		if (FcrossE_R > 0) {//�k���u������
			if (EcrossF_R >= 0 && EcrossF_R <= 1) {//�k���u�P��ۥ�
				drawStart[Maze::X] = edgeline[edgecount]->start[Maze::X] + EcrossF_R * (edgeline[edgecount]->end[Maze::X] - edgeline[edgecount]->start[Maze::X]);
				drawStart[Maze::Y] = edgeline[edgecount]->start[Maze::Y] + EcrossF_R * (edgeline[edgecount]->end[Maze::Y] - edgeline[edgecount]->start[Maze::Y]);
				if (side == Edge::LEFT) {//�b����
					drawEnd[Maze::X] = edgeline[edgecount]->end[Maze::X];
					drawEnd[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
					draw = true;
				}
				else if (side == Edge::RIGHT) {//�b�k��
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X];
					drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
					draw = true;
				}

				if ((EcrossF_L >= 0) && (EcrossF_L <= 1) && (FcrossE_L > 0)) {//�����u�P��ۥ�
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X] + EcrossF_L * (edgeline[edgecount]->end[Maze::X] - edgeline[edgecount]->start[Maze::X]);
					drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y] + EcrossF_L * (edgeline[edgecount]->end[Maze::Y] - edgeline[edgecount]->start[Maze::Y]);
					draw = true;
				}
			}
			else if (side == Edge::LEFT && EcrossF_R < 0) {
				drawStart[Maze::X] = edgeline[edgecount]->start[Maze::X];
				drawStart[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
				if ((EcrossF_L >= 0) && (EcrossF_L <= 1) && (FcrossE_L > 0)) {
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X] + EcrossF_L * (edgeline[edgecount]->end[Maze::X] - edgeline[edgecount]->start[Maze::X]);
					drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y] + EcrossF_L * (edgeline[edgecount]->end[Maze::Y] - edgeline[edgecount]->start[Maze::Y]);
					draw = true;
				}
				else if (EcrossF_L > 1) {
					drawEnd[Maze::X] = edgeline[edgecount]->end[Maze::X];
					drawEnd[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
					draw = true;
				}
				else if (EcrossF_L < 0 && FcrossE_L < 0) {//
					drawEnd[Maze::X] = edgeline[edgecount]->end[Maze::X];
					drawEnd[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
					draw = true;
				}
			}
			else if (side == Edge::RIGHT && EcrossF_R > 1) {//
				drawStart[Maze::X] = edgeline[edgecount]->end[Maze::X];
				drawStart[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
				if (EcrossF_L >= 0 && EcrossF_L <= 1 && FcrossE_L > 0) {
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X] + EcrossF_L * (edgeline[edgecount]->end[Maze::X] - edgeline[edgecount]->start[Maze::X]);
					drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y] + EcrossF_L * (edgeline[edgecount]->end[Maze::Y] - edgeline[edgecount]->start[Maze::Y]);
					draw = true;
				}
				else if (EcrossF_L < 0) {
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X];
					drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
					draw = true;
				}
				else if (EcrossF_L > 1 && FcrossE_L < 0) {
					drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X];
					drawEnd[Maze::Y] - edgeline[edgecount]->start[Maze::Y];
					draw = true;
				}
			}
			if (draw) {
				if (currentCell->edges[edgecount]->opaque) {
					float color[3] = { currentCell->edges[edgecount]->color[0],currentCell->edges[edgecount]->color[1],currentCell->edges[edgecount]->color[2] };
					DrawWall(drawStart[Maze::X], drawStart[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y], color);
					draw = false;
				}
				else {
					Vector3 a(drawStart[Maze::X] - MazeWidget::maze->viewer_posn[Maze::X], 0, drawStart[Maze::Y] - MazeWidget::maze->viewer_posn[Maze::Y]);
					Vector3 b(drawEnd[Maze::X] - MazeWidget::maze->viewer_posn[Maze::X], 0, drawEnd[Maze::Y] - MazeWidget::maze->viewer_posn[Maze::Y]);
					Vector3 c = cross(a, b);
					LineSeg newfrustumR;
					LineSeg newfrustumL;
					Frustum* newF;
					if (c.y < 0) {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							drawStart[Maze::X], drawStart[Maze::Y],
							drawEnd[Maze::X], drawEnd[Maze::Y]);
						LineSeg newfrustumR(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawStart[Maze::X], drawStart[Maze::Y]);
						newfrustumR.end[Maze::X] += (newfrustumR.end[Maze::X] - newfrustumR.start[Maze::X]) * 100;
						newfrustumR.end[Maze::Y] += (newfrustumR.end[Maze::Y] - newfrustumR.start[Maze::Y]) * 100;
						LineSeg newfrustumL(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y]);
						newfrustumL.end[Maze::X] += (newfrustumL.end[Maze::X] - newfrustumL.start[Maze::X]) * 100;
						newfrustumL.end[Maze::Y] += (newfrustumL.end[Maze::Y] - newfrustumL.start[Maze::Y]) * 100;
					}
					else {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							drawEnd[Maze::X], drawEnd[Maze::Y],
							drawStart[Maze::X], drawStart[Maze::Y]);
						LineSeg newfrustumR(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y]);
						newfrustumR.end[Maze::X] += (newfrustumR.end[Maze::X] - newfrustumR.start[Maze::X]) * 100;
						newfrustumR.end[Maze::Y] += (newfrustumR.end[Maze::Y] - newfrustumR.start[Maze::Y]) * 100;
						LineSeg newfrustumL(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawStart[Maze::X], drawStart[Maze::Y]);
						newfrustumL.end[Maze::X] += (newfrustumL.end[Maze::X] - newfrustumL.start[Maze::X]) * 100;
						newfrustumL.end[Maze::Y] += (newfrustumL.end[Maze::Y] - newfrustumL.start[Maze::Y]) * 100;
					}
					Cell* newCell = currentCell->edges[edgecount]->Neighbor(currentCell);
					draw = false;
					if (!newCell->isDrawn)
						My_Draw_Cell(newCell, newF);
				}
			}
		}

		else if (FcrossE_L > 0) {

			if (EcrossF_L >= 0 && EcrossF_L <= 1) {
				drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X] + EcrossF_L * (edgeline[edgecount]->end[Maze::X] - edgeline[edgecount]->start[Maze::X]);
				drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y] + EcrossF_L * (edgeline[edgecount]->end[Maze::Y] - edgeline[edgecount]->start[Maze::Y]);
				if (side == Edge::RIGHT) {
					drawStart[Maze::X] = edgeline[edgecount]->end[Maze::X];
					drawStart[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
					draw = true;
				}
				else if (side == Edge::LEFT) {
					drawStart[Maze::X] = edgeline[edgecount]->start[Maze::X];
					drawStart[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
					draw = true;
				}
			}
			else if (side == Edge::LEFT && EcrossF_L > 1) {
				drawStart[Maze::X] = edgeline[edgecount]->start[Maze::X];
				drawStart[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
				drawEnd[Maze::X] = edgeline[edgecount]->end[Maze::X];
				drawEnd[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
				draw = true;
			}
			else if (side == Edge::RIGHT && EcrossF_L < 0) {//
				drawEnd[Maze::X] = edgeline[edgecount]->start[Maze::X];
				drawEnd[Maze::Y] = edgeline[edgecount]->start[Maze::Y];
				drawStart[Maze::X] = edgeline[edgecount]->end[Maze::X];
				drawStart[Maze::Y] = edgeline[edgecount]->end[Maze::Y];
				draw = true;
			}
			if (draw) {
				if (currentCell->edges[edgecount]->opaque) {
					float color[3] = { currentCell->edges[edgecount]->color[0],currentCell->edges[edgecount]->color[1],currentCell->edges[edgecount]->color[2] };
					DrawWall(drawStart[Maze::X], drawStart[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y], color);
					draw = false;
				}
				else {
					Vector3 a(drawStart[Maze::X] - MazeWidget::maze->viewer_posn[Maze::X], 0, drawStart[Maze::Y] - MazeWidget::maze->viewer_posn[Maze::Y]);
					Vector3 b(drawEnd[Maze::X] - MazeWidget::maze->viewer_posn[Maze::X], 0, drawEnd[Maze::Y] - MazeWidget::maze->viewer_posn[Maze::Y]);
					Vector3 c = cross(a, b);
					LineSeg newfrustumR;
					LineSeg newfrustumL;
					Frustum* newF;
					if (c.y < 0) {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							drawStart[Maze::X], drawStart[Maze::Y],
							drawEnd[Maze::X], drawEnd[Maze::Y]);
						LineSeg newfrustumR(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawStart[Maze::X], drawStart[Maze::Y]);
						newfrustumR.end[Maze::X] += (newfrustumR.end[Maze::X] - newfrustumR.start[Maze::X]) * 100;
						newfrustumR.end[Maze::Y] += (newfrustumR.end[Maze::Y] - newfrustumR.start[Maze::Y]) * 100;
						LineSeg newfrustumL(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y]);
						newfrustumL.end[Maze::X] += (newfrustumL.end[Maze::X] - newfrustumL.start[Maze::X]) * 100;
						newfrustumL.end[Maze::Y] += (newfrustumL.end[Maze::Y] - newfrustumL.start[Maze::Y]) * 100;
					}
					else {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							drawEnd[Maze::X], drawEnd[Maze::Y],
							drawStart[Maze::X], drawStart[Maze::Y]);
						LineSeg newfrustumR(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawEnd[Maze::X], drawEnd[Maze::Y]);
						newfrustumR.end[Maze::X] += (newfrustumR.end[Maze::X] - newfrustumR.start[Maze::X]) * 100;
						newfrustumR.end[Maze::Y] += (newfrustumR.end[Maze::Y] - newfrustumR.start[Maze::Y]) * 100;
						LineSeg newfrustumL(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], drawStart[Maze::X], drawStart[Maze::Y]);
						newfrustumL.end[Maze::X] += (newfrustumL.end[Maze::X] - newfrustumL.start[Maze::X]) * 100;
						newfrustumL.end[Maze::Y] += (newfrustumL.end[Maze::Y] - newfrustumL.start[Maze::Y]) * 100;
					}
					Cell* newCell = currentCell->edges[edgecount]->Neighbor(currentCell);
					draw = false;
					if (!newCell->isDrawn)
						My_Draw_Cell(newCell, newF);

				}
			}
		}

	}
	//int frustumEdge, edgeId, side;
	//float f_e_ValueR, e_f_ValueR, f_e_ValueL, e_f_ValueL, xDrawStart, yDrawStart, xDrawEnd, yDrawEnd;
	//const float err = 0.000001;
	//LineSeg* edgeLines[4];
	//Vertex* drawVertex[4];
	//bool draw = false;

	//currentCell->isDrawn = true;

	//for (int i = 0; i < 4; i++)//convert edges into segements
	//	edgeLines[i] = new LineSeg(currentCell->edges[i]);
	//for (int i = 0; i < 4; i++)//vertex for draw
	//	drawVertex[i] = new Vertex();

	//for (edgeId = 0; edgeId < 4; edgeId++) {
	//	/*if (currentCell->edges[edgeId]->index == 2) {
	//		std::cout << std::endl; 
	//	}*/
	//	f_e_ValueR = f->edgelines[0].Cross_Param(*edgeLines[edgeId]);
	//	e_f_ValueR = edgeLines[edgeId]->Cross_Param(f->edgelines[0]);
	//	f_e_ValueL = f->edgelines[1].Cross_Param(*edgeLines[edgeId]);
	//	e_f_ValueL = edgeLines[edgeId]->Cross_Param(f->edgelines[1]);

	//	side = currentCell->edges[edgeId]->Point_Side(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y]);
	//	if (f_e_ValueR > -err) {//面對牆

	//		if (e_f_ValueR >= -err && e_f_ValueR - 1 <= err) {// 視線與牆有交點
	//			xDrawStart = edgeLines[edgeId]->start[0] + e_f_ValueR * (edgeLines[edgeId]->end[0] - edgeLines[edgeId]->start[0]);
	//			yDrawStart = edgeLines[edgeId]->start[1] + e_f_ValueR * (edgeLines[edgeId]->end[1] - edgeLines[edgeId]->start[1]);

	//			if (side == Edge::LEFT) {
	//				//位置在牆的左邊 取牆的終點
	//				xDrawEnd = edgeLines[edgeId]->end[0];
	//				yDrawEnd = edgeLines[edgeId]->end[1];
	//				draw = true;
	//			}
	//			else if (side == Edge::RIGHT) {
	//				//位置在牆的右邊 取牆的起點
	//				xDrawEnd = edgeLines[edgeId]->start[0];
	//				yDrawEnd = edgeLines[edgeId]->start[1];
	//				draw = true;
	//			}

	//			if ((e_f_ValueL >= -err) && (e_f_ValueL - 1 <= err) && (f_e_ValueL > -err)) { //視線左界與目前牆實際相交
	//				xDrawEnd = edgeLines[edgeId]->start[0] + e_f_ValueL * (edgeLines[edgeId]->end[0] - edgeLines[edgeId]->start[0]);
	//				yDrawEnd = edgeLines[edgeId]->start[1] + e_f_ValueL * (edgeLines[edgeId]->end[1] - edgeLines[edgeId]->start[1]);
	//				draw = true;
	//			}
	//			
	//		}

	//		else if (side == Edge::LEFT && e_f_ValueR < err) { 
	//			xDrawStart = edgeLines[edgeId]->start[0];
	//			yDrawStart = edgeLines[edgeId]->start[1];
	//			if ((e_f_ValueL >= -err) && (e_f_ValueL - 1 <= err) && (f_e_ValueL > -err)) { //視線左界與目前牆實際相交
	//				xDrawEnd = edgeLines[edgeId]->start[0] + e_f_ValueL * (edgeLines[edgeId]->end[0] - edgeLines[edgeId]->start[0]);
	//				yDrawEnd = edgeLines[edgeId]->start[1] + e_f_ValueL * (edgeLines[edgeId]->end[1] - edgeLines[edgeId]->start[1]);
	//				draw = true;
	//			}
	//			else if (e_f_ValueL - 1 > err) {
	//				xDrawEnd = edgeLines[edgeId]->end[0];
	//				yDrawEnd = edgeLines[edgeId]->end[1];
	//				draw = true;
	//			}
	//			else if (e_f_ValueL < -err && f_e_ValueL < err) {
	//				xDrawEnd = edgeLines[edgeId]->end[0];
	//				yDrawEnd = edgeLines[edgeId]->end[1];
	//				draw = true;
	//			}
	//		}

	//		else if (side == Edge::RIGHT && e_f_ValueR - 1 > err) {
	//			xDrawStart = edgeLines[edgeId]->end[0];
	//			yDrawStart = edgeLines[edgeId]->end[1];
	//			if ((e_f_ValueL >= -err) && (e_f_ValueL - 1 <= err) && (f_e_ValueL > -err)) { //視線左界與目前牆實際相交
	//				xDrawEnd = edgeLines[edgeId]->start[0] + e_f_ValueL * (edgeLines[edgeId]->end[0] - edgeLines[edgeId]->start[0]);
	//				yDrawEnd = edgeLines[edgeId]->start[1] + e_f_ValueL * (edgeLines[edgeId]->end[1] - edgeLines[edgeId]->start[1]);
	//				draw = true;
	//			}
	//			else if(e_f_ValueL < -err) {
	//				xDrawEnd = edgeLines[edgeId]->start[0];
	//				yDrawEnd = edgeLines[edgeId]->start[1];
	//				draw = true;
	//			}
	//			else if(e_f_ValueL - 1 > err && f_e_ValueL < err) {
	//				xDrawEnd = edgeLines[edgeId]->start[0];
	//				yDrawEnd = edgeLines[edgeId]->start[1];
	//				draw = true;
	//			}
	//		}

	//		if (draw) {
	//			if (currentCell->edges[edgeId]->opaque) {
	//				float subangle = 90, focal_dist = float(240.0/90);
	//				if (MazeWidget::maze->viewer_fov == 120)
	//					focal_dist = 2;

	//				drawVertex[0]->
	//					posn[Vertex::X] = 1 * ((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
	//					focal_dist /
	//					((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[0]->
	//					posn[Vertex::Y] = 1 * focal_dist /
	//					((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[1]->posn[Vertex::X] = drawVertex[0]->posn[Vertex::X];
	//				drawVertex[1]->posn[Vertex::Y] = -drawVertex[0]->posn[Vertex::Y];

	//				drawVertex[2]->
	//					posn[Vertex::X] = 1 * ((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
	//					focal_dist /
	//					((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[2]->
	//					posn[Vertex::Y] = -1 * focal_dist /
	//					((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[3]->posn[Vertex::X] = drawVertex[2]->posn[Vertex::X];
	//				drawVertex[3]->posn[Vertex::Y] = -drawVertex[2]->posn[Vertex::Y];
	//				
	//				float* point1 = new float[4];
	//				float* point2 = new float[4];
	//				float* point3 = new float[4];
	//				float* point4 = new float[4];
	//				
	//				point1[0] = yDrawStart;
	//				point1[1] = wallH;
	//				point1[2] = xDrawStart;
	//				point1[3] = 1;
	//				point2[0] = yDrawStart;
	//				point2[1] = -wallH;
	//				point2[2] = xDrawStart;
	//				point2[3] = 1;
	//				point3[0] = yDrawEnd;
	//				point3[1] = wallH;
	//				point3[2] = xDrawEnd;
	//				point3[3] = 1;
	//				point4[0] = yDrawEnd;
	//				point4[1] = -wallH;
	//				point4[2] = xDrawEnd;
	//				point4[3] = 1;
	//				float* pt1 = multMat(mvpMat, point1, 4, 4, 1);
	//				float* pt2 = multMat(mvpMat, point2, 4, 4, 1);
	//				float* pt3 = multMat(mvpMat, point3, 4, 4, 1);
	//				float* pt4 = multMat(mvpMat, point4, 4, 4, 1);

	//				/*for (int i = 0; i < 4; i++) {
	//					std::cout << pt1[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt2[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt3[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt4[i] << " ";
	//				}
	//				std::cout << std::endl;*/

	//				pt1 = NDC(pt1);
	//				pt2 = NDC(pt2);
	//				pt3 = NDC(pt3);
	//				pt4 = NDC(pt4);

	//				/*for (int i = 0; i < 4; i++) {
	//					std::cout << pt1[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt2[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt3[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt4[i] << " ";
	//				}
	//				std::cout << std::endl;*/

	//				/*std::cout << drawVertex[0]->posn[0] << " " << drawVertex[1]->posn[0] << std::endl;
	//				std::cout << drawVertex[1]->posn[0] << " " << drawVertex[1]->posn[1] << std::endl;
	//				std::cout << drawVertex[2]->posn[0] << " " << drawVertex[2]->posn[1] << std::endl;
	//				std::cout << drawVertex[3]->posn[0] << " " << drawVertex[3]->posn[1] << std::endl;*/


	//				glBegin(GL_QUADS);

	//				glColor3f(currentCell->edges[edgeId]->color[0], currentCell->edges[edgeId]->color[1], currentCell->edges[edgeId]->color[2]);
	//				if (pt1[1] < pt2[1])
	//					std::swap(pt1, pt2);
	//				if (pt3[1] < pt4[1])
	//					std::swap(pt3, pt4);
	//				if (pt1[0] < pt3[0]) {
	//					std::swap(pt1, pt3);
	//					std::swap(pt2, pt4);
	//				}
	//				glVertex2f(pt1[0], pt1[1]);
	//				glVertex2f(pt2[0], pt2[1]);
	//				glVertex2f(pt4[0], pt4[1]);
	//				glVertex2f(pt3[0], pt3[1]);
	//				

	//				glEnd();
	//				draw = false;
	//			}
	//			else {
	//				Vector3 a(xDrawStart - MazeWidget::maze->viewer_posn[0], 0, yDrawStart - MazeWidget::maze->viewer_posn[1]);
	//				Vector3 b(xDrawEnd - MazeWidget::maze->viewer_posn[0], 0, yDrawEnd - MazeWidget::maze->viewer_posn[1]);
	//				Vector3 c = cross(a, b);
	//				Frustum* newF;
	//				if (c.y < 0) {
	//					newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
	//						xDrawStart, yDrawStart,
	//						xDrawEnd, yDrawEnd);
	//				}
	//				else {
	//					newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
	//						xDrawEnd, yDrawEnd,
	//						xDrawStart, yDrawStart);
	//				}
	//				Cell* newCell = currentCell->edges[edgeId]->Neighbor(currentCell);
	//				draw = false;
	//				if (!newCell->isDrawn)
	//					My_Draw_Cell(newCell, newF);
	//			}
	//		}
	//	}

	//	else if (f_e_ValueL > -err) {
	//		if (e_f_ValueL >= -err && e_f_ValueL - 1 <= 0.000001) {// 視線與牆有交點
	//			xDrawEnd = edgeLines[edgeId]->start[0] + e_f_ValueL * (edgeLines[edgeId]->end[0] - edgeLines[edgeId]->start[0]);
	//			yDrawEnd = edgeLines[edgeId]->start[1] + e_f_ValueL * (edgeLines[edgeId]->end[1] - edgeLines[edgeId]->start[1]);

	//			if (side == Edge::LEFT) {
	//				//位置在牆的左邊 取牆的終點
	//				xDrawStart = edgeLines[edgeId]->start[0];
	//				yDrawStart = edgeLines[edgeId]->start[1];
	//				draw = true;
	//			}
	//			else if (side == Edge::RIGHT) {
	//				//位置在牆的右邊 取牆的起點
	//				xDrawStart = edgeLines[edgeId]->end[0];
	//				yDrawStart = edgeLines[edgeId]->end[1];
	//				draw = true;
	//			}
	//		}
	//		else if (side == Edge::LEFT && e_f_ValueL - 1 > err) {
	//			//位置在牆的左邊 取牆的終點
	//			xDrawEnd = edgeLines[edgeId]->end[0];
	//			yDrawEnd = edgeLines[edgeId]->end[1];
	//			xDrawStart = edgeLines[edgeId]->start[0];
	//			yDrawStart = edgeLines[edgeId]->start[1];
	//			draw = true;
	//		}
	//		else if (side == Edge::RIGHT && e_f_ValueL < err) {
	//			//位置在牆的右邊 取牆的起點
	//			xDrawEnd = edgeLines[edgeId]->start[0];
	//			yDrawEnd = edgeLines[edgeId]->start[1];
	//			xDrawStart = edgeLines[edgeId]->end[0];
	//			yDrawStart = edgeLines[edgeId]->end[1];
	//			draw = true;
	//		}
	//		if (draw) {
	//			if (currentCell->edges[edgeId]->opaque) {
	//				float subangle = 90, focal_dist = float(240.0 / 90);
	//				if (MazeWidget::maze->viewer_fov == 120)
	//					focal_dist = 2;

	//				drawVertex[0]->
	//					posn[Vertex::X] = 1 * ((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
	//					focal_dist /
	//					((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[0]->
	//					posn[Vertex::Y] = 1 * focal_dist /
	//					((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[3]->posn[Vertex::X] = drawVertex[0]->posn[Vertex::X];
	//				drawVertex[3]->posn[Vertex::Y] = -drawVertex[0]->posn[Vertex::Y];

	//				drawVertex[2]->
	//					posn[Vertex::X] = 1 * ((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
	//					focal_dist /
	//					((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[2]->
	//					posn[Vertex::Y] = -1 * focal_dist /
	//					((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
	//						sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
	//						(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
	//						cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

	//				drawVertex[1]->posn[Vertex::X] = drawVertex[2]->posn[Vertex::X];
	//				drawVertex[1]->posn[Vertex::Y] = -drawVertex[2]->posn[Vertex::Y];

	//				float* point1 = new float[4];
	//				float* point2 = new float[4];
	//				float* point3 = new float[4];
	//				float* point4 = new float[4];
	//				point1[0] = yDrawStart;
	//				point1[1] = wallH;
	//				point1[2] = xDrawStart;
	//				point1[3] = 1;
	//				point2[0] = yDrawStart;
	//				point2[1] = -wallH;
	//				point2[2] = xDrawStart;
	//				point2[3] = 1;
	//				point3[0] = yDrawEnd;
	//				point3[1] = wallH;
	//				point3[2] = xDrawEnd;
	//				point3[3] = 1;
	//				point4[0] = yDrawEnd;
	//				point4[1] = -wallH;
	//				point4[2] = xDrawEnd;
	//				point4[3] = 1;
	//				float* pt1 = multMat(mvpMat, point1, 4, 4, 1);
	//				float* pt2 = multMat(mvpMat, point2, 4, 4, 1);
	//				float* pt3 = multMat(mvpMat, point3, 4, 4, 1);
	//				float* pt4 = multMat(mvpMat, point4, 4, 4, 1);
	//				/*for (int i = 0; i < 4; i++) {
	//					std::cout << pt1[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt2[i] << " ";
	//				}*/
	//				pt1 = NDC(pt1);
	//				pt2 = NDC(pt2);
	//				pt3 = NDC(pt3);
	//				pt4 = NDC(pt4);

	//				glBegin(GL_QUADS);

	//				glColor3f(currentCell->edges[edgeId]->color[0], currentCell->edges[edgeId]->color[1], currentCell->edges[edgeId]->color[2]);
	//				/*glVertex2f(drawVertex[0]->posn[0], drawVertex[0]->posn[1]);
	//				glVertex2f(drawVertex[1]->posn[0], drawVertex[1]->posn[1]);
	//				glVertex2f(drawVertex[2]->posn[0], drawVertex[2]->posn[1]);
	//				glVertex2f(drawVertex[3]->posn[0], drawVertex[3]->posn[1]);*/
	//				/*for (int i = 0; i < 4; i++) {
	//					std::cout << pt1[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt2[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt3[i] << " ";
	//				}
	//				std::cout << std::endl;
	//				for (int i = 0; i < 4; i++) {
	//					std::cout << pt4[i] << " ";
	//				}
	//				std::cout << std::endl;*/
	//				if (pt1[1] < pt2[1])
	//					std::swap(pt1, pt2);
	//				if (pt3[1] < pt4[1])
	//					std::swap(pt3, pt4);
	//				if (pt1[0] < pt3[0]) {
	//					std::swap(pt1, pt3);
	//					std::swap(pt2, pt4);
	//				}
	//				glVertex2f(pt1[0], pt1[1]);
	//				glVertex2f(pt2[0], pt2[1]);
	//				glVertex2f(pt4[0], pt4[1]);
	//				glVertex2f(pt3[0], pt3[1]);

	//				glEnd();
	//				draw = false;
	//			}
	//			else {
	//				Vector3 a(xDrawStart - MazeWidget::maze->viewer_posn[0], 0, yDrawStart - MazeWidget::maze->viewer_posn[1]);
	//				Vector3 b(xDrawEnd - MazeWidget::maze->viewer_posn[0], 0, yDrawEnd - MazeWidget::maze->viewer_posn[1]);
	//				Vector3 c = cross(a, b);
	//				Frustum* newF;
	//				if (c.y < 0) {
	//					newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
	//						xDrawStart, yDrawStart,
	//						xDrawEnd, yDrawEnd);
	//				}
	//				else {
	//					newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
	//						xDrawEnd, yDrawEnd,
	//						xDrawStart, yDrawStart);
	//				}
	//				Cell* newCell = currentCell->edges[edgeId]->Neighbor(currentCell);
	//				draw = false;
	//				if (!newCell->isDrawn)
	//					My_Draw_Cell(newCell, newF);
	//			}
	//		}
	//	}
	//}
}

void OpenGLWidget::DrawWall(float xDrawStart, float yDrawStart, float xDrawEnd, float yDrawEnd, float color[3])
{
	float* point1 = new float[4];
	float* point2 = new float[4];
	float* point3 = new float[4];
	float* point4 = new float[4];
	point1[0] = yDrawStart;
	point1[1] = wallH;
	point1[2] = xDrawStart;
	point1[3] = 1;
	point2[0] = yDrawStart;
	point2[1] = -wallH;
	point2[2] = xDrawStart;
	point2[3] = 1;
	point3[0] = yDrawEnd;
	point3[1] = wallH;
	point3[2] = xDrawEnd;
	point3[3] = 1;
	point4[0] = yDrawEnd;
	point4[1] = -wallH;
	point4[2] = xDrawEnd;
	point4[3] = 1;
	float* pt1 = multMat(OpenGLWidget::mvpMat, point1, 4, 4, 1);
	float* pt2 = multMat(mvpMat, point2, 4, 4, 1);
	float* pt3 = multMat(mvpMat, point3, 4, 4, 1);
	float* pt4 = multMat(mvpMat, point4, 4, 4, 1);
	pt1 = NDC(pt1);
	pt2 = NDC(pt2);
	pt3 = NDC(pt3);
	pt4 = NDC(pt4);
	glBegin(GL_QUADS);

	glColor3f(color[0], color[1], color[2]);
	if (pt1[1] < pt2[1])
			std::swap(pt1, pt2);
	if (pt3[1] < pt4[1])
		std::swap(pt3, pt4);
	if (pt1[0] < pt3[0]) {
		std::swap(pt1, pt3);
		std::swap(pt2, pt4);
	}
	glVertex2f(pt1[0], pt1[1]);
	glVertex2f(pt2[0], pt2[1]);
	glVertex2f(pt4[0], pt4[1]);
	glVertex2f(pt3[0], pt3[1]);

	glEnd();
}

void OpenGLWidget::CellUndraw()
{
	for (int i = 0; i < MazeWidget::maze->num_cells; i++)
		MazeWidget::maze->cells[i]->isDrawn = false;
}

void OpenGLWidget::loadTexture2D(QString str, GLuint &textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}
float OpenGLWidget::degree_change(float num)
{
	return num / 180.0f * 3.14159f;
}

float* OpenGLWidget::NDC(float* Vec)
{
	float* n = new float[4]();
	n[0] = Vec[0] / Vec[3];
	n[1] = Vec[1] / Vec[3];
	n[2] = Vec[2] / Vec[3];
	n[3] = Vec[3] / Vec[3];
	return n;
}

void OpenGLWidget::Print44Mat(float Mat[16])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			std::cout << Mat[i * 4 + j] << " ";
		}
		std::cout << std::endl;
	}
}

void OpenGLWidget::MyLookAt(Vector3 eye, Vector3 center, Vector3 up)
{
	Vector3 forward = center - eye;
	forward.normalize();
	Vector3 side = cross(forward, up);
	side.normalize();
	up = cross(side, forward);
	float m2[16] = { 1, 0, 0, -eye.x,
					 0, 1, 0, -eye.y,
					 0, 0, 1, -eye.z,
					 0, 0, 0,	   1 };
	float m1[16] = { side.x, side.y, side.z, 0,
					 up.x, up.y, up.z, 0,
					 -forward.x, -forward.y, -forward.z, 0,
					 0,		    0,			0, 1 };

	m1[3] = m1[0] * -eye.x + m1[1] * -eye.y + m1[2] * -eye.z + m1[3];
	m1[7] = m1[4] * -eye.x + m1[5] * -eye.y + m1[6] * -eye.z + m1[7];
	m1[11] = m1[8] * -eye.x + m1[9] * -eye.y + m1[10] * -eye.z + m1[11];

	mvMat = new float[16]();

	memcpy(mvMat, m1, 16 * sizeof(float));;
}

void OpenGLWidget::MyPerspective(double fovy, double aspect, double zNear, double zFar)
{
	/*static float c = M_PI / 180.0f;
	double d = 1 / tan(fovy*c / 2);
	double A = -(zFar + zNear) / (zFar - zNear);
	double B = -2 * zFar*zNear / (zFar - zNear);
	float tmp[16] = { d / aspect, 0, 0, 0,
	0, d, 0, 0,
	0,0,A,B,
	0,0,-1,0 };
	pMat = tmp;*/
	pMat = new float[16]();
	float ymax, xmax;
	ymax = zNear * tanf(fovy * M_PI / 360.0);
	xmax = ymax * aspect;
	float tmp[16] = { 0, 0, 0, 0,
					  0, 0, 0, 0,
					  0, 0, 0, 0,
					  0, 0, 0, 0 };
	float temp, temp2, temp3, temp4;
	float right = xmax, left = -xmax, bottom = -ymax, top = ymax;
	temp = 2.0 * zNear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zFar - zNear;
	pMat[0] = temp / temp2;
	pMat[1] = 0.0;
	pMat[2] = (right + left) / temp2;
	pMat[3] = 0.0;
	pMat[4] = 0.0;
	pMat[5] = temp / temp3;
	pMat[6] = (top + bottom) / temp3;
	pMat[7] = 0.0;
	pMat[8] = 0.0;
	pMat[9] = 0.0;
	pMat[10] = (-zFar - zNear) / temp4;
	pMat[11] = (-temp * zFar) / temp4;
	pMat[12] = 0.0;
	pMat[13] = 0.0;
	pMat[14] = -1.0;
	pMat[15] = 0.0;
}

void OpenGLWidget::MyNDC(double fovy, double aspect, double zNear, double zFar)
{
	NDCMat = new float[16]();
	float ymax, xmax;
	ymax = zNear * tanf(fovy * M_PI / 360.0);
	xmax = ymax * aspect;
	float right = xmax, left = -xmax, bottom = -ymax, top = ymax;
	NDCMat[0] = 2 * zNear / (right - left);
	NDCMat[2] = (right + left) / (right - left);
	NDCMat[5] = 2 * zNear / (top - bottom);
	NDCMat[6] = (top + bottom) / (top - bottom);
	NDCMat[10] = -(zFar + zNear) / (zFar - zNear);
	NDCMat[11] = -2 * zFar*zNear / (zFar - zNear);
	NDCMat[14] = -1;
}

void OpenGLWidget::Draw_Cell(Cell * nextCell, Frustum* f, float focal_dist, int iteration)
{
	int e, frustumEdge, side, isgood, edgeCount, c;

	Frustum* newF;
	Cell* newCell;

	LineSeg *edgeLines[4];

	float t, t1, xDrawEnd, yDrawEnd, xDrawStart, yDrawStart, subangle;

	Vertex* drawVertex[4];

	nextCell->isDrawn = true;

	/* convert the edges to lines */
	for (int i = 0; i < 4; i++) {
		edgeLines[i] = new LineSeg(nextCell->edges[i]);
		drawVertex[i] = new Vertex();
	}

	e = 0;
	c = 0;
	frustumEdge = 0;
	edgeCount = 3;

	while (frustumEdge < 2) {
		float test;
		if (((test = (f->edgelines[frustumEdge]).Cross_Param(*edgeLines[e])) >= 0) || (frustumEdge == 1)) { //does frustum face this edge?
			t = edgeLines[e]->Cross_Param(f->edgelines[frustumEdge]);

			isgood = ((t >= 0) && (t <= 1)) ? 1 : 0;//實際上相交

			//check for the recursive call... if this is the point the frustum enters on, then it's no good
			if (isgood == 1) {
				xDrawStart = edgeLines[e]->start[0] + t * (edgeLines[e]->end[0] - edgeLines[e]->start[0]);
				yDrawStart = edgeLines[e]->start[1] + t * (edgeLines[e]->end[1] - edgeLines[e]->start[1]);
				if (!(nextCell->Point_In_Cell(f->edgelines[0].start[0], f->edgelines[0].start[1])))
					//不在cell內 -> 遞迴
					if ((f->edgelines[frustumEdge].end[0] == xDrawStart) && (f->edgelines[frustumEdge].end[1] == yDrawStart)) {
						//f終點 = 交點
						for (int i = 0; i < edgeCount; i++) {
							t1 = edgeLines[(e + i + 1) % 4]->Cross_Param(f->edgelines[frustumEdge]);//其他牆跟視線cross param
							if ((t1 >= 0) && (t1 <= 1))
								isgood = 0;
						}
						edgeCount--;
					}
			}

			if ((isgood == 1) || (frustumEdge == 1)) {
				//有交叉 or 整面牆在視線範圍內
				if ((isgood == 1)) { //有實際相交

					if ((side = nextCell->edges[e]->Point_Side(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y])) == Edge::LEFT) {
						//位置在牆的左邊 取牆的終點
						xDrawEnd = (frustumEdge == 0) ? edgeLines[e]->end[0] : edgeLines[e]->start[0];
						yDrawEnd = (frustumEdge == 0) ? edgeLines[e]->end[1] : edgeLines[e]->start[1];
					}
					else {
						//位置在牆的右邊 取牆的起點
						xDrawEnd = (frustumEdge == 0) ? edgeLines[e]->start[0] : edgeLines[e]->end[0];
						yDrawEnd = (frustumEdge == 0) ? edgeLines[e]->start[1] : edgeLines[e]->end[1];
					}

					if (frustumEdge == 0) {
						t = edgeLines[e]->Cross_Param(f->edgelines[1]);
						test = f->edgelines[1].Cross_Param(*edgeLines[e]);
						if ((t > 0) && (t < 1) && (test > 0)) { //視線左界與目前牆實際相交
							xDrawEnd = edgeLines[e]->start[0] + t * (edgeLines[e]->end[0] - edgeLines[e]->start[0]);
							yDrawEnd = edgeLines[e]->start[1] + t * (edgeLines[e]->end[1] - edgeLines[e]->start[1]);
						}
					}

				}

				else {
					//整面牆在視線範圍內
					if ((side = nextCell->edges[e]->Point_Side(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y])) == Edge::LEFT) {
						//在牆的左邊
						xDrawStart = edgeLines[e]->start[0];
						yDrawStart = edgeLines[e]->start[1];
						xDrawEnd = edgeLines[e]->end[0];
						yDrawEnd = edgeLines[e]->end[1];
					}
					else {
						//在牆的右邊
						xDrawStart = edgeLines[e]->end[0];
						yDrawStart = edgeLines[e]->end[1];
						xDrawEnd = edgeLines[e]->start[0];
						yDrawEnd = edgeLines[e]->start[1];
					}
				}

				//----------------------------------------------------------------
				if (nextCell->edges[e]->opaque) {

					subangle = 90;

					drawVertex[0]->
						posn[Vertex::X] = 1 * ((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
							(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
						focal_dist /
						((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
							(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

					drawVertex[0]->
						posn[Vertex::Y] = 1 * focal_dist /
						((xDrawEnd - MazeWidget::maze->viewer_posn[0]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
							(yDrawEnd - MazeWidget::maze->viewer_posn[1]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

					//!isgood -> frustumEdge == 1
					drawVertex[((frustumEdge == 0) || !isgood) ? 1 : 3]->posn[Vertex::X] = drawVertex[0]->posn[Vertex::X];
					drawVertex[((frustumEdge == 0) || !isgood) ? 1 : 3]->posn[Vertex::Y] = -drawVertex[0]->posn[Vertex::Y];

					drawVertex[2]->
						posn[Vertex::X] = 1 * ((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) -
							(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle)))) *
						focal_dist /
						((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
							(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

					drawVertex[2]->
						posn[Vertex::Y] = -1 * focal_dist /
						((xDrawStart - MazeWidget::maze->viewer_posn[0]) *
							sin(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))) +
							(yDrawStart - MazeWidget::maze->viewer_posn[1]) *
							cos(Maze::To_Radians(-(MazeWidget::maze->viewer_dir - subangle))));

					drawVertex[((frustumEdge == 0) || !isgood) ? 3 : 1]->posn[Vertex::X] = drawVertex[2]->posn[Vertex::X];
					drawVertex[((frustumEdge == 0) || !isgood) ? 3 : 1]->posn[Vertex::Y] = -drawVertex[2]->posn[Vertex::Y];

					glBegin(GL_QUADS);

					glColor3f(nextCell->edges[e]->color[0], nextCell->edges[e]->color[1], nextCell->edges[e]->color[2]);
					glVertex2f(drawVertex[0]->posn[0], drawVertex[0]->posn[1]);
					glVertex2f(drawVertex[1]->posn[0], drawVertex[1]->posn[1]);
					glVertex2f(drawVertex[2]->posn[0], drawVertex[2]->posn[1]);
					glVertex2f(drawVertex[3]->posn[0], drawVertex[3]->posn[1]);

					glEnd();

				}

				//----------------------------------------------------------------
				else {

					if ((frustumEdge == 0) || !isgood) {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							xDrawStart, yDrawStart,
							xDrawEnd, yDrawEnd);
					}
					else {
						newF = new Frustum(MazeWidget::maze->viewer_posn[0], MazeWidget::maze->viewer_posn[1],
							xDrawEnd, yDrawEnd,
							xDrawStart, yDrawStart);
					}

					newCell = nextCell->edges[e]->Neighbor(nextCell);

					if (!newCell->isDrawn)
						Draw_Cell(newCell, newF, focal_dist, iteration + 1);
				}

				//----------------------------------------------------------------

				if (frustumEdge == 1)
					frustumEdge++;
				if (isgood) {
					frustumEdge++;
					edgeCount = 0;
				}
				if (frustumEdge == 1) {
					t = edgeLines[e]->Cross_Param(f->edgelines[frustumEdge]);
					test = f->edgelines[frustumEdge].Cross_Param(*edgeLines[e]);
					if ((t >= 0) && (t <= 1) && (test > 0)) {
						frustumEdge++;
					}
				}
			}
		}
		e = (e + 1) % 4;
		c++;
		if (c == 4 && frustumEdge == 0 && isgood != 1) {
			c = 0;
			frustumEdge += 2;
		}
	}
}

Vector3::Vector3()
{
	x = y = z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::normalize()
{
	float norm = sqrt(x * x + y * y + z * z);
	x /= norm;
	y /= norm;
	z /= norm;
}

Vector3 Vector3::operator+(const Vector3 & vec)
{
	return Vector3(x + vec.x, y + vec.y, z + vec.z);
}

Vector3 Vector3::operator-(const Vector3 & vec)
{
	return Vector3(x - vec.x, y - vec.y, z - vec.z);
}

float Vector3::operator*(const Vector3 &vecB)
{
	return x * vecB.x + y * vecB.y + z * vecB.z;
}

Vector3 Vector3::operator*(const float & scale)
{
	return Vector3(x * scale, y * scale, z * scale);
}

bool Vector3::operator!=(const Vector3 &vecB)
{
	if (x == vecB.x && y == vecB.y && z == vecB.z)
		return false;
	else
		return true;
}

/*Vector3 Vector3::cross(Vector3 a, Vector3 b)
{
	float cX = a.y * b.z - a.z * b.y;
	float cY = a.z * b.x - a.x * b.z;
	float cZ = a.x * b.y - a.y * b.x;
	return Vector3(cX, cY, cZ);
}*/
Vector3 cross(Vector3 a, Vector3 b) {
	float cX = a.y * b.z - a.z * b.y;
	float cY = a.z * b.x - a.x * b.z;
	float cZ = a.x * b.y - a.y * b.x;
	return Vector3(cX, cY, cZ);
}

float* multMat(float* a, float* b, int m, int p, int n) {
	float* c = new float[m * n];
	for (int i = 0; i < m * n; i++)
		c[i] = 0;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < p; k++)
				c[i * n + j] += a[i * p + k] * b[k * n + j];
	return c;
}
