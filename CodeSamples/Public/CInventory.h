#pragma once
#include "CUi.h"
#include "CItemSlot.h"
#include "CPlayerAnim.h"

class CInventory : public CUi
{
public:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT Ready_Font();
	void Refresh_SlotData(int& iStartDataIndex);
	void Trash_Fish();

private:
	CUi_Image* m_pBG = nullptr;

	CUi_Image* m_pEquipPanel = nullptr;
	CUi_Image* m_pCostumePlate = nullptr;
	CUi_Image* m_pHarpoonGunSlot = nullptr;
	CUi_Image* m_pHarpoonProjectile = nullptr;
	CUi_Image* m_pHarpoonGunSlot2 = nullptr;
	CUi_Image* m_pMeleeWeaponSlot = nullptr;
	CUi_Image* m_pDaggerIcon = nullptr;
	CUi_Image* m_pFirearmsSlot = nullptr;
	CUi_Image* m_pAmuletSlot1 = nullptr;
	CUi_Image* m_pAmuletSlot2 = nullptr;
	CUi_Image* m_pAuxiliaryToolSlot1 = nullptr;
	CUi_Image* m_pAuxiliaryToolSlot2 = nullptr;

	CUi_Image* m_pHarpoon = nullptr;
	CUi_Image* m_pGun = nullptr;

	CUi_Image* m_pMissionPanel = nullptr;
	CUi_Image* m_pMissionSlot1 = nullptr;
	CUi_Image* m_pMissionSlot2 = nullptr;
	CUi_Image* m_pMissionSlot3 = nullptr;

	CUi_Image* m_pEcologyPanel = nullptr;
	CUi_Image* m_pEcowatcherLogo = nullptr;
	CUi_Image* m_pEcowatcherMedal = nullptr;

	CUi_Image* m_pGuestnPanel = nullptr;

	CUi_Image* m_pAcquiredObjectPanel = nullptr;
	CUi_Image* m_pChuckSlot = nullptr;
	CUi_Image* m_pSelectSlot = nullptr;

	CUi_Image* m_pTitle = nullptr;
	CUi_Image* m_pBoatIcon = nullptr;
	CUi_Image* m_pCKey = nullptr;
	CUi_Image* m_pTildaKey = nullptr;
	CUi_Image* m_pF1Key = nullptr;
	CUi_Image* m_pSunIcon = nullptr;
	CUi_Image* m_pWaveKey = nullptr;

	CUi_Image* m_pTrash_Panel = nullptr;
	CUi_Image* m_pExit_Panel = nullptr;

	CPlayerAnim* m_pPlayerAnim;

// Slot
	vector<CItemSlot*> m_vecSlot;

	_int m_iMaxSlotCount = 9;
	_int iStartDataIndex;

	_float m_fStartY = 136.8f;
	_float m_fSlotHeight = 65.f;
	_float m_fScrollY;

	RECT m_rcScissor = { 700, 165, 1345, 750 }; // ÆÐ³Î ³»ºÎ ½½·Ô ¿µ¿ªÀÇ ÇÈ¼¿ ÁÂÇ¥

	_int m_iSelectIdx;

	_bool bFind;

public:
	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};