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
Date                   :=01/05/15
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=C:/MinGW-4.8.1/bin/g++.exe 
SharedObjectLinkerName :=C:/MinGW-4.8.1/bin/g++.exe -shared -fPIC
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
RcCompilerName         :=C:/MinGW-4.8.1/bin/windres.exe 
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/Users/Blue/Downloads/SDL2-devel-2.0.3-mingw/SDL2-2.0.3/include $(IncludeSwitch)C:/Users/Blue/Downloads/SDL2_net-devel-2.0.0-mingw/SDL2_net-2.0.0/i686-w64-mingw32/include/SDL2 $(IncludeSwitch)C:/libssh-0.6.0/include $(IncludeSwitch)libz-1.2.7-1-mingw32-dev $(IncludeSwitch)C:/libz-1.2.7-1-mingw32-dev/include $(IncludeSwitch)C:/libz-1.2.7-1-mingw32-dev/include/minizip 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)mingw32 $(LibrarySwitch)SDL2main $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_net $(LibrarySwitch)ssh $(LibrarySwitch)minizip 
ArLibs                 :=  "mingw32" "SDL2main" "SDL2" "SDL2_net" "libssh" "libminizip" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:/Users/Blue/Downloads/SDL2_image-devel-2.0.0-mingw/SDL2_image-2.0.0/i686-w64-mingw32/lib $(LibraryPathSwitch)C:/Users/Blue/Downloads/SDL2_net-devel-2.0.0-mingw/SDL2_net-2.0.0/i686-w64-mingw32/lib $(LibraryPathSwitch)C:/libssh-0.6.0/bin $(LibraryPathSwitch)C:/libz-1.2.7-1-mingw32-dev/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-4.8.1/bin/ar.exe rcu
CXX      := C:/MinGW-4.8.1/bin/g++.exe 
CC       := C:/MinGW-4.8.1/bin/gcc.exe 
CXXFLAGS :=  -g -O0 -Wall -std=gnu++0x $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall -std=gnu++0x $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-4.8.1/bin/as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) $(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) $(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) $(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) $(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) $(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) $(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) $(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix): termStateMachine.cpp $(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/termStateMachine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix): termStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix) -MM "termStateMachine.cpp"

$(IntermediateDirectory)/termStateMachine.cpp$(PreprocessSuffix): termStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/termStateMachine.cpp$(PreprocessSuffix) "termStateMachine.cpp"

$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix): termObjectFactory.cpp $(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/termObjectFactory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix): termObjectFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix) -MM "termObjectFactory.cpp"

$(IntermediateDirectory)/termObjectFactory.cpp$(PreprocessSuffix): termObjectFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/termObjectFactory.cpp$(PreprocessSuffix) "termObjectFactory.cpp"

$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix): mainMenuState.cpp $(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/mainMenuState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix): mainMenuState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix) -MM "mainMenuState.cpp"

$(IntermediateDirectory)/mainMenuState.cpp$(PreprocessSuffix): mainMenuState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mainMenuState.cpp$(PreprocessSuffix) "mainMenuState.cpp"

$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix): inputHandler.cpp $(IntermediateDirectory)/inputHandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/inputHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/inputHandler.cpp$(DependSuffix): inputHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/inputHandler.cpp$(DependSuffix) -MM "inputHandler.cpp"

$(IntermediateDirectory)/inputHandler.cpp$(PreprocessSuffix): inputHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/inputHandler.cpp$(PreprocessSuffix) "inputHandler.cpp"

$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix): stateParser.cpp $(IntermediateDirectory)/stateParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/stateParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/stateParser.cpp$(DependSuffix): stateParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/stateParser.cpp$(DependSuffix) -MM "stateParser.cpp"

$(IntermediateDirectory)/stateParser.cpp$(PreprocessSuffix): stateParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/stateParser.cpp$(PreprocessSuffix) "stateParser.cpp"

$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix): tinystr.cpp $(IntermediateDirectory)/tinystr.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/tinystr.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinystr.cpp$(DependSuffix): tinystr.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinystr.cpp$(DependSuffix) -MM "tinystr.cpp"

$(IntermediateDirectory)/tinystr.cpp$(PreprocessSuffix): tinystr.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinystr.cpp$(PreprocessSuffix) "tinystr.cpp"

$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix): tinyxml.cpp $(IntermediateDirectory)/tinyxml.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/tinyxml.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxml.cpp$(DependSuffix): tinyxml.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxml.cpp$(DependSuffix) -MM "tinyxml.cpp"

$(IntermediateDirectory)/tinyxml.cpp$(PreprocessSuffix): tinyxml.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxml.cpp$(PreprocessSuffix) "tinyxml.cpp"

$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix): tinyxmlerror.cpp $(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/tinyxmlerror.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix): tinyxmlerror.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix) -MM "tinyxmlerror.cpp"

