##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=unittest
ConfigurationName      :=Debug
WorkspacePath          :=C:/Users/Blue/Desktop/EtherTerm/win32
ProjectPath            :=C:/Users/Blue/Desktop/EtherTerm/unittest
IntermediateDirectory  :=Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Blue
Date                   :=30/12/2018
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
ObjectsFileList        :="unittest.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW-5.1.0/mingw32/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch).. $(IncludeSwitch)../src $(IncludeSwitch)C:/UnitTest++-1.3/src $(IncludeSwitch)F:/Users/Blue/Downloads/SDL2-2.0.5/i686-w64-mingw32/include/SDL2 $(IncludeSwitch)F:/Users/Blue/Downloads/SDL2-2.0.5/i686-w64-mingw32/include $(IncludeSwitch)C:/OpenSSL-Win32/include $(IncludeSwitch)F:/Users/Blue/Downloads/libssh-0.7.2/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)UnitTest++ $(LibrarySwitch)mingw32 $(LibrarySwitch)SDL2main $(LibrarySwitch)SDL2 $(LibrarySwitch)ws2_32 $(LibrarySwitch)wsock32 $(LibrarySwitch)ssleay32 $(LibrarySwitch)eay32 $(LibrarySwitch)ssh $(LibrarySwitch)iphlpapi 
ArLibs                 :=  "libUnitTest++.a" "mingw32" "SDL2main" "SDL2" "libws2_32.a" "libwsock32.a" "ssleay32.a" "libeay32.a" "libssh" "libiphlpapi.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch).. $(LibraryPathSwitch)C:/UnitTest++-1.3/Debug $(LibraryPathSwitch)mingw32/lib $(LibraryPathSwitch)C:/libz-1.2.7-1-mingw32-dev/lib $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/MinGW $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/ $(LibraryPathSwitch)F:/Users/Blue/Downloads/libssh-0.7.2\lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-5.1.0/mingw32/bin/ar.exe rcu
CXX      := C:/MinGW-5.1.0/mingw32/bin/g++.exe
CC       := C:/MinGW-5.1.0/mingw32/bin/gcc.exe
CXXFLAGS :=  -g -std=c++11  $(Preprocessors)
CFLAGS   :=  -g -std=c++11  $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-5.1.0/mingw32/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) $(IntermediateDirectory)/session_io_test.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) 



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

MakeIntermediateDirs:
	@$(MakeDirCommand) "Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnet.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/SDL2_net/SDLnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix): ../src/SDL2_net/SDLnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnet.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnet.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnet.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetTCP.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/SDL2_net/SDLnetTCP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix): ../src/SDL2_net/SDLnetTCP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetTCP.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetTCP.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetTCP.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetTCP.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetUDP.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/SDL2_net/SDLnetUDP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix): ../src/SDL2_net/SDLnetUDP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetUDP.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetUDP.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetUDP.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetUDP.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/unittest/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix): ../src/SDL2_net/SDLnetselect.cpp $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/SDL2_net/SDLnetselect.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix): ../src/SDL2_net/SDLnetselect.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(DependSuffix) -MM ../src/SDL2_net/SDLnetselect.cpp

$(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(PreprocessSuffix): ../src/SDL2_net/SDLnetselect.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_SDL2_net_SDLnetselect.cpp$(PreprocessSuffix) ../src/SDL2_net/SDLnetselect.cpp

$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix): ../src/common_io.cpp $(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/common_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix): ../src/common_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_common_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_common_io.cpp$(DependSuffix) -MM ../src/common_io.cpp

$(IntermediateDirectory)/up_src_common_io.cpp$(PreprocessSuffix): ../src/common_io.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_common_io.cpp$(PreprocessSuffix) ../src/common_io.cpp

$(IntermediateDirectory)/session_io_test.cpp$(ObjectSuffix): session_io_test.cpp $(IntermediateDirectory)/session_io_test.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/unittest/session_io_test.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/session_io_test.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/session_io_test.cpp$(DependSuffix): session_io_test.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/session_io_test.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/session_io_test.cpp$(DependSuffix) -MM session_io_test.cpp

$(IntermediateDirectory)/session_io_test.cpp$(PreprocessSuffix): session_io_test.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/session_io_test.cpp$(PreprocessSuffix) session_io_test.cpp

$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix): ../src/sequence_decoder.cpp $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/sequence_decoder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix): ../src/sequence_decoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(DependSuffix) -MM ../src/sequence_decoder.cpp

$(IntermediateDirectory)/up_src_sequence_decoder.cpp$(PreprocessSuffix): ../src/sequence_decoder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_sequence_decoder.cpp$(PreprocessSuffix) ../src/sequence_decoder.cpp

$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix): ../src/socket_ssh.cpp $(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/socket_ssh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix): ../src/socket_ssh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_ssh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_ssh.cpp$(DependSuffix) -MM ../src/socket_ssh.cpp

$(IntermediateDirectory)/up_src_socket_ssh.cpp$(PreprocessSuffix): ../src/socket_ssh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_ssh.cpp$(PreprocessSuffix) ../src/socket_ssh.cpp

$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(ObjectSuffix): ../src/sequenc_decoder_test.cpp $(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/sequenc_decoder_test.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(DependSuffix): ../src/sequenc_decoder_test.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(DependSuffix) -MM ../src/sequenc_decoder_test.cpp

$(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(PreprocessSuffix): ../src/sequenc_decoder_test.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_sequenc_decoder_test.cpp$(PreprocessSuffix) ../src/sequenc_decoder_test.cpp

$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix): ../src/socket_telnet.cpp $(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/socket_telnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix): ../src/socket_telnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_telnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_telnet.cpp$(DependSuffix) -MM ../src/socket_telnet.cpp

$(IntermediateDirectory)/up_src_socket_telnet.cpp$(PreprocessSuffix): ../src/socket_telnet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_telnet.cpp$(PreprocessSuffix) ../src/socket_telnet.cpp

$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix): ../src/socket_irc.cpp $(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/socket_irc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix): ../src/socket_irc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_irc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_irc.cpp$(DependSuffix) -MM ../src/socket_irc.cpp

$(IntermediateDirectory)/up_src_socket_irc.cpp$(PreprocessSuffix): ../src/socket_irc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_irc.cpp$(PreprocessSuffix) ../src/socket_irc.cpp

$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix): ../src/socket_handler.cpp $(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/socket_handler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix): ../src/socket_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_socket_handler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_socket_handler.cpp$(DependSuffix) -MM ../src/socket_handler.cpp

$(IntermediateDirectory)/up_src_socket_handler.cpp$(PreprocessSuffix): ../src/socket_handler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_socket_handler.cpp$(PreprocessSuffix) ../src/socket_handler.cpp

$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix): ../src/session_io.cpp $(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/session_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix): ../src/session_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_session_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_session_io.cpp$(DependSuffix) -MM ../src/session_io.cpp

$(IntermediateDirectory)/up_src_session_io.cpp$(PreprocessSuffix): ../src/session_io.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_session_io.cpp$(PreprocessSuffix) ../src/session_io.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r Debug/


