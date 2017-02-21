//@	{"targets":[{"name":"texture2d.o","type":"object","pkgconfig_libs":["glew"]}]}

#include "texture2d.hpp"

using namespace Angle;

void Texture2D::realloc(GLsizei width_in,GLsizei height_in)
	{
	GLuint handle;
	glCreateTextures(GL_TEXTURE_2D,1,&handle);
	glTextureStorage2D(handle,m_levels,native_type(m_format),width_in,height_in);
	auto error=glGetError();
	if(error!=GL_NO_ERROR)
		{
		glDeleteTextures(1,&handle);
		exceptionRaise(Error("Failed to allocate texture storage."));
		}
	glFinish();
	if(m_unit!=static_cast<GLuint>(-1))
		{glBindTextureUnit(m_unit,0);}
	glDeleteTextures(1,&m_handle);
	m_handle=handle;
	m_width=width_in;
	m_height=height_in;
	}
