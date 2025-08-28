#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		10, 10, 30,
		0, 0, 0,
		0, 1, 0
	);

	DrawAxes(50);

	/*DrawChest(10, 5, 10, 2, alfa, 10, 45);*/
	DrawSnake(5, 0.5, 2, 20, alfa);


	glFlush();
	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)w/(double)h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes(double a)
{

	glBegin(GL_LINES);

		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, a, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, a);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(a, 0, 0);
	glEnd();
}

void CGLRenderer::LookAt(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz)
{

	double dx = centerx - eyex;
	double dy = centery - eyey;
	double dz = centerz - eyez;
	double r = sqrt(dx * dx + dz * dz);

	double r2 = sqrt(r * r + dy * dy);

	double fiRad = acos(dz/r);
	double tetaRad = acos(dy / r2);
	/*double fiRad = atan2(dx, dz);
	double tetaRad = atan2(dy, r);*/

	double fi = (180 * fiRad) / 3.14;
	double teta = (180 * tetaRad) / 3.14;


	double axisX = dx;
	double axisY = dy;
	double axisZ = dz;

	/*glTranslatef(eyex, eyey, eyez);
	glRotatef(fi, axisX, axisY, axisZ);
	glTranslatef(-eyex, -eyey, -eyez);

	glTranslatef(centerx, centery, centerz);*/

	glRotated(-fi, 0, 1, 0);
	//glRotated(teta, 1, 0, 1);
	glTranslated(-eyex, -eyey, -eyez);
}

void CGLRenderer::DrawPrism(double x, double y, double z)
{

	glBegin(GL_QUADS);

	glVertex3f(0, y, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(x, 0, 0);
	glVertex3f(x, y, 0);

	glVertex3f(x, y, 0);
	glVertex3f(x, 0, 0);
	glVertex3f(x, 0, z);
	glVertex3f(x, y, z);

	glVertex3f(x, y, z);
	glVertex3f(x, 0, z);
	glVertex3f(0, 0, z);
	glVertex3f(0, y, z);

	glVertex3f(0, y, z);
	glVertex3f(0, 0, z);
	glVertex3f(0, 0, 0);
	glVertex3f(0, y, 0);

	glVertex3f(0, y, z);
	glVertex3f(0, y, 0);
	glVertex3f(x, y, 0);
	glVertex3f(x, y, z);

	glVertex3f(0, 0, z);
	glVertex3f(0, 0, 0);
	glVertex3f(x, 0, 0);
	glVertex3f(x, 0, z);
	
	glEnd();



}

void CGLRenderer::DrawChest(double x, double y, double z, double d, double alfa, double beta, double gama)
{

	double dz = z - 2 * d;
	double dx = x - 2 * d;

	DrawPrism(x, d, z);


	glPushMatrix();

	glTranslatef(0, d, 0);

	glColor3f(1, 0, 0);
	DrawPrism(x, y, d);

	glTranslatef(0, 0, d);
	glColor3f(1, 1, 0);
	DrawPrism(d, y, dz);

	glPushMatrix();
	glTranslatef(d+dx, 0, 0);
	glColor3f(1, 0, 1);
	DrawPrism(d, y, dz);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, dz);
	glColor3f(0, 1, 0);
	DrawPrism(x, y, d);
	glPopMatrix();

	glPopMatrix();


	glPushMatrix();

	glTranslatef(0, y+d, 0);

	glRotatef(alfa, 1, 0, 0);

	glColor3f(0.5, 0.5, 1);
	DrawPrism(x, d, z);

	glTranslatef(0, d, 0);

	glTranslatef(0, 0, 0.8 * z);
	glTranslatef(0.3 * x, 0, 0);
	DrawRucka(x, y, z, beta);
	glTranslatef(0.5*x, 0, 0);
	DrawRucka(x, y, z, gama);


	glPopMatrix();



}

void CGLRenderer::DrawRucka(double x, double y, double z, double fi)
{

	glColor3f(0, 1, 0);
	glPushMatrix();
	glRotatef(fi, 0, 1, 0);
	glTranslatef(-0.04 * x/2, 0, -0.04 * x/2);
	DrawPrism(0.04*x, 0.1*x, 0.04*x);


	glTranslatef((- 0.2 * x / 2) + 0.04 * x/2, 0.1 * x, 0);
	DrawPrism(0.2*x, 0.04*x, 0.04*x);


	glPopMatrix();
}

void CGLRenderer::DrawSnake(double x, double y, double z, double nPart, double alfa)
{
	double h = sin(alfa) / x;

	glTranslatef(0, y, 0);
	glRotatef(alfa, 0, 0, 1);
	glTranslatef(0, -y, 0);

	DrawPrism(x, y, z);
	glTranslatef(x, 0, 0);

	for (int i = 1; i < nPart; i++) {
		if (i % 2 == 0){
			glTranslatef(0, y, 0);
			glRotatef(2 * alfa, 0, 0, 1);
			glTranslatef(0, -y, 0);
		}
		else {
			double d = -(2) * alfa;
			glRotatef(d, 0, 0, 1);
		}
		DrawPrism(x, y, z);

		glTranslatef(x, 0, 0);
	}
}
