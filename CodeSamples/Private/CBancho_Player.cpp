// 포트폴리오용 발췌본: hyeons820의 초밥집 상호작용 구현을 중심으로 정리했습니다.
// 팀 공통 클래스·함수 틀을 사용하며, 플레이어 전체를 단독 구현했다는 의미는 아닙니다.
// [중략] 부분이 있는 코드 열람용 샘플로, 단독 빌드·실행용 파일이 아닙니다.
// 근거 커밋: 35e63ba, 116733a, 511c93d, 684c358, f1dce7f, 4950f19, 7694603.
#include "pch.h"
#include "CBancho_Player.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"

// [중략] 팀원이 구현한 생성자, 컴포넌트·애니메이션 초기화와 렌더링.
HRESULT CBancho_Player::Ready_GameObject()
{
    // [중략] 기존 플레이어 초기화. 아래는 본인이 추가한 효과음 로딩입니다.

	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Serve/sushi_food_receive_02.wav", L"BanchoPlayer_Serve");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/Sushi_Sound/Customer/Serve/sushi_dump_02.wav", L"BanchoPlayer_Dump");

    // [중략] 공통 초기화 반환 처리.
}

// 녹차 제공 중 기존 키 입력 처리를 제한합니다.

void CBancho_Player::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bTea)
	{
		Key_Input(fTimeDelta);
	}

    // [중략] 공통 LateUpdate 호출.
}

// 완성된 음식 수령, 손님 주문에 맞는 서빙, 녹차 UI 연결.

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

    // [중략] 팀원의 UI 이벤트 연결이 포함된 와사비 분기는 제외했습니다.
}

void CBancho_Player::Key_Input(const _float& fTimeDelta)
{
    // [중략] 기존 입력 가능 여부 검사와 상태 갱신.

	if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_Q))
	{
		CSoundMgr::GetInstance()->PlaySFX_Once(L"BanchoPlayer_Dump", SOUND_BANCHO_PLAYER_SERVE, 1.f);
		Chuck_Food();

        // [중략] 팀원이 추가한 먹기 애니메이션 전환과 반환 처리.

	}

    // [중략] 기존 이동 입력 처리.
}

// 들고 있는 음식 버리기 및 주문 데이터·서빙 상태 초기화.

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
