#ifndef MyCanvas_DEFINED
#define MyCanvas_DEFINED

#include "GTypes.h"
#include "GCanvas.h"
#include "GPixel.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GRect.h"
#include "GMath.h"
#include "GPaint.h"
#include "GBlendMode.h"
#include "MyBlend.h"
#include "GPoint.h"
#include "GShader.h"
#include "GMatrix.h"
#include "MyEdge.h"
#include "GPath.h"
#include "TriColorShader.h"
#include "ProxyShader.h"
#include "ComposeShader.h"
#include <assert.h> 
#include <stack>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>


GPixel coloredPixel(const GColor& c) {
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

class MyCanvas : public GCanvas {
public:
    const GBitmap map;
    std::stack<GMatrix> cTMQueue;
    MyCanvas(const GBitmap&fmap): map(fmap) {
        GMatrix idMatrix = GMatrix();
        cTMQueue.push(idMatrix);
    }
    
    /**
     *  Fill the entire canvas with the specified color, using SRC porter-duff mode.
     */

    void drawPaint(const GPaint&p) {
        // convert color to pixel

        // clear the canvas by replacing each pixel in the canvas with the bitmap pixels
        GColor color = p.getColor();
        GPixel src = coloredPixel(color);
        for (int i = 0; i < map.width(); i++) {
            for (int j = 0; j < map.height(); j++) {
                GPixel *pixel = map.getAddr(i,j);
                *pixel = src;
            }
        }
    };

    /**
     *  Fill the rectangle with the color, using SRC_OVER porter-duff mode.
     *
     *  The affected pixels are those whose centers are "contained" inside the rectangle:
     *      e.g. contained == center > min_edge && center <= max_edge
     *
     *  Any area in the rectangle that is outside of the bounds of the canvas is ignored.
     */
    void drawRect(const GRect& rect, const GPaint& paint) {
        GPoint points[4] = {
            GPoint::Make(rect.left(), rect.top()),
            GPoint::Make(rect.right(), rect.top()),
            GPoint::Make(rect.right(), rect.bottom()),
            GPoint::Make(rect.left(), rect.bottom())
        };

        drawConvexPolygon(points, 4, paint);
    }
    
    //draw at y from leftx to rightx in gpaint paint
    void blit(int y, int leftx, int rightx, const GPaint& p) {
        leftx = std::max(0, leftx);
        rightx = std::min(map.width(), rightx);
        BlendImp blendImp = getBlend_Modes(p.getBlendMode());
        GShader* shader = p.getShader();
        if (shader == nullptr) {
            GColor color = p.getColor().pinToUnit();
            GPixel source = coloredPixel(color);
            for (int x = leftx; x < rightx; x++) {
                GPixel* addr = map.getAddr(x, y);
                *addr = blendImp(source, *addr);
            }
        } else {
            if (!shader->setContext(cTMQueue.top())) {
                return;
            }
            int count = rightx - leftx;
            GPixel shaded[count];
            shader->shadeRow(leftx, y, count, shaded);

            for (int x = leftx; x < rightx; x++) {
                GPixel* addr = map.getAddr(x, y);
                *addr = blendImp(shaded[x - leftx], *addr);
            }
        }
    }

    void drawConvexPolygon(const GPoint points[], int count, const GPaint&p) {
        GRect bound= GRect::MakeWH(map.width(), map.height());
        GPoint dstPoints[count];
        cTMQueue.top().mapPoints(dstPoints, points, count);
        std::vector<Edge> edges;
        GPoint oldPoint = dstPoints[0];
        for (int i = 1; i <= count; i++) {
            GPoint nextPoint = dstPoints[i%count];
            clipEdge(oldPoint, nextPoint, bound, edges);
            oldPoint = nextPoint;
        }
        //sort edges from top to bottom, least to greatest
        std::sort(edges.begin(), edges.end(), lessThan);
        if (edges.size() < 2) {
            return;
        }
        int veryBottom = edges.back().bottom;
        //now, back of list is the top edge
        Edge left = edges.at(0);
        Edge right = edges.at(1);
        int currentPos = 2;
        float veryTop = left.top;
        for (int y = veryTop; y < veryBottom; y++) {
            bool edgeChange = false;
            if (y >= left.bottom) {
                left = edges.at(currentPos);
                currentPos++;
                edgeChange = true;
            }
            if (y >= right.bottom) {
                right = edges.at(currentPos);
                currentPos++;
                edgeChange = true;
            }
            if (edgeChange && !(left.starterx < right.starterx)) {
                std::swap(left, right);
            }

            float leftX = left.starterx;
            float rightX = right.starterx;
            left.starterx+=left.slope;
            right.starterx+=right.slope; 
            blit(y, GRoundToInt(leftX), GRoundToInt(rightX), p);
        } 
    };

        

    void drawPath(const GPath& path, const GPaint& paint) {
        GRect boundingRect= GRect::MakeWH(map.width(), map.height());
        GPath::Edger iter(path);
        std::vector<Edge> edges;
        for (;;) {
            GPoint pts[GPath::kMaxNextPoints];
            GPath::Verb verb = iter.next(pts);
            if (verb == GPath::kDone) {
                break;
            }
            switch (verb) {
                case GPath::kLine: {
                    GPoint newPts[2];
                    cTMQueue.top().mapPoints(newPts, pts, 2);
                    clipEdge( newPts[0], newPts[1],boundingRect, edges);
                    break;
                }
                case GPath::kQuad: {
                    GPoint newPts[3];
                    cTMQueue.top().mapPoints(newPts, pts, 3);
                    //calculate t so that tol = 0.25
                    float k = 4*((0.25 * (-1 * newPts[0].x() + 2 * newPts[1].x() - newPts[2].x()))*(0.25 * (-1 * newPts[0].x() + 2 * newPts[1].x() - newPts[2].x())) 
                                + (0.25 * (-1 * newPts[0].y() + 2 * newPts[1].y() - newPts[2].y()))*(0.25 * (-1 * newPts[0].y() + 2 * newPts[1].y() - newPts[2].y())));
                    int numSegments = GCeilToInt(sqrt(k));
                    GPoint lastPoint = newPts[0]; //for accuracy, use the original point
                    float invK = 1.0f/numSegments;
                    for (int i = 1; i < numSegments; i++) {
                        //given a, b, c, which is newPts, calculate p(t)
                        float t = i*invK;
                        GPoint newPoint = evaluateQuadCurve(newPts[0],newPts[1],newPts[2],t);
                        clipEdge(lastPoint, newPoint,boundingRect, edges);
                        lastPoint = newPoint;
                    }
                    clipEdge(lastPoint, newPts[2],boundingRect, edges);
                    break;
                }
                case GPath::kCubic: {
                    GPoint newPts[4];
                    cTMQueue.top().mapPoints(newPts, pts, 4);
                    float k0 = 4*((0.25 * (-1 * newPts[0].x() + 2 * newPts[1].x() - newPts[2].x()))*(0.25 * (-1 * newPts[0].x() + 2 * newPts[1].x() - newPts[2].x())) 
                                + (0.25 * (-1 * newPts[0].y() + 2 * newPts[1].y() - newPts[2].y()))*(0.25 * (-1 * newPts[0].y() + 2 * newPts[1].y() - newPts[2].y())));
                    float k1 = 4*((0.25 * (-1 * newPts[1].x() + 2 * newPts[2].x() - newPts[3].x()))*(0.25 * (-1 * newPts[1].x() + 2 * newPts[2].x() - newPts[3].x())) 
                                + (0.25 * (-1 * newPts[1].y() + 2 * newPts[2].y() - newPts[3].y()))*(0.25 * (-1 * newPts[1].y() + 2 * newPts[2].y() - newPts[3].y())));
                    float k = std::max(k0,k1);
                    int numSegments = GCeilToInt(sqrt(0.75*k));
                    GPoint lastPoint = newPts[0]; // use the original point for accuracy
                    // when dividing ints with ints, you need to let cpp compiler know you want a float division, so one number must be a float
                    float invK = 1.0f/numSegments;
                    for (int i = 1; i < numSegments; i++) {
                        //given a, b, c, d which is newPts, calculate p(t)
                        float t = i*invK;
                        GPoint newPoint = evaluateCubicCurve(newPts[0],newPts[1],newPts[2],newPts[3],t);
                        clipEdge(lastPoint, newPoint,boundingRect, edges);
                        lastPoint = newPoint;
                    }
                    clipEdge(lastPoint, newPts[3],boundingRect, edges);

                    break;
                }
            }
        }
        std::sort(edges.begin(), edges.end(), lessThan);
        if (edges.size() == 0) {
            return;
        }
        complexScan(edges, paint);
        return;
    }

    void complexScan(std::vector<Edge> edges, const GPaint& paint) {
        int count = edges.size();
        for (int y = edges[0].top; count > 0 && y < map.height(); y++) {
            int index = 0;
            int winding = 0;
            int leftX = 0;
            int rightX = 0;
            while (index < count && edges[index].top <= y) {
                Edge& edge = edges[index];
                if (winding == 0) {
                    leftX = GRoundToInt(edge.starterx);
                }
                winding += edge.winding;
                if (winding == 0) {
                    rightX = GRoundToInt(edge.starterx);
                    blit(y, leftX, rightX, paint);
                }
                if (edge.bottom - 1 <= y) {
                    edges.erase(edges.begin() + index);
                    count--;
                    continue;
                }
                edge.starterx += edge.slope;
                index++;
            }
            while (index < count && y+1 == edges[index].top) {
                index += 1;
            }
            std::sort(edges.begin(), edges.begin() + index, compareEdgesStarterX);
        }
    }

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                          int count, const int indices[], const GPaint&paint) {
        GPoint point0, point1, point2, texs0, texs1, texs2;
        GColor color0, color1, color2;
        int n = 0;
        //if there is colors but no texs, tricolorshader
        //if there is no colors but texs, proxyshader
        //if there is color and texs, composeshader
        for (int i = 0; i < count; i++) {
            point0 = verts[indices[n+0]];
            point1 = verts[indices[n+1]];
            point2 = verts[indices[n+2]];
            GPoint currentVerts[3] = {point0, point1, point2};
            if (colors) {
                color0 = colors[indices[n+0]];
                color1 = colors[indices[n+1]];
                color2 = colors[indices[n+2]];
                GColor currentColors[3] = {color0, color1, color2};
                TriColorShader *sT = new TriColorShader(currentVerts, currentColors);
                if (texs && paint.getShader() != nullptr) {
                    //composeshader
                    texs0 = texs[indices[n+0]];
                    texs1 = texs[indices[n+1]];
                    texs2 = texs[indices[n+2]];
                    GPoint currentTexs[3] = {texs0, texs1, texs2}; 
                    ProxyShader *sP =  new ProxyShader(currentVerts, currentTexs, paint.getShader());
                    ComposeShader *sC = new ComposeShader(sP, sT);
                    GPaint newPaint = GPaint(sC);
                    drawConvexPolygon(currentVerts, 3, newPaint);
                } else {
                    //tricolorshader
                    GPaint newPaint = GPaint(sT);
                    drawConvexPolygon(currentVerts, 3, newPaint);
                }
            } else if (texs && paint.getShader() != nullptr) {
                //proxyshader
                texs0 = texs[indices[n+0]];
                texs1 = texs[indices[n+1]];
                texs2 = texs[indices[n+2]];
                GPoint currentTexs[3] = {texs0, texs1, texs2}; 
                ProxyShader *sP =  new ProxyShader(currentVerts, currentTexs, paint.getShader());
                GPaint newPaint = GPaint(sP);
                drawConvexPolygon(currentVerts, 3, newPaint);
            }
            n+=3;
        }
    }

    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint&paint) {   
        int n = level+1;
        GPoint newVerts[4];
        GPoint newTexs[4];
        GColor newColors[4];
        GPoint *pT = nullptr;
        GColor *pC = nullptr;
        int indices[] = {0,1,2,1,3,2};
        //if no colors or texs, pass nullptr
        float div = 1.0f/((float) n);
        for (int y = 0; y < n; y++) {
            for (int x=0; x < n; x++) {
                float u = x*div;
                float v = y*div;
                //bilerp
                newVerts[0] = bilerp(verts, u, v);
                newVerts[1] = bilerp(verts, u + div, v);
                newVerts[2] = bilerp(verts, u, v + div);
                newVerts[3] = bilerp(verts, u+div, v+div);
                if (texs) {
                    newTexs[0] = bilerp(texs, u, v);
                    newTexs[1] = bilerp(texs, u+div, v);
                    newTexs[2] = bilerp(texs, u, v+div);
                    newTexs[3] = bilerp(texs, u+div, v+div);
                    pT = newTexs;
                } 
                if (colors) {
                    newColors[0] = bilerp(colors, u, v);
                    newColors[1] = bilerp(colors, u+div, v);
                    newColors[2] = bilerp(colors, u, v+div);
                    newColors[3] = bilerp(colors, u+div, v+div);
                    pC = newColors;
                }
                drawMesh(newVerts, pC, pT, 2, indices, paint);
            }
        }
        
    }

    const GColor bilerp(const GColor coords[4], float u, float v) {
        return (1-u)*(1-v)*coords[0] + u*(1-v)*coords[1] + u*v*coords[2] + (1-u)*v*coords[3];
    }
    const GPoint bilerp(const GPoint coords[4], float u, float v) {
        return (1-u)*(1-v)*coords[0] + u*(1-v)*coords[1] + u*v*coords[2] + (1-u)*v*coords[3];
    }

    GPoint evaluateQuadCurve(GPoint a, GPoint b, GPoint c, float t) {
        //DONT USE POW, MEGA SLOW
        //refactor the equation using horners method to make faster
        return GPoint::Make(
            ((a.x()-2*b.x()+c.x())*t+2*b.x() - 2*a.x())*t + a.x(),
            ((a.y()-2*b.y()+c.y())*t+2*b.y() - 2*a.y())*t + a.y()
        );
    }

    GPoint evaluateCubicCurve(GPoint a, GPoint b, GPoint c, GPoint d, float t) {
        //DONT USE POW, MEGA SLOW
        //refactor the equation using horners method to make faster
        return GPoint::Make(
            (((-a.x()+3*b.x()-3*c.x()+d.x())*t+3*a.x()-6*b.x()+3*c.x())*t-3*a.x()+3*b.x())*t+a.x(),
            (((-a.y()+3*b.y()-3*c.y()+d.y())*t+3*a.y()-6*b.y()+3*c.y())*t-3*a.y()+3*b.y())*t+a.y()
        );
    }

    void save() {
        GMatrix topCTM = cTMQueue.top(); 
        GMatrix state(
            topCTM[0],topCTM[1],topCTM[2],
            topCTM[3],topCTM[4],topCTM[5]
        );
        cTMQueue.push(state);

    }
    void restore() {
        cTMQueue.pop();
    }
    void concat(const GMatrix&matrix) {
        cTMQueue.top().preConcat(matrix);
    }
    
};


/**
 *  Implemnt this, returning an instance of your subclass of GCanvas.
 */
std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap&newmap) {
    if (!newmap.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new MyCanvas(newmap));
}
/**
 *  Implement this, drawing into the provided canvas, and returning the title of your artwork.
 */
std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    return "a6 drawing";
    
};

#endif
