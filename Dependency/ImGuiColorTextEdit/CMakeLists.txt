cmake_minimum_required(VERSION 3.25)

project(imguicolortextedit)

add_library(imguicolortextedit)

set(ICTE_DIR "ImGuiColorTextEdit-ca2f9f1")

target_sources(imguicolortextedit PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/${ICTE_DIR}/TextEditor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${ICTE_DIR}/NSOLD.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${ICTE_DIR}/PALETTE.cpp"
)

target_include_directories(imguicolortextedit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${ICTE_DIR}")

target_link_libraries(imguicolortextedit PUBLIC imgui)
