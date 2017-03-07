//@	{"targets":[{"name":"vertexarray.hpp","type":"include"}]}

#ifndef ANGLE_VERTEXARRAY_HPP
#define ANGLE_VERTEXARRAY_HPP

#include "arraysize.hpp"
#include "vertexbuffer.hpp"
#include "state.hpp"
#include <utility>
#include <cassert>

namespace Angle
	{
	struct VertexAttribute
		{
		GLint components;
		ValueType type;
		bool normalized;
		GLuint offset;
		};

	template<class BatchLayout>
	class VertexArray
		{
		private:
			template<GLuint k,bool dummy>
			class AttribContextPrev;

		public:
			VertexArray(const VertexArray&)=delete;

			VertexArray(VertexArray&& obj) noexcept:m_handle(obj.m_handle)
				{obj.m_handle=0;}

			VertexArray& operator=(const VertexArray&)=delete;

			VertexArray& operator=(VertexArray&& obj)
				{
				std::swap(obj.m_handle,m_handle);
				return *this;
				}

			VertexArray() noexcept
				{
				assert(glGenVertexArrays!=nullptr);
				glCreateVertexArrays(1,&m_handle);
              	Init<size(BatchLayout::attributes),true>::doIt(m_handle);
				}
				
			~VertexArray()
				{
				glBindVertexArray(0);
				glDeleteVertexArrays(1,&m_handle);
				}

			void bind() const noexcept
				{
				glBindVertexArray(m_handle);
				bindElementIndexData(m_index_type,m_index_count);
				}

			template<GLuint attrib>
			VertexArray& enableVertexAttrib() noexcept
				{
				static_assert(attrib>=0 && attrib<size(BatchLayout::attributes),"Attribute index out of bounds");
				glEnableVertexArrayAttrib(m_handle,attrib);
				return *this;
				}

			template<GLuint attrib>
			VertexArray& disableVertexAttrib() noexcept
				{
				static_assert(attrib>=0 && attrib<size(BatchLayout::attributes),"Attribute index out of bounds");
				glDisableVertexArrayAttrib(m_handle,attrib);
				return *this;
				}

			template<GLsizei attrib>
			class AttribContext
				{
				public:
					explicit AttribContext(VertexArray& vao) noexcept:
						r_vao(vao)
						{r_vao.enableVertexAttrib<attrib>();}
					~AttribContext() noexcept
						{r_vao.disableVertexAttrib<attrib>();}
				private:
					VertexArray& r_vao;
				};

			class AttribContextAll:public AttribContextPrev<size(BatchLayout::attributes),true>
				{
				public:
					explicit AttribContextAll(VertexArray& vao) noexcept:
						AttribContextPrev<size(BatchLayout::attributes),true>(vao)
						{}
				};

			template<GLuint attrib,class ElementType,BufferUsage usage_type>
			VertexArray& vertexBuffer(const VertexBuffer<ElementType,usage_type>& vbo) noexcept
				{
				static_assert(attrib>=0 && attrib<size(BatchLayout::attributes),"Attribute index out of bounds");
				typedef typename TypeGet<BatchLayout::attributes[attrib].type>::type attrib_type;
				typedef typename VertexBuffer<ElementType>::value_type value_type;
				static_assert(std::is_same<value_type,attrib_type>::value,"Attribute type mismatch");
				static_assert(VertexBuffer<ElementType>::components==BatchLayout::attributes[attrib].components,"Component count mismatch");
				glVertexArrayVertexBuffer(m_handle,attrib,vbo.handle(),BatchLayout::attributes[attrib].offset,VertexBuffer<ElementType>::vector_size);
				return *this;
				}

			template<class IndexType,BufferUsage usage_type>
			VertexArray& elementBuffer(const VertexBuffer<IndexType,usage_type>& buffer) noexcept
				{
				static_assert(std::is_integral<IndexType>::value,"IndexType must be an intger");
				glVertexArrayElementBuffer(m_handle,buffer.handle());
				m_index_type=ConstantGet<IndexType>::value;
				m_index_count=buffer.size();
				return *this;
				}

			GLuint handle() const noexcept
				{return m_handle;}

			ValueType indexTypeGet() const noexcept
				{return m_index_type;}

		private:
			template<GLuint k,bool dummy>
			struct Init
				{
				static void doIt(GLuint handle) noexcept
					{
					Init<k-1,dummy>::doIt(handle);                  
					const auto& attribute=BatchLayout::attributes[k-1];
					glVertexArrayAttribFormat(handle,k-1
						,attribute.components
						,native_type(attribute.type)
						,attribute.normalized
						,attribute.offset);
                	}
				};
      
			template<bool dummy>
			struct Init<0,dummy>
				{
				static void doIt(GLuint) noexcept
					{}
				};

			template<GLuint k,bool dummy>
			class AttribContextPrev:public AttribContextPrev<k-1,dummy>
				{
				public:
					AttribContextPrev(VertexArray& vao) noexcept:
						AttribContextPrev<k-1,dummy>(vao),m_context(vao)
						{}

				private:
					AttribContext<k-1> m_context;
				};
			template<bool dummy>
			class AttribContextPrev<0,dummy>
				{
				public:
					AttribContextPrev(VertexArray& vao) noexcept{}
				};

			GLuint m_handle;
		//	Additional data to improve glDrawElements
			ValueType m_index_type;
			size_t m_index_count;
		};
	}

#endif