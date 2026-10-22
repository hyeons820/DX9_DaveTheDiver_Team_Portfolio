#pragma once
#include "CGameObject.h"
#include "CBase.h"
#include "CTransform.h"
#include <iostream>
#include <queue>
#include "CChair.h"
#include "CCustomer.h"
#include "CLayer.h"
#include "CBoat_SushiMgr.h"

class CBancho_Player;
class CBancho;
class CUI_GreenTea;
class CWasabi;

class CRestaurantMgr : public CBase
{
public:
	struct CUSTOMER_ORDER
	{
		CCustomer* pCustomer;
		CBoat_SushiMgr::Registered_Sushi Sushi_Data;
	};

	struct RESULT_INFO
	{
		_int iTip = 0; // ³ìÂ÷ ÆÁ
		_int iTotalGold = 0; // ½º½Ã °ñµå
		_int iSushiTextureID = -1; // Á© ¸¹ÀÌ ÆÈ¸° ½º½Ã
		_int iSushiCount = 0; // ¸î °³ ÆÈ·È´ÂÁö
	};

#pragma region Singleton
public:
	static CRestaurantMgr* GetInstance();
	static void DestroyInstance();

private:
	static CRestaurantMgr* m_pInstance;
#pragma endregion

private:
	explicit CRestaurantMgr();
	virtual ~CRestaurantMgr();

public:
	void Push_Chair(CChair* pChair);
	void Create_Customer(_int iMenuCount, CLayer* pLayer, LPDIRECT3DDEVICE9 m_pGraphicDev);
	void Set_Player(CBancho_Player* pPlayer);
	void Set_Bancho(CBancho* pBancho);
	void Set_GreenTeaUI(CUI_GreenTea* pGreenTeaUI);
	void Set_Wasabi(CWasabi* pWasabi);
	void GreenTea_Tip(_int iTip) { m_iTotalTip += iTip; }
	void Register_Sales(_int iTextureID, _int iPrice)	{ m_iTotalGold += iPrice; m_mapSalesCount[iTextureID]++; }
	void Set_GreenTeaUIScreen(CCustomer* pCustomer);
	void Finish_GreenTea();

	CChair* Get_EmptyChair();
	CCustomer* Get_EmptyTarget();
	CBancho_Player* Get_Player();
	CBancho* Get_Bancho();
	CWasabi* Get_Wasabi();

	// °á°ú Á¾ÇÕ Á¤º¸
	RESULT_INFO Get_ResultInfo();
	
public:
	void Update_RestaurantMgr(const _float fTimeDelta);

	_bool Place_Order(CCustomer* pCustomer);
	_bool Start_Cook(CUSTOMER_ORDER* tOrder);

private:
	vector<CChair*> vecChair;
	vector<CCustomer*> vecCustomer;
	_float m_fTimer;
	_float m_fNextWaitTime;

	_int m_iTotalGold;
	_int m_iTotalTip;

	vector<CBoat_SushiMgr::Registered_Sushi>* m_vecMenu;
	queue<CUSTOMER_ORDER> m_queueOrder;

	CBancho_Player* m_pPlayer;
	CBancho* m_pBancho;
	CUI_GreenTea* m_pGreenTeaUI;
	CCustomer* m_pGreenTeaCustomer;
	CWasabi* m_pWasabi;

	map<int, int> m_mapSalesCount;

	RESULT_INFO m_tResultInfo;

	_bool m_bSpawnFinished;

private:
	void Free() override;
};

