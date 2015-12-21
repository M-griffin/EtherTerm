##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=EtherTerm
ConfigurationName      :=Debug
WorkspacePath          := "C:\Users\Blue\Desktop\EtherTerm"
ProjectPath            := "C:\Users\Blue\Desktop\EtherTerm"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Blue
Date                   :=12/20/2015
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
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
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/Users/Blue/Downloads/SDL2-devel-2.0.4-mingw/SDL2-2.0.4/include $(IncludeSwitch)C:/Users/Blue/Downloads/SDL2_ttf-2.0.12/i686-w64-mingw32/include/SDL2 $(IncludeSwitch)C:/boost/include/boost-1_58 $(IncludeSwitch)C:/boost_1_58_0/boost $(IncludeSwitch)C:/OpenSSL-Win32/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)mingw32 $(LibrarySwitch)SDL2main $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_ttf $(LibrarySwitch)boost_system-mgw51-mt-1_58 $(LibrarySwitch)boost_thread-mgw51-mt-1_58 $(LibrarySwitch)boost_chrono-mgw51-mt-1_58 $(LibrarySwitch)boost_serialization-mgw51-mt-1_58 $(LibrarySwitch)boost_locale-mgw51-mt-1_58 $(LibrarySwitch)boost_filesystem-mgw51-mt-1_58 $(LibrarySwitch)boost_timer-mgw51-mt-1_58 $(LibrarySwitch)boost_regex-mgw51-mt-1_58 $(LibrarySwitch)ws2_32 $(LibrarySwitch)wsock32 $(LibrarySwitch)iconv $(LibrarySwitch)icuuc.dll $(LibrarySwitch)icuin.dll $(LibrarySwitch)ssleay32 $(LibrarySwitch)eay32 
ArLibs                 :=  "mingw32" "SDL2main" "SDL2" "libSDL2_ttf" "libboost_system-mgw51-mt-1_58" "libboost_thread-mgw51-mt-1_58" "libboost_chrono-mgw51-mt-1_58" "libboost_serialization-mgw51-mt-1_58" "libboost_locale-mgw51-mt-1_58" "libboost_filesystem-mgw51-mt-1_58" "libboost_timer-mgw51-mt-1_58" "libboost_regex-mgw51-mt-1_58" "libws2_32.a" "libwsock32.a" "libiconv.a" "libicuuc.dll.a" "libicuin.dll.a" "ssleay32.a" "libeay32.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:/Users/Blue/Downloads/SDL2_ttf-2.0.12/i686-w64-mingw32/lib $(LibraryPathSwitch)C:/libz-1.2.7-1-mingw32-dev/lib $(LibraryPathSwitch)C:/boost/lib $(LibraryPathSwitch)C:/msys/1.0/icu/source/lib $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/MinGW $(LibraryPathSwitch)C:/OpenSSL-Win32/lib/ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-5.1.0/mingw32/bin/ar.exe rcu
CXX      := C:/MinGW-5.1.0/mingw32/bin/g++.exe
CC       := C:/MinGW-5.1.0/mingw32/bin/gcc.exe
CXXFLAGS :=  -g -O0 -pedantic -W -std=c++11 -Wall -Wno-deprecated -Wno-unused-local-typedefs $(Preprocessors)
CFLAGS   :=  -g -O0 -pedantic -W -std=c++11 -Wall -Wno-deprecated -Wno-unused-local-typedefs $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-5.1.0/mingw32/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_window_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_surface_manager.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_input_handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_sequence_parser.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_screen_buffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_sequence_decoder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_menu_io.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_menu_config.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_menu_function.cpp$(ObjectSuffix) 



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
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM "src/main.cpp"

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) "src/main.cpp"

$(IntermediateDirectory)/src_window_manager.cpp$(ObjectSuffix): src/window_manager.cpp $(IntermediateDirectory)/src_window_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/window_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_window_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_window_manager.cpp$(DependSuffix): src/window_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_window_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_window_manager.cpp$(DependSuffix) -MM "src/window_manager.cpp"

$(IntermediateDirectory)/src_window_manager.cpp$(PreprocessSuffix): src/window_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_window_manager.cpp$(PreprocessSuffix) "src/window_manager.cpp"

$(IntermediateDirectory)/src_surface_manager.cpp$(ObjectSuffix): src/surface_manager.cpp $(IntermediateDirectory)/src_surface_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/surface_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_surface_manager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_surface_manager.cpp$(DependSuffix): src/surface_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_surface_manager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_surface_manager.cpp$(DependSuffix) -MM "src/surface_manager.cpp"

