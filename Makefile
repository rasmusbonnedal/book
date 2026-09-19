WIX_DIR ?= C:/tools/wix-3.11.2
export PATH := $(WIX_DIR);$(PATH)

.PHONY: help installer

help:
	@echo Run make installer to build the installer.

installer:
	cmake --build build --config Release --target package
