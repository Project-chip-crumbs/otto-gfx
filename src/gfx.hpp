#pragma once

#include <VG/openvg.h>

#include <string>

#include "nanosvg.h"
#include "glm.hpp"

namespace otto {

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;

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

struct Rect {
  vec2 pos, size;

  Rect(const vec2 &pos, const vec2 &size) : pos{ pos }, size{ size } {}
  Rect(float x, float y, float width, float height) : pos{ x, y }, size{ width, height } {}

  float getArea() const { return size.x * size.y; }
};

using Svg = NSVGimage;

vec3 colorBGR(uint32_t color);

void strokePaint(const NSVGpaint &svgPaint, float opacity = 1.0f);
void fillPaint(const NSVGpaint &svgPaint, float opacity = 1.0f);

void strokeColor(float r, float g, float b, float a = 1.0f);
void strokeColor(const vec4 &color);
void strokeColor(const vec3 &color);
void strokeColor(uint32_t color);
void fillColor(float r, float g, float b, float a = 1.0f);
void fillColor(const vec4 &color);
void fillColor(const vec3 &color);
void fillColor(uint32_t color);

void strokeWidth(VGfloat width);
void strokeCap(VGCapStyle cap);
void strokeJoin(VGJoinStyle join);

void moveTo(VGPath path, float x, float y);
void lineTo(VGPath path, float x, float y);
void cubicTo(VGPath path, float x1, float y1, float x2, float y2, float x3, float y3);
void arc(VGPath path, float x, float y, float w, float h, float startAngle, float endAngle);
void circle(VGPath path, float x, float y, float radius);
void ellipse(VGPath path, float x, float y, float rx, float ry);
void rect(VGPath path, float x, float y, float width, float height);
void roundRect(VGPath path, float x, float y, float width, float height, float radius);

void beginPath();

void moveTo(float x, float y);
void moveTo(const vec2 &pos);
void lineTo(float x, float y);
void lineTo(const vec2 &pos);
void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3);
void cubicTo(const vec2 &p1, const vec2 p2, const vec2 &p3);
void arc(float cx, float cy, float w, float h, float angleStart, float angleEnd);
void arc(const vec2 &ctr, const vec2 &size, float angleStart, float angleEnd);
void circle(float cx, float cy, float radius);
void circle(const vec2 &ctr, float radius);
void ellipse(float cx, float cy, float rx, float ry);
void ellipse(const vec2 &ctr, const vec2 &radius);
void rect(float x, float y, float width, float height);
void rect(const vec2 &pos, const vec2 &size);
void rect(const Rect &r);
void roundRect(float x, float y, float width, float height, float radius);
void roundRect(const vec2 &pos, const vec2 &size, float radius);
void roundRect(const Rect &r, float radius);

void fillRuleEvenOdd();
void fillRuleNonZero();
VGFillRule getFillRule();

void fill();
void stroke();
void fillAndStroke();

void clearColor(float r, float g, float b, float a = 1.0f);
void clearColor(const vec4 &color);
void clearColor(const vec3 &color);
void clear(int x, int y, int w, int h);
void clear(const vec2 &pos, const vec2 &size);
void clear(const Rect &rect);

void draw(const Svg &svg, bool flipY = true);
void draw(const Svg *svg, bool flipY = true);

void setColorTransform(float sr, float sg, float sb, float sa,
                       float br, float bg, float bb, float ba);
void setColorTransform(const vec4 &scale, const vec4 &bias);
void enableColorTransform();
void disableColorTransform();

void pushMask(int width, int height);
void pushMask(const vec2 &size);
void popMask();
void beginMask();
void endMask();
void enableMask();
void disableMask();
void fillMask(int x, int y, int width, int height);
void fillMask(const vec2 &pos, const vec2 &size);
void fillMask(const Rect &rect);
void clearMask(int x, int y, int width, int height);
void clearMask(const vec2 &pos, const vec2 &size);
void clearMask(const Rect &rect);
void maskOperation(VGMaskOperation operation);

void pushTransform();
void popTransform();
void setTransform(const mat3 &xf);
void setTransformIdentity();
const mat3 getTransform();

void translate(const vec2 &vec);
void translate(float x, float y);
void rotate(float radians);
void scale(const vec2 &vec);
void scale(float x, float y);
void scale(float s);

Svg *loadSvg(const std::string &path, const std::string &units = "px", float dpi = 96);
void loadFont(const std::string &path);

void fontSize(float size);
void textAlign(uint32_t align);
void fillText(const std::string &text);
void fillText(const std::string &text, float x, float y);
void fillText(const std::string &text, const vec2 &pos);

Rect getTextBounds(const std::string &text);

struct Noncopyable {
protected:
  Noncopyable() = default;
  ~Noncopyable() = default;

  Noncopyable(const Noncopyable &) = delete;
  Noncopyable &operator=(const Noncopyable &) = delete;
};

struct ScopedTransform : private Noncopyable {
  ScopedTransform() { pushTransform(); }
  ~ScopedTransform() { popTransform(); }
};

struct ScopedMask : private Noncopyable {
  ScopedMask(int width, int height) { pushMask(width, height); }
  ScopedMask(const vec2 &size) { pushMask(size); }
  ~ScopedMask() { popMask(); }
};

struct ScopedFillRule : private Noncopyable {
  VGFillRule prevFillRule;

  ScopedFillRule(VGFillRule fillRule) : prevFillRule{ getFillRule() } {
    vgSeti(VG_FILL_RULE, fillRule);
  }
  ~ScopedFillRule() { vgSeti(VG_FILL_RULE, prevFillRule); }
};

} // otto
