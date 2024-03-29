-------------------------------------------------
-- window messages
-------------------------------------------------
WM_NULL = 0x0000;
WM_CREATE = 0x0001;
WM_DESTROY = 0x0002;
WM_MOVE = 0x0003;
WM_SIZE = 0x0005;
WM_ACTIVATE = 0x0006;
WM_SETFOCUS = 0x0007;
WM_KILLFOCUS = 0x0008;
WM_ENABLE = 0x000A;
WM_SETREDRAW = 0x000B;
WM_SETTEXT = 0x000C;
WM_GETTEXT = 0x000D;
WM_GETTEXTLENGTH = 0x000E;
WM_PAINT = 0x000F;
WM_CLOSE = 0x0010;
WM_QUERYENDSESSION = 0x0011;
WM_QUIT = 0x0012;
WM_QUERYOPEN = 0x0013;
WM_ERASEBKGND = 0x0014;
WM_SYSCOLORCHANGE = 0x0015;
WM_ENDSESSION = 0x0016;
WM_SHOWWINDOW = 0x0018;
WM_WININICHANGE = 0x001A;
WM_SETTINGCHANGE = WM_WININICHANGE;
WM_DEVMODECHANGE = 0x001B;
WM_ACTIVATEAPP = 0x001C;
WM_FONTCHANGE = 0x001D;
WM_TIMECHANGE = 0x001E;
WM_CANCELMODE = 0x001F;
WM_SETCURSOR = 0x0020;
WM_MOUSEACTIVATE = 0x0021;
WM_CHILDACTIVATE = 0x0022;
WM_QUEUESYNC = 0x0023;
WM_GETMINMAXINFO = 0x0024;
WM_PAINTICON = 0x0026;
WM_ICONERASEBKGND = 0x0027;
WM_NEXTDLGCTL = 0x0028;
WM_SPOOLERSTATUS = 0x002A;
WM_DRAWITEM = 0x002B;
WM_MEASUREITEM = 0x002C;
WM_DELETEITEM = 0x002D;
WM_VKEYTOITEM = 0x002E;
WM_CHARTOITEM = 0x002F;
WM_SETFONT = 0x0030;
WM_GETFONT = 0x0031;
WM_SETHOTKEY = 0x0032;
WM_GETHOTKEY = 0x0033;
WM_QUERYDRAGICON = 0x0037;
WM_COMPAREITEM = 0x0039;
WM_GETOBJECT = 0x003D;
WM_COMPACTING = 0x0041;
WM_COMMNOTIFY = 0x0044;
WM_WINDOWPOSCHANGING = 0x0046;
WM_WINDOWPOSCHANGED = 0x0047;
WM_POWER = 0x0048;
WM_COPYDATA = 0x004A;
WM_CANCELJOURNAL = 0x004B;
WM_NOTIFY = 0x004E;
WM_INPUTLANGCHANGEREQUEST = 0x0050;
WM_INPUTLANGCHANGE = 0x0051;
WM_TCARD = 0x0052;
WM_HELP = 0x0053;
WM_USERCHANGED = 0x0054;
WM_NOTIFYFORMAT = 0x0055;
WM_CONTEXTMENU = 0x007B;
WM_STYLECHANGING = 0x007C;
WM_STYLECHANGED = 0x007D;
WM_DISPLAYCHANGE = 0x007E;
WM_GETICON = 0x007F;
WM_SETICON = 0x0080;
WM_NCCREATE = 0x0081;
WM_NCDESTROY = 0x0082;
WM_NCCALCSIZE = 0x0083;
WM_NCHITTEST = 0x0084;
WM_NCPAINT = 0x0085;
WM_NCACTIVATE = 0x0086;
WM_GETDLGCODE = 0x0087;
WM_SYNCPAINT = 0x0088;
WM_NCMOUSEMOVE = 0x00A0;
WM_NCLBUTTONDOWN = 0x00A1;
WM_NCLBUTTONUP = 0x00A2;
WM_NCLBUTTONDBLCLK = 0x00A3;
WM_NCRBUTTONDOWN = 0x00A4;
WM_NCRBUTTONUP = 0x00A5;
WM_NCRBUTTONDBLCLK = 0x00A6;
WM_NCMBUTTONDOWN = 0x00A7;
WM_NCMBUTTONUP = 0x00A8;
WM_NCMBUTTONDBLCLK = 0x00A9;
WM_KEYFIRST = 0x0100;
WM_KEYDOWN = 0x0100;
WM_KEYUP = 0x0101;
WM_CHAR = 0x0102;
WM_DEADCHAR = 0x0103;
WM_SYSKEYDOWN = 0x0104;
WM_SYSKEYUP = 0x0105;
WM_SYSCHAR = 0x0106;
WM_SYSDEADCHAR = 0x0107;
WM_KEYLAST = 0x0108;
WM_IME_STARTCOMPOSITION = 0x010D;
WM_IME_ENDCOMPOSITION = 0x010E;
WM_IME_COMPOSITION = 0x010F;
WM_IME_KEYLAST = 0x010F;
WM_INITDIALOG = 0x0110;
WM_COMMAND = 0x0111;
WM_SYSCOMMAND = 0x0112;
WM_TIMER = 0x0113;
WM_HSCROLL = 0x0114;
WM_VSCROLL = 0x0115;
WM_INITMENU = 0x0116;
WM_INITMENUPOPUP = 0x0117;
WM_MENUSELECT = 0x011F;
WM_MENUCHAR = 0x0120;
WM_ENTERIDLE = 0x0121;
WM_MENURBUTTONUP = 0x0122;
WM_MENUDRAG = 0x0123;
WM_MENUGETOBJECT = 0x0124;
WM_UNINITMENUPOPUP = 0x0125;
WM_MENUCOMMAND = 0x0126;
WM_CTLCOLORMSGBOX = 0x0132;
WM_CTLCOLOREDIT = 0x0133;
WM_CTLCOLORLISTBOX = 0x0134;
WM_CTLCOLORBTN = 0x0135;
WM_CTLCOLORDLG = 0x0136;
WM_CTLCOLORSCROLLBAR = 0x0137;
WM_CTLCOLORSTATIC = 0x0138;
WM_MOUSEFIRST = 0x0200;
WM_MOUSEMOVE = 0x0200;
WM_LBUTTONDOWN = 0x0201;
WM_LBUTTONUP = 0x0202;
WM_LBUTTONDBLCLK = 0x0203;
WM_RBUTTONDOWN = 0x0204;
WM_RBUTTONUP = 0x0205;
WM_RBUTTONDBLCLK = 0x0206;
WM_MBUTTONDOWN = 0x0207;
WM_MBUTTONUP = 0x0208;
WM_MBUTTONDBLCLK = 0x0209;
WM_MOUSEWHEEL = 0x020A;
WM_MOUSELAST = 0x020A;
WM_MOUSELAST = 0x0209;
WM_PARENTNOTIFY = 0x0210;
WM_ENTERMENULOOP = 0x0211;
WM_EXITMENULOOP = 0x0212;
WM_NEXTMENU = 0x0213;
WM_SIZING = 0x0214;
WM_CAPTURECHANGED = 0x0215;
WM_MOVING = 0x0216;
WM_POWERBROADCAST = 0x0218;
WM_DEVICECHANGE = 0x0219;
WM_MDICREATE = 0x0220;
WM_MDIDESTROY = 0x0221;
WM_MDIACTIVATE = 0x0222;
WM_MDIRESTORE = 0x0223;
WM_MDINEXT = 0x0224;
WM_MDIMAXIMIZE = 0x0225;
WM_MDITILE = 0x0226;
WM_MDICASCADE = 0x0227;
WM_MDIICONARRANGE = 0x0228;
WM_MDIGETACTIVE = 0x0229;
WM_MDISETMENU = 0x0230;
WM_ENTERSIZEMOVE = 0x0231;
WM_EXITSIZEMOVE = 0x0232;
WM_DROPFILES = 0x0233;
WM_MDIREFRESHMENU = 0x0234;
WM_IME_SETCONTEXT = 0x0281;
WM_IME_NOTIFY = 0x0282;
WM_IME_CONTROL = 0x0283;
WM_IME_COMPOSITIONFULL = 0x0284;
WM_IME_SELECT = 0x0285;
WM_IME_CHAR = 0x0286;
WM_IME_REQUEST = 0x0288;
WM_IME_KEYDOWN = 0x0290;
WM_IME_KEYUP = 0x0291;
WM_MOUSEHOVER = 0x02A1;
WM_MOUSELEAVE = 0x02A3;
WM_CUT = 0x0300;
WM_COPY = 0x0301;
WM_PASTE = 0x0302;
WM_CLEAR = 0x0303;
WM_UNDO = 0x0304;
WM_RENDERFORMAT = 0x0305;
WM_RENDERALLFORMATS = 0x0306;
WM_DESTROYCLIPBOARD = 0x0307;
WM_DRAWCLIPBOARD = 0x0308;
WM_PAINTCLIPBOARD = 0x0309;
WM_VSCROLLCLIPBOARD = 0x030A;
WM_SIZECLIPBOARD = 0x030B;
WM_ASKCBFORMATNAME = 0x030C;
WM_CHANGECBCHAIN = 0x030D;
WM_HSCROLLCLIPBOARD = 0x030E;
WM_QUERYNEWPALETTE = 0x030F;
WM_PALETTEISCHANGING = 0x0310;
WM_PALETTECHANGED = 0x0311;
WM_HOTKEY = 0x0312;
WM_PRINT = 0x0317;
WM_PRINTCLIENT = 0x0318;
WM_HANDHELDFIRST = 0x0358;
WM_HANDHELDLAST = 0x035F;
WM_AFXFIRST = 0x0360;
WM_AFXLAST = 0x037F;
WM_PENWINFIRST = 0x0380;
WM_PENWINLAST = 0x038F;
WM_APP = 0x8000;
WM_USER = 0x0400;

