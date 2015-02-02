##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased
##
## Debug
ProjectName            :=EtherTerm
ConfigurationName      :=Debug
WorkspacePath          := "/EtherTerm"
ProjectPath            := "/EtherTerm"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Michae Griffin
Date                   :=01/16/15
CodeLitePath           :="/home/merc/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
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
MakeDirCommand         :=mkdir -p
LinkOptions            :=
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch).
IncludePCH             :=
RcIncludePath          :=
Libs                   := $(LibrarySwitch)SDL2main $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_net $(LibrarySwitch)ssh
ArLibs                 :=  "SDL2main" "SDL2" "SDL2_net" "libssh"
LibPath                := $(LibraryPathSwitch).

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall  -std=gnu++0x $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall  -std=gnu++0x $(Preprocessors)
ASFLAGS  :=
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) $(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) $(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) $(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketFTP.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) $(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) $(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) $(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) $(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) $(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) $(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) $(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix)



Objects=$(Objects0)

##
## Main Build Targets
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)
	cp ./Debug/EtherTerm .

$(OutputFile): $(IntermediateDirectory)/.d $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix): ansiParser.cpp $(IntermediateDirectory)/ansiParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "ansiParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ansiParser.cpp$(DependSuffix): ansiParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ansiParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ansiParser.cpp$(DependSuffix) -MM "ansiParser.cpp"

$(IntermediateDirectory)/ansiParser.cpp$(PreprocessSuffix): ansiParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ansiParser.cpp$(PreprocessSuffix) "ansiParser.cpp"

$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix): inputHandler.cpp $(IntermediateDirectory)/inputHandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "inputHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/inputHandler.cpp$(DependSuffix): inputHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/inputHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/inputHandler.cpp$(DependSuffix) -MM "inputHandler.cpp"

$(IntermediateDirectory)/inputHandler.cpp$(PreprocessSuffix): inputHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/inputHandler.cpp$(PreprocessSuffix) "inputHandler.cpp"

$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix): linkList.cpp $(IntermediateDirectory)/linkList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "linkList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/linkList.cpp$(DependSuffix): linkList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/linkList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/linkList.cpp$(DependSuffix) -MM "linkList.cpp"

$(IntermediateDirectory)/linkList.cpp$(PreprocessSuffix): linkList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/linkList.cpp$(PreprocessSuffix) "linkList.cpp"

$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix): mainMenuState.cpp $(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "mainMenuState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix): mainMenuState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mainMenuState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mainMenuState.cpp$(DependSuffix) -MM "mainMenuState.cpp"

$(IntermediateDirectory)/mainMenuState.cpp$(PreprocessSuffix): mainMenuState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mainMenuState.cpp$(PreprocessSuffix) "mainMenuState.cpp"

$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix): menuFunction.cpp $(IntermediateDirectory)/menuFunction.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "menuFunction.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/menuFunction.cpp$(DependSuffix): menuFunction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/menuFunction.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/menuFunction.cpp$(DependSuffix) -MM "menuFunction.cpp"

$(IntermediateDirectory)/menuFunction.cpp$(PreprocessSuffix): menuFunction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/menuFunction.cpp$(PreprocessSuffix) "menuFunction.cpp"

$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix): sequenceParser.cpp $(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "sequenceParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix): sequenceParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sequenceParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sequenceParser.cpp$(DependSuffix) -MM "sequenceParser.cpp"

$(IntermediateDirectory)/sequenceParser.cpp$(PreprocessSuffix): sequenceParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sequenceParser.cpp$(PreprocessSuffix) "sequenceParser.cpp"

$(IntermediateDirectory)/socketFTP.cpp$(ObjectSuffix): socketFTP.cpp $(IntermediateDirectory)/socketFTP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "socketFTP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketFTP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketFTP.cpp$(DependSuffix): socketFTP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketFTP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketFTP.cpp$(DependSuffix) -MM "socketFTP.cpp"

$(IntermediateDirectory)/socketFTP.cpp$(PreprocessSuffix): socketFTP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketFTP.cpp$(PreprocessSuffix) "socketFTP.cpp"

$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix): socketHandler.cpp $(IntermediateDirectory)/socketHandler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "socketHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketHandler.cpp$(DependSuffix): socketHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketHandler.cpp$(DependSuffix) -MM "socketHandler.cpp"

$(IntermediateDirectory)/socketHandler.cpp$(PreprocessSuffix): socketHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketHandler.cpp$(PreprocessSuffix) "socketHandler.cpp"

$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix): socketICMP.cpp $(IntermediateDirectory)/socketICMP.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "socketICMP.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketICMP.cpp$(DependSuffix): socketICMP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketICMP.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketICMP.cpp$(DependSuffix) -MM "socketICMP.cpp"

$(IntermediateDirectory)/socketICMP.cpp$(PreprocessSuffix): socketICMP.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketICMP.cpp$(PreprocessSuffix) "socketICMP.cpp"

