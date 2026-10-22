#pragma once
#include "CUi.h"
#include "CUI_BanchoImage.h"

class  CBancho;

class CUI_Bancho : public CUi
{
public:
	struct COOKING_SLOT {
		CUI_BanchoImage* pBase;
		CUI_BanchoImage* pGauge;
		CUI_BanchoImage* pSushi;
	};

	struct UI_RENDER_DATA {
		_int   iSushiIdx; // 음식 이미지 인덱스
		_float fRatio;    // 게이지 비율 (0.0 ~ 1.0)
		_bool  bIsDone;   // 음식 완료 여부
	};

public:
	explicit CUI_Bancho(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_Bancho();

public:
	virtual			HRESULT		Ready_GameObject(CBancho* m_pOwner);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void Set_Owner(CBancho* pOwner) { m_pOwner = pOwner; }

private:
	CUI_BanchoImage* m_pCookingBase0 = nullptr;
	CUI_BanchoImage* m_pCookingGauge0 = nullptr;
	CUI_BanchoImage* m_pCookingSushi0 = nullptr;
	
	CUI_BanchoImage* m_pCookingBase1 = nullptr;
	CUI_BanchoImage* m_pCookingGauge1 = nullptr;
	CUI_BanchoImage* m_pCookingSushi1 = nullptr;
	
	CUI_BanchoImage* m_pCookingBase2 = nullptr;
	CUI_BanchoImage* m_pCookingGauge2 = nullptr;
	CUI_BanchoImage* m_pCookingSushi2 = nullptr;
	
	CUI_BanchoImage* m_pCookingBase3 = nullptr;
	CUI_BanchoImage* m_pCookingGauge3 = nullptr;
	CUI_BanchoImage* m_pCookingSushi3 = nullptr;

public:
	static CUI_Bancho* Create(LPDIRECT3DDEVICE9 pGraphicDev, CBancho* pOwner);

private:
	virtual void Free();

private:
	CBancho* m_pOwner;

	_float m_fOffsetY;
	_float m_fOffsetX;

	COOKING_SLOT m_tCookingSlots[4];
};

