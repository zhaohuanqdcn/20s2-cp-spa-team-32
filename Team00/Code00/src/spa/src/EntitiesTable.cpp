#include "EntitiesTable.h"

std::unordered_map<RelationshipType, std::set<EntityType>> EntitiesTable::relRefLeftArgTable = 
{
    {RelationshipType::MODIFIES, {EntityType::ASSIGN, EntityType::STMT, EntityType::READ, EntityType::PROC, EntityType::IF, EntityType::WHILE }},
    {RelationshipType::USES, { EntityType::ASSIGN, EntityType::STMT, EntityType::PRINT, EntityType::PROC, EntityType::IF, EntityType::WHILE }},
    {RelationshipType::PARENT, { EntityType::STMT, EntityType::WHILE, EntityType::IF }},
    {RelationshipType::FOLLOWS, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ }},
    {RelationshipType::CALLS, { EntityType::PROC }},
    {RelationshipType::NEXT, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
            EntityType::IF, EntityType::WHILE }}, 
    {RelationshipType::NEXTBIP, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
            EntityType::IF, EntityType::WHILE }},
    {RelationshipType::AFFECTS, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
    {RelationshipType::AFFECTSBIP, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
};

std::unordered_map<RelationshipType, std::set<EntityType>> EntitiesTable::relRefRightArgTable =
{
    {RelationshipType::MODIFIES, { EntityType::VAR } },
    { RelationshipType::USES, { EntityType::VAR } },
    { RelationshipType::PARENT, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ } },
    { RelationshipType::FOLLOWS, { EntityType::ASSIGN, EntityType::STMT, EntityType::WHILE, EntityType::IF, EntityType::PRINT, EntityType::READ } },
    { RelationshipType::CALLS, { EntityType::PROC } },
    { RelationshipType::NEXT, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
                EntityType::IF, EntityType::WHILE } },
    { RelationshipType::NEXTBIP, { EntityType::PROGLINE, EntityType::ASSIGN, EntityType::STMT, EntityType::CALL, EntityType::PRINT, EntityType::READ,
    EntityType::IF, EntityType::WHILE } },
    {RelationshipType::AFFECTS, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
    {RelationshipType::AFFECTSBIP, { EntityType::STMT, EntityType::ASSIGN, EntityType::PROGLINE }},
};

std::unordered_map<PatternType, std::set<EntityType>> EntitiesTable::patternArgTable =
{
    {PatternType::PATTERN_ASSIGN, { EntityType::VAR }},
    {PatternType::PATTERN_IF, { EntityType::VAR }},
    {PatternType::PATTERN_WHILE, { EntityType::VAR }},
};

const std::string STMTNUM_ATTR = "stmt#";
const std::string VALUE_ATTR = "value";
const std::string PROCNAME_ATTR = "procName";
const std::string VARNAME_ATTR = "varName";

std::unordered_map<EntityType, std::set<std::string>> EntitiesTable::entityTypeToValidAttrTable =
{
    {EntityType::ASSIGN, {STMTNUM_ATTR}},
    {EntityType::STMT, {STMTNUM_ATTR}},
    {EntityType::IF, {STMTNUM_ATTR}},
    {EntityType::WHILE, {STMTNUM_ATTR}},
    {EntityType::PROGLINE, {}},
    {EntityType::CONST, {VALUE_ATTR}},
    {EntityType::PROC, {PROCNAME_ATTR}},
    {EntityType::VAR, {VARNAME_ATTR}},
    {EntityType::PRINT, {STMTNUM_ATTR, VARNAME_ATTR}},
    {EntityType::READ, {STMTNUM_ATTR, VARNAME_ATTR}},
    {EntityType::CALL, {STMTNUM_ATTR, PROCNAME_ATTR}},
};

std::unordered_map<EntityType, std::string> EntitiesTable::entityTypeSecondaryAttrTable =
{
    {EntityType::PRINT, VARNAME_ATTR},
    {EntityType::READ, VARNAME_ATTR},
    {EntityType::CALL, PROCNAME_ATTR},
};

