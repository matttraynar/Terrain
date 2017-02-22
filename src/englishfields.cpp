#include "englishfields.h"

EnglishFields::EnglishFields()
{

}

EnglishFields::EnglishFields(const std::vector<std::vector<float> > &_terrainHeightMap)
{
    //Point the local shared pointer to the terrain height map reference, looks horrible
    //but thats because the pointer type is vector<vector<float>>
    m_heightMapCopy = std::make_shared< std::vector< std::vector<float> > >(_terrainHeightMap);

    checkAvailableSpace();
}

EnglishFields::~EnglishFields()
{

}

void EnglishFields::checkAvailableSpace()
{

}

void EnglishFields::threeFieldModel()
{

}


