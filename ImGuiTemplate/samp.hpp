#ifndef _SAMP_LIBRARY_HPP_
#define _SAMP_LIBRARY_HPP_

#include <Windows.h>

namespace SAMP
{
	enum class sampVersion : int
	{
		unknown = 0,
		notLoaded = -1,
		R1 = 1,
		R3 = 2,
		R5 = 3,
		DL = 4,
	}; // enum class sampVersion
	unsigned long GetSAMPHandle() {
		static unsigned long samp{ 0 };
		if (!samp) {
			samp = reinterpret_cast<unsigned long>(GetModuleHandleA("samp"));
			if (!samp || samp == -1 || reinterpret_cast<HANDLE>(samp) == INVALID_HANDLE_VALUE) {
				samp = 0;
			}
		}
		return samp;
	}

	enum sampVersion GetSAMPVersion() {
		static sampVersion sampVersion = sampVersion::unknown;
		if (sampVersion != sampVersion::unknown) {
			return sampVersion;
		}
		unsigned long samp = GetSAMPHandle();
		if (!samp) {
			sampVersion = sampVersion::notLoaded;
		}
		else {
			unsigned long EntryPoint = reinterpret_cast<IMAGE_NT_HEADERS*>(samp + reinterpret_cast<IMAGE_DOS_HEADER*>(samp)->e_lfanew)->OptionalHeader.AddressOfEntryPoint;
			switch (EntryPoint) {
			case (0x31DF13): {
				sampVersion = sampVersion::R1;
				break;
			}
			case (0xCC4D0): {
				sampVersion = sampVersion::R3;
				break;
			}
			case (0xCBC90): {
				sampVersion = sampVersion::R5;
				break;
			}
			case (0xFDB60): {
				sampVersion = sampVersion::DL;
				break;
			}
			default: {
				sampVersion = sampVersion::unknown;
				break;
			}
			}
		}
		return sampVersion;
	}

	bool IsSAMPInitialized() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0F8) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8DC) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB94) != nullptr;
		else if (GetSAMPVersion() == sampVersion::DL)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x2ACA24) != nullptr;
		return false;
	}

	int GetFontSize() {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xB3C60)();
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5B20)();
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5290)();
		else if (GetSAMPVersion() == sampVersion::DL)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC6950)();
		return 0;
	}

	void NopDeathWindowDraw() {
		if (GetSAMPVersion() == sampVersion::R1)
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x714A0), 0x90, 5);
		else if (GetSAMPVersion() == sampVersion::R3)
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x75390), 0x90, 5);
		else if (GetSAMPVersion() == sampVersion::R5)
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x75A9F), 0x90, 5);
		else if (GetSAMPVersion() == sampVersion::DL)
			APatch::memory_fill((void*)(GetSAMPHandle() + 0x75520), 0x90, 5);
	}

	CDeathWindow* RefDeathWindow() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<CDeathWindow**>(GetSAMPHandle() + 0x21A0F8);
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<CDeathWindow**>(GetSAMPHandle() + 0x26E8DC);
		else if (GetSAMPVersion() == sampVersion::R5)  
			return *reinterpret_cast<CDeathWindow**>(GetSAMPHandle() + 0x26EB88);
		else if (GetSAMPVersion() == sampVersion::DL)
			return *reinterpret_cast<CDeathWindow**>(GetSAMPHandle() + 0x2ACA18);
		return nullptr;
	}

	void* RefGame() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A10C);
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8F4);
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EBAC);
		else if (GetSAMPVersion() == sampVersion::DL)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x2ACA3C);
		return nullptr;
	}

	bool ToggleCursor(bool state) {
		int SetCursorModeAddr{};
		int LockCamAddr{};
		if (GetSAMPVersion() == sampVersion::R1) {
			LockCamAddr = { 0x9BC10 };
			SetCursorModeAddr = { 0x9BD30 };
		}
		else if (GetSAMPVersion() == sampVersion::R3) {
			LockCamAddr = { 0x9FEC0 };
			SetCursorModeAddr = { 0x9FFE0 };
		}
		else if (GetSAMPVersion() == sampVersion::R5) {
			LockCamAddr = { 0xA05D0 };
			SetCursorModeAddr = { 0xA06F0 };
		}
		else if (GetSAMPVersion() == sampVersion::DL) {
			LockCamAddr = { 0xA0410 };
			SetCursorModeAddr = { 0xA0430 };
		}

		reinterpret_cast<void(__thiscall*)(void*, int, bool)>(GetSAMPHandle() + SetCursorModeAddr)(RefGame(), state ? 3 : 0, !state);

		if (!state)
			reinterpret_cast<void(__thiscall*)(void*)>(GetSAMPHandle() + LockCamAddr)(RefGame());
		return state;
	}

	const char* CDeathWindow__GetSpriteId(char nId) {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<const char* (__thiscall*)(CDeathWindow*, char)>(GetSAMPHandle() + 0x661B0)(RefDeathWindow(), nId);
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<const char* (__thiscall*)(CDeathWindow*, char)>(GetSAMPHandle() + 0x696E0)(RefDeathWindow(), nId);
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<const char* (__thiscall*)(CDeathWindow*, char)>(GetSAMPHandle() + 0x69E50)(RefDeathWindow(), nId);
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<const char* (__thiscall*)(CDeathWindow*, char)>(GetSAMPHandle() + 0x69890)(RefDeathWindow(), nId);
		return "A";
	}

}; // namespace SAMP

#endif // !_SAMP_LIBRARY_HPP_