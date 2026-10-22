#include "pch.h"
#include "CUI_Customer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CGameObjMgr.h"
#include "CRcFlipUITex.h"
#include "CUIFontMgr.h"
#include "CBancho_Player.h"
#include "CCustomer.h"

CUI_Customer::CUI_Customer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_fOffsetY(0.f), m_fOffsetX(0.f), m_bAnim(false), m_bGreenTeaAnim(false), m_pOwner(nullptr)
{
	m_wstrName = L"CUI_Customer";
}

CUI_Customer::~CUI_Customer()
{
}


HRESULT CUI_Customer::Ready_GameObject(CCustomer* pOwner)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Animation();

	m_UI_SequenceNum = UI_TWO;

	Set_Owner(pOwner);

	if (!m_pOwner)
		return E_FAIL;

	_vec2 vPos = { m_pOwner->Get_Position().x, m_pOwner->Get_Position().y };
	m_fOffsetY = m_pOwner->Get_Transform()->m_vScale.y * 1.75f;
	m_fOffsetX = m_pOwner->Get_Transform()->m_vScale.x * 0.25f;

	m_pOrderBase = CreateBanChoSlot(L"Proto_OrderSushi_Base", vPos, { 64.f, 64.f }, COM_FILPALPHA);
	m_pOrderBaseFull = CreateBanChoSlot(L"Proto_OrderSushi_Base_Full", vPos, { 64.f, 64.f }, COM_FILPALPHA);
	m_pOrderGreenTea = CreateBanChoSlot(L"Proto_OrderGreenTea", vPos, { 64.f, 64.f }, COM_FILPALPHA);
	m_pOrderGreenTeaFull = CreateBanChoSlot(L"Proto_OrderGreenTea_Full", vPos, { 64.f, 64.f }, COM_FILPALPHA);
	m_pOrderGreenTeaIcon = CreateBanChoSlot(L"Proto_GreenTeaIcon", vPos, { 9.f, 18.f }, COM_FILPALPHA);
	m_pOrderSushi = CreateBanChoSlot(L"Proto_UI_Sushi_Image", vPos, { 28.f, 18.f }, COM_NORMAL);

	return S_OK;
}

_int CUI_Customer::Update_GameObject(const _float& fTimeDelta)
{
	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (CGameObject::m_bPause)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	CCustomer::CUSTOMER_STATE eState = m_pOwner->Get_CurState();

	for (auto& pImg : m_vecBanChoImgs)
	{
		pImg->Get_TransformCom()->m_vInfo[INFO_POS] = { m_pOwner->Get_Position().x - m_fOffsetX, m_pOwner->Get_Position().y + m_fOffsetY, m_pOwner->Get_Position().z };
		pImg->Set_Active(false);
	}

	_float fActionTimer = m_pOwner->Get_ActionTimer();   // 현재 흐른 시간
	_float fMaxTime = m_pOwner->Get_TargetActionTime();  // 한계 시간 
	_float fGracePeriod = 10.f;                          // 기본 대기 시간 (여유 시간)

	_float fInpatientRatio = 0.f;

	// 흐른 시간이 10초보다 클 때만 게이지를 계산
	if (fActionTimer > fGracePeriod)
	{
		// (현재시간 - 10초) / (전체시간 - 10초)
		// 즉, 10초일 때 0% 시작 -> 전체시간일 때 100% 도달
		fInpatientRatio = (fActionTimer - fGracePeriod) / (fMaxTime - fGracePeriod);
	}
	else
	{
		// 10초 이전에는 게이지가 0에 고정
		fInpatientRatio = 0.f;
	}

	const _float fGaugeTop = 26.5f / 128.f;  // (가장 높이 올라갔을 때)
	const _float fGaugeBottom = 103.f / 128.f; // (가장 아래에 있을 때)
	const _float fGaugeRange = fGaugeBottom - fGaugeTop; // (실제 움직이는 구간)

	_float fCorrectedRatio = (1.f - fGaugeBottom) + (fInpatientRatio * fGaugeRange);

	if (eState == CCustomer::TEA)
	{
		m_pOrderGreenTea->Set_Active(true);
		m_pOrderGreenTea->Set_UISequence(UI_TWO);

		m_pOrderGreenTeaFull->Set_Active(true);
		m_pOrderGreenTeaFull->Set_UISequence(UI_THR);

		CRcFlipUITex* pBuffer = static_cast<CRcFlipUITex*>(m_pOrderGreenTeaFull->Get_Component(ID_STATIC, L"Com_Buffer"));

		if (pBuffer)
			pBuffer->Set_FillUpRatio(fCorrectedRatio, true);

		m_pOrderGreenTeaIcon->Set_Active(true);
		m_pOrderGreenTeaIcon->Set_Position({ m_pOwner->Get_Position().x - m_fOffsetX, m_pOwner->Get_Position().y + m_fOffsetY + 6.5f });
	}
	else if (eState == CCustomer::WAIT && m_pOwner->Get_OrderInfo().SushiTextureId != -1)
	{
		m_pOrderBase->Set_Active(true);
		m_pOrderBase->Set_UISequence(UI_TWO);

		m_pOrderBaseFull->Set_Active(true);
		m_pOrderBaseFull->Set_FillUI(true);
		m_pOrderBaseFull->Set_UISequence(UI_THR);

		// 플레이어가 동일한 음식을 들고 있다면 하이라이트 출력 
		if (m_pOwner->Is_SameOrder(CRestaurantMgr::GetInstance()->Get_Player()->Get_OrderFood().Sushi_Data.SushiTextureId))
			m_pOrderBase->Set_TextureNum(1);
		else
			m_pOrderBase->Set_TextureNum(0); 

		CRcFlipUITex* pBuffer = static_cast<CRcFlipUITex*>(m_pOrderBaseFull->Get_Component(ID_STATIC, L"Com_Buffer"));

		if (pBuffer)
			pBuffer->Set_FillUpRatio(fCorrectedRatio, true);

		m_pOrderSushi->Set_Active(true);
		m_pOrderSushi->Set_TextureNum(m_pOwner->Get_OrderInfo().SushiTextureId);
		m_pOrderSushi->Set_Position({ m_pOwner->Get_Position().x - m_fOffsetX, m_pOwner->Get_Position().y + m_fOffsetY + 5.f });
	}
	else
	{
		for (auto& pImg : m_vecBanChoImgs)
		{
			pImg->Set_Active(false);
		}
	}

	// 애니메이션 위치 조절
	if (eState != CCustomer::EXIT)
		m_pGreenTeaTransformCom->m_vInfo[INFO_POS] = { m_pOwner->Get_Position().x - 15.f, m_pOwner->Get_Position().y + 47.f,  m_pOwner->Get_Position().z + 12.f };
	else
		m_pGreenTeaTransformCom->m_vInfo[INFO_POS] = { m_pOwner->Get_Position().x, m_pOwner->Get_Position().y + 47.f,  m_pOwner->Get_Position().z + 12.f };


	// 골드 애니메이션 자연스럽게
	if (eState == CCustomer::HAPPY)
	{
		if (m_pAnimator->Is_Finish())
			m_bAnim = false;
	}


	for (auto& pImg : m_vecBanChoImgs)
		pImg->Update_GameObject(fTimeDelta);

	Enter_Animation(eState);

	return iExit;
}

