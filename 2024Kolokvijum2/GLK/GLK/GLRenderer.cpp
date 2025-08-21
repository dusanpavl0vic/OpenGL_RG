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
	glClearColor(1, 1, 1, 0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	env = LoadTexture("Env.jpg");
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	GLfloat globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
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
	double eyeX = cam_dist * cos(cam_alpha * 3.14 / 180.0) * sin(cam_beta * 3.14 / 180.0);
	double eyeY = cam_dist * cos(cam_beta * 3.14 / 180.0);
	double eyeZ = cam_dist * sin(cam_alpha * 3.14 / 180.0) * sin(cam_beta * 3.14 / 180.0);

	gluLookAt(
		eyeX, eyeY, eyeZ,   // pozicija posmatrača
		0.0, 0.0, 0.0,      // gleda u centar scene
		0.0, 1.0, 0.0       // vektor "gore"
	);



	GLfloat light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // bela difuzna
	GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // bela spekularna
	GLfloat light0_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // tražena ambijentalna

	// Pravac svetlosti "iz pravca posmatrača" => (0,0,1) posle transformacije kamere
	GLfloat light0_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT0);

	GLfloat light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light1_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	GLfloat light1_position[] = { 0.0f, 1.0f, 1.0f, 0.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glEnable(GL_LIGHT1);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	DrawAxes();

	DrawSphere(50, 30, 15);


	glPushMatrix();
	DrawFlower();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-9, 0, -9);
	DrawFlower();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(9, 0, -9);
	DrawFlower();
	glPopMatrix();

	/*glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 10);
	glEnd();*/

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
	glDisable(GL_LIGHTING);

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

	glEnable(GL_LIGHTING);

}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(CString(fileName));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::PolarToCartesian(double R, double alfa, double beta, double& x, double& y, double& z)
{
	x = R * sin(beta) * cos(alfa);
	y = R * cos(beta);
	z = R * sin(beta) * sin(alfa);
}

void CGLRenderer::DrawSphere(float R, int n, int m)
{
	glPushMatrix();

	glRotatef(-75, 0, 1, 0);
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	double alfa = 2 * 3.14 / n;
	double beta = 3.14 / m;


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, env);

	/*glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);
	glVertex3f(0, 10, 0);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f(1, 0);
	glVertex3f(10, 0, 0);

	glTexCoord2f(1, 1);
	glVertex3f(10, 10, 0);

	glEnd();*/

	for (int i = 0; i < n; i++) {

		for (int j = 0; j < m; j++) {
			double x1;
			double y1;
			double z1;

			PolarToCartesian(R, alfa*i, beta*j, x1, y1, z1);

			double x2;
			double y2;
			double z2;

			PolarToCartesian(R, alfa * (i+1), beta * j, x2, y2, z2);

			double x3;
			double y3;
			double z3;

			PolarToCartesian(R, alfa * (i + 1), beta * (j + 1), x3, y3, z3);


			double x4;
			double y4;
			double z4;

			PolarToCartesian(R, alfa * (i), beta * (j + 1), x4, y4, z4);

			double u1 = (double)i / n;
			double v1 = (double)j / m;
			double u2 = (double)(i + 1) / n;
			double v2 = v1;
			double u3 = u2;
			double v3 = (double)(j + 1) / m;
			double u4 = u1;
			double v4 = v3;

			glBegin(GL_QUADS);
				//glColor3f(1/x1, 1/y1, 1/z1);
				glTexCoord2f(u1, v1); glVertex3f(x1, y1, z1);
				glTexCoord2f(u2, v2); glVertex3f(x2, y2, z2);
				glTexCoord2f(u3, v3); glVertex3f(x3, y3, z3);
				glTexCoord2f(u4, v4); glVertex3f(x4, y4, z4);
			glEnd();
		}
	}

	glPopMatrix();

	glEnable(GL_LIGHTING);

}

void CGLRenderer::CalcRotAxis(double x1, double y1, double z1, double x2, double y2, double z2, double& x3, double& y3, double& z3)
{

	double vx = x2 - x1;
	double vy = y2 - y1;
	double vz = z2 - z1;


	double L = sqrt(vx * vx + vy * vy + vz * vz);

	if (L != 0.0) {
		x3 = vx / L;
		y3 = vy / L;
		z3 = vz / L;
	}
	else {
		x3 = y3 = z3 = 0.0;
	}
}

void CGLRenderer::DrawSphFlower(float R, int n, int m, float factor, unsigned char R1, unsigned char G1, unsigned char B1, unsigned char R2, unsigned char G2, unsigned char B2)
{
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	

	double alfa = 2 * 3.14 / n;
	double beta = 3.14 / m;



	for (int i = 0; i < n; i++) {
		glPushMatrix();
		for (int j = 0; j < m; j++) {

			

			double x1;
			double y1;
			double z1;

			PolarToCartesian(R, alfa * i, beta * j, x1, y1, z1);

			double x2;
			double y2;
			double z2;

			PolarToCartesian(R, alfa * (i + 1), beta * j, x2, y2, z2);

			double x3;
			double y3;
			double z3;

			PolarToCartesian(R, alfa * (i + 1), beta * (j + 1), x3, y3, z3);


			double x4;
			double y4;
			double z4;

			PolarToCartesian(R, alfa * (i), beta * (j + 1), x4, y4, z4);

			float t = (float)j / (float)m;

			// difuzna boja
			GLfloat diffuse[4];
			diffuse[0] = ((1 - t) * R2 + t * R1) / 255.0f;
			diffuse[1] = ((1 - t) * G2 + t * G1) / 255.0f;
			diffuse[2] = ((1 - t) * B2 + t * B1) / 255.0f;
			diffuse[3] = 1.0f;

			// ambijentalna boja = 20% južnog pola
			GLfloat ambient[4];
			ambient[0] = 0.2f * (R1 / 255.0f);
			ambient[1] = 0.2f * (G1 / 255.0f);
			ambient[2] = 0.2f * (B1 / 255.0f);
			ambient[3] = 1.0f;

			// spekularna = bela
			GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat shininess = 50.0f;

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);


			double axisX, axisY, axisZ;

			CalcRotAxis(x1, y1, z1, x2, y2, z2, axisX, axisY, axisZ);

			glTranslatef(x1, y1, z1);
			glRotatef(angleRF * factor, axisX, axisY, axisZ);
			glTranslatef(-x1, -y1, -z1);


			glBegin(GL_QUADS);
				glNormal3f(x1/R, y1/R, z1/R); glVertex3f(x1, y1, z1);
				glNormal3f(x2 / R, y2 / R, z2 / R); glVertex3f(x2, y2, z2);
				glNormal3f(x3 / R, y3 / R, z3 / R); glVertex3f(x3, y3, z3);
				glNormal3f(x4 / R, y4 / R, z4 / R); glVertex3f(x4, y4, z4);
			glEnd();
		}
		glPopMatrix();
	}

	glPopMatrix();


}

void CGLRenderer::DrawFlower()
{
	DrawSphFlower(2, 36, 18, 1, 0, 192, 0, 0, 255, 0);
	DrawSphFlower(2 * 0.75, 36, 18, 0.5, 0, 0, 255, 255, 0, 0);
	DrawSphFlower(2 * 0.5, 36, 18, 0.25, 192, 192, 0, 192, 192, 192);
}
