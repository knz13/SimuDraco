#include "color.h"
#include "../global.h"



Color Color::White = Color(255,255,255);
Color Color::Black = Color(0,0,0);
Color Color::Red = Color(255,0,0);
Color Color::Green = Color(0,255,0);
Color Color::Blue = Color(0,0,255);

glm::ivec3 Color::AsIntegers() {
    return m_ColorValue;
}

glm::vec3 Color::Normalized() {
    return glm::vec3(m_ColorValue.x*255,m_ColorValue.y*255,m_ColorValue.z*255);
}

void Color::Set(unsigned char r, unsigned char g, unsigned char b) {
    m_ColorValue.x = r;
    m_ColorValue.y = g;
    m_ColorValue.z = b;
}

void Color::Set(float r, float g, float b) {
    m_ColorValue.x = r*255;
    m_ColorValue.y = g*255;
    m_ColorValue.z = b*255;
}

void Color::Modify(std::function<void(glm::ivec3&)> changeFunction) {
    changeFunction(m_ColorValue);
}
Color::Color(unsigned char r, unsigned char g, unsigned char b) {
    m_ColorValue.x = r;
    m_ColorValue.y = g;
    m_ColorValue.z = b;
}
