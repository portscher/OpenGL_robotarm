#include "lightsetting.hpp"

LightSettings::LightSettings()
    : ambient(0.2),
    diffuse(0.2),
    specular(0.4)
{
}

LightSettings::LightSettings(float ambient, float diffuse, float specular)
{
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}