#pragma once
#include <functional>
#include "glm/glm.hpp"


class Color {
public:
    
    Color(unsigned char r,unsigned char g,unsigned char b);


    /**
     * 
     * @return the color as floats from 0 to 1
     */
    glm::vec3 Normalized();
    /**
     * 
     * @return the color as integers from 0 to 255
     */
    glm::ivec3 AsIntegers();
    
    /**
     * Sets the color from rgb values normalized
     */
    void Set(float r,float g,float b);
    /**
     * Sets the color from rgb values from 0 to 255
     */
    void Set(unsigned char r,unsigned char g,unsigned char b);
    
    /**
     * A function to modify the color value as needed
     * @param  changeFunction -> a lambda or function to use for changing (take a glm::ivec3 as parameter for the color(0-255))
     */
    void Modify(std::function<void(glm::ivec3&)> changeFunction);
    
    static Color White;
    static Color Black;
    static Color Red;
    static Color Green;
    static Color Blue;
    

private:
    glm::ivec3 m_ColorValue;


};