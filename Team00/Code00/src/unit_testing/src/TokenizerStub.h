#pragma once
#ifndef TOKENIZERSTUB_H
#define TOKENIZERSTUB_H
#include "TokenizerInterface.h"
#include <vector>
#include <queue>

class TokenizerStub : public TokenizerInterface {
private:

	std::vector<Token> m_tokens;
	int m_index{ 0 };
    std::queue<std::shared_ptr<Token>> tokenBuffer;

public:

	TokenizerStub(std::vector<Token> tokens);

	std::shared_ptr<Token> readNext();
    void addToTokenBuffer(std::shared_ptr<Token> token);

};

#endif