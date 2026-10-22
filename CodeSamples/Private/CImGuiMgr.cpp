// 포트폴리오용 발췌본: 전체 프로젝트에서 사용하는 완전한 구현이 아닙니다.
// 공동 구현: 맵 저장·로드, 피킹, 오브젝트 생성·선택·속성 편집.
// 물고기 편집 기능은 제외하고, 초기 설정과 반복 코드는 [중략]으로 표시했습니다.

#include "pch.h"
#include "CImGuiMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CProtoMgr.h"

#include "CFactoryMgr.h"
#include "CRock1.h"
#include "CBox.h"
#include "CItem.h"
#include "CCoralBush.h"
#include "CCoral.h"

#include <fstream>
#include "CUi.h"
#include "CUi_Image.h"
#include "CBanchoSushi.h"

#include "CFontMgr.h"
#include "CUIFontMgr.h"

#if defined(USE_IMGUI)

#pragma region Singleton

CImGuiMgr* CImGuiMgr::m_pInstance = nullptr;

CImGuiMgr* CImGuiMgr::GetInstance()
{
    if (!m_pInstance) {
        m_pInstance = new CImGuiMgr;
    }

    return m_pInstance;
}
void CImGuiMgr::DestroyInstance()
{
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}
#pragma endregion

CImGuiMgr::CImGuiMgr()
    : m_pGraphicDev(nullptr)
    , m_hWnd{}
    , m_pGameObject(nullptr)

{
}

CImGuiMgr::~CImGuiMgr()
{
    Free();
}

// [중략] ImGui 초기화, 프레임·도킹·패널 배치, 렌더링 및 폰트 로딩.

bool CImGuiMgr::IsInputCapturedByUI()
{
    //마우스 입력(클릭, 이동, 휠 등)이 UI에 의해 캡처되었는지 확인
    return ImGui::GetIO().WantCaptureMouse;
}

void CImGuiMgr::Save_Map(const _tchar* pLayerTag, const std::string& filename)
{
    Json::Value root;
    auto pLayer = CManagement::GetInstance()->Get_Layer(pLayerTag);
    if (!pLayer) return;

    auto mapObj = pLayer->Get_mapObj();

    for (auto& pair : *mapObj) {
        CGameObject* pObj = pair.second;
        if (!pObj) continue;

        wstring wCleanName = pObj->Get_Name();
        size_t findIndex = wCleanName.find(L'(');

        if (findIndex != wstring::npos)
        {
            wCleanName = wCleanName.substr(0, findIndex);
        }

        Json::Value j;
        j["Name"] = WStringToUTF8(wCleanName);

        CTransform* pTransform = pObj->Get_Transform();
        if (pTransform) {

            _vec3 vPos;
            pTransform->Get_Info(INFO_POS, &vPos);
            j["vPos"]["x"] = vPos.x;
            j["vPos"]["y"] = vPos.y;
            j["vPos"]["z"] = vPos.z;

            _vec3 vRotation = pTransform->m_vAngle;
            j["vRotation"]["x"] = vRotation.x;
            j["vRotation"]["y"] = vRotation.y;
            j["vRotation"]["z"] = vRotation.z;

            _vec3 vScale = pTransform->m_vScale;
            j["vScale"]["x"] = vScale.x;
            j["vScale"]["y"] = vScale.y;
            j["vScale"]["z"] = vScale.z;
        }
        root.append(j);
    }

    // 파일 쓰기
    std::ofstream file(filename);
    if (file.is_open()) {
        file << root.toStyledString(); // 이 한 줄이면 끝납니다.
        file.close();
    }

    MSG_BOX("Save");
}

