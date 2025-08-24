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

	excavator = LoadTexture("excavator.png");
	front = LoadTexture("front.jpg");
	left = LoadTexture("left.jpg");
	right = LoadTexture("right.jpg");
	back = LoadTexture("back.jpg");
	top = LoadTexture("top.jpg");
	bot = LoadTexture("bot.jpg");
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

	gluLookAt(
		x, y, z,
		0, 0, 0,
		0, 1, 0
	);
	
	//glDisable(GL_DEPTH_TEST);

	//(GL_FRONT);
	DrawEnvCube(30);
	//glCullFace(GL_BACK);


	glEnable(GL_DEPTH_TEST);


	DrawAxes();

	DrawExcavator();

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
	gluPerspective(55, (double)w/(double)h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
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
	glVertex3f(10, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_FAN);

		for (int i = 0; i < n; i++) {

			glTexCoord2f(texCoords[i].x, texCoords[i].y);
			glVertex3f(points[i].x, points[i].y, 0);
		}
	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glPushMatrix();

	DrawPolygon(points, texCoords, n);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUAD_STRIP);

		for (int i = 0; i < n; i++) {
			glColor3f(r, g, b);
			glVertex3f(points[i].x, points[i].y, 0);
			glVertex3f(points[i].x, points[i].y, zh);
		}

		glVertex3f(points[0].x, points[0].y, 0);
		glVertex3f(points[0].x, points[0].y, zh);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);

	glPopMatrix();

}

void CGLRenderer::DrawBody()
{
	glPushMatrix();

	glTranslatef(0, 0, -2);
	//glRotatef(body, 0, 1, 0);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 5;
	POINTF points[5] = { {-2, 4}, {-2, 0}, {2, 0}, {2, 2}, {0, 4} };
	POINTF texCoords[5] = { {0.5, 0}, {0.5, 0.5}, {1, 0.5}, {1, 0.25}, {0.75, 0} };
	DrawExtrudedPolygon(points, texCoords, n, 4, 0.96, 0.5, 0.12);

	glPopMatrix();
}

void CGLRenderer::DrawArm(double zh)
{
	glPushMatrix();

	glRotatef(90, 0, 0, 1);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 8;
	float pom = 0.0625;
	POINTF points[8] = { {-0.5, 1}, {-1,0.5}, {-1, -0.5}, {-0.5, -1}, {1, -1} , {7, -0.5}, {7, 0.5}, {1, 1}};
	POINTF texCoords[8] = { {pom * 1, pom * 8}, {pom * 0, pom * 9}, {pom * 0, pom * 11}, {pom * 1, pom * 12}, {pom * 4, pom * 12}, {pom * 16, pom * 11}, {pom * 16, pom * 9}, {pom * 4, pom * 8} };
	DrawExtrudedPolygon(points, texCoords, n, zh, 0.96, 0.5, 0.12);

	glPopMatrix();
}

void CGLRenderer::DrawFork()
{
	glPushMatrix();

	glRotatef(fork, 0, 0, 1);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 6;
	float pom = 0.0625;
	POINTF points[6] = { {3, -1}, {3,1.5}, {2.0, 2.}, {-0.5, 2}, {-1, 1.5} , {-1, -1}};
	POINTF texCoords[6] = { {pom * 8, pom * 6}, {pom * 8, pom * 1}, {pom * 7, pom * 0}, {pom * 1, pom * 0}, {pom * 0, pom * 1}, {pom * 0, pom * 6}};
	DrawExtrudedPolygon(points, texCoords, n, 1, 0.7, 0.7, 0.7);

	glPopMatrix();
}

void CGLRenderer::DrawExcavator()
{
	glPushMatrix();

	glColor3f(1, 1, 1);


	DrawBase();
	glTranslatef(0, 2, 0);
	
	glRotatef(body, 0, 1, 0);

	DrawBody();
	glTranslatef(1, 1, -1-2);
	glRotatef(arm1, 0, 0, 1);
	DrawArm(1);
	glTranslatef(0, 6.5, 0);
	glRotatef(arm2-90, 0, 0, 1);
	DrawArm(1.5);
	glTranslatef(0, 6.5, 0);
	DrawFork();



	glPopMatrix();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, top);

	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex3f(-a/2, a, -a/2);

		glTexCoord2f(0,1);
		glVertex3f(-a/2, a, a/2);

		glTexCoord2f(1, 1);
		glVertex3f(a/2, a, a/2);

		glTexCoord2f(1,0);
		glVertex3f(a/2, a, -a/2);


	glEnd();

	glBindTexture(GL_TEXTURE_2D, bot);

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


	glBindTexture(GL_TEXTURE_2D, front);
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

	glBindTexture(GL_TEXTURE_2D, right);
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

	glBindTexture(GL_TEXTURE_2D, back);
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

	glBindTexture(GL_TEXTURE_2D, left);
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
}

void CGLRenderer::DrawBase()
{
	glPushMatrix();

	glTranslatef(0, 0, -2.5);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 8;
	float pom = 0.0625;
	POINTF points[8] = { {-3.5, 2}, {-4,1.5}, {-4, 0.5}, {-3.5, 0}, {3.5, 0}, {4, 0.5}, {4, 1.5}, {3.5, 2} };
	POINTF texCoords[8] = { {pom*1, pom*12}, {pom*0, pom*13}, {pom * 0, pom * 15}, {pom * 1, pom * 16}, {pom * 15, pom * 16}, {pom * 16, pom * 15}, {pom * 16, pom * 13}, {pom * 15, pom * 12} };
	DrawExtrudedPolygon(points, texCoords, n, 5, 0, 0, 0);

	glPopMatrix();
}
