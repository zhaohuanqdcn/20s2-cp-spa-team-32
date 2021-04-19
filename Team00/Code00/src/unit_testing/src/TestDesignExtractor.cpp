#include "DesignExtractor.h"
#include "DesignExtractorHelper.h"
#include "SIMPLETokenStream.h"
#include "Parser.h"

#include "catch.hpp"
#include <memory>
#include <algorithm>
#include <vector>


using namespace std;

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



TEST_CASE("Next BIP chang things") {
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
        "}",

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
        vector<int> answer = {3};
        vector<int> result = extractor.getNextBip(10);
        sort(answer.begin(), answer.end());
        sort(result.begin(), result.end());

        REQUIRE(answer == result); 
    }
}


TEST_CASE("Test 2") {
    vector<string> codes = {
        "procedure A {",
            "read p;",
            "q = p + 4;",
            "while (q != 0) {",
            "print q;",
            "q = q - 1;",
            "}}",
        "procedure B {",
            "s = B + x + 10;",
            "z = B + x / y * z + 10;",
            "y = 26;",
            "while (s == 4) {",
            "y = s + B;",
            "s = x;",
            "call A;",
            "if (A == (s - 4)) then {",
            "read x;",
            "x = A + 4;",
            "print A;",
            "} else {",
            "while (q > s) {",
            "A = B + X + 10 / y * z;",
            "call C; }}}",
            "y = 7;}",
            "procedure C {",
            "if ((A - q) == 18) then {",
            "print A;",
            "call A;",
            "} else {",
            "B = B + x; }",
            "read B; }"
    };

    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    auto pkb = extractor.extractToPKB();


    SECTION("NextStar") {
        vector<int> answer = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        vector<int> result = extractor.getNextStar(14);
        sort(answer.begin(), answer.end());
        sort(result.begin(), result.end());

        REQUIRE(answer == result);
    }

    SECTION("Check affect(11, 10)") {
        auto result = extractor.getAffect(11);
        // cerr << "------------------" << endl;
        // for (auto x: result) cerr << x << " "; cerr << endl;
        // cerr << "------------------" << endl;
        REQUIRE(findVector(extractor.getAffect(11), 10));
    }


    SECTION("Check affect(18, 15)") {
        auto result = extractor.getAffect(18);
        // cerr << "------------------" << endl;
        // for (auto x: result) cerr << x << " "; cerr << endl;
        // cerr << "------------------" << endl;
        REQUIRE(findVector(extractor.getAffect(18), 15));
    }

    SECTION("Affect Star") {
        vector<int> answer = {10, 11, 15, 18};
        vector<int> result = extractor.getAffectStar(18);  
        sort(answer.begin(), answer.end());
        sort(result.begin(), result.end());

        REQUIRE(answer == result); 
    }
}
