#include "TokenizerStub.h"

TokenizerStub::TokenizerStub(std::vector<Token> tokens) : m_tokens(tokens)
{

}

std::shared_ptr<Token> TokenizerStub::readNext() 
{
    if (m_index >= m_tokens.size()) return std::shared_ptr<Token>();  // No more tokens left
    Token currToken = m_tokens.at(m_index);
    m_index++;
    return std::make_shared<Token>(currToken);
}

void TokenizerStub::addToTokenBuffer(std::shared_ptr<Token> token)
{
    tokenBuffer.push(token);
}