#include "pch.h"
#include "CCustomer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CEventMgr.h"
#include "Event.h"
#include "CMonster.h"
#include "CSoundMgr.h"
#include "CRestaurantMgr.h"
#include "CHeartMgr.h"
#include "CAngerMgr.h"
#include "CSmileMgr.h"
#include "CRewardMgr.h"
#include "CFactoryMgr.h"

CCustomer::CCustomer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_pTargetChair(nullptr), m_fSpeed(120.f), m_fTargetPosX(0.f), m_fDistance(0.f), m_bExit(false), m_bGreenTea(false), m_bTeaServe(false), m_bTimerFlow(true), m_vHeartPos(0.f, 0.f, 0.f), m_pUI(nullptr)
{
}

CCustomer::CCustomer(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CCustomer::~CCustomer()
{
}

HRESULT CCustomer::Ready_GameObject()
{
	m_eCustomerType = (CUSTOMER_TYPE)(rand() % CUSTOMER_END);
	m_eClothType = (CLOTH_TYPE)(rand() % CLOTH_END);

	switch (m_eCustomerType)
	{
	case MAN00:
		m_strProto = L"Man00";
		break;
	
	case MAN01:
		m_strProto = L"Man01";
		break;
	
	case WOMAN00:
		m_strProto = L"Woman00";
		break;
	
	case WOMAN01:
		m_strProto = L"Woman01";
		break;
	
	case WOMAN02:
		m_strProto = L"Woman02";
		break;

	default:
		break;
	}

	switch (m_eClothType)
	{
	case CLOTH01:
		m_strCloth = L"Cloth01";
		break;

	default:
		break;
	}

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vInfo[INFO_POS] = { - WINCX * 0.6f, -180.f, 0.f };

	Ready_Animation();

	m_bGreenTea = (rand() % 100 < 30);

	m_pUI = CUI_Customer::Create(m_pGraphicDev, this);

	wchar_t szPath[256], szKey[256];

	// Eat Soundt
	for (int i = 1; i <= 5; ++i)
	{
		swprintf_s(szPath, L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Eat/sushi_customer_eat_0%d.wav", i);
		swprintf_s(szKey, L"Customer_Eat0%d", i);
		CSoundMgr::GetInstance()->LoadSound(szPath, szKey);
	}
	
	// Menu Sound
	for (int i = 1; i <= 6; ++i)
	{
		swprintf_s(szPath, L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Menu/sushi_customer_read_menu_0%d.wav", i);
		swprintf_s(szKey, L"Customer_Menu0%d", i);
		CSoundMgr::GetInstance()->LoadSound(szPath, szKey);
	}

	// Talk
	for (int i = 1; i <= 7; ++i)
	{
		swprintf_s(szPath, L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Talk/sushi_customer_enter_talk_0%d.wav", i);
		swprintf_s(szKey, L"Customer_Talk0%d", i);
		CSoundMgr::GetInstance()->LoadSound(szPath, szKey);
	}

	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Serve/sushi_customer_served.wav", L"Customer_Served");

	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/EffectSound/sound_sushibar_give_tips_02.wav", L"Customer_Tip");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/EffectSound/sound_sushibar_pay_02.wav", L"Customer_Pay");
	return S_OK;
}

_int CCustomer::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_pUI)
		m_pUI->Update_GameObject(fTimeDelta);

	if (CGameObject::m_bPause)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Set_CurState(m_eCurState);

	if (m_pTargetChair && m_eCurState == WALK)
	{
		m_pTransformCom->m_vInfo[INFO_POS].x += m_fSpeed * fTimeDelta;
	}
	else if (m_eCurState == TEA)
	{
		if (m_bTimerFlow)
			m_fActionTimer += fTimeDelta;

		if (m_bTeaServe)
		{
			m_eCurState = MENU;
			CSoundMgr::GetInstance()->PlaySFX(L"Customer_Tip", SOUND_CUSTOMER_TIP, 1.f);
			m_bGreenTea = false;
		}
		else if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = ANGER;
			m_bGreenTea = false;
		}
	}
	else if (m_eCurState == MENU)
	{
		m_fActionTimer += fTimeDelta;

		if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = WAIT;
		}
	}
	else if (m_eCurState == WAIT)
	{
		m_fActionTimer += fTimeDelta;

		if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = ANGER;
		}
	}
	else if (m_eCurState == EAT)
	{
		m_fActionTimer += fTimeDelta;
		// 진행률이 50 % 일 때 추가 하트
		if (m_fActionTimer >= m_fTargetActionTime * 0.5f && !m_bHeartSecondTrigger)
		{
			CHeartMgr::GetInstance()->Generate_Heart(m_vHeartPos, 5, 0.07f, 1.5f);
			m_bHeartSecondTrigger = true;
		}
		// 음식을 거의 다 먹었을 때 웃는 표정 미리 생성
		/*if (m_fActionTimer >= m_fTargetActionTime * 0.9f && !m_bSmileTrigger)
		{
			m_bSmileTrigger = true;
		}*/
		if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = HAPPY;
		}
	}
	else if (m_eCurState == HAPPY)
	{
		m_fActionTimer += fTimeDelta;

		if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = EXIT;
		}
	}
	else if (m_eCurState == ANGER)
	{
		m_fActionTimer += fTimeDelta;

		if (m_fActionTimer >= m_fTargetActionTime)
		{
			m_eCurState = EXIT;
		}
	}
	else if (m_eCurState == EXIT)
	{
		if (m_pTargetChair)
			m_pTargetChair = nullptr;

		m_pTransformCom->m_vInfo[INFO_POS].x -= m_fSpeed * fTimeDelta;

		if (m_pTransformCom->m_vInfo[INFO_POS].x <= -WINCX * 0.6f)
			m_bDead = true;
	}

	return iExit;
}

