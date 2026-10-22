#pragma once
#include "CUi.h"
#include "CUI_BanchoImage.h"

class  CCustomer;

namespace Engine
{
	class CTexture;
	class CAnimator;
	class CTransform;
}

class CUI_Customer : public CUi
{
public:
	explicit CUI_Customer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_Customer();

public:
	virtual			HRESULT		Ready_GameObject(CCustomer* m_pOwner);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void Set_Owner(CCustomer* pOwner) { m_pOwner = pOwner; }


private:
	HRESULT Add_Component();
	HRESULT Ready_Material();
	void Ready_Animation();
	void Enter_Animation(_int eNewAnim);


private:
	Engine::CRcTex* m_pBufferCom = nullptr;

	CUI_BanchoImage* m_pOrderBase = nullptr;
	CUI_BanchoImage* m_pOrderBaseFull = nullptr;
	CUI_BanchoImage* m_pOrderGreenTea = nullptr;
	CUI_BanchoImage* m_pOrderGreenTeaFull = nullptr;
	CUI_BanchoImage* m_pOrderGreenTeaIcon = nullptr;
	CUI_BanchoImage* m_pOrderSushi = nullptr;

	Engine::CTexture* m_pTextureCom_Choose_Menu = nullptr;
	Engine::CTexture* m_pTextureCom_Gold = nullptr;
	Engine::CTexture* m_pTextureCom_GreenTeaEffect = nullptr;

	Engine::CTransform* m_pGreenTeaTransformCom = nullptr;

	Engine::CAnimator* m_pAnimator = nullptr;
	Engine::CAnimator* m_pGreenTeaAnimator = nullptr;

public:
	static CUI_Customer* Create(LPDIRECT3DDEVICE9 pGraphicDev, CCustomer* m_pOwner);

private:
	virtual void Free();

private:
	CCustomer* m_pOwner;
	_float m_fOffsetY;
	_float m_fOffsetX;

	_int m_eCurAnim = -1;
	_int m_ePrevAnim = -1;

	_bool m_bAnim;
	_bool m_bGreenTeaAnim;
};

