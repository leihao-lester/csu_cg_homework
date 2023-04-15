#include <csugl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glog/logging.h>

class FrameBuffer {
public:
    FrameBuffer(const glm::ivec2& target_size, unsigned int tex_ogl_id) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ogl_id, 0);
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, target_size.x, target_size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LOG(ERROR) << "FRAMEBUFFER:: Framebuffer is not complete!";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

    inline void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

private:
    unsigned int fbo;
    unsigned int rbo;
};