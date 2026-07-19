cmake_minimum_required(VERSION 3.8)

project(BroodOS VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_library(broodos_storage STATIC
    src/storage_scan.cpp
)
target_include_directories(broodos_storage PUBLIC src)
target_compile_options(broodos_storage PRIVATE -Wall -Wextra -Wpedantic)

add_executable(broodos
    src/main.cpp
    src/pixel_font.cpp
)
target_include_directories(broodos SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_options(broodos PRIVATE ${SDL2_CFLAGS_OTHER} -Wall -Wextra -Wpedantic)
target_link_libraries(broodos PRIVATE broodos_storage ${SDL2_LDFLAGS})

install(TARGETS broodos RUNTIME DESTINATION .)
install(FILES appinfo.json assets/icon.png DESTINATION .)

option(BROODOS_BUILD_TESTS "Build the storage scanner tests" ON)
if(BROODOS_BUILD_TESTS)
    enable_testing()
    add_executable(storage_scan_test tests/storage_scan_test.cpp)
    target_link_libraries(storage_scan_test PRIVATE broodos_storage)
    add_test(NAME storage_scan_test COMMAND storage_scan_test)
endif()

set(CPACK_GENERATOR "External")
set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/AresPackage.cmake")
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
set(CPACK_MONOLITHIC_INSTALL TRUE)
include(CPack)
