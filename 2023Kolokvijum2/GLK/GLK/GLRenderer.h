#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,


	double alfaCam = 1;
	double betaCam = 1;
	double rCam = 70;


	void DrawT();
	void DrawAxes();
	UINT LoadTexture(char* fileName);

	void DrawEnvCube(double a);
	void DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen);
	void DrawLampBase();
	void DrawLampArm();
	void DrawLampHead();
	void DrawLamp();

	double angleArm1 = 0;
	double angleArm2 = 0;
	double angleHead = 0;

	UINT t, b;
	UINT o[4];
	UINT lampa;
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
