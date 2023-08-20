#ifndef _MAIN_H_
#define _MAIN_H_

#include <d3d9.h>
#include <Windows.h>
#include <memory>
#include <kthook/kthook.hpp>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "APatch/APatch.h"
#include "CDeathWindow.h"
#include "samp.hpp"
#include "gtaweap3.h"
#include "CVector.h"
#include "Config.h"
#include <vector>

using CTimer__UpdateSignature = void(__cdecl*)();
using DeathWindow__DrawSignature = void(__thiscall*)(CDeathWindow*, void*);
using GUILostSignature = void(__cdecl*)();
using PresentSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
using ResetSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using WndProcSignature = HRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);

kthook::kthook_simple<CTimer__UpdateSignature> CTimerHook{};
kthook::kthook_simple<DeathWindow__DrawSignature> DeathWindowHook{};
kthook::kthook_simple<GUILostSignature> GUILostHook{};
kthook::kthook_signal<PresentSignature> PresentHook{};
kthook::kthook_signal<ResetSignature> ResetHook{};
kthook::kthook_simple<WndProcSignature> WndProcHook{};

CDeathWindow* pDeathWindow;

ImFont* standartFont;
ImFont* gtaweap3;
ImGuiIO io;
bool ImGuiMenuState{};
int FontsComboItem{};
bool changeFont{};
std::string newFontName{};
std::vector<std::string> files;

bool isCursor{};

CVector getScreenSize() {
    CVector size;
    size.x = APatch::read_memory<int>((void*)(0xC17044));
    size.y = APatch::read_memory<int>((void*)(0xC17048));

    return size;
}

int argbToABGR(int argbColor) {
    int r = (argbColor >> 16) & 0xFF;
    int b = argbColor & 0xFF;
    return (argbColor & 0xFF00FF00) | (b << 16) | r;
}

void drawShadowText(ImDrawList* dl, ImVec2 position, ImU32 nColor, std::string szText) {
    dl->AddText(ImVec2(position.x - 1.f, position.y - 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x, position.y - 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x - 1.f, position.y), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x + 1.f, position.y), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x, position.y + 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x + 1.f, position.y - 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x - 1.f, position.y + 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x + 1.f, position.y + 1.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x + 1.f, position.y + 2.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x, position.y + 2.f), 0xFF000000, szText.c_str());
    dl->AddText(ImVec2(position.x - 1.f, position.y + 2.f), 0xFF000000, szText.c_str());
    dl->AddText(position, nColor, szText.c_str());
}

#endif // !_MAIN_H_