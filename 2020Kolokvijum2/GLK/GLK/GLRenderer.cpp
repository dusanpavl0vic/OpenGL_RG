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
	glEnable(GL_DEPTH_TEST);

	pauk = LoadTexture("spider.png");

	pozadina[0] = LoadTexture("top.jpg");
	pozadina[1] = LoadTexture("bot.jpg");
	pozadina[2] = LoadTexture("left.jpg");
	pozadina[3] = LoadTexture("front.jpg");
	pozadina[4] = LoadTexture("right.jpg");
	pozadina[5] = LoadTexture("back.jpg");
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

	double xCam = rCam * sin(betaCam) * cos(alfaCam);
	double yCam = rCam * cos(betaCam);
	double zCam = rCam * sin(betaCam) * sin(alfaCam);
	gluLookAt(
		xCam, yCam, zCam,
		0, 10, 0,
		0, 1, 0
	);


	DrawAxes();
	DrawEnvCube(200);

	//glTranslatef(0, 20, 0);
	//DrawLeg();
	glRotatef(90, 0, 1, 0);
	DrawSpider();
	//DrawSphere(2, 10, 0.75, 0.25, 0.245);
	



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

	glDeleteTextures(1, &pauk);
	glDeleteTextures(6, pozadina);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{

	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();

}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texId;
	DImage img;
	img.Load(CString(fileName));
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texId;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, pozadina[0]);
	glBegin(GL_QUADS);

		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, a, -a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, a, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, a, a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a, -a / 2);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, pozadina[1]);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, 0, -a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, pozadina[2]);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, pozadina[3]);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, -a / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, pozadina[4]);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glEnd();


	glBindTexture(GL_TEXTURE_2D, pozadina[5]);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, a / 2);
	glEnd();
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{

	//glEnable(GL_TEXTURE_2D);
	//glColor3f(1, 1, 1);
	//glBindTexture(GL_TEXTURE_2D, pauk);

	double alfa = 2 * 3.14 / nSeg;
	double beta = 3.14 / nSeg;

	for (int i = 0; i < nSeg; i++) {

		for (int j = 0; j < nSeg * 2; j++) {

			double x1 = r * sin(beta * (j)) * cos(alfa * (i));
			double y1 = r * cos(beta * (j));
			double z1 = r * sin(beta * (j)) * sin(alfa * (i));

			double x2 = r * sin(beta * (j)) * cos(alfa * (i + 1));
			double y2 = r * cos(beta * (j));
			double z2 = r * sin(beta * (j)) * sin(alfa * (i + 1));

			double x3 = r * sin(beta * (j + 1)) * cos(alfa * (i + 1));
			double y3 = r * cos(beta * (j + 1));
			double z3 = r * sin(beta * (j + 1)) * sin(alfa * (i + 1));

			double x4 = r * sin(beta * (j + 1)) * cos(alfa * (i));
			double y4 = r * cos(beta * (j + 1));
			double z4 = r * sin(beta * (j + 1)) * sin(alfa * (i));


			double u1 = texU + x1 / r * texR;
			double v1 = texV + z1 / r * texR;

			double u2 = texU + x2 / r * texR;
			double v2 = texV + z2 / r * texR;

			double u3 = texU + x3 / r * texR;
			double v3 = texV + z3 / r * texR;

			double u4 = texU + x4 / r * texR;
			double v4 = texV + z4 / r * texR;


			glBegin(GL_QUADS);
				glTexCoord2f(u1, v1);
				glVertex3f(x1, y1, z1);
				glTexCoord2f(u2, v2);
				glVertex3f(x2, y2, z2);
				glTexCoord2f(u3, v3);
				glVertex3f(x3, y3, z3);
				glTexCoord2f(u4, v4);
				glVertex3f(x4, y4, z4);
			glEnd();
		}
	}
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	/*glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, pauk);*/

	double alfa = 2 * 3.14 / nSeg;
	double beta = 3.14 / nSeg;

	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(texU, texV);
		glVertex3f(0, h, 0);
	for (int i = 1; i <= nSeg + 1; i++) {
		double x = r * cos(alfa * i);
		double z = r * sin(alfa * i);


		double u = texU + x / r * texR;
		double v = texV + z / r * texR;

		glTexCoord2f(u, v);
		glVertex3f(x, 0, z);
	}
	glEnd();
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, pauk);

	glPushMatrix();
	glTranslatef(0, r, 0);


	DrawSphere(r, 2*nSeg, 0.25, 0.25, 0.245);
	glTranslatef(0, r/2, 0);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.245);


	glPopMatrix();
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();
	glRotatef(45, 0, 0, 1);

	DrawLegSegment(1, 10, 5);

	glTranslatef(0, 10+1, 0);
	glRotatef(85, 0, 0, 1);

	DrawLegSegment(1, 15, 5);
	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, pauk);
	glPushMatrix();


	

		//glava
		glPushMatrix();
		glTranslatef(-2-1.5, 0, 0);
		glScalef(1.0, 0.5, 1.0);
		DrawSphere(2, 10, 0.75, 0.25, 0.245);
		glPopMatrix();

		//grudi
		glPushMatrix();
		glScalef(1.0, 0.5, 1.0);
		DrawSphere(3, 10, 0.25, 0.25, 0.245);
		glPopMatrix();

		//stomak
		glPushMatrix();
		glTranslatef(5 + 1.5, 0, 0);
		glScalef(1.0, 0.8, 1.0);
		DrawSphere(5, 10, 0.25, 0.25, 0.245);
		glPopMatrix();


	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	glTranslatef(0, 3, 0);
	DrawSpiderBody();

		//glRotatef(45, 0, 0, 1);
	for (int i = 1; i <= 4; i++) {
		glPushMatrix();
		glRotatef(30*i, 0, 1, 0);
		DrawLeg();
		glPopMatrix();
	}

	for (int i = 1; i <= 4; i++) {
		glPushMatrix();
		glRotatef(30 * i + 180, 0, 1, 0);
		DrawLeg();
		glPopMatrix();
	}

}
