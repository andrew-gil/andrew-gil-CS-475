#ifndef MyBlend_DEFINED 
#define MyBlend_DEFINED

#include "GPaint.h"
#include "GPixel.h"
#include "GBlendMode.h"
//#include "MyCanvas.cpp"




/**
    kClear,    //!<     0
    kSrc,      //!<     S
    kDst,      //!<     D
    kSrcOver,  //!<     S + (1 - Sa)*D
    kDstOver,  //!<     D + (1 - Da)*S
    kSrcIn,    //!<     Da * S
    kDstIn,    //!<     Sa * D
    kSrcOut,   //!<     (1 - Da)*S
    kDstOut,   //!<     (1 - Sa)*D
    kSrcATop,  //!<     Da*S + (1 - Sa)*D
    kDstATop,  //!<     Sa*D + (1 - Da)*S
    kXor,      //!<     (1 - Sa)*D + (1 - Da)*S
*/
static inline unsigned FasterRDiv255(unsigned x) {
    return (x+128)*257 >> 16;
}
static inline int RDiv255(int x) {
    return (x+127)/255;
}

static inline GPixel bClear(const GPixel source, const GPixel destination) {
    return GPixel_PackARGB(0,0,0,0);
}

static inline GPixel bSrc(const GPixel source, const GPixel destination) {
    return source;
}
static inline GPixel bDst(const GPixel source, const GPixel destination) {
    return destination;
}
static inline GPixel bSrcOver(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = srcr + RDiv255((255-srca)*otherr);
    int newg = srcg + RDiv255((255-srca)*otherg);
    int newb = srcb + RDiv255((255-srca)*otherb);
    int newa = srca + RDiv255((255-srca)*othera);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bDstOver(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = otherr + RDiv255(srcr*(255-othera));
    int newg = otherg + RDiv255(srcg*(255-othera));
    int newb = otherb + RDiv255(srcb*(255-othera));
    int newa = othera + RDiv255(srca*(255-othera));
    return GPixel_PackARGB(newa, newr, newg, newb);
}   

static inline GPixel bSrcIn(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int newr = RDiv255(othera*srcr);
    int newg = RDiv255(othera*srcg);
    int newb = RDiv255(othera*srcb);
    int newa = RDiv255(srca*othera);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bDstIn(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = RDiv255(srca*otherr);
    int newg = RDiv255(srca*otherg);
    int newb = RDiv255(srca*otherb);
    int newa = RDiv255(othera*srca);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bSrcOut(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int newr = RDiv255((255-othera)*srcr);
    int newg = RDiv255((255-othera)*srcg);
    int newb = RDiv255((255-othera)*srcb);
    int newa = RDiv255((255-othera)*srca);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bDstOut(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int othera = GPixel_GetA(destination);
    int newr = RDiv255((255-srca)*otherr);
    int newg = RDiv255((255-srca)*otherg);
    int newb = RDiv255((255-srca)*otherb);
    int newa = RDiv255((255-srca)*othera);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bSrcATop(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = RDiv255(othera*srcr) + RDiv255((255-srca)*otherr);
    int newg = RDiv255(othera*srcg) + RDiv255((255-srca)*otherg);
    int newb = RDiv255(othera*srcb) + RDiv255((255-srca)*otherb);
    //int newa = RDiv255(srca*othera) + RDiv255((255-othera)*srca);
    int newa = othera;
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bDstATop(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = RDiv255(srca*otherr) + RDiv255((255-othera)*srcr);
    int newg = RDiv255(srca*otherg) + RDiv255((255-othera)*srcg);
    int newb = RDiv255(srca*otherb) + RDiv255((255-othera)*srcb);
    //int newa = RDiv255(srca*othera) + RDiv255((255-othera)*srca);
    int newa = srca;
    return GPixel_PackARGB(newa, newr, newg, newb);
}

static inline GPixel bXor(const GPixel source, const GPixel destination) {
    int srca = GPixel_GetA(source);
    int srcr = GPixel_GetR(source);
    int srcg = GPixel_GetG(source);
    int srcb = GPixel_GetB(source);
    int othera = GPixel_GetA(destination);
    int otherr = GPixel_GetR(destination);
    int otherg = GPixel_GetG(destination);
    int otherb = GPixel_GetB(destination);
    int newr = RDiv255((255-srca)*otherr)+RDiv255((255-othera)*srcr);
    int newg = RDiv255((255-srca)*otherg)+RDiv255((255-othera)*srcg);
    int newb = RDiv255((255-srca)*otherb)+RDiv255((255-othera)*srcb);
    int newa = RDiv255((255-srca)*othera)+RDiv255((255-othera)*srca);
    return GPixel_PackARGB(newa, newr, newg, newb);
}

typedef GPixel (*BlendImp)(GPixel, GPixel);

BlendImp Blend_Modes[] = {
    bClear,    
    bSrc,      
    bDst,      
    bSrcOver,  
    bDstOver,  
    bSrcIn,
    bDstIn,    
    bSrcOut,   
    bDstOut,   
    bSrcATop,  
    bDstATop,  
    bXor,
};

static inline BlendImp getBlend_Modes(const GBlendMode mode, const GPixel src) {
    return Blend_Modes[static_cast<int>(mode)];
}

static inline BlendImp getBlend_Modes(const GBlendMode mode) {
    return Blend_Modes[static_cast<int>(mode)];
}

#endif