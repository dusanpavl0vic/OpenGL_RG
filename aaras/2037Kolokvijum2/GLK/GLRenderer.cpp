#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"

#include <cstdlib>
//#pragma comment(lib, "GL\\glut32.lib")

static double yofu(double u) {
	return 1 + sin(u * 2 * PI - PI/2);
}

static double yofv(double v) {
	return (1 + sin(v * 2 * PI - PI/2)) * .3;
}

static double calcy(double u, double v) {
	return yofu(u) * yofv(v);
}

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

	tiles = LoadTexture("assets/tiles.jpg");
	env = LoadTexture("assets/env.png");
	leaf = LoadTexture("assets/leaf.png");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	double r = 40;
	double x = r * cos(phi) * cos(theta);
	double y = r * sin(phi);
	double z = r * cos(phi) * sin(theta);

	gluLookAt(x, y, z, 0, 5, 0, 0, 1, 0);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	DrawAxes();

	float ambient[4] = {0.3, 0.3, 0.3, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	float position[4] = {1, 1, 1, 0};
	float diffuse[4] = {1, 1, 1, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	//glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

	//float position2[4] = {-10, -1, 1, 0};
	//glLightfv(GL_LIGHT2, GL_POSITION, position2);
	//glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);

	glDisable(GL_BLEND);
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, tiles);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	DrawQuad(30);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	DrawBase();

	DrawStem();

	glEnable(GL_LIGHTING);
	DrawHead();
	glDisable(GL_LIGHTING);




	//glColor3f(1, 1, 1);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, env);
	//DrawQuad(1);
	//glEnable(GL_TEXTURE_2D);
	
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

	glDeleteTextures(1, &tiles);
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

	if (img.Load(CString(fileName)) == false) {
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

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());

	glDisable(GL_TEXTURE_2D);
	return id;
}

void CGLRenderer::DrawQuad(double a)
{
	double s = a / 2;

	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex3f(-s, -s, 0);
	glTexCoord2f(1, 1);
	glVertex3f(s, -s, 0);
	glTexCoord2f(1, 0);
	glVertex3f(s, s, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-s, s, 0);

	glEnd();
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double L = sqrt(pow(r, 2) + pow(h, 2));
	double ny = r / L;

	glBegin(GL_TRIANGLE_FAN);

	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);

	double angle = 0;
	double delta = 2. * PI / (double)n;

	for (int i = 0; i <= n; i++) {
		double nx = cos(angle);
		double nz = sin(angle);

		glNormal3f(nx, ny, nz);
		glVertex3f(r * nx, 0, r * nz);

		angle -= delta;
	}

	glEnd();

}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int n)
{
	double angle = 0;
	double delta = 2. * PI / (double)n;

	glBegin(GL_TRIANGLE_STRIP);


	for (int i = 0; i <= n; i++) {
		double nx = cos(angle);
		double nz = sin(angle);

		glNormal3f(nx, 0, nz);

		glVertex3f(r2 * nx, h, r2 * nz);
		glVertex3f(r1 * nx, 0, r1 * nz);

		angle -= delta;
	}

	glEnd();
}

void CGLRenderer::DrawDiamond(double r1, double r2, double h, int n)
{
	double coneH = h / 3;

	glPushMatrix();
	DrawCylinder(r1, r2, h, n);
	glTranslated(0, h, 0);
	DrawCone(r2, coneH, n);
	glPopMatrix();
}

void CGLRenderer::DrawSphere(double r, int n)
{
	double delta = 2 * PI / n ;

	double x, y, z;

	double alpha = PI/2;
	for (int i = 0; i < n / 2; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);

		double beta = 0;
		for (int j = 0; j <= n; j++)
		{
			double nextAlpha = alpha - delta;

			x = r * cos(alpha) * cos(beta);
			y = r * sin(alpha);
			z = r * cos(alpha) * sin(beta);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);

			x = r * cos(nextAlpha) * cos(beta);
			y = r * sin(nextAlpha);
			z = r * cos(nextAlpha) * sin(beta);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);

			beta -= delta;
		}

		glEnd();

		alpha -= delta;
	}
}

