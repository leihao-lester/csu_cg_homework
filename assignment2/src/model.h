#include <csugl.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using csugl::ml::MTextureType;
using csugl::ml::MTexture;
using csugl::ml::MMaterial;
using csugl::ml::MMesh;
using csugl::ml::MModel;

struct Texture{
    // MTextureType type;
    int w, h, ch;
    unsigned int tex_ogl_id;
    
    // load from meta texture
    explicit Texture(csugl::Ref<MTexture> meta_tex)
        : w(meta_tex->width), h(meta_tex->height), ch(meta_tex->channel) {
        init(meta_tex->data.get());
    }

    // load from image file
    explicit Texture(const std::string& fileName){
        auto meta_data = stbi_load(fileName.c_str(), &w, &h, &ch, 0);
        init(meta_data);
    }

    Texture(int width, int height, int channel = 3, unsigned char *data = nullptr)
        : w(width), h(height), ch(channel) {
        init(data);
    }

    void bind() { glBindTexture(GL_TEXTURE_2D, tex_ogl_id); }

    private:
    void init(unsigned char* data) {
        glGenTextures(1, &tex_ogl_id);
        glBindTexture(GL_TEXTURE_2D, tex_ogl_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (ch == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (ch == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else if (ch == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        // gen mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
};

struct Material{
    glm::vec3 albedo = glm::vec3(1.0f);
    csugl::Ref<Texture> diff_tex;   // diffuse texture
    // more textures coming soon ...
    
    explicit Material(csugl::Ref<MMaterial> meta_material) {
        albedo = {meta_material->Kd[0], meta_material->Kd[1], meta_material->Kd[2]};
        if(meta_material->map_Kd) {
            diff_tex = csugl::MakeRef<Texture>(meta_material->map_Kd);
        }
    }
};

struct Mesh{
    GLuint vao, vbo, ebo;
    size_t vertex_num;
    size_t face_num;
    csugl::Ref<Material> material;

    explicit Mesh(csugl::Ref<MMesh> meta_mesh)
        : vao(0), vbo(0), ebo(0), vertex_num(meta_mesh->vertex_num),face_num(meta_mesh->indices_num) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        auto vert_size = 3 * vertex_num * sizeof(float);
        auto norm_size = 3 * vertex_num * sizeof(float);
        auto uvs_size = 2 * vertex_num * sizeof(float);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vert_size + norm_size + uvs_size, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vert_size, meta_mesh->vertices.get());
        glBufferSubData(GL_ARRAY_BUFFER, vert_size, norm_size, meta_mesh->normals.get());
        glBufferSubData(GL_ARRAY_BUFFER, vert_size + norm_size, uvs_size, meta_mesh->uvs.get());

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)(vert_size));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)(vert_size + norm_size));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * 3 * sizeof(unsigned int), meta_mesh->indices.get(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        // material
        material = csugl::MakeRef<Material>(meta_mesh->material);
    }

    void bind(){
        glBindVertexArray(vao);
    }
};

struct Model{
    std::vector<csugl::Ref<Mesh>> meshes;

    Model(csugl::Ref<MModel>meta_model){
        for(auto mesh : meta_model->meshes){
            meshes.push_back(csugl::MakeRef<Mesh>(mesh));
        }
    }
};

class AsyncModel
{
public:
    explicit AsyncModel(const std::string &fileName) {
        auto modelName = csugl::fs::get_file_name(fileName);
        auto thPool = csugl::singleton<csugl::thread_pool>::getInstance(csugl::get_core_numbers());
        auto loader = SGT_MODEL_LOADER();
        load_future = thPool->submit([loader, fileName, modelName, this]() {
            if (loader->pre_load(fileName, modelName)) {
                meta_model = loader->load(modelName);
            } else {
                LOG(ERROR) << "Model " << fileName << "load failed";
            }
        });
    }

    ~AsyncModel() {
        if(load_future.valid()) {
            load_future.wait();
        }
    }

    inline csugl::Ref<Model> getModel() const { return model; }

    inline bool isLoaded() const { return is_loaded; }

    bool tryLoad() {
        if (!is_loaded && meta_model) {
            model = csugl::MakeRef<Model>(meta_model);
            is_loaded = true;
            return true;
        } else if(is_loaded) {
            return true;
        }
        return false;
    }

private:
    csugl::Ref<Model> model;
    csugl::Ref<MModel> meta_model;
    std::future<void> load_future;
    bool is_loaded = false;
};