-------------------------------------------------
-- virtual keycodes
-------------------------------------------------
VK_LBUTTON = 0x01;
VK_RBUTTON = 0x02;
VK_CANCEL = 0x03;
VK_MBUTTON = 0x04;
VK_BACK = 0x08;
VK_TAB = 0x09;
VK_CLEAR = 0x0C;
VK_RETURN = 0x0D;
VK_SHIFT = 0x10;
VK_CONTROL = 0x11;
VK_MENU = 0x12;
VK_PAUSE = 0x13;
VK_CAPITAL = 0x14;
VK_KANA = 0x15;
VK_HANGEUL = 0x15;
VK_HANGUL = 0x15;
VK_JUNJA = 0x17;
VK_FINAL = 0x18;
VK_HANJA = 0x19;
VK_KANJI = 0x19;
VK_ESCAPE = 0x1B;
VK_CONVERT = 0x1C;
VK_NONCONVERT = 0x1D;
VK_ACCEPT = 0x1E;
VK_MODECHANGE = 0x1F;
VK_SPACE = 0x20;
VK_PRIOR = 0x21;
VK_NEXT = 0x22;
VK_END = 0x23;
VK_HOME = 0x24;
VK_LEFT = 0x25;
VK_UP = 0x26;
VK_RIGHT = 0x27;
VK_DOWN = 0x28;
VK_SELECT = 0x29;
VK_PRINT = 0x2A;
VK_EXECUTE = 0x2B;
VK_SNAPSHOT = 0x2C;
VK_INSERT = 0x2D;
VK_DELETE = 0x2E;
VK_HELP = 0x2F;
VK_LWIN = 0x5B;
VK_RWIN = 0x5C;
VK_APPS = 0x5D;
VK_NUMPAD0 = 0x60;
VK_NUMPAD1 = 0x61;
VK_NUMPAD2 = 0x62;
VK_NUMPAD3 = 0x63;
VK_NUMPAD4 = 0x64;
VK_NUMPAD5 = 0x65;
VK_NUMPAD6 = 0x66;
VK_NUMPAD7 = 0x67;
VK_NUMPAD8 = 0x68;
VK_NUMPAD9 = 0x69;
VK_MULTIPLY = 0x6A;
VK_ADD = 0x6B;
VK_SEPARATOR = 0x6C;
VK_SUBTRACT = 0x6D;
VK_DECIMAL = 0x6E;
VK_DIVIDE = 0x6F;
VK_F1 = 0x70;
VK_F2 = 0x71;
VK_F3 = 0x72;
VK_F4 = 0x73;
VK_F5 = 0x74;
VK_F6 = 0x75;
VK_F7 = 0x76;
VK_F8 = 0x77;
VK_F9 = 0x78;
VK_F10 = 0x79;
VK_F11 = 0x7A;
VK_F12 = 0x7B;
VK_F13 = 0x7C;
VK_F14 = 0x7D;
VK_F15 = 0x7E;
VK_F16 = 0x7F;
VK_F17 = 0x80;
VK_F18 = 0x81;
VK_F19 = 0x82;
VK_F20 = 0x83;
VK_F21 = 0x84;
VK_F22 = 0x85;
VK_F23 = 0x86;
VK_F24 = 0x87;
VK_NUMLOCK = 0x90;
VK_SCROLL = 0x91;
VK_LSHIFT = 0xA0;
VK_RSHIFT = 0xA1;
VK_LCONTROL = 0xA2;
VK_RCONTROL = 0xA3;
VK_LMENU = 0xA4;
VK_RMENU = 0xA5;
VK_PROCESSKEY = 0xE5;
VK_ATTN = 0xF6;
VK_CRSEL = 0xF7;
VK_EXSEL = 0xF8;
VK_EREOF = 0xF9;
VK_PLAY = 0xFA;
VK_ZOOM = 0xFB;
VK_NONAME = 0xFC;
VK_PA1 = 0xFD;
VK_OEM_CLEAR = 0xFE;
VK_A = 65;
VK_B = 66;
VK_C = 67;
VK_D = 68;
VK_E = 69;
VK_F = 70;
VK_G = 71;
VK_H = 72;
VK_I = 73;
VK_J = 74;
VK_K = 75;
VK_L = 76;
VK_M = 77;
VK_N = 78;
VK_O = 79;
VK_P = 80;
VK_Q = 81;
VK_R = 82;
VK_S = 83;
VK_T = 84;
VK_U = 85;
VK_V = 86;
VK_W = 87;
VK_X = 88;
VK_Y = 89;
VK_Z = 90;
VK_0 = 48;
VK_1 = 49;
VK_2 = 50;
VK_3 = 51;
VK_4 = 52;
VK_5 = 53;
VK_6 = 54;
VK_7 = 55;
VK_8 = 56;
VK_9 = 57;

