#include "pch.h"
#include "CInventory.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CUIFontMgr.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev) : CUi(pGraphicDev), m_fScrollY(0.f), m_iSelectIdx(-1), m_pPlayerAnim(nullptr), bFind(false)
{
	m_wstrName = L"CUI_Inventory";

	m_vecSlot.reserve(m_iMaxSlotCount);
}

CInventory::~CInventory()
{}

HRESULT CInventory::Ready_GameObject()
{
	Ready_Font();

	m_UI_SequenceNum = UI_THR;

	//Get_Using_Main_Weapon
	//Get_Using_Sub_Weapon

	m_pBG = CreateSlot(L"Proto_Inventory_BG", { WINCX * 0.5f, WINCY * 0.5f }, { (_float)WINCX , (_float)WINCY }, COM_PAUSE);

	m_pEquipPanel = CreateSlot(L"Proto_Inventory_Equip_Slot", { -193.4f, 159.f }, { 200.f , 105.f }, COM_PAUSE);
	m_pCostumePlate = CreateSlot(L"Proto_Inventory_CostumePlate", { -335.f, 146.8f }, { 52.f , 69.5f }, COM_PAUSE, UI_TWO);
	m_pHarpoonGunSlot = CreateSlot(L"Proto_Inventory_HarpoonGun_Slot", { -235.f, 182.f }, { 47.f , 39.f }, COM_PAUSE, UI_TWO);
	m_pHarpoonProjectile = CreateSlot(L"Proto_Inventory_HarpoonProjectile", { -141.f, 174.f }, { 25.f , 5.f }, COM_PAUSE, UI_THR);
	m_pHarpoonGunSlot2 = CreateSlot(L"Proto_Inventory_HarpoonGun_Slot", { -141.8f, 182.f }, { 47.f , 39.f }, COM_PAUSE, UI_TWO);
	m_pMeleeWeaponSlot = CreateSlot(L"Proto_Inventory_HarpoonGun_Slot", { -48.8f, 182.f }, { 47.f , 39.f }, COM_PAUSE, UI_TWO);
	m_pDaggerIcon = CreateSlot(L"Proto_Inventory_Dagger_Icon", { -48.4f, 172.f }, { 20.f , 10.f }, COM_PAUSE, UI_THR);
	
	m_pHarpoon = CreateSlot(L"Proto_UI_Item", { -235.f, 172.f }, { 35.f , 35.f }, COM_PAUSE, UI_THR);
	m_pGun = CreateSlot(L"Proto_UI_Item", { -235.f, 102.f }, { 35.f , 35.f }, COM_PAUSE, UI_THR);

	m_pFirearmsSlot = CreateSlot(L"Proto_Inventory_HarpoonGun_Slot", { -235.f, 111.8f }, { 47.f , 39.f }, COM_PAUSE, UI_TWO);
	m_pAmuletSlot1 = CreateSlot(L"Proto_Inventory_AuxiliaryTool_Slot", { -165.f, 111.8f }, { 24.5f , 39.f }, COM_PAUSE, UI_TWO);
	m_pAmuletSlot2 = CreateSlot(L"Proto_Inventory_AuxiliaryTool_Slot", { -118.6f, 111.8f }, { 24.5f , 39.f }, COM_PAUSE, UI_TWO);
	m_pAuxiliaryToolSlot1 = CreateSlot(L"Proto_Inventory_AuxiliaryTool_Slot", { -71.8f, 111.8f }, { 24.5f , 39.f }, COM_PAUSE, UI_TWO);
	m_pAuxiliaryToolSlot2 = CreateSlot(L"Proto_Inventory_AuxiliaryTool_Slot", { -25.4f, 111.8f }, { 24.5f , 39.f }, COM_PAUSE, UI_TWO);

	m_pAcquiredObjectPanel = CreateSlot(L"Proto_Inventory_AcquiredGoods_Slot", { 199.f, -28.8f }, { 200.f , 284.f }, COM_PAUSE);
	m_pChuckSlot = CreateSlot(L"Proto_Chuck_Slot", { 350.f, 226.f }, { 33.f , 13.f }, COM_PAUSE);
	m_pSelectSlot = CreateSlot(L"Proto_Select_Slot", { 280.f, 227.2f }, { 33.f , 12.f }, COM_PAUSE);


	m_pMissionPanel = CreateSlot(L"Proto_Inventory_None_Slot", { -195.f, -65.f }, { 197.f , 135.f }, COM_PAUSE, UI_TWO);
	m_pMissionSlot1 = CreateSlot(L"Proto_Inventory_Mission_Slot", { -194.4f, -24.4f }, { 215.f , 33.f }, COM_PAUSE, UI_TWO);
	m_pMissionSlot2 = CreateSlot(L"Proto_Inventory_Mission_Slot", { -194.4f, -90.4f }, { 215.f , 33.f }, COM_PAUSE, UI_TWO);
	m_pMissionSlot3 = CreateSlot(L"Proto_Inventory_Mission_Slot", { -194.4f, -158.2f }, { 215.f , 33.f }, COM_PAUSE, UI_TWO);

	m_pTitle = CreateSlot(L"Proto_Inventory_Title", { 0, 271.f }, { 600.f , 23.f }, COM_PAUSE);
	m_pBoatIcon = CreateSlot(L"Proto_Inventory_Icon_Boat", { -345.f, 271.f }, { 16.f , 11.f }, COM_PAUSE, UI_TWO);
	m_pCKey = CreateSlot(L"Proto_C_Key_Dark", { 345.f, 271.f }, { 9.f , 12.f }, COM_PAUSE, UI_TWO);
	m_pTildaKey = CreateSlot(L"Proto_TildaKey", { -375.f, 271.f }, { 9.f , 12.f }, COM_PAUSE, UI_TWO);
	m_pF1Key = CreateSlot(L"Proto_F1Key", { 290.f, 271.f }, { 9.f , 12.f }, COM_PAUSE, UI_TWO);
	m_pSunIcon = CreateSlot(L"Proto_Sun_Icon", { 77.f, 271.f }, { 10.f , 12.f }, COM_PAUSE, UI_TWO);

	m_pEcologyPanel = CreateSlot(L"Proto_Inventory_Ecology_Slot", { -294.6f, -246.2f }, { 97.f , 47.f }, COM_PAUSE);
	m_pEcowatcherLogo = CreateSlot(L"Proto_Inventory_Ecowatcher_Logo", { -228.6f, -257.6f }, { 30.f , 33.4f }, COM_PAUSE, UI_TWO);
	m_pEcowatcherMedal = CreateSlot(L"Proto_Inventory_Ecowatcher_Medal", { -378.5f, -273.2f }, { 5.f , 7.f }, COM_PAUSE, UI_TWO);

	m_pGuestnPanel = CreateSlot(L"Proto_Inventory_None_Slot2", { -95.8f, -246.2f }, { 97.f , 47.f }, COM_PAUSE);

	m_pTrash_Panel = CreateSlot(L"Proto_Inven_Trash_Panel", { 0.f, 0.f }, { 400.f , 270.f }, COM_PAUSE, UI_FOR);
	m_pExit_Panel = CreateSlot(L"Proto_Inven_Exit_Panel", { 0.f, 0.f }, { 400.f , 270.f }, COM_PAUSE, UI_FOR);
	m_pTrash_Panel->Set_Active(false);
	m_pExit_Panel->Set_Active(false);

	for (int i = 0; i < m_iMaxSlotCount; ++i)
	{
		m_vecSlot.push_back(CItemSlot::Create(m_pGraphicDev));
	}

	CEventMgr::GetInstance()->Subscribe(
		EVENT::GAME::START_GAME,
		this,
		[this](const Event& e) {
			m_fScrollY = 0.f;
			bFind = false;
			m_iSelectIdx = -1;
			for (int i = 0; i < m_iMaxSlotCount; ++i)
			{
				m_vecSlot[i]->Set_Select(false);
			}
		}
	);

	m_pPlayerAnim = CPlayerAnim::Create(m_pGraphicDev , { -336.f, 143.f, 1.f },	{ 40.f, 55.f, 1.f });

	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/UI_Inven/ui_button_choice.wav", L"Inven_Choice");
	CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/SFX/UI_Inven/ui_button_click.wav", L"Inven_Click");

	return S_OK;
}

