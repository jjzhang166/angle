//@	{"targets":[{"name":"program.hpp","type":"include"}]}

#ifndef ANGLE_PROGRAM_HPP
#define ANGLE_PROGRAM_HPP

#include "shader.hpp"

namespace Angle
	{
	class Program
		{
		public:
			template<class ... T>
			Program(const Shader& shader,const T&...shaders);


			~Program()
				{glDeleteProgram(m_handle);}

		private:
      		static constexpr void collect(GLuint* handles)
				{}
			template<class ...T>
			static constexpr void collect(GLuint* handles,const Shader& shader,const T&...shaders)
				{
				handles[0]=shader.handle();
				collect(handles+1,shaders...);
				}
			GLuint m_handle;
		};

	template<class ... T>
	Program::Program(const Shader& shader,const T&...shaders)
		{
		assert(glCreateProgram!=nullptr);
		m_handle=glCreateProgram();
		GLuint handles[sizeof...(T)+1];
		collect(handles,shader,shaders...);
		for(size_t k=0;k<sizeof...(T)+1;++k)
			{glAttachShader(m_handle,handles[k]);}
		glLinkProgram(m_handle);
		auto status=static_cast<GLint>(0);
		glGetProgramiv(m_handle,GL_LINK_STATUS,&status);
		if(status!=GL_TRUE)
			{
			auto length=static_cast<GLint>(0);
			glGetProgramiv(m_handle,GL_INFO_LOG_LENGTH,&length);
			char message[1024];
			glGetShaderInfoLog(m_handle,std::min(1024,length),NULL,message);
			for(size_t k=0;k<sizeof...(T)+1;++k)
				{glDetachShader(m_handle,handles[k]);}
			glDeleteProgram(m_handle);
			r_eh->raise(Error("It was not possible to link the shader program. ",message));
			}
		}
	}

#endif