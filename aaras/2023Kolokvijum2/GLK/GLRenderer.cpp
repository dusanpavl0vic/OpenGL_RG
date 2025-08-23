#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	lamp = LoadTexture("assets/lamp.jpg");
	side = LoadTexture("assets/side.jpg");
	top = LoadTexture("assets/top.jpg");
	bot = LoadTexture("assets/bot.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double r = 50. + zoom;
	double x = r * cos(alpha) * cos(beta);
	double y = r * sin(alpha);
	double z = r * cos(alpha) * sin(beta);

	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0, 10, 0, 0, 1, 0);

	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	DrawEnvCube(100);
	DrawAxes();

	DrawLamp();
	
	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, w, h);

	double aspect = (double)w / (double)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, aspect, 0.01, 2000);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &lamp);
	glDeleteTextures(1, &side);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glBegin(GL_LINES);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50 , 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50 , 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0 , 50);

	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT id;
	DImage img;
	auto res = img.Load(CString(fileName));
	
	if (res == false) {
		throw;
	}

	glPixelStorei(GL_UNSIGNED_BYTE, 4);

	glGenTextures(1, &id);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	glDisable(GL_TEXTURE_2D);

	return id;
}

void CGLRenderer::DrawQuad(double a)
{
	double s = a / 2.;

	glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex3f(-s, -s, s);
		glTexCoord2f(1, 1);
		glVertex3f(s, -s, s);
		glTexCoord2f(1, 0);
		glVertex3f(s, s, s);
		glTexCoord2f(0, 0);
		glVertex3f(-s, s, s);
	glEnd();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glColor3f(1, 1, 1);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glTranslated(0, a/2, 0);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, side);

		DrawQuad(a);
		glRotated(90, 0, 1, 0);
		DrawQuad(a);
		glRotated(90, 0, 1, 0);
		DrawQuad(a);
		glRotated(90, 0, 1, 0);
		DrawQuad(a);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, top);
		glRotated(-90, 1, 0, 0);
		DrawQuad(a);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, bot);
		glRotated(90, 1, 0, 0);
		glRotated(90, 0, 0, 1);
		DrawQuad(a);
	glPopMatrix();

	glTranslated(0, -a/2, 0);

	glDisable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen)
{
	double phi = 0;
	double delta = -PI * 2 / nSeg;

	glColor3f(1, 1, 1);

	double bv, tv;
	if (texMode == 0) {
		bv = 0;
		tv = .5;
	}
	else {
		bv = .5;
		tv = 1;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lamp);

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double u = -phi / PI ;

		double x = r1 * cos(phi);
		double z = r1 * sin(phi);

		glTexCoord2f(u, bv);
		glVertex3d(x, h, z);

		x = r2 * cos(phi);
		z = r2 * sin(phi);

		glTexCoord2f(u, tv);
		glVertex3d(x, 0, z);


		phi += delta;
	}
	glEnd();

	if (bIsOpen) return;

	phi = 0;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(.5, .25);
	glVertex3f(0, h, 0);
	for (int i = 0; i <= nSeg; i++) {
		double x = r1 * cos(phi);
		double z = r1 * sin(phi);

		glTexCoord2f((x / r1 + 1) / 2, (z / r1 + 1) / 4);
		glVertex3f(x, h, z);

		phi += delta;
	}
	glEnd();

	phi = 0;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(.5, .25);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= nSeg; i++) {
		double x = r2 * cos(phi);
		double z = r2 * sin(phi);

		glTexCoord2f((x / r2 + 1) / 2, .5 - (z / r2 + 1) / 4);
		glVertex3f(x, 0, z);

		phi -= delta;
	}
	glEnd();
}

void CGLRenderer::DrawLampBase()
{
	DrawCylinder(7, 8, 2, 30, 1, false);
}

void CGLRenderer::DrawLampArm()
{
	glPushMatrix();
	glTranslated(0, 0, 1);
	glRotated(-90, 1, 0, 0);
	DrawCylinder(3, 3, 2, 30, 1, false);
	glPopMatrix();
	DrawCylinder(.9, .9, 15, 30, 1, true);
}

void CGLRenderer::DrawLampHead()
{
	// 2, 3 ili 6, a visine: 1, 2 ili 5
	glPushMatrix();
	glTranslated(0, 0, 1);
	glRotated(-90, 1, 0, 0);
	DrawCylinder(2, 2, 2, 30, 1, false);
	glPopMatrix();

	glTranslated(3, 2.5, 0);
	DrawCylinder(2, 3, 1, 30, 1, false);
	glTranslated(0, -5, 0);
	DrawCylinder(3, 3, 5, 30, 1, false);
	glTranslated(0, -5, 0);
	glDisable(GL_CULL_FACE);
	DrawCylinder(3, 6, 5, 30, 0, true);
	glEnable(GL_CULL_FACE);
}

void CGLRenderer::DrawLamp()
{
	//glPushMatrix();
	DrawLampBase();
	glRotated(-a1, 0, 0, 1);
	DrawLampArm();
	glTranslated(0, 15, 0);
	glRotated(-a2, 0, 0, 1);
	DrawLampArm();
	glTranslated(0, 15, 0);
	glRotated(-a3, 0, 0, 1);
	DrawLampHead();
}

