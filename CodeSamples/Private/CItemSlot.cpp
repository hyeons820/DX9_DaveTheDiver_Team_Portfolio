#include "pch.h"
#include "CItemSlot.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CFontMgr.h"

using namespace SLOT_OFFSET;

CItemSlot::CItemSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_FishData(nullptr), m_bActive(true), m_iIndex(-1), m_bSelect(false)
{
}

CItemSlot::~CItemSlot()
{
}

HRESULT CItemSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Set_UISequence(UI_THR);

	m_pTransformCom->m_vScale = { 192.f , 35.f, 1.f };

	return S_OK;
}

_int CItemSlot::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_FishData)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CItemSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_FishData)
		return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CItemSlot::Render_GameObject()
{
	if (!m_FishData || !m_bActive)
		return;

	// 1. 슬롯 배경 그리기
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pSlotBaseTex->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// 슬롯의 현재 월드 좌표 추출
	_vec3 vParentPos = m_pTransformCom->m_vInfo[INFO_POS];

	// 2. 물고기 아이콘 렌더링
	_matrix matIconWorld;
	D3DXMatrixIdentity(&matIconWorld);

	// 크기 설정
	_vec3 vIconScale = { SLOT_OFFSET::FISH_ICON_SIZE.x, SLOT_OFFSET::FISH_ICON_SIZE.y, 1.f };
	// 위치 설정 (부모 위치 + 오프셋)
	_vec3 vIconPos = vParentPos + _vec3(SLOT_OFFSET::FISH_ICON_POS.x, SLOT_OFFSET::FISH_ICON_POS.y, 0.f);

	D3DXMatrixTransformation(&matIconWorld, NULL, NULL, &vIconScale, NULL, NULL, &vIconPos);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matIconWorld);
	m_pFishIconTex->Set_Texture(m_FishData->Id); // 데이터에 맞는 텍스처
	m_pBufferCom->Render_Buffer();
	
	// 3. 별 아이콘 렌더링
	_matrix matStarWorld;
	D3DXMatrixIdentity(&matStarWorld);

	_vec3 vStarnScale = { SLOT_OFFSET::RANK_STAR_SIZE.x, SLOT_OFFSET::RANK_STAR_SIZE.y, 1.f };
	_vec3 vStarPos = vParentPos + _vec3(SLOT_OFFSET::RANK_STAR_POS.x, SLOT_OFFSET::RANK_STAR_POS.y, 0.f);

	D3DXMatrixTransformation(&matIconWorld, NULL, NULL, &vStarnScale, NULL, NULL, &vStarPos);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matIconWorld);
	m_pStarTex->Set_Texture(m_FishData->Star - 1); // 데이터에 맞는 텍스처
	m_pBufferCom->Render_Buffer();

	// 4. 이름 폰트 출력

	// 비율 계산
	float fScaleX = (float)WINCX / 800.f;
	float fScaleY = (float)WINCY / 600.f;

	// 폰트의 화면 중앙(원점) 좌표
	float fScreenCenterX = WINCX * 0.5f;
	float fScreenCenterY = WINCY * 0.5f;

	// 슬롯 위치(vParentPos)에 비율을 적용하여 원점으로부터의 실제 픽셀 거리 계산
	_vec2 vNamePos;
	vNamePos.x = fScreenCenterX + (vParentPos.x * fScaleX);
	vNamePos.y = fScreenCenterY - (vParentPos.y * fScaleY); 
	_vec2 vFinalNamePos = { vNamePos.x + SLOT_OFFSET::NAME_TEXT_POS.x, vNamePos.y + SLOT_OFFSET::NAME_TEXT_POS.y };
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_FishName", m_FishData->Korean_Name.c_str(), &vFinalNamePos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	
	_vec2 vRankPos;
	vRankPos.x = fScreenCenterX + (vParentPos.x * fScaleX);
	vRankPos.y = fScreenCenterY - (vParentPos.y * fScaleY); 
	_vec2 vFinalRankPos = { vRankPos.x + SLOT_OFFSET::RANK_TEXT_POS.x, vRankPos.y + SLOT_OFFSET::RANK_TEXT_POS.y };
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_FishRank", to_wstring(m_FishData->Rank).c_str(), &vFinalRankPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));	
	
	_vec2 vWeightPos;
	vWeightPos.x = fScreenCenterX + (vParentPos.x * fScaleX);
	vWeightPos.y = fScreenCenterY - (vParentPos.y * fScaleY); 
	_vec2 vFinalWeightPos = { vWeightPos.x + SLOT_OFFSET::WEIGHT_TEXT_POS.x, vWeightPos.y + SLOT_OFFSET::WEIGHT_TEXT_POS.y };
	wchar_t szWeight[16];
	swprintf_s(szWeight, L"%.1fkg", m_FishData->Kg);
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_FishRank", szWeight, &vFinalWeightPos, D3DXCOLOR(120.f / 255.f, 169.f / 255.f, 210.f / 255.f, 1.f));
	
	_vec2 vFleshPos;
	vFleshPos.x = fScreenCenterX + (vParentPos.x * fScaleX);
	vFleshPos.y = fScreenCenterY - (vParentPos.y * fScaleY);
	_vec2 vFinalFleshPos = { vWeightPos.x + SLOT_OFFSET::FLESH_TEXT_POS.x, vWeightPos.y + SLOT_OFFSET::FLESH_TEXT_POS.y };
	switch (m_FishData->Star)
	{
	case 1:
		CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_FishRank", to_wstring(m_FishData->Star1_flesh).c_str(), &vFinalFleshPos, D3DXCOLOR(120.f / 255.f, 169.f / 255.f, 210.f / 255.f, 1.f));
		break;
	case 2:
		CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_FishRank", to_wstring(m_FishData->Star2_flesh).c_str(), &vFinalFleshPos, D3DXCOLOR(120.f / 255.f, 169.f / 255.f, 210.f / 255.f, 1.f));
		break;
	case 3:
		CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_FishRank", to_wstring(m_FishData->Star3_flesh).c_str(), &vFinalFleshPos, D3DXCOLOR(120.f / 255.f, 169.f / 255.f, 210.f / 255.f, 1.f));
		break;
	}

	// 5. 선택 시 렌더
	if (m_bSelect)
	{
		_matrix matSelectWorld;
		D3DXMatrixIdentity(&matSelectWorld);

		_vec3 vSelectScale = { SLOT_OFFSET::SELECT_SIZE.x, SLOT_OFFSET::SELECT_SIZE.y, 1.f };
		_vec3 vSelectrPos = vParentPos + _vec3(SLOT_OFFSET::SELECT_POS.x, SLOT_OFFSET::SELECT_POS.y, 0.f);

		D3DXMatrixTransformation(&matSelectWorld, NULL, NULL, &vSelectScale, NULL, NULL, &vSelectrPos);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matSelectWorld);
		m_pSelectTex->Set_Texture(0); // 데이터에 맞는 텍스처
		m_pBufferCom->Render_Buffer();
	}
}

