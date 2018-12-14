cmake_minimum_required(VERSION 3.6)

# Global option #
#################

set (CMAKE_CXX_STANDARD 14)

project(CacaoProcessTools LANGUAGES CXX)

if(NOT TARGET wyrm)
    if(NOT DEFINED WYRM_ROOT)
        set(WYRM_ROOT $ENV{WYRM_ROOT})
    endif()
    if(IS_DIRECTORY ${WYRM_ROOT})
        add_subdirectory(${WYRM_ROOT} wyrm_dir)
    else()
        message( FATAL_ERROR "wyrm path not found, specify it by set WYRM_ROOT to the right path" )
    endif()
endif()

add_library(CacaoProcessTools MODULE
CacaoProcessTools.cpp
)

# add_definitions(${CLIcore_DEFINITIONS})

# target_compile_options(CacaoProcessTools PRIVATE -fopenmp)

target_link_libraries(CacaoProcessTools PRIVATE wyrm CLIcore)

set_target_properties(CacaoProcessTools PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                            SUFFIX "${PYTHON_MODULE_EXTENSION}")