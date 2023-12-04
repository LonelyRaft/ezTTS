
cmake_minimum_required(VERSION 3.20)

set(EZTTS "eztts")

project(${EZTTS} LANGUAGES CXX)

add_executable(${EZTTS})

file(GLOB EZTTS_SRC *.cpp *.h)
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    file(GLOB EZTTS_SRC_PLAT ./windows/*.cpp ./windows/*.h)
    list(APPEND EZTTS_SRC ${EZTTS_SRC_PLAT})
    target_compile_definitions(
            ${EZTTS} PRIVATE COBJMACROS)
    target_link_libraries(
            ${EZTTS} PRIVATE sapi)
else ()
    file(GLOB EZTTS_SRC_PLAT ./linux/*.cpp ./linux/*.h)
    list(APPEND EZTTS_SRC ${EZTTS_SRC_PLAT})
endif ()

target_sources(${EZTTS} PRIVATE ${EZTTS_SRC})
target_compile_definitions(
        ${EZTTS} PRIVATE EZTTS_EXPORT)


