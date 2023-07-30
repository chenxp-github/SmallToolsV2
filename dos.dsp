# Microsoft Developer Studio Project File - Name="dos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dos - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dos.mak" CFG="dos - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dos - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dos - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dos - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "common" /I "platform" /I "lualib" /I "ximage" /I "resource_manager" /I "messagepeer" /I "lua" /I "websocket" /I "win32" /I "x-file-server" /I "remote-desktop" /I "remote-desktop\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "dos - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "common" /I "platform" /I "lualib" /I "ximage" /I "resource_manager" /I "messagepeer" /I "lua" /I "websocket" /I "win32" /I "x-file-server" /I "remote-desktop" /I "remote-desktop\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "dos - Win32 Release"
# Name "dos - Win32 Debug"
# Begin Source File

SOURCE=.\websocket\bigendian.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\bigendian.h
# End Source File
# Begin Source File

SOURCE=.\common\callback.cpp
# End Source File
# Begin Source File

SOURCE=.\common\callback.h
# End Source File
# Begin Source File

SOURCE=.\common\callbackmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\callbackmap.h
# End Source File
# Begin Source File

SOURCE=.\common\closure.cpp
# End Source File
# Begin Source File

SOURCE=.\common\closure.h
# End Source File
# Begin Source File

SOURCE=.\common\closurelist.cpp
# End Source File
# Begin Source File

SOURCE=.\common\closurelist.h
# End Source File
# Begin Source File

SOURCE=.\common\cmdentry.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdentry.h
# End Source File
# Begin Source File

SOURCE=.\common\combfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\combfile.h
# End Source File
# Begin Source File

SOURCE=.\common\commandline.cpp
# End Source File
# Begin Source File

SOURCE=.\common\commandline.h
# End Source File
# Begin Source File

SOURCE=.\common\common_time.cpp
# End Source File
# Begin Source File

SOURCE=.\common\common_time.h
# End Source File
# Begin Source File

SOURCE=.\common\commonarray.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\platform\crt_win32.h
# End Source File
# Begin Source File

SOURCE=.\common\cruntime.c
# End Source File
# Begin Source File

SOURCE=.\common\cruntime.h
# End Source File
# Begin Source File

SOURCE=.\common\dirmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dirmgr.h
# End Source File
# Begin Source File

SOURCE=.\common\encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encoder.h
# End Source File
# Begin Source File

SOURCE=.\common\epoll.cpp
# End Source File
# Begin Source File

SOURCE=.\common\epoll.h
# End Source File
# Begin Source File

SOURCE=.\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\file.h
# End Source File
# Begin Source File

SOURCE=.\common\filebase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filebase.h
# End Source File
# Begin Source File

SOURCE=.\common\fileinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileinfo.h
# End Source File
# Begin Source File

SOURCE=.\common\fileinfolist.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileinfolist.h
# End Source File
# Begin Source File

SOURCE=.\fileutils.cpp
# End Source File
# Begin Source File

SOURCE=.\fileutils.h
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\common\hashfile.h
# End Source File
# Begin Source File

SOURCE=.\websocket\httpheader.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\httpheader.h
# End Source File
# Begin Source File

SOURCE=.\websocket\httppair.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\httppair.h
# End Source File
# Begin Source File

SOURCE=.\common\icmp_socket.cpp
# End Source File
# Begin Source File

SOURCE=.\common\icmp_socket.h
# End Source File
# Begin Source File

SOURCE=.\common\indexfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\indexfile.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_capimin.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_capistd.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_ccoefct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_ccolor.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_chuff.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_chuff.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cinit.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cmainct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cmarker.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cmaster.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_comapi.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_config.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cparam.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cphuff.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_cprepct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_csample.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_ctrans.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dapimin.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dapistd.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_datadst.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_datasrc.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dcoefct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dcolor.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dct.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_ddctmgr.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dhuff.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dhuff.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dinput.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dmainct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dmarker.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dmaster.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dmerge.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dphuff.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dpostct.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dsample.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_dtrans.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_error.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_error.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_fdctflt.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_fdctfst.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_fdctint.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_idctflt.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_idctfst.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_idctint.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_idctred.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_include.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_int.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_lib.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_memmgr.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_memnobs.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_memsys.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_morecfg.h
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_quant1.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_quant2.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_utils.c
# End Source File
# Begin Source File

SOURCE=.\ximage\jpeg_version.h
# End Source File
# Begin Source File

SOURCE=.\lua\lapi.c
# End Source File
# Begin Source File

SOURCE=.\lua\lapi.h
# End Source File
# Begin Source File

SOURCE=.\lua\lauxlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lauxlib.h
# End Source File
# Begin Source File

