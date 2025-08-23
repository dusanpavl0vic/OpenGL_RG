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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	lampa = LoadTexture("lamp.jpg");
	t = LoadTexture("top.jpg");
	b = LoadTexture("bot.jpg");

	o[0] = LoadTexture("left.jpg");
	o[1] = LoadTexture("side.jpg");
	o[2] = LoadTexture("right.jpg");
	o[3] = LoadTexture("back.jpg");
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


	double eyeX = rCam*sin(betaCam)*cos(alfaCam);
	double eyeY = rCam*cos(betaCam);
	double eyeZ = rCam*sin(betaCam)*sin(alfaCam);

	gluLookAt(
		eyeX, eyeY, eyeZ,
		0, 0, 0,
		0, 1, 0
	);
	DrawAxes();
	//DrawT();
	DrawEnvCube(150);
	glColor3f(1, 1, 1);
	//DrawCylinder(2.5, 5, 5, 20, 0, false);
	DrawLamp();


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
	gluPerspective(50, (double)w/(double)h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 

	glDeleteTextures(1, &lampa);
	glDeleteTextures(1, &t);
	glDeleteTextures(1, &b);
	glDeleteTextures(4, o);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawT()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);

	
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 1, 0);
	glVertex3f(-5, 0, 0);

	glColor3f(0, 0, 1);
	glVertex3f(5, 0, 0);
	glEnd();
}

void CGLRenderer::DrawAxes()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(CString(fileName));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, t);

	glBegin(GL_QUADS);
		glNormal3f(0, -1, 0);
		glTexCoord2f(0,1);
		glVertex3f(-a/2, a, -a/2);
		glTexCoord2f(0, 0);
		glVertex3f(-a/2, a, a/2);
		glTexCoord2f(1, 0);
		glVertex3f(a/2, a, a/2);
		glTexCoord2f(1, 1);
		glVertex3f(a/2, a, -a/2);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, b);
	glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 1);
		glVertex3f(-a/2, 0, -a/2);
		glTexCoord2f(0, 0);
		glVertex3f(-a/2, 0, a/2);
		glTexCoord2f(1, 0);
		glVertex3f(a/2, 0, a/2);
		glTexCoord2f(1, 1);
		glVertex3f(a/2, 0, -a/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, o[0]);
	glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, a, a / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, 0, a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, 0, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, a, -a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, o[1]);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, a, -a / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, a, -a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, o[2]);
	glBegin(GL_QUADS);
		glNormal3f(-1, 0, 0);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, a, -a / 2);
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, a, a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, o[2]);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, a, a / 2);
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, 0, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, a, a / 2);
	glEnd();

	glDisable(GL_TEXTURE);
}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen)
{
	if (!bIsOpen) {
		texMode = 0;
	}
	double repy1 = 0;
	double repy2 = 0;
	glEnable(GL_TEXTURE_2D);
	if (texMode == 0) {
		repy1 = 0.5;
		repy2 = 1.0;
	}
	else {
		repy1 = 0.0;
		repy2 = 0.5;
	}

	glBindTexture(GL_TEXTURE_2D, lampa);

	double alfa = 2 * 3.14 / nSeg;

	if (!bIsOpen) {
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, 1, 0);
		glTexCoord2f(0.5, 0.25);
		glVertex3f(0, h, 0);

		for (int i = 0; i <= nSeg; i++) {
			double x = r1 * cos(i * alfa);
			double z = r1 * sin(i * alfa);

			glTexCoord2f(0.5 + (x/r1)/4, (z/r1)/4 + 0.25);

			glVertex3f(x, h, z);
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, -1, 0);
		glTexCoord2f(0.5, 0.25);

		glVertex3f(0, 0, 0);

		for (int i = 0; i <= nSeg; i++) {
			double x = r2 * cos(i * alfa);
			double z = r2 * sin(i * alfa);

			glTexCoord2f(0.5 + (x / r2) / 4, (z / r2) / 4 + 0.25);
			glVertex3f(x, 0, z);
		}
		glEnd();

	}
	glBegin(GL_QUADS);

		for (int i = 0; i < nSeg; i++) {

			double x1 = r1 * cos(i * alfa);
			double z1 = r1 * sin(i * alfa);

			double x2 = r2 * cos(i * alfa);
			double z2 = r2 * sin(i * alfa);

			double x12 = r1 * cos((i + 1) * alfa);
			double z12 = r1 * sin((i + 1) * alfa);

			double x22 = r2 * cos((i + 1) * alfa);
			double z22 = r2 * sin((i + 1) * alfa);

			double L = sqrt((r1 - r2)*(r1 - r2) + h*h);

			double nx = (h / L) * cos(alfa * i);
			double ny = ((r1 - r2) / L);
			double nz = (h / L) * sin(alfa * i);

			glNormal3f(nx, ny, nz);

			double tx1 = (double)i / nSeg;
			double tx2 = (double)(i + 1) / nSeg;
			double ty = 0.5;
			glTexCoord2f(tx2, ty + repy1);
			glVertex3f(x12, h, z12);

			glTexCoord2f(tx2, 0 + repy1);
			glVertex3f(x22, 0, z22);

			glTexCoord2f(tx1, 0 + repy1);
			glVertex3f(x2, 0, z2);

			glTexCoord2f(tx1, ty + repy1);
			glVertex3f(x1, h, z1);
		}
	glEnd();

	
}

void CGLRenderer::DrawLampBase()
{
	DrawCylinder(7, 8, 2, 20, 1, false);
}

void CGLRenderer::DrawLampArm()
{
	glPushMatrix();
	glTranslatef(0, 0, -1);
	glRotatef(90, 1, 0, 0);
	DrawCylinder(3, 3, 2, 20, 1, false);

	glPopMatrix();

	glPushMatrix();
	DrawCylinder(1, 1, 15, 20, 1, false);
	glPopMatrix();

}

void CGLRenderer::DrawLampHead()
{
	glPushMatrix();
	glTranslatef(-1.5, 1.5, -1);
	glRotatef(angleHead, 0, 0, 1);
	glTranslatef(1.5, -1.5, 1);

	DrawCylinder(3, 3, 5, 20, 1, false);

	glPushMatrix();
	glTranslatef(0, -5, 0);
	DrawCylinder(3, 6, 5, 20, 1, true);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 5, 0);
	DrawCylinder(2, 3, 1, 20, 1, false);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-1.5, 1.5, -1);
	glRotatef(90, 1, 0, 0);
	DrawCylinder(3, 3, 2, 20, 1, false);
	glPopMatrix();


	glPopMatrix();


	


}

void CGLRenderer::DrawLamp()
{
	glPushMatrix();

	DrawLampBase();
	glRotatef(angleArm1+45, 0, 0, 1);
	DrawLampArm();
	glTranslatef(0, 15, 0);
	glRotatef(angleArm2-90, 0, 0, 1);
	DrawLampArm();
	glTranslatef(0, 15, 0);
	glTranslatef(1.5, -1.5, 1);
	glRotatef(90, 0, 0, 1);
	DrawLampHead();

	glPopMatrix();

}
