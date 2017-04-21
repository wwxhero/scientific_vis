#pragma once

// CObject3D command target

class CObject3D : public CObject
{
public:
	DECLARE_SERIAL( CObject3D )
	CObject3D();
	virtual ~CObject3D();
private:
	virtual void Serialize(CArchive& ar);
	void DumpData();

	Matrix4x4 m_local2parent;
	Matrix4x4 m_parent2local;
};


