#include "platform.h"
#include "Renderer.h"
#include GLHEADER
#include <iostream>
#include "Font.h"
#include <boost/filesystem.hpp>
#include "Log.h"
#include <stack>

namespace Renderer {
	std::stack<Rect> clipStack;

	void setColor4bArray(GLubyte* array, unsigned int color)
	{
		array[0] = (color & 0xff000000) >> 24;
		array[1] = (color & 0x00ff0000) >> 16;
		array[2] = (color & 0x0000ff00) >> 8;
		array[3] = (color & 0x000000ff);
	}

	void buildGLColorArray(GLubyte* ptr, unsigned int color, unsigned int vertCount)
	{
		//convert color from ???? to RGBA?
		unsigned int colorRGBA;
		setColor4bArray((GLubyte *)&colorRGBA, color);
		//write color to unsigned int array
		GLuint * uiPtr = (GLuint *)ptr;
		for(unsigned int i = 0; i < vertCount; i++)
		{
			uiPtr[i] = colorRGBA;
		}
	}

	void translatef(float x, float y)
	{
		glTranslatef(x, y, 0);
	}

	void translate(Vector2i offset)
	{
		translatef((float)offset.x, (float)offset.y);
	}

	void pushClipRect(int x, int y, unsigned int w, unsigned int h)
	{
		Rect rect(x, y, w, h);
		if(rect.size.x == 0)
			rect.size.x = Renderer::getScreenWidth() - rect.pos.x;
		if(rect.size.y == 0)
			rect.size.y = Renderer::getScreenHeight() - rect.pos.y;

		//glScissor starts at the bottom left of the window
		//so (0, 0, 1, 1) is the bottom left pixel
		//everything else uses y+ = down, so flip it to be consistent
		rect.pos.y = Renderer::getScreenHeight() - rect.pos.y - rect.size.y;

		clipStack.push(rect);
		glScissor(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
		glEnable(GL_SCISSOR_TEST);
	}

	void pushClipRect(Vector2i pos, Vector2u size)
	{
		pushClipRect(pos.x, pos.y, size.x, size.y);
	}

	void popClipRect()
	{
		if(clipStack.empty())
		{
			LOG(LogError) << "Tried to popClipRect while the stack was empty!";
			return;
		}
		clipStack.pop();
		if(clipStack.empty())
		{
			glDisable(GL_SCISSOR_TEST);
		}else{
			Rect top = clipStack.top();
			glScissor(top.pos.x, top.pos.y, top.size.x, top.size.y);
		}
	}

	void drawRect(int x, int y, int w, int h, unsigned int color)
	{
#ifdef USE_OPENGL_ES
		GLshort points[12];
#else
		GLint points[12];
#endif

		points[0] = x; points [1] = y;
		points[2] = x; points[3] = y + h;
		points[4] = x + w; points[5] = y;

		points[6] = x + w; points[7] = y;
		points[8] = x; points[9] = y + h;
		points[10] = x + w; points[11] = y + h;

		GLubyte colors[6*4];
		buildGLColorArray(colors, color, 6);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

#ifdef USE_OPENGL_ES
		glVertexPointer(2, GL_SHORT, 0, points);
#else
		glVertexPointer(2, GL_INT, 0, points);
#endif
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableClientState(GL_BLEND);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_COLOR_ARRAY);
	}
};
