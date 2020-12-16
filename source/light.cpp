#include "light.hpp"

Light::Light(LightSettings settings)
{
    this->settings = settings;
}

void Light::Update(GLuint shaderProgram, KeyboardState* keyboard)
{
    if (keyboard->lightUp)
    {
        this->LightUp(keyboard);
    }

    if (keyboard->lightDown)
    {
        this->LightDown(keyboard);
    }
    
    this->Initialize(shaderProgram);
}

void Light::LightUp(KeyboardState* keyboard)
{
    std::cout << keyboard->lightMode << std::endl;
    double factor = 0.2; 
    if (keyboard->lightMode == 0)
    {
        this->settings.ambient += factor;
        if (this->settings.ambient > 10)
        {
            this->settings.ambient = 10;
        }        
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse += factor;
        if (this->settings.diffuse > 10)
        {
            this->settings.diffuse = 10;
        }
    }

    if (keyboard->lightMode == 2)
    {
        
        this->settings.specular += factor;
        if (this->settings.specular > 10)
        {
            this->settings.specular = 10;
        }
    }
}

void Light::LightDown(KeyboardState* keyboard)
{
    double factor = 0.1;
    if (keyboard->lightMode == 0)
    {
        this->settings.ambient -= factor;
        if (this->settings.ambient < 0)
        {
            this->settings.ambient = 0;
        }
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse -= factor;
        if (this->settings.diffuse < 0)
        {
            this->settings.diffuse = 0;
        }
    }

    if (keyboard->lightMode == 2)
    {
        this->settings.specular -= factor;
        if (this->settings.specular < 0)
        {
            this->settings.specular = 0;
        }
    }
}

void Light::Initialize(GLuint shaderProgram)
{
    BindUniform1f("AmbientFactor", shaderProgram, this->settings.ambient);
    BindUniform1f("DiffuseFactor", shaderProgram, this->settings.diffuse);
    BindUniform1f("SpecularFactor", shaderProgram, this->settings.specular);    
}







