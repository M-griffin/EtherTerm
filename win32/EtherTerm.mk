##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=EtherTerm
ConfigurationName      :=Debug
WorkspacePath          :=./EtherTerm/win32
ProjectPath            :=./EtherTerm/win32
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Blue
Date                   :=10/12/2020
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/MinGW-5.1.0/mingw32/bin/g++.exe
SharedObjectLinkerName :=C:/MinGW-5.1.0/mingw32/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="EtherTerm.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW-5.1.0/mingw32/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch).. $(IncludeSwitch)../src $(IncludeSwitch)F:/Users/Blue/Downloads/SDL2-2.0.5/i686-w64-mingw32/include/SDL2 $(IncludeSwitch)F:/Users/Blue/Downloads/SDL2-2.0.5/i686-w64-mingw32/include $(IncludeSwitch)C:/OpenSSL-Win32/include $(IncludeSwitch)F:/Users/Blue/Downloads/libssh-0.7.2/include $(IncludeSwitch)C:/Users/Blue/Documents/GitHub/yaml-cpp/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)mingw32 $(LibrarySwitch)SDL2main $(LibrarySwitch)SDL2 $(LibrarySwitch)ws2_32 $(LibrarySwitch)wsock32 $(LibrarySwitch)ssleay32 $(LibrarySwitch)eay32 $(LibrarySwitch)ssh $(LibrarySwitch)iphlpapi $(LibrarySwitch)yaml-cpp 
ArLibs                 :=  "mingw32" "SDL2main" "SDL2" "libws2_32.a" "libwsock32.a" "ssleay32.a" "libeay32.a" "libssh" "libiphlpapi.a" "yaml-cpp.dll" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)mingw32/lib $(LibraryPathSwitch)C:/libz-1.2.7-1-mingw32-dev/lib $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/MinGW $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/ $(LibraryPathSwitch)F:/Users/Blue/Downloads/libssh-0.7.2\lib $(LibraryPathSwitch)C:/Users/Blue/Documents/GitHub/yaml-cpp/build 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-5.1.0/mingw32/bin/ar.exe rcu
CXX      := C:/MinGW-5.1.0/mingw32/bin/g++.exe
CC       := C:/MinGW-5.1.0/mingw32/bin/gcc.exe
CXXFLAGS :=  -g -O0 -pedantic -W -std=c++11 -Wall  $(Preprocessors)
CFLAGS   :=  -g -O0 -pedantic -W -std=c++11 -Wall  $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-5.1.0/mingw32/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_io_service.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_window_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_telnet_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_surface_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_session_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_screen_buffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_irc_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_font_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_sequence_parser.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_protocol.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_dialing_manager.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_src_font_set_dao.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_input_handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_link_list.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_menu_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_menu_config.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_menu_function.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_menu_io.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	'C:\Program Files (x86)\GnuWin32\bin\sed.exe' -i 's+../src/+../src/+' C:\Users\Blue\Desktop\EtherTerm\win32\EtherTerm.mk
	
	'C:\Program Files (x86)\GnuWin32\bin\sed.exe' -i 's+./EtherTerm/win32+./EtherTerm/win32+' C:\Users\Blue\Desktop\EtherTerm\win32\EtherTerm.mk
	del /Q sed*
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetUDP.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//SDL2_net/SDLnetUDP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix): ../src/SDL2_net/SDLnetUDP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetUDP.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetUDP.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetUDP.cpp

$(IntermediateDirectory)/up_src_io_service.cpp$(ObjectSuffix): ../src/io_service.cpp $(IntermediateDirectory)/up_src_io_service.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//io_service.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_io_service.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_io_service.cpp$(DependSuffix): ../src/io_service.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_io_service.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_io_service.cpp$(DependSuffix) -MM ../src/io_service.cpp

$(IntermediateDirectory)/up_src_io_service.cpp$(PreprocessSuffix): ../src/io_service.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_io_service.cpp$(PreprocessSuffix) ../src/io_service.cpp

