#pragma once

const double PI = 3.14159;

class CGLRenderer
{
private:
	UINT brick, env;
public:
	double phi = 0, theta = PI / 2;
	double q = 0;
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
	void DrawQuad(double a, double x, double y);
	void DrawCube(double a);
	void DrawTube(double r1, double r2, double h, int n);
	void DrawCone(double r, double h, int n);
	void DrawPaddle(double length, double width);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
