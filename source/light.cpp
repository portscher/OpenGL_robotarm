#include "light.hpp"

Light::Light(LightSettings settings)
{
    this->settings = settings;
}

void Light::Update(KeyboardState* keyboard)
{
    if (keyboard->lightUp)
    {
        this->LightUp(keyboard);
    }

    if (keyboard->lightDown)
    {
        this->LightDown(keyboard);
    }
}

void Light::LightUp(KeyboardState* keyboard)
{
    double factor = 0.2;
    if (keyboard->lightMode == 0)
    {
        this->settings.ambient += factor;
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse += factor;
    }

    if (keyboard->lightMode == 2)
    {
        this->settings.specular += factor;
    }
}

void Light::LightDown(KeyboardState* keyboard)
{
    double factor = 0.2;
    if (keyboard->lightMode == 0)
    {
        this->settings.ambient -= factor;
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse -= factor;
    }

    if (keyboard->lightMode == 2)
    {
        this->settings.specular -= factor;
    }
}

void Light::Initialize(GLuint shaderProgram)
{
    BindUniform1f("AmbientFactor", shaderProgram, this->settings.ambient);
    BindUniform1f("DiffuseFactor", shaderProgram, this->settings.diffuse);
    BindUniform1f("SpecularFactor", shaderProgram, this->settings.specular);    
}







