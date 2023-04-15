#include "transform.h"
#include "primitive.h"

#include <glad/glad.h>

#include <glog/logging.h>

///////////////////////
/// Quad
///////////////////////

Quad::Quad() { init(1.0f); }

Quad::Quad(int width, int height) {
    CHECK(height > 0) << "Quad: invalid height value: " << height;
    init(static_cast<float>(width) / height);
}

Quad::~Quad() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

// 绘制四边形
void Quad::draw(csugl::Ref<csugl::Shader> shader, const Transform& trans) const {    
    shader->use();
    // 传入当前变换的矩阵
    shader->set_mat4("_model", trans.get_trans_mat());
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

// 按 宽/长 比例生成顶点数据
void Quad::init(float ratio) {
    float vertices[] = {
        //坐标                           //纹理坐标  
        -0.5f * ratio,  -0.5f, 0.0f,     0.0f, 0.0f, 
         0.5f * ratio,  -0.5f, 0.0f,     1.0f, 0.0f, 
         0.5f * ratio,   0.5f, 0.0f,     1.0f, 1.0f, 
        -0.5f * ratio,   0.5f, 0.0f,     0.0f, 1.0f, 
    };
    uint32_t indices[] = {
        0, 1, 2, 
        2, 3, 0, 
    };
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, vertices, GL_STATIC_DRAW);
    // 设置两个顶点属性，分别是 position 和 texCoords
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(sizeof(float) * 3));
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);
}

///////////////////////
/// Circle
///////////////////////

// 在此实现圆
#define CircleSampleNumber 80 // 圆周顶点个数
Circle::Circle() {
    //原始顶点数据
    float vertices[(CircleSampleNumber + 1) * 5];
    float radius = 0.4f;
    float PI = 3.1415926f;
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;
    // 注意此处是(0.5f,0.5f)，而不是(0.0f,0.0f)
    vertices[3] = 0.5f; 
    vertices[4] = 0.5f;
    for (int i = 1; i <= CircleSampleNumber; i++)
    {
        vertices[i * 5] = radius * cos(9 * i * 2 * PI / 360.0);
        vertices[i * 5 + 1] = radius * sin(9 * i * 2 * PI / 360.0);
        vertices[i * 5 + 2] = 0.0f;
        
        // 方法一：截取（0，1）区间内的圆所占的部分进行纹理坐标映射
        vertices[i * 5 + 3] = (cos(9 * i * 2 * PI / 360.0) + 1.0f) * 0.5f;
        vertices[i * 5 + 4] = (sin(9 * i * 2 * PI / 360.0) +  1.0f) * 0.5f;

        // 方法二：将圆周中心映射到正方向的四个边

    }
    unsigned int indices[CircleSampleNumber * 3];
    for (unsigned int i = 1; i < CircleSampleNumber; i++)
    {
        indices[(i - 1) * 3] = 0;
        indices[(i - 1) * 3 + 1] = i;
        indices[(i - 1) * 3 + 2] = i + 1;
    }
    indices[(CircleSampleNumber - 1) * 3] = 0;
    indices[(CircleSampleNumber - 1) * 3 + 1] = 1;
    indices[(CircleSampleNumber - 1) * 3 + 2] = 40;

    //顶点数组对象 Vertex Array Object
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    //顶点缓冲对象 Vertex Buffer Object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //索引缓冲对象 Eelement Buffer Object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //设置顶点属性（告诉GPU如何理解vertices），包括position 和 texCoords
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(sizeof(float) * 3));
}

Circle::~Circle() {
    // 不要忘记释放 vao vbo ibo
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void Circle::draw(csugl::Ref<csugl::Shader> shader, const Transform &trans) const {
    // 激活shader程序
    shader->use();
    // 传入当前变换的矩阵
    shader->set_mat4("_model", trans.get_trans_mat());
    // 绑定当前要绘制的顶点数组
    glBindVertexArray(vao);
    // 绘制
    glDrawElements(GL_TRIANGLES, CircleSampleNumber * 3, GL_UNSIGNED_INT, 0);
}