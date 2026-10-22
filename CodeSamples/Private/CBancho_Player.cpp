#include "pch.h"
#include "CBancho_Player.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CEventMgr.h"
#include "Event.h"
#include "CMonster.h"
#include "CWasabi.h"
#include "CSoundMgr.h"

CBancho_Player::CBancho_Player(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBancho_Player::CBancho_Player(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CBancho_Player::~CBancho_Player()
{
}

HRESULT CBancho_Player::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vInfo[INFO_POS] = { 0.f, -153.00, 0.6f };
	m_pTransformCom->m_vScale = { 50.f, 75.f, 1.f };

	m_fSpeed = 5.f;

	Ready_Animation();

	BanCho_Player_Subscribe();
	BanCho_PlayerSound();
	m_fCurRun_Gauage = m_fMaxRun_Gauage;

	//CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Diver_Player/dave_hit_01.wav", L"Diver_Player_Hit");
	//CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Diver_Player/harpoon_shot.wav", L"Harpoon_Shot");
	//CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Diver_Player/harpoon_tap_button.wav", L"Harpoon_Tap");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Serve/sushi_food_receive_02.wav", L"BanchoPlayer_Serve");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Serve/sushi_dump_02.wav", L"BanchoPlayer_Dump");

	return S_OK;
}

_int CBancho_Player::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (CGameObject::m_bPause)
		return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CBancho_Player::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bTea)
	{
		Key_Input(fTimeDelta);
	}

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBancho_Player::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (FAILED(Ready_Material()))
		return;

	m_pAnimator->Render_Animation();
	m_pBufferCom->Render_Buffer(m_bFlipX);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CBancho_Player::OnCollision_Enter(CGameObject* pObj)
{
	
	
}

void CBancho_Player::OnCollision_Exit(CGameObject* pObj)
{
	
}

void CBancho_Player::OnCollision_Stay(CGameObject* pObj)
{
	if (pObj->Get_Layer() & LAYER_BANCHO)
	{
		CBancho* pBancho = static_cast<CBancho*>(pObj);

		if (!pBancho->Is_EmptyFinishedDish() && !m_bServe)
		{
			if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
			{
				m_bServe = true;
 				m_tOrderFood = pBancho->Get_FinishedDish();
			}
		}
	}
	if (pObj->Get_Layer() & LAYER_CUSTOMER)
	{
		CCustomer* pCustomer = static_cast<CCustomer*>(pObj);

		if (pCustomer->Get_WantGreenTea())
		{
			if (m_bServe || m_bTea)
				return;

			if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
			{
				pCustomer->Set_TimerFlow(false);
				CRestaurantMgr::GetInstance()->Set_GreenTeaUIScreen(pCustomer);
				Enter_Animation(TEA);
				m_bTea = true;
				return;
			}
		}

		if (!m_bServe)
			return;

		if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
		{
			if (pCustomer->Is_MyOrder(m_tOrderFood.Sushi_Data.SushiTextureId))
			{
				CSoundMgr::GetInstance()->PlaySFX_Once(L"BanchoPlayer_Serve", SOUND_BANCHO_PLAYER_SERVE, 1.f);
				Reset_Food();
			}
		}
	}
	if (pObj->Get_Layer() & LAYER_WASABI)
	{
		if (m_tOrderFood.Sushi_Data.SushiTextureId != -1)
			return;

		if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
		{
			CWasabi* pWasabi = static_cast<CWasabi*>(pObj);

			if (pWasabi->Get_WasabiAmount() < 100)
			{
				CEventMgr::GetInstance()->Trigger(Event(EVENT::UI::WASABI_UI_OPEN));
			}
		}
	}
}

