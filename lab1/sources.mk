include common/dirs.mk

SOURCES += $(shell find lab1/ -name '*.cpp')
include build_tools/compile.mk

