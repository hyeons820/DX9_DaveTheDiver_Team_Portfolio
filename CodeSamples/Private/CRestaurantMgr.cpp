#include "pch.h"
#include "CRestaurantMgr.h"
#include "CFactoryMgr.h"
#include "CBancho_Player.h"
#include "CUI_GreenTea.h"
#include "CWasabi.h"

CRestaurantMgr* CRestaurantMgr::m_pInstance = nullptr;

CRestaurantMgr* CRestaurantMgr::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CRestaurantMgr;
	}

	return m_pInstance;
}

void CRestaurantMgr::DestroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CRestaurantMgr::CRestaurantMgr() : m_fTimer(0.f), m_fNextWaitTime(0.f), m_iTotalGold(0), m_iTotalTip(0), m_pPlayer(nullptr), m_pGreenTeaUI(nullptr), m_pGreenTeaCustomer(nullptr), m_pWasabi(nullptr), m_pBancho(nullptr), m_bSpawnFinished(false)
{
}

CRestaurantMgr::~CRestaurantMgr()
{
	Free();
}

void CRestaurantMgr::Push_Chair(CChair* pChair)
{
	if (pChair)
	{
		vecChair.push_back(pChair);
	}
}

void CRestaurantMgr::Create_Customer(_int iMenuCount, CLayer* pLayer, LPDIRECT3DDEVICE9 pGraphicDev)
{
	for (int i = 0; i < iMenuCount; ++i)
	{
		CCustomer* pCustomer = CCustomer::Create(pGraphicDev);
		if (FAILED(CFactoryMgr::GetInstance()->Insert_Object(pCustomer, L"CCustomer", pLayer)))
			return;
		vecCustomer.push_back(pCustomer);
	}
	m_bSpawnFinished = true;
}

void CRestaurantMgr::Set_Player(CBancho_Player* pPlayer)
{ 
	m_pPlayer = pPlayer; 
}

void CRestaurantMgr::Set_Bancho(CBancho* pBancho)
{ 
	m_pBancho = pBancho;
}

void CRestaurantMgr::Set_GreenTeaUI(CUI_GreenTea* pGreenTeaUI)
{
	m_pGreenTeaUI = pGreenTeaUI;
}

void CRestaurantMgr::Set_Wasabi(CWasabi* pWasabi)
{
	m_pWasabi = pWasabi;
}

CChair* CRestaurantMgr::Get_EmptyChair()
{
	vector<CChair*> vecEmpty;

	for (auto& pChair : vecChair) 
	{
		if (pChair->Get_IsEmpty()) 
			vecEmpty.push_back(pChair);
	}

	if (vecEmpty.empty()) 
		return nullptr;

	return vecEmpty[rand() % vecEmpty.size()];
}

CCustomer* CRestaurantMgr::Get_EmptyTarget()
{
	vector<CCustomer*> vecEmpty;

	for (auto& pCustomer : vecCustomer)
	{
		if (pCustomer->Get_EmptyTargetChair())
			vecEmpty.push_back(pCustomer);
	}

	if (vecEmpty.empty())
		return nullptr;

	return vecEmpty[rand() % vecEmpty.size()];
}

CBancho_Player* CRestaurantMgr::Get_Player()
{
	if (m_pPlayer)
		return m_pPlayer;
}

CBancho* CRestaurantMgr::Get_Bancho()
{
	if (m_pBancho)
		return m_pBancho;
}

CWasabi* CRestaurantMgr::Get_Wasabi()
{
	if (m_pWasabi)
		return m_pWasabi;
}

void CRestaurantMgr::Set_GreenTeaUIScreen(CCustomer* pCustomer)
{
	if (m_pGreenTeaUI)
	{
		m_pGreenTeaCustomer = pCustomer;
		m_pGreenTeaUI->Set_OpenScreen();
	}
}

