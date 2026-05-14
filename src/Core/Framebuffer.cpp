#include "Framebuffer.hpp"

Framebuffer::Framebuffer(const unsigned int W, const unsigned int H)
:   width(W),
    height(H),
    samples(0)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //attachments:
    //texture (main color output)
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //allocate empty memory (not immutable, so we can resize) -> glTexStorage2D() is immutable
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, __null);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //so we dont get weird artifacts when doing kernel effects
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    //render object
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    //32 bits, 24 for depth buffer 8 for stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W, H);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

Framebuffer::Framebuffer(const unsigned int W, const unsigned int H, unsigned int samples)
:   width(W),
    height(H),
    samples(samples)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //attachments:
    //texture (main color output)
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);

    //allocate empty memory (not immutable, so we can resize) -> glTexStorage2D() is immutable8D56)
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, W, H, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

    //render object
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    //32 bits, 24 for depth buffer 8 for stencil buffer
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, W, H);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &RBO);
}

GLenum Framebuffer::check_status()
{
    bind();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return status;
}

void Framebuffer::resize(unsigned int W, unsigned int H)
{
    if(!samples){
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, __null);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W, H);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }else
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, W, H, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, W, H);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

unsigned int Framebuffer::get_width() {return width;}

unsigned int Framebuffer::get_height() {return height;}

unsigned int Framebuffer::get_texture_handle() {return texture;}

void Framebuffer::bind() {glBindFramebuffer(GL_FRAMEBUFFER, FBO);}

void Framebuffer::bind_as_read() {glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);}

void Framebuffer::bind_as_draw() {glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);}