void CImGuiMgr::Load_Map(const _tchar* pLayerTag, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return;

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    // 1. 파일 전체를 JSON 객체(배열)로 파싱
    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        // 파싱 실패 시 에러 출력 (디버깅용)
        std::cout << "Json Parse Error: " << errs << std::endl;
        return;
    }
    file.close();

    // 2. JSON 배열 순회 (저장된 모든 객체 정보 읽기)
    for (unsigned int i = 0; i < root.size(); ++i) {
        Json::Value j = root[i];

        // 이름 읽기 및 wstring 변환
        std::wstring wName = UTF8ToWString(j["Name"].asString());

        // 여기서 보통 신규 객체를 생성하거나 기존 객체를 찾습니다.
        // 예시: CGameObject* pObj = CGameObject::Create(...);

        // Transform 정보 추출
        _vec3 vScale, vPos, vRotation;

        // Scale
        vScale.x = j["vScale"]["x"].asFloat();
        vScale.y = j["vScale"]["y"].asFloat();
        vScale.z = j["vScale"]["z"].asFloat();

        // Position
        vPos.x = j["vPos"]["x"].asFloat();
        vPos.y = j["vPos"]["y"].asFloat();
        vPos.z = j["vPos"]["z"].asFloat();

        // Rotation
        vRotation.x = j["vRotation"]["x"].asFloat();
        vRotation.y = j["vRotation"]["y"].asFloat();
        vRotation.z = j["vRotation"]["z"].asFloat();

        // 3. 실제 객체에 적용 (엔진 구조에 맞게 호출)
        CGameObject* pObj = nullptr;
        if (wName == L"SunlightRock") {
            pObj = CRock1::Create(m_pGraphicDev, CRock1::SUNLIGHT);
            if (FAILED(CFactoryMgr::GetInstance()->Insert_Object(pObj, wName.c_str(), CManagement::GetInstance()->Get_Layer(pLayerTag)))){

            }
        }

        // [중략] 다른 맵 오브젝트 종류별 생성·레이어 등록 분기.

        if (pObj != nullptr) {
            pObj->Get_Transform()->m_vAngle = vRotation;
            pObj->Get_Transform()->m_vInfo[INFO_POS] = vPos;
            pObj->Get_Transform()->m_vScale = vScale;
        }

        pObj = nullptr;
    }

    MSG_BOX("Load");
}

