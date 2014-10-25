#include "../common/common.h"

#define STBI_HEADER_FILE_ONLY
#include "../buildin/stb_image.c"
#include "../buildin/stb_image_write.h"

#include "glbase.h"

#if defined _ANDROID
#include <AndroidGlueCompat.h>
#endif

#ifdef _DEBUG
void GLCheckError(const std::string& file, int line) {
    auto err = glGetError();
    switch (err) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
#ifdef _ANDROID
        	LOGW("Invalid enum at %s:%d", file.c_str(), line);
#else
            std::cout << "Invalid enum at " << file << ":" << line << std::endl;
#endif
            break;
        case GL_INVALID_VALUE:
#ifdef _ANDROID
        	LOGW("Invalid value at %s:%d", file.c_str(), line);
#else
            std::cout << "Invalid value at " << file << ":" << line << std::endl;
#endif
            break;
        case GL_INVALID_OPERATION:
#ifdef _ANDROID
        	LOGW("Invalid operation at %s:%d", file.c_str(), line);
#else
            std::cout << "Invalid operation at " << file << ":" << line << std::endl;
#endif
            break;
#if not defined BUILD_WITH_GLES
        case GL_STACK_OVERFLOW:
            std::cerr << "Stack overflow at " << file << ":" << line << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << "Stack underflow at " << file << ":" << line << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "Out of memory at " << file << ":" << line << std::endl;
            break;
        case GL_TABLE_TOO_LARGE:
            std::cerr << "Table to large at " << file << ":" << line << std::endl;
            break;
#endif
        default:
#ifdef _ANDROID
        	LOGW("Unknown error # %d at %s:%d", err,  file.c_str(), line);
#else
        	std::cout << "Unknown error #" << err << " at " << file << ":" << line << std::endl;
#endif
            break;
    }
}
#endif

namespace glbase {
    
    Shader::~Shader() {
        if(vert_shader)
            glDeleteShader(vert_shader);
        if(frag_shader)
            glDeleteShader(frag_shader);
        if(program)
            glDeleteProgram(program);
    }
    
