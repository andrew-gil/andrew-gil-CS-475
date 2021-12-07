#ifndef Matrix_DEFINED
#define Matrix_DEFINED

#include <math.h>
#include "GMatrix.h"
#include "GPoint.h"

GMatrix::GMatrix() {
    fMat[0] = 1;    fMat[1] = 0;    fMat[2] = 0;
    fMat[3] = 0;    fMat[4] = 1;    fMat[5] = 0;
}

GMatrix GMatrix::Translate(float tx, float ty) {
    GMatrix m = GMatrix();
    m[2] = tx;
    m[5] = ty;
    return m;
}

GMatrix GMatrix::Scale(float sx, float sy) {
    GMatrix m = GMatrix();
    m[0] = sx;
    m[4] = sy;
    return m;
}

GMatrix GMatrix::Rotate(float radians) {
    GMatrix m = GMatrix(
        cos(radians), -sin(radians), 0,
        sin(radians), cos(radians), 0
    );
    return m;
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    GMatrix m = GMatrix(
        b[0] * a[0] + b[3] * a[1],
        b[1] * a[0] + b[4] * a[1],
        b[2] * a[0] + b[5] * a[1] + a[2],
        b[0] * a[3] + b[3] * a[4],
        b[1] * a[3] + b[4] * a[4],
        b[2] * a[3] + b[5] * a[4] + a[5]);
    return m;
}


bool GMatrix::invert(GMatrix* inverse) const{
    float a = this->fMat[0];
    float b = this->fMat[1];
    float c = this->fMat[2];
    float d = this->fMat[3];
    float e = this->fMat[4];
    float f = this->fMat[5];
    float determinant = a*e-b*d;
    if (determinant == 0) {
        return false;
    }
    float inverseDeterminant = 1 / determinant;
    inverse->fMat[0] = e * inverseDeterminant;
    inverse->fMat[1] = -b * inverseDeterminant;
    inverse->fMat[2] = -(c * e - b * f) * inverseDeterminant;
    inverse->fMat[3] = -d * inverseDeterminant;
    inverse->fMat[4] = a * inverseDeterminant;
    inverse->fMat[5] = (c * d - a * f) * inverseDeterminant;
    return true;
}
void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; i++) {
        GPoint point = src[i];

        float xPrime = point.x();
        float yPrime = point.y();

        float x = this->fMat[GMatrix::SX] * xPrime + this->fMat[GMatrix::KX] * yPrime + this->fMat[GMatrix::TX];
        float y = this->fMat[GMatrix::SY] * yPrime + this->fMat[GMatrix::KY] * xPrime + this->fMat[GMatrix::TY];

        dst[i].set(x, y);
    }
}
#endif