-------------------------------------------------
-- edit control styles
-------------------------------------------------

ES_LEFT = 0x0000;
ES_CENTER = 0x0001;
ES_RIGHT = 0x0002;
ES_MULTILINE = 0x0004;
ES_UPPERCASE = 0x0008;
ES_LOWERCASE = 0x0010;
ES_PASSWORD = 0x0020;
ES_AUTOVSCROLL = 0x0040;
ES_AUTOHSCROLL = 0x0080;
ES_NOHIDESEL = 0x0100;
ES_OEMCONVERT = 0x0400;
ES_READONLY = 0x0800;
ES_WANTRETURN = 0x1000;
ES_NUMBER = 0x2000;

-------------------------------------------------
-- window styles
-------------------------------------------------
WS_OVERLAPPED = 0x00000000;
WS_POPUP = 0x80000000;
WS_CHILD = 0x40000000;
WS_MINIMIZE = 0x20000000;
WS_VISIBLE = 0x10000000;
WS_DISABLED = 0x08000000;
WS_CLIPSIBLINGS = 0x04000000;
WS_CLIPCHILDREN = 0x02000000;
WS_MAXIMIZE = 0x01000000;
WS_CAPTION = 0x00C00000;
WS_BORDER = 0x00800000;
WS_DLGFRAME = 0x00400000;
WS_VSCROLL = 0x00200000;
WS_HSCROLL = 0x00100000;
WS_SYSMENU = 0x00080000;
WS_THICKFRAME = 0x00040000;
WS_GROUP = 0x00020000;
WS_TABSTOP = 0x00010000;
WS_MINIMIZEBOX = 0x00020000;
WS_MAXIMIZEBOX = 0x00010000;