HRESULT CItemSlot::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

#pragma region Texture

	// Texture
	pComponent = m_pSlotBaseTex = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Inventory_ItemSlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_SlotTexture", pComponent });

	// Texture
	pComponent = m_pFishIconTex = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_UI_Fish"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_FishTexture", pComponent });

	// Texture
	pComponent = m_pStarTex = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Star_Icon"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_StarTexture", pComponent });
	
	// Texture
	pComponent = m_pSelectTex = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Equip_SelectPlate"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_SelectTexture", pComponent });

#pragma endregion

	return S_OK;
}

_bool CItemSlot::Is_MouseOver(POINT ptMouse)
{
	// 1. 현재 실제 윈도우 해상도와 기준 해상도의 비율 계산
	float fScaleX = (float)WINCX / 800.f; // 1366 / 800
	float fScaleY = (float)WINCY / 600.f; // 768 / 600

	// 2. 슬롯의 중심 좌표를 스크린 좌표로 변환
	float fCenterX = (m_pTransformCom->m_vInfo[INFO_POS].x + 400.f) * fScaleX;
	float fCenterY = (-m_pTransformCom->m_vInfo[INFO_POS].y + 300.f) * fScaleY;

	// 3. 슬롯의 크기(Scale)도 비율에 맞춰 확장
	float fHalfSizeX = (m_pTransformCom->m_vScale.x) * fScaleX;
	float fHalfSizeY = (m_pTransformCom->m_vScale.y);

	// 4. 최종 스크린 기준 RECT 생성
	RECT rc = 
	{
		(long)(fCenterX - fHalfSizeX),
		(long)(fCenterY - fHalfSizeY),
		(long)(fCenterX + fHalfSizeX),
		(long)(fCenterY + fHalfSizeY)
	};

	// 5. 영역 판정 (Windows RECT는 top < bottom 임에 주의)
	if (PtInRect(&rc, ptMouse))
		return true;

	return false;
}


CItemSlot* CItemSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CItemSlot* pItemSlot = new CItemSlot(pGraphicDev);

	if (FAILED(pItemSlot->Ready_GameObject()))
	{
		Safe_Release(pItemSlot);
		MSG_BOX("ItemSlot Create Failed");
		return nullptr;
	}

	return pItemSlot;
}

void CItemSlot::Free()
{
	CGameObject::Free();
}
