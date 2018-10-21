project(ut_support)

set(LIB_SOURCES
    ut_support.cpp
)

set(LIB_HEADERS
    public/ut_support.h
	public/stdint.h
	public/stddef.h
	public/stdbool.h
)

mark3_add_library(ut_support ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(ut_support
    PUBLIC
        public
    )

target_link_libraries(ut_support
    driver
    mark3
    memutil
    semihosting
)

