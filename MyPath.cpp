#ifndef MyPath_DEFINED
#define MyPath_DEFINED

#include "GPath.h"
#include "GRect.h"
#include "GPoint.h"
#include "GMatrix.h"
#include <math.h>
#include <vector>


void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    dst[0] = src[0];
    dst[1] = GPoint::Make(((1.0f-t)*src[0].x() + t*src[1].x()), (1.0f-t)*src[0].y() + t*src[1].y());
    dst[3] = GPoint::Make((1.0f-t)*src[1].x() + t*src[2].x(), (1.0f-t)*src[1].y() + t*src[2].y());
    dst[4] = src[2];
    dst[2] = GPoint::Make(((1.0f-t)*dst[1].x() + t*dst[3].x()), (1.0f-t)*dst[1].y() + t*dst[3].y());

}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    dst[0] = src[0];
    dst[1] = GPoint::Make(((1.0f-t)*src[0].x() + t*src[1].x()), (1.0f-t)*src[0].y() + t*src[1].y());
    GPoint point1 = GPoint::Make(((1.0f-t)*src[1].x() + t*src[2].x()), (1.0f-t)*src[1].y() + t*src[2].y());
    dst[5] = GPoint::Make(((1.0f-t)*src[2].x() + t*src[3].x()), (1.0f-t)*src[2].y() + t*src[3].y());
    dst[6] = src[3];
    dst[2] = GPoint::Make(((1.0f-t)*dst[1].x() + t*point1.x()), (1.0f-t)*dst[1].y() + t*point1.y());
    dst[4] = GPoint::Make(((1.0f-t)*point1.x() + t*dst[5].x()), (1.0f-t)*point1.y() + t*dst[5].y());
    dst[3] = GPoint::Make(((1.0f-t)*dst[2].x() + t*dst[4].x()), (1.0f-t)*dst[2].y() + t*dst[4].y());
}


