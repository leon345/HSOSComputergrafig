#ifndef __SimpleRayTracer__rgbimage__
#define __SimpleRayTracer__rgbimage__

#include <iostream>
#include "Matrix.h"
#include "color.h"
#include "assert.h"
#include "math.h"
class Color;

class RGBImage
{
public:
    RGBImage( unsigned int Width, unsigned Height);
    ~RGBImage();
    void setPixelColor( unsigned int x, unsigned int y, const Color& c);
    const Color& getPixelColor( unsigned int x, unsigned int y) const;
    bool saveToDisk( const char* Filename);
    unsigned int width() const;
    unsigned int height() const;
    
    static unsigned char convertColorChannel( float f);
	static RGBImage& SobelFilter(RGBImage& dst, const RGBImage& src, float factor=1.0f);
	static RGBImage& GaussFilter(RGBImage& dst, const RGBImage& src, float factor = 1.0f);
    
protected:
    Color* m_Image;
    unsigned int m_Height;
    unsigned int m_Width;
    static void computeGradientComponent(Color& c, float factor, const Matrix& matrix,const RGBImage& src ,int x, int y, int i, int j);
    
    
};

#endif /* defined(__SimpleRayTracer__rgbimage__) */
