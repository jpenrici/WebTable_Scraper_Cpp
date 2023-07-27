cmake_minimum_required(VERSION 3.5)

project(WebTableScraper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    customScraper.h customScraper.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

find_package(LibXml2 REQUIRED)
if (NOT LibXml2_FOUND)
    message(WARNING "LibXml2 library installed is required!")
endif()

find_package(CURL REQUIRED)
if (NOT CURL_FOUND)
    message(WARNING "Curl library installed is required!")
endif()

target_link_libraries (
    ${PROJECT_NAME} PUBLIC
    LibXml2::LibXml2
    CURL::libcurl
)

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
