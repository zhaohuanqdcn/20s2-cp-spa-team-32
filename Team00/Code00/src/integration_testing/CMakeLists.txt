file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(integration_testing ${srcs} "src/ParserTokenizerIntegrationTester.cpp" "src/TestPKBQuery.cpp" "src/TestQuerySystem.cpp" "src/TestResultsTableUtil.h" "src/TestResultsTableUtil.cpp" "src/TestParserPKBPQL.cpp")


target_link_libraries(integration_testing spa)
