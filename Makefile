.PHONY: clean All

All:
	@echo "----------Building project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk"

win:
	@echo "----------Building project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk"

osx:
	@echo "----------Building project:[ EtherTerm - (OSX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.osx.mk"

clean:
	@echo "----------Cleaning project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk" clean

clean-win:
	@echo "----------Cleaning project:[ EtherTerm - (WIN) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.mk" clean

clean-osx:
	@echo "----------Cleaning project:[ EtherTerm - (OSX) Debug ]----------"
	@$(MAKE) -f  "EtherTerm.osx.mk" clean
