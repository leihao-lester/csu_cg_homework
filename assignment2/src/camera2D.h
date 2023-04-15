#include <csugl.h>

// 正交相机 (for 2D)
struct Camera2D {
    Camera2D(
        glm::vec3 look_from, // 自身位置
        glm::vec3 look_at,   // 观察点
        glm::vec3 up,        // 上方向
        glm::vec2 size,      // 视口大小
        float _near,         // 近平面
        float _far           // 远平面
        )
        : look_from(look_from), look_at(look_at), up(up), size(size), _near(_near), _far(_far) {
    }

    glm::mat4 get_view_mat()  const {
        // 注: font方向，指的是摄像机方向向量，即从观察点指向自身位置，即摄像机空间z轴正方向
        //     right方向，指的是摄像机右轴方向向量，即摄像机空间y轴的正方向
        // 为获取右向量（摄像机空间y轴正方向）我们需要先使用一个小技巧：先定义一个上向量(Up Vector)。
        // 接下来把上向量和第二步得到的方向向量进行叉乘。两个向量叉乘的结果会同时垂直于两向量，
        // 因此我们会得到指向x轴正方向的那个向量（如果我们交换两个向量叉乘的顺序就会得到相反的指向x轴负方向的向量）。

        // 可以模仿csugl里面的camera类

        // 1.计算 front 方向
		// glm::vec3 front = glm::normalize(look_from - look_at);
        glm::vec3 front = glm::normalize(look_at - look_from );
        // 2.根据 up 和 front 计算 right
		glm::vec3 right = glm::normalize(glm::cross(up, front));
        // 3.根据 right 和 front 计算 实际的up 并 传入 glm::lookAt
        glm::vec3 cameraUp = glm::normalize(glm::cross(right, front));

        return glm::lookAt(look_from, look_at, cameraUp);
    }

    glm::mat4 get_projection_mat() const{
        // 根据 size 和 near,far , 使用 glm::ortho 计算投影矩阵并返回

        // glm::ortho(float left, float right, float bottom, float top, float zNear, float zFar); 
        // 前两个参数指定了平截头体的左右坐标，第三和第四参数指定了平截头体的底部和顶部。通过这四个参数我们定义了近平面和远平面的大小，然后第五和第六个参数则定义了近平面和远平面的距离。
        // glm::perspective(float fovy, float aspect, float zNear, float zFar); 
        // 第一个参数为视锥上下面之间的夹角(弧度），第二个参数为宽高比，即视窗的宽/高，第三第四个参数分别为近截面和远界面的深度
        // 可以模仿csugl里的camera.cpp类

        return glm::ortho(-size.x * 0.5f, size.x * 0.5f, -size.y * 0.5f, size.y*0.5f, _near, _far);
    }
    
    glm::vec3 look_from;
    glm::vec3 look_at;
    glm::vec3 up;
    glm::vec2 size;
    float _near, _far;
};
