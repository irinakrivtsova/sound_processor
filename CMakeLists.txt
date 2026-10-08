cmake_minimum_required(VERSION 3.16)

project(sound_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOUND_PROCESSOR_SOURCES
    src/parser.cpp
    src/waveform.cpp
    src/wavio.cpp
    src/filters.cpp
    src/pipeline.cpp
    src/converter.cpp
    src/producer.cpp
    src/application.cpp
)

add_library(sound_processor_core
    ${SOUND_PROCESSOR_SOURCES}
)

target_include_directories(sound_processor_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(sound_processor
    main.cpp
)

target_link_libraries(sound_processor PRIVATE
    sound_processor_core
)

include(CTest)

if (BUILD_TESTING)
set(SOUND_PROCESSOR_TEST_SOURCES
    tests/test_parser.cpp
    tests/test_waveform.cpp
    tests/test_filters.cpp
    tests/test_pipeline.cpp
    tests/test_converter.cpp
    tests/test_wavio.cpp
    tests/test_application.cpp
)

find_package(Catch2 3 QUIET)

if (Catch2_FOUND)
    add_executable(sound_processor_tests
        ${SOUND_PROCESSOR_TEST_SOURCES}
    )

    target_link_libraries(sound_processor_tests PRIVATE
        sound_processor_core
        Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(sound_processor_tests)
else()
    message(WARNING "Catch2 was not found. Using a small local fallback runner for build checking.")

    add_executable(sound_processor_tests
        tests/test_main.cpp
        ${SOUND_PROCESSOR_TEST_SOURCES}
    )

    target_include_directories(sound_processor_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    target_link_libraries(sound_processor_tests PRIVATE
        sound_processor_core
    )

    add_test(NAME sound_processor_tests COMMAND sound_processor_tests)
endif()
endif()
