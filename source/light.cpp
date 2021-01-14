#include "light.hpp"

const int MaxValue = 10;
const int MinValue = 0;

const Vector red = Vector{1, 0, 0};
const Vector green = Vector{0, 1, 0};
const Vector blue = Vector{0, 0, 1};
const Vector yellow = Vector{1, 1, 0};
const Vector orange = Vector{1, 0.5, 0};

const Vector colors[5] = {
    red,
    green,
    blue,
    yellow,
    orange
};

/**
 * @brief Construct a new Light object
 *
 * @param settings The settings the light should have.
 */
Light::Light(LightSettings settings, Vector position, Vector color)
{
    this->settings = settings;
    this->colorCounter = 0;
    this->color = color;
    this->position = position;
}

/**
 * @brief Updates the light according to the pressed keys.
 *
 * @param shaderProgram The instance of the shader for the current program.
 * @param keyboard The current keyboard state.
 */
void Light::Update(KeyboardState* keyboard)
{
    if (!keyboard->moveLight)
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
 
    if (keyboard->moveLight)
    {
        this->MoveLight(keyboard);
        this->ChangeColor(keyboard);
    }

    if (keyboard->reset)
    {
        this->Reset();
    }
}

/**
 * @brief Moves the light according to the pressed keys.
 * @param keyboard The current keyboard state.
 */
void Light::MoveLight(KeyboardState* keyboard)
{
    float moveFactor = 0.1;
    if (keyboard->left)
    {
        this->position.x -= moveFactor;
    }

    if (keyboard->right)
    {
        this->position.x += moveFactor;
    }

    if (keyboard->up)
    {
        this->position.z += moveFactor;
    }

    if (keyboard->down)
    {
        this->position.z -= moveFactor;
    }
}

/**
 * @brief Changes the color of the light.  
 * @param keyboard The current keyboard state
 */
void Light::ChangeColor(KeyboardState* keyboard)
{
    if (colorCounter < 0)
    {
        colorCounter = 4;
    }

    if (colorCounter > 4){
        colorCounter = 0;
    }

    if (keyboard->lightUp)
    {
        colorCounter++;
    }

    if (keyboard->lightDown)
    {
        colorCounter--;
    }

    this->color = colors[colorCounter];
}

/**
 * @brief Resets the light to its default values.
 */
void Light::Reset()
{
    colorCounter = 0;
    this->color = colors[colorCounter];
    this->position = Vector{1.2, 1.0, 3.0};
    this->settings.ambient = 0.5;
    this->settings.diffuse = 0.2;
    this->settings.specular = 0.4;
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
    SetTranslation(this->position.x, this->position.y, this->position.z, pos);

    float scale[16];
    SetScaleMatrix(.1, .1, .1, scale);
    MultiplyMatrix(pos, scale, pos);
    BindUniform4f("Transform", shaderProgram, pos);

    BindUniform3f("Color", shaderProgram, this->color);

    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

    BindUniform3f("light.position", shaderProgram, this->position);
    BindUniform3f("light.color", shaderProgram, this->color);
}