#pragma once
#include "CGameObject.h"
#include "CGameObjMgr.h"

namespace SLOT_OFFSET 
{
	const _vec2 BASE_POS = { 198.5f, 136.8f };
	const _vec2 BASE_SIZE = { 192.f , 35.f };

	// 1. 물고기 아이콘
	const _vec2 FISH_ICON_POS = { -167.9f, 0.f };  // 슬롯 중심에서 왼쪽으로 
	const _vec2 FISH_ICON_SIZE = { 21.f, 28.f }; // 아이콘 크기

	// 2. 랭크 별 이미지
	const _vec2 RANK_STAR_POS = { -120.f, -11.f }; 
	const _vec2 RANK_STAR_SIZE = { 22.f, 10.f };

	// 3. 아이템 이름 텍스트
	const _vec2 NAME_TEXT_POS = { -145.f, -26.f };
	
	// 4. 랭크 텍스트
	const _vec2 RANK_TEXT_POS = { -187.f, -29.f };
	
	// 5. 무게 텍스트
	const _vec2 WEIGHT_TEXT_POS = { 248.f, 5.f };	
	
	// 6. 살점 텍스트
	const _vec2 FLESH_TEXT_POS = { 190.f, 5.f };

	// 7. 선택 상자 이미지
	const _vec2 SELECT_POS = { -0.8f, 0.2f };
	const _vec2 SELECT_SIZE = { 188.f, 28.f };
}

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CItemSlot : public CGameObject
{
public:
	explicit CItemSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItemSlot();

public:
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	void Set_Select(_bool bSelect) { m_bSelect = bSelect; }
	void Set_FishData(int iIndex, const FishData* FishData) { m_iIndex = iIndex; m_FishData = FishData; }

	int Get_Index() { return m_iIndex; }
	int Get_Active() { return m_bActive; }
	float Get_Weight()
	{
		if (!m_FishData)
			return 0;
		else
			return m_FishData->Kg;
	}
	bool Is_HaveFish() { return m_FishData != nullptr; }
	const FishData Get_FishData() { return *m_FishData; }

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	_bool		Is_MouseOver(POINT ptMouse);


private:
	HRESULT		Add_Component();

private:
	const FishData* m_FishData;

	_bool m_bActive;
	_bool m_bSelect;

	_int m_iIndex;

	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pSlotBaseTex;   // 슬롯 배경 텍스처
	Engine::CTexture* m_pFishIconTex;   // 물고기 아이콘
	Engine::CTexture* m_pStarTex;       // 별 개수
	Engine::CTexture* m_pSelectTex;     // 선택 상자

public:
	static CItemSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};

