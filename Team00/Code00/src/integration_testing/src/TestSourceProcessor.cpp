#include "catch.hpp"
#include "Parser.h"

using namespace std;

// TEST_CASE("Advanced test") {
//     vector<string> codes = {
//         "procedure A {\
//             if (y == 1) then {\
//                 while (y == 2) {
//                     if (x == 3) then {
                       
//                     } else {

//                     }
//                 }
//             } else {

//             }
//         "
//     };
// }

TEST_CASE("AffectBip") {
    vector<string> codes = {
        "procedure A {\
            y = 3;\
            if (x == 0) then {\
                call B;\
                while (x > 2) {\
                    call C;\
                }\
            } else {\
                y = 5;\
            }\
            z = y * 3;\
            call D;\
        }\
        procedure B {\
            z = 6;\
            if (z == 3) then {\
                call B1;\
            } else {\
                if (z == 2) then {\
                    call B2;\
                } else {\
                    a = z + 5;\
                }\
            }\
        }\
        procedure B1 {\
            z = 3;\
        }\
        procedure B2 {\
            while (z != 3) {\
                call B2W;\
            }\
        }\
        procedure B2W {\
            z = 5 + y;\
        }\
        procedure C {\
            y = z + 2;\
        }\
        procedure D {\
            y = x + z;\
            if (x == 1) then {\
                while (y == 2) {\
                    if (y == 2) then {\
                        z = y;\
                    } else {\
                        z = x;\
                    }\
                }\
            } else {\
                read aaaa;\
            }\
        }"
    };

    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
   
    auto pkb = extractor.extractToPKB(); 

    SECTION("Affect Bip") {
        REQUIRE(extractor.getAffectBip(1) == vector<int>{7, 18});
        REQUIRE(extractor.getAffectBip(6) == vector<int>{7});
        REQUIRE(extractor.getAffectBip(7) == vector<int>{20});
        REQUIRE(sorted(extractor.getAffectBip(9)) == vector<int>{14, 19});
        REQUIRE(extractor.getAffectBip(20) == vector<int>{24});;
        REQUIRE(extractor.getAffectBip(15) == vector<int>{19});
        REQUIRE(extractor.getAffectBip(18) == vector<int>{19});
        REQUIRE(extractor.getAffectBip(19) == vector<int>{7});
        REQUIRE(extractor.getAffectBip(20) == vector<int>{24});
        REQUIRE(extractor.getAffectBip(24) == vector<int>{});
        REQUIRE(extractor.getAffectBip(25) == vector<int>{});
    }

    SECTION("Next bip") {
        REQUIRE(findVector(extractor.getEndingProcedure("B2"), 16));
        REQUIRE(findVector(extractor.getEndingProcedure("B"), 14));
        REQUIRE(findVector(extractor.getCFGBip(14), 28));

        REQUIRE(findVector(extractor.getNextBip(1), 2));
        REQUIRE(findVector(extractor.getNextBip(2), 3));
        REQUIRE(findVector(extractor.getNextBip(3), 9));
        REQUIRE(findVector(extractor.getNextBip(4), 5));
        REQUIRE(findVector(extractor.getNextBip(4), 7));
        REQUIRE(findVector(extractor.getNextBip(5), 19));
        REQUIRE(findVector(extractor.getNext(6), 7));
        REQUIRE(findVector(extractor.getNextBip(6), 7));
        REQUIRE(findVector(extractor.getNextBip(7), 8));
        REQUIRE(findVector(extractor.getNextBip(8), 20));
        REQUIRE(findVector(extractor.getNextBip(9), 10));
        REQUIRE(findVector(extractor.getNextBip(10), 11));
        REQUIRE(findVector(extractor.getNextBip(10), 12));
        REQUIRE(findVector(extractor.getNextBip(11), 15));
        REQUIRE(findVector(extractor.getNextBip(12), 13));
        REQUIRE(findVector(extractor.getNextBip(12), 14));
        REQUIRE(findVector(extractor.getNextBip(13), 16));
        REQUIRE(findVector(extractor.getNextBip(14), 4));
        REQUIRE(findVector(extractor.getNext(3), 4));
        REQUIRE(findVector(extractor.getCFGBip(28), 4));
        REQUIRE(findVector(extractor.getNextBip(15), 4));
        REQUIRE(findVector(extractor.getNextBip(16), 17));
        REQUIRE(findVector(extractor.getNextBip(16), 4));
        REQUIRE(findVector(extractor.getNextBip(17), 18));
        REQUIRE(findVector(extractor.getNextBip(18), 16));
        REQUIRE(findVector(extractor.getNextBip(19), 4));
        REQUIRE(findVector(extractor.getNextBip(20), 21));
        REQUIRE(findVector(extractor.getNextBip(21), 22));
        REQUIRE(findVector(extractor.getNextBip(22), 23));
        REQUIRE(findVector(extractor.getNextBip(23), 24));
        REQUIRE(findVector(extractor.getNextBip(23), 25));
        REQUIRE(findVector(extractor.getNextBip(24), 22));
        REQUIRE(findVector(extractor.getNextBip(25), 22));
    }
}

TEST_CASE("Next BIP wiki") {
    vector<string> codes = {
        "procedure Bill {",
        "x = 5;",
        "call Mary;",
        "y = x + 6;",
        "x = 5;",
        "z = x * y + 2; }",
        "procedure Mary {",
        "y = x * 3;",
        "call John;",
        "z = x + y; }",
        "procedure John {",
        "if (i > 0) then {",
        "x = x + z; }",
        "else {",
        "y = y * x; }}"
    };

    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    auto pkb = extractor.extractToPKB(); 

    SECTION("check nextBip(10, _)") {
        vector<int> answer = {8};
        vector<int> result = extractor.getNextBip(10);
        sort(answer.begin(), answer.end());
        sort(result.begin(), result.end());

        REQUIRE(answer == result); 
    }

    SECTION("check nextBipStar(7, _)") {
        vector<int> answer = {3, 4, 5, 8, 9, 10, 11};

        vector<int> result = extractor.getNextBipStar(7);
        sort(answer.begin(), answer.end());
        sort(result.begin(), result.end());

        REQUIRE(answer == result); 
    }
}
