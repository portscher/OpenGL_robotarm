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
        LightSettings(float ambient, float diffuse, float specular);
        LightSettings();
};

#endif /* LIGHT_SETTING_H */