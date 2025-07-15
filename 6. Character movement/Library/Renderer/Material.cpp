// Material.cpp
#include "Material.h"

Material::Material()
    : pDiffuse(nullptr), pSpecular(nullptr),
    AmbientColor(0.0f, 0.0f, 0.0f),
    DiffuseColor(0.0f, 0.0f, 0.0f),
    SpecularColor(0.0f, 0.0f, 0.0f)
{
}
