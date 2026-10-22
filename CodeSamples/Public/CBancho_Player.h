#pragma once
#include "CGameObject.h"
#include "CGameObjMgr.h"
#include "CBancho.h"
#include "CRestaurantMgr.h"

namespace Engine
{
	class CRcPlayerTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CAnimator;
	class CAnimation;
}


class CBancho_Player : public CGameObject
{
	using	WEAPON = CGameObjMgr::WEAPON;

public:
	enum BANCHOPLAYER_ANIM {
		None,
		IDLE, WALK, RUN, TIRED,
		SERVE_IDLE, SERVE_WALK, SERVE_RUN, SERVE_TIRED,
		WASABI,TEA,EAT
	};


private:
	explicit CBancho_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBancho_Player(const CGameObject& rhs);
	virtual ~CBancho_Player();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision_Enter(CGameObject* pObj) override;
	virtual			void		OnCollision_Stay(CGameObject* pObj) override;
	virtual			void		OnCollision_Exit(CGameObject* pObj) override;
private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Key_Input_Status_Update(const _float& fTimeDelta);
	_bool			Key_Input_Interaction(const _float& fTimeDelta);
	void			Key_Input_Movement(const _float& fTimeDelta);


	HRESULT			Ready_Material();
	void			Ready_Animation();

private:
	void			BanCho_Player_Subscribe();
	void			BanCho_PlayerSound();

private:
	void			Enter_Animation(BANCHOPLAYER_ANIM eNewAnim);
	void			Chuck_Food();
	void			Reset_Food();

public:
	_float			Get_CurRun_Gauage() { return m_fCurRun_Gauage; }
	_bool			Get_FlipX() {return m_bFlipX;}
	CRestaurantMgr::CUSTOMER_ORDER	Get_OrderFood() {return m_tOrderFood;}
	
	void			Set_StateTea() { m_bTea = false; }

private:
	Engine::CRcPlayerTex* m_pBufferCom;

	// 일반 상태
	Engine::CTexture* m_pTextureCom_BanCho_PlayerIdle;				// 일반 idle

	Engine::CTexture* m_pTextureCom_BanCho_PlayerWalk;				// 일반 walk
	Engine::CTexture* m_pTextureCom_BanCho_PlayerRun;				// 일반 뛰기
	Engine::CTexture* m_pTextureCom_BanCho_PlayerTired;				// 일반 힘듬

	// SERVE 상태
	Engine::CTexture* m_pTextureCom_BanCho_PlayerServeIdle;			// 일반 idle

	Engine::CTexture* m_pTextureCom_BanCho_PlayerServeWalk;			// 일반 walk
	Engine::CTexture* m_pTextureCom_BanCho_PlayerServeRun;			// 일반 뛰기
	Engine::CTexture* m_pTextureCom_BanCho_PlayerServeTired;		// 일반 힘듬


	//QTE		WASABI,TEA
	Engine::CTexture* m_pTextureCom_BanCho_PlayerWasabi;			// 일반 walk
	Engine::CTexture* m_pTextureCom_BanCho_PlayerTea;				// 일반 뛰기
	Engine::CTexture* m_pTextureCom_BanCho_PlayerEat;				// 먹기


	Engine::CCalculator* m_pCalculatorCom;
	Engine::CAnimation* m_pAnimation;
	Engine::CAnimator* m_pAnimator;


public:
	static CBancho_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();


private:
	BANCHOPLAYER_ANIM m_eCurAnim = None;
	BANCHOPLAYER_ANIM m_ePrevAnim = None;

private:
	_float m_fSpeed;

	CCollider* pCollider;							// 콜라이더

	_bool	m_bFlipX = false;

	_bool	m_bTired = false;
	_float  m_fMaxRun_Gauage = 100.f;
	_float  m_fCurRun_Gauage;
	_float  m_fRecovery_Time = 0.f;

	_bool   m_bServe = false;
	_bool   m_bTea = false;

	_float  m_fWalkTime = 0.f;

	CRestaurantMgr::CUSTOMER_ORDER m_tOrderFood = {};

};

