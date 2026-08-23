#pragma once

#include <string>

// Default window/table layout, generated from %APPDATA%/Bollbok/imgui.ini.
// Loaded together with (before) the user's imgui.ini in a single
// LoadIniSettingsFromMemory() call so saved user settings override these.
inline const char* kDefaultWindowLayout = R"(

[Window][Verifikat]
Pos=0,29
Size=860,923
Collapsed=0
DockId=0x00000003,0

[Window][Konton]
Pos=862,965
Size=538,235
Collapsed=0
DockId=0x00000007,0

[Window][Rapporter]
Pos=862,29
Size=538,632
Collapsed=0
DockId=0x00000008,0

[Window][Saldo]
Pos=0,954
Size=860,246
Collapsed=0
DockId=0x00000004,0

[Window][Ett verifikat]
Pos=862,663
Size=538,300
Collapsed=0
DockId=0x00000009,0

[Table][0xAC27EA87,4]
Column 0  Weight=0.2895 Sort=0v
Column 1  Weight=0.7149
Column 2  Weight=2.0798
Column 3  Weight=0.9158

[Table][0xF014F054,5]
Column 0  Weight=0.5882 Sort=0^
Column 1  Weight=2.8792
Column 2  Weight=0.7121
Column 3  Weight=0.3638
Column 4  Weight=0.4566

[Table][0xC6138CB3,3]
RefScale=23
Column 0  Width=52
Column 1  Weight=1.0000
Column 2  Width=201

[Table][0xC4E375E2,4]
Column 0  Weight=0.6774
Column 1  Weight=1.8602
Column 2  Weight=0.7097
Column 3  Weight=0.7527

[Table][0x3319BF13,3]
RefScale=23
Column 0  Width=60
Column 1  Weight=1.0000
Column 2  Width=133

[Table][0x5279D6A9,5]
RefScale=23
Column 0  Width=43
Column 1  Weight=1.0000
Column 2  Width=141
Column 3  Width=144
Column 4  Width=140

[Table][0x81221987,5]
RefScale=23
Column 0  Weight=0.9231
Column 1  Weight=1.0769
Column 2  Width=169
Column 3  Width=136
Column 4  Width=153

[Table][0xC59A2518,4]
Column 0  Weight=0.3374 ID=0x6A0E07A6
Column 1  Weight=0.6857 Sort=0v ID=0xC762133F
Column 2  Weight=1.9755 ID=0xE8FE0AA2
Column 3  Weight=1.0014 ID=0xCF68C9A5

[Table][0x38EA14AD,5]
Column 0  Weight=1.0000 Sort=0v ID=0x6D779EBC
Column 1  Weight=1.0000 ID=0xE8FE0AA2
Column 2  Weight=1.0000 ID=0xBD1A6697
Column 3  Weight=1.0000 ID=0x090A2176
Column 4  Weight=1.0000 ID=0xA16D3999

[Table][0xD89749AF,3]
RefScale=23
Column 0  Width=54 ID=0x5B3AC8E9
Column 1  Weight=1.0000 ID=0x6D779EBC
Column 2  Width=174 ID=0x1262FB6F

)";

// Default dock node tree. Only loaded when the user's imgui.ini has no
// docking data of its own: imgui clears previously read docking data at the
// start of every settings load, so defaults and user data must never be fed
// through two separate loads, and two [Docking][Data] sections in one load
// would merge into a conflicting node tree.
inline const char* kDefaultDockingLayout = R"(
[Docking][Data]
DockSpace       ID=0x08BD597D Window=0x1BBC0F80 Pos=0,29 Size=1400,1171 Split=X
  DockNode      ID=0x00000001 Parent=0x08BD597D SizeRef=860,1665 Split=Y Selected=0xF15726DF
    DockNode    ID=0x00000003 Parent=0x00000001 SizeRef=636,1091 Selected=0xF15726DF
    DockNode    ID=0x00000004 Parent=0x00000001 SizeRef=636,291 Selected=0x5C98F6EF
  DockNode      ID=0x00000002 Parent=0x08BD597D SizeRef=1317,1665 Split=Y Selected=0x60AB0774
    DockNode    ID=0x00000006 Parent=0x00000002 SizeRef=1541,1147 Split=Y Selected=0x60AB0774
      DockNode  ID=0x00000008 Parent=0x00000006 SizeRef=1541,845 CentralNode=1 Selected=0x60AB0774
      DockNode  ID=0x00000009 Parent=0x00000006 SizeRef=1541,300 Selected=0xAB2D3A7B
    DockNode    ID=0x00000007 Parent=0x00000002 SizeRef=1541,235 Selected=0xD87CF422

)";

// Composes the ini contents to load: default layout first, then the user's
// saved settings (which override the defaults). If the user file contains its
// own docking data the baked-in dock tree is skipped.
inline std::string composeImguiIni(const std::string& user_ini) {
    std::string ini = kDefaultWindowLayout;
    const bool user_has_docking =
        user_ini.find("[Docking][Data]") != std::string::npos &&
        (user_ini.find("\nDockSpace") != std::string::npos ||
         user_ini.find("\nDockNode") != std::string::npos);
    if (!user_has_docking) {
        ini += kDefaultDockingLayout;
    }
    ini += "\n";
    ini += user_ini;
    return ini;
};
