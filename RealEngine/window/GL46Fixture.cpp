/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/GL46Fixture.hpp>

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL_video.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/Capabilities.hpp>
#include <RealEngine/rendering/output/Framebuffer.hpp>
#include <RealEngine/rendering/Ordering.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>

namespace RE {

void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "---OpenGL Callback Start---\n";
    std::cerr << "Message: " << message << '\n';
    std::cerr << "Type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: std::cerr << "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "DEPRECATED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "UNDEFINED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER: std::cerr << "MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: std::cerr << "PUSH GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP: std::cerr << "POP GROUP"; break;
    case GL_DEBUG_TYPE_OTHER: std::cerr << "OTHER"; break;
    default: std::cerr << "UNKNOWN";
    }
    std::cerr << '\n';

    std::cerr << "Id: " << id << '\n';
    std::cerr << "Severity: ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW: std::cerr << "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH: std::cerr << "HIGH"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "NOTIFICATION"; break;
    default: std::cerr << "UNKNOWN";
    }

    std::cerr << "\n----OpenGL Callback End----" << std::endl;
}

GL46Fixture::GL46Fixture(SDL_Window* sdlWindow) :
    m_glContext(createContext(sdlWindow)),
    m_defaultFramebufferLateBind(FramebufferID{0u}),
    m_defaultFramebufferGL46(FramebufferID{0u}) {
    //Initialize ImGui
    if (!ImGui_ImplSDL2_InitForOpenGL(sdlWindow, m_glContext)) {
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for OpenGL!"};
    };
    if (!ImGui_ImplOpenGL3_Init("#version 460 core")) {
        ImGui_ImplSDL2_Shutdown();
        throw std::runtime_error{"Could not initialize ImGui for OpenGL!"};
    }

    DefaultFrameBuffer<RendererLateBind>::s_defaultFramebuffer = &m_defaultFramebufferLateBind;
    DefaultFrameBuffer<RendererGL46>::s_defaultFramebuffer = &m_defaultFramebufferGL46;
}

GL46Fixture::~GL46Fixture() {
    DefaultFrameBuffer<RendererLateBind>::s_defaultFramebuffer = nullptr;
    DefaultFrameBuffer<RendererGL46>::s_defaultFramebuffer = nullptr;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    SDL_GL_DeleteContext(m_glContext);
}

void GL46Fixture::prepareFrame(bool useImGui) {
    if (useImGui) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}

void GL46Fixture::finishFrame(bool useImGui, SDL_Window* sdlWindow) {
    if (useImGui) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    SDL_GL_SwapWindow(sdlWindow);
}

SDL_GLContext GL46Fixture::createContext(SDL_Window* sdlWindow) {
    //Set attributes for the context
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
        throw std::runtime_error{"Could not use doublebuffer!"};
    }

    int contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
#ifndef NDEBUG
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif // DEBUG

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags)) {
        throw std::runtime_error{"Could not set context flags!"};
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
        throw std::runtime_error{"Could not use core profile context!"};
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)) {
        throw std::runtime_error{"Could not use OpenGL 4!"};
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6)) {
        throw std::runtime_error{"Could not use OpenGL 4.6!"};
    }

    //Create the context
    auto context = SDL_GL_CreateContext(sdlWindow);
    if (!context) {
        throw std::runtime_error{SDL_GetError()};
    }

    //Initialize GLEW
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error{"GLEW failed initialization!"};
    }

    //Print OpenGL info - should be OpenGL 4.6 (or higher? XD)
    std::printf("OpenGL:       %s\n", glGetString(GL_VERSION));
    std::printf("GLSL:         %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    std::printf("Renderer:     %s\n", glGetString(GL_RENDERER));
    std::printf("Vendor:       %s\n", glGetString(GL_VENDOR));

#ifndef NDEBUG
    //Enable OpenGL error callbacks
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint ids[] = {
            131185 //HORDCODE INGORE: Buffer object video memory notification
        };
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
    }
#endif // DEBUG

    //Disable dither that is probably no-op anyway
    glDisable(GL_DITHER);

    //Do not use multisample by default
    glDisable(GL_MULTISAMPLE);

    //Byte alignment
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //Primitive restart index
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

    //Use blenbing by default
    BlendingCapability<RendererGL46>::enable();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return context;
}

template<Renderer R>
void GL46Fixture::Implementations::assignReferences() {
    Buffer<R>::s_impl = &m_bufferImpl;
    Capabilities<R>::s_impl = &m_capabilitiesImpl;
    Framebuffer<R>::s_impl = &m_mainFramebufferImpl;
    Ordering<R>::s_impl = &m_orderingImpl;
    ShaderProgram<R>::s_impl = &m_shaderProgramImpl;
    Texture<R>::s_impl = &m_textureImpl;
    TextureProxy<R>::s_impl = &m_textureImpl;
    VertexArray<R>::s_impl = &m_vertexArrayImpl;
    Viewport<R>::s_impl = &m_viewportImpl;
    Viewport<R>::s_state = &m_viewportState;
}

template<Renderer R>
void GL46Fixture::Implementations::clearReferences() {
    Buffer<R>::s_impl = nullptr;
    Capabilities<R>::s_impl = nullptr;
    Framebuffer<R>::s_impl = nullptr;
    Ordering<R>::s_impl = nullptr;
    ShaderProgram<R>::s_impl = nullptr;
    Texture<R>::s_impl = nullptr;
    TextureProxy<R>::s_impl = nullptr;
    VertexArray<R>::s_impl = nullptr;
    Viewport<R>::s_impl = nullptr;
    Viewport<R>::s_state = nullptr;
}

void checkForOpenGLErrors() {
    GLenum err;
    do {
        err = glGetError();
        switch (err) {
        case GL_INVALID_ENUM: error("GL_INVALID_ENUM ENCOUNTERED!"); break;
        case GL_INVALID_VALUE: error("GL_INVALID_VALUE ENCOUNTERED!"); break;
        case GL_INVALID_OPERATION: error("GL_INVALID_OPERATION ENCOUNTERED!"); break;
        case GL_STACK_OVERFLOW: error("GL_STACK_OVERFLOW ENCOUNTERED!"); break;
        case GL_STACK_UNDERFLOW: error("GL_STACK_UNDERFLOW ENCOUNTERED!"); break;
        case GL_OUT_OF_MEMORY: error("GL_OUT_OF_MEMORY ENCOUNTERED!"); break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error("GL_INVALID_FRAMEBUFFER_OPERATION ENCOUNTERED!"); break;
        case GL_CONTEXT_LOST: error("GL_CONTEXT_LOST ENCOUNTERED!"); break;
        case GL_TABLE_TOO_LARGE: error("GL_TABLE_TOO_LARGE ENCOUNTERED!"); break;
        }
    } while (err != GL_NO_ERROR);
}

}