.PHONY: clean All

All:
	@echo "----------Building project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk"

win:
	@echo "----------Building project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk"

osx:
	@echo "----------Building project:[ EtherTerm - (OSX) Debug ]----------"
	@echo "----------Build Supports: [Lion, Mountain Lion, Mavericks, Yosemite builds ]-------"
	@$(MAKE) -f  "EtherTerm.osx.mk"
    
linux:
	@echo "----------Building project:[ EtherTerm - (LINUX) Debug ]----------"
	@echo "----------Build Supports: [Ubuntu, Mint and Debian builds ]-------"
	@$(MAKE) -f  "EtherTerm.linux.mk"
	
linux-arch:
	@echo "----------Building project:[ EtherTerm - (ARCH LINUX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.linux-arch.mk"

clean:
	@echo "----------Cleaning project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk" clean

clean-win:
	@echo "----------Cleaning project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk" clean

clean-osx:
	@echo "----------Cleaning project:[ EtherTerm - (OSX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.osx.mk" clean

clean-linux:
	@echo "----------Cleaning project:[ EtherTerm - (LINUX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.linux.mk" clean

clean-linux-arch:
	@echo "----------Cleaning project:[ EtherTerm - (ARCH LINUX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.linux-arch.mk" clean