$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix): ../src/socket_handler.cpp $(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//socket_handler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix): ../src/socket_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix) -MM ../src/socket_handler.cpp

$(IntermediateDirectory)/up_src_socket_handler.cpp$(PreprocessSuffix): ../src/socket_handler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_handler.cpp$(PreprocessSuffix) ../src/socket_handler.cpp

$(IntermediateDirectory)/up_src_window_manager.cpp$(ObjectSuffix): ../src/window_manager.cpp $(IntermediateDirectory)/up_src_window_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//window_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_window_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_window_manager.cpp$(DependSuffix): ../src/window_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_window_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_window_manager.cpp$(DependSuffix) -MM ../src/window_manager.cpp

$(IntermediateDirectory)/up_src_window_manager.cpp$(PreprocessSuffix): ../src/window_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_window_manager.cpp$(PreprocessSuffix) ../src/window_manager.cpp

$(IntermediateDirectory)/up_src_telnet_manager.cpp$(ObjectSuffix): ../src/telnet_manager.cpp $(IntermediateDirectory)/up_src_telnet_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//telnet_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_telnet_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_telnet_manager.cpp$(DependSuffix): ../src/telnet_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_telnet_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_telnet_manager.cpp$(DependSuffix) -MM ../src/telnet_manager.cpp

$(IntermediateDirectory)/up_src_telnet_manager.cpp$(PreprocessSuffix): ../src/telnet_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_telnet_manager.cpp$(PreprocessSuffix) ../src/telnet_manager.cpp

$(IntermediateDirectory)/up_src_surface_manager.cpp$(ObjectSuffix): ../src/surface_manager.cpp $(IntermediateDirectory)/up_src_surface_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//surface_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_surface_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_surface_manager.cpp$(DependSuffix): ../src/surface_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_surface_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_surface_manager.cpp$(DependSuffix) -MM ../src/surface_manager.cpp

$(IntermediateDirectory)/up_src_surface_manager.cpp$(PreprocessSuffix): ../src/surface_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_surface_manager.cpp$(PreprocessSuffix) ../src/surface_manager.cpp

$(IntermediateDirectory)/up_src_session_manager.cpp$(ObjectSuffix): ../src/session_manager.cpp $(IntermediateDirectory)/up_src_session_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//session_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_session_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_session_manager.cpp$(DependSuffix): ../src/session_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_session_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_session_manager.cpp$(DependSuffix) -MM ../src/session_manager.cpp

$(IntermediateDirectory)/up_src_session_manager.cpp$(PreprocessSuffix): ../src/session_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_session_manager.cpp$(PreprocessSuffix) ../src/session_manager.cpp

$(IntermediateDirectory)/up_src_screen_buffer.cpp$(ObjectSuffix): ../src/screen_buffer.cpp $(IntermediateDirectory)/up_src_screen_buffer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//screen_buffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_screen_buffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_screen_buffer.cpp$(DependSuffix): ../src/screen_buffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_screen_buffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_screen_buffer.cpp$(DependSuffix) -MM ../src/screen_buffer.cpp

$(IntermediateDirectory)/up_src_screen_buffer.cpp$(PreprocessSuffix): ../src/screen_buffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_screen_buffer.cpp$(PreprocessSuffix) ../src/screen_buffer.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetTCP.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//SDL2_net/SDLnetTCP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix): ../src/SDL2_net/SDLnetTCP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetTCP.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetTCP.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetTCP.cpp

$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix): ../src/session_io.cpp $(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//session_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix): ../src/session_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix) -MM ../src/session_io.cpp

$(IntermediateDirectory)/up_src_session_io.cpp$(PreprocessSuffix): ../src/session_io.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_session_io.cpp$(PreprocessSuffix) ../src/session_io.cpp

$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix): ../src/socket_telnet.cpp $(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//socket_telnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix): ../src/socket_telnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix) -MM ../src/socket_telnet.cpp

$(IntermediateDirectory)/up_src_socket_telnet.cpp$(PreprocessSuffix): ../src/socket_telnet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_telnet.cpp$(PreprocessSuffix) ../src/socket_telnet.cpp

$(IntermediateDirectory)/up_src_irc_manager.cpp$(ObjectSuffix): ../src/irc_manager.cpp $(IntermediateDirectory)/up_src_irc_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//irc_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_irc_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_irc_manager.cpp$(DependSuffix): ../src/irc_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_irc_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_irc_manager.cpp$(DependSuffix) -MM ../src/irc_manager.cpp

$(IntermediateDirectory)/up_src_irc_manager.cpp$(PreprocessSuffix): ../src/irc_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_irc_manager.cpp$(PreprocessSuffix) ../src/irc_manager.cpp

$(IntermediateDirectory)/up_src_font_manager.cpp$(ObjectSuffix): ../src/font_manager.cpp $(IntermediateDirectory)/up_src_font_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//font_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_font_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_font_manager.cpp$(DependSuffix): ../src/font_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_font_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_font_manager.cpp$(DependSuffix) -MM ../src/font_manager.cpp

$(IntermediateDirectory)/up_src_font_manager.cpp$(PreprocessSuffix): ../src/font_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_font_manager.cpp$(PreprocessSuffix) ../src/font_manager.cpp

$(IntermediateDirectory)/up_src_sequence_parser.cpp$(ObjectSuffix): ../src/sequence_parser.cpp $(IntermediateDirectory)/up_src_sequence_parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//sequence_parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_sequence_parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_sequence_parser.cpp$(DependSuffix): ../src/sequence_parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_sequence_parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_sequence_parser.cpp$(DependSuffix) -MM ../src/sequence_parser.cpp

$(IntermediateDirectory)/up_src_sequence_parser.cpp$(PreprocessSuffix): ../src/sequence_parser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_sequence_parser.cpp$(PreprocessSuffix) ../src/sequence_parser.cpp

$(IntermediateDirectory)/up_src_protocol.cpp$(ObjectSuffix): ../src/protocol.cpp $(IntermediateDirectory)/up_src_protocol.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//protocol.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_protocol.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_protocol.cpp$(DependSuffix): ../src/protocol.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_protocol.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_protocol.cpp$(DependSuffix) -MM ../src/protocol.cpp

$(IntermediateDirectory)/up_src_protocol.cpp$(PreprocessSuffix): ../src/protocol.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_protocol.cpp$(PreprocessSuffix) ../src/protocol.cpp

$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix): ../src/socket_irc.cpp $(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//socket_irc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix): ../src/socket_irc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix) -MM ../src/socket_irc.cpp

$(IntermediateDirectory)/up_src_socket_irc.cpp$(PreprocessSuffix): ../src/socket_irc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_irc.cpp$(PreprocessSuffix) ../src/socket_irc.cpp

$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix): ../src/common_io.cpp $(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//common_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix): ../src/common_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix) -MM ../src/common_io.cpp

$(IntermediateDirectory)/up_src_common_io.cpp$(PreprocessSuffix): ../src/common_io.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_common_io.cpp$(PreprocessSuffix) ../src/common_io.cpp

$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(ObjectSuffix): ../src/dialing_directory_dao.cpp $(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//dialing_directory_dao.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(DependSuffix): ../src/dialing_directory_dao.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(DependSuffix) -MM ../src/dialing_directory_dao.cpp

$(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(PreprocessSuffix): ../src/dialing_directory_dao.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_dialing_directory_dao.cpp$(PreprocessSuffix) ../src/dialing_directory_dao.cpp

$(IntermediateDirectory)/up_src_renderer.cpp$(ObjectSuffix): ../src/renderer.cpp $(IntermediateDirectory)/up_src_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_renderer.cpp$(DependSuffix): ../src/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_renderer.cpp$(DependSuffix) -MM ../src/renderer.cpp

$(IntermediateDirectory)/up_src_renderer.cpp$(PreprocessSuffix): ../src/renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_renderer.cpp$(PreprocessSuffix) ../src/renderer.cpp

$(IntermediateDirectory)/up_src_dialing_manager.cpp$(ObjectSuffix): ../src/dialing_manager.cpp $(IntermediateDirectory)/up_src_dialing_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//dialing_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_dialing_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_dialing_manager.cpp$(DependSuffix): ../src/dialing_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_dialing_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_dialing_manager.cpp$(DependSuffix) -MM ../src/dialing_manager.cpp

$(IntermediateDirectory)/up_src_dialing_manager.cpp$(PreprocessSuffix): ../src/dialing_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_dialing_manager.cpp$(PreprocessSuffix) ../src/dialing_manager.cpp

$(IntermediateDirectory)/up_src_font_set_dao.cpp$(ObjectSuffix): ../src/font_set_dao.cpp $(IntermediateDirectory)/up_src_font_set_dao.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//font_set_dao.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_font_set_dao.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_font_set_dao.cpp$(DependSuffix): ../src/font_set_dao.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_font_set_dao.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_font_set_dao.cpp$(DependSuffix) -MM ../src/font_set_dao.cpp

$(IntermediateDirectory)/up_src_font_set_dao.cpp$(PreprocessSuffix): ../src/font_set_dao.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_font_set_dao.cpp$(PreprocessSuffix) ../src/font_set_dao.cpp

$(IntermediateDirectory)/up_src_input_handler.cpp$(ObjectSuffix): ../src/input_handler.cpp $(IntermediateDirectory)/up_src_input_handler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//input_handler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_input_handler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_input_handler.cpp$(DependSuffix): ../src/input_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_input_handler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_input_handler.cpp$(DependSuffix) -MM ../src/input_handler.cpp

$(IntermediateDirectory)/up_src_input_handler.cpp$(PreprocessSuffix): ../src/input_handler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_input_handler.cpp$(PreprocessSuffix) ../src/input_handler.cpp

$(IntermediateDirectory)/up_src_link_list.cpp$(ObjectSuffix): ../src/link_list.cpp $(IntermediateDirectory)/up_src_link_list.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//link_list.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_link_list.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_link_list.cpp$(DependSuffix): ../src/link_list.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_link_list.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_link_list.cpp$(DependSuffix) -MM ../src/link_list.cpp

$(IntermediateDirectory)/up_src_link_list.cpp$(PreprocessSuffix): ../src/link_list.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_link_list.cpp$(PreprocessSuffix) ../src/link_list.cpp

$(IntermediateDirectory)/up_src_main.cpp$(ObjectSuffix): ../src/main.cpp $(IntermediateDirectory)/up_src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_main.cpp$(DependSuffix): ../src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_main.cpp$(DependSuffix) -MM ../src/main.cpp

$(IntermediateDirectory)/up_src_main.cpp$(PreprocessSuffix): ../src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_main.cpp$(PreprocessSuffix) ../src/main.cpp

$(IntermediateDirectory)/up_src_menu_manager.cpp$(ObjectSuffix): ../src/menu_manager.cpp $(IntermediateDirectory)/up_src_menu_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//menu_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_menu_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_menu_manager.cpp$(DependSuffix): ../src/menu_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_menu_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_menu_manager.cpp$(DependSuffix) -MM ../src/menu_manager.cpp

$(IntermediateDirectory)/up_src_menu_manager.cpp$(PreprocessSuffix): ../src/menu_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_menu_manager.cpp$(PreprocessSuffix) ../src/menu_manager.cpp

$(IntermediateDirectory)/up_src_menu_config.cpp$(ObjectSuffix): ../src/menu_config.cpp $(IntermediateDirectory)/up_src_menu_config.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//menu_config.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_menu_config.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_menu_config.cpp$(DependSuffix): ../src/menu_config.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_menu_config.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_menu_config.cpp$(DependSuffix) -MM ../src/menu_config.cpp

$(IntermediateDirectory)/up_src_menu_config.cpp$(PreprocessSuffix): ../src/menu_config.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_menu_config.cpp$(PreprocessSuffix) ../src/menu_config.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetselect.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//SDL2_net/SDLnetselect.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix): ../src/SDL2_net/SDLnetselect.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetselect.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetselect.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetselect.cpp

$(IntermediateDirectory)/up_src_menu_function.cpp$(ObjectSuffix): ../src/menu_function.cpp $(IntermediateDirectory)/up_src_menu_function.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//menu_function.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_menu_function.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_menu_function.cpp$(DependSuffix): ../src/menu_function.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_menu_function.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_menu_function.cpp$(DependSuffix) -MM ../src/menu_function.cpp

$(IntermediateDirectory)/up_src_menu_function.cpp$(PreprocessSuffix): ../src/menu_function.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_menu_function.cpp$(PreprocessSuffix) ../src/menu_function.cpp

$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix): ../src/sequence_decoder.cpp $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//sequence_decoder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix): ../src/sequence_decoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix) -MM ../src/sequence_decoder.cpp

$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(PreprocessSuffix): ../src/sequence_decoder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(PreprocessSuffix) ../src/sequence_decoder.cpp

$(IntermediateDirectory)/up_src_menu_io.cpp$(ObjectSuffix): ../src/menu_io.cpp $(IntermediateDirectory)/up_src_menu_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//menu_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_menu_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_menu_io.cpp$(DependSuffix): ../src/menu_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_menu_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_menu_io.cpp$(DependSuffix) -MM ../src/menu_io.cpp

$(IntermediateDirectory)/up_src_menu_io.cpp$(PreprocessSuffix): ../src/menu_io.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_menu_io.cpp$(PreprocessSuffix) ../src/menu_io.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnet.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//SDL2_net/SDLnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix): ../src/SDL2_net/SDLnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnet.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnet.cpp

$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix): ../src/socket_ssh.cpp $(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "../src//socket_ssh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix): ../src/socket_ssh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix) -MM ../src/socket_ssh.cpp

$(IntermediateDirectory)/up_src_socket_ssh.cpp$(PreprocessSuffix): ../src/socket_ssh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_ssh.cpp$(PreprocessSuffix) ../src/socket_ssh.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


