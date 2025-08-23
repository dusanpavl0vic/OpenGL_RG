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
	glClearColor(1., 1., 1., 1.);

	back = LoadTexture("assets/back.jpg");
	bot = LoadTexture("assets/bot.jpg");
	front = LoadTexture("assets/front.jpg");
	left = LoadTexture("assets/left.jpg");
	right = LoadTexture("assets/right.jpg");
	spider = LoadTexture("assets/spider.png");
	top = LoadTexture("assets/top.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC, int xt, int yt, int zt)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	double PI = 3.14159;

	double mult = 0.1;
	double alpha = PI / 2 - xt * mult;
	double beta = yt * mult;

	double R = min(max(50. - zt, 8), 50);

	double x = R * cos(beta) * cos(alpha);
	double y = R * sin(beta);
	double z = R * cos(beta) * sin(alpha);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y + 10, z, 0, 0, 0, 0, 1., 0);

	DrawAxes();

	glCullFace(GL_FRONT);
	glTranslated(0, 46, 0);
	DrawEnvCube(100.);
	glTranslated(0, -46, 0);

	glCullFace(GL_BACK);
	DrawSpider();

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
	gluPerspective(50.0, aspect, 0.1, 2000);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &bot);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &spider);
	glDeleteTextures(1, &top);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2.0);

	glBegin(GL_LINES);
	glColor3f(0, 0, 1.f);
	glVertex3f(0, 0, 0);
	glVertex3f(50.f, 0, 0);

	glColor3f(1.f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50.f, 0);

	glColor3f(0, 1.f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50.f);
	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT id;

	DImage img;
	auto res = img.Load(CString(fileName));

	if (res == false) {
		throw "Neuspesno ucitavanje texture";
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return id;
}

void CGLRenderer::DrawQuad(double a)
{
	double x = a / 2.;

	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 1.f);
		glVertex3f(-x, -x, x);
		glTexCoord2f(1.f, 1.f);
		glVertex3f(x, -x, x);
		glTexCoord2f(1.f, 0.f);
		glVertex3f(x, x, x);
		glTexCoord2f(0.f, 0.f);
		glVertex3f(-x, x, x);
	glEnd();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, front);
		DrawQuad(a);

		glRotated(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, right);
		DrawQuad(a);

		glRotated(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, back);
		DrawQuad(a);

		glRotated(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, left);
		DrawQuad(a);

	glPopMatrix();

	glPushMatrix();

		glRotated(180, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		glBindTexture(GL_TEXTURE_2D, bot);
		DrawQuad(a);

		glRotated(180, 1, 0, 0);
		glBindTexture(GL_TEXTURE_2D, top);
		DrawQuad(a);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	double PI = 3.14159;
	double x, y, z;
	double delta = PI / (double)nSeg;

	double alpha = PI / 2;
	for (int i = 0; i < nSeg; i++) {

		double beta = 0.;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= nSeg*2; j++) {

			x = r * cos(alpha) * cos(beta);
			y = r * sin(alpha);
			z = r * cos(alpha) * sin(beta);

			glTexCoord2f(texU + x / r * texR, texV + z / r * texR);
			glVertex3f(x, y, z);

			x = r * cos(alpha - delta) * cos(beta);
			y = r * sin(alpha - delta);
			z = r * cos(alpha - delta) * sin(beta);

			glTexCoord2f(texU + x / r * texR, texV + z / r * texR);
			glVertex3f(x, y, z);

			beta -= delta;
		}
		glEnd();

		alpha -= delta;
	}
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	double x, z;
	double PI = 3.14159;
	double delta = 2 * PI / (double)nSeg;

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(texU, texV);
	glVertex3f(0, h, 0);

	double alpha = 0.;
	for (int i = 0; i <= nSeg; i++) {
		x = cos(alpha) * r;
		z = sin(alpha) * r;

		glTexCoord2f(texU + x / r * texR, texV + z / r * texR);
		glVertex3f(x, 0, z);

		alpha -= delta;
	}

	glEnd();
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spider);

	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.245);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.245);

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();
	DrawLegSegment(1., 10., 5);
	glTranslated(0, 10., 0);
	glRotated(-85, 0, 0, 1);
	DrawLegSegment(1., 15., 5);
	glPopMatrix();

}

void CGLRenderer::DrawSpiderBody()
{
	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spider);
	glPushMatrix();

	glPushMatrix();
	glScaled(1, 0.5, 1);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, 5);
	glScaled(1, 0.5, 1);
	glRotated(90, 0, 1, 0);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -8);
	glScaled(1, 0.8, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);
	glPopMatrix();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSpider()
{
	glPushMatrix();

	DrawSpiderBody();

	glRotated(-45, 0, 1, 0);

	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(90, 0, 1, 0);

	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);

	glRotated(30, 0, 1, 0);
	glRotated(45, 0, 0, -1);
	DrawLeg();
	glRotated(-45, 0, 0, -1);


	glPopMatrix();
}
