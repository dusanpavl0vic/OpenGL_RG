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
	double moonr = 0;

	UINT m[6];
	UINT s[6];
	UINT tsc[6];

	UINT LoadTexture(char* fileName);

	void DrawAxes();
	void DrawPatch(double R, int n);

	void inverseTSC(double x,
		double y, double& phi, double& theta);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);


	bool ukljuciS = false;

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