HRESULT CBancho_Player::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcPlayerTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcPlayerTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//플레이어 이동-----------------------------------------------------------------------------------------

	 //BanCho_Player_Idle
	pComponent = m_pTextureCom_BanCho_PlayerIdle = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Idle"));
	
	if (nullptr == pComponent)
		return E_FAIL;
	
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Idle", pComponent });
	
	
	// BanCho_Player_Walk
	pComponent = m_pTextureCom_BanCho_PlayerWalk = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Walk"));
	
	if (nullptr == pComponent)
		return E_FAIL;
	
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Walk", pComponent });
	

	// BanChoPlayer_Run
	pComponent = m_pTextureCom_BanCho_PlayerRun = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Run"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Run", pComponent });


	// BanChoPlayer_Tired
	pComponent = m_pTextureCom_BanCho_PlayerTired = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Tired"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Tired", pComponent });


	// BanChoPlayer_Serve_Idle
	pComponent = m_pTextureCom_BanCho_PlayerServeIdle = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Serve_Idle"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Serve_Idle", pComponent });


	// BanChoPlayer_Serve_Walk
	pComponent = m_pTextureCom_BanCho_PlayerServeWalk = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Serve_Walk"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Serve_Walk", pComponent });


	// BanChoPlayer_Serve_run
	pComponent = m_pTextureCom_BanCho_PlayerServeRun = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Serve_run"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Serve_run", pComponent });


	// BanChoPlayer_Serve_Tired
	pComponent = m_pTextureCom_BanCho_PlayerServeTired = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Serve_Tired"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BanCho_Player_Serve_Tired", pComponent });


	// BanChoPlayer_Tea
	pComponent = m_pTextureCom_BanCho_PlayerTea = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Tea"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Proto_BanChoPlayer_Tea", pComponent });


	// BanChoPlayer_Wasabi
	pComponent = m_pTextureCom_BanCho_PlayerWasabi = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Wasabi"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Proto_BanChoPlayer_Wasabi", pComponent });

	// BanChoPlayer_Eat
	pComponent = m_pTextureCom_BanCho_PlayerEat = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BanChoPlayer_Eat"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Proto_BanChoPlayer_Eat", pComponent });


	//--------------------------------------------------------------------------

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pAnimator = dynamic_cast<Engine::CAnimator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pCollider = CCollider::Create(m_pGraphicDev, COLLIDER_AABB, _vec3(2.f, 2.f, 1.f), _vec3(0.f, 0.f, 0.f));

	if (pCollider == nullptr)
		return E_FAIL;

	pCollider->Set_Owner(this);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pCollider });

	m_iLayer = LAYER_PLAYER;
	m_iCollisionMask = LAYER_BANCHO | LAYER_CUSTOMER | LAYER_WASABI;
	m_eObjType = OBJ_PLAYER;

	return S_OK;
}

void CBancho_Player::Key_Input(const _float& fTimeDelta)
{

	if (!m_pAnimator)
		return;

	if (CBoat_SushiMgr::GetInstance()->Get_TodayMenuOpen())
		return;

	Key_Input_Status_Update(fTimeDelta);

	//if (Key_Input_Interaction(fTimeDelta))
	//	return;

	if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_Q))
	{
		CSoundMgr::GetInstance()->PlaySFX_Once(L"BanchoPlayer_Dump", SOUND_BANCHO_PLAYER_SERVE, 1.f);
		Chuck_Food();
		Enter_Animation(EAT);
		return;
	}



	Key_Input_Movement(fTimeDelta);

}



void CBancho_Player::Key_Input_Status_Update(const _float& fTimeDelta)
{

	if (m_eCurAnim == EAT)
	{
		if (m_pAnimator->Is_Finish())
		{
			Enter_Animation(IDLE);
		}
		return;
	}
	
}

_bool CBancho_Player::Key_Input_Interaction(const _float& fTimeDelta)
{
	/*if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_T))
	{
		Enter_Animation(TEA);
		return true;
	}

	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_Y))
	{
		Enter_Animation(WASABI);
		return true;
	}

	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_U))
	{
		m_bServe = true;
	}*/



	return false;
}