std::string CImGuiMgr::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty())
        return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    std::string strTo(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

std::wstring CImGuiMgr::UTF8ToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    std::wstring wstrTo(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}


// [중략] 물고기 및 군집 저장·로드 기능은 공개 샘플에서 제외.

void CImGuiMgr::Pick_GameObject(const _tchar* pLayerTag)
{
    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(pLayerTag);

    //Layer 없으면 리턴
    if (!pLayer) {
        return;
    }

    auto mapObj = pLayer->Get_mapObj();

    // ray로부터 거리가 가장 짧은 obj 선택
    _float fDistance = FLT_MAX;

    //map에서 obj 순회
    for (auto iter = mapObj->begin(); iter != mapObj->end(); ++iter) {

        //obj Box AABB 가져옴
        _vec3 pBoxMin, pBoxMax;
        Get_AABB(iter->second, &pBoxMin, &pBoxMax);

        //마우스 클릭 좌표, 방향 가져옴
        _vec3 vRayPos, vRayDir;
        Get_MouseRay(&vRayPos, &vRayDir);

        //공식: P(t) = vRayPos + t * vRayDir
        _float fTMin(-FLT_MAX);
        _float fTMax(FLT_MAX);

        bool bCollision = TRUE;

        //x, y, z 검사
        for (int i = 0; i < 3; ++i) {

            const _float& fPos = *((_float*)(&vRayPos) + i);
            const _float& fDir = *((_float*)(&vRayDir) + i);
            const _float& fBoxMin = *((_float*)(&pBoxMin) + i);
            const _float& fBoxMax = *((_float*)(&pBoxMax) + i);

            //축과 평행한 경우
            if (fabsf(fDir) <= 0.0001f) {
                if (fPos < fBoxMin || fPos > fBoxMax)
                {
                    bCollision = FALSE;
                    break;
                }
            }
            //아닌 경우
            else {
                //공식
                float fCoordMin = (fBoxMin - fPos) / fDir;
                float fCoordMax = (fBoxMax - fPos) / fDir;

                //max랑 min중 더 짧은게(먼저 닿은게) min임
                if (fCoordMax < fCoordMin) {
                    swap(fCoordMax, fCoordMin);
                }

                //x, y, z 중 Tmin 중 최대, Tmax 중 최소 선택
                fTMin = max(fTMin, fCoordMin);
                fTMax = min(fTMax, fCoordMax);
            }

            //충돌하지 않은 경우
            if (fTMin > fTMax) {
                bCollision = FALSE;
                break;
            }
        }

        //충돌한 경우 가장 가까운 obj 선택
        if (fTMin >= 0 && bCollision) {
            if (fDistance > fTMin) {
                fDistance = fTMin;


                m_pGameObject = iter->second;
            }
        }
    }
}

//충돌체 com 생기면 교체
void CImGuiMgr::Get_AABB(CGameObject* pObj, _vec3* pBoxMin, _vec3* pBoxMax)
{
    CTransform* pTransform = pObj->Get_Transform();

    if (!pTransform) {
        *pBoxMin = *pBoxMax = { 0.f,0.f,0.f };
        return;
    }

    //정확한 판정 원하면 실제 obj 정점으로
    _vec3 vBox[8] =
    {
        _vec3(-1.f, -1.f, 1.f),
        _vec3(-1.f, 1.f, 1.f),
        _vec3(1.f, 1.f, 1.f),
        _vec3(1.f, -1.f, 1.f),
        _vec3(-1.f, -1.f, -1.f),
        _vec3(-1.f, 1.f, -1.f),
        _vec3(1.f, 1.f, -1.f),
        _vec3(1.f, -1.f, -1.f),
    };

    const _matrix* pWorldMatrix = pTransform->Get_World();

    *pBoxMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    *pBoxMin = { FLT_MAX, FLT_MAX, FLT_MAX };

    for (int i = 0; i < sizeof(vBox) / sizeof(_vec3); ++i)
    {
        D3DXVec3TransformCoord(&vBox[i], &vBox[i], pWorldMatrix);

        pBoxMax->x = max(pBoxMax->x, vBox[i].x);
        pBoxMax->y = max(pBoxMax->y, vBox[i].y);
        pBoxMax->z = max(pBoxMax->z, vBox[i].z);

        pBoxMin->x = min(pBoxMin->x, vBox[i].x);
        pBoxMin->y = min(pBoxMin->y, vBox[i].y);
        pBoxMin->z = min(pBoxMin->z, vBox[i].z);
    }
}

void CImGuiMgr::Get_MouseRay(_vec3* pRayPos, _vec3* pRayDir)
{
    POINT ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    // ViewPort
    _vec3 vMousePos;
    D3DVIEWPORT9 ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

    m_pGraphicDev->GetViewport(&ViewPort);

    vMousePos.x = (_float(ptMouse.x) / _float(ViewPort.Width)) * 2.f - 1.f;
    vMousePos.y = -(_float(ptMouse.y) / _float(ViewPort.Height)) * 2.f + 1.f;
    vMousePos.z = 0;

    // Projection
    _matrix matInvProj;
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matInvProj);
    D3DXMatrixInverse(&matInvProj, 0, &matInvProj);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matInvProj);

    // ViewSpace
    _matrix matInvView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matInvView);
    D3DXMatrixInverse(&matInvView, 0, &matInvView);

    *pRayPos = { 0.f, 0.f, 0.f };
    *pRayDir = vMousePos - *pRayPos;

    D3DXVec3TransformCoord(pRayPos, pRayPos, &matInvView);
    D3DXVec3TransformNormal(pRayDir, pRayDir, &matInvView);

    D3DXVec3Normalize(pRayDir, pRayDir);
}


// [중략] 물고기 배치용 마우스 좌표 계산 제외.

