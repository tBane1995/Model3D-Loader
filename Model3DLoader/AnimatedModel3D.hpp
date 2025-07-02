#ifndef AnimatedModel3D_hpp
#define AnimatedModel3D_hpp

struct AnimationVertice {
    Mesh* mesh = nullptr;
    int vertice_index;
    float x, y, z;
};

struct Frame {
    float time;
    std::vector < AnimationVertice > move_vertices;
};

class AnimatedModel3D {
public:
    Transform transform;

    std::vector<Mesh> meshes;

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    std::vector<Frame> frames;
    std::vector<Mesh> animated_meshes;
    unsigned int current_frame;

    FbxManager* manager = nullptr;
    FbxIOSettings* ios = nullptr;
    FbxScene* modelScene = nullptr;

    AnimatedModel3D() {
        meshes.clear();
        current_frame = 0;

        manager = FbxManager::Create();

        ios = FbxIOSettings::Create(manager, IOSROOT);
        manager->SetIOSettings(ios);
    }
    ~AnimatedModel3D() {}

    void setPosition(float x, float y, float z) {
        transform.setPosition(glm::vec3(x, y, z));
    }

    void setScale(float x, float y, float z) {
        transform.setScale(glm::vec3(x, y, z));
    }

    void updateVerticesBuffer() {
        // Łączenie wszystkich meshów w jeden bufor
        std::vector<vertice> buffer_vertices;

        for (const auto& mesh : meshes)
            buffer_vertices.insert(buffer_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
     

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

    void loadFBX(std::wstring pathfile) {

        meshes.clear();

        FbxImporter* importer = FbxImporter::Create(manager, "");
        if (!importer->Initialize(ConvertWideToUtf8(pathfile).c_str(), -1, manager->GetIOSettings())) {
            std::wcout << L"Błąd: nie udało się wczytać animacji: " << pathfile << L"\n";
            return;
        }

        modelScene = FbxScene::Create(manager, "myScene");

        importer->Import(modelScene);
        importer->Destroy();

        FbxNode* root = modelScene->GetRootNode();
        if (!root) return;

        // load the Meshes
        std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
            FbxMesh* mesh = node->GetMesh();
            if (mesh) {
                Mesh m;
                
                const int polygonCount = mesh->GetPolygonCount();
                FbxVector4* controlPoints = mesh->GetControlPoints();
                FbxGeometryElementUV* uvElement = mesh->GetElementUV();
                FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();

                for (int polyIndex = 0; polyIndex < polygonCount; ++polyIndex) {
                    for (int vert = 0; vert < 3; ++vert) {
                        int ctrlIndex = mesh->GetPolygonVertex(polyIndex, vert);

                        // --- V ---
                        FbxVector4 v = controlPoints[ctrlIndex];

                        // --- UV ---
                        FbxVector2 uv(0, 0);
                        bool unmapped = false;
                        if (uvElement)
                            mesh->GetPolygonVertexUV(polyIndex, vert, uvElement->GetName(), uv, unmapped);

                        // --- Normal ---
                        FbxVector4 n(0, 1, 0);
                        if (normalElement) {
                            int idx = polyIndex * 3 + vert;
                            if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                                n = normalElement->GetDirectArray().GetAt(idx);
                            else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                                int nidx = normalElement->GetIndexArray().GetAt(idx);
                                n = normalElement->GetDirectArray().GetAt(nidx);
                            }
                        }

                        FbxDouble3 localScale = node->LclScaling.Get();
                        double toMeters = FbxSystemUnit::m.GetConversionFactorFrom(modelScene->GetGlobalSettings().GetSystemUnit());

                        vertice vv;
                        vv.x = float(v[0] * localScale[0] * toMeters);
                        vv.y = float(v[2] * localScale[2] * toMeters);
                        vv.z = -float(v[1] * localScale[1] * toMeters);

                        vv.u = float(uv[0]);
                        vv.v = float(uv[1]);

                        vv.nx = float(n[0]);
                        vv.ny = float(n[2]);
                        vv.nz = -float(n[1]);

                        m.vertices.push_back(vv);
                    }
                }

                m.material = getMaterial(L"DemoKnightMaterial");
                meshes.push_back(m);
            }

            for (int i = 0; i < node->GetChildCount(); ++i)
                traverse(node->GetChild(i));
            };

        traverse(root);

        updateVerticesBuffer();
    }

