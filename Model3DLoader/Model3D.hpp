#ifndef Model3D_hpp
#define Model3D_hpp


struct vertice {
    float x, y, z;
    float u, v;
    float nx, ny, nz;
};

struct Mesh {
    std::vector<vertice> vertices;
    Material* material = nullptr;
};


class Model3D {
public:

    Transform transform;

    std::vector<float> v;    // pozycje
    std::vector<float> vt;   // UV
    std::vector<float> vn;   // normalne

    std::vector<Mesh> meshes;

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Model3D() {
        v.clear();
        vt.clear();
        vn.clear();
        meshes.clear();
    }

    ~Model3D() {

    }

    void setPosition(float x, float y, float z) {
        transform.setPosition(glm::vec3(x, y, z));
    }

    void setScale(float x, float y, float z) {
        transform.setScale(glm::vec3(x, y, z));
    }

    void loadObj(std::wstring path) {

        meshes.clear();
        v.clear(); 
        vt.clear(); 
        vn.clear();

        std::wifstream file(path);
        if (!file.is_open()) {
            std::cout << "Can't open file: " << ConvertWideToUtf8(path) << "\n";
            return;
        }

        std::wstring line;
        Mesh currentMesh;
        currentMesh.material = nullptr;

        while (std::getline(file, line)) {
            std::wistringstream ss(line);
            std::wstring prefix;
            ss >> prefix;

            if (prefix == L"v") {
                float x, y, z;
                ss >> x >> y >> z;
                v.insert(v.end(), { x, y, z });
            }
            else if (prefix == L"vt") {
                float u, t;
                ss >> u >> t;
                vt.insert(vt.end(), { u, t });
            }
            else if (prefix == L"vn") {
                float nx, ny, nz;
                ss >> nx >> ny >> nz;
                vn.insert(vn.end(), { nx, ny, nz });
            }
            else if (prefix == L"usemtl") {
                std::wstring mtl;
                ss >> mtl;
                if (!currentMesh.vertices.empty()) {
                    meshes.push_back(currentMesh);
                    currentMesh.vertices.clear();
                }

                currentMesh.material = getMaterial(mtl);
                   
            }
            else if (prefix == L"f") {
                std::wstring vertexStr;
                std::vector<vertice> triangle;

                while (ss >> vertexStr) {
                    std::wistringstream vs(vertexStr);
                    std::wstring viStr, vtiStr, vniStr;
                    int vi = 0, vti = 0, vni = 0;

                    std::getline(vs, viStr, L'/');
                    if (!viStr.empty()) vi = std::stoi(viStr);
                    if (vs.peek() == L'/') vs.get();
                    if (std::getline(vs, vtiStr, L'/') && !vtiStr.empty()) vti = std::stoi(vtiStr);
                    if (std::getline(vs, vniStr) && !vniStr.empty()) vni = std::stoi(vniStr);

                    vertice vert = {};
                    if (vi > 0) {
                        vert.x = v[(vi - 1) * 3 + 0];
                        vert.y = v[(vi - 1) * 3 + 1];
                        vert.z = v[(vi - 1) * 3 + 2];
                    }
                    if (vti > 0 && vt.size() >= (vti * 2)) {
                        vert.u = vt[(vti - 1) * 2 + 0];
                        vert.v = vt[(vti - 1) * 2 + 1];
                    }
                    if (vni > 0 && vn.size() >= (vni * 3)) {
                        vert.nx = vn[(vni - 1) * 3 + 0];
                        vert.ny = vn[(vni - 1) * 3 + 1];
                        vert.nz = vn[(vni - 1) * 3 + 2];
                    }
                    triangle.push_back(vert);
                }

                if (triangle.size() >= 3) {
                    for (size_t i = 1; i + 1 < triangle.size(); ++i) {
                        currentMesh.vertices.push_back(triangle[0]);
                        currentMesh.vertices.push_back(triangle[i]);
                        currentMesh.vertices.push_back(triangle[i + 1]);
                    }
                }
            }
        }

        if (!currentMesh.vertices.empty()) {
            meshes.push_back(currentMesh);
        }

        // Łączenie wszystkich meshów w jeden bufor
        std::vector<vertice> buffer_vertices;        
        int offset = 0;
        for (const auto& mesh : meshes) {
            buffer_vertices.insert(buffer_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
            offset += mesh.vertices.size();
        }

        // VAO i VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, buffer_vertices.size() * sizeof(vertice), buffer_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertice), (void*)offsetof(vertice, x));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertice), (void*)offsetof(vertice, u));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertice), (void*)offsetof(vertice, nx));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
    
    void draw() {

        Program* program = getProgram(L"advanced program");

        glBindVertexArray(VAO);
        
        unsigned int offset = 0;

        for (int i = 0; i < meshes.size(); ++i) {

            glUseProgram(program->shader_program);

            glActiveTexture(GL_TEXTURE0);

            (meshes[i].material != nullptr)? glBindTexture(GL_TEXTURE_2D, meshes[i].material->texture->id) : glBindTexture(GL_TEXTURE_2D, 0);
          
            glUniform1i(glGetUniformLocation(program->shader_program, "texture1"), 0);
            glUniform3f(glGetUniformLocation(program->shader_program, "fogColor"), 0.25f, 0.25f, 0.25f);
            glUniform1f(glGetUniformLocation(program->shader_program, "fogStart"), 1.0f);
            glUniform1f(glGetUniformLocation(program->shader_program, "fogEnd"), 200.0f);

            // send metrices
            glm::mat4 view = cam->GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 400.0f);
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, transform.position);
            model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, transform.scale);

            glUniformMatrix4fv(glGetUniformLocation(program->shader_program, "projection"), 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(program->shader_program, "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(program->shader_program, "model"), 1, GL_FALSE, &model[0][0]);

            glm::vec3 lightPos = glm::vec3(-10, 10, 0);
            glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            static float lightPower = 0.1f;
            //lightPower += 0.00005f;   
            static float lightRange = 10.0f;
            //lightRange += 0.0001f;
            


            // Pozycje światła i kamery
            glUniform3f(glGetUniformLocation(program->shader_program, "LightPosition"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3fv(glGetUniformLocation(program->shader_program, "LightColor"), 1, &lightColor[0]);
            glUniform1f(glGetUniformLocation(program->shader_program, "LightPower"), lightPower);
            glUniform1f(glGetUniformLocation(program->shader_program, "LightRange"), lightRange);

            glUniform3fv(glGetUniformLocation(program->shader_program, "Ka"), 1, &meshes[i].material->Ka[0]);
            glUniform3fv(glGetUniformLocation(program->shader_program, "Ks"), 1, &meshes[i].material->Ks[0]);
            //glUniform3fv(glGetUniformLocation(program->shader_program, "Ke"), 1, &meshes[i].material->Ks[0]);

            glDrawArrays(GL_TRIANGLES, offset, meshes[i].vertices.size());
            offset += meshes[i].vertices.size();
        }

        glBindVertexArray(0);
    }
};


#endif