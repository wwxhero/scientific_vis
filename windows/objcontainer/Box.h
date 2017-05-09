#pragma once
#include "object3d.h"
class CBox :
	public CObject3D
{
public:
	DECLARE_SERIAL( CBox )
	CBox(void);
	virtual ~CBox(void);

};

