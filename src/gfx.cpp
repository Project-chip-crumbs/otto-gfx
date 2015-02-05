#include "gfx.hpp"
#define GLM_FORCE_RADIANS 1
#include "gtx/matrix_transform_2d.hpp"

#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <VG/vgu.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>

namespace otto {

static void unpackRGB(uint32_t color, float &r, float &g, float &b) {
  r = (color         & 0xff) / 255.0f;
  g = ((color >> 8)  & 0xff) / 255.0f;
  b = ((color >> 16) & 0xff) / 255.0f;
}

static VGColorRampSpreadMode fromNSVG(NSVGspreadType spread) {
  switch (spread) {
    case NSVG_SPREAD_REFLECT: return VG_COLOR_RAMP_SPREAD_REFLECT;
    case NSVG_SPREAD_REPEAT: return VG_COLOR_RAMP_SPREAD_REPEAT;
    case NSVG_SPREAD_PAD:
    default:
      return VG_COLOR_RAMP_SPREAD_PAD;
  }
}

static VGJoinStyle fromNSVG(NSVGlineJoin join) {
  switch (join) {
    case NSVG_JOIN_ROUND: return VG_JOIN_ROUND;
    case NSVG_JOIN_BEVEL: return VG_JOIN_BEVEL;
    case NSVG_JOIN_MITER:
    default:
      return VG_JOIN_MITER;
  }
}

static VGCapStyle fromNSVG(NSVGlineCap cap) {
  switch (cap) {
    case NSVG_CAP_ROUND: return VG_CAP_ROUND;
    case NSVG_CAP_SQUARE: return VG_CAP_SQUARE;
    case NSVG_CAP_BUTT:
    default:
      return VG_CAP_BUTT;
  }
}

static VGPaint createPaintFromRGBA(float r, float g, float b, float a) {
  auto paint = vgCreatePaint();
  VGfloat color[] = { r, g, b, a };
  vgSetParameteri(paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
  vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
  return paint;
}

static VGPaint createPaintFromNSVGpaint(const NSVGpaint &svgPaint, float opacity = 1.0f) {
  auto paint = vgCreatePaint();

  if (svgPaint.type == NSVG_PAINT_COLOR) {
    VGfloat color[4];
    unpackRGB(svgPaint.color, color[0], color[1], color[2]);
    color[3] = opacity;
    vgSetParameteri(paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
  }

  // TODO(ryan): We don't need gradients yet, but I got about halfway through
  // implementing them. Finish this up when we actually need them.

  // else if (svgPaint.type == NSVG_PAINT_LINEAR_GRADIENT ||
  //          svgPaint.type == NSVG_PAINT_RADIAL_GRADIENT) {
  //   const auto &grad = *svgPaint.gradient;

  //   if (svgPaint.type == NSVG_PAINT_LINEAR_GRADIENT) {
  //     VGfloat points[] = {
  //     };
  //     vgSetParameteri(paint, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
  //     vgSetParameterfv(paint, VG_PAINT_LINEAR_GRADIENT, 4, points);
  //   }

  //   VGfloat stops[5 * grad.nstops];
  //   for (int i = 0; i < grad.nstops; ++i) {
  //     auto s = &stops[i * 5];
  //     s[0] = grad.stops[i].offset;
  //     unpackRGB(grad.stops[i].color, s[1], s[2], s[3]);
  //     s[4] = 1.0f; // TODO(ryan): Are NSVG gradients always opaque?
  //   }

  //   vgSetParameteri(paint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, fromNSVG(static_cast<NSVGspreadType>(grad.spread)));
  //   vgSetParameteri(paint, VG_PAINT_COLOR_RAMP_PREMULTIPLIED, false);
  //   vgSetParameterfv(paint, VG_PAINT_COLOR_RAMP_STOPS, 5 * grad.nstops, stops);
  // }

  return paint;
}


void strokePaint(const NSVGpaint &svgPaint, float opacity) {
  auto paint = createPaintFromNSVGpaint(svgPaint, opacity);
  vgSetPaint(paint, VG_STROKE_PATH);
  vgDestroyPaint(paint);
}

void fillPaint(const NSVGpaint &svgPaint, float opacity) {
  auto paint = createPaintFromNSVGpaint(svgPaint, opacity);
  vgSetPaint(paint, VG_FILL_PATH);
  vgDestroyPaint(paint);
}

void strokeColor(float r, float g, float b, float a) {
  auto paint = createPaintFromRGBA(r, g, b, a);
  vgSetPaint(paint, VG_STROKE_PATH);
  vgDestroyPaint(paint);
}
void strokeColor(const glm::vec4 &color) {
  strokeColor(color.r, color.g, color.b, color.a);
}
void strokeColor(const glm::vec3 &color) {
  strokeColor(color.r, color.g, color.b);
}

void fillColor(float r, float g, float b, float a) {
  auto paint = createPaintFromRGBA(r, g, b, a);
  vgSetPaint(paint, VG_FILL_PATH);
  vgDestroyPaint(paint);
}
void fillColor(const glm::vec4 &color) {
  fillColor(color.r, color.g, color.b, color.a);
}
void fillColor(const glm::vec3 &color) {
  fillColor(color.r, color.g, color.b);
}

void strokeWidth(VGfloat width) {
  vgSetf(VG_STROKE_LINE_WIDTH, width);
}

void strokeCap(VGCapStyle cap) {
  vgSeti(VG_STROKE_CAP_STYLE, cap);
}

void strokeJoin(VGJoinStyle join) {
  vgSeti(VG_STROKE_JOIN_STYLE, join);
}


void moveTo(VGPath path, float x, float y) {
  VGubyte segs[] = { VG_MOVE_TO };
  VGfloat coords[] = { x, y };
  vgAppendPathData(path, 1, segs, coords);
}

void lineTo(VGPath path, float x, float y) {
  VGubyte segs[] = { VG_LINE_TO };
  VGfloat coords[] = { x, y };
  vgAppendPathData(path, 1, segs, coords);
}

void cubicTo(VGPath path, float x1, float y1, float x2, float y2, float x3, float y3) {
  VGubyte segs[] = { VG_CUBIC_TO };
  VGfloat coords[] = { x1, y1, x2, y2, x3, y3 };
  vgAppendPathData(path, 1, segs, coords);
}

static float RAD_TO_DEG = 180.0f / M_PI;

void arc(VGPath path, float x, float y, float w, float h, float startAngle, float endAngle) {
  float as = startAngle * RAD_TO_DEG;
  float ae = (endAngle - startAngle) * RAD_TO_DEG;
  vguArc(path, x, y, w, h, as, ae, VGU_ARC_OPEN);
}


//
// Scratch Path Operators
//

static VGPath scratchPath = 0;

void beginPath() {
  if (scratchPath == VG_INVALID_HANDLE) {
    scratchPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
  }
  else {
    vgClearPath(scratchPath, VG_PATH_CAPABILITY_ALL);
  }
}

void moveTo(float x, float y) {
  moveTo(scratchPath, x, y);
}
void moveTo(const glm::vec2 &pos) {
  moveTo(pos.x, pos.y);
}

void lineTo(float x, float y) {
  lineTo(scratchPath, x, y);
}
void lineTo(const glm::vec2 &pos) {
  lineTo(pos.x, pos.y);
}

void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  cubicTo(scratchPath, x1, y1, x2, y2, x3, y3);
}
void cubicTo(const glm::vec2 &p1, const glm::vec2 p2, const glm::vec2 &p3) {
  cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void arc(float cx, float cy, float w, float h, float angleStart, float angleEnd) {
  arc(scratchPath, cx, cy, w, h, angleStart, angleEnd);
}
void arc(const glm::vec2 &ctr, const glm::vec2 &size, float angleStart, float angleEnd) {
  arc(ctr.x, ctr.y, size.x, size.y, angleStart, angleEnd);
}

void circle(float cx, float cy, float radius) {
  auto d = radius * 2.0f;
  arc(cx, cy, d, d, 0.0f, M_PI * 2.0f);
}
void circle(const glm::vec2 &ctr, float radius) {
  circle(ctr.x, ctr.y, radius);
}


void fill() {
  vgDrawPath(scratchPath, VG_FILL_PATH);
}

void stroke() {
  vgDrawPath(scratchPath, VG_STROKE_PATH);
}

void fillAndStroke() {
  vgDrawPath(scratchPath, VG_FILL_PATH | VG_STROKE_PATH);
}


void clearColor(float r, float g, float b, float a) {
  VGfloat color[] = { r, g, b, a };
  vgSetfv(VG_CLEAR_COLOR, 4, color);
}
void clearColor(const glm::vec4 &color) {
  clearColor(color.r, color.g, color.b, color.a);
}
void clearColor(const glm::vec3 &color) {
  clearColor(color.r, color.g, color.b);
}

void clear(float x, float y, float w, float h) {
  vgClear(x, y, w, h);
}


//
// SVG
//

void draw(const NSVGimage &svg) {
  for (auto shape = svg.shapes; shape != NULL; shape = shape->next) {
    bool hasStroke = shape->stroke.type != NSVG_PAINT_NONE;
    bool hasFill = shape->fill.type != NSVG_PAINT_NONE;

    if (hasFill) {
      fillPaint(shape->fill, shape->opacity);
    }

    if (hasStroke) {
      strokeWidth(shape->strokeWidth);
      strokeJoin(fromNSVG(static_cast<NSVGlineJoin>(shape->strokeLineJoin)));
      strokeCap(fromNSVG(static_cast<NSVGlineCap>(shape->strokeLineCap)));
      strokePaint(shape->stroke, shape->opacity);
    }

    auto vgPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);

    for (auto path = shape->paths; path != NULL; path = path->next) {
      moveTo(vgPath, path->pts[0], path->pts[1]);
      for (int i = 0; i < path->npts - 1; i += 3) {
        float* p = &path->pts[i * 2];
        cubicTo(vgPath, p[2], p[3], p[4], p[5], p[6], p[7]);
      }
    }

    vgDrawPath(vgPath, (hasFill   ? VG_FILL_PATH   : 0) |
                       (hasStroke ? VG_STROKE_PATH : 0));
    vgDestroyPath(vgPath);
  }
}

void draw(const NSVGimage *img) {
  draw(*img);
}


//
// Stack
//

using namespace glm;

static std::vector<mat3> transformStack = { mat3() };

static void loadMatrix() {
  vgLoadMatrix(&transformStack.back()[0][0]);
}


void pushTransform() {
  transformStack.push_back(transformStack.back());
}

void popTransform() {
  transformStack.pop_back();
  loadMatrix();
}

void setTransform(const mat3 &xf) {
  transformStack.back() = xf;
  loadMatrix();
}

const mat3 getTransform() {
  return transformStack.back();
}


void translate(const vec2 &vec) {
  transformStack.back() = translate(transformStack.back(), vec);
  loadMatrix();
}
void translate(float x, float y) {
  translate(vec2(x, y));
}

void rotate(float radians) {
  transformStack.back() = rotate(transformStack.back(), radians);
  loadMatrix();
}

void scale(const glm::vec2 &vec) {
  transformStack.back() = scale(transformStack.back(), vec);
  loadMatrix();
}
void scale(float x, float y) {
  scale(vec2(x, y));
}
void scale(float s) {
  scale(vec2(s));
}


//
// Text
//

struct Font {
  VGFont font = VG_INVALID_HANDLE;
  stbtt_fontinfo info;
};

static float fontSize = 14.0f;
static Font font;

static std::unique_ptr<char[]> loadFileBinary(const std::string &path) {
  std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    auto size = file.tellg();
    auto buffer = std::unique_ptr<char[]>(new char[size]);

    file.seekg(0, std::ios::beg);
    file.read(buffer.get(), size);
    file.close();

    return buffer;
  }
  return {};
}

static VGFont createVGFontFromFontInfo(const stbtt_fontinfo &info) {
  auto font = vgCreateFont(info.numGlyphs);
  for (int i = 0; i < info.numGlyphs; ++i) {
    stbtt_vertex *verts;
    auto numVerts = stbtt_GetGlyphShape(&info, i, &verts);

    VGubyte segs[numVerts];
    VGshort coords[numVerts * 4];
    int numCoords = 0;
    for (int j = 0; j < numVerts; ++j) {
      const auto &v = verts[j];
      switch (v.type) {
        case STBTT_vmove: {
          segs[j] = VG_MOVE_TO;
          coords[numCoords++] = v.x;
          coords[numCoords++] = v.y;
        } break;
        case STBTT_vline: {
          segs[j] = VG_LINE_TO;
          coords[numCoords++] = v.x;
          coords[numCoords++] = v.y;
        } break;
        case STBTT_vcurve: {
          segs[j] = VG_QUAD_TO;
          coords[numCoords++] = v.cx;
          coords[numCoords++] = v.cy;
          coords[numCoords++] = v.x;
          coords[numCoords++] = v.y;
        } break;
      }
    }

    stbtt_FreeShape(&info, verts);

    auto path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_S_16, 1.0f / 256.0f, 0.0f, 0,
                             0, VG_PATH_CAPABILITY_ALL);
    vgAppendPathData(path, numVerts, segs, coords);

    int advanceWidth;
    stbtt_GetGlyphHMetrics(&info, i, &advanceWidth, nullptr);

    VGfloat origin[] = { 0.0f, 0.0f };
    VGfloat escapement[] = { advanceWidth / 256.0f, 0.0f };
    vgSetGlyphToPath(font, i, path, VG_FALSE, origin, escapement);

    vgDestroyPath(path);
  }
  return font;
}

void loadFont(const std::string &path) {
  auto fontBuffer = loadFileBinary(path);
  if (fontBuffer) {
    if (stbtt_InitFont(&font.info, reinterpret_cast<uint8_t *>(fontBuffer.get()), 0)) {
      font.font = createVGFontFromFontInfo(font.info);
    } else {
      std::cerr << "Failed to load font from: " << path << std::endl;
    }
  }
}

void text(const std::string &text) {
  auto count = text.length();

  VGuint indices[count];

  for (int i = 0; i < count; ++i) {
    indices[i] = stbtt_FindGlyphIndex(&font.info, text[i]);
  }

  VGfloat origin[] = { 0.0f, 0.0f };
  vgSetfv(VG_GLYPH_ORIGIN, 2, origin);

  vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
  loadMatrix();
  vgScale(fontSize, -fontSize);

  vgDrawGlyphs(font.font, count, indices, nullptr, nullptr, VG_FILL_PATH, true);

  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
}

} // otto
