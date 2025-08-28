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


	double eyeAlfa = 1;
	double eyeBeta = 1;
	double eyeR = 50;

	UINT CGLRenderer::LoadTexture(char* fileName);

	UINT env;
	UINT brick;

	void CGLRenderer::DrawCube(double a);
	void CGLRenderer::DrawTube(double r1, double r2, double h, int n);

	void CGLRenderer::DrawCone(double r, double h, int n);

	void CGLRenderer::DrawPaddle(double length, double width);


	double vetrenjaca = 0;
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
