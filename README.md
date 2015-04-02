# Goals

This library is designed to provide an easy-to-use immediate mode API backed by [OpenVG](https://www.khronos.org/openvg). It follows HTML5 canvas where possible.

# Usage

## Building

This project requires a C++ compiler and standard library that support C++14. We are currently testing it with `clang 3.5.1` and `libc++ 3.5.1` on Arch linux.

From the root directory make a `build/` directory and run `cmake` from inside it.

	mkdir build && cd build
	cmake -D CMAKE_CXX_COMPILER=clang++ ..

Then from the build directory you can run `make`.

	make

## Drawing Lines & Shapes

	// Line
	gfx::beginShape();
	gfx::moveTo(0.0f, 0.0f);
	gfx::lineTo(100.0f, 100.0f);
	gfx::strokeColor(0.0f, 0.5f, 1.0f);
	gfx::strokeWidth(5.0f);
	gfx::stroke(); // Draw the line
	
	// Circle
	gfx::beginShape();
	gfx::circle(50.0f, 50.0f, 25.0f);
	gfx::fillColor(1.0f, 1.0f, 0.0f);
	gfx::fill();

## Loading and Drawing SVG Graphics

	gfx::Svg icon = gfx::loadSvg("icon.svg", "px", 96);
	gfx::drawSvg(icon);

## Vectors and Matrices

gfx uses [OpenGL Mathematics](http://glm.g-truc.net) for vectors and matrices. You can use these in place of individual components in most functions.

	gfx::translate(gfx::vec2(5.0f));
	gfx::scale(gfx::vec2(2.0f, 1.0f));
	gfx::fillColor(vec4(vec3(1.0f), 0.5f));
	
## Masking

	// Draw into the mask
	gfx::clearMask(screenWidth, screenHeight);
	gfx::beginMask();
	gfx::drawSvg(maskIcon);
	gfx::endMask();
	
	gfx::enableMask();
	drawSomeStuff();
	gfx::disableMask();

## Scoped State

With some state modification like the matrix stack, masks, and color transforms it can be useful to scope a change to a block or function. We provide a few Scoped objects to handle this for you.

	void draw() {
		using namespace gfx;
		
		ScopedTransform xf; // pushTransform()
		translate(vec2(50.0f));
		scale(25.0f);
		
		for (int i = 0; i < 8; i) {
			beginShape();
			moveTo(0.0f, 0.0f);
			lineTo(1.0f, 0.0f);
			float t = i / 8.0f;
			strokeColor(vec3(t));
			stroke();
		}
		
		// Implicit popTransform() when we exit the block
	}

## TODO

* Allow loading multiple fonts
* Linear and radial gradient fills