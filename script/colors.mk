# Set shell to BASH: echo requires -e on mac and Linux
ifndef I
  ECHO := echo -e
else 
  ECHO := echo -e > /dev/null
endif
# SHELL := /bin/bash

define make_echo_color_bold
	@if [ "$(colorful_output)" = "on" ]; then \
	case $(1) in \
		red)     $(ECHO) '\e[1;31m$(2)\e[0m' ;; \
		green)   $(ECHO) '\e[1;32m$(2)\e[0m' ;; \
		yellow)  $(ECHO) '\e[1;33m$(2)\e[0m' ;; \
		blue)    $(ECHO) '\e[1;34m$(2)\e[0m' ;; \
		magenta) $(ECHO) '\e[1;35m$(2)\e[0m' ;; \
		cyan)    $(ECHO) '\e[1;36m$(2)\e[0m' ;; \
		white)   $(ECHO) '\e[1;37m$(2)\e[0m' ;; \
		*)       $(ECHO) '\e[1;4m$(2)\e[0m' ;; \
	esac else $(ECHO) '$(2)'; fi
endef

define make_echo_color
	@if [ "$(colorful_output)" = "on" ]; then \
	case $(1) in \
		red)     $(ECHO) '\e[31m$(2)\e[0m' ;; \
		green)   $(ECHO) '\e[32m$(2)\e[0m' ;; \
		yellow)  $(ECHO) '\e[33m$(2)\e[0m' ;; \
		blue)    $(ECHO) '\e[34m$(2)\e[0m' ;; \
		magenta) $(ECHO) '\e[35m$(2)\e[0m' ;; \
		cyan)    $(ECHO) '\e[36m$(2)\e[0m' ;; \
		white)   $(ECHO) '\e[37m$(2)\e[0m' ;; \
		*)       $(ECHO) '\e[4m$(2)\e[0m' ;; \
	esac else $(ECHO) '$(2)'; fi
endef

define big_success
	@$(ECHO) '$@ : \e[1;32m✓\e[0m'
endef
define success
	@$(ECHO) '$@ : \e[32m✓\e[0m'
endef
define make_echo_build_c_object
	$(call make_echo_color,white,    CC\t\t$<)
endef
define make_echo_build_asm_object
	$(call make_echo_color,white,    CC\t\t$<)
endef
define make_echo_generate_file
	$(call make_echo_color,white,    GEN\t\t$(1))
endef
define make_echo_ld
	$(call make_echo_color,white,    LD\t\t$(1))
endef
# define make_echo_link_c_executable
# 	$(call make_echo_color,green,Linking C executable $@)
# endef

# define make_echo_build_cxx_object
# 	$(call make_echo_color,green,Building CXX object $@)
# endef
# define make_echo_link_cxx_executable
# 	$(call make_echo_color_bold,green,Linking CXX executable $@)
# endef

# define make_echo_link_fortran_executable
# 	$(call make_echo_color_bold,green,Linking Fortran executable $@)
# endef
# define make_echo_build_fortran_object
# 	$(call make_echo_color,green,Building Fortran object $@)
# endef

# define make_echo_link_static_library
# 	$(call make_echo_color_bold,green,Linking static library $@)
# endef
# define make_echo_link_shared_library
# 	$(call make_echo_color_bold,green,Linking shared library $@)
# endef

# define make_echo_run_test
# 	$(call make_echo_color_bold,cyan,$@ : $(1))
# endef
