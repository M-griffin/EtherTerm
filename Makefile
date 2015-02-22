.PHONY: clean All

All:
	@echo "----------------Building EtherTerm-------------------"
	@echo "make win        = Windows Build"
	@echo "make linux      = Linux (Debian, Mint, Ubuntu)"
	@echo "make linux-arch = Arch Linux"
	@echo "make osx        = Mac OSX Lion - Yosemite"
	@echo "-------------Clean EtherTerm Project-----------------"
	@echo "make clean-win"
	@echo "make clean-linux"
	@echo "make clean-osx"

win:
	@echo "----------Building project:[ EtherTerm - (WIN) Release ]----------"
	@$(MAKE) -f  "EtherTerm.mk"

osx:
	@echo "----------Building project:[ EtherTerm - (OSX) Release ]----------"
	@$(MAKE) -f  "EtherTerm.osx.mk"

linux:
	@echo "----------Building project:[ EtherTerm - (LINUX) Release ]----------"
	@$(MAKE) -f  "EtherTerm.linux.mk"

linux-arch:
	@echo "----------Building project:[ EtherTerm - (ARCH LINUX) Release ]----------"
	@$(MAKE) -f  "EtherTerm.linux-arch.mk"

clean:
	@echo "----------------Building EtherTerm-------------------"
	@echo "make win        = Windows Build"
	@echo "make linux      = Linux (Debian, Mint, Ubuntu)"
	@echo "make linux-arch = Arch Linux"
	@echo "make osx        = Mac OSX Lion - Yosemite"
	@echo "-------------Clean EtherTerm Project-----------------"
	@echo "make clean-win"
	@echo "make clean-linux"
	@echo "make clean-osx"

clean-win:
	@echo "----------Cleaning project:[ EtherTerm - (WIN) Release ]----------"
	@$(MAKE) -f  "EtherTerm.mk" clean

clean-osx:
	@echo "----------Cleaning project:[ EtherTerm - (OSX) Release ]----------"
	@$(MAKE) -f  "EtherTerm.osx.mk" clean

clean-linux:
	@echo "----------Cleaning project:[ EtherTerm - (LINUX) Release ]----------"
	@$(MAKE) -f  "EtherTerm.linux.mk" clean