_int CInventory::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_pPlayerAnim->Update_GameObject(fTimeDelta);

	CGameObjMgr::WEAPON eMain = CGameObjMgr::GetInstance()->Get_Using_Main_Weapon();
	CGameObjMgr::WEAPON eSub = CGameObjMgr::GetInstance()->Get_Using_Sub_Weapon();

#pragma region 작살
	if (eMain == CGameObjMgr::OLD_HARPOONGUN || eSub == CGameObjMgr::OLD_HARPOONGUN)
		m_pHarpoon->Set_TextureNum(5);
	else if (eMain == CGameObjMgr::HARPOONGUN || eSub == CGameObjMgr::HARPOONGUN)
		m_pHarpoon->Set_TextureNum(6);

#pragma endregion

#pragma region 총

	if (eMain == CGameObjMgr::BASE_RIFLE || eSub == CGameObjMgr::BASE_RIFLE)
		m_pGun->Set_TextureNum(0);
	else if (eMain == CGameObjMgr::BASE_SINPER || eSub == CGameObjMgr::BASE_SINPER)
		m_pGun->Set_TextureNum(3);
	else if (eMain == CGameObjMgr::FIRE_SINPER || eSub == CGameObjMgr::FIRE_SINPER)
		m_pGun->Set_TextureNum(4);
	else if (eMain == CGameObjMgr::BASE_SHOTGUN || eSub == CGameObjMgr::BASE_SHOTGUN)
		m_pGun->Set_TextureNum(1);
	else if (eMain == CGameObjMgr::FIRE_SHOTGUN || eSub == CGameObjMgr::FIRE_SHOTGUN)
		m_pGun->Set_TextureNum(2);

