set(SOURCE_LIST
include/cpu/state.h
include/cpu/common.h
include/cpu/functions.h
include/cpu/impl/interface.h
include/cpu/disasm/functions.h
include/cpu/disasm/state.h

src/disasm.cpp
src/cpu.cpp
)

set(SOURCE_UNICORN_CPU
include/cpu/impl/unicorn_cpu.h

src/unicorn_cpu.cpp
)

set(SOURCE_DYNARMIC_CPU
include/cpu/impl/dynarmic_cpu.h

src/dynarmic_cpu.cpp
)

add_library( cpu STATIC ${SOURCE_LIST})
if(USE_UNICORN)
    target_sources( cpu PRIVATE ${SOURCE_UNICORN_CPU})
endif()

if(USE_DYNARMIC)
    target_sources( cpu PRIVATE ${SOURCE_DYNARMIC_CPU})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(cpu PUBLIC include)
target_link_libraries(cpu PUBLIC mem util)
target_link_libraries(cpu PRIVATE capstone)
if(USE_UNICORN)
    target_link_libraries(cpu PRIVATE unicorn)
endif()

if(USE_DYNARMIC)
    target_link_libraries(cpu PRIVATE dynarmic merry::mcl)
endif()
