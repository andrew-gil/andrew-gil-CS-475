#ifndef Final_DEFINED
#define Final_DEFINED

#include "GFinal.h"
#include <math.h>
#include <string>
#include <iostream>


class Final : public GFinal {
    public:
    Final() {

    }

    void addLine(GPath* path, GPoint p0, GPoint p1, float width, CapType cap) {
        //given a line segment two points, we need to construct a polygon (convex) that we can fill with a bunch of smaller lines 
        float x, y;
        x = p1.x() - p0.x();
        y = p1.y() - p0.y();
        float len = sqrt(x*x + y*y);
        float rad = width*0.5;
        float dx = x* rad / len;
        float dy = y * rad / len;
        GPoint newPts[4];
        newPts[0] = GPoint::Make(p0.x() + dy, p0.y() - dx);
        newPts[1] = GPoint::Make(p0.x() - dy, p0.y() + dx);
        newPts[3] = GPoint::Make(p1.x() + dy, p1.y() - dx);
        newPts[2] = GPoint::Make(p1.x() - dy, p1.y() + dx);
        if (cap == kButt) {
            path -> GPath::addPolygon(newPts, 4);
        } else if (cap == kSquare){
            //square cap extending width/2
            newPts[0].fX -= dx;
            newPts[0].fY -= dy;
            newPts[1].fX -= dx;
            newPts[1].fY -= dy;
            newPts[3].fX += dx;
            newPts[3].fY += dy;
            newPts[2].fX += dx;
            newPts[2].fY += dy;
            path -> GPath::addPolygon(newPts, 4);
        } else if (cap == kRound) {
            path -> GPath::addCircle(p0, rad, GPath::kCCW_Direction);
            path -> GPath::addCircle(p1, rad, GPath::kCCW_Direction);
            path -> GPath::addPolygon(newPts, 4);
        }
    }

    private:
};


std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new Final());
}

#endif