#pragma once

#include <VG/openvg.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

namespace otto {

void strokePaint(const NSVGpaint &svgPaint, float opacity = 1.0f);
void fillPaint(const NSVGpaint &svgPaint, float opacity = 1.0f);

void strokeWidth(VGfloat width);
void strokeCap(VGCapStyle cap);
void strokeJoin(VGJoinStyle join);

void moveTo(VGPath path, float x, float y);
void cubicTo(VGPath path, float x1, float y1, float x2, float y2, float x3, float y3);

void draw(const NSVGimage &svg);

} // otto
