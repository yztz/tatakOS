
# Set echo based on $(shell uname)
UNAME := $(shell uname)

ECHO = echo

ifeq ($(VERBOSE),1)
	at=
else
	at=@
endif

# Set shell to BASH: echo requires -e on mac and Linux
# ECHO := echo -e
# SHELL := /bin/bash

define make_echo_color_bold
	@case $(1) in \
		red)     $(ECHO) '[1;31m$(2)[0m' ;; \
		green)   $(ECHO) '[1;32m$(2)[0m' ;; \
		yellow)  $(ECHO) '[1;33m$(2)[0m' ;; \
		blue)    $(ECHO) '[1;34m$(2)[0m' ;; \
		magenta) $(ECHO) '[1;35m$(2)[0m' ;; \
		cyan)    $(ECHO) '[1;36m$(2)[0m' ;; \
		white)   $(ECHO) '[1;37m$(2)[0m' ;; \
		*)       $(ECHO) '[1;4m$(2)[0m' ;; \
	esac
endef

define make_echo_color
	@case $(1) in \
		red)     $(ECHO) '[31m$(2)[0m' ;; \
		green)   $(ECHO) '[32m$(2)[0m' ;; \
		yellow)  $(ECHO) '[33m$(2)[0m' ;; \
		blue)    $(ECHO) '[34m$(2)[0m' ;; \
		magenta) $(ECHO) '[35m$(2)[0m' ;; \
		cyan)    $(ECHO) '[36m$(2)[0m' ;; \
		white)   $(ECHO) '[37m$(2)[0m' ;; \
		*)       $(ECHO) '[4m$(2)[0m' ;; \
	esac
endef

define big_success
	@$(ECHO) '$@ : [1;32m✓[0m'
endef
define success
	@$(ECHO) '$@ : [32m✓[0m'
endef
define make_echo_build_c_object
	$(call make_echo_color,green,Building C object $@)
endef
define make_echo_link_c_executable
	$(call make_echo_color_bold,green,Linking C executable $@)
endef

define make_echo_build_cxx_object
	$(call make_echo_color,green,Building CXX object $@)
endef
define make_echo_link_cxx_executable
	$(call make_echo_color_bold,green,Linking CXX executable $@)
endef

define make_echo_link_fortran_executable
	$(call make_echo_color_bold,green,Linking Fortran executable $@)
endef
define make_echo_build_fortran_object
	$(call make_echo_color,green,Building Fortran object $@)
endef

define make_echo_link_static_library
	$(call make_echo_color_bold,green,Linking static library $@)
endef
define make_echo_link_shared_library
	$(call make_echo_color_bold,green,Linking shared library $@)
endef
define make_echo_generate_file
	$(call make_echo_color_bold,blue,Generating $@)
endef
define make_echo_run_test
	$(call make_echo_color_bold,cyan,$@ : $(1))
endef