SOURCE=.\lua\lbaselib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lbitlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lcode.c
# End Source File
# Begin Source File

SOURCE=.\lua\lcode.h
# End Source File
# Begin Source File

SOURCE=.\lua\lcorolib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lctype.c
# End Source File
# Begin Source File

SOURCE=.\lua\lctype.h
# End Source File
# Begin Source File

SOURCE=.\lua\ldblib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldebug.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldebug.h
# End Source File
# Begin Source File

SOURCE=.\lua\ldo.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldo.h
# End Source File
# Begin Source File

SOURCE=.\lua\ldump.c
# End Source File
# Begin Source File

SOURCE=.\lua\lfunc.c
# End Source File
# Begin Source File

SOURCE=.\lua\lfunc.h
# End Source File
# Begin Source File

SOURCE=.\lua\lgc.c
# End Source File
# Begin Source File

SOURCE=.\lua\lgc.h
# End Source File
# Begin Source File

SOURCE=.\lua\linit.c
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\linkrpc_rd_types.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\linkrpc_xfs_types.h"
# End Source File
# Begin Source File

SOURCE=.\messagepeer\linkrpccommon.h
# End Source File
# Begin Source File

SOURCE=.\lua\liolib.c
# End Source File
# Begin Source File

SOURCE=.\lua\llex.c
# End Source File
# Begin Source File

SOURCE=.\lua\llex.h
# End Source File
# Begin Source File

SOURCE=.\lua\llimits.h
# End Source File
# Begin Source File

SOURCE=.\lua\lmathlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lmem.c
# End Source File
# Begin Source File

SOURCE=.\lua\lmem.h
# End Source File
# Begin Source File

SOURCE=.\lua\loadlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lobject.c
# End Source File
# Begin Source File

SOURCE=.\lua\lobject.h
# End Source File
# Begin Source File

SOURCE=.\lua\lopcodes.c
# End Source File
# Begin Source File

SOURCE=.\lua\lopcodes.h
# End Source File
# Begin Source File

SOURCE=.\lua\loslib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lparser.c
# End Source File
# Begin Source File

SOURCE=.\lua\lparser.h
# End Source File
# Begin Source File

SOURCE=.\lua\lprefix.h
# End Source File
# Begin Source File

SOURCE=.\lua\lstate.c
# End Source File
# Begin Source File

SOURCE=.\lua\lstate.h
# End Source File
# Begin Source File

SOURCE=.\lua\lstring.c
# End Source File
# Begin Source File

SOURCE=.\lua\lstring.h
# End Source File
# Begin Source File

SOURCE=.\lua\lstrlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltable.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltable.h
# End Source File
# Begin Source File

SOURCE=.\lua\ltablib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltm.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltm.h
# End Source File
# Begin Source File

SOURCE=.\lua\lua.c
# End Source File
# Begin Source File

SOURCE=.\lua\lua.h
# End Source File
# Begin Source File

SOURCE=.\win32\lua_hdc.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\lua_hdc.h
# End Source File
# Begin Source File

SOURCE=.\lua_helper.cpp
# End Source File
# Begin Source File

SOURCE=.\lua_helper.h
# End Source File
# Begin Source File

SOURCE=.\lua\luaconf.h
# End Source File
# Begin Source File

SOURCE=.\lua\lualib.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_app.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_app.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_cfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_cfunc.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_cmdentry.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_cmdentry.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_commandline.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_commandline.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_commontime.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_commontime.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_file.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_file.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_filebase.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_filebase.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_filemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_filemanager.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_icmpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_icmpsocket.h
# End Source File
# Begin Source File

SOURCE=.\win32\lualib_luahdc.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\lualib_luahdc.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_mem.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_memfile.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_memfile.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_messagepeer.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_messagepeer.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_minibson.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_minibson.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_mmapfile.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_mmapfile.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_peerproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_peerproxy.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_queuefile.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_queuefile.h
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopclient.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopclient.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopclientupdater.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopclientupdater.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktoppixelbuffer.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktoppixelbuffer.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopserver.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopserver.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopsnapshottor.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopsnapshottor.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\win32\lualib_remotedesktopsnapshottor_gdi.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\win32\lualib_remotedesktopsnapshottor_gdi.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopsnapshottormanager.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\lualib_remotedesktopsnapshottormanager.h"
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_resource.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_resource.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_resourcemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_resourcemanager.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_serial.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_serial.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_serversidepeer.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_serversidepeer.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_stream.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_stream.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_tasktimer.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_tasktimer.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_tcpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_tcpsocket.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_udpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_udpsocket.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_websocketmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_websocketmessage.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_websocketserver.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_websocketserver.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xfileserver.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xfileserver.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_ximage.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_ximage.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_ximagehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_ximagehelper.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xml.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xml.h
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xmlnode.cpp
# End Source File
# Begin Source File

