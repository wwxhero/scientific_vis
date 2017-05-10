#pragma once
#include "TreeNodePersist.h"
// CObject3D command target



class CObject3D : public CObject
				, public CTreeNodePersist<CObject3D>
{
	friend class CTreeNodePersist<CObject3D>;
	friend class CScene;
public:
	DECLARE_SERIAL( CObject3D )
	CObject3D();
	virtual ~CObject3D();
	void DumpData();

	void AddChild(CObject3D*);
	CObject3D* GetFirstChild() const
	{
		return m_firstChild;
	}
	CObject3D* GetNextSibbling() const
	{
		return m_nextSibbling;
	}
	void GetName(CString& strName) const
	{
		strName = m_strName;
	}

	void Model2World(Matrix4x4& model2world)
	{
		model2world = glm::mat4(1.0f);
	}


	virtual void RemoveSelf();

	virtual void glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c);
	virtual void glUpdate();
	virtual void glDestroy();

	static bool Connect(CObject3D* parent, CObject3D* child);

	static bool SetName(CObject3D* pThis, const _variant_t& vName);
	static bool GetName(const CObject3D* pThis, _variant_t& vName);
	static bool SetPosX(CObject3D* pThis, const _variant_t& posX);
	static bool GetPosX(const CObject3D* pThis, _variant_t& posX);
	static bool SetPosY(CObject3D* pThis, const _variant_t& posY);
	static bool GetPosY(const CObject3D* pThis, _variant_t& posY);
	static bool SetPosZ(CObject3D* pThis, const _variant_t& posZ);
	static bool GetPosZ(const CObject3D* pThis, _variant_t& posZ);
	static bool SetRotX(CObject3D* pThis, const _variant_t& rotX);
	static bool GetRotX(const CObject3D* pThis, _variant_t& rotX);
	static bool SetRotY(CObject3D* pThis, const _variant_t& rotY);
	static bool GetRotY(const CObject3D* pThis, _variant_t& rotY);
	static bool SetRotZ(CObject3D* pThis, const _variant_t& rotZ);
	static bool GetRotZ(const CObject3D* pThis, _variant_t& rotZ);
	static bool SetScaleX(CObject3D* pThis, const _variant_t& scaleX);
	static bool GetScaleX(const CObject3D* pThis, _variant_t& scaleX);
	static bool SetScaleY(CObject3D* pThis, const _variant_t& scaleY);
	static bool GetScaleY(const CObject3D* pThis, _variant_t& scaleY);
	static bool SetScaleZ(CObject3D* pThis, const _variant_t& scaleZ);
	static bool GetScaleZ(const CObject3D* pThis, _variant_t& scaleZ);

private:
	virtual void Serialize(CArchive& ar);

	Matrix4x4 m_local2parent;
	Matrix4x4 m_parent2local;

	CObject3D* m_parent;
	CObject3D* m_firstChild;
	CObject3D* m_nextSibbling;
protected:
	CString m_strName;
	Vector3 m_pos;
	Vector3 m_rot; //in angle
	Vector3 m_scale;


	GLuint m_programID;



};