void CCustomer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (m_pUI)
		m_pUI->LateUpdate_GameObject(fTimeDelta);
}

void CCustomer::Render_GameObject()
{
	if (m_pUI)
		m_pUI->Render_GameObject();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (FAILED(Ready_Material()))
		return;

	m_pBodyAnimator->Render_Animation();
	m_pBufferCom->Render_Buffer();

	m_pClothAnimator->Render_Animation();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CCustomer::OnCollision_Enter(CGameObject* pObj)
{	
}

void CCustomer::OnCollision_Exit(CGameObject* pObj)
{

}

void CCustomer::OnCollision_Stay(CGameObject* pObj)
{
	if (m_eCurState == WALK)
	{
		if (pObj->Get_Layer() & LAYER_CHAIR)
		{
			m_fDistance = abs(m_pTransformCom->m_vInfo[INFO_POS].x - m_pTargetChair->Get_Position().x);

			if (m_fDistance <= 20.f)
			{
				m_pTransformCom->m_vInfo[INFO_POS].x = m_pTargetChair->Get_Position().x + 18.f;
				m_pTransformCom->m_vInfo[INFO_POS].z = 0.3f;

				if (m_bGreenTea)
					m_eCurState = TEA;
				else
					m_eCurState = MENU;
			}
		}
	}
}

_bool CCustomer::Is_MyOrder(int iSushi)
{
	if (m_eCurState == WAIT && m_tOrderInfo.SushiTextureId == iSushi)
	{
		m_eCurState = EAT;
		return true;
	}

	return false;
}

_bool CCustomer::Is_SameOrder(int iSushi)
{
	if (m_eCurState == WAIT && m_tOrderInfo.SushiTextureId == iSushi)
	{
		return true;
	}

	return false;
}

HRESULT CCustomer::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


#pragma region 텍스쳐

	wstring strBase = L"Proto_" + m_strProto;

	//Customer_Walk
	pComponent = m_pTextureCom_Customer_Walk = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Walk").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Walk";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent});

	// Customer_Menu	
	pComponent = m_pTextureCom_Customer_Menu = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Back_Menu").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Back_Menu";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });

	// Customer_Wait
	pComponent = m_pTextureCom_Customer_Wait = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Back_Wait").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Back_Wait";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });
	

	// Customer_Eat
	pComponent = m_pTextureCom_Customer_Eat = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Back_Eat").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Back_Eat";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent});


	// Customer_Happy
	pComponent = m_pTextureCom_Customer_Happy = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Back_Happy").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Back_Happy";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });

	// Customer_Anger
	pComponent = m_pTextureCom_Customer_Anger = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strBase + L"_Back_Anger").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_Back_Anger";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent});


	// ------------------------- 옷 ------------------------------

	wstring strClothBase = strBase + L"_" + m_strCloth;

	//Cloth_Walk
	pComponent = m_pTextureCom_Cloth_Walk = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Walk").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Walk";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent});

	//Cloth_Menu	
	pComponent = m_pTextureCom_Cloth_Menu = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Back_Menu").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Back_Menu";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });

	//Cloth_Wait
	pComponent = m_pTextureCom_Cloth_Wait = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Back_Wait").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Back_Wait";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });


	//Cloth_Eat
	pComponent = m_pTextureCom_Cloth_Eat = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Back_Eat").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Back_Eat";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });


	//Cloth_Happy
	pComponent = m_pTextureCom_Cloth_Happy = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Back_Happy").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Back_Happy";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });

	//Cloth_Anger
	pComponent = m_pTextureCom_Cloth_Anger = dynamic_cast<Engine::CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype((strClothBase + L"_Back_Anger").c_str()));

	if (nullptr == pComponent)
		return E_FAIL;

	m_strWalkKey = L"Com_Texture_" + m_strProto + L"_" + m_strCloth + L"_Back_Anger";
	m_vecTag.push_back(m_strWalkKey);

	m_mapComponent[ID_STATIC].insert({ m_vecTag.back().c_str(), pComponent });