SOURCE=.\lualib\lualib_xmlnode.h
# End Source File
# Begin Source File

SOURCE=.\common\luavm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\luavm.h
# End Source File
# Begin Source File

SOURCE=.\lua\lundump.c
# End Source File
# Begin Source File

SOURCE=.\lua\lundump.h
# End Source File
# Begin Source File

SOURCE=.\lua\lutf8lib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lvm.c
# End Source File
# Begin Source File

SOURCE=.\lua\lvm.h
# End Source File
# Begin Source File

SOURCE=.\lua\lzio.c
# End Source File
# Begin Source File

SOURCE=.\lua\lzio.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mem.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mem.h
# End Source File
# Begin Source File

SOURCE=.\common\mem_tool.c
# End Source File
# Begin Source File

SOURCE=.\common\mem_tool.h
# End Source File
# Begin Source File

SOURCE=.\platform\memdc.cpp
# End Source File
# Begin Source File

SOURCE=.\platform\memdc.h
# End Source File
# Begin Source File

SOURCE=.\common\memfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memfile.h
# End Source File
# Begin Source File

SOURCE=.\common\memstk.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memstk.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\messagepeer.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\messagepeer.h
# End Source File
# Begin Source File

SOURCE=.\common\minibson.cpp
# End Source File
# Begin Source File

SOURCE=.\common\minibson.h
# End Source File
# Begin Source File

SOURCE=.\common\minitask.cpp
# End Source File
# Begin Source File

SOURCE=.\common\minitask.h
# End Source File
# Begin Source File

SOURCE=.\common\misc.c
# End Source File
# Begin Source File

SOURCE=.\common\misc.h
# End Source File
# Begin Source File

SOURCE=.\common\mmapfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mmapfile.h
# End Source File
# Begin Source File

SOURCE=.\common\mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mutex.h
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_get_next_screen.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_get_next_screen.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_login.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_login.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_send_input.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\param_rd_send_input.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_change_current_directory.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_change_current_directory.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_close_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_close_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_create_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_create_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_delete_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_delete_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_delete_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_delete_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_get_info.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_get_info.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_list_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_list_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_open_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_open_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_read_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_read_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_write_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\param_xfs_write_file.h"
# End Source File
# Begin Source File

SOURCE=.\common\partfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\partfile.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peercommon.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerglobals.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerglobals.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peermessage.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peermessage.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peermessagestk.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peermessagestk.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxy.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxymanager.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxymanager.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxystk.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerproxystk.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerservicebase.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\peerservicebase.h
# End Source File
# Begin Source File

SOURCE=.\ximage\png_conf.h
# End Source File
# Begin Source File

SOURCE=.\ximage\png_error.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_gccrd.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_get.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_lib.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_lib.h
# End Source File
# Begin Source File

SOURCE=.\ximage\png_mem.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_pread.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_read.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_rio.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_rtran.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_rutil.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_set.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_trans.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_vcrd.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_wio.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_write.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_wtran.c
# End Source File
# Begin Source File

SOURCE=.\ximage\png_wutil.c
# End Source File
# Begin Source File

SOURCE=.\messagepeer\ptrhashtable.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\ptrhashtable.h
# End Source File
# Begin Source File

SOURCE=.\common\ptrstk.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ptrstk.h
# End Source File
# Begin Source File

SOURCE=.\common\queuefile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\queuefile.h
# End Source File
# Begin Source File

SOURCE=.\common\raw_weak_pointer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\raw_weak_pointer.h
# End Source File
# Begin Source File

SOURCE=.\common\rect.cpp
# End Source File
# Begin Source File

SOURCE=.\common\rect.h
# End Source File
# Begin Source File

SOURCE=.\common\refcnt.h
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_client.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_client.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_client_messages.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_client_updater.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_client_updater.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_common.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_input.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_input.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_keybd_input.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_keybd_input.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_mouse_input.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_mouse_input.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_pixel_buffer.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_pixel_buffer.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_pixel_buffer_list.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_pixel_buffer_list.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_server.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_server.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_server_messages.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_session.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_session.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_session_manager.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_session_manager.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\win32\remote_desktop_snapshottor_gdi.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\win32\remote_desktop_snapshottor_gdi.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor_list.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor_list.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor_manager.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\remote_desktop_snapshottor_manager.h"
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resourcehashmap.cpp
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resourcehashmap.h
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resourcemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\resource_manager\resourcemanager.h
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_get_monitor_list.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_get_monitor_list.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_get_next_screen.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_get_next_screen.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_login.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_login.h"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_send_input.cpp"
# End Source File
# Begin Source File

