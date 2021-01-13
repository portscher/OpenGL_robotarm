#ifndef LIGHT_SETTING_H
#define LIGHT_SETTING_H

#include "Vector.hpp"

using namespace std;

class LightSettings
{
    public:
        float ambient;
        float diffuse;
        float specular;
        Vector position;
        Vector color;
        LightSettings(float ambient, float diffuse, float specular, Vector position, Vector color);
        LightSettings();
};

#endif /* LIGHT_SETTING_H */