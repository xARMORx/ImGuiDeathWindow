#pragma once

struct ID3DXFont;
struct ID3DXSprite;
typedef unsigned long D3DCOLOR;
typedef int           BOOL;
struct IDirect3DDevice9;

#pragma pack(push, 1)
struct CDeathWindow {
    enum { MAX_DEATHMESSAGES = 5 };

    BOOL m_bEnabled;

    struct {
        char     m_szKiller[25];
        char     m_szVictim[25];
        D3DCOLOR m_killerColor;
        D3DCOLOR m_victimColor;
        char     m_nWeapon;
    } m_entry[MAX_DEATHMESSAGES];

    int               m_nLongestNickWidth;
    int               m_position[2];
    ID3DXFont* m_pFont;
    ID3DXFont* m_pWeaponFont1;
    ID3DXFont* m_pWeaponFont2;
    ID3DXSprite* m_pSprite;
    IDirect3DDevice9* m_pDevice;
    BOOL              m_bAuxFontInitialized;
    ID3DXFont* m_pAuxFont1;
    ID3DXFont* m_pAuxFont2;
};
#pragma pack(pop)