void CGLRenderer::DrawCircle(double r, int n)
{
	double alpha = 0;
	double delta = 2 * PI / n;

	glNormal3f(0, 1, 0);

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= n; i++)
	{
		double x = r * cos(alpha);
		double z = r * sin(alpha);

		glVertex3f(x, 0, z);

		alpha += delta;
	}
	glEnd();
}
void CGLRenderer::DrawIciclesLayer(double angle, double s)
{
	std::srand(1261723);
	for (int i = 0; i < 10; i++) {
		glPushMatrix();

		glRotated(-60 * i, 0, 0, 1);
		glRotated(-angle, 1, 0, 0);
		DrawDiamond(.25 * s, .5 * s, 2, 5);

		glPopMatrix();
	}

}

void CGLRenderer::DrawIcicles()
{
	glPushMatrix();
	DrawIciclesLayer(30, 1);
	glRotated(30, 0, 1, 0);
	glRotated(30, 0, 0, 1);
	DrawIciclesLayer(30, 1);
	glPopMatrix();

	glPushMatrix();
	glRotated(15, 0, 0, 1);
	DrawIciclesLayer(60, .75);
	glPopMatrix();
}

void CGLRenderer::DrawHead()
{
	double r = 2;

	glTranslated(0, r, 0);

	//----------------------------------------------------
	float clr[4] = { .725, .949, 1, 1 };
	float emission[3] = { .725/2, .949/2, 1/2.};

	glMaterialfv(GL_FRONT, GL_AMBIENT, clr);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, clr);
	glMaterialf(GL_FRONT, GL_SHININESS, 1);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	glPushMatrix();
	glTranslated(0, 0, -r + .5);

	DrawIcicles();
	glPopMatrix();
	//----------------------------------------------------

	//float noEmission[4] = { .725/4, .949/4, 1/4.};
	//glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

	//glMaterialf(GL_FRONT, GL_SHININESS, 0);

	glDisable(GL_LIGHTING);

	glColor3f(.427, .8, .89);
	DrawSphere(r, 30);

	glPushMatrix();
	glTranslated(0, -r, 0);
	glScaled(1, .5, 1);
	DrawSphere(.75, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, r - .5);

	glRotated(90, 1, 0, 0);
	DrawCylinder(1.3, 1, .5, 30);
	glTranslated(0, .5, 0);
	DrawCylinder(1, 1, .5, 30);
	glTranslated(0, .5, 0);
	DrawCylinder(1, 1.5, 1, 30);
	glTranslated(0, 1, 0);
	DrawCylinder(1.5, 1.5, .1, 30);
	DrawCircle(1.5, 30);
	glColor3f(0, 0, 0);
	glTranslated(0, .01, 0);
	DrawCircle(1.3, 30);
	glPopMatrix();
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(-1, 1.2, 1);
	DrawSphere(.3, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslated(1, 1.2, 1);
	DrawSphere(.3, 10);
	glPopMatrix();
}

void CGLRenderer::DrawStem()
{
	double h = 3;
	glColor3f(0.153, 0.639, 0.278);

	DrawCylinder(.3, .3, h, 30);
	glTranslated(0, h, 0);
	DrawCylinder(.3, .5, h, 30);
	glTranslated(0, h, 0);
}

void CGLRenderer::DrawLeaf()
{
	int n = 10;

	double a = 6.;
	double delta = a / n;
	double x, y, z, nz;
	double u, v;

	glPushMatrix();

	glRotated(-90, 0, 1, 0);
	glTranslated(-a/2 - .5, 0, -a + .5);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, leaf);
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);

	z = 0;
	for (int i = 0; i < n; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		x = 0;
		for (int j = 0; j <= n; j++)
		{
			u = x / a;
			v = z / a;

			glTexCoord2f(u, v);
			glVertex3f(x, calcy(u, v), z);

			nz = z + delta;
			u = x / a;
			v = nz / a;
			glTexCoord2f(u, v);
			glVertex3f(x, calcy(u, v), nz);


			x += delta;
		}
		glEnd();

		z += delta;
	}

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void CGLRenderer::DrawBase()
{
	glPushMatrix();
	glRotated(40 , 0, 1, 0);
	DrawLeaf();
	glRotated(-80 , 0, 1, 0);
	DrawLeaf();
	glPopMatrix();


	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glRotated(-40 , 0, 1, 0);
	DrawLeaf();
	glRotated(80 , 0, 1, 0);
	DrawLeaf();
	glPopMatrix();
}
