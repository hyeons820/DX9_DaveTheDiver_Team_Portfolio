#include "CFactoryMgr.h"

IMPLEMENT_SINGLETON(CFactoryMgr)

CFactoryMgr::CFactoryMgr()
{

}

CFactoryMgr::~CFactoryMgr()
{
	Free();
}

void CFactoryMgr::Free()
{
	m_mapObjCount.clear();
}

HRESULT CFactoryMgr::Insert_Object(CGameObject* pObj, const wchar_t* pTag, CLayer* pLayer)
{
	if (pObj == nullptr)
		return E_FAIL;

	Generate_Name(&pObj, pTag);

	if (FAILED(pLayer->Add_GameObject(pObj->Get_Name().c_str(), pObj)))
		return E_FAIL;

	return S_OK;
}

 void CFactoryMgr::Generate_Name(CGameObject** ppGameObject, const wchar_t* pName) {
	wstring wstrName(pName);

	int& iCnt = m_mapObjCount[wstrName];

	wstring wstrFinalName = wstrName + L"(" + std::to_wstring(iCnt) + L")";

	(*ppGameObject)->Set_Name(wstrFinalName);

	++iCnt;
}

 void CFactoryMgr::Reset_ObjectCount()
 {
	 m_mapObjCount.clear();
 }