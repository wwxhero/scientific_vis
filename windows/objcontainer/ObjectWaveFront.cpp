#include "StdAfx.h"
#include "ObjectWaveFront.h"

CShaderCompiler CObjectWaveFront::s_shader("E:\\Users\\wanxwang\\scientific_vis\\course_project\\objview\\windows\\Debug\\CObjectWaveFront", CObjectWaveFront::s_programID, CObjectWaveFront::s_vIDs);
GLuint CObjectWaveFront::s_programID = 0;
GLuint CObjectWaveFront::s_vIDs[CShaderCompiler::Total] = {0};


void CShaderCompiler::Build()
{
	enum {vert = 0, frag, total};
	LPSTR paths[total] = {NULL};
	LPCSTR suffix[total] = {".vert", ".frag"};
	int lenStub = strlen(m_path);
	for (int i = 0; i < sizeof(suffix)/sizeof(LPCSTR); i ++)
	{
		paths[i] = (LPSTR)malloc((lenStub + strlen(suffix[i])) + 1);
		sprintf(paths[i], "%s%s", m_path, suffix[i]);
	}
	r_programID = LoadShaders(paths[vert], paths[frag]);
	for (int i = 0; i < sizeof(suffix)/sizeof(LPCSTR); i ++)
		free(paths[i]);
	if (0 != r_programID)
	{
		// Get a handle for our m_model, m_view and projection uniforms
		r_vIDs[M2W] = glGetUniformLocation(r_programID, "model2world");
		r_vIDs[W2V] = glGetUniformLocation(r_programID, "world2view");
		r_vIDs[V2C] = glGetUniformLocation(r_programID, "view2clip");
		r_vIDs[NORMAL] = glGetUniformLocation(r_programID, "model2view");
		glm::vec4 light_ambient = glm::vec4( 0.1, 0.1, 0.1, 0.5 );
		glm::vec4 light_diffuse = glm::vec4 ( 0.8, 1.0, 1.0, 1.0 );
		glm::vec4 light_specular = glm::vec4( 0.8, 1.0, 1.0, 1.0 );
		glUseProgram(r_programID);
		glUniform4fv( glGetUniformLocation(r_programID, "light_ambient"), 1, &light_ambient[0]);
		glUniform4fv( glGetUniformLocation(r_programID, "light_diffuse"), 1, &light_diffuse[0]);
		glUniform4fv( glGetUniformLocation(r_programID, "light_specular"), 1, &light_specular[0]);
		// Initialize a light
		glm::vec4 lightPosition = glm::vec4(-20, -10, 0, 0);
		glUniform4fv( glGetUniformLocation(r_programID, "lightPos"), 1, &lightPosition[0]);
	}
}

IMPLEMENT_SERIAL(CObjectWaveFront, CObject3D, 1)

CObjectWaveFront::CObjectWaveFront(void) : m_objModel(NULL)
{
	m_strName = _T("Wavefront");

	m_objModel = glmReadOBJ("E:\\Users\\wanxwang\\scientific_vis\\course_project\\objview\\data\\al.obj");
	// Normilize vertices
	glmUnitize(m_objModel);
	// Compute facet normals
	glmFacetNormals(m_objModel);
	// Comput vertex normals
	glmVertexNormals(m_objModel, 90.0);

}


CObjectWaveFront::~CObjectWaveFront(void)
{
	if (NULL != m_objModel)
	{
		glmDelete(m_objModel);
		m_objModel = NULL;
	}
}

void CObjectWaveFront::glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c)
{
	// Load the m_model (vertices and normals) into a vertex buffer
	glmLoadInVBO(m_objModel);

	GLuint programID = (0 == m_programID ? CObjectWaveFront::s_programID : m_programID);
	ASSERT(0 != programID);
	Matrix4x4 model2world;
	Model2World(model2world);
	glUseProgram(programID);

	// Send the m_model, m_view and projection matrices to the shader
	glUniformMatrix4fv(s_vIDs[CShaderCompiler::M2W], 1, GL_FALSE, &model2world[0][0]);
	glUniformMatrix4fv(s_vIDs[CShaderCompiler::W2V], 1, GL_FALSE, &w2v[0][0]);
	glUniformMatrix4fv(s_vIDs[CShaderCompiler::V2C], 1, GL_FALSE, &v2c[0][0]);
	glm::mat4 model2view = w2v * model2world;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model2view)); // Normal Matrix
	glUniformMatrix3fv(s_vIDs[CShaderCompiler::NORMAL], 1, GL_FALSE, &normalMatrix[0][0]);

	glmDrawVBO(m_objModel, programID);

}
