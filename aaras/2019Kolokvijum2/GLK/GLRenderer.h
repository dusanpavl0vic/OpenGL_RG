#pragma once

const double PI = 3.14159;

class CGLRenderer
{
private:
	UINT excavator, front, left, right, back, top, bot;

public:
	double phi = 0, theta = PI / 2;
	double r = 30;
	double a1 = 0, a2 = 90, a3 = -90, a4 = -90;

public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	UINT LoadTexture(char* fileName);
	void DrawPolygon(POINTF* points, POINTF* texCoords, int n);
	void DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b);
	void DrawBase();
	void DrawBody();
	void DrawArm(double zh);
	void DrawFork();
	void DrawExcavator();
	void DrawQuad(double a);
	void DrawEnvCube(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
