project(startup C ASM)

set(BIN_SOURCES
	init.c
	startup.s
)
 
mark3_add_library(startup ${BIN_SOURCES})

set_source_files_properties(startup.s PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")
set_target_properties(
    startup
    PROPERTIES
        COMPILE_FLAGS "${mark3_cc_flags}"
        LINK_FLAGS "${mark3_ln_flags}"
    )

