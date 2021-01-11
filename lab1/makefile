include build_tools/common.mk

NPROC = $(shell nproc)

.PHONY: lab1
lab1:
	@echo Building $(TARGET) with -j$(NPROC)
	@$(MAKE) all -j$(NPROC) --makefile=lab1/sources.mk
	@echo Building common
	@$(MAKE) all -j$(NPROC) --makefile=common/sources.mk
	@echo - Executing linker
	@$(MAKE) all -j$(NPROC) --makefile=build_tools/link.mk