$(IntermediateDirectory)/src_surface_manager.cpp$(PreprocessSuffix): src/surface_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_surface_manager.cpp$(PreprocessSuffix) "src/surface_manager.cpp"

$(IntermediateDirectory)/src_input_handler.cpp$(ObjectSuffix): src/input_handler.cpp $(IntermediateDirectory)/src_input_handler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/input_handler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_input_handler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_input_handler.cpp$(DependSuffix): src/input_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_input_handler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_input_handler.cpp$(DependSuffix) -MM "src/input_handler.cpp"

$(IntermediateDirectory)/src_input_handler.cpp$(PreprocessSuffix): src/input_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_input_handler.cpp$(PreprocessSuffix) "src/input_handler.cpp"

$(IntermediateDirectory)/src_renderer.cpp$(ObjectSuffix): src/renderer.cpp $(IntermediateDirectory)/src_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_renderer.cpp$(DependSuffix): src/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_renderer.cpp$(DependSuffix) -MM "src/renderer.cpp"

$(IntermediateDirectory)/src_renderer.cpp$(PreprocessSuffix): src/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_renderer.cpp$(PreprocessSuffix) "src/renderer.cpp"

$(IntermediateDirectory)/src_sequence_parser.cpp$(ObjectSuffix): src/sequence_parser.cpp $(IntermediateDirectory)/src_sequence_parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/sequence_parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_sequence_parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_sequence_parser.cpp$(DependSuffix): src/sequence_parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_sequence_parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_sequence_parser.cpp$(DependSuffix) -MM "src/sequence_parser.cpp"

$(IntermediateDirectory)/src_sequence_parser.cpp$(PreprocessSuffix): src/sequence_parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_sequence_parser.cpp$(PreprocessSuffix) "src/sequence_parser.cpp"

$(IntermediateDirectory)/src_screen_buffer.cpp$(ObjectSuffix): src/screen_buffer.cpp $(IntermediateDirectory)/src_screen_buffer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/screen_buffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_screen_buffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_screen_buffer.cpp$(DependSuffix): src/screen_buffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_screen_buffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_screen_buffer.cpp$(DependSuffix) -MM "src/screen_buffer.cpp"

$(IntermediateDirectory)/src_screen_buffer.cpp$(PreprocessSuffix): src/screen_buffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_screen_buffer.cpp$(PreprocessSuffix) "src/screen_buffer.cpp"

$(IntermediateDirectory)/src_sequence_decoder.cpp$(ObjectSuffix): src/sequence_decoder.cpp $(IntermediateDirectory)/src_sequence_decoder.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/sequence_decoder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_sequence_decoder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_sequence_decoder.cpp$(DependSuffix): src/sequence_decoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_sequence_decoder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_sequence_decoder.cpp$(DependSuffix) -MM "src/sequence_decoder.cpp"

$(IntermediateDirectory)/src_sequence_decoder.cpp$(PreprocessSuffix): src/sequence_decoder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_sequence_decoder.cpp$(PreprocessSuffix) "src/sequence_decoder.cpp"

$(IntermediateDirectory)/src_menu_io.cpp$(ObjectSuffix): src/menu_io.cpp $(IntermediateDirectory)/src_menu_io.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/menu_io.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_menu_io.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_menu_io.cpp$(DependSuffix): src/menu_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_menu_io.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_menu_io.cpp$(DependSuffix) -MM "src/menu_io.cpp"

$(IntermediateDirectory)/src_menu_io.cpp$(PreprocessSuffix): src/menu_io.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_menu_io.cpp$(PreprocessSuffix) "src/menu_io.cpp"

$(IntermediateDirectory)/src_menu_config.cpp$(ObjectSuffix): src/menu_config.cpp $(IntermediateDirectory)/src_menu_config.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/menu_config.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_menu_config.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_menu_config.cpp$(DependSuffix): src/menu_config.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_menu_config.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_menu_config.cpp$(DependSuffix) -MM "src/menu_config.cpp"

$(IntermediateDirectory)/src_menu_config.cpp$(PreprocessSuffix): src/menu_config.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_menu_config.cpp$(PreprocessSuffix) "src/menu_config.cpp"

$(IntermediateDirectory)/src_menu_function.cpp$(ObjectSuffix): src/menu_function.cpp $(IntermediateDirectory)/src_menu_function.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/src/menu_function.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_menu_function.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_menu_function.cpp$(DependSuffix): src/menu_function.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_menu_function.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_menu_function.cpp$(DependSuffix) -MM "src/menu_function.cpp"

$(IntermediateDirectory)/src_menu_function.cpp$(PreprocessSuffix): src/menu_function.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_menu_function.cpp$(PreprocessSuffix) "src/menu_function.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


