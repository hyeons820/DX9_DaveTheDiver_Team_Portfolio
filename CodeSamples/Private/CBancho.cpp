#include "pch.h"
#include "CBancho.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CEventMgr.h"
#include "Event.h"
#include "CMonster.h"
#include "CSoundMgr.h"

CBancho::CBancho(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_CookCount(0), m_fCookTime(0.f), m_fTargetTime(7.f)
{
	m_tOrderDish = {};
}

CBancho::CBancho(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CBancho::~CBancho()
{
}

HRESULT CBancho::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vInfo[INFO_POS] = { 520.f, -153.00, 0.7f };

	Ready_Animation();

	BanCho_Subscribe();

	m_eCurState = IDLE;
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Bancho/effect_chopping_board_02.wav", L"Bancho_Chopping");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Bancho/sushi_bancho_foodready_02.wav", L"Bancho_Foodready0");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Bancho/sushi_bancho_foodready_04.wav", L"Bancho_Foodready1");

	return S_OK;
}

_int CBancho::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (CGameObject::m_bPause)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_eCurState == IDLE)
	{
		if (CRestaurantMgr::GetInstance()->Start_Cook(&m_tOrderDish))
		{
			m_eCurState = COOK;
		}
	}
	else if (m_eCurState == COOK)
	{
		m_fCookTime += fTimeDelta;

		if (m_fCookTime >= m_fTargetTime)
		{
			bool sound = (rand() % 100 >= 50);
			if (sound)
				CSoundMgr::GetInstance()->PlaySFX(L"Bancho_Foodready0", SOUND_BANCHO, 1.f);
			else
				CSoundMgr::GetInstance()->PlaySFX(L"Bancho_Foodready1", SOUND_BANCHO, 1.f);

			m_vecFinishedDish.push_back(m_tOrderDish);
			m_eCurState = IDLE;
		}
	}

	Set_CurState(m_eCurState);

	if (m_eCurAnim == COOK && m_pAnimator->Get_Frame() == 1)
	{
		m_CookCount++;

		if (m_CookCount >= 15)
		{
			Enter_Animation(COOK_WAIT);
			m_CookCount = 0;
		}
	}
	else if (m_eCurAnim == COOK_WAIT && m_pAnimator->Is_Finish())
	{
		Enter_Animation(COOK);
	}

	return iExit;
}

void CBancho::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBancho::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (FAILED(Ready_Material()))
		return;

	m_pAnimator->Render_Animation();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CBancho::OnCollision_Enter(CGameObject* pObj)
{

}

void CBancho::OnCollision_Exit(CGameObject* pObj)
{

}

void CBancho::OnCollision_Stay(CGameObject* pObj)
{

}

CRestaurantMgr::CUSTOMER_ORDER CBancho::Get_FinishedDish()
{
	CRestaurantMgr::CUSTOMER_ORDER tDish = m_vecFinishedDish.front();

	m_vecFinishedDish.erase(m_vecFinishedDish.begin());

	return tDish;
}

HRESULT CBancho::Add_Component()
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


	 //BanCho_Idle
	pComponent = m_pTextureCom_BanCho_Idle = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanCho_Idle"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Idle", pComponent });


	// BanCho_Cook
	pComponent = m_pTextureCom_BanCho_Cook = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanCho_Cook"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanChoCook", pComponent });
	
	// BanCho_CookWait
	pComponent = m_pTextureCom_BanCho_CookWait = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanCho_CookWait"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanChoCookWait", pComponent });


	//--------------------------------------------------------------------------


	pComponent = m_pAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	m_pColliderCom = CCollider::Create(m_pGraphicDev, COLLIDER_AABB, _vec3(2.f, 2.f, 1.f), _vec3(0.f, 0.f, 0.f));

	if (m_pColliderCom == nullptr)
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", m_pColliderCom });

	m_iLayer = LAYER_BANCHO;
	m_iCollisionMask = LAYER_PLAYER;
	m_eObjType = OBJ_BANCHO;

	return S_OK;
}


CBancho* CBancho::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBancho* pBancho = new CBancho(pGraphicDev);

	if (FAILED(pBancho->Ready_GameObject()))
	{
		Safe_Release(pBancho);
		MSG_BOX("pBancho Create Failed");
		return nullptr;
	}

	return pBancho;
}

void CBancho::Free()
{
	CEventMgr::GetInstance()->Unsubscribe_All(this);

	CGameObject::Free();
}


HRESULT CBancho::Ready_Material()
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

void CBancho::Ready_Animation()
{
	if (!m_pAnimator) return;

	// 기본 상태
	CAnimation* pBanCho_Idle = Engine::CAnimation::Create(L"BanCho_Idle", m_pTextureCom_BanCho_Idle, 0, 4, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Idle", pBanCho_Idle);

	CAnimation* pBanCho_Cook = Engine::CAnimation::Create(L"BanCho_Cook", m_pTextureCom_BanCho_Cook, 0, 1, 0.05f, true);
	m_pAnimator->Add_Animation(L"BanCho_Cook", pBanCho_Cook);
	
	CAnimation* pBanCho_CookWait = Engine::CAnimation::Create(L"BanCho_CookWait", m_pTextureCom_BanCho_CookWait, 0, 0, 0.5f, false);
	m_pAnimator->Add_Animation(L"BanCho_CookWait", pBanCho_CookWait);

	Enter_Animation(IDLE);
}


void CBancho::BanCho_Subscribe()
{
	//CEventMgr::GetInstance()->Subscribe(EVENT::PLAYER::PLAYER_EXITFAIL, this, [this](const Event& e)
	//	{
	//		
	//
	//	}
	//);
}

void CBancho::Set_CurState(BANCHO_STATE eState)
{
	m_eCurState = eState;

	if (m_ePreState != m_eCurState)
	{
		m_fCookTime = 0.f;

		switch (m_eCurState)
		{
		case BANCHO_STATE::IDLE:
			Enter_Animation(IDLE);
			break;

		case BANCHO_STATE::COOK:
			Enter_Animation(COOK);
			break;

		default:
			break;
		}

		m_ePreState = m_eCurState;
	}
}


void CBancho::Enter_Animation(BANCHO_STATE eNewAnim)
{
	m_eCurAnim = eNewAnim;

	if (m_ePrevAnim != m_eCurAnim)
	{
		switch (m_eCurAnim)
		{
		case BANCHO_STATE::IDLE:
			m_pAnimator->Play_Animation(L"BanCho_Idle", true);
			m_pTransformCom->m_vScale = { 84.f * 0.5f, 180.f * 0.5f, 1.f };
			break;

		case BANCHO_STATE::COOK:
			CSoundMgr::GetInstance()->PlaySFX(L"Bancho_Chopping", SOUND_BANCHO_COOK, 1.f);
			m_pAnimator->Play_Animation(L"BanCho_Cook", true);
			m_pTransformCom->m_vScale = { 94.f * 0.5f, 180.f * 0.5f, 1.f };
			break;
		
		case BANCHO_STATE::COOK_WAIT:
			m_pAnimator->Play_Animation(L"BanCho_CookWait", true);
			m_pTransformCom->m_vScale = { 94.f * 0.5f, 180.f * 0.5f, 1.f };
			break;

		default:
			break;
		}

		m_ePrevAnim = m_eCurAnim;
	}
}