#pragma endregion

	for (auto& Text : m_vecImgs)
		Text->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PAUSESCREEN, this);

	if (m_pExit_Panel->Get_Active())
	{
		if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
		{
			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);
			SCENE_TRANSITION_DATA data(SCENE_BOAT, 0.5f);
			CEventMgr::GetInstance()->Trigger(Event(EVENT::GAME::SCENE_TRANSITION_START, &data));
		}
		else if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_C))
		{
			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);
			m_pExit_Panel->Set_Active(false);
		}
	}
	else if (m_pTrash_Panel->Get_Active())
	{
		if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE))
		{
			Trash_Fish();
			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);
			m_pTrash_Panel->Set_Active(false);
		}
		else if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_C))
		{
			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);
			m_pTrash_Panel->Set_Active(false);
		}
	}

#pragma region 아이템 슬롯

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	int iActualItemCount = CGameObjMgr::GetInstance()->Get_FishInven().size();
	float m_fTotalHeight = m_iMaxSlotCount * m_fSlotHeight;

	// 첫 번째 위치의 슬롯의 물고기 정보 인덱스(Fish Inven)
	iStartDataIndex = (int)(m_fScrollY / m_fSlotHeight);

	if (PtInRect(&m_rcScissor, ptMouse) && !m_pExit_Panel->Get_Active() && !m_pTrash_Panel->Get_Active())
	{
		_long dwMouseMove(0);

		if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
		{
			m_fScrollY -= dwMouseMove * 0.2f;

			//스크롤이 너무 위나 아래로 가지 않도록 제한(아이템 개수에 따라 계산)
			float fMaxScroll = max(0.f, (iActualItemCount * m_fSlotHeight) - (m_fTotalHeight - m_fSlotHeight * 2.f));

			if (m_fScrollY <= -10.f)
				m_fScrollY = 0.f;
			else if (m_fScrollY >= fMaxScroll)
				m_fScrollY = fMaxScroll;
		}

		if (CDInputMgr::GetInstance()->Get_DIMouseDown(DIM_LB))
		{
			for (int i = 0; i < m_iMaxSlotCount; ++i)
			{
				int iCurDataIndex = iStartDataIndex + i; // 이 슬롯이 들고 있는 실제 데이터 번호

				if (m_vecSlot[i]->Get_Active() && m_vecSlot[i]->Is_MouseOver(ptMouse))
				{
					CSoundMgr::GetInstance()->PlaySFX(L"Inven_Click", SOUND_INVEN, 1.f);
					m_iSelectIdx = iCurDataIndex; // 실제 데이터 인덱스로 저장
					break;
				}
			}
		}
	}


	if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_W) && !m_pExit_Panel->Get_Active() && !m_pTrash_Panel->Get_Active()) // 위로
	{
		if (m_iSelectIdx > 0) // 첫 번째 아이템이 아니면
		{
			m_iSelectIdx--;

			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);

			if (m_iSelectIdx < iStartDataIndex)
			{
				m_fScrollY -= m_fSlotHeight;
			}
		}
	}
	else if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_S) && !m_pExit_Panel->Get_Active() && !m_pTrash_Panel->Get_Active()) // 아래로
	{
		if (m_iSelectIdx < iActualItemCount - 1) // 마지막 아이템이 아니면
		{
			m_iSelectIdx++;

			CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);

			if (m_iSelectIdx > iStartDataIndex + m_iMaxSlotCount - 3)
			{
				m_fScrollY += m_fSlotHeight;
			}
		}
	}
	else if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_GRAVE) && !m_pTrash_Panel->Get_Active())
	{
		m_pExit_Panel->Set_Active(true);
		CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);

	}
	else if (CDInputMgr::GetInstance()->Get_DIKeyDown(DIK_SPACE) && !m_pExit_Panel->Get_Active()
		&& m_iSelectIdx > -1 && m_iSelectIdx < iActualItemCount)
	{
		m_pTrash_Panel->Set_Active(true);
		CSoundMgr::GetInstance()->PlaySFX(L"Inven_Choice", SOUND_INVEN, 1.f);

	}

	// iStartDataIndex 기준으로 슬롯의 데이터 채우기
	Refresh_SlotData(iStartDataIndex);

	// 슬롯 설정
	for (int i = 0; i < m_iMaxSlotCount; ++i)
	{
		int iCurDataIndex = iStartDataIndex + i;

		// 데이터 범위를 벗어나는 슬롯은 숨기기
		if (iCurDataIndex >= iActualItemCount || iCurDataIndex < 0)
		{
			m_vecSlot[i]->Set_Active(false);
			continue;
		}
		m_vecSlot[i]->Set_Active(true);


		if (iCurDataIndex == m_iSelectIdx)
			m_vecSlot[i]->Set_Select(true);  // 선택 전용 테두리나 텍스처 켜기
		else
			m_vecSlot[i]->Set_Select(false); // 끄기


		// iCurDataIndex 기준으로 슬롯 포지션 바꿔주기
		float fRenderY = SLOT_OFFSET::BASE_POS.y - (iCurDataIndex * m_fSlotHeight) + m_fScrollY;
		m_vecSlot[i]->Get_Transform()->Set_Pos(SLOT_OFFSET::BASE_POS.x, fRenderY, 1.f);
	}

	for (auto& Slot : m_vecSlot)
	{
		Slot->Update_GameObject(fTimeDelta);
	}

