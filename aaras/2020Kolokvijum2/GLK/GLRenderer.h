#pragma once

class CGLRenderer
{
private:
	UINT back;
	UINT bot;
	UINT front;
	UINT left;
	UINT right;
	UINT spider;
	UINT top;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);						// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);						// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);				// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC, int xt, int yt, int zt);	// iscrtava scenu
	void DestroyScene(CDC* pDC);						// dealocira resurse alocirane u drugim funkcijama ove klase,
	void DrawAxes();
	UINT LoadTexture(char* fileName);
	void DrawQuad(double a);
	void DrawEnvCube(double a);
	void DrawSphere(double r, int nSeg, double texU, double texV, double texR);
	void DrawCone(double r, double h, int nSeg, double texU, double texV, double texR);
	void DrawLegSegment(double r, double h, int nSeg);
	void DrawLeg();
	void DrawSpiderBody();
	void DrawSpider();


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
