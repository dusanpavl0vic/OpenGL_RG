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
	double eyeR = 20;

	

	void DrawAxes();
	UINT LoadTexture(char* fileName);

	void DrawPolygon(POINTF* points, POINTF* texCoords, int n);

	void DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b);

	void DrawBase();
	void CGLRenderer::DrawBody();
	void CGLRenderer::DrawArm(double zh);
	void CGLRenderer::DrawFork();

	void CGLRenderer::DrawExcavator();

	void CGLRenderer::DrawEnvCube(double a);

	UINT excavator;
	UINT front;
	UINT left;
	UINT right;
	UINT back;
	UINT top;
	UINT bot;

	double arm1 = 0;
	double arm2 = 0;
	double body = 0;
	double fork = 0;



protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
