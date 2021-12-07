#ifndef MyEdge_DEFINED
#define MyEdge_DEFINED

#include "GPath.h"
#include "GRect.h"
#include <vector>

struct Edge {
    int top;
    int bottom;
    float slope;
    //starterx is x when y = 0, aka b when x = my+b
    float starterx;
    int winding;
};
Edge* createEdge(GPoint p0, GPoint p1, int winding);
void clipEdge(GPoint p0, GPoint p1, GRect bounds, std::vector<Edge>& edges);
bool lessThan(Edge a, Edge b);
bool compareEdgesStarterX(Edge a, Edge b);

#endif