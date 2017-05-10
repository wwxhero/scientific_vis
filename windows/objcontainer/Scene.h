#pragma once
#include "object3d.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
class CCamera
{
public:
	CCamera()
	{
		m_world2view = glm::lookAt( glm::vec3(0, 0, 3), // Camera position in World Space
	                    glm::vec3(0, 0, 0), // and looks at the origin
	                    glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
	                  );
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		m_view2clip = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	}
	const Matrix4x4& World2View()
	{
		return m_world2view;
	}
	const Matrix4x4& View2Clip()
	{
		return m_view2clip;
	}

	void OnGLSize(int cx, int cy)
	{
		m_view2clip = glm::perspective(45.0f, float(cx)/float(cy), 0.1f, 100.0f);
	}

private:
	Matrix4x4 m_world2view;
	Matrix4x4 m_view2clip;
};

class CScene :
	public CObject3D
{
public:
	DECLARE_SERIAL( CScene )
	CScene(void);
	virtual ~CScene(void);


	virtual void RemoveSelf()
	{
	}

	CCamera* GetCamera()
	{
		return &m_camera;
	}

	virtual void glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c);

private:
	CCamera m_camera;
};

