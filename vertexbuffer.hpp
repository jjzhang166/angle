//@	{"targets":[{"name":"vertexbuffer.hpp","type":"include"}]}

#ifndef ANGLE_VERTEXBUFFER_HPP
#define ANGLE_VERTEXBUFFER_HPP

#include "exceptionhandler.hpp"
#include <cassert>
#include <utility>

namespace Angle
	{
	enum class BufferUsage:GLenum
		{
		 STREAM_DRAW=GL_STREAM_DRAW
		,STREAM_READ=GL_STREAM_READ
		,STREAM_COPY=GL_STREAM_COPY
		,STATIC_DRAW=GL_STATIC_DRAW
		,STATIC_READ=GL_STATIC_READ
		,STATIC_COPY=GL_STATIC_COPY
		,DYNAMIC_DRAW=GL_DYNAMIC_DRAW
		,DYNAMIC_READ=GL_DYNAMIC_READ
		,DYNAMIC_COPY=GL_DYNAMIC_COPY
		};

	constexpr GLenum native_type(BufferUsage usage_type) noexcept
		{return static_cast<GLenum>(usage_type);}

	template<class ElementType,BufferUsage usage_type=BufferUsage::STATIC_DRAW>
	class VertexBuffer
		{
		public:
			typedef ElementType value_type;
			static constexpr auto vector_size=sizeof(value_type);
			static constexpr auto components=1;

			VertexBuffer(size_t n_elems):m_capacity(n_elems)
				{
				glCreateBuffers(1,&m_handle);
				glNamedBufferData(m_handle,sizeof(ElementType)*n_elems,NULL,native_type(usage_type));
				auto error=glGetError();
				if(error!=GL_NO_ERROR)
					{
					glDeleteBuffers(1,&m_handle);
					exceptionRaise(Error("Failed to allocate vertex buffer storage."));
					}
				}

			~VertexBuffer() noexcept
				{glDeleteBuffers(1,&m_handle);}

			VertexBuffer(const VertexBuffer&)=delete;

			VertexBuffer(VertexBuffer&& obj) noexcept:m_handle(obj.m_handle)
				{obj.m_handle=0;}

			VertexBuffer& operator=(const VertexBuffer&)=delete;

			VertexBuffer& operator=(VertexBuffer&& obj)
				{
				std::swap(obj.m_handle,m_handle);
				return *this;
				}

			VertexBuffer& bufferData(const ElementType* data,size_t n_elems) noexcept
				{
				assert(m_capacity==n_elems);
				glNamedBufferSubData(m_handle,0,sizeof(ElementType)*n_elems,data);
				return *this;
				}

			VertexBuffer& bufferDataResize(const ElementType* data,size_t n_elems)
				{
				if(m_capacity==n_elems)
					{glNamedBufferSubData(m_handle,0,sizeof(ElementType)*n_elems,data);}
				else
					{
					glNamedBufferData(m_handle,sizeof(ElementType)*n_elems,NULL,native_type(usage_type));
					auto error=glGetError();
					if(error!=GL_NO_ERROR)
						{exceptionRaise(Error("Failed to allocate vertex buffer storage."));}
					m_capacity=n_elems;
					}
				return *this;
				}

			GLuint handle() const noexcept
				{return m_handle;}

			size_t size() const noexcept
				{return m_capacity;}

		private:
			GLuint m_handle;
			size_t m_capacity;
		};

	template<BufferUsage usage_type>
	class VertexBuffer<float __attribute__ ((vector_size (16))),usage_type>:
		private VertexBuffer<float,usage_type>
		{
		public:
			typedef VertexBuffer<float,usage_type> base;
			typedef float vector_type __attribute__ ((vector_size (16)));
			static constexpr auto components=4;
			static constexpr auto vector_size=components*sizeof(float);
			using typename base::value_type;

			VertexBuffer(size_t n_elems):base(components*n_elems)
				{}

			VertexBuffer& bufferData(const vector_type* data,size_t n_elems) noexcept
				{
				base::bufferData(reinterpret_cast<const float*>(data),components*n_elems);
				return *this;
				}

			VertexBuffer& bufferDataResize(const vector_type* data,size_t n_elems) noexcept
				{
				base::bufferDataResize(reinterpret_cast<const float*>(data),components*n_elems);
				return *this;
				}

			using base::handle;
		};

	template<BufferUsage usage_type>
	class VertexBuffer<float __attribute__ ((vector_size (8))),usage_type>:
		private VertexBuffer<float,usage_type>
		{
		public:
			typedef VertexBuffer<float,usage_type> base;
			typedef float vector_type __attribute__ ((vector_size (8)));
			static constexpr auto components=2;
			static constexpr auto vector_size=components*sizeof(float);
			using typename base::value_type;

			VertexBuffer(size_t n_elems):base(components*n_elems)
				{}

			VertexBuffer& bufferData(const vector_type* data,size_t n_elems) noexcept
				{
				base::bufferData(reinterpret_cast<const float*>(data),components*n_elems);
				return *this;
				}

			VertexBuffer& bufferDataResize(const vector_type* data,size_t n_elems) noexcept
				{
				base::bufferDataResize(reinterpret_cast<const float*>(data),components*n_elems);
				return *this;
				}

			using base::handle;
		};
	};

#endif
