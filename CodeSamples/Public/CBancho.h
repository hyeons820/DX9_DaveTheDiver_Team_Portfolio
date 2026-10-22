#pragma once
#include "CGameObject.h"
#include "CGameObjMgr.h"
#include "CRestaurantMgr.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimator;
	class CAnimation;
}


class CBancho : public CGameObject
{
public:
	enum BANCHO_STATE {
		IDLE, COOK, COOK_WAIT, BANCHO_END
	};

private:
	explicit CBancho(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBancho(const CGameObject& rhs);
	virtual ~CBancho();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision_Enter(CGameObject* pObj) override;
	virtual			void		OnCollision_Stay(CGameObject* pObj) override;
	virtual			void		OnCollision_Exit(CGameObject* pObj) override;

public:
	_bool Is_EmptyFinishedDish() { return m_vecFinishedDish.empty(); }
	CRestaurantMgr::CUSTOMER_ORDER Get_FinishedDish();
	CRestaurantMgr::CUSTOMER_ORDER Get_FinishedDish_Info() { return m_tOrderDish; }
	vector<CRestaurantMgr::CUSTOMER_ORDER>& Get_CookingList() { return m_vecFinishedDish; }
	_float Get_CookTime() { return m_fCookTime; }
	_float Get_TargetTime() { return m_fTargetTime; }
	BANCHO_STATE Get_CurState() { return m_eCurState; }

private:
	HRESULT			Add_Component();

	HRESULT			Ready_Material();
	void			Ready_Animation();

private:
	void			BanCho_Subscribe();
	void			Set_CurState(BANCHO_STATE eState);
	void			Enter_Animation(BANCHO_STATE eNewAnim);

private:
	Engine::CRcTex* m_pBufferCom;

	// 일반 상태
	Engine::CTexture* m_pTextureCom_BanCho_Idle;				// 일반 idle
	Engine::CTexture* m_pTextureCom_BanCho_Cook;				// 요리
	Engine::CTexture* m_pTextureCom_BanCho_CookWait;			// 요리 대기

	Engine::CAnimation* m_pAnimation;
	Engine::CAnimator* m_pAnimator;

	_int m_CookCount;

	_float m_fCookTime;
	_float m_fTargetTime;

	CRestaurantMgr::CUSTOMER_ORDER m_tOrderDish;
	vector<CRestaurantMgr::CUSTOMER_ORDER> m_vecFinishedDish;

public:
	static CBancho* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();


private:
	BANCHO_STATE m_eCurAnim = BANCHO_END;
	BANCHO_STATE m_ePrevAnim = BANCHO_END;

	BANCHO_STATE m_eCurState = BANCHO_END;
	BANCHO_STATE m_ePreState = BANCHO_END;
};

