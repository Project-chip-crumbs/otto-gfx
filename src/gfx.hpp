#pragma once

#include <VG/openvg.h>

#include <string>

#include "nanosvg.h"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat3x3.hpp"

namespace otto {

enum Align {
  // Horizontal
  ALIGN_LEFT   = 1 << 0,
  ALIGN_CENTER = 1 << 1,
  ALIGN_RIGHT  = 1 << 2,

  // Vertical
  ALIGN_TOP      = 1 << 3,
  ALIGN_MIDDLE   = 1 << 4,
  ALIGN_BOTTOM   = 1 << 5,
  ALIGN_BASELINE = 1 << 6
};

void strokePaint(const NSVGpaint &svgPaint, float opacity = 1.0f);
void fillPaint(const NSVGpaint &svgPaint, float opacity = 1.0f);

void strokeColor(float r, float g, float b, float a = 1.0f);
void strokeColor(const glm::vec4 &color);
void strokeColor(const glm::vec3 &color);
void fillColor(float r, float g, float b, float a = 1.0f);
void fillColor(const glm::vec4 &color);
void fillColor(const glm::vec3 &color);

void strokeWidth(VGfloat width);
void strokeCap(VGCapStyle cap);
void strokeJoin(VGJoinStyle join);

void moveTo(VGPath path, float x, float y);
void lineTo(VGPath path, float x, float y);
void cubicTo(VGPath path, float x1, float y1, float x2, float y2, float x3, float y3);
void arc(VGPath path, float x, float y, float w, float h, float startAngle, float endAngle);

void beginPath();

void moveTo(float x, float y);
void moveTo(const glm::vec2 &pos);
void lineTo(float x, float y);
void lineTo(const glm::vec2 &pos);
void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3);
void cubicTo(const glm::vec2 &p1, const glm::vec2 p2, const glm::vec2 &p3);
void arc(float cx, float cy, float w, float h, float angleStart, float angleEnd);
void arc(const glm::vec2 &ctr, const glm::vec2 &size, float angleStart, float angleEnd);
void circle(float cx, float cy, float radius);
void circle(const glm::vec2 &ctr, float radius);

void fill();
void stroke();
void fillAndStroke();

void clearColor(float r, float g, float b, float a = 1.0f);
void clearColor(const glm::vec4 &color);
void clearColor(const glm::vec3 &color);
void clear(float x, float y, float w, float h);

void draw(const NSVGimage &svg);
void draw(const NSVGimage *svg);

void pushTransform();
void popTransform();
void setTransform(const glm::mat3 &xf);
const glm::mat3 getTransform();

void translate(const glm::vec2 &vec);
void translate(float x, float y);
void rotate(float radians);
void scale(const glm::vec2 &vec);
void scale(float x, float y);
void scale(float s);

void loadFont(const std::string &path);
void textAlign(uint32_t align);
void fillText(const std::string &text);

} // otto
