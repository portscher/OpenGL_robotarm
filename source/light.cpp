#include "light.hpp"

/**
 * @brief Construct a new Light:: Light object
 * 
 * @param settings The settings the light should have.
 */
Light::Light(LightSettings settings)
{
    this->settings = settings;
}

/**
 * @brief Updates the light according to the pressed keys.
 * 
 * @param shaderProgram The instance of the shader for the current program.
 * @param keyboard The current keyboard state.
 */
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
    
    this->UpdateShader(shaderProgram);
}

/**
 * @brief Amplifies the light of the current selected effect.
 * 
 * @param keyboard The current state of the keyboard.
 */
void Light::LightUp(KeyboardState* keyboard)
{
    double factor = 0.1; 
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

/**
 * @brief Damplifies the light of the current selected effect.
 * 
 * @param keyboard The current state of the keyboard.
 */
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

/**
 * @brief Updates the shader of the current program with the current light settings.
 * 
 * @param shaderProgram The shader of the current program.
 */
void Light::UpdateShader(GLuint shaderProgram)
{
    BindUniform1f("AmbientFactor", shaderProgram, this->settings.ambient);
    BindUniform1f("DiffuseFactor", shaderProgram, this->settings.diffuse);
    BindUniform1f("SpecularFactor", shaderProgram, this->settings.specular);    
}