#pragma endregion

	return S_OK;
}

void CInventory::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pPlayerAnim->LateUpdate_GameObject(fTimeDelta);

	for (auto& Slot : m_vecSlot)
		Slot->LateUpdate_GameObject(fTimeDelta);
}

void CInventory::Render_GameObject()
{
	_vec2		vPos;
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_GoOutByBoat");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_GoOutByBoat", L"배로 나가기", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_Setting");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_Setting", L"설정", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_Close");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_Close", L"닫기", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_MissionInProgress");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_MissionInProgress", L"진행 중인 미션", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_Item");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_Item", L"장비", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_EcologyResearch");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_EcologyResearch", L"환경 생태 연구", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_SpecialGuest");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_SpecialGuest", L"특별 손님", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_Snowstorm_EcowatcherLv");
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_EcowatcherLv", L"Lv.3", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_Snowstorm_EcowatcherExp");
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_EcowatcherExp", L"10/580", &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));	
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_3Missions");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_3Missions", L"미션 등록은 최대 3개까지 가능합니다.", &vPos, D3DXCOLOR(0.f, 171.f / 255.f, 178.f / 255.f, 1.f));	
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_NoGuest");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_NoGuest", L"방문 예정인 특별 손님이 없습니다.", &vPos, D3DXCOLOR(68.f / 255.f, 101.f / 255.f, 127.f / 255.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_NoMisson1");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_NoMisson1", L"현재 진행중인 미션이 없습니다.", &vPos, D3DXCOLOR(68.f / 255.f, 101.f / 255.f, 127.f / 255.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_NoMisson2");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_NoMisson2", L"현재 진행중인 미션이 없습니다.", &vPos, D3DXCOLOR(68.f / 255.f, 101.f / 255.f, 127.f / 255.f, 1.f));
	
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_NoMisson3");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_NoMisson3", L"현재 진행중인 미션이 없습니다.", &vPos, D3DXCOLOR(68.f / 255.f, 101.f / 255.f, 127.f / 255.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot1");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot1", L"작살총", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot2");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot2", L"작살촉", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot3");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot3", L"근접무기", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot4");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot4", L"총기", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot5");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot5", L"부적", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot6");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot6", L"부적", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot7");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot7", L"보조 도구", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_210YouthL_ItemSlot8");
	CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_ItemSlot8", L"보조 도구", &vPos, D3DXCOLOR(134.f / 255.f, 185.f / 255.f, 229.f / 255.f, 1.f));

	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_Snowstorm_Date");
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_Date", L"Thu 10. 13. Morning.", &vPos, D3DXCOLOR(83.f / 255.f, 207.f / 255.f, 255.f / 255.f, 1.f));
	
	wchar_t szWeight[16];
	swprintf_s(szWeight, L"%.1fkg", CGameObjMgr::GetInstance()->Get_MaxWeight());
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_Snowstorm_MaxWeight");
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_MaxWeight", szWeight, &vPos, D3DXCOLOR(66.f / 255.f, 142.f / 255.f, 177.f / 255.f, 1.f));
	
	float fCurWeight(0.f);
	for (auto& Item : CGameObjMgr::GetInstance()->Get_FishInven())
	{
		fCurWeight += Item.Kg;
	}
	wchar_t szCurWeight[16];
	swprintf_s(szCurWeight, L"%.1fkg", fCurWeight);
	vPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(m_wstrName, L"Font_Snowstorm_CurWeight");
	CFontMgr::GetInstance()->Render_Font(L"Font_Snowstorm_CurWeight", szCurWeight, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	m_pGraphicDev->SetScissorRect(&m_rcScissor);

	for (auto& Slot : m_vecSlot)
		Slot->Render_GameObject();

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	m_pPlayerAnim->Render_GameObject();

	m_pTrash_Panel->Render_GameObject();
	if (m_pTrash_Panel->Get_Active())
	{
		_vec2 vNamePos;
		_vec2 vFinalNamePos = { 115.f, 323.f };
		CFontMgr::GetInstance()->Render_Font(L"Font_210YouthL_FishTrashName", m_vecSlot[m_iSelectIdx - iStartDataIndex]->Get_FishData().Korean_Name.c_str(), &vFinalNamePos, D3DXCOLOR(1.f, 235.f / 255.f, 0.f, 1.f), DT_RIGHT);
	}

	m_pExit_Panel->Render_GameObject();
}

HRESULT CInventory::Ready_Font()
{
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_GoOutByBoat", DEFAULT_CHARSET, L"210 맨발의청춘 L", 20, 28, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_Setting", DEFAULT_CHARSET, L"210 맨발의청춘 L", 20, 28, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_Close", DEFAULT_CHARSET, L"210 맨발의청춘 L", 20, 28, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_MissionInProgress", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_EcologyResearch", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_SpecialGuest", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_3Missions", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 20, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_NoGuest", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_NoMisson1", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_NoMisson2", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_NoMisson3", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_Item", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot1", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot2", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot3", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot4", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot5", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot6", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 23, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot7", DEFAULT_CHARSET, L"210 맨발의청춘 L", 13, 22, FW_THIN, m_wstrName)))
		return E_FAIL;
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_ItemSlot8", DEFAULT_CHARSET, L"210 맨발의청춘 L", 13, 22, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_EcowatcherLv", DEFAULT_CHARSET, L"Snowstorm", 13, 28, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_EcowatcherExp", DEFAULT_CHARSET, L"Snowstorm", 14, 33, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_Date", DEFAULT_CHARSET, L"Snowstorm", 18, 36, FW_LIGHT, m_wstrName)))
		return E_FAIL;	
	
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_CurWeight", DEFAULT_CHARSET, L"Snowstorm", 11, 33, FW_LIGHT, m_wstrName)))
		return E_FAIL;
	
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_MaxWeight", DEFAULT_CHARSET, L"Snowstorm", 10, 25, FW_LIGHT, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_FishName", DEFAULT_CHARSET, L"210 맨발의청춘 L", 15, 20, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_210YouthL_FishTrashName", DEFAULT_CHARSET, L"210 맨발의청춘 L", 30, 50, FW_THIN, m_wstrName)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Snowstorm_FishRank", DEFAULT_CHARSET, L"Snowstorm", 13, 25, FW_THIN, m_wstrName)))
		return E_FAIL;

	return S_OK;
}