std::unordered_map<EntityType, QueryInputType> EntitiesTable::primaryAttributeToQueryInputTypeTable =
{
    {EntityType::ASSIGN, QueryInputType::STMT_NUM},
    {EntityType::STMT, QueryInputType::STMT_NUM},
    {EntityType::IF, QueryInputType::STMT_NUM},
    {EntityType::WHILE, QueryInputType::STMT_NUM},
    {EntityType::PROGLINE, QueryInputType::STMT_NUM},
    {EntityType::CONST, QueryInputType::STMT_NUM},
    {EntityType::PROC, QueryInputType::IDENT},
    {EntityType::VAR, QueryInputType::IDENT},
    {EntityType::PRINT, QueryInputType::STMT_NUM},
    {EntityType::READ, QueryInputType::STMT_NUM},
    {EntityType::CALL, QueryInputType::STMT_NUM},
};

std::unordered_map<EntityType, QueryInputType> EntitiesTable::secondaryAttributeToQueryInputTypeTable =
{
    {EntityType::PRINT, QueryInputType::IDENT},
    {EntityType::READ, QueryInputType::IDENT},
    {EntityType::CALL, QueryInputType::IDENT},
};

std::unordered_map<EntityType, std::set<EntityType>> EntitiesTable::entityTypeToValidAttrCompareEntities =
{
    {EntityType::ASSIGN, {EntityType::ASSIGN, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
    {EntityType::STMT, {EntityType::ASSIGN, EntityType::STMT, EntityType::PROGLINE, EntityType::CALL, EntityType::CONST,
        EntityType::IF, EntityType::WHILE, EntityType::PRINT, EntityType::READ}},
    {EntityType::IF, {EntityType::IF, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
    {EntityType::WHILE, {EntityType::WHILE, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
    {EntityType::PROGLINE, {EntityType::ASSIGN, EntityType::STMT, EntityType::PROGLINE, EntityType::CALL, EntityType::CONST,
        EntityType::IF, EntityType::WHILE, EntityType::PRINT, EntityType::READ}},
    {EntityType::CONST, {EntityType::ASSIGN, EntityType::STMT, EntityType::PROGLINE, EntityType::CALL, EntityType::CONST,
        EntityType::IF, EntityType::WHILE, EntityType::PRINT, EntityType::READ}},
    {EntityType::PRINT, {EntityType::PRINT, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
    {EntityType::READ, {EntityType::READ, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
    {EntityType::CALL, {EntityType::CALL, EntityType::STMT, EntityType::PROGLINE, EntityType::CONST}},
};

std::set<EntityType> EntitiesTable::withClauseAllowedSynonymTypes = { EntityType::PROGLINE };

std::set<EntityType> EntitiesTable::patternClauseAllowedSynonymTypes =
{
    EntityType::ASSIGN, EntityType::WHILE, EntityType::IF
};


std::set<EntityType> EntitiesTable::getRelAllowedLeftEntities(RelationshipType relationshipType)
{
    return relRefLeftArgTable.at(relationshipType);
}

std::set<EntityType> EntitiesTable::getRelAllowedRightEntities(RelationshipType relationshipType)
{
    return relRefRightArgTable.at(relationshipType);
}

std::set<EntityType> EntitiesTable::getPatternAllowedLeftEntities(PatternType patternType)
{
    return patternArgTable.at(patternType);
}

std::set<std::string> EntitiesTable::getValidSynonymAttrs(EntityType entityType)
{
    return entityTypeToValidAttrTable.at(entityType);
}

bool EntitiesTable::isSecondaryAttr(EntityType entityType, std::string attrName)
{
    return entityTypeSecondaryAttrTable.find(entityType) != entityTypeSecondaryAttrTable.end() &&
        attrName == entityTypeSecondaryAttrTable.at(entityType);
}

QueryInputType EntitiesTable::getAttrRefType(std::shared_ptr<Declaration> declaration)
{
    if (declaration->getIsAttribute()) {
        return secondaryAttributeToQueryInputTypeTable.at(declaration->getEntityType());
    }
    else {
        return primaryAttributeToQueryInputTypeTable.at(declaration->getEntityType());
    }
}

std::set<EntityType> EntitiesTable::getEntityTypeValidAttrCompareEntities(EntityType entityType)
{
    return entityTypeToValidAttrCompareEntities.at(entityType);
}

std::set<EntityType> EntitiesTable::getWithClauseAllowedSynonymTypes()
{
    return withClauseAllowedSynonymTypes;
}

std::set<EntityType> EntitiesTable::getPatternClauseAllowedSynonymTypes()
{
    return patternClauseAllowedSynonymTypes;
}