GPath& GPath::addCircle(GPoint center, float radius, Direction direction) {
    float tanSegment = tan(M_PI/8) * radius;
    float cosEqualsin = (sqrt(2)/2) * radius;
    GPoint points[] = {
        GPoint::Make(center.x() + radius, center.y()),
        GPoint::Make(center.x() + radius, center.y() + tanSegment),
        GPoint::Make(center.x() + cosEqualsin, center.y() + cosEqualsin),
        GPoint::Make(center.x() + tanSegment, center.y() + radius),
        GPoint::Make(center.x(), center.y() + radius),
        GPoint::Make(center.x() - tanSegment, center.y() + radius),
        GPoint::Make(center.x() - cosEqualsin, center.y() + cosEqualsin),
        GPoint::Make(center.x() - radius, center.y() + tanSegment),
        GPoint::Make(center.x() - radius, center.y()),
        GPoint::Make(center.x() - radius, center.y() - tanSegment),
        GPoint::Make(center.x() - cosEqualsin, center.y() - cosEqualsin),
        GPoint::Make(center.x() - tanSegment, center.y() - radius),
        GPoint::Make(center.x(), center.y() - radius),
        GPoint::Make(center.x() + tanSegment, center.y() - radius),
        GPoint::Make(center.x() + cosEqualsin, center.y() - cosEqualsin),
        GPoint::Make(center.x() + radius, center.y() - tanSegment),
        GPoint::Make(center.x() + radius, center.y())
    };
    moveTo(points[0]);
    if (direction==kCCW_Direction){
        for (int i = 1; i <= 16; i=i+2){
            quadTo(points[i], points[i+1]);
        }
    }
    if (direction == kCW_Direction) {
        for (int i = 15; i >= 0; i=i-2){
            quadTo(points[i], points[i-1]);
        }
    }

  return *this;
}
/**
GPath& GPath::addCircle(GPoint center, float radius, Direction direction) {
    //calculate the circle by finding the a,b,c for each quad, 1/8 of the circle
    GPoint quad0[3] = {
        GPoint::Make(center.x()+radius,center.y()),
        GPoint::Make(center.x()+radius,center.y()+radius*tan(M_PI*0.125)),
        GPoint::Make(center.x()+radius*cos(M_PI*0.25),center.y()+radius*sin(M_PI*0.25))};
    GPoint quad1[3] = {
        GPoint::Make(center.x()+radius*cos(M_PI*0.25),center.y()+radius*sin(M_PI*0.25)),
        GPoint::Make(center.x()+radius*tan(M_PI*0.125),center.y()+radius),
        GPoint::Make(center.x(),center.y()+radius)};
    GPoint quad2[3] = {
        GPoint::Make(center.x(),center.y()+radius),
        GPoint::Make(center.x()-radius*tan(M_PI*0.125),center.y()+radius),
        GPoint::Make(center.x()-radius*cos(M_PI*0.25),center.y()+radius*sin(M_PI*0.25))};
    GPoint quad3[3] = {
        GPoint::Make(center.x()-radius*cos(M_PI*0.25),center.y()-radius*sin(M_PI*0.25)),
        GPoint::Make(center.x()-radius,center.y()+radius*tan(M_PI*0.125)),
        GPoint::Make(center.x()-radius,center.y())};
    GPoint quad4[3] = {
        GPoint::Make(center.x()-radius,center.y()),
        GPoint::Make(center.x()-radius,center.y()-radius*tan(M_PI*0.125)),
        GPoint::Make(center.x()-radius*cos(M_PI*0.25),center.y()-radius*sin(M_PI*0.25))};
    GPoint quad5[3] = {
        GPoint::Make(center.x()-radius*cos(M_PI*0.25),center.y()-radius*sin(M_PI*0.25)),
        GPoint::Make(center.x()-radius*tan(M_PI*0.125),center.y()-radius),
        GPoint::Make(center.x(),center.y()-radius)};
    GPoint quad6[3] = {
        GPoint::Make(center.x(),center.y()-radius),
        GPoint::Make(center.x()+radius*tan(M_PI*0.125),center.y()-radius),
        GPoint::Make(center.x()+radius*cos(M_PI*0.25),center.y()-radius*sin(M_PI*0.25))};
    GPoint quad7[3] = {
        GPoint::Make(center.x()+radius*cos(M_PI*0.25),center.y()-radius*sin(M_PI*0.25)),
        GPoint::Make(center.x()+radius,center.y()-radius*tan(M_PI*0.125)),
        GPoint::Make(center.x()+radius,center.y())};
    moveTo(quad0[0]);
    if (direction ==kCCW_Direction){
        quadTo(quad0[1],quad0[2]);
        quadTo(quad1[1],quad1[2]);
        quadTo(quad2[1],quad2[2]);
        quadTo(quad3[1],quad3[2]);
        quadTo(quad4[1],quad4[2]);
        quadTo(quad5[1],quad5[2]);
        quadTo(quad6[1],quad6[2]);
        quadTo(quad7[1],quad7[2]);
    }
    if (direction == kCW_Direction) {
        quadTo(quad7[1],quad7[0]);
        quadTo(quad6[1],quad6[0]);
        quadTo(quad5[1],quad5[0]);
        quadTo(quad4[1],quad4[0]);
        quadTo(quad3[1],quad3[0]);
        quadTo(quad2[1],quad2[0]);
        quadTo(quad1[1],quad1[0]);
        quadTo(quad0[1],quad0[0]);

    }


    return *this;
}
*/

GPath& GPath::addRect(const GRect& rect, Direction dir) {
    this->moveTo(GPoint::Make(rect.left(), rect.top()));

    if (dir == Direction::kCW_Direction) {
        this->lineTo(GPoint::Make(rect.right(), rect.top()));
        this->lineTo(GPoint::Make(rect.right(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.left(), rect.bottom()));
    } else {
        this->lineTo(GPoint::Make(rect.left(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.right(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.right(), rect.top()));
    }

    return *this;
}


GPath& GPath::addPolygon(const GPoint pts[], int count) {
    //if count < 3, return *this
    if (count < 3){return *this;}
    moveTo(pts[0]);
    for (int i = 1; i < count; i++) {
        lineTo(pts[i]);
    }
    return *this;
}

GRect GPath::bounds() const{
    int count =  fPts.size();
    if (count == 0) {
        return GRect::MakeLTRB(0,0,0,0);
    }
    float left =  fPts.at(0).x();
    float top =  fPts.at(0).y();
    float right =  fPts.at(0).x();
    float bottom =  fPts.at(0).y();
    for (int i = 1; i < count; i++) {
        left = std::min(left,  fPts.at(i).x());
        top = std::min(top,  fPts.at(i).y());
        right = std::max(right,  fPts.at(i).x());
        bottom = std::max(bottom,  fPts.at(i).y());
    }
    return GRect::MakeLTRB(left, top, right, bottom);

}
void GPath::transform(const GMatrix&matrix) {
    matrix.mapPoints(this->fPts.data(), this->fPts.data(), this->fPts.size());
}

#endif