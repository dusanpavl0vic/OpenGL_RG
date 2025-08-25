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
	glClearColor(1, 1, 1, 1);

	brick = LoadTexture("assets/brick.png");
	env = LoadTexture("assets/env.png");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	double r = 30;
	double x = r * cos(phi) * cos(theta);
	double y = r * sin(phi);
	double z = r * cos(phi) * sin(theta);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
	glTranslated(0, -5, 0);
	
	DrawAxes();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glColor3f(1, 1, 1);
	DrawCube(100);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	float col[3] = {1, 1, 1};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col);

	float pos[4] = {1, 1, 0, 0};
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	//float col[3] = {1, 1, 1};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, col);

	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brick);
	DrawTube(2.5, 3.5, 10, 32);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_NORMALIZE);
	glTranslated(0, 10 , 0);
	DrawCone(3.8, 2, 32);
	glDisable(GL_NORMALIZE);

	glPushMatrix();
	glRotated(q, 0, 0, -1);
	glTranslated(0, 0, 3.8);
	DrawPaddle(8, 1.5);
	glRotated(90, 0, 0, -1);
	DrawPaddle(8, 1.5);
	glRotated(90, 0, 0, -1);
	DrawPaddle(8, 1.5);
	glRotated(90, 0, 0, -1);
	DrawPaddle(8, 1.5);
	glPopMatrix();


	glFlush();
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
	gluPerspective(40, aspect, 0.1, 2000);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &brick);
	glDeleteTextures(1, &env);
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

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);

	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT id;

	DImage img;
	bool res = img.Load(CString(fileName));

	if (res == false) {
		throw;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glGenTextures(1, &id);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	glDisable(GL_TEXTURE_2D);

	return id;
}

void CGLRenderer::DrawQuad(double a, double x, double y)
{
	double s = a / 2.;

	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);

	glTexCoord2f(x, y + 0.33);
	glVertex3f(-s, -s, s);

	glTexCoord2f(x + .25, y + .33);
	glVertex3f(s, -s, s);

	glTexCoord2f(x + .25, y);
	glVertex3f(s, s, s);

	glTexCoord2f(x, y);
	glVertex3f(-s, s, s);

	glEnd();
}

void CGLRenderer::DrawCube(double a)
{
	glPushMatrix();
	glRotated(-90, 0, 0, 1);
	glRotated(90, 1, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, env);
	DrawQuad(a, 0, 0.33);
	glRotated(90, 0, 1, 0);
	DrawQuad(a, 0.25, 0.33);
	glRotated(90, 0, 1, 0);
	DrawQuad(a, 0.5, 0.33);
	glRotated(90, 0, 1, 0);
	DrawQuad(a, 0.75, 0.33);

	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	DrawQuad(a, 0.25, 0);
	glPopMatrix();

	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	DrawQuad(a, 0.25, 0.66);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{
	double alpha = 2 * PI;
	double deltaAlpha = 2 * PI / n;

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= n; i++) {
		double x1 = r1 * cos(alpha);
		double z1 = r1 * sin(alpha);

		double x2 = r2 * cos(alpha);
		double z2 = r2 * sin(alpha);

		double u = alpha / (2 * PI);

		glTexCoord2f(u, 0);
		glVertex3f(x1, h, z1);
		glTexCoord2f(u, 1);
		glVertex3f(x2, 0, z2);

		alpha -= deltaAlpha;
	}

	glEnd();
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double alpha = 2 * PI;
	double deltaAlpha = 2 * PI / n;

	glBegin(GL_TRIANGLE_FAN);

	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);
	for (int i = 0; i <= n; i++) {
		double x = r * cos(alpha);
		double z = r * sin(alpha);

		//double u = alpha / (2 * PI);
		glNormal3f(x, 0, z);
		glVertex3f(x, 0, z);

		alpha -= deltaAlpha;
	}

	glEnd();

}

void CGLRenderer::DrawPaddle(double length, double width)
{
	double lpart = length / 8;
	double wpart = width / 8;

	glNormal3f(0, 0, 1);

	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -wpart, 0);
	glVertex3f(2*lpart, -wpart, 0);
	glVertex3f(2*lpart, 0, 0);

	glVertex3f(2*lpart, 0, 0);
	glVertex3f(2*lpart, -width, 0);
	glVertex3f(length, -width, 0);
	glVertex3f(length, 0, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(lpart, -wpart, 0);
	glVertex3f(2*lpart, -width, 0);
	glVertex3f(2*lpart, -wpart, 0);
	glEnd();
}
