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


	void DrawAxes(double a);

	void LookAt(double eyex,
		double eyey,
		double eyez,
		double centerx,
		double centery,
		double centerz,
		double upx,
		double upy,
		double upz);

	void DrawPrism(double x, double y, double z);
	void DrawChest(double x, double y, double z, double d, double alfa, double beta, double gama);
	void DrawRucka(double x, double y, double z, double fi);

	void DrawSnake(double x, double y, double z, double nPart, double alfa);

	double alfa = 0;
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
