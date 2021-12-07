#ifndef MyLinearGradient_DEFINED
#define MyLinearGradient_DEFINED

#include <math.h>
#include <vector>
#include <iostream>

#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GPoint.h"
#include "GShader.h"


class MyLinearGradient : public GShader {
public:
    MyLinearGradient(GPoint a, GPoint b, const GColor colors[], int count, TileMode fTile) {
        if (a.x() > b.x()) {
            std::swap(a, b);
        }
        
        for (int i = 0; (i < count); i++) {
            colorsList.push_back(colors[i%count]);
        }
        colorsList.push_back(colors[count-1]);
        colorCount = count;
        tile = fTile;
        float dx = b.x() - a.x();
        float dy = b.y() - a.y();
        localMatrix = GMatrix(
            dx, -dy, a.x(), 
            dy, dx, a.x());
    }

    bool isOpaque() override {
        for (GColor color: colorsList) {
            if (color.a != 1.0f) {
                return false;
            }
        }
        return true;
    }

    bool setContext(const GMatrix& ctm) override {
        return (ctm * localMatrix).invert(&inverseMatrix);
    }


    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; i++) {
            GPoint point = GPoint::Make(x+0.5f+i, y+0.5f);
            GPoint local = inverseMatrix * point;
            float source = local.x();
            switch (tile) {
                case GShader::TileMode::kClamp: {
                    source = std::max(0.0f, std::min(1.0f, source));
                }
                case GShader::TileMode::kRepeat: {
                    source -= GFloorToInt(source);
                }
                case GShader::TileMode::kMirror: {
                    source = source * .5;
                    source = source - GFloorToInt(source);
                    if (source > .5) {
                        source = 1 - source;
                    }
                    source = source * 2;
                }
            }
            source = std::max(0.0f, std::min(1.0f, source));
            int index = GFloorToInt(source * (colorCount - 1));
            GColor col0 = colorsList.at(index).pinToUnit();
            GColor col1 = colorsList.at(index + 1).pinToUnit();
            float t = source*(colorCount - 1) - index;
            float r = (1 - t) * col0.r + t * col1.r;
            float g = (1 - t) * col0.g + t * col1.g;
            float b = (1 - t) * col0.b + t * col1.b;
            float a = (1 - t) * col0.a + t * col1.a;
            GColor color = GColor::RGBA(r, g, b, a);
            row[i] = colorToPixel(color);
            local.fX += inverseMatrix[0];
        }
    }
private:

    std::vector<GColor> colorsList;
    int colorCount;
    GMatrix localMatrix;
    GMatrix inverseMatrix;
    TileMode tile;

    GPixel colorToPixel(const GColor& c) {
        GColor retc;
        GPinToUnit(c.a);
        retc.a = floor((c.a*255)+0.5);
        GPinToUnit(c.a*c.r);
        retc.r = floor(((c.a*c.r)*255)+0.5);
        GPinToUnit(c.a*c.g);
        retc.g = floor(((c.a*c.g)*255)+0.5);
        GPinToUnit(c.a*c.b);
        retc.b = floor(((c.a*c.b)*255)+0.5);
        return GPixel_PackARGB(retc.a, retc.r, retc.g, retc.b);
    }


};

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode fTile) {
    if (count < 1) {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new MyLinearGradient(p0, p1, colors, count, fTile));
}
#endif