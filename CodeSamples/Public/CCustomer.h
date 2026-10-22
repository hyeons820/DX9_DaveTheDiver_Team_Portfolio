#pragma once
#include "CGameObject.h"
#include "CChair.h"
#include "CBoat_SushiMgr.h"
#include "CUI_Customer.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimator;
	class CAnimation;
}

class CCustomer : public CGameObject
{
public:
	enum CUSTOMER_TYPE {
		MAN00, MAN01, WOMAN00, WOMAN01, WOMAN02, CUSTOMER_END
	};
	
	enum CLOTH_TYPE {
		CLOTH01, CLOTH_END
	};

	enum CUSTOMER_STATE {
		WALK, TEA, MENU, WAIT, EAT, HAPPY, ANGER, EXIT, CUSSTATE_END
	};

private:
	explicit CCustomer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCustomer(const CGameObject& rhs);
	virtual ~CCustomer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision_Enter(CGameObject* pObj) override;
	virtual			void		OnCollision_Stay(CGameObject* pObj) override;
	virtual			void		OnCollision_Exit(CGameObject* pObj) override;

public:
	void Set_TargetChair(CChair* pChair) { m_pTargetChair = pChair; }
	void Set_TimerFlow(_bool bTimerFlow) { m_bTimerFlow = bTimerFlow; }
	void Set_CurState(CUSTOMER_STATE eState);
	void Set_OrderInfo(CBoat_SushiMgr::Registered_Sushi tSushi) { m_tOrderInfo = tSushi; }
	void Set_ServeGreenTea() { m_bTeaServe = true; }

	_bool Get_EmptyTargetChair() { return m_pTargetChair == nullptr; }
	_bool Get_Exit() { return m_bExit; }
	_bool Get_WantGreenTea() { return m_bGreenTea; }
	_bool Get_ServeGreenTea() { return m_bTeaServe; }
	CUSTOMER_STATE Get_CurState() { return m_eCurState; }
	CBoat_SushiMgr::Registered_Sushi Get_OrderInfo() { return m_tOrderInfo; }
	_float Get_PatienceRatio() { return m_fActionTimer / m_fTargetActionTime; }
	_float Get_ActionTimer() { return m_fActionTimer; }
	_float Get_TargetActionTime() { return m_fTargetActionTime; }

	_bool Is_MyOrder(int iSushi);
	_bool Is_SameOrder(int iSushi);

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Material();
	void			Ready_Animation();

	void Play_RandomSound(const wstring& strKey, _int iCount, CHANNELID eID);

private:
	void			Enter_Animation(CUSTOMER_STATE eNewAnim);

private:
	Engine::CRcTex* m_pBufferCom;

	// 몸
	Engine::CTexture* m_pTextureCom_Customer_Walk;				// 걷기
	Engine::CTexture* m_pTextureCom_Customer_Menu;				// 메뉴 선택
	Engine::CTexture* m_pTextureCom_Customer_Wait;				// 대기
	Engine::CTexture* m_pTextureCom_Customer_Eat;				// 먹기
	Engine::CTexture* m_pTextureCom_Customer_Happy;				// 기분 굿
	Engine::CTexture* m_pTextureCom_Customer_Anger;				// 화남
	
	// 옷 상태
	Engine::CTexture* m_pTextureCom_Cloth_Walk;				// 걷기
	Engine::CTexture* m_pTextureCom_Cloth_Menu;				// 메뉴 선택
	Engine::CTexture* m_pTextureCom_Cloth_Wait;				// 대기
	Engine::CTexture* m_pTextureCom_Cloth_Eat;				// 먹기
	Engine::CTexture* m_pTextureCom_Cloth_Happy;			// 기분 굿
	Engine::CTexture* m_pTextureCom_Cloth_Anger;			// 화남

	Engine::CAnimator* m_pBodyAnimator;
	Engine::CAnimator* m_pClothAnimator;

	wstring m_strProto;
	wstring m_strCloth;

	wstring m_strWalkKey;
	vector<wstring> m_vecTag;

	CChair* m_pTargetChair;

	_float m_fSpeed;
	_float m_fTargetPosX;
	_float m_fDistance;

	_float m_fActionTimer = 0.f;      // 현재 행동의 누적 시간
	_float m_fTargetActionTime = 0.f; // 현재 행동이 완료될 목표 시간

	_bool m_bExit;
	_bool m_bGreenTea;
	_bool m_bTeaServe;
	_bool m_bTimerFlow;
	_bool m_bHeartSecondTrigger = false;
	_bool m_bSmileTrigger = false;

	_vec3 m_vHeartPos;
	_vec3 m_vAngerPos;

	CBoat_SushiMgr::Registered_Sushi m_tOrderInfo;

	CUI_Customer* m_pUI;

public:
	static CCustomer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

private:
	CUSTOMER_STATE m_eCurAnim = CUSSTATE_END;
	CUSTOMER_STATE m_ePrevAnim = CUSSTATE_END;

	CUSTOMER_STATE m_eCurState = CUSSTATE_END;
	CUSTOMER_STATE m_ePreState = CUSSTATE_END;

	CUSTOMER_TYPE m_eCustomerType = CUSTOMER_END;
	CLOTH_TYPE	  m_eClothType = CLOTH_END;
};

