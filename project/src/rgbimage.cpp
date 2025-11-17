#include "rgbimage.h"


RGBImage::RGBImage( unsigned int Width, unsigned int Height) : m_Width(Width), m_Height(Height) {
	m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage() {
	delete[] m_Image;
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c) {
	if (x >= m_Width || y >= m_Height) { // bei einer Breite von 800 wird von 0 bis 799 gezählt. Daher >=
		return;
	}
	/*
	* Die Pixel werden Zeilenweise abgespeichert. Das bedeutet, dass die Pixel in der ersten Zeile von links nach rechts abgespeichert werden, dann die Pixel in der zweiten Zeile von links nach rechts usw.
	* Daraus folgt, dass der Index des Pixels in der Zeile y und Spalte x der Index des Pixels im Array m_Image ist.
	*/ 
	m_Image[y * m_Width + x] = c;
}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const {
	if (x >= m_Width || y >= m_Height) {
		return m_Image[0]; // muss noch verbessert werden.
	}
	return m_Image[y * m_Width + x];
}

unsigned int RGBImage::width() const {
	return m_Width;
}
unsigned int RGBImage::height() const {
	return m_Height;
}

unsigned char RGBImage::convertColorChannel( float v) {
	if (v < 0.0f) {
		return 0;
	}
	if (v > 1.0f) {
		return 255;
	}
	return (unsigned char)(v * 255.0f);
}

// Quelle: https://github.com/Nelson-Morais/HS-Computergrafik/blob/main/Praktikum01_SimpleRayTracing/src/rgbimage.cpp
// und ChatGPT für die Erklärung
bool RGBImage::saveToDisk(const char* Filename) {
    FILE* file = fopen(Filename, "wb");
    if (file == NULL) {
        return false;
    }

    int imgSize = 3 * m_Height * m_Width;
    int fileSize = 54 + imgSize;

    unsigned char bmpheader[54];
    //BMPFILEHEADER//
    //Magic
    bmpheader[0] = 'B';
    bmpheader[1] = 'M';
    //bfSize 4Bytes: gesamten file size, imagsize = 3* hohe *breite // filesize = 54 + imagsize
    bmpheader[2] = (unsigned char)(fileSize);
    bmpheader[3] = (unsigned char)(fileSize >> 8);
    bmpheader[4] = (unsigned char)(fileSize >> 16);
    bmpheader[5] = (unsigned char)(fileSize >> 24);
    //reserved 4Bytes
    bmpheader[6] = 0;
    bmpheader[7] = 0;
    bmpheader[8] = 0;
    bmpheader[9] = 0;
    //bfOffBits 4Bytes: kann auch NICHT 54 sein (40+14 / info+file headers)
    bmpheader[10] = 54;
    bmpheader[11] = 0;
    bmpheader[12] = 0;
    bmpheader[13] = 0;
    //BMPINFOHEADER//
    //biSize: info header size
    bmpheader[14] = 40;
    bmpheader[15] = 0;
    bmpheader[16] = 0;
    bmpheader[17] = 0;
    //biWidt: Breite der BMP
    bmpheader[18] = (unsigned char)(this->m_Width);
    bmpheader[19] = (unsigned char)(this->m_Width >> 8);
    bmpheader[20] = (unsigned char)(this->m_Width >> 16);
    bmpheader[21] = (unsigned char)(this->m_Width >> 24);
    //biHeight: Höhe der BMP
    //positiv - bottom up, negativ - top down.
    bmpheader[22] = (unsigned char)(this->m_Height * -1);
    bmpheader[23] = (unsigned char)(this->m_Height * -1 >> 8);
    bmpheader[24] = (unsigned char)(this->m_Height * -1 >> 16);
    bmpheader[25] = (unsigned char)(this->m_Height * -1 >> 24);
    //biPlanes: anzahl an farbe "planes", standard 1, (wird nicht bei BMP verwendet ?)
    bmpheader[26] = 1;
    bmpheader[27] = 0;
    //biBitCount: Color bit depth - 1,4,8,16,24,32
    bmpheader[28] = 24;
    bmpheader[29] = 0;
    //biCompression: 0-3 ( 0 (BI_RGB) kein komprimierung, 1 (BI_RLE8), 2 (BI_RLE4), 3 (BI_BITFIELDS))
    bmpheader[30] = 0;
    bmpheader[31] = 0;
    bmpheader[32] = 0;
    bmpheader[33] = 0;
    //biSizeImage: bei BI_RGB compression entweder 0 oder Größe der Bilddaten in Byte. 
    bmpheader[34] = (unsigned char)(imgSize);
    bmpheader[35] = (unsigned char)(imgSize >> 8);
    bmpheader[36] = (unsigned char)(imgSize >> 16);
    bmpheader[37] = (unsigned char)(imgSize >> 24);
    //biXPelsPerMeter: horizontal auflösung
    bmpheader[38] = 0;
    bmpheader[39] = 0;
    bmpheader[40] = 0;
    bmpheader[41] = 0;
    //biYPelsPerMeter: Vertikal auflösung
    bmpheader[42] = 0;
    bmpheader[43] = 0;
    bmpheader[44] = 0;
    bmpheader[45] = 0;
    //biClrUsed: colors in pallete
    bmpheader[46] = 0;
    bmpheader[47] = 0;
    bmpheader[48] = 0;
    bmpheader[49] = 0;
    //biClrimportant: important colors
    bmpheader[50] = 0;
    bmpheader[51] = 0;
    bmpheader[52] = 0;
    bmpheader[53] = 0;

    fwrite(bmpheader, 1, 54, file);

    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            Color c = m_Image[(y * m_Width) + x];
            unsigned char R = convertColorChannel(c.R);
            unsigned char G = convertColorChannel(c.G);
            unsigned char B = convertColorChannel(c.B);
            fwrite(&B, 1, 1, file);
            fwrite(&G, 1, 1, file);
            fwrite(&R, 1, 1, file);
        }
    }

    fclose(file);
    std::cout << "saveImage" << std::endl;
    return true;
}

RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) {
    assert(dst.width() == src.width() && dst.height() == src.height());
    Matrix K = Matrix(-1, 0, 1, 0,
                      -2, 0, 2, 0,
                      -1, 0, 1, 0,
                       0, 0, 0, 0 );
    Matrix KTrans = K;
    KTrans.transpose();
    for(int y = 0; y < src.height(); ++y){
        for (int x = 0; x < src.width(); ++x) {
            Color colorU = Color();
            Color colorV = Color();
            for (int i = 0; i <= 2; ++i) {
                for (int j = 0; j <= 2; ++j) {
                    // U Berechnen
                    computeGradientComponent(colorU, factor, K, src, x, y, i, j);
                    
                    // V berechnen
                    computeGradientComponent(colorV, factor, KTrans, src, x, y, i, j);
                }
            }
            // S berechnen
            Color s = colorU * colorU + colorV * colorV;
            s.R = std::sqrt(s.R);
            s.G = std::sqrt(s.G);
            s.B = std::sqrt(s.B);
        
            dst.setPixelColor(x, y, s);
        }
    }
    return dst;
}

void RGBImage::computeGradientComponent(Color& c, float factor, const Matrix& matrix,const RGBImage& src ,int x, int y, int i, int j){
	int xn = x-i-1; //Laut Aufgabenblatt muss das so sein. Davor war x+i-1
    int yn = y-j-1;
    if (yn >= 0 && yn < src.height() && xn >= 0 && xn < src.width()){
        Color pixel = src.getPixelColor(xn, yn);
        
        float weight = matrix.m[j*4 + i];
        
        c += pixel * weight * factor;
    }
    
}
// Bemerkung: So ist es effizienter als wenn man k[i+3] * k[j+3] macht. 
// Pro Pixel würde man sonst 49 Multiplikationen machen. Jetzt aber nur 14
RGBImage& RGBImage::GaussFilter(RGBImage& dst, const RGBImage& src, float factor) {
    assert(dst.width() == src.width() && dst.height() == src.height());
    float k[7] = {0.006f, 0.061f, 0.242f, 0.383f, 0.242, 0.061f, 0.006f}; // Glockenkurve des Gauss Filters

	int width = src.width();
	int height = src.height();

	RGBImage temp(width, height);

	// Horizontaler Filter
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color sum;
            for (int i = -3; i <= 3; ++i) {
				int xi = x + i;
                if (xi >= 0 && xi < width) {
					sum += src.getPixelColor(xi, y) * k[i + 3];
                }
            }
			temp.setPixelColor(x, y, sum);
        }
    }

	// Vertikaler Filter
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color sum;
            for (int j = -3; j <= 3; ++j) {
                int yj = y + j;
                if (yj >= 0 && yj < height) {
                    sum += src.getPixelColor(x, yj) * k[j + 3];
                }
            }
            dst.setPixelColor(x, y, sum * factor);
        }
    }
	return dst;
}

