#include "lightsetting.hpp"

/**
 * @brief Construct a new LightSettings object
 * 
 */
LightSettings::LightSettings()
    : ambient(0.2),
    diffuse(0.2),
    specular(0.4)
{
}

/**
 * @brief Construct a new LightSettings object
 * 
 * @param ambient The value for the ambient effect.
 * @param diffuse The value for the diffuse effect.
 * @param specular The value for the specular effect.
 */
LightSettings::LightSettings(float ambient, float diffuse, float specular)
{
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}