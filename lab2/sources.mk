include common/dirs.mk

SOURCES += $(shell find lab2/ -name '*.cpp')
include build_tools/compile.mk

