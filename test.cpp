//@ {
//@	 "targets":
//@		[
//@			{
//@			 "name":"test","type":"application"
//@			,"pkgconfig_libs":["glfw3"]
//@			}
//@		]
//@	}

#include "vertexarray.hpp"
#include "program.hpp"
#include "init.hpp"
#include "contextguard.hpp"
#include "texture2d.hpp"

#include <geosimd/point.hpp>
#include <GLFW/glfw3.h>

struct GLFWContext
	{
	GLFWContext(const Angle::VersionRequest& version)
		{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);
		switch(version.profile)
			{
			case Angle::VersionRequest::Profile::CORE:
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				break;
			case Angle::VersionRequest::Profile::COMPAT:
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
				break;
			case Angle::VersionRequest::Profile::ANY:
			default:
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
				break;
			}
		if(version.forward_compatible)
			{glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);}
		}
	~GLFWContext()
		{glfwTerminate();}
	};

class Window
	{
	public:
		typedef GLFWwindow* ContextHandle;

		Window(const Window&)=delete;
		Window& operator=(const Window&)=delete;

		Window()
			{m_handle=glfwCreateWindow(800,600,"",nullptr,nullptr);}

		~Window()
			{glfwDestroyWindow(m_handle);}

		auto contextCapture() noexcept
			{
			auto ret=glfwGetCurrentContext();
			glfwMakeContextCurrent(m_handle);
			return ret;
			}

		static void contextRelease(GLFWwindow* window) noexcept
			{glfwMakeContextCurrent(window);}

		auto handle() noexcept
			{return m_handle;}

		bool shouldClose() const noexcept
			{return glfwWindowShouldClose(m_handle);}

	private:
		GLFWwindow* m_handle;
	};

static constexpr GeoSIMD::Point<float> verts[]=
	{
	 GeoSIMD::Point<float>{0.5f,  0.5f, 0.0f}
	,GeoSIMD::Point<float>{0.5f, -0.5f, 0.0f}
	,GeoSIMD::Point<float>{-0.5f, -0.5f, 0.0f}
	,GeoSIMD::Point<float>{-0.5f,  0.5f, 0.0f}
	};

static constexpr uint16_t faces[]=
	{
	 0,1,3
	,1,2,3
	};

template<class T>
static constexpr const GeoSIMD::vec4_t<T>* native_type(const GeoSIMD::Point<T>* point_ptr)
	{return reinterpret_cast<const GeoSIMD::vec4_t<T>*>(point_ptr);}

struct MyShaderLayout
	{
	static constexpr Angle::VertexAttribute attributes[]=
		{
			{4,Angle::ConstantGet<float>::value}
		};
	};

constexpr Angle::VertexAttribute MyShaderLayout::attributes[];

struct RGB
	{
	uint8_t B;
	uint8_t G;
	uint8_t R;
	uint8_t A;
	};

static RGB g_texture[1200][1920];

static void textureFill()
	{
	for(size_t k=0;k<1200;++k)
		{
		for(size_t l=0;l<1920;++l)
			{
			g_texture[k][l].R=uint8_t(255*k/1200.0f);
			g_texture[k][l].G=uint8_t(255*l/1920.0f);
			g_texture[k][l].B=0;
			g_texture[k][l].A=255;
			}
		}
	}

inline constexpr auto gl_format(RGB)
	{return GL_BGRA;}

inline constexpr auto gl_type(RGB)
	{return GL_UNSIGNED_INT_8_8_8_8_REV;}

int main()
	{
	textureFill();
	GLFWContext glfw(Angle::gl_version_requirements());
	try
		{
		Window mainwin;
		Angle::ContextGuard<Window> context(mainwin);
		auto version=Angle::init();

		printf("%s, %s, %s, %s\n"
			,version.vendor
			,version.renderer
			,version.version
			,version.glsl_version);

		Angle::VertexBuffer<GeoSIMD::vec4_t<float>>	vertbuff(4);
		vertbuff.bufferData(native_type(verts),4);
		Angle::VertexBuffer<uint16_t> facebuff(6);
		facebuff.bufferData(faces,6);

		Angle::Texture2D texture(1,Angle::TextureFormat::SRGB8_ALPHA8 ,1920,1200);

		Angle::Program prgm(
R"EOF(#version 450 core
layout(location=0) in vec4 position;
out vec2 tex_coords;
void main()
	{
	gl_Position=position;
	tex_coords=position.xy + vec2(0.5,0.5);
	}
)EOF"_vert,R"EOF(#version 450 core
out vec4 color;
in vec2 tex_coords;
layout(location=0) uniform sampler2D texture_data;

void main()
	{
	color=texture(texture_data,tex_coords);
	}
)EOF"_frag);

		Angle::VertexArray<MyShaderLayout> vertex_array;
		vertex_array.vertexBuffer<0>(vertbuff).enableVertexAttrib<0>()
			.elementBuffer(facebuff);

		texture.dataSet(&g_texture[0][0],1920,1200);

		texture.bind(1);
		glEnable(GL_FRAMEBUFFER_SRGB);

		while(!mainwin.shouldClose())
			{
			glfwPollEvents();
			vertex_array.bind();
			prgm.bind();
			glUniform1i(0,1);
			Angle::drawElements(Angle::DrawMode::TRIANGLES,0,6);
			glfwSwapBuffers(mainwin.handle());
			}

		}
	catch(const Angle::Error& err)
		{
		fprintf(stderr,"Error: %s\n",err.message());
		return -1;
		}
	return 0;
	}
