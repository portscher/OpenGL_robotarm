#include "light.hpp"

const int MaxValue = 10;
const int MinValue = 0;

/**
 * @brief Construct a new Light object
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
        if (this->settings.ambient > MaxValue)
        {
            this->settings.ambient = MaxValue;
        }        
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse += factor;
        if (this->settings.diffuse > MaxValue)
        {
            this->settings.diffuse = MaxValue;
        }
    }

    if (keyboard->lightMode == 2)
    {
        
        this->settings.specular += factor;
        if (this->settings.specular > MaxValue)
        {
            this->settings.specular = MaxValue;
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
        if (this->settings.ambient < MinValue)
        {
            this->settings.ambient = MinValue;
        }
    }

    if (keyboard->lightMode == 1)
    {
        this->settings.diffuse -= factor;
        if (this->settings.diffuse < MinValue)
        {
            this->settings.diffuse = MinValue;
        }
    }

    if (keyboard->lightMode == 2)
    {
        this->settings.specular -= factor;
        if (this->settings.specular < MinValue)
        {
            this->settings.specular = MinValue;
        }
    }
}

/**
 * @brief Updates the shader of the current program with the current light settings.
 * 
 * @param shaderProgram The shader of the current program.
 */
void Light::LightUpScene(GLuint shaderProgram)
{
    BindUniform1f("AmbientFactor", shaderProgram, this->settings.ambient);
    BindUniform1f("DiffuseFactor", shaderProgram, this->settings.diffuse);
    BindUniform1f("SpecularFactor", shaderProgram, this->settings.specular);

    int size = BindBasics(this->VBO, this->CBO, this->IBO, 0, 0);

    // create light position matrix and resize
    float pos[16];
    SetTranslation(this->settings.position.x, this->settings.position.y, this->settings.position.z, pos);
    
    float scale[16];
    SetScaleMatrix(.1, .1, .1, scale);
    MultiplyMatrix(pos, scale, pos);
    BindUniform4f("Transform", shaderProgram, pos);
    
    BindUniform3f("Color", shaderProgram, this->settings.color);
    
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

    BindUniform3f("light.position", shaderProgram, this->settings.position);
    BindUniform3f("light.color", shaderProgram, this->settings.color);
}
