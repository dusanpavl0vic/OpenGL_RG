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
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);

	env = LoadTexture("env.png");
	brick = LoadTexture("brick.png");

	
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


	double x = eyeR * sin(eyeBeta) * cos(eyeAlfa);
	double y = eyeR * cos(eyeBeta);
	double z = eyeR * sin(eyeBeta) * sin(eyeAlfa);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	// ---- LIGHT1 (crveno svetlo koje ide sa kamerom) ----
	GLfloat pos1[] = { 0.f, 0.f, 1.f, 0.f }; // direkciono iz kamere
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	GLfloat diff1[] = { 1.f, 0.f, 0.f, 1.f };
	GLfloat amb1[] = { 0.1f, 0.f, 0.f, 1.f };
	GLfloat specular1[] = { 1.f, 1.f, 1.f, 1.f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

	gluLookAt(
		x, y, z,
		0, 0, 0,
		0, 1, 0
	);


	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);*/
	glTranslatef(0, -50, 0);
	DrawCube(100);
	glTranslatef(0, 50, 0);

	/*glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);*/
	
	
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE); // da normalni ostanu dobri posle skaliranja

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// ---- LIGHT0 (globalno belo svetlo sa strane) ----
	GLfloat pos0[] = { 1.f, 1.f, 0.f, 0.f }; // direkciono
	GLfloat diff0[] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat specular0[] = { 1.f, 1.f, 1.f, 1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	


	DrawTube(2.5,3.5,10, 32);
	glTranslatef(0, 10, 0);

	glEnable(GL_NORMALIZE);
	DrawCone(3.8, 2, 32);

	glTranslatef(0, 0, 3.8);
	for (int i = 0; i < 4; i++) {
		glRotatef(vetrenjaca + 90, 0, 0, 1);
		DrawPaddle(8, 1.5);
	}


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

	glDeleteTextures(1, &env);
	glDeleteTextures(1, &brick);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
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

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texId;
}

void CGLRenderer::DrawCube(double a)
{
	double dx = (double)1 / (double)4;
	double dy = 1. / 3.;
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, env);

	glBegin(GL_QUADS);
		glTexCoord2f(4*dx, 1*dy);
		glVertex3f(-a/2, a, -a/2);

		glTexCoord2f(4*dx, 2*dy);
		glVertex3f(-a / 2, a, a / 2);

		glTexCoord2f(3*dx, 2*dy);
		glVertex3f(a / 2, a, a / 2);

		glTexCoord2f(3*dx, 1*dy);
		glVertex3f(a / 2, a, -a / 2);


		glTexCoord2f(1 * dx, dy);
		glVertex3f(-a / 2, 0, -a / 2);

		glTexCoord2f(1 * dx, 2 * dy);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(2 * dx, 2 * dy);
		glVertex3f(a / 2, 0, a / 2);

		glTexCoord2f(2 * dx, dy);
		glVertex3f(a / 2, 0, -a / 2);


		glTexCoord2f(1 * dx, 0);
		glVertex3f(-a / 2, a, -a / 2);

		glTexCoord2f(1 * dx, 1 * dy);
		glVertex3f(-a / 2, 0, -a / 2);

		glTexCoord2f(2 * dx, 1 * dy);
		glVertex3f(a / 2, 0, -a / 2);

		glTexCoord2f(2 * dx, 0*dy);
		glVertex3f(a / 2, a, -a / 2);


		glTexCoord2f(3 * dx, 1*dy);
		glVertex3f(a / 2, a, -a / 2);

		glTexCoord2f(2 * dx, 1 * dy);
		glVertex3f(a / 2, 0, -a / 2);

		glTexCoord2f(2 * dx, 2 * dy);
		glVertex3f(a / 2, 0, a / 2);

		glTexCoord2f(3 * dx, 2 * dy);
		glVertex3f(a / 2, a, a / 2);



		glTexCoord2f(2 * dx, 3 * dy);
		glVertex3f(a / 2, a, a / 2);

		glTexCoord2f(2 * dx, 2 * dy);
		glVertex3f(a / 2, 0, a / 2);

		glTexCoord2f(1 * dx, 2 * dy);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(1 * dx, 3 * dy);
		glVertex3f(-a / 2, a, a / 2);


		glTexCoord2f(0 * dx, 2 * dy);
		glVertex3f(-a / 2, a, a / 2);

		glTexCoord2f(1 * dx, 2 * dy);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(1 * dx, 1 * dy);
		glVertex3f(-a / 2, 0, -a / 2);

		glTexCoord2f(0 * dx, 1 * dy);
		glVertex3f(-a / 2, a, -a / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{

	double da = 2*3.14 / n;
	double alfa = 0.;

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brick);

	double d = 1. / (double)n;
	glBegin(GL_QUADS);
	for (int i = 0; i < n; i++) {
		double x1 = r1 * cos(alfa);
		double z1 = r1 * sin(alfa);

		double x2 = r1 * cos(alfa-da);
		double z2 = r1 * sin(alfa-da);

		double x3 = r2 * cos(alfa);
		double z3 = r2 * sin(alfa);

		double x4 = r2 * cos(alfa-da);
		double z4 = r2 * sin(alfa-da);

		double r = r2 - r1;
		double L = sqrt(r * r + h * h);

		double dr = r / L;
		double dy = h / L;

		double nx = dr * cos(alfa);
		double nz = dr * sin(alfa);
		double ny = dy;


		glTexCoord2f(i*d, 0);
		glVertex3f(x1, h, z1);

		glTexCoord2f(i * d, 1);
		glVertex3f(x3, 0, z3);

		glTexCoord2f((i+1) * d, 1);
		glVertex3f(x4, 0, z4);

		glTexCoord2f((i + 1) * d, 0);
		glVertex3f(x2, h, z2);



		alfa -= da;
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double da = 2 * 3.14 / n;
	double alfa = 0.;
	
	glEnable(GL_LIGHTING);

	double d = 1. / (double)n;
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(0, h, 0);
	for (int i = 0; i <= n; i++) {
		double x1 = r * cos(alfa);
		double z1 = r * sin(alfa);


		double L = sqrt(r*r + h*h);

		double dr = r / L;
		double dy = h / L;

		double nx = dr * cos(alfa);
		double nz = dr * sin(alfa);
		double ny = dy;
		glNormal3f(nx, ny, nz);
		glVertex3f(x1, 0, z1);



		alfa -= da;
	}
	glEnd();
}

void CGLRenderer::DrawPaddle(double length, double width)
{

	glPushMatrix();

	glTranslatef(0, width / 16., 0);
	glEnable(GL_LIGHTING);

	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(0,0, 0);
	glVertex3f(0, -width/8., 0);
	glVertex3f(2*length/8.,-width/8., 0);
	glVertex3f(2*length/8.,0, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, 1);
		glVertex3f(2 * length / 8., -width, 0);
		glVertex3f(2*length / 8., -(width / 8.), 0);
		glVertex3f(length/8., -(width / 8.), 0);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(2 * length / 8., 0, 0);
	glVertex3f(2 * length / 8., -width, 0);
	glVertex3f(length, -width, 0);
	glVertex3f(length, 0, 0);
	glEnd();


	glPopMatrix();
}
