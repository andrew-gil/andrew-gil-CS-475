#ifndef TriColorShader_DEFINED
#define TriColorShader_DEFINED

#include <math.h>
#include <vector>
#include <iostream>

#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GPoint.h"
#include "GShader.h"

class TriColorShader : public GShader {
    public:
    TriColorShader(GPoint p[], GColor c[]) {
        p0 = p[0];
        p1 = p[1];
        p2 = p[2];
        c0 = c[0];
        c1 = c[1];
        c2 = c[2];
        GPoint a = p1-p0;
        GPoint b = p2-p0;
        localMatrix = GMatrix(a.x(), b.x(), p0.x(), a.y(), b.y(), p0.y());
    }
    
    bool isOpaque() override {
        return false;
    }

    bool setContext(const GMatrix& ctm) override {
        return (ctm * localMatrix).invert(&inverseMatrix);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GColor dc1 = c1-c0;
        GColor dc2 = c2-c0;
        GPoint point = GPoint::Make(x+0.5f, y+0.5f);
        GPoint local = inverseMatrix * point;
        float sourceX = local.x();
        float sourceY = local.y();
        GColor c = c0 + sourceX*dc1 + sourceY*dc2;
        GColor ddc1 = inverseMatrix[0]*dc1;
        GColor ddc2 = inverseMatrix[3]*dc2;
        GColor dc = ddc1 + ddc2;
        for (int i = 0; i < count; i++) {
            //c.a = std::max(0.0f, std::min(1.0f, c.a));
            //c.r = std::max(0.0f, std::min(1.0f, c.r));
            //c.g = std::max(0.0f, std::min(1.0f, c.g));
            //c.b = std::max(0.0f, std::min(1.0f, c.b));
            row[i] = colorToPixel(c);
            c += dc;
            dc1 += ddc1;
            dc2 += ddc2;
            dc = ddc2 + ddc1;
        }
    }
    private:
    GPoint p0, p1, p2;
    GColor c0, c1, c2;
    GMatrix localMatrix;
    GMatrix inverseMatrix;

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

#endif