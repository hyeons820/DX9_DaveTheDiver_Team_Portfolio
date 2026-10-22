#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "CLayer.h"

BEGIN(Engine)

class ENGINE_DLL CFactoryMgr : public CBase
{
	DECLARE_SINGLETON(CFactoryMgr)

private:
	explicit CFactoryMgr();
	virtual ~CFactoryMgr();

public:
	HRESULT Insert_Object(CGameObject* pObj, const wchar_t* pTag, CLayer* pLayer);
	void Generate_Name(CGameObject** ppGameObject, const wchar_t* pName);
	void Reset_ObjectCount();

private:
	map<wstring, int> m_mapObjCount;

private:
	virtual void Free();
};

END