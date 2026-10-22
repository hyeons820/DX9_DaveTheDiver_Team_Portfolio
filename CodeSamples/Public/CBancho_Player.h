#pragma once
// 포트폴리오용 발췌 선언입니다. 팀 공통 기반과 본인 추가 항목을 구분했습니다.
// 전체 선언·구현을 포함하지 않으므로 단독 빌드용이 아닙니다.
#include "CGameObject.h"
#include "CBancho.h"
#include "CRestaurantMgr.h"

class CBancho_Player : public CGameObject
{
public:
    // 기존 팀 코드: 본인의 녹차 연동 코드에서 사용하는 애니메이션 상태입니다.
    enum BANCHOPLAYER_ANIM {
        None,
        IDLE, WALK, RUN, TIRED,
        SERVE_IDLE, SERVE_WALK, SERVE_RUN, SERVE_TIRED,
        WASABI, TEA, EAT
    };

    // 기존 함수 틀에 본인이 추가한 부분만 CPP에 발췌했습니다.
    virtual HRESULT Ready_GameObject();
    virtual void LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void OnCollision_Stay(CGameObject* pObj) override;

private:
    // 기존 입력·애니메이션 인터페이스. 애니메이션 구현은 중략했습니다.
    void Key_Input(const _float& fTimeDelta);
    void Enter_Animation(BANCHOPLAYER_ANIM eNewAnim);

    // 본인 추가: 음식 버리기와 서빙 데이터 초기화.
    void Chuck_Food();
    void Reset_Food();

public:
    // 본인 추가: 주문 데이터 조회 및 녹차 제공 종료 시 상태 해제.
    CRestaurantMgr::CUSTOMER_ORDER Get_OrderFood() { return m_tOrderFood; }
    void Set_StateTea() { m_bTea = false; }

private:
    // 기존 팀 코드의 서빙 상태를 음식 수령·전달 로직에 사용했습니다.
    _bool m_bServe = false;

    // 본인 추가: 녹차 제공 상태와 현재 들고 있는 주문 데이터.
    _bool m_bTea = false;
    CRestaurantMgr::CUSTOMER_ORDER m_tOrderFood = {};

    // [중략] 생성자, 이동·렌더링·애니메이션 및 컴포넌트 관련 선언·멤버.
};