SOURCE=".\remote-desktop\ret_val_rd_send_input.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_change_current_directory.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_change_current_directory.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_close_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_close_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_create_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_create_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_delete_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_delete_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_delete_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_delete_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_get_current_directory.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_get_current_directory.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_get_info.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_get_info.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_list_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_list_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_open_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_open_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_read_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_read_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_write_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\ret_val_xfs_write_file.h"
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_call_context.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_call_context.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_param_base.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_param_base.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_service_base.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\rpc_service_base.h
# End Source File
# Begin Source File

SOURCE=.\common\serial.cpp
# End Source File
# Begin Source File

SOURCE=.\common\serial.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\serversidepeer.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\serversidepeer.h
# End Source File
# Begin Source File

SOURCE=.\websocket\sha1.c
# End Source File
# Begin Source File

SOURCE=.\websocket\sha1.h
# End Source File
# Begin Source File

SOURCE=.\websocket\sha1calculator.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\sha1calculator.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\socketreaderwriter.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\socketreaderwriter.h
# End Source File
# Begin Source File

SOURCE=.\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stream.h
# End Source File
# Begin Source File

SOURCE=.\common\sys_log.c
# End Source File
# Begin Source File

SOURCE=.\common\sys_log.h
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_list_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_list_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_read_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_read_file.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_sync_folder.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_sync_folder.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_write_file.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\task_xserver_write_file.h"
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpc.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpc.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpcreader.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpcreader.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpcwriter.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasklinkrpcwriter.h
# End Source File
# Begin Source File

SOURCE=.\common\taskmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\taskmgr.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\taskpeerclient.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\taskpeerclient.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\taskpeerserver.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\taskpeerserver.h
# End Source File
# Begin Source File

SOURCE=.\common\taskrunner.cpp
# End Source File
# Begin Source File

SOURCE=.\common\taskrunner.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasktcpacceptor.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasktcpacceptor.h
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasktcpconnector.cpp
# End Source File
# Begin Source File

SOURCE=.\messagepeer\tasktcpconnector.h
# End Source File
# Begin Source File

SOURCE=.\common\tasktimer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tasktimer.h
# End Source File
# Begin Source File

SOURCE=.\websocket\taskwebsocketserver.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\taskwebsocketserver.h
# End Source File
# Begin Source File

SOURCE=.\common\tcp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tcp.h
# End Source File
# Begin Source File

SOURCE=.\common\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\common\thread.h
# End Source File
# Begin Source File

SOURCE=.\common\udpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\common\udpsocket.h
# End Source File
# Begin Source File

SOURCE=.\common\userfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\userfunc.h
# End Source File
# Begin Source File

SOURCE=.\common\weak_pointer.h
# End Source File
# Begin Source File

SOURCE=.\common\weak_ref.h
# End Source File
# Begin Source File

SOURCE=.\common\weakptr.h
# End Source File
# Begin Source File

SOURCE=.\websocket\web_socket_call_context.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\web_socket_call_context.h
# End Source File
# Begin Source File

SOURCE=.\websocket\web_socket_param_base.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\web_socket_param_base.h
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketframeheader.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketframeheader.h
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketmessage.h
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketmessagequeue.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketmessagequeue.h
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketserver.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketserver.h
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketservicebase.cpp
# End Source File
# Begin Source File

SOURCE=.\websocket\websocketservicebase.h
# End Source File
# Begin Source File

SOURCE=.\ximage\wintypes.h
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_client.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_client.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_client_messages.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_info.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_info.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_info_list.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_info_list.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_server.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_server.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_session.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_session.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_session_map.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_file_session_map.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_folder_differ.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_folder_differ.h"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_opened_file_map.cpp"
# End Source File
# Begin Source File

SOURCE=".\x-file-server\x_opened_file_map.h"
# End Source File
# Begin Source File

SOURCE=.\ximage\ximage.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximage.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagebmp.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagebmp.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagegif.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagegif.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagehelper.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximageico.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximageico.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagejpg.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagejpg.h
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagepng.cpp
# End Source File
# Begin Source File

SOURCE=.\ximage\ximagepng.h
# End Source File
# Begin Source File

SOURCE=.\common\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xml.h
# End Source File
# Begin Source File

SOURCE=.\common\xmlreader.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xmlreader.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_adler32.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_compress.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_crc32.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_crc32.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_deflate.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_deflate.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_gzio.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_infback.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inffast.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inffast.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inffixed.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inflate.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inflate.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inftrees.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_inftrees.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_trees.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_trees.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_uncompr.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_zconf.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_zconf.in.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_zlib.h
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_zutil.c
# End Source File
# Begin Source File

SOURCE=.\ximage\zlib_zutil.h
# End Source File
# End Target
# End Project
