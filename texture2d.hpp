//@	{
//@	"targets":[{"name":"texture2d.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"texture2d.o","rel":"implementation"}]
//@	}

#ifndef ANGLE_TEXTURE2D_HPP
#define ANGLE_TEXTURE2D_HPP

#include "exceptionhandler.hpp"
#include <GL/glew.h>
#include <cassert>
#include <utility>

namespace Angle
	{
	enum class TextureFormat:GLenum
		{
		 R8=GL_R8
		,R8_SNORM=GL_R8_SNORM
		,R16=GL_R16
		,R16_SNORM=GL_R16_SNORM
		,RG8=GL_RG8
		,RG8_SNORM=GL_RG8_SNORM
		,RG16=GL_RG16
		,RG16_SNORM=GL_RG16_SNORM
		,R3_G3_B2=GL_R3_G3_B2
		,RGB4=GL_RGB4
		,RGB5=GL_RGB5
		,RGB8=GL_RGB8
		,RGB8_SNORM=GL_RGB8_SNORM
		,RGB10=GL_RGB10
		,RGB12=GL_RGB12
		,RGB16_SN=GL_RGB16_SNORM
		,RGBA2=GL_RGBA2
		,RGBA4=GL_RGBA4
		,RGB5_A1=GL_RGB5_A1
		,RGBA8=GL_RGBA8
		,RGBA8_SN=GL_RGBA8_SNORM
		,RGB10_A2=GL_RGB10_A2
		,RGB10_A2UI=GL_RGB10_A2UI
		,RGBA12=GL_RGBA12
		,RGBA16=GL_RGBA16
		,SRGB8=GL_SRGB8
		,SRGB8_ALPHA8=GL_SRGB8_ALPHA8
		,R16F=GL_R16F
		,RG16F=GL_RG16F
		,RGB16F=GL_RGB16F
		,RGBA16F=GL_RGBA16F
		,R32F=GL_R32F
		,RG32F=GL_RG32F
		,RGB32F=GL_RGB32F
		,RGBA32F=GL_RGBA32F
		,R11F_G11F_B10F=GL_R11F_G11F_B10F
		,RGB9_E5=GL_RGB9_E5
		,R8I=GL_R8I
		,R8UI=GL_R8UI
		,R16I=GL_R16I
		,R16UI=GL_R16UI
		,R32I=GL_R32I
		,R32UI=GL_R32UI
		,RG8I=GL_RG8I
		,RG8UI=GL_RG8UI
		,RG16I=GL_RG16I
		,RG16UI=GL_RG16UI
		,RG32I=GL_RG32I
		,RG32UI=GL_RG32UI
		,RGB8I=GL_RGB8I
		,RGB8UI=GL_RGB8UI
		,RGB16I=GL_RGB16I
		,RGB16UI=GL_RGB16UI
		,RGB32I=GL_RGB32I
		,RGB32UI=GL_RGB32UI
		,RGBA8I=GL_RGBA8I
		,RGBA8UI=GL_RGBA8UI
		,RGBA16I=GL_RGBA16I
		,RGBA16UI=GL_RGBA16UI
		,RGBA32I=GL_RGBA32I
		,RGBA32UI=GL_RGBA32UI
		};

	constexpr GLenum native_type(TextureFormat format) noexcept
		{return static_cast<GLenum>(format);}

	class Texture2D
		{
		public:
			Texture2D(GLsizei levels,TextureFormat format,GLsizei width,GLsizei height)
				{
				glCreateTextures(GL_TEXTURE_2D,1,&m_handle);
				glTextureStorage2D(m_handle,levels,native_type(format),width,height);
				auto error=glGetError();
				if(error!=GL_NO_ERROR)
					{
					glDeleteTextures(1,&m_handle);
					exceptionRaise(Error("Failed to allocate texture storage."));
					}
				m_width=width;
				m_height=height;
				m_format=format;
				m_levels=levels;
				m_unit=static_cast<GLuint>(-1);
				}

			~Texture2D() noexcept
				{
				if(m_unit!=static_cast<GLuint>(-1))
					{glBindTextureUnit(m_unit,0);}
				glDeleteTextures(1,&m_handle);
				}

			Texture2D(const Texture2D&)=delete;

			Texture2D(Texture2D&& obj) noexcept:m_handle(obj.m_handle)
				{obj.m_handle=0;}

			Texture2D& operator=(const Texture2D&)=delete;

			Texture2D& operator=(Texture2D&& obj)
				{
				std::swap(obj.m_handle,m_handle);
				return *this;
				}

			GLuint handle() const noexcept
				{return m_handle;}

			GLsizei width() const noexcept
				{return m_width;}

			GLsizei height() const noexcept
				{return m_width;}

			template<class T>
			void dataSet(const T* data,GLsizei width_in,GLsizei height_in)
				{
				if(m_width!=width_in || m_height!=height_in)
					{realloc(width_in,height_in);}
				glTextureSubImage2D(m_handle,0,0,0,width_in,height_in,gl_format(T{}),gl_type(T{})
					,data);
				if(m_levels>1)
					{glGenerateTextureMipmap(m_handle);}
				}

			template<class T>
			void dataSet(const T* data,GLint x,GLint y,GLsizei width_in,GLsizei height_in)
				{
				if(width_in + x>m_width || height_in + y>m_height)
					{realloc(width_in,height_in);}
				glTextureSubImage2D(m_handle,0,x,y,width_in,height_in,gl_format(T{}),gl_type(T{})
					,data);
				if(m_levels>1)
					{glGenerateTextureMipmap(m_handle);}
				}

			void bind(GLuint unit) const
				{
				glBindTextureUnit(unit,m_handle);
				m_unit=unit;
				}

			GLuint handle() noexcept
				{return m_handle;}

			void realloc(GLsizei width_in,GLsizei height_in);

		private:
			GLuint m_handle;
			mutable GLuint m_unit;
			GLsizei m_width;
			GLsizei m_height;
			TextureFormat m_format;
			GLsizei m_levels;
		};
	};

#endif
