#include "main.h"

#pragma execution_character_set("utf-8")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void embraceTheDarkness();

HRESULT WndProc(const decltype(WndProcHook)& hook, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYUP) {
        if (wParam == VK_F2) {
            ImGuiMenuState = !ImGuiMenuState;
        }
    }

    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
        return 1;
    }
    hook.get_trampoline()(hwnd, uMsg, wParam, lParam);
}

std::optional<HRESULT> D3D9Present(const decltype(PresentHook)& hook, IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
    static bool ImGuiInit{};
    if (!ImGuiInit) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
        ImGui_ImplDX9_Init(pDevice);
        io = ImGui::GetIO();
        ImGui::GetIO().IniFilename = nullptr;
        io.Fonts->AddFontFromFileTTF(std::string(getenv("WINDIR") + std::string("\\Fonts\\Trebucbd.ttf")).c_str(), 14.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
#pragma warning(push)
#pragma warning(disable: 4996)
        std::string font{ getenv("WINDIR") }; font += "\\Fonts\\" + Config::Get()->FONTNAME + ".ttf";
#pragma warning(pop)
        standartFont = io.Fonts->AddFontFromFileTTF(font.c_str(), Config::Get()->FONTSIZE, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        gtaweap3 = io.Fonts->AddFontFromMemoryCompressedBase85TTF(iconsFont, Config::Get()->FONTSIZE + 7.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        auto latest_wndproc_ptr = GetWindowLongPtrW(**reinterpret_cast<HWND**>(0xC17054), GWLP_WNDPROC);
        WndProcHook.set_dest(latest_wndproc_ptr);
        WndProcHook.set_cb(&WndProc);
        WndProcHook.install();

        embraceTheDarkness();

        ImGuiInit = { true };
    }

    if (changeFont) {
        standartFont = io.Fonts->AddFontFromFileTTF(newFontName.c_str(), Config::Get()->FONTSIZE, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        gtaweap3 = io.Fonts->AddFontFromMemoryCompressedBase85TTF(iconsFont, Config::Get()->FONTSIZE + 7.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        io.Fonts->Build();
        ImGui_ImplDX9_InvalidateDeviceObjects();
        changeFont = { false };
    }
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    auto dl = ImGui::GetBackgroundDrawList();

    CVector screenSize = getScreenSize();

    if (SAMP::IsSAMPInitialized()) {
        if (ImGuiMenuState) {
            ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.f, screenSize.y / 2.f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(330.f, 130.f), ImGuiCond_FirstUseEver);
            if (ImGui::Begin(u8"ImGuiDeathWindow Settings", &ImGuiMenuState, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                static const char* currentItem = Config::Get()->FONTNAME.c_str();

                if (!isCursor)
                    isCursor = SAMP::ToggleCursor(true);

                ImGui::PushItemWidth(150.f);
                ImGui::InputInt(u8"Выберите размер шрифта", &Config::Get()->FONTSIZE);

                if (ImGui::BeginCombo(u8"Выберите шрифт", currentItem)) {
                    for (int i{}; i < files.size(); i++) {
                        bool is_selected = (currentItem == files[i].c_str());
                        if (ImGui::Selectable(files[i].c_str(), is_selected))
                            currentItem = files[i].c_str();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::RadioButton(u8"Стиль 1", &Config::Get()->BORDERSTYLE, 0);
                ImGui::SameLine();
                ImGui::RadioButton(u8"Стиль 2", &Config::Get()->BORDERSTYLE, 1);

                if (ImGui::Button(u8"Сохранить настройки")) {
#pragma warning(push)
#pragma warning(disable: 4996)
                    newFontName = { getenv("WINDIR") }; newFontName += "\\Fonts\\" + std::string(currentItem) + ".ttf";
#pragma warning(pop)

                    Config::Get()->FONTNAME = currentItem;
                    Config::Save();
                    changeFont = { true };
                }

                ImGui::End();
            }
        }
        else {
            if (isCursor)
                isCursor = SAMP::ToggleCursor(false);
        }
    }

    

    if (pDeathWindow != nullptr) {
        if (pDeathWindow->m_bEnabled) {
            for (int i{}; i < CDeathWindow::MAX_DEATHMESSAGES; i++) {
                if (std::string(pDeathWindow->m_entry[i].m_szKiller).length() == 0 && std::string(pDeathWindow->m_entry[i].m_szVictim).length() == 0)
                    continue;
                CVector screenSize{ getScreenSize() };
                ImGui::PushFont(gtaweap3);
                switch (Config::Get()->BORDERSTYLE)
                {
                case 0:
                    drawShadowText(dl, ImVec2((screenSize.x * 0.88075f), (screenSize.y * 0.36437f) + (i * (screenSize.y * 0.028148f))), -1, SAMP::CDeathWindow__GetSpriteId(pDeathWindow->m_entry[i].m_nWeapon));
                    break;
                case 1:
                    dl->AddText(ImVec2((screenSize.x * 0.88075f), (screenSize.y * 0.36337f) + (i * (screenSize.y * 0.028148f))), 0xFF000000, "G");
                    dl->AddText(ImVec2((screenSize.x * 0.87975f), (screenSize.y * 0.36437f) + (i * (screenSize.y * 0.028148f))), -1, SAMP::CDeathWindow__GetSpriteId(pDeathWindow->m_entry[i].m_nWeapon));
                    break;
                }
                ImGui::PopFont();

                ImGui::PushFont(standartFont);
                drawShadowText(dl, ImVec2((screenSize.x * 0.88375f) - (ImGui::CalcTextSize(pDeathWindow->m_entry[i].m_szKiller).x + 5), (screenSize.y * 0.37037f) + (i * (screenSize.y * 0.028148f))), argbToABGR(pDeathWindow->m_entry[i].m_killerColor), pDeathWindow->m_entry[i].m_szKiller);
                drawShadowText(dl, ImVec2((screenSize.x * 0.897375f), (screenSize.y * 0.37037f) + (i * (screenSize.y * 0.028148f))), argbToABGR(pDeathWindow->m_entry[i].m_victimColor), pDeathWindow->m_entry[i].m_szVictim);
                ImGui::PopFont();
            }
        }
    }
    

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    return std::nullopt;
}

std::optional<HRESULT> D3D9Lost(const decltype(ResetHook)& hook, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return std::nullopt;
}

void D3D9Reset(const decltype(ResetHook)& hook, HRESULT& return_value, IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters) {

}

void setD3D9Hooks() {
    DWORD pDevice = *reinterpret_cast<DWORD*>(0xC97C28);
    void** vTable = *reinterpret_cast<void***>(pDevice);

    PresentHook.set_dest(vTable[17]);
    PresentHook.before.connect(&D3D9Present);
    PresentHook.install();

    ResetHook.set_dest(vTable[16]);
    ResetHook.before.connect(&D3D9Lost);
    ResetHook.after.connect(&D3D9Reset);
    ResetHook.install();
}

void CTimer__Update(const decltype(CTimerHook)& hook) {
    static bool sampInit{};
    static bool init{};

    if (!sampInit && SAMP::IsSAMPInitialized()) {

        SAMP::NopDeathWindowDraw();
        pDeathWindow = SAMP::RefDeathWindow();
        sampInit = { true };
    }

    if (!init) {
        std::string path = { getenv("WINDIR") }; path += "\\Fonts\\";
        std::string ext = { ".ttf" };
        int i{};
        for (auto& p : std::filesystem::recursive_directory_iterator(path))
        {
            if (p.path().extension() == ext) {
                files.push_back(p.path().stem().string().c_str());
            }
        }

        setD3D9Hooks();
        init = { true };
    }

    hook.get_trampoline()();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Config::Load();
        
        CTimerHook.set_dest(0x561B10);
        CTimerHook.set_cb(&CTimer__Update);
        CTimerHook.install();
        break;
    case DLL_PROCESS_DETACH:
        CTimerHook.remove();
        break;
    }
    return TRUE;
}

void embraceTheDarkness()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}