#pragma endregion

	//--------------------------------------------------------------------------


	// 몸 애니메이터
	pComponent = m_pBodyAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Body_Animator", pComponent });
	

	// 옷 애니메이터
	pComponent = m_pClothAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Cloth_Animator", pComponent });


	m_pColliderCom = CCollider::Create(m_pGraphicDev, COLLIDER_AABB, _vec3(2.f, 2.f, 1.f), _vec3(0.f, 0.f, 0.f));

	if (m_pColliderCom == nullptr)
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", m_pColliderCom });

	m_iLayer = LAYER_CUSTOMER;
	m_iCollisionMask = LAYER_PLAYER | LAYER_CHAIR;
	m_eObjType = OBJ_CUSTOMER;

	return S_OK;
}

CCustomer* CCustomer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCustomer* pBancho = new CCustomer(pGraphicDev);

	if (FAILED(pBancho->Ready_GameObject()))
	{
		Safe_Release(pBancho);
		MSG_BOX("pBancho Create Failed");
		return nullptr;
	}

	return pBancho;
}

void CCustomer::Free()
{
	Safe_Release(m_pUI);

	CEventMgr::GetInstance()->Unsubscribe_All(this);

	CGameObject::Free();
}

HRESULT CCustomer::Ready_Material()
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

void CCustomer::Ready_Animation()
{
	if (!m_pBodyAnimator) return;
	if (!m_pClothAnimator) return;

	// 몸
	CAnimation* pCustomer_Walk = Engine::CAnimation::Create(L"Customer_Walk", m_pTextureCom_Customer_Walk, 0, 7, 0.1f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Walk", pCustomer_Walk);

	CAnimation* pCustomer_Menu = Engine::CAnimation::Create(L"Customer_Menu", m_pTextureCom_Customer_Menu, 0, 0, 3.f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Menu", pCustomer_Menu);

	CAnimation* pCustomer_Wait = Engine::CAnimation::Create(L"Customer_Wait", m_pTextureCom_Customer_Wait, 0, 2, 0.2f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Wait", pCustomer_Wait);

	CAnimation* pCustomer_Eat = Engine::CAnimation::Create(L"Customer_Eat", m_pTextureCom_Customer_Eat, 0, 2, 0.2f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Eat", pCustomer_Eat);

	CAnimation* pCustomer_Happy = Engine::CAnimation::Create(L"Customer_Happy", m_pTextureCom_Customer_Happy, 0, 1, 0.2f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Happy", pCustomer_Happy);

	CAnimation* pCustomer_Anger = Engine::CAnimation::Create(L"Customer_Anger", m_pTextureCom_Customer_Anger, 0, 1, 0.2f, true);
	m_pBodyAnimator->Add_Animation(L"Customer_Anger", pCustomer_Anger);

	
	// 옷
	CAnimation* pCloth_Walk = Engine::CAnimation::Create(L"Cloth_Walk", m_pTextureCom_Cloth_Walk, 0, 7, 0.1f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Walk", pCloth_Walk);

	CAnimation* pCloth_Menu = Engine::CAnimation::Create(L"Cloth_Menu", m_pTextureCom_Cloth_Menu, 0, 0, 3.f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Menu", pCloth_Menu);

	CAnimation* pCloth_Wait = Engine::CAnimation::Create(L"Cloth_Wait", m_pTextureCom_Cloth_Wait, 0, 2, 0.2f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Wait", pCloth_Wait);

	CAnimation* pCloth_Eat = Engine::CAnimation::Create(L"Cloth_Eat", m_pTextureCom_Cloth_Eat, 0, 2, 0.2f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Eat", pCloth_Eat);

	CAnimation* pCloth_Happy = Engine::CAnimation::Create(L"Cloth_Happy", m_pTextureCom_Cloth_Happy, 0, 1, 0.2f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Happy", pCloth_Happy);

	CAnimation* pCloth_Anger = Engine::CAnimation::Create(L"Cloth_Anger", m_pTextureCom_Cloth_Anger, 0, 1, 0.2f, true);
	m_pClothAnimator->Add_Animation(L"Cloth_Anger", pCloth_Anger);

	CUSTOMER_STATE eCur = (CUSTOMER_STATE)(rand() % CUSSTATE_END);

	Enter_Animation(WALK);
}

void CCustomer::Play_RandomSound(const wstring& strKey, _int iCount, CHANNELID eID)
{
	int iRandom = rand() % iCount + 1;
	wchar_t szFinalKey[256];
	wsprintf(szFinalKey, L"%s0%d", strKey.c_str(), iRandom);

	CSoundMgr::GetInstance()->PlaySFX(szFinalKey, eID, 1.f);
}

void CCustomer::Enter_Animation(CUSTOMER_STATE eNewAnim)
{
	m_eCurAnim = eNewAnim;

	if (m_ePrevAnim != m_eCurAnim)
	{
		switch (m_eCurAnim)
		{
		case CUSTOMER_STATE::WALK:
			m_pBodyAnimator->Play_Animation(L"Customer_Walk", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Walk", true);
			break;

		case CUSTOMER_STATE::MENU:
			m_pBodyAnimator->Play_Animation(L"Customer_Menu", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Menu", true);
			break;

		case CUSTOMER_STATE::WAIT:
			m_pBodyAnimator->Play_Animation(L"Customer_Wait", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Wait", true);
			break;

		case CUSTOMER_STATE::EAT:
			m_pBodyAnimator->Play_Animation(L"Customer_Eat", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Eat", true);
			break;
		
		case CUSTOMER_STATE::HAPPY:
			m_pBodyAnimator->Play_Animation(L"Customer_Happy", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Happy", true);
			break;
		
		case CUSTOMER_STATE::ANGER:
			m_pBodyAnimator->Play_Animation(L"Customer_Anger", true);
			m_pClothAnimator->Play_Animation(L"Cloth_Anger", true);
			break;

		default:
			break;
		}
		m_pTransformCom->m_vScale = { 92.f * 0.5f, 135.f * 0.5f, 1.f };

		m_ePrevAnim = m_eCurAnim;
	}
}

void CCustomer::Set_CurState(CUSTOMER_STATE eState)
{
	m_eCurState = eState;

	if (m_ePreState != m_eCurState)
	{
		m_fActionTimer = 0.f;

		switch (m_eCurState)
		{
		case CUSTOMER_STATE::WALK:
			Play_RandomSound(L"Customer_Talk", 7, SOUND_CUSTOMER_TALK);
			Enter_Animation(WALK);
			break;

		case CUSTOMER_STATE::TEA:
			Enter_Animation(WAIT);
			m_fTargetActionTime = 30.f;
			break;

		case CUSTOMER_STATE::MENU:
			Enter_Animation(MENU);
			m_fTargetActionTime = (_float)(rand() % 5 + 2);
			break;

		case CUSTOMER_STATE::WAIT:
			Play_RandomSound(L"Customer_Menu", 6, SOUND_CUSTOMER_MENU);
			if (CRestaurantMgr::GetInstance()->Place_Order(this))
			{
				CUSTOMER_SUSHI iCustomerSushi_ID = {};
				iCustomerSushi_ID.iCustomer_SushiID = m_tOrderInfo.SushiTextureId;
				CEventMgr::GetInstance()->Trigger(Event(EVENT::UI::SUSHI_OPEN_MINUS_SUSHI, &iCustomerSushi_ID));
				Enter_Animation(WAIT);
				m_fTargetActionTime = 40.f;
			}
			else
			{
				m_eCurState = EXIT;
				return;
			}
			break;

		case CUSTOMER_STATE::EAT:
			m_vHeartPos = m_pTransformCom->m_vInfo[INFO_POS];
			m_vHeartPos.x -= m_pTransformCom->m_vScale.x * 0.25f;
			m_vHeartPos.y += m_pTransformCom->m_vScale.y * 1.3f;

			CSmileMgr::GetInstance()->Generate_Smile(m_vHeartPos, 1, 0.f, 0.f);
			CSoundMgr::GetInstance()->PlaySFX(L"Customer_Served", SOUND_CUSTOMER_SMILE, 1.f);
			Enter_Animation(EAT);
			Play_RandomSound(L"Customer_Eat", 5, SOUND_CUSTOMER_EAT);
			{
				_int iHeartCnt = 1 + rand() % 9;
				_int iMultiply = 1 + rand() % 4;

				CHeartMgr::GetInstance()->Generate_Heart(m_vHeartPos, iHeartCnt, 0.07f, 1.f);
				CHeartMgr::GetInstance()->Generate_Heart(m_vHeartPos, iHeartCnt * iMultiply, 0.07f, 1.8f);
				m_fTargetActionTime = (_float)(rand() % 6 + 5);
				break;
			}

		case CUSTOMER_STATE::HAPPY:
			CRestaurantMgr::GetInstance()->Register_Sales(m_tOrderInfo.SushiTextureId, m_tOrderInfo.Price);
			CBoat_SushiMgr::GetInstance()->Add_Money(m_tOrderInfo.Price);
			CRewardMgr::GetInstance()->Generate_Reward(m_vHeartPos, m_tOrderInfo.Price, 1, 0.f, 0.f);
			CSoundMgr::GetInstance()->PlaySFX(L"Customer_Pay", SOUND_CUSTOMER_PAY, 1.f);
			Enter_Animation(HAPPY);
			m_fTargetActionTime = 3.f;
			break;

		case CUSTOMER_STATE::ANGER:
			Enter_Animation(ANGER);
			m_vAngerPos = m_pTransformCom->m_vInfo[INFO_POS];
			m_vAngerPos.x -= m_pTransformCom->m_vScale.x * 0.25f;
			m_vAngerPos.y += m_pTransformCom->m_vScale.y * 1.3f;
			CAngerMgr::GetInstance()->Generate_Anger(m_vAngerPos, 1, 0.f, 0.f);
			m_fTargetActionTime = 3.f;
			break;

		case CUSTOMER_STATE::EXIT:
			Enter_Animation(WALK);
			m_pTransformCom->m_vScale.x *= -1.f;
			m_pTransformCom->m_vInfo[INFO_POS].z = 0.f;
			if (m_pTargetChair) m_pTargetChair->Clear_Chair();
			m_bExit = true;
			break;

		default:
			break;
		}

		m_ePreState = m_eCurState;
	}
}
