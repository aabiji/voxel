#pragma once

class Framebuffer {
public:
    Framebuffer() { }
    ~Framebuffer();

    void init(int width, int height);
    void enable();
    void disable();

private:
    unsigned int m_texture;
    // frame buffer object for sampling color
    unsigned int m_fbo;
    // render buffer object for depth & stencil testing
    unsigned int m_rbo;
};
