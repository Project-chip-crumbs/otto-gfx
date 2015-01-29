#pragma once

#include <VG/openvg.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "nanosvg.h"

namespace otto {

void strokePaint(const NSVGpaint &svgPaint, float opacity = 1.0f);
void fillPaint(const NSVGpaint &svgPaint, float opacity = 1.0f);

void strokeColor(float r, float g, float b, float a = 1.0f);
void fillColor(float r, float g, float b, float a = 1.0f);

void strokeWidth(VGfloat width);
void strokeCap(VGCapStyle cap);
void strokeJoin(VGJoinStyle join);

void moveTo(VGPath path, float x, float y);
void lineTo(VGPath path, float x, float y);
void cubicTo(VGPath path, float x1, float y1, float x2, float y2, float x3, float y3);
void arc(VGPath path, float x, float y, float w, float h, float startAngle, float endAngle);

void beginPath();

void moveTo(float x, float y);
void lineTo(float x, float y);
void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3);
void arc(float x, float y, float w, float h, float startAngle, float endAngle);

void fill();
void stroke();
void fillAndStroke();

void draw(const NSVGimage &svg);

} // otto