    bool Shader::LoadVertShader(const char* buffer) {
        vert_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_shader, 1, &buffer, nullptr);
        glCompileShader(vert_shader);
        int result;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            std::cerr << "Vertex shader compilation failed!" << std::endl;
            GLint logLen;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &logLen);
            if(logLen > 0) {
                char* log = new char[logLen];
                GLsizei written;
                glGetShaderInfoLog(vert_shader, logLen, &written, log);
                std::cerr << "Shader log:" << log << std::endl;
                delete[] log;
            }
            glDeleteShader(vert_shader);
            vert_shader = 0;
            return false;
        }
        return true;
    }
    
    bool Shader::LoadFragShader(const char* buffer) {
        frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_shader, 1, &buffer, nullptr);
        glCompileShader(frag_shader);
        int result;
        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            std::cerr << "Fragment shader compilation failed!" << std::endl;
            GLint logLen;
            glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &logLen);
            if(logLen > 0) {
                char* log = new char[logLen];
                GLsizei written;
                glGetShaderInfoLog(frag_shader, logLen, &written, log);
                std::cerr << "Shader log:" << log << std::endl;
                delete[] log;
            }
            glDeleteShader(frag_shader);
            frag_shader = 0;
            return false;
        }
        return true;
    }
    
    bool Shader::Link() {
        program = glCreateProgram();
        glAttachShader(program, vert_shader);
        glAttachShader(program, frag_shader);
        glLinkProgram(program);
        int result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if(result == GL_FALSE) {
#ifdef _ANDROID
        	LOGI("Failed to link shader program!\n");
#else
            std::cerr << "Failed to link shader program!\n" << std::endl;
#endif
            GLint logLen;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
            if(logLen > 0) {
                char* log = new char[logLen];
                GLsizei written;
                glGetProgramInfoLog(program, logLen, &written, log);
#ifdef _ANDROID
                LOGI("Program log: %s", log);
#else
                std::cerr << "Program log: " << log << std::endl;
#endif
                delete[] log;
            }
            glDeleteProgram(program);
            program = 0;
            return false;
        }
        return true;
    }
    
    bool Shader::Use() {
        if(program) {
            glUseProgram(program);
            return true;
        }
        return false;
    }
    
    void Shader::SetParam1i(const char* varname, const int value) {
        auto loc = glGetUniformLocation(program, varname);
        if(loc >= 0)
            glUniform1i(loc, value);
    }
    
    void Shader::SetParamMat4(const char* varname, const float m[]) {
        auto loc = glGetUniformLocation(program, varname);
        if(loc >= 0)
            glUniformMatrix4fv(loc, 1, GL_FALSE, m);
    }
    
    void Shader::Unload() {
        if(vert_shader)
            glDeleteShader(vert_shader);
        if(frag_shader)
            glDeleteShader(frag_shader);
        if(program)
            glDeleteProgram(program);
        vert_shader = 0;
        frag_shader = 0;
        program = 0;
    }
    
    Shader& Shader::GetDefaultShader() {
        static Shader default_shader;
        static bool inited = false;
        static const char* vert_shader = "\
        #version 120\n\
        attribute vec2 v_position;\n\
        attribute vec4 v_color;\n\
        attribute vec2 v_texcoord;\n\
        varying vec4 color;\n\
        varying vec2 texcoord;\n\
        void main() {\n\
        color = v_color;\n\
        texcoord = v_texcoord;\n\
        gl_Position = vec4(v_position, 0.0, 1.0);\n\
        }\n\
        ";
        static const char* frag_shader = "\
        #version 120\n\
        varying vec4 color;\n\
        varying vec2 texcoord;\n\
        uniform sampler2D texid;\n\
        void main() {\n\
        vec4 texcolor = texture2D(texid, texcoord);\n\
        gl_FragColor = texcolor * color;\n\
        }\n\
        ";
        if(!inited) {
            default_shader.LoadVertShader(vert_shader);
            default_shader.LoadFragShader(frag_shader);
            default_shader.Link();
            inited = true;
        }
        return default_shader;
    }
    
    void Shader::Unuse() {
        glUseProgram(0);
    }
    
    Image::~Image() {
        if(buffer != nullptr)
            delete[] buffer;
    }
    
    bool Image::LoadFile(const std::string &file) {
        unsigned char* data = stbi_load(file.c_str(), &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete[] buffer;
        buffer = new unsigned char[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    bool Image::LoadMemory(const unsigned char* mem, unsigned int sz) {
        unsigned char* data = stbi_load_from_memory(mem, sz, &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete[] buffer;
        buffer = new unsigned char[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    void Texture::Load(const unsigned char* data, int x, int y) {
        if(texture_id)
            glDeleteTextures(1, &texture_id);
        tex_width = texlen(x);
        tex_height = texlen(y);
        img_width = x;
        img_height = y;
        glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(data) {
            unsigned char* px = new unsigned char[tex_width * tex_height * 4];
            memset(px, 0, tex_width * tex_height * 4);
            for(int h = 0; h < y; ++h)
                memcpy(&px[tex_width * 4 * h], &data[x * 4 * h], x * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
            delete[] px;
        } else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Unload() {
        if(!texture_id)
            return;
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    
    void Texture::Update(const unsigned char* data, int offx, int offy, int width, int height) {
        if(!texture_id)
            return;
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, offx, offy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Bind() {
        if(texture_id)
            glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    
    TextureInfo<4> Texture::GetTextureInfo() {
        static TextureInfo<4> ti;
        if(!texture_id)
            return ti;
        TextureInfo<4> ret;
        ret.vert[0] = {0.0f, 0.0f};
        ret.vert[1] = {(float)img_width / tex_width, 0.0f};
        ret.vert[2] = {0.0f, (float)img_height / tex_height};
        ret.vert[3] = {(float)img_width / tex_width, (float)img_height / tex_height};
        return ret;
    }
    
    Font::Font() {
        FT_Init_FreeType(&library);
    }
    
    Font::~Font() {
        FT_Done_FreeType(library);
        char_tex.Unload();
    }
    
    bool Font::Load(const std::string &file, unsigned int sz) {
        if (FT_New_Face(library, file.c_str(), 0, &face))
            return false;
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        FT_Set_Pixel_Sizes(face, 0, sz);
        char_tex.Load(nullptr, 2048, 2048);
        tex_posx = 0;
        tex_posy = 0;
        font_size = sz;
        return true;
    }

    void Font::Unload() {
        char_tex.Unload();
    }
	
    const FontGlyph& Font::GetGlyph(unsigned int ch) {
        auto& gl = glyphs[ch];
        if(gl.loaded)
            return gl;
        FT_UInt glyph_index = FT_Get_Char_Index(face, ch);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        gl.bounds = {face->glyph->bitmap_left, -face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows};
        gl.advance = face->glyph->advance.x / 64.0f;
        if(2048 - tex_posx < gl.bounds.width) {
            tex_posx = 0;
            tex_posy += font_size;
        }
        gl.textureRect = {tex_posx, tex_posy, gl.bounds.width, gl.bounds.height};
        unsigned int* px = new unsigned int[gl.bounds.width * gl.bounds.height];
        unsigned char* buf = face->glyph->bitmap.buffer;
        for(int i = 0; i < gl.bounds.width * gl.bounds.height; ++i)
            px[i] = (((unsigned int)buf[i]) << 24) | 0xffffff;
        char_tex.Update((const unsigned char*)px, tex_posx, tex_posy, gl.bounds.width, gl.bounds.height);
        tex_posx += gl.bounds.width;
        gl.loaded = true;
        delete[] px;
        return gl;
    }
}
