add_library(packages STATIC
            src/license.cpp
            src/pkg.cpp
            src/pup.cpp
            src/sce_utils.cpp
            src/sfo.cpp
            include/packages/functions.h
            include/packages/pkg.h
            include/packages/sce_types.h
            include/packages/sfo.h
)
target_include_directories(packages PUBLIC include)
target_link_libraries(packages PUBLIC emuenv util)
target_link_libraries(packages PRIVATE config crypto emuenv FAT16 host_dialog io miniz psvpfsparser vita-toolchain)
