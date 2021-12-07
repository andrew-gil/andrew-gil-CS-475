
#include "GPoint.h"
#include "MyEdge.h"
#include "GRect.h"
#include <vector>


Edge* createEdge(GPoint a, GPoint b, int winding) {
    int top = GRoundToInt(std::min(a.y(), b.y()));
    int bottom = GRoundToInt(std::max(a.y(), b.y()));
    assert(top < bottom);
    float slope = ((b.x() - a.x()) / (b.y()-a.y()));
    float starterx;
    if (top == GRoundToInt(a.y())) {
        starterx = a.x() + slope * (top - a.y() + 0.5);
    } else {
        starterx = b.x() + slope * (top - b.y() + 0.5);
    }
    Edge* edge = new Edge();
    edge->starterx = starterx;
    edge->slope = slope;
    edge->bottom = bottom;
    edge->top = top;
    edge->winding = winding;
    if (GRoundToInt(a.y()) == bottom) {
        edge->winding = winding * -1;
    }
    return edge;
}

void clipEdge(GPoint a, GPoint b, GRect boundingRect, std::vector<Edge>&edges) {
    int winding = 1;
    if (b.y() < a.y()) {
        GPoint temp = b;
        b = a;
        a = temp;
        winding *= -1;
    }
    if (b.y() <= boundingRect.top() || a.y() >= boundingRect.bottom()) {return;}
    if (a.y() < boundingRect.top()) {
        float fX = a.x() + (boundingRect.top() - a.y())*((b.x()-a.x())/(b.y()-a.y()));
        float fY = boundingRect.top();
        a.set(fX, fY);
    }
    if (b.y() > boundingRect.bottom()) {
        float fX = b.x() - (b.y() - boundingRect.bottom())*((b.x()-a.x())/(b.y()-a.y()));
        float fY = boundingRect.bottom();
        b.set(fX,fY);
    }
    if (b.x() < a.x()) {
        GPoint temp = b;
        b = a;
        a = temp;
        winding *= -1;
    }
    if (b.x() <= boundingRect.left()) {
        GPoint p0 = GPoint::Make(boundingRect.left(), a.y());
        GPoint p1 = GPoint::Make(boundingRect.left(), b.y());
        if (GRoundToInt(p0.y()) != GRoundToInt(p1.y())) {
            edges.push_back(*createEdge(p0, p1, winding));
        }
        return;
    }
    if (a.x() >= boundingRect.right()) {
        GPoint p0 = GPoint::Make(boundingRect.right(), a.y());
        GPoint p1 = GPoint::Make(boundingRect.right(), b.y());
        if (GRoundToInt(p0.y()) != GRoundToInt(p1.y())) {
            edges.push_back(*createEdge(p0, p1, winding));
        }
        return;
    }
    if (a.x() < boundingRect.left()) {
        GPoint p0 = GPoint::Make(boundingRect.left(), a.y());
        GPoint p1 = GPoint::Make(boundingRect.left(), a.y() + (boundingRect.left() - a.x())*((b.y()-a.y())/(b.x()-a.x())));
        if (GRoundToInt(p0.y()) != GRoundToInt(p1.y())) {
            edges.push_back(*createEdge(p0, p1, winding));
        }
        a.set(boundingRect.left(), a.y() + (boundingRect.left() - a.x())*((b.y()-a.y())/(b.x()-a.x())));
    }
    if (b.x() > boundingRect.right()) {
        GPoint p0 = GPoint::Make(boundingRect.right(), b.y() - (b.x()-boundingRect.right())*((b.y()-a.y())/(b.x()-a.x())));
        GPoint p1 = GPoint::Make(boundingRect.right(), b.y());
        if (GRoundToInt(p0.y()) != GRoundToInt(p1.y())) {
            edges.push_back(*createEdge(p0, p1, winding));
        }
        b.set(boundingRect.right(), b.y() - (b.x()-boundingRect.right())*((b.y()-a.y())/(b.x()-a.x())));
    }
    if (GRoundToInt(a.y()) != GRoundToInt(b.y())) {
        Edge* edge = createEdge(a,b,winding);
        edges.push_back(*edge);
    }
}

//if lessThan returns true, the first edge parameter must be higher and therefore first on the queue
bool lessThan (Edge a, Edge b) {
    //if a is less than b, return true; otherwise return false;
    if (a.top < b.top) {
        return true;
    }
    if (a.top > b.top) {
        return false;
    }
    //its assumed tops are equal now
    if (GRoundToInt(a.starterx) < GRoundToInt(b.starterx)) {
        return true;
    } 
    if (GRoundToInt(a.starterx) > GRoundToInt(b.starterx)) {
        return false;
    }
    return a.slope < b.slope;
    /**now the starterx are equal, so test slope
    if (a.slope < b.slope) {
        return true;
    } 
    return false;
    */
}

bool compareEdgesStarterX(Edge a, Edge b) {
    return a.starterx < b.starterx;
}