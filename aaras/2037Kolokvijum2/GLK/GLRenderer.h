#pragma once

const double PI = 3.14159;

class CGLRenderer
{
private:
	UINT tiles, env, leaf;
public:
	double phi = PI / 8, theta = PI / 2 - PI / 8;
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

	void DrawCone(double r, double h, int n);
	void DrawCylinder(double r1, double r2, double h, int n);
	void DrawDiamond(double r1, double r2, double h, int n);
	void DrawCircle(double r, int n);
	void DrawIciclesLayer(double angle, double s);
	void DrawIcicles();
	void DrawHead();
	void DrawStem();
	void DrawLeaf();
	void DrawBase();
	void DrawSphere(double r, int n);


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
