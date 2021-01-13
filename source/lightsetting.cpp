#include "lightsetting.hpp"

/**
 * @brief Construct a new LightSettings object
 * 
 */
LightSettings::LightSettings()
    : ambient(0.2),
    diffuse(0.2),
    specular(0.4),
    position(Vector{0.0, 0.0, 0.0}),
    color(Vector{1.0, 0.8, 0.43})
{
}

/**
 * @brief Construct a new LightSettings object
 * 
 * @param ambient The value for the ambient effect.
 * @param diffuse The value for the diffuse effect.
 * @param specular The value for the specular effect.
 */
LightSettings::LightSettings(float ambient, float diffuse, float specular, Vector position, Vector color)
{
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->position = position;
    this->color = color;
}