file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_library(spa ${srcs} ${headers} "src/InputStream.h" "src/Tokenizer.h" "src/Token.h" "src/QueryParser.h" "src/Tokenizer.cpp" "src/Token.cpp" "src/QueryParser.cpp" "src/InputStream.cpp" "src/TokenTypes.h" "src/QueryInput.h" "src/QueryInput.cpp"  "src/Any.h" "src/Any.cpp" "src/Declaration.h" "src/Declaration.cpp"  "src/Expression.h" "src/Expression.cpp" "src/Ident.h" "src/Ident.cpp" "src/StmtNum.h" "src/StmtNum.cpp" "src/SelectClause.h" "src/SelectClause.cpp" "src/RelationshipClause.h" "src/RelationshipClause.cpp" "src/PatternClause.h" "src/PatternClause.cpp" "src/Query.h" "src/Query.cpp" "src/QueryEvaluator.h" "src/QueryEvaluator.cpp" "src/ResultUtil.h" "src/PKBInterface.h" "src/ResultsTable.cpp" "src/ResultsTable.h" "src/ResultsProjector.h" "src/ResultsProjector.cpp" "src/QueryInterface.h" "src/ExpressionType.h" "src/SimpleParseError.h" "src/SimpleParseError.cpp" "src/SIMPLEToken.h" "src/SIMPLEToken.cpp" "src/SIMPLEHelper.h" "src/SIMPLEHelper.cpp" "src/SIMPLETokenStream.h" "src/SIMPLETokenStream.cpp" "src/Parser.h" "src/Parser.cpp" "src/DesignExtractor.h" "src/DesignExtractor.cpp" "src/TokenizerInterface.h"       "src/OptionalClause.h" "src/ClauseType.h" "src/OptionalClause.cpp" "src/WithClause.h" "src/WithClause.cpp" "src/DisjointClausesSet.h" "src/DisjointClausesSet.cpp" "src/ClauseList.h" "src/ClauseList.cpp" "src/ClauseNode.h" "src/ClauseNode.cpp" "src/QueryOptimizer.h" "src/QueryOptimizer.cpp" "src/ClauseResultType.h")

# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)