void CUI_Customer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUI_Customer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if (FAILED(Ready_Material()))
		return;

	if (m_bAnim)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

		m_pAnimator->Render_Animation();
		m_pBufferCom->Render_Buffer();
	}
	if (m_bGreenTeaAnim)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pGreenTeaTransformCom->Get_World());

		m_pGreenTeaAnimator->Render_Animation();
		m_pBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CUI_Customer::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// 애니메이션
	pComponent = m_pTextureCom_Choose_Menu = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Choose_Menu"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Choose_Menu", pComponent });


	pComponent = m_pTextureCom_Gold = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gold"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Gold", pComponent });


	pComponent = m_pTextureCom_GreenTeaEffect = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GreenTeaEffect"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_GreenTeaEffect", pComponent });


	pComponent = m_pAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });
	

	pComponent = m_pGreenTeaAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_GreenTea_Animator", pComponent });



	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });



	pComponent = m_pGreenTeaTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_GreenTeaTransform", pComponent });


	return S_OK;
}

HRESULT CUI_Customer::Ready_Material()
{
	D3DMATERIAL9 tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}


void CUI_Customer::Ready_Animation()
{
	if (!m_pAnimator) return;

	CAnimation* pChoose_Menu = Engine::CAnimation::Create(L"Choose_Menu", m_pTextureCom_Choose_Menu, 0, 39, 0.05f, true);
	m_pAnimator->Add_Animation(L"Choose_Menu", pChoose_Menu);

	CAnimation* pGold = Engine::CAnimation::Create(L"Gold", m_pTextureCom_Gold, 0, 26, 0.01f, false);
	m_pAnimator->Add_Animation(L"Gold", pGold);



	CAnimation* pGreenTeaEffect = Engine::CAnimation::Create(L"GreenTeaEffect", m_pTextureCom_GreenTeaEffect, 0, 15, 0.1f, true);
	m_pGreenTeaAnimator->Add_Animation(L"GreenTeaEffect", pGreenTeaEffect);
}

void CUI_Customer::Enter_Animation(_int eNewAnim)
{
	m_eCurAnim = (CCustomer::CUSTOMER_STATE)eNewAnim;

	if (m_ePrevAnim != m_eCurAnim)
	{
		switch (m_eCurAnim)
		{
		case CCustomer::CUSTOMER_STATE::MENU:

			if (m_pOwner->Get_ServeGreenTea())
			{
				m_pGreenTeaTransformCom->m_vScale = { 45.f, 45.f, 1.f };
				m_pGreenTeaAnimator->Play_Animation(L"GreenTeaEffect", true);
				m_bGreenTeaAnim = true;
			}

			m_pTransformCom->m_vScale = { 16.f, 4.f, 1.f };
			m_pAnimator->Play_Animation(L"Choose_Menu", true);
			m_bAnim = true;
			break;

		case CCustomer::CUSTOMER_STATE::HAPPY:
			m_pTransformCom->m_vScale = { 32.f, 30.f, 1.f };
			m_pAnimator->Play_Animation(L"Gold", true);
			m_bAnim = true;
			break;

		default:
			m_pAnimator->Stop_Animation();
			m_bAnim = false;
			break;
		}

		m_ePrevAnim = m_eCurAnim;
	}
	m_pTransformCom->m_vInfo[INFO_POS] = { m_pOwner->Get_Position().x - 5.f, m_pOwner->Get_Position().y + 80.f,  m_pOwner->Get_Position().z };
}


CUI_Customer* CUI_Customer::Create(LPDIRECT3DDEVICE9 pGraphicDev, CCustomer* m_pOwner)
{
	CUI_Customer* pUI_Customer = new CUI_Customer(pGraphicDev);

	if (FAILED(pUI_Customer->Ready_GameObject(m_pOwner)))
	{
		Safe_Release(pUI_Customer);
		MSG_BOX("pUI_Customer Create Failed");
		return nullptr;
	}

	return pUI_Customer;
}

void CUI_Customer::Free()
{
	for (auto& p : m_vecBanChoImgs)
		Safe_Release(p);

	m_vecBanChoImgs.clear();

	CUi::Free();
}