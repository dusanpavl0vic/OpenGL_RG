#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#include <string>
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

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);

	M[0] = LoadTexture("assets/M0.jpg");
	M[1] = LoadTexture("assets/M1.jpg");
	M[2] = LoadTexture("assets/M2.jpg");
	M[3] = LoadTexture("assets/M3.jpg");
	M[4] = LoadTexture("assets/M4.jpg");
	M[5] = LoadTexture("assets/M5.jpg");

	S[0] = LoadTexture("assets/S0.jpg");
	S[1] = LoadTexture("assets/S1.jpg");
	S[2] = LoadTexture("assets/S2.jpg");
	S[3] = LoadTexture("assets/S3.jpg");
	S[4] = LoadTexture("assets/S4.jpg");
	S[5] = LoadTexture("assets/S5.jpg");

	T[0] = LoadTexture("assets/TSC0.jpg");
	T[1] = LoadTexture("assets/TSC1.jpg");
	T[2] = LoadTexture("assets/TSC2.jpg");
	T[3] = LoadTexture("assets/TSC3.jpg");
	T[4] = LoadTexture("assets/TSC4.jpg");
	T[5] = LoadTexture("assets/TSC5.jpg");
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC, boolean holding, CPoint prev, CPoint curr)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double mult = .01;
	if (holding) {
		if (prev.x != -1 && prev.y != -1) {
			long diffX = curr.x - prev.x;
			long diffY = curr.y - prev.y;

			alpha += diffX * mult;
			beta += diffY * mult;
		}
	}

	double r = 40.0;
	double x = r * cos(beta) * cos(alpha);
	double y = r * sin(beta);
	double z = r * cos(beta) * sin(alpha);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0, 0, 0, 0, 1.0, 0);

	glDisable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_FRONT);
	glTranslatef(x, y, z);
	glDisable(GL_DEPTH_TEST);
	DrawSpace(1.0, 20);
	glEnable(GL_DEPTH_TEST);
	glTranslatef(-x, -y, -z);
	glCullFace(GL_BACK);

	GLfloat lmodel_ambient[] = { 0., 0., 0., 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	GLfloat light1_ambient[] = { .0, .0, .0, 1.0 };
	GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light1_position[] = { 0., 0., -0.1, 0. };
	GLfloat light1_direction[] = { 0., 0., 1.0};

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.0);

	DrawEarth(3.0, 20);

	glTranslated(50, 0, 0);
	DrawMoon(1.0, 20);

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
	double aspect = (double)w / (double)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, aspect, .1, 2000);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 

	glDeleteTextures(6, S);
	glDeleteTextures(6, M);
	glDeleteTextures(6, T);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT id;

	DImage img;
	boolean success = img.Load(CString(fileName));

	if (!success) {
		throw;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	auto a = img.Width(), b = img.Height();

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return id;
}

void CGLRenderer::DrawPatch(double R, int n)
{
	double delta = 2. / (double)n;

	double phi, theta;
	double x, y, z;
	double texx, texy;

	double yt = -1.;
	for (double i = 0; i < n; i ++) {
		glBegin(GL_TRIANGLE_STRIP);

		double xt = -1.;
		for (double j = 0; j <= n; j ++) {
			texx = (xt + 1.) / 2.;

			phi = atan(xt);
			theta = atan((yt + delta) * cos(phi));

			x = cos(theta) * sin(phi);
			z = cos(theta) * cos(phi);
			y = sin(theta);

			texy = ((yt + delta) + 1.) / 2.;

			glNormal3f(x, y, z);
			glTexCoord2f(texx, 1. - texy);
			glVertex3f(x * R, y * R, z * R);

			phi = atan(xt);
			theta = atan(yt * cos(phi));

			x = cos(theta) * sin(phi);
			z = cos(theta) * cos(phi);
			y = sin(theta);

			texy = (yt + 1.) / 2.;

			glNormal3f(x, y, z);
			glTexCoord2f(texx, 1. - texy);
			glVertex3f(x * R, y * R, z * R);


			xt += delta;
		}

		glEnd();
		yt += delta;
	}
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, T[0]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, T[1]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, T[2]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, T[3]);
		DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
		glRotated(90.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, T[5]);
		DrawPatch(R, tes);

		glRotated(180.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, T[4]);
		DrawPatch(R, tes);
	glPopMatrix();
}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, M[0]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, M[1]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, M[2]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, M[3]);
		DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
		glRotated(90.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, M[5]);
		DrawPatch(R, tes);

		glRotated(180.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, M[4]);
		DrawPatch(R, tes);
	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, S[0]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, S[1]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, S[2]);
		DrawPatch(R, tes);

		glRotated(90.0, 0, 1., 0);
		glBindTexture(GL_TEXTURE_2D, S[3]);
		DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
		glRotated(90.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, S[5]);
		DrawPatch(R, tes);

		glRotated(180.0, 1., 0, 0);
		glBindTexture(GL_TEXTURE_2D, S[4]);
		DrawPatch(R, tes);
	glPopMatrix();
}