    void ProcessNode(FbxNode* pNode, FbxAnimLayer* pAnimLayer, FbxTime time) {
        if (!pNode) {
            return;
        }
        
        // mesh is nullptr so make a metrices
        FbxAnimEvaluator* anim_eval = pNode->GetAnimationEvaluator();

        FbxAMatrix localTransform = anim_eval->GetNodeLocalTransform(pNode, time);
        
        FbxVector4 t = localTransform.GetT(); // translacja
        FbxVector4 r = localTransform.GetR(); // rotacja
        FbxVector4 s = localTransform.GetS(); // skala

        std::cout << "Node: " << pNode->GetName() << "\n";
        std::cout << "  Local T: " << t[0] << ", " << t[1] << ", " << t[2] << "\n";
        std::cout << "  Local R: " << r[0] << ", " << r[1] << ", " << r[2] << "\n";
        std::cout << "  Local S: " << s[0] << ", " << s[1] << ", " << s[2] << "\n";

        //
        // teraz przypisac wierzcholki do modelScene
        // modelScene
        //

        /*
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 400.0f);
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, transform.position);
        model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform.scale);
        */

        /////////////

        ////////////

        int childCount = pNode->GetChildCount();
        for (int i = 0; i < childCount; i++) {
            ProcessNode(pNode->GetChild(i), pAnimLayer, time);
        }
    }
    
    void loadFBXAnimation(std::wstring pathfile) {

        FbxImporter* importer = FbxImporter::Create(manager, "");
        if (!importer->Initialize(ConvertWideToUtf8(pathfile).c_str(), -1, manager->GetIOSettings())) {
            std::wcout << L"Błąd: nie udało się wczytać animacji: " << pathfile << L"\n";
            return;
        }
        
        FbxScene* animScene = FbxScene::Create(manager, "animation");

        importer->Import(animScene);
        importer->Destroy();

        FbxAnimStack* animStack = animScene->GetSrcObject<FbxAnimStack>(0);
        if (!animStack) {
            std::cout << "Brak animacji\n";
            return;
        }

        int animCount = animScene->GetSrcObjectCount<FbxAnimStack>();
        if (animCount == 0) {
            std::cout << "Brak animacji\n";
            return;
        }


        for (int i = 0; i < animCount; i++) {
            FbxAnimStack* animStack = animScene->GetSrcObject<FbxAnimStack>(i);
            FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(0);

            // ...

            std::cout << "load animation no " << i << " : \"" << animStack->GetName() << "\"\n";

            animScene->SetCurrentAnimationStack(animStack);

            // show time of animation
            FbxTakeInfo* takeInfo = animScene->GetTakeInfo(animStack->GetName());
            std::cout << "anim start: " << takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble() << "s \n";
            std::cout << "anim stop: " << takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble() << "s \n";
            
            // ...

            FbxTime currentTime = takeInfo->mLocalTimeSpan.GetStart();
            FbxTime frameTime;
            frameTime.SetTime(0, 0, 0, 1, 0, animScene->GetGlobalSettings().GetTimeMode()); // klatka co 1 frame

            FbxNode* root = animScene->GetRootNode();
            if (!root) {
                std::cout << "rootNode is nullptr\n";
                return;
            }
                

            while (currentTime <= takeInfo->mLocalTimeSpan.GetStop()) {
                static int i = 0;
                std::cout << "frame: " << i++ << "\ttime: " << currentTime.GetSecondDouble() << "s\n";
                
                ProcessNode(root, animLayer, currentTime);
                currentTime += frameTime;
            }

        }
    }
    
    void animate() {

        //std::cout << "frame: " << current_frame << "\n";
        /*
        if (frames.size() < 3000) {
            Frame f;
            f.time = current_time;
            for (auto& m : meshes) {
                for (int i = 0; i < m.vertices.size(); i++) {
                    AnimationVertice av;
                    av.mesh = &m;
                    av.vertice_index = i;
                    av.x = m.vertices[i].x;
                    av.y = m.vertices[i].y;
                    av.z = m.vertices[i].z + 0.001f;
                    f.move_vertices.push_back(av);
                }
            }
            frames.push_back(f);
        }

        */

        if (frames.empty() || meshes.empty())
            return;

        const Frame& frame = frames[current_frame];

            for (auto& mv : frame.move_vertices) {
                mv.mesh->vertices[mv.vertice_index].x = mv.x;
                mv.mesh->vertices[mv.vertice_index].y = mv.y;
                mv.mesh->vertices[mv.vertice_index].z = mv.z;
            }

            updateVerticesBuffer();

            current_frame++;
            if (current_frame >= frames.size()) {
                current_frame = 0;
            }
    }
    

    void update() {
        animate();
    }

    void draw() {

        Program* program = getProgram(L"advanced program");

        glBindVertexArray(VAO);
        
        unsigned int offset = 0;

        for (int i = 0; i < meshes.size(); ++i) {


            glUseProgram(program->shader_program);

            glActiveTexture(GL_TEXTURE0);

            (meshes[i].material != nullptr) ? glBindTexture(GL_TEXTURE_2D, meshes[i].material->texture->id) : glBindTexture(GL_TEXTURE_2D, 0);

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