$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix): socketSSH.cpp $(IntermediateDirectory)/socketSSH.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "socketSSH.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketSSH.cpp$(DependSuffix): socketSSH.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketSSH.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketSSH.cpp$(DependSuffix) -MM "socketSSH.cpp"

$(IntermediateDirectory)/socketSSH.cpp$(PreprocessSuffix): socketSSH.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketSSH.cpp$(PreprocessSuffix) "socketSSH.cpp"

$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix): socketTelnet.cpp $(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "socketTelnet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix): socketTelnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketTelnet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/socketTelnet.cpp$(DependSuffix) -MM "socketTelnet.cpp"

$(IntermediateDirectory)/socketTelnet.cpp$(PreprocessSuffix): socketTelnet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketTelnet.cpp$(PreprocessSuffix) "socketTelnet.cpp"

$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix): sshState.cpp $(IntermediateDirectory)/sshState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "sshState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sshState.cpp$(DependSuffix): sshState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sshState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sshState.cpp$(DependSuffix) -MM "sshState.cpp"

$(IntermediateDirectory)/sshState.cpp$(PreprocessSuffix): sshState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sshState.cpp$(PreprocessSuffix) "sshState.cpp"

$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix): stateParser.cpp $(IntermediateDirectory)/stateParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "stateParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/stateParser.cpp$(DependSuffix): stateParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/stateParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/stateParser.cpp$(DependSuffix) -MM "stateParser.cpp"

$(IntermediateDirectory)/stateParser.cpp$(PreprocessSuffix): stateParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/stateParser.cpp$(PreprocessSuffix) "stateParser.cpp"

$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix): telnetState.cpp $(IntermediateDirectory)/telnetState.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "telnetState.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/telnetState.cpp$(DependSuffix): telnetState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/telnetState.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/telnetState.cpp$(DependSuffix) -MM "telnetState.cpp"

$(IntermediateDirectory)/telnetState.cpp$(PreprocessSuffix): telnetState.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/telnetState.cpp$(PreprocessSuffix) "telnetState.cpp"

$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix): termObjectFactory.cpp $(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "termObjectFactory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix): termObjectFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/termObjectFactory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/termObjectFactory.cpp$(DependSuffix) -MM "termObjectFactory.cpp"

$(IntermediateDirectory)/termObjectFactory.cpp$(PreprocessSuffix): termObjectFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/termObjectFactory.cpp$(PreprocessSuffix) "termObjectFactory.cpp"

$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix): termStateMachine.cpp $(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "termStateMachine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix): termStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/termStateMachine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/termStateMachine.cpp$(DependSuffix) -MM "termStateMachine.cpp"

$(IntermediateDirectory)/termStateMachine.cpp$(PreprocessSuffix): termStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/termStateMachine.cpp$(PreprocessSuffix) "termStateMachine.cpp"

$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix): terminal.cpp $(IntermediateDirectory)/terminal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "terminal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/terminal.cpp$(DependSuffix): terminal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/terminal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/terminal.cpp$(DependSuffix) -MM "terminal.cpp"

$(IntermediateDirectory)/terminal.cpp$(PreprocessSuffix): terminal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/terminal.cpp$(PreprocessSuffix) "terminal.cpp"

$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix): tinystr.cpp $(IntermediateDirectory)/tinystr.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "tinystr.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinystr.cpp$(DependSuffix): tinystr.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinystr.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinystr.cpp$(DependSuffix) -MM "tinystr.cpp"

$(IntermediateDirectory)/tinystr.cpp$(PreprocessSuffix): tinystr.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinystr.cpp$(PreprocessSuffix) "tinystr.cpp"

$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix): tinyxml.cpp $(IntermediateDirectory)/tinyxml.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "tinyxml.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxml.cpp$(DependSuffix): tinyxml.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxml.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxml.cpp$(DependSuffix) -MM "tinyxml.cpp"

$(IntermediateDirectory)/tinyxml.cpp$(PreprocessSuffix): tinyxml.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxml.cpp$(PreprocessSuffix) "tinyxml.cpp"

$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix): tinyxmlerror.cpp $(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "tinyxmlerror.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix): tinyxmlerror.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxmlerror.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxmlerror.cpp$(DependSuffix) -MM "tinyxmlerror.cpp"

$(IntermediateDirectory)/tinyxmlerror.cpp$(PreprocessSuffix): tinyxmlerror.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxmlerror.cpp$(PreprocessSuffix) "tinyxmlerror.cpp"

$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix): tinyxmlparser.cpp $(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "tinyxmlparser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix): tinyxmlparser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tinyxmlparser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tinyxmlparser.cpp$(DependSuffix) -MM "tinyxmlparser.cpp"

$(IntermediateDirectory)/tinyxmlparser.cpp$(PreprocessSuffix): tinyxmlparser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tinyxmlparser.cpp$(PreprocessSuffix) "tinyxmlparser.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) ".build-debug/EtherTerm"
	$(RM) ./EtherTerm


