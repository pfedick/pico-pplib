
#include "ppl7-light.h"
#include "grafix.h"
#include <math.h>

namespace ppl7light {


static Grafix* pplgfx=NULL;
Grafix* GetGrafix()
{
    if (pplgfx) return pplgfx;
    throw Exception("NoGrafixEngineException");
}

Grafix::Grafix()
{
    if (pplgfx) {
        throw Exception("DuplicateGrafixEngineException");
    }
    pplgfx=this;
    FontEngineFont6* font6=new FontEngineFont6();
    addFontEngine(font6);
}

Grafix::~Grafix()
{
    clearFonts();
    clearFontEngines();
}

} // EOF namespace ppl7light
