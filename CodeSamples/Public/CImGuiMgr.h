// 포트폴리오용 발췌본: 전체 프로젝트에서 사용하는 완전한 구현이 아닙니다.
// 공동 구현: 맵 저장·로드, 피킹, 오브젝트 생성·선택·속성 편집.
// 물고기 편집 기능은 제외하고, 초기 설정과 반복 코드는 [중략]으로 표시했습니다.
// [중략] 초기 설정·물고기 편집·종료 처리의 선언 및 전용 멤버.
#pragma once

#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"
#include <iostream>
#include <fstream>
#include <json/json.h>

#if defined(USE_IMGUI)

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"


namespace Engine
{
    class CGameObject;
}

class CImGuiMgr final : public CBase
{
#pragma region Singleton
public:
    static CImGuiMgr* GetInstance();
    static void DestroyInstance();

private:
    static CImGuiMgr* m_pInstance;
#pragma endregion

private:
    CImGuiMgr();
    CImGuiMgr(const CImGuiMgr& rhs) = delete;
    CImGuiMgr& operator=(const CImGuiMgr& rhs) = delete;
    virtual ~CImGuiMgr();

public:

public:
    bool IsInputCapturedByUI();
    void Reset_Object() { m_pGameObject = nullptr; }

public:
    void Save_Map(const _tchar* pLayerTag, const std::string& filename);
    std::string WStringToUTF8(const std::wstring& wstr);

    void Load_Map(const _tchar* pLayerTag, const std::string& filename);
    std::wstring UTF8ToWString(const std::string& str);


public:
    void Pick_GameObject(const _tchar* pLayerTag);
    void Get_AABB(CGameObject* pObj, _vec3* pBoxMin, _vec3* pBoxMax);
    void Get_MouseRay(_vec3* pRayPos, _vec3* pRayDir);


private:
    void Edit_MapObject();
    void Show_Obj();
    void Show_Component_Prototype();

private:
    LPDIRECT3DDEVICE9       m_pGraphicDev;
    HWND                    m_hWnd;

    CGameObject* m_pGameObject;


private:
    wstring      m_SelectedFontTag = L"";
    _vec2        m_FontPreviewPos = { 0.f, 0.f };
    D3DXCOLOR    m_FontPreviewColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

public:
};

#endif