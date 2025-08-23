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

	tsc[0] = LoadTexture("TSC0.jpg");
	tsc[1] = LoadTexture("TSC1.jpg");
	tsc[2] = LoadTexture("TSC2.jpg");
	tsc[3] = LoadTexture("TSC3.jpg");
	tsc[4] = LoadTexture("TSC4.jpg");
	tsc[5] = LoadTexture("TSC5.jpg");

	m[0] = LoadTexture("M0.jpg");
	m[1] = LoadTexture("M1.jpg");
	m[2] = LoadTexture("M2.jpg");
	m[3] = LoadTexture("M3.jpg");
	m[4] = LoadTexture("M4.jpg");
	m[5] = LoadTexture("M5.jpg");

	s[0] = LoadTexture("S0.jpg");
	s[1] = LoadTexture("S1.jpg");
	s[2] = LoadTexture("S2.jpg");
	s[3] = LoadTexture("S3.jpg");
	s[4] = LoadTexture("S4.jpg");
	s[5] = LoadTexture("S5.jpg");
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

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);


	double x = eyeR * sin(eyeBeta) * cos(eyeAlfa);
	double y = eyeR * cos(eyeBeta);
	double z = eyeR * sin(eyeBeta) * sin(eyeAlfa);

	gluLookAt(
		x, y, z,
		0, 0, 0,
		0, 1, 0
	);

	DrawAxes();


	DrawSpace(100, 20);

	if (ukljuciS)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	float ambient[] = {0,0,0,1};
	float diff[] = {1,1,1,1};
	float spec[] = {1,1,1,1};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	float pos[] = { 0, 0, 1, 0 };

	glEnable(GL_DEPTH_TEST);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	DrawEarth(3, 20);
	glTranslatef(30, 0, 0);
	glRotatef(moonr, 0, 1, 0);
	DrawMoon(1, 20);


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

	glDeleteTextures(6, tsc);
	glDeleteTextures(6, m);
	glDeleteTextures(6, s);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::DrawPatch(double R, int n)
{
	glColor3f(1,1,1);
	double ugao = 3.1415926535 /2;

	double dA = ugao / n;

	double alfa = -ugao/2;
	double beta = -ugao/2;


	for (int i = 0; i < n; i++) {
		beta = -ugao/2;
		for (int j = 0; j < n; j++) {

			double x1 = R * sin(beta) * cos(alfa);
			double y1 = R * cos(beta);
			double z1 = R * sin(beta) * sin(alfa);

			double x2 = R * sin(beta) * cos(alfa + dA);
			double y2 = R * cos(beta);
			double z2 = R * sin(beta) * sin(alfa + dA);

			double x3 = R * sin(beta - dA) * cos(alfa + dA);
			double y3 = R * cos(beta - dA);
			double z3 = R * sin(beta - dA) * sin(alfa + dA);

			double x4 = R * sin(beta - dA) * cos(alfa);
			double y4 = R * cos(beta - dA);
			double z4 = R * sin(beta - dA) * sin(alfa);

			double u1 = 1.0 - (double)i / (double)n;
			double v1 = 1.0 - (double)j / (double)n;

			double u2 = 1.0 - (double)(i +1) / (double)n;
			double v2 = 1.0 - (double)j / (double)n;

			double u3 = 1.0 - (double)(i + 1) / (double)n;
			double v3 = 1.0 - (double)(j + 1) / (double)n;

			double u4 = 1.0 - (double)i / (double)n;
			double v4 = 1.0 - (double)(j + 1) / (double)n;

			glBegin(GL_QUADS);
				glNormal3f(x1 / R, y1 / R, z1 / R);
				glTexCoord2f(u1, v1);
				glVertex3f(x1, y1, z1);

				glTexCoord2f(u2, v2);
				glVertex3f(x2, y2, z2);

				glTexCoord2f(u3, v3);
				glVertex3f(x3, y3, z3);

				glTexCoord2f(u4, v4);
				glVertex3f(x4, y4, z4);
			glEnd();

			beta -= dA;
		}

		alfa += dA;
	}

	

}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glRotatef(180, 1, 0, 0);

	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glRotatef(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, tsc[i]);
		DrawPatch(R, tes);
	}
	glPopMatrix();


	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tsc[4]);
		DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glRotatef(-90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tsc[5]);
		DrawPatch(R, tes);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glRotatef(180, 1, 0, 0);

	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glRotatef(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, m[i]);
		DrawPatch(R, tes);
	}
	glPopMatrix();


	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, m[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(-90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, m[5]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glRotatef(180, 1, 0, 0);

	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glRotatef(90, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, s[i]);
		DrawPatch(R, tes);
	}
	glPopMatrix();


	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, s[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(-90, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, s[5]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawAxes()
{

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();


}

//void CGLRenderer::DrawPatch(double R, int n)
//{
//	glColor3f(1,1,1);
//	double delta = 2. / (double)n;
//	double y = 1.0;
//	for (int i = 0; i < n; i++)
//	{
//		glBegin(GL_TRIANGLE_STRIP);
//		double x = -1.0;
//		for (int j = 0; j < n + 1; j++)
//		{
//			double phi, theta;
//			inverseTSC(x, y, phi, theta);
//			double xd, yd, zd;
//			xd = R * cos(theta) * sin(phi);
//			yd = R * sin(theta);
//			zd = R * cos(theta) * cos(phi);
//			glNormal3f(xd / R, yd / R, zd / R);
//			glTexCoord2f((x + 1.0) / 2.0,
//				(-y + 1.0) / 2.0);
//			glVertex3f(xd, yd, zd);
//			inverseTSC(x, y - delta, phi, theta);
//			xd = R * cos(theta) * sin(phi);
//			yd = R * sin(theta);
//			zd = R * cos(theta) * cos(phi);
//			glNormal3f(xd / R, yd / R, zd / R);
//			glTexCoord2f((x + 1.0) / 2.0,
//				(-y + delta + 1.0) / 2.0);
//			glVertex3f(xd, yd, zd);
//			x += delta;
//		}
//		glEnd();
//		y -= delta;
//	}
//}
void CGLRenderer::inverseTSC(double x,
	double y, double& phi, double& theta)
{
	phi = atan(x);
	theta = atan(y * cos(phi));
}
