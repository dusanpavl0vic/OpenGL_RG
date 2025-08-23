#pragma once

const double PI = 3.14159;

class CGLRenderer
{
private:
	UINT lamp, side, top, bot;
public:
	double alpha = 0.1, beta = PI/2;
	double zoom = 0;
	double a1 = -45, a2 = 90, a3 = -90;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	void DrawAxes();
	UINT LoadTexture(char* fileName);
	void DrawQuad(double a);
	void DrawEnvCube(double a);
	void DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen);
	void DrawLampBase();
	void DrawLampArm();
	void DrawLampHead();
	void DrawLamp();


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
