#pragma once
#include "tinyddsloader.h"
#include "stdafx.h"
#include <iostream>

#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_COMPRESSED_RED_RGTC1_EXT 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1_EXT 0x8DBC
#define GL_COMPRESSED_RED_GREEN_RGTC2_EXT 0x8DBD
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT 0x8DBE
#endif /* GL_EXT_texture_compression_s3tc */

using namespace tinyddsloader;
namespace tinyDDS
{
    struct GLSwizzle {
        GLenum m_r, m_g, m_b, m_a;
    };

    struct GLFormat {
        DDSFile::DXGIFormat m_dxgiFormat;
        GLenum m_type;
        GLenum m_format;
        GLSwizzle m_swizzle;
    };

    bool TranslateFormat(DDSFile::DXGIFormat fmt, GLFormat* outFormat);
    bool IsCompressed(GLenum fmt);
    bool LoadGLTexture(GLuint tex, DDSFile& dds);
}