-------------------------------------------------
-- window ex styles
-------------------------------------------------
WS_EX_DLGMODALFRAME = 0x00000001;
WS_EX_NOPARENTNOTIFY = 0x00000004;
WS_EX_TOPMOST = 0x00000008;
WS_EX_ACCEPTFILES = 0x00000010;
WS_EX_TRANSPARENT = 0x00000020;
WS_EX_MDICHILD = 0x00000040;
WS_EX_TOOLWINDOW = 0x00000080;
WS_EX_WINDOWEDGE = 0x00000100;
WS_EX_CLIENTEDGE = 0x00000200;
WS_EX_CONTEXTHELP = 0x00000400;
WS_EX_RIGHT = 0x00001000;
WS_EX_LEFT = 0x00000000;
WS_EX_RTLREADING = 0x00002000;
WS_EX_LTRREADING = 0x00000000;
WS_EX_LEFTSCROLLBAR = 0x00004000;
WS_EX_RIGHTSCROLLBAR = 0x00000000;
WS_EX_CONTROLPARENT = 0x00010000;
WS_EX_STATICEDGE = 0x00020000;
WS_EX_APPWINDOW = 0x00040000;
-------------------------------------------------
-- EditBox Messages
-------------------------------------------------
EM_GETSEL = 0x00B0;
EM_SETSEL = 0x00B1;
EM_GETRECT = 0x00B2;
EM_SETRECT = 0x00B3;
EM_SETRECTNP = 0x00B4;
EM_SCROLL = 0x00B5;
EM_LINESCROLL = 0x00B6;
EM_SCROLLCARET = 0x00B7;
EM_GETMODIFY = 0x00B8;
EM_SETMODIFY = 0x00B9;
EM_GETLINECOUNT = 0x00BA;
EM_LINEINDEX = 0x00BB;
EM_SETHANDLE = 0x00BC;
EM_GETHANDLE = 0x00BD;
EM_GETTHUMB = 0x00BE;
EM_LINELENGTH = 0x00C1;
EM_REPLACESEL = 0x00C2;
EM_GETLINE = 0x00C4;
EM_LIMITTEXT = 0x00C5;
EM_CANUNDO = 0x00C6;
EM_UNDO = 0x00C7;
EM_FMTLINES = 0x00C8;
EM_LINEFROMCHAR = 0x00C9;
EM_SETTABSTOPS = 0x00CB;
EM_SETPASSWORDCHAR = 0x00CC;
EM_EMPTYUNDOBUFFER = 0x00CD;
EM_GETFIRSTVISIBLELINE = 0x00CE;
EM_SETREADONLY = 0x00CF;
EM_SETWORDBREAKPROC = 0x00D0;
EM_GETWORDBREAKPROC = 0x00D1;
EM_GETPASSWORDCHAR = 0x00D2;
EM_SETMARGINS = 0x00D3;
EM_GETMARGINS = 0x00D4;
EM_SETLIMITTEXT = EM_LIMITTEXT;
EM_GETLIMITTEXT = 0x00D5;
EM_POSFROMCHAR = 0x00D6;
EM_CHARFROMPOS = 0x00D7;
EM_SETIMESTATUS = 0x00D8;
EM_GETIMESTATUS = 0x00D9;
-------------------------------------------------
-- others
-------------------------------------------------
DEFAULT_GUI_FONT = 17;
SYSTEM_FIXED_FONT = 16;
-------------------------------------------------
-- SendInput
-------------------------------------------------
INPUT_MOUSE     = 0;
INPUT_KEYBOARD  = 1;
INPUT_HARDWARE  = 2;