void CBancho_Player::Key_Input_Movement(const _float& fTimeDelta)
{
	_vec3 vUp, vRight;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	BANCHOPLAYER_ANIM anim = IDLE;
	bool bShift = false;

	const float fTired_lock = 10.f;
	const float fTired_Release = 40.f;

	m_fWalkTime += fTimeDelta;

	if (m_fCurRun_Gauage <= fTired_lock)
		m_bTired = true;
	else if (m_fCurRun_Gauage >= fTired_Release)
		m_bTired = false;

	bool bCanRun = !m_bTired;

	// 속도 및 게이지
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_LSHIFT) && bCanRun)
	{
		m_fSpeed = 200.f;
		bShift = true;

		m_fCurRun_Gauage -= 0.5f;
		if (m_fCurRun_Gauage < 0.f)
			m_fCurRun_Gauage = 0.f;
	}
	else
	{
		if (m_bTired)
			m_fSpeed = 80.f;
		else
			m_fSpeed = 150.f;

		m_fCurRun_Gauage += 0.1f;
		if (m_fCurRun_Gauage > 100.f)
			m_fCurRun_Gauage = 100.f;
	}

	_vec3 vDir = { 0.f, 0.f, 0.f };
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A)) vDir -= vRight;
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D)) vDir += vRight;

	
	if (D3DXVec3Length(&vDir) > 0.1f)
	{
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed);

		// 나가기 방지
		if (m_pTransformCom->m_vInfo[INFO_POS].x < -375.f) 	m_pTransformCom->m_vInfo[INFO_POS].x = -375.f;
		else if (m_pTransformCom->m_vInfo[INFO_POS].x > 650.f) 	m_pTransformCom->m_vInfo[INFO_POS].x = 650.f;
		
		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A)) m_bFlipX = false;
		else if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D)) m_bFlipX = true;

		if (m_bTired)
			anim = (m_bServe ? SERVE_TIRED : TIRED);
		else
		{
			if (m_bServe)
				anim = (bShift ? SERVE_RUN : SERVE_WALK);
			else
				anim = (bShift ? RUN : WALK);
		}
	}
	else
	{
		anim = (m_bServe ? SERVE_IDLE : IDLE);
	}

	if ((anim == SERVE_RUN || anim == RUN) && m_fWalkTime > 0.3f)
	{
		CSoundMgr::GetInstance()->PlaySFX_Once(L"daveDash", SOUND_PLAYER);
		m_fWalkTime = 0.f;
	}

	else if ((anim == SERVE_WALK || anim == WALK)&& m_fWalkTime>0.5f)
	{
		CSoundMgr::GetInstance()->PlaySFX(L"Bancho_Player_Move", SOUND_PLAYER);
		m_fWalkTime = 0.f;
	}


	Enter_Animation(anim);
}



CBancho_Player* CBancho_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBancho_Player* pBancho_Player = new CBancho_Player(pGraphicDev);

	if (FAILED(pBancho_Player->Ready_GameObject()))
	{
		Safe_Release(pBancho_Player);
		MSG_BOX("pBancho_Player Create Failed");
		return nullptr;
	}

	return pBancho_Player;
}

void CBancho_Player::Free()
{
	CEventMgr::GetInstance()->Unsubscribe_All(this);

	CGameObject::Free();
}


HRESULT CBancho_Player::Ready_Material()
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