void CRestaurantMgr::Finish_GreenTea()
{
	if (m_pGreenTeaCustomer) {
		m_pGreenTeaCustomer->Set_ServeGreenTea();  
		m_pGreenTeaCustomer->Set_TimerFlow(true); 
		m_pGreenTeaCustomer = nullptr;            
	}

	m_pPlayer->Set_StateTea();
}

CRestaurantMgr::RESULT_INFO CRestaurantMgr::Get_ResultInfo()
{
	if (m_mapSalesCount.empty())
	{
		m_tResultInfo.iSushiTextureID = -1;
		m_tResultInfo.iSushiCount = 0;
	}
	else
	{
		auto bestIt = max_element(m_mapSalesCount.begin(), m_mapSalesCount.end(),
			[](const auto& a, const auto& b) {
				return a.second < b.second;
			});

		m_tResultInfo.iSushiTextureID = bestIt->first;
		m_tResultInfo.iSushiCount = bestIt->second;
	}
	
	m_tResultInfo.iTotalGold = m_iTotalGold;
	m_tResultInfo.iTip = m_iTotalTip;

	return m_tResultInfo;
}

void CRestaurantMgr::Update_RestaurantMgr(const _float fTimeDelta)
{
	if (vecChair.empty())
		return;

	if (m_bSpawnFinished && vecCustomer.empty())
	{
		// 스폰 종료 및 손님 다 나가면
		CEventMgr::GetInstance()->Trigger(EVENT::UI::SUSHI_CLOSE);
		return;
	}

	m_fTimer += fTimeDelta;

	if (m_fNextWaitTime <= 0.f)
	{
		m_fNextWaitTime = (_float)(rand() % 6 + 2);
	}
	if (m_fTimer >= m_fNextWaitTime)
	{
		// 손님이 비어 있는 좌석으로 목표 설정
		CCustomer* pCustomer = Get_EmptyTarget();
		CChair* pChair = Get_EmptyChair();

		if (pCustomer && pChair)
		{
			pCustomer->Set_TargetChair(pChair);
			pCustomer->Set_CurState(CCustomer::WALK);
			pChair->Set_Occupant(pCustomer);
		}

		m_fNextWaitTime = (_float)(rand() % 6 + 2);

		m_fTimer = 0.f;
	}

	for (auto iter = vecCustomer.begin(); iter != vecCustomer.end(); )
	{
		// 손님이 화면 밖으로 나갔는지 확인
		if ((*iter)->Get_Exit())
		{
			iter = vecCustomer.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

_bool CRestaurantMgr::Place_Order(CCustomer* pCustomer)
{
	m_vecMenu = &CBoat_SushiMgr::GetInstance()->Get_Registered_SushiList();

	if (m_vecMenu->empty())
		return false;

	vector<int> vecPossibleOrder;

	// 개수가 1개 이상인 초밥만 주문 받게 함
	for (int i = 0; i < (int)m_vecMenu->size(); ++i)
	{
		if ((*m_vecMenu)[i].Total_SushiNum > 0)
		{
			vecPossibleOrder.push_back(i);
		}
	}

	if (vecPossibleOrder.empty())
	{
		return false;
	}

	int iRandomPick = rand() % vecPossibleOrder.size();
	int iTargetIndex = vecPossibleOrder[iRandomPick];

	(*m_vecMenu)[iTargetIndex].Total_SushiNum--;
	
	m_queueOrder.push({ pCustomer, (*m_vecMenu)[iTargetIndex] });
	pCustomer->Set_OrderInfo((*m_vecMenu)[iTargetIndex]);

	return true;
}

_bool CRestaurantMgr::Start_Cook(CUSTOMER_ORDER* tOrder)
{
	if (m_pWasabi->Get_NeedWasabi())
		return false;

	if (m_queueOrder.empty())
		return false;

	*tOrder = m_queueOrder.front();
	m_queueOrder.pop();

	m_pWasabi->Get_WasabiAmount() -= 25;

	return true;
}

void CRestaurantMgr::Free()
{
	vecChair.clear();
}
