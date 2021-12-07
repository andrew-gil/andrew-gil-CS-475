#ifndef BitmapShader_DEFINED
#define BitmapShader_DEFINED

#include "GBitmap.h"
#include "GMatrix.h"
#include "GShader.h"
#include "GPixel.h"
#include <math.h>
#include <string>

class BitmapShader : public GShader {
    public:
    GBitmap sourceBitmap;
    GMatrix inverseMatrix;
    GMatrix localMatrix;
    TileMode tile;
    BitmapShader(const GBitmap& bitmap, const GMatrix& localM, GShader::TileMode fTile)
        : sourceBitmap(bitmap) 
        , localMatrix(localM)
        , tile(fTile) {}

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque() override {
        return sourceBitmap.isOpaque();
    };

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix& ctm) override {
        return (ctm * localMatrix).invert(&inverseMatrix);
    };

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPoint point = GPoint::Make(x+0.5f, y+0.5f);
        GPoint local = inverseMatrix * point;

        for (int i = 0; i < count; i++) {
            int sourceX = GFloorToInt(local.fX);
            int sourceY = GFloorToInt(local.fY);
            switch (tile) {
                case GShader::TileMode::kClamp: {
                    sourceX = std::max(0, std::min(sourceBitmap.width() - 1, sourceX));
                    sourceY = std::max(0, std::min(sourceBitmap.height() - 1, sourceY));

                }
                case GShader::TileMode::kRepeat: {
                    sourceX %= sourceBitmap.width();
                    if (sourceX < 0) {
                        sourceX += sourceBitmap.width();
                    }

                    sourceY %= sourceBitmap.height();
                    if (sourceY < 0) {
                        sourceY += sourceBitmap.height();
                    }
                }
                case GShader::TileMode::kMirror: {

                    sourceX %= 2*sourceBitmap.width();
                    if(sourceX > sourceBitmap.width()) {
                        sourceX = sourceBitmap.width()-1-sourceX;
                    }
                    if (sourceX < 0) {
                        sourceX += sourceBitmap.width();
                    }

                    sourceY %= 2*sourceBitmap.height();
                    if(sourceY > sourceBitmap.height()) {
                        sourceY = sourceBitmap.height()-1-sourceY;
                    }
                    if (sourceY < 0) {
                        sourceY += sourceBitmap.height();
                    }
                }
            }
            row[i] = *sourceBitmap.getAddr(
                std::max(0, std::min(sourceBitmap.width() - 1, sourceX)), 
                std::max(0, std::min(sourceBitmap.height() - 1, sourceY)) 
            );
            local.fX += inverseMatrix[GMatrix::SX];
            local.fY += inverseMatrix[GMatrix::KY];
        }
    }
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localM, GShader::TileMode fTile) {
    if (!bitmap.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new BitmapShader(bitmap, localM, fTile));
}

#endif