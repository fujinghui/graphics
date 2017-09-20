#ifndef _CGfxOpengl_H_
#define _CGfxOpengl_H_
class CGfxOpenGL{
private:
	int m_windowWidth;
	int m_windowHeight;
	float m_angle;
public:
	CGfxOpenGL();
	virtual ~CGfxOpenGL();
	bool Init();
	bool Shutdown();
	void SetupProjection(int width, int height);
	void Prepare(float dt);
	void Render();
};
#endif