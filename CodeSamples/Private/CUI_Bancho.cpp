#include "pch.h"
#include "CUI_Bancho.h"
#include "CBancho.h"
#include "CRcUITex.h"

CUI_Bancho::CUI_Bancho(LPDIRECT3DDEVICE9 pGraphicDev) : CUi(pGraphicDev), m_fOffsetY(0.f), m_pOwner(nullptr), m_fOffsetX(0.f)
{
}

CUI_Bancho::~CUI_Bancho()
{
}

HRESULT CUI_Bancho::Ready_GameObject(CBancho* pOwner)
{
	Set_Owner(pOwner);

	m_fOffsetY = 40.f;
	m_fOffsetX = 45.f;

	for (_int i = 0; i < 4; ++i)
	{
		_vec2 vPos = { m_pOwner->Get_Position().x, m_pOwner->Get_Position().y + 100.f + (m_fOffsetY * (i + 1))	};

		m_tCookingSlots[i].pBase = CreateBanChoSlot(L"Proto_Cooking", vPos, { 70.f, 20.f }, COM_FILPALPHA);
		m_tCookingSlots[i].pGauge = CreateBanChoSlot(L"Proto_CookingGauge", vPos, { 70.f, 20.f }, COM_FILPALPHA);
		m_tCookingSlots[i].pSushi = CreateBanChoSlot(L"Proto_UI_Sushi_Image", { vPos.x + m_fOffsetX, vPos.y + 1.5f }, { 15.f, 10.f }, COM_FILPALPHA);

		// 초기에는 모두 비활성화
		m_tCookingSlots[i].pBase->Set_Active(false);
		m_tCookingSlots[i].pGauge->Set_Active(false);
		m_tCookingSlots[i].pSushi->Set_Active(false);
	}
	return S_OK;
}

_int CUI_Bancho::Update_GameObject(const _float& fTimeDelta)
{
	if (CGameObject::m_bPause)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	for (auto& pImg : m_vecBanChoImgs)
	{
		pImg->Set_Active(false);
		pImg->Update_GameObject(fTimeDelta);
	}

	// 슬롯에 그려야 할 데이터를 모을 벡터
	vector<UI_RENDER_DATA> vecFinalDraw;

	// 완료된 요리들 벡터에 담기 (4개)
	for (auto& tDish : m_pOwner->Get_CookingList())
	{
		if (vecFinalDraw.size() >= 4) break;
		vecFinalDraw.push_back({ tDish.Sushi_Data.SushiTextureId, 1.0f, true });
	}
	
	// 자리가 남는다면, 현재 요리 중인 요리를 마지막에 추가
	if (m_pOwner->Get_CurState() == CBancho::COOK && vecFinalDraw.size() < 4)
	{
		_float fProgress = m_pOwner->Get_CookTime() / m_pOwner->Get_TargetTime();
		// 2. 픽셀 기반 보정 공식 적용
		// 최종 비율 = 시작점 비율 + (진행률 * 이동 범위)
		_float fRatio = 0.2635f + (fProgress * 0.4662f);

		// (혹시 모를 부동소수점 오차로 1.0을 넘는 것을 방지)
		if (fRatio > 0.7297f) fRatio = 0.7297f;

		vecFinalDraw.push_back({ m_pOwner->Get_FinishedDish_Info().Sushi_Data.SushiTextureId, fRatio, false });
	}

	// 미리 생성해둔 4개의 슬롯에 데이터 입히기
	for (_int i = 0; i < 4; ++i)
	{
		if (i < (_int)vecFinalDraw.size()) // 데이터가 있는 슬롯만 활성화
		{
			m_tCookingSlots[i].pBase->Set_Active(true);
			m_tCookingSlots[i].pGauge->Set_Active(true);
			m_tCookingSlots[i].pSushi->Set_Active(true);

			// 이미지 및 게이지 갱신
			m_tCookingSlots[i].pSushi->Set_TextureNum(vecFinalDraw[i].iSushiIdx);

			// 가로 게이지 적용 (보정치는 픽셀 여백에 따라 조절)
			CRcUITex* pBuffer = static_cast<CRcUITex*>(m_tCookingSlots[i].pGauge->Get_Component(ID_STATIC, L"Com_Buffer"));

			if (pBuffer) 
				pBuffer->Set_SideFillRatio(vecFinalDraw[i].fRatio);

			// 완료 상태라면 베이스 이미지를 'Base1(노란색)'으로 교체
			if (vecFinalDraw[i].bIsDone) 
				m_tCookingSlots[i].pBase->Set_TextureNum(1);
			else 
				m_tCookingSlots[i].pBase->Set_TextureNum(0);
		}
		else // 데이터가 없으면 슬롯을 숨깁니다.
		{
			m_tCookingSlots[i].pBase->Set_Active(false);
			m_tCookingSlots[i].pGauge->Set_Active(false);
			m_tCookingSlots[i].pSushi->Set_Active(false);
		}
	}
	return iExit;
}

void CUI_Bancho::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUI_Bancho::Render_GameObject()
{
}

CUI_Bancho* CUI_Bancho::Create(LPDIRECT3DDEVICE9 pGraphicDev, CBancho* pOwner)
{
	CUI_Bancho* pUI_Bancho = new CUI_Bancho(pGraphicDev);

	if (FAILED(pUI_Bancho->Ready_GameObject(pOwner)))
	{
		Safe_Release(pUI_Bancho);
		MSG_BOX("pUI_Bancho Create Failed");
		return nullptr;
	}

	return pUI_Bancho;
}

void CUI_Bancho::Free()
{
	for (auto& p : m_vecBanChoImgs)
		Safe_Release(p);

	m_vecBanChoImgs.clear();

	CUi::Free();
}
