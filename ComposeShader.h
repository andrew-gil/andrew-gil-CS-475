#ifndef ComposeShader_DEFINED
#define ComposeShader_DEFINED

#include <math.h>
#include <vector>
#include <iostream>

#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GPoint.h"
#include "GShader.h"

class ComposeShader : public GShader {
    public:
    ComposeShader(GShader* s0, GShader* s1): s0(s0), s1(s1) {
    }
    bool isOpaque() override {
        return s0->isOpaque() && s1 -> isOpaque();
    }


    bool setContext(const GMatrix& ctm) override {
        return s0 -> setContext(ctm) && s1 -> setContext(ctm);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel temp[count+1];
        s0 -> shadeRow(x,y,count,row);
        s1 -> shadeRow(x,y,count,temp);
        float div = 1.0f/255.0f;
        for (int i = 0; i < count; i++) {
            GPixel p = GPixel_PackARGB(GRoundToInt(GPixel_GetA(row[i])*GPixel_GetA(temp[i])*div),
            GRoundToInt(GPixel_GetR(row[i])*GPixel_GetR(temp[i])*div), 
            GRoundToInt(GPixel_GetG(row[i])*GPixel_GetG(temp[i])*div),
            GRoundToInt(GPixel_GetB(row[i])*GPixel_GetB(temp[i])*div));
            row[i] = p;
        }
    }
    private:
    GShader* s0;
    GShader* s1;

};

#endif