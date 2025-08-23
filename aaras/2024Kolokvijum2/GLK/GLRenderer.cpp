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
	env = LoadTexture("Env.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC, int xt, int yt, int zt, int ft)
{
	this->ft = ft;

	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	double PI = 3.14159;
	double r = 10 - zt * 0.1;
	double mult = 0.1;
	double alpha = yt * mult;
	double beta = PI/2 - xt * mult;

	double x, y, z;

	x = r * cos(alpha) * cos(beta);
	y = r * sin(alpha);
	z = r * cos(alpha) * sin(beta);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);

	float g_ambient[4] = { 0.3, 0.3, 0.3, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_ambient);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	float ambient[4] = { 0.5, 0.5, 0.5, 1.0 };
	float diffuse[4] = { 1, 1, 1, 1.0 };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);

	float f_position[4] = { 0, 0, 1, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, f_position);

	glEnable(GL_LIGHT1);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

	float position[4] = { 0, 1, 1, 0.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, position);

	glEnable(GL_LIGHT2);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glTranslated(x, y, z);
	DrawSphere(1, 20, 30);
	glTranslated(-x, -y, -z);
	glEnable(GL_DEPTH_TEST);

	DrawAxes();
	glEnable(GL_LIGHTING);

	DrawFlower();
	glTranslated(9, 0, 0);
	DrawFlower();
	glTranslated(-9, 0, -9);
	DrawFlower();

	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, w, h);

	double aspect = (double)w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50., aspect, 0.1, 2000);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
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
	UINT id = 0;

	DImage img;
	auto res = img.Load(CString(fileName));

	if (res == false) {
		throw "Losa putanja";
	}

	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNSIGNED_BYTE, 4);

	glGenTextures(1, &id);
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

void CGLRenderer::PolarToCartesian(double R, double phi, double theta, double& x, double& y, double& z)
{
	x = R * cos(phi) * cos(theta);
	y = R * sin(phi);
	z = R * cos(phi) * sin(theta);
}

void CGLRenderer::DrawSphere(float R, int n, int m)
{
	double PI = 3.14159;
	double x, y, z;
	double phi, theta;
	double deltaPhi = -PI / m;
	double deltaTheta = - 2*PI / n;

	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, env);
	glCullFace(GL_FRONT);

	phi = PI / 2;
	for (int i = 0; i < m; i++) {
		glBegin(GL_TRIANGLE_STRIP);

		theta = 0;
		for (int j = 0; j <= n; j++) {
			PolarToCartesian(R, phi, theta, x, y, z);
			glTexCoord2f(phi / PI, theta / (2* PI));
			glVertex3f(x, y, z);

			PolarToCartesian(R, phi + deltaPhi, theta, x, y, z);
			glTexCoord2f((phi + deltaPhi) / PI, theta / (2* PI));
			glVertex3f(x, y, z);

			theta += deltaTheta;
		}

		glEnd();

		phi += deltaPhi;
	}

	glDisable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
}

void CGLRenderer::CalcRotAxis(double x1, double y1, double z1, double x2, double y2, double z2, double& x3, double& y3, double& z3)
{
	double x = x2 - x1;
	double y = y2 - y1;
	double z = z2 - z1;

	double mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

	x3 = x / mag;
	y3 = y / mag;
	z3 = z / mag;
}

void CGLRenderer::DrawSphFlower(float R, int n, int m, float factor, unsigned char R1, unsigned char G1, unsigned char B1, unsigned char R2, unsigned char G2, unsigned char B2)
{
	double PI = 3.14159;
	double phi, theta;
	double deltaPhi = PI / m;
	double deltaTheta = - 2*PI / n;

	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glColor3f(1, 1, 1);

	theta = 0;
	for (int i = 0; i < n; i++) {

		glPushMatrix();
		phi = -PI / 2;
		for (int j = 0; j < m; j++) {
			double x1, y1, z1;
			double x2, y2, z2;
			double x3, y3, z3;
			double x4, y4, z4;

			PolarToCartesian(R, phi, theta, x1, y1, z1);
			PolarToCartesian(R, phi, theta + deltaTheta, x2, y2, z2);
			PolarToCartesian(R, phi + deltaPhi, theta + deltaTheta, x3, y3, z3);
			PolarToCartesian(R, phi + deltaPhi, theta, x4, y4, z4);

			double x, y, z;
			CalcRotAxis(x1, y1, z1, x2, y2, z2, x, y, z);

			double x5 = (x1 + x2) / 2;
			double y5 = (y1 + y2) / 2;
			double z5 = (z1 + z2) / 2;

			double r1 = R1 / (double)255;
			double g1 = G1 / (double)255;
			double b1 = B1 / (double)255;

			double r2 = R2 / (double)255;
			double g2 = G2 / (double)255;
			double b2 = B2 / (double)255;

			double t = 1 - (phi + PI / 2) / PI;

			float clr_diffuse[3] = {
				t * r1 + (1 - t) * r2,
				t * g1 + (1 - t) * g2,
				t * b1 + (1 - t) * b2,
			};

			//glColor3fv(clr_diffuse);

			float clr_ambient[3] = { r1, g1, b1 };

			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, clr_diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, clr_ambient);

			glTranslatef(x5, y5, z5);
			glRotated(ft * factor, x, y, z);
			glTranslatef(-x5, -y5, -z5);

			glBegin(GL_QUADS);
				glNormal3f(x1, y1, z1);
				glVertex3f(x1, y1, z1);
				glNormal3f(x2, y2, z2);
				glVertex3f(x2, y2, z2);
				glNormal3f(x3, y3, z3);
				glVertex3f(x3, y3, z3);
				glNormal3f(x4, y4, z4);
				glVertex3f(x4, y4, z4);
			glEnd();

			phi += deltaPhi;
		}
		glPopMatrix();

		theta += deltaTheta;
	}

	glEnable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
}

void CGLRenderer::DrawFlower()
{
	glPushMatrix();

	DrawSphFlower(2,	36, 18,	1,		0, 192, 0,	0,	255,0);
	glTranslated(0, -.49, 0);
	DrawSphFlower(1.5,	36, 18,	.5,		0, 0, 255,	255,0,	0);
	glTranslated(0, -.49, 0);
	DrawSphFlower(1,	36, 18,	.25,	192, 192, 0,192,192,192);

	glPopMatrix();
}

	//// Canary
	//// ======================
	//glColor3f(1, 1, 1);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, env);
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0, 0);
	//	glVertex3f(0, 0, 0);
	//	glTexCoord2f(1, 0);
	//	glVertex3f(1, 0, 0);
	//	glTexCoord2f(1, 1);
	//	glVertex3f(1, 1, 0);
	//	glTexCoord2f(0, 1);
	//	glVertex3f(0, 1, 0);
	//glEnd();
	//glDisable(GL_TEXTURE_2D);
	//// ======================