void CImGuiMgr::Edit_MapObject()
{
    CGameObject* pObj = nullptr;

    if (ImGui::CollapsingHeader("Environment_Layer")) {

        if (ImGui::Button("Coral0")) {
            pObj = CCoral::Create(m_pGraphicDev, CCoral::C0);
            if (FAILED(CFactoryMgr::GetInstance()->Insert_Object(pObj, L"Coral0", CManagement::GetInstance()->Get_Layer(L"Environment_Layer")))) {
                pObj = nullptr;
                return;
            }
            m_pGameObject = pObj;
        }

        // [중략] 산호·수초·배경 장식별 반복 생성 버튼.

    }

    if (ImGui::CollapsingHeader("GameLogic_Layer")) {

        if (ImGui::Button("SunlightRock")) {
            pObj = CRock1::Create(m_pGraphicDev, CRock1::SUNLIGHT);
            if (FAILED(CFactoryMgr::GetInstance()->Insert_Object(pObj, L"SunlightRock", CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")))) {
                pObj = nullptr;
                return;
            }
            m_pGameObject = pObj;
        }

        if (ImGui::Button("ChestBox")) {
            pObj = CBox::Create(m_pGraphicDev, CBox::CHEST);
            if (FAILED(CFactoryMgr::GetInstance()->Insert_Object(pObj, L"ChestBox", CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")))) {
                pObj = nullptr;
                return;
            }
            m_pGameObject = pObj;
        }

        // [중략] 다른 바위·상자·아이템별 반복 생성 버튼.

    }

    if (ImGui::CollapsingHeader("UI_Layer")) {
    }
}

// [중략] 물고기 편집 패널 제외.

void CImGuiMgr::Show_Obj()
{
    auto mapLayer = CManagement::GetInstance()->Get_Scene()->Get_mapLayer();

    // [중략] 배경 레이어 목록: 아래 환경 레이어와 같은 선택·삭제 구조.

    auto iter1 = find_if(mapLayer->begin(), mapLayer->end(), CTag_Finder(L"Environment_Layer"));

    if (iter1 == mapLayer->end()) {
        return;
    }

    if (ImGui::CollapsingHeader("Environment_Layer")) {

        auto mapObj = iter1->second->Get_mapObj();
        ImGui::Dummy(ImVec2(0.f, 5.f));

        int iCount(0);
        for (auto iter1 = mapObj->begin(); iter1 != mapObj->end();)
        {
            iCount++;
            wstring wstr = iter1->first;
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

            ImGui::PushID(iCount);

            ImGui::SameLine(200.f, 0.f);

            if (ImGui::Button(u8"삭제"))
            {
                m_pGameObject = nullptr;
                iter1 = CManagement::GetInstance()->Get_Layer(L"Environment_Layer")->Delete_Object(iter1->first);

                ImGui::PopID();
                continue;
            }

            ImGui::SameLine(10.f, 0.f);

            if (ImGui::Selectable(strTo.c_str())) {
                m_pGameObject = iter1->second;
                m_SelectedFontTag.clear();
            }
            ++iter1;

            ImGui::PopID();
            ImGui::Dummy(ImVec2(0.f, 5.f));
        }
    }

    // [중략] 게임 로직 레이어 목록: 같은 선택·삭제 구조 반복.

    auto iter3 = find_if(mapLayer->begin(), mapLayer->end(), CTag_Finder(L"UI_Layer"));

    if (iter3 == mapLayer->end()) {
        return;
    }

    if (ImGui::CollapsingHeader("UI_Layer")) {
        auto mapObj = iter3->second->Get_mapObj();

        CGameObject* pPicked = nullptr;

        for (auto it = mapObj->begin(); it != mapObj->end(); ++it)
        {
            wstring wstr = it->first;
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

            if (ImGui::TreeNode(strTo.c_str())) {
                CUi* pUi = dynamic_cast<CUi*>(it->second);
                if (pUi)
                {
                    const auto& imgs = pUi->Get_Images();

                    for (size_t i = 0; i < imgs.size(); ++i)
                    {
                        CUi_Image* pImg = imgs[i];
                        if (!pImg) continue;

                        const wchar_t* wkey = pImg->Get_TexProtoKey();
                        if (!wkey) wkey = L"(null)";

                        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wkey, -1, NULL, 0, NULL, NULL);
                        string keyUtf8(size_needed > 0 ? size_needed : 1, 0);
                        if (size_needed > 0)
                            WideCharToMultiByte(CP_UTF8, 0, wkey, -1, &keyUtf8[0], size_needed, NULL, NULL);

                        string label = "Image[" + to_string(i) + "] : " + keyUtf8;

                        if (ImGui::Selectable(label.c_str()))
                        {
                            m_pGameObject = pImg;
                            m_SelectedFontTag.clear();
                        }
                    }

                    if (!imgs.empty())
                    {
                        ImGui::TreePop();
                        continue;
                    }

                    const auto& BanChoimgs = pUi->Get_BanchoImage();

                    for (size_t i = 0; i < BanChoimgs.size(); ++i)
                    {
                        CUI_BanchoImage* pImg = BanChoimgs[i];
                        if (!pImg) continue;

                        const wchar_t* wkey = pImg->Get_TexProtoKey();
                        if (!wkey) wkey = L"(null)";

                        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wkey, -1, NULL, 0, NULL, NULL);
                        string keyUtf8(size_needed > 0 ? size_needed : 1, 0);
                        if (size_needed > 0)
                            WideCharToMultiByte(CP_UTF8, 0, wkey, -1, &keyUtf8[0], size_needed, NULL, NULL);

                        string label = "Image[" + to_string(i) + "] : " + keyUtf8;

                        if (ImGui::Selectable(label.c_str()))
                        {
                            m_pGameObject = pImg;
                            m_SelectedFontTag.clear();
                        }
                    }

                }
                ImGui::TreePop();
            }
        }
    }

    auto iter4 = CManagement::GetInstance()->Get_Scene()->Get_PauseLayer();

    if (!iter4)
        return;

    if (ImGui::CollapsingHeader("Pause_Layer")) {
        auto mapObj = iter4->Get_mapObj();

        CGameObject* pPicked = nullptr;

        for (auto it = mapObj->begin(); it != mapObj->end(); ++it)
        {
            wstring wstr = it->first;
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

            if (ImGui::TreeNode(strTo.c_str())) {
                CUi* pUi = dynamic_cast<CUi*>(it->second);
                if (pUi)
                {
                    const auto& imgs = pUi->Get_Images();

                    for (size_t i = 0; i < imgs.size(); ++i)
                    {
                        CUi_Image* pImg = imgs[i];
                        if (!pImg) continue;

                        const wchar_t* wkey = pImg->Get_TexProtoKey();
                        if (!wkey) wkey = L"(null)";

                        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wkey, -1, NULL, 0, NULL, NULL);
                        string keyUtf8(size_needed > 0 ? size_needed : 1, 0);
                        if (size_needed > 0)
                            WideCharToMultiByte(CP_UTF8, 0, wkey, -1, &keyUtf8[0], size_needed, NULL, NULL);

                        string label = "Image[" + to_string(i) + "] : " + keyUtf8;

                        if (ImGui::Selectable(label.c_str()))
                        {
                            m_pGameObject = pImg;
                            m_SelectedFontTag.clear();
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Font"))
    {
        const auto& fontMap = CFontMgr::GetInstance()->Get_FontMap();


        std::map<wstring, std::vector<wstring>> groupedFonts;
        for (auto& it : fontMap)
        {
            const wstring& fontTagStr = it.first;

            wstring parents = CFontMgr::GetInstance()->Find_Font_Parents(fontTagStr.c_str());

            if (parents.empty()) parents = L"Default";

            groupedFonts[parents].push_back(fontTagStr);
        }

        int idx = 0;
        for (auto& group : groupedFonts)
        {
            const wstring& parentName = group.first;

            // 부모 이름 변환
            int parent_size = WideCharToMultiByte(CP_UTF8, 0, parentName.c_str(), -1, NULL, 0, NULL, NULL);
            std::string parentUtf8(parent_size > 0 ? parent_size : 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, parentName.c_str(), -1, &parentUtf8[0], parent_size, NULL, NULL);

            // 핵심 수정 사항: ID를 명시적으로 부여하여 빈 문자열 에러 방지
            ImGui::PushID(idx++);

            // 만약 parentUtf8이 비어있을 경우를 대비해 고유 인덱스
            string treeLabel = parentUtf8 + "##" + to_string(idx);

            if (ImGui::TreeNode(treeLabel.c_str()))
            {
                for (const wstring& wtagStr : group.second)
                {
                    std::string childUtf8 = WStringToUTF8(wtagStr);

                    ImGui::PushID(childUtf8.c_str());

                    bool bSelected = (m_SelectedFontTag == wtagStr);

                    if (ImGui::Selectable(childUtf8.c_str(), bSelected))
                    {
                        m_SelectedFontTag = wtagStr;
                        m_pGameObject = nullptr;

                        m_FontPreviewPos = CUIFontMgr::GetInstance()->Get_FontList_Pos(parentName, wtagStr.c_str());
                    }

                    ImGui::PopID(); // PushID와 1:1 대응 확인
                }
                ImGui::TreePop();
            }

            ImGui::PopID(); // 부모용 PushID 대응
        }
    }
}

void CImGuiMgr::Show_Component_Prototype()
{
    if (!m_SelectedFontTag.empty())
    {
        // FontTag utf8 출력용
        std::string tagUtf8;
        {
            int size_needed = WideCharToMultiByte(
                CP_UTF8, 0,
                m_SelectedFontTag.c_str(), -1,
                NULL, 0, NULL, NULL);

            tagUtf8.assign(size_needed > 0 ? size_needed : 1, 0);
            if (size_needed > 0)
                WideCharToMultiByte(
                    CP_UTF8, 0,
                    m_SelectedFontTag.c_str(), -1,
                    &tagUtf8[0], size_needed, NULL, NULL);
        }

        ImGui::Separator();
        ImGui::Text("Font : %s", tagUtf8.c_str());

        const _tchar* wtag = m_SelectedFontTag.c_str();

        // 부모 UI 찾기
        std::wstring wsName = CFontMgr::GetInstance()->Find_Font_Parents(wtag);


        ImGui::Text("Parent : %ls", wsName.c_str());


        ImGui::Text("Pos");

        ImGui::Text("X  ");
        ImGui::SameLine(20.f, 0.f);
        bool changedX = ImGui::InputFloat("##FontPosX",&m_FontPreviewPos.x, 1.f, 10.f, "%.1f");

        ImGui::Text("Y  ");
        ImGui::SameLine(20.f, 0.f);
        bool changedY = ImGui::InputFloat("##FontPosY", &m_FontPreviewPos.y,  1.f, 10.f, "%.1f");


        if (changedX || changedY)
        {
            CUIFontMgr::GetInstance()->Set_FontList_Pos( wsName, wtag, m_FontPreviewPos);
        }

        ImGui::Separator();
        return;
    }

    if (!m_pGameObject)
    {
        return;
    }

if (m_pGameObject->Get_Dead() == TRUE)
{
    m_pGameObject = nullptr;
    return;
}

wstring wstr = m_pGameObject->Get_Name();
int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

std::string strTo(size_needed, 0);
WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
ImGui::Text(strTo.c_str());

CTransform* pTransform = static_cast<Engine::CTransform*>(m_pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));

if (!pTransform)
{
    return;
}

float fSpeed = 5.f;
float fSize = 0.1f;

if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

#pragma region Scale
    ImGui::Text("Scale");

    _vec3& vScale = pTransform->m_vScale;

    ImGui::Text("X  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat(" ", &vScale.x, fSize, fSize, "%.2f");

    ImGui::Text("Y  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("  ", &vScale.y, fSize, fSize / 5, "%.2f");

    ImGui::Text("Z  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("   ", &vScale.z, fSize, fSize / 5, "%.2f");
#pragma endregion

#pragma region Pos
    ImGui::Text("Pos");

    _vec3& vPos = pTransform->m_vInfo[INFO_POS];

    ImGui::Text("X  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("    ", &vPos.x, fSpeed, fSpeed / 25, "%.2f");

    ImGui::Text("Y  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("     ", &vPos.y, fSpeed, fSpeed / 25, "%.2f");

    ImGui::Text("Z  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("      ", &vPos.z, fSpeed, fSpeed / 25, "%.2f");

#pragma endregion

#pragma region Rotation
    ImGui::Text("Rotation");

    _vec3& vRotation = pTransform->m_vAngle;

    ImGui::Text("X  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("       ", &vRotation.x, fSpeed, fSpeed / 5, "%.2f");

    ImGui::Text("Y  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("        ", &vRotation.y, fSpeed, fSpeed / 5, "%.2f");

    ImGui::Text("Z  ");
    ImGui::SameLine(20.f, 0.f);
    ImGui::InputFloat("         ", &vRotation.z, fSpeed, fSpeed / 5, "%.2f");

#pragma endregion
}
}

// [중략] 물고기 군집 목록·속성 패널과 ImGui 종료 처리.

#endif
