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

	excavator = LoadTexture("assets/excavator.png");
	front = LoadTexture("assets/front.jpg");
	left = LoadTexture("assets/left.jpg");
	right = LoadTexture("assets/right.jpg");
	back = LoadTexture("assets/back.jpg");
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	double x = r * cos(phi) * cos(theta);
	double y = r * sin(phi);
	double z = r * cos(phi) * sin(theta);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);


	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glTranslated(0, 50, 0);
	DrawEnvCube(100);
	glDisable(GL_CULL_FACE);
	glPopMatrix();

	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
	glEnd();

	DrawExcavator();
	
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
	gluPerspective(55, aspect, 0.1, 2000);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);

	glDeleteTextures(1, &excavator);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);

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

	DImage img; if (img.Load(CString(fileName)) == false) throw;

	glGenTextures(1, &id);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);

	glPixelStorei(GL_UNSIGNED_INT, 4);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	glDisable(GL_TEXTURE_2D);

	return id;
}

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	double xa = 0, ya = 0;
	double ua = 0, va = 0;

	for (int i = 0; i < n; i++) {
		xa += points[i].x;
		ya += points[i].y;

		ua += texCoords[i].x;
		va += texCoords[i].y;
	}

	double cx = xa / (double)n;
	double cy = ya / (double)n;
	double cu = ua / (double)n;
	double cv = va / (double)n;

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(cu, cv);
	glVertex3f(cx, cy, 0);

	for (int i = 0; i < n; i++) {
		glTexCoord2f(texCoords[i].x, texCoords[i].y);
		glVertex3f(points[i].x, points[i].y, 0);
	}

	glTexCoord2f(texCoords[0].x, texCoords[0].y);
	glVertex3f(points[0].x, points[0].y, 0);

	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glPushMatrix();

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, excavator);

	glEnable(GL_TEXTURE_2D);
	DrawPolygon(points, texCoords, n);

	glDisable(GL_TEXTURE_2D);
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < n; i++) {
		glVertex3f(points[i].x, points[i].y, 0);
		glVertex3f(points[i].x, points[i].y, zh);
	}
	glVertex3f(points[0].x, points[0].y, 0);
	glVertex3f(points[0].x, points[0].y, zh);
	glEnd();

	glTranslated(0, 0, zh);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);
	DrawPolygon(points, texCoords, n);

	glPopMatrix();
}

void CGLRenderer::DrawBase()
{
	POINTF points[8] = {
		POINTF { 0, .5 },
		POINTF { .5, 0 },
		POINTF { 7.5, 0 },
		POINTF { 8, .5 },
		POINTF { 8, 1.5 },
		POINTF { 7.5, 2 },
		POINTF { .5, 2 },
		POINTF { 0, 1.5 },
	};

	POINTF uv[8] = {
		POINTF { 0,  1. - 0.0625},
		POINTF { 0.0625, 1. },
		POINTF { 1 - 0.0625, 1.},
		POINTF { 1, 1. - 0.0625},
		POINTF { 1, .75 + 0.0625},
		POINTF { 1 - 0.0625, .75},
		POINTF { 0.0625, .75},
		POINTF { 0 , .75 + 0.0625},
	};

	glPushMatrix();
	glTranslated(-4, 0, -2.5);
	DrawExtrudedPolygon(points, uv, 8, 5, 0, 0, 0);
	glPopMatrix();
}

void CGLRenderer::DrawBody()
{
	POINTF points[5] = {
		POINTF { 0, 0 },
		POINTF { 4, 0 },
		POINTF { 4, 2 },
		POINTF { 2, 4 },
		POINTF { 0, 4 },
	};

	POINTF uv[5] = {
		POINTF { .5, .5 },
		POINTF { 1, .5 },
		POINTF { 1, .25 },
		POINTF { .75, 0 },
		POINTF { .5, 0},
	};

	glPushMatrix();
	glTranslated(-2, 0, -2);
	DrawExtrudedPolygon(points, uv, 5, 4, 0.96, 0.5, 0.12);
	glPopMatrix();
}

void CGLRenderer::DrawArm(double zh)
{
	POINTF points[6] = {
		POINTF { 0, .5 },
		POINTF { .5, 0 },
		POINTF { 8, .5 },
		POINTF { 8, 1.5 },
		POINTF { .5, 2 },
		POINTF { 0, 1.5 },
	};

	POINTF uv[6] = {
		POINTF { 0,  .75 - 0.0625},
		POINTF { 0.0625, .75 },
		POINTF { 1, .75 - 0.0625},
		POINTF { 1, .5 + 0.0625},
		POINTF { 0.0625, .5},
		POINTF { 0 , .5 + 0.0625},
	};

	glPushMatrix();
	glTranslated(-1, -1, -zh / 2);
	DrawExtrudedPolygon(points, uv, 6, zh, 0.96, 0.5, 0.12);
	glPopMatrix();
}

void CGLRenderer::DrawFork()
{
	POINTF points[6] = {
		POINTF { 0, 0 },
		POINTF { 4, 0 },
		POINTF { 4, 2.5 },
		POINTF { 3.5, 3 },
		POINTF { .5, 3 },
		POINTF { 0, 2.5 },
	};

	POINTF uv[6] = {
		POINTF { 0, 6 * 0.0625 },
		POINTF { .5, 6 * 0.0625 },
		POINTF { .5, 0.0625 },
		POINTF { .5 - 0.0625, 0 },
		POINTF { 0.0625, 0 },
		POINTF { 0, 0.0625 },
	};

	glPushMatrix();
	glTranslated(-1, -1, 0);
	DrawExtrudedPolygon(points, uv, 6, 1, 0.7, 0.7, 0.7);
	glPopMatrix();
}

void CGLRenderer::DrawExcavator()
{
	DrawBase();
	glTranslated(0, 2, 0);
	glRotated(a1, 0, 1, 0);
	DrawBody();
	glTranslated(1.5, 1, -2.5);
	glRotated(a2, 0, 0, 1);
	DrawArm(1);
	glTranslated(8 - 1.5, 0, 0);
	glRotated(a3, 0, 0, 1);
	DrawArm(1.5);
	glTranslated(8 - 1.5, 0, 0);
	glRotated(a4, 0, 0, 1);
	DrawFork();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

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
	glRotated(90, 0, 1, 0);

	glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, bot);
	DrawQuad(a);

	glRotated(180, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, top);
	DrawQuad(a);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
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