void CInventory::Refresh_SlotData(int& iStartDataIndex)
{
	for (int i = 0; i < m_iMaxSlotCount; ++i)
	{
		int iCurDataIndex = iStartDataIndex + i;

		if (iCurDataIndex < CGameObjMgr::GetInstance()->Get_FishInven().size())
		{
			m_vecSlot[i]->Set_FishData(iCurDataIndex, &CGameObjMgr::GetInstance()->Get_FishInven()[iCurDataIndex]);
		}
	}
}

void CInventory::Trash_Fish()
{
	if (m_iSelectIdx == -1)
		return;

	CGameObjMgr::GetInstance()->Delete_FishInven(m_iSelectIdx);

	m_iSelectIdx = -1;

	for (int i = 0; i < m_iMaxSlotCount; ++i)
	{
		m_vecSlot[i]->Set_Select(false);
	}
}

CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInventory* pInventory = new CInventory(pGraphicDev);

	if (FAILED(pInventory->Ready_GameObject()))
	{
		Safe_Release(pInventory);
		MSG_BOX("pInventory Create Failed");
		return nullptr;
	}

	return pInventory;
}

void CInventory::Free()
{
	CEventMgr::GetInstance()->Unsubscribe_All(this);

	Safe_Release(m_pPlayerAnim);

	for (auto& p : m_vecImgs)
		Safe_Release(p);
	m_vecImgs.clear();
	
	for (auto& p : m_vecSlot)
		Safe_Release(p);
	m_vecSlot.clear();

	CUi::Free();
}
