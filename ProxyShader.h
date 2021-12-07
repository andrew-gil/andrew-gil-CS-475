#ifndef ProxyShader_DEFINED
#define ProxyShader_DEFINED

#include <math.h>
#include <vector>
#include <iostream>

#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GPoint.h"
#include "GShader.h"

class ProxyShader : public GShader {
    public:
    ProxyShader(GPoint verts[], GPoint texs[], GShader* otherShader): osh(otherShader) {
        texsMatrix = GMatrix(texs[1].x()-texs[0].x(),texs[2].x()-texs[0].x(),texs[0].x(),texs[1].y()-texs[0].y(),texs[2].y()-texs[0].y(),texs[0].y());
        vertsMatrix = GMatrix(verts[1].x()-verts[0].x(),verts[2].x()-verts[0].x(),verts[0].x(),verts[1].y()-verts[0].y(),verts[2].y()-verts[0].y(),verts[0].y());
    }
    
    bool isOpaque() override {
        return osh->isOpaque();
    }
    bool setContext(const GMatrix& ctm) override {
        texsMatrix.invert(&texsInverse);
        return osh->setContext(ctm * (vertsMatrix * texsInverse));
    }
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        osh->shadeRow(x,y,count,row); 
    }
    private:
    GShader* osh;
    GMatrix texsMatrix, vertsMatrix, texsInverse;
};

#endif