void CBancho_Player::Ready_Animation()
{
	if (!m_pAnimator) return;

	// 기본 상태
	CAnimation* pBanCho_PlayerIdle = Engine::CAnimation::Create(L"BanCho_Player_Idle", m_pTextureCom_BanCho_PlayerIdle, 0, 3, 0.35f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Idle", pBanCho_PlayerIdle);

	CAnimation* pBanCho_PlayerWalk = Engine::CAnimation::Create(L"BanCho_Player_Walk", m_pTextureCom_BanCho_PlayerWalk, 0, 7, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Walk", pBanCho_PlayerWalk);

	CAnimation* pBanCho_PlayerRun = Engine::CAnimation::Create(L"BanCho_Player_Run", m_pTextureCom_BanCho_PlayerRun, 0, 7, 0.10f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Run", pBanCho_PlayerRun);

	CAnimation* pBanCho_PlayerTired = Engine::CAnimation::Create(L"BanCho_Player_Tired", m_pTextureCom_BanCho_PlayerTired, 0, 7, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Tired", pBanCho_PlayerTired);

	// 서빙 상태
	CAnimation* pBanCho_ServeIdle = Engine::CAnimation::Create(L"BanCho_Player_Serve_Idle", m_pTextureCom_BanCho_PlayerServeIdle, 0, 0, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Serve_Idle", pBanCho_ServeIdle);

	CAnimation* pBanCho_ServeWalk = Engine::CAnimation::Create(L"BanCho_Player_Serve_Walk", m_pTextureCom_BanCho_PlayerServeWalk, 0, 7, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Serve_Walk", pBanCho_ServeWalk);

	CAnimation* pBanCho_ServeRun = Engine::CAnimation::Create(L"BanCho_Player_Serve_Run", m_pTextureCom_BanCho_PlayerServeRun, 0, 7, 0.10f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Serve_Run", pBanCho_ServeRun);

	CAnimation* pBanCho_ServeTired = Engine::CAnimation::Create(L"BanCho_Player_Serve_Tired", m_pTextureCom_BanCho_PlayerServeTired, 0, 7, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Serve_Tired", pBanCho_ServeTired);

	// QTE
	CAnimation* pBanCho_Tea = Engine::CAnimation::Create(L"BanCho_Player_Tea", m_pTextureCom_BanCho_PlayerTea, 0, 4, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Tea", pBanCho_Tea);

	CAnimation* pBanCho_Wasabi = Engine::CAnimation::Create(L"BanCho_Player_Wasabi", m_pTextureCom_BanCho_PlayerWasabi, 0, 1, 0.15f, true);
	m_pAnimator->Add_Animation(L"BanCho_Player_Wasabi", pBanCho_Wasabi);

	CAnimation* pBanCho_Eat = Engine::CAnimation::Create(L"BanCho_Player_Eat", m_pTextureCom_BanCho_PlayerEat, 0, 6, 0.15f, false);
	m_pAnimator->Add_Animation(L"BanCho_Player_Eat", pBanCho_Eat);

	Enter_Animation(IDLE);
}


void CBancho_Player::BanCho_Player_Subscribe()
{
	
	
	//CEventMgr::GetInstance()->Subscribe(EVENT::PLAYER::PLAYER_EXITFAIL, this, [this](const Event& e)
	//	{
	//		
	//
	//	}
	//);



}

void CBancho_Player::BanCho_PlayerSound()
{
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Bancho_Player/dave_step_001.wav", L"Bancho_Player_Move");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Bancho_Player/daveDash.mp3", L"daveDash");
}


void CBancho_Player::Enter_Animation(BANCHOPLAYER_ANIM eNewAnim)
{
	m_eCurAnim = eNewAnim;

	if (m_ePrevAnim != m_eCurAnim)
	{
		switch (m_eCurAnim)
		{
		case BANCHOPLAYER_ANIM::IDLE:
			m_pAnimator->Play_Animation(L"BanCho_Player_Idle", true);
			break;

		case BANCHOPLAYER_ANIM::WALK:
			m_pAnimator->Play_Animation(L"BanCho_Player_Walk", true);
			break;

		case BANCHOPLAYER_ANIM::RUN:
			m_pAnimator->Play_Animation(L"BanCho_Player_Run", true);
			break;

		case BANCHOPLAYER_ANIM::TIRED:
			m_pAnimator->Play_Animation(L"BanCho_Player_Tired", true);
			break;

		case BANCHOPLAYER_ANIM::SERVE_IDLE:
			m_pAnimator->Play_Animation(L"BanCho_Player_Serve_Idle", true);
			break;

		case BANCHOPLAYER_ANIM::SERVE_WALK:
			m_pAnimator->Play_Animation(L"BanCho_Player_Serve_Walk", true);
			break;

		case BANCHOPLAYER_ANIM::SERVE_RUN:
			m_pAnimator->Play_Animation(L"BanCho_Player_Serve_Run", true);
			break;

		case BANCHOPLAYER_ANIM::SERVE_TIRED:
			m_pAnimator->Play_Animation(L"BanCho_Player_Serve_Tired", true);
			break;

		case BANCHOPLAYER_ANIM::TEA:
			m_pAnimator->Play_Animation(L"BanCho_Player_Tea", true);
			break;

		case BANCHOPLAYER_ANIM::WASABI:
			m_pAnimator->Play_Animation(L"BanCho_Player_Wasabi", true);
			break;

		case BANCHOPLAYER_ANIM::EAT:
			m_pAnimator->Play_Animation(L"BanCho_Player_Eat", true);
			break;

		default:
			break;
		}

		m_ePrevAnim = m_eCurAnim;
	}
}

void CBancho_Player::Chuck_Food()
{
	if (m_tOrderFood.Sushi_Data.SushiTextureId == -1)
		return;

	Reset_Food();
}

void CBancho_Player::Reset_Food()
{
	m_tOrderFood.pCustomer = nullptr;
	m_tOrderFood.Sushi_Data.SushiTextureId = -1; 
	m_tOrderFood.Sushi_Data.Korean_Name = L"";
	m_bServe = false;
}