$(IntermediateDirectory)/tinyxmlerror.cpp$(PreprocessSuffix): tinyxmlerror.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxmlerror.cpp$(PreprocessSuffix) "tinyxmlerror.cpp"

$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix): tinyxmlparser.cpp $(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/tinyxmlparser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix): tinyxmlparser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix) -MM "tinyxmlparser.cpp"

$(IntermediateDirectory)/tinyxmlparser.cpp$(PreprocessSuffix): tinyxmlparser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxmlparser.cpp$(PreprocessSuffix) "tinyxmlparser.cpp"

$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix): telnetState.cpp $(IntermediateDirectory)/telnetState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/telnetState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/telnetState.cpp$(DependSuffix): telnetState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/telnetState.cpp$(DependSuffix) -MM "telnetState.cpp"

$(IntermediateDirectory)/telnetState.cpp$(PreprocessSuffix): telnetState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/telnetState.cpp$(PreprocessSuffix) "telnetState.cpp"

$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix): socketSSH.cpp $(IntermediateDirectory)/socketSSH.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/socketSSH.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketSSH.cpp$(DependSuffix): socketSSH.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketSSH.cpp$(DependSuffix) -MM "socketSSH.cpp"

$(IntermediateDirectory)/socketSSH.cpp$(PreprocessSuffix): socketSSH.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketSSH.cpp$(PreprocessSuffix) "socketSSH.cpp"

$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix): socketTelnet.cpp $(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/socketTelnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix): socketTelnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix) -MM "socketTelnet.cpp"

$(IntermediateDirectory)/socketTelnet.cpp$(PreprocessSuffix): socketTelnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketTelnet.cpp$(PreprocessSuffix) "socketTelnet.cpp"

$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix): socketHandler.cpp $(IntermediateDirectory)/socketHandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/socketHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketHandler.cpp$(DependSuffix): socketHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketHandler.cpp$(DependSuffix) -MM "socketHandler.cpp"

$(IntermediateDirectory)/socketHandler.cpp$(PreprocessSuffix): socketHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketHandler.cpp$(PreprocessSuffix) "socketHandler.cpp"

$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix): socketICMP.cpp $(IntermediateDirectory)/socketICMP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/socketICMP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketICMP.cpp$(DependSuffix): socketICMP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketICMP.cpp$(DependSuffix) -MM "socketICMP.cpp"

$(IntermediateDirectory)/socketICMP.cpp$(PreprocessSuffix): socketICMP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketICMP.cpp$(PreprocessSuffix) "socketICMP.cpp"

$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix): menuFunction.cpp $(IntermediateDirectory)/menuFunction.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/menuFunction.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/menuFunction.cpp$(DependSuffix): menuFunction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/menuFunction.cpp$(DependSuffix) -MM "menuFunction.cpp"

$(IntermediateDirectory)/menuFunction.cpp$(PreprocessSuffix): menuFunction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/menuFunction.cpp$(PreprocessSuffix) "menuFunction.cpp"

$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix): ansiParser.cpp $(IntermediateDirectory)/ansiParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/ansiParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ansiParser.cpp$(DependSuffix): ansiParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ansiParser.cpp$(DependSuffix) -MM "ansiParser.cpp"

$(IntermediateDirectory)/ansiParser.cpp$(PreprocessSuffix): ansiParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ansiParser.cpp$(PreprocessSuffix) "ansiParser.cpp"

$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix): linkList.cpp $(IntermediateDirectory)/linkList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/linkList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/linkList.cpp$(DependSuffix): linkList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/linkList.cpp$(DependSuffix) -MM "linkList.cpp"

$(IntermediateDirectory)/linkList.cpp$(PreprocessSuffix): linkList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/linkList.cpp$(PreprocessSuffix) "linkList.cpp"

$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix): sshState.cpp $(IntermediateDirectory)/sshState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/sshState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sshState.cpp$(DependSuffix): sshState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sshState.cpp$(DependSuffix) -MM "sshState.cpp"

$(IntermediateDirectory)/sshState.cpp$(PreprocessSuffix): sshState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sshState.cpp$(PreprocessSuffix) "sshState.cpp"

$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix): terminal.cpp $(IntermediateDirectory)/terminal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/terminal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/terminal.cpp$(DependSuffix): terminal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/terminal.cpp$(DependSuffix) -MM "terminal.cpp"

$(IntermediateDirectory)/terminal.cpp$(PreprocessSuffix): terminal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/terminal.cpp$(PreprocessSuffix) "terminal.cpp"

$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix): sequenceParser.cpp $(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Blue/Desktop/EtherTerm/sequenceParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix): sequenceParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix) -MM "sequenceParser.cpp"

$(IntermediateDirectory)/sequenceParser.cpp$(PreprocessSuffix): sequenceParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sequenceParser.cpp$(PreprocessSuffix) "sequenceParser.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) ".build-debug/EtherTerm"


