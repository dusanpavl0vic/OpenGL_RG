#pragma once

class CGLRenderer
{
private:
	double alpha = 1.3;
	double beta = 0.0;
	UINT S[6];
	UINT M[6];
	UINT T[6];
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC, boolean holding, CPoint prev, CPoint curr);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	UINT LoadTexture(char* fileName);
	void DrawPatch(double R, int n);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
