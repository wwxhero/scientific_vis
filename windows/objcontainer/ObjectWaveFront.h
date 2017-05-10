#pragma once
#include "object3d.h"
#include "libobj.h"
class CShaderCompiler
{
public:
	enum VARID {M2W = 0, W2V, V2C, NORMAL, Total};
	CShaderCompiler(LPCSTR path, GLuint& programID, GLuint* vIDs)
	 : m_path(NULL)
	 , r_programID(programID)
	 , r_vIDs(vIDs)
	{
		m_path = (LPSTR)malloc(strlen(path)+1);
		strcpy(m_path, path);
	}
	~CShaderCompiler()
	{
		free(m_path);
	}

	void Build();
private:
	LPSTR m_path;
	GLuint& r_programID;
	GLuint* r_vIDs;
};

class CObjectWaveFront :
	public CObject3D
{
public:
	DECLARE_SERIAL( CObjectWaveFront )
	CObjectWaveFront(void);
	virtual ~CObjectWaveFront(void);
	void glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c);
private:
	GLMmodel* m_objModel;
private:
	static GLuint s_programID;
	static GLuint s_vIDs[CShaderCompiler::Total];
public:
	static CShaderCompiler s_shader;
};

