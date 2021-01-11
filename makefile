include build_tools/common.mk

.PHONY: lab1
lab1:
	@make lab1 --makefile=lab1/makefile

.PHONY: lab2
lab2:
	@make lab2 --makefile=lab2/makefile

all: clean lab1 lab2