cmake_minimum_required(VERSION 3.16)
project(HTMLEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_SOURCES
    src/types.cpp
    src/lexer/html_lexer.cpp
    src/parser/html_parser.cpp
    src/dom/node.cpp
    src/dom/document.cpp
    src/dom/style.cpp
    src/layout/layout_engine.cpp
    src/render/render_tree.cpp
    src/gpu/scene_graph.cpp
    src/gpu/gpu_renderer.cpp
    src/veo/veo_runtime.cpp
    src/engine.cpp
)

add_library(html_engine STATIC ${ENGINE_SOURCES})
target_include_directories(html_engine PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(html_engine_example examples/main.cpp)
target_link_libraries(html_engine_example html_engine)

add_executable(test_lexer tests/test_lexer.cpp)
target_link_libraries(test_lexer html_engine)

add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser html_engine)

add_executable(test_layout tests/test_layout.cpp)
target_link_libraries(test_layout html_engine)

enable_testing()
add_test(NAME LexerTests COMMAND test_lexer)
add_test(NAME ParserTests COMMAND test_parser)
add_test(NAME LayoutTests COMMAND test_layout)
