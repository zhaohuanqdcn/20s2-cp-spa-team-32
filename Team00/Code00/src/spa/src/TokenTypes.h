#pragma once
#ifndef TOKENTYPES_H
#define TOKENTYPES_H

enum class TokenTypes
{
    Integer,
    Identifier,
    DesignEntity,
    Modifies,
    Uses,
    Parent,
    ParentT,
    Follows,
    FollowsT,
    Calls,
    CallsT,
    Next,
    NextT,
    Affects,
    AffectsT,
    NextBip,
    NextBipT,
    AffectsBip,
    AffectsBipT,
    Select,
    Such,
    That,
    Pattern,
    And,
    Semicolon,
    Underscore,
    LeftParen,
    RightParen,
    Comma,
    DoubleQuote,
    ExprSymbol,
    TermSymbol,
    LeftAngleBracket,
    RightAngleBracket,
    AttrName,
    Equals,
    With,
    Boolean,
    Dot,
};

#endif