KEYEVENTF_EXTENDEDKEY = 0x0001;
KEYEVENTF_KEYUP       = 0x0002;

MOUSEEVENTF_MOVE      =  0x0001;
MOUSEEVENTF_LEFTDOWN  =  0x0002;
MOUSEEVENTF_LEFTUP    =  0x0004;
MOUSEEVENTF_RIGHTDOWN =  0x0008;
MOUSEEVENTF_RIGHTUP   =  0x0010;
MOUSEEVENTF_MIDDLEDOWN = 0x0020;
MOUSEEVENTF_MIDDLEUP  =  0x0040;
MOUSEEVENTF_WHEEL     =  0x0800;
MOUSEEVENTF_ABSOLUTE  =  0x8000;

-------------------------------------------------
-- ShowWindow
-------------------------------------------------
SW_HIDE               = 0;
SW_NORMAL             = 1;
SW_SHOWMINIMIZED      = 2
SW_MAXIMIZE           = 3
SW_SHOWNOACTIVATE     = 4
SW_SHOW               = 5
SW_MINIMIZE           = 6
SW_SHOWMINNOACTIVE    = 7
SW_SHOWNA             = 8
SW_RESTORE            = 9
SW_SHOWDEFAULT        = 10
SW_FORCEMINIMIZE      = 11


