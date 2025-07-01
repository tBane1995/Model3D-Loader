#ifndef AnimatedModel3D_hpp
#define AnimatedModel3D_hpp

struct BoneKeyframe {
    FbxTime time;
    FbxVector4 translation;
    FbxVector4 rotation; // euler angles
    FbxVector4 scale;
};

struct BoneAnim {
    std::string name;
    std::vector<BoneKeyframe> keyframes;
};

struct AnimationClip {
    double duration; // in seconds
    std::map<std::string, BoneAnim> boneAnimations;
};

/*
BoneKeyframe Interpolate(const BoneAnim& anim, double timeInSec) {
    if (anim.keyframes.size() == 1)
        return anim.keyframes[0];

    for (size_t i = 0; i < anim.keyframes.size() - 1; ++i) {
        double t1 = anim.keyframes[i].time.GetSecondDouble();
        double t2 = anim.keyframes[i + 1].time.GetSecondDouble();
        if (timeInSec >= t1 && timeInSec <= t2) {
            double factor = (timeInSec - t1) / (t2 - t1);
            BoneKeyframe result;
            result.translation = anim.keyframes[i].translation + (anim.keyframes[i + 1].translation - anim.keyframes[i].translation) * factor;
            result.rotation = anim.keyframes[i].rotation + (anim.keyframes[i + 1].rotation - anim.keyframes[i].rotation) * factor;
            result.scale = anim.keyframes[i].scale + (anim.keyframes[i + 1].scale - anim.keyframes[i].scale) * factor;
            return result;
        }
    }

    return anim.keyframes.back();
}
*/

class AnimatedModel3D {
public:
    Transform transform;

    std::vector<Mesh> meshes;
    std::vector<std::pair<int, int>> mesh_draw_ranges; // offset, count

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    AnimationClip animation;
    double animationTime = 0.0;

    AnimatedModel3D() {
        meshes.clear();
    }
    ~AnimatedModel3D() {}

    void setPosition(float x, float y, float z) {
        transform.setPosition(glm::vec3(x, y, z));
    }

    void setScale(float x, float y, float z) {
        transform.setScale(glm::vec3(x, y, z));
    }

    void loadFBX() {

        meshes.clear();

        FbxManager* manager = FbxManager::Create();
        FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
        manager->SetIOSettings(ios);

        FbxImporter* importer = FbxImporter::Create(manager, "");
        if (!importer->Initialize("mdl\\knight.FBX", -1, manager->GetIOSettings())) {
            std::cerr << "Błąd: nie udało się wczytać modelu." << std::endl;
            return;
        }

        FbxScene* scene = FbxScene::Create(manager, "myScene");
        importer->Import(scene);
        importer->Destroy();

        FbxNode* root = scene->GetRootNode();
        if (!root) return;


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

                        

                        vertice vv;
                        const float globalFix = 0.1f; // zmniejsz 100×
                        double sceneScale = scene->GetGlobalSettings().GetSystemUnit().GetScaleFactor(); // np. 2.54
                        float scaleFactor = 1.0f / static_cast<float>(sceneScale);

                        vv.x = float(v[0]) * scaleFactor * globalFix;
                        vv.y = float(v[2]) * scaleFactor * globalFix;
                        vv.z = -float(v[1]) * scaleFactor * globalFix;

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

            // Rekurencja dla dzieci
            for (int i = 0; i < node->GetChildCount(); ++i)
                traverse(node->GetChild(i));
            };

        traverse(root);
        
        // Łączenie wszystkich meshów w jeden bufor
        std::vector<vertice> buffer_vertices;
        mesh_draw_ranges.clear();

        int offset = 0;
        for (const auto& mesh : meshes) {
            mesh_draw_ranges.emplace_back(offset, mesh.vertices.size());
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

    
    /*
    AnimationClip LoadFBXAnimation(FbxScene* scene) {
        AnimationClip clip;
        FbxAnimStack* animStack = scene->GetCurrentAnimationStack();
        if (!animStack) return clip;

        FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(0);
        FbxNode* rootNode = scene->GetRootNode();
        if (!rootNode) return clip;

        FbxTimeSpan timeSpan;
        scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeSpan);
        clip.duration = timeSpan.GetDuration().GetSecondDouble();

        std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
            BoneAnim boneAnim;
            boneAnim.name = node->GetName();

            FbxAnimCurve* tCurveX = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
            FbxAnimCurve* tCurveY = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
            FbxAnimCurve* tCurveZ = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

            int keyCount = tCurveX ? tCurveX->KeyGetCount() : 0;
            for (int i = 0; i < keyCount; ++i) {
                FbxTime time = tCurveX->KeyGetTime(i);

                BoneKeyframe kf;
                kf.time = time;
                kf.translation = node->LclTranslation.EvaluateValue(time);
                kf.rotation = node->LclRotation.EvaluateValue(time);
                kf.scale = node->LclScaling.EvaluateValue(time);
                boneAnim.keyframes.push_back(kf);
            }

            if (!boneAnim.keyframes.empty())
                clip.boneAnimations[boneAnim.name] = boneAnim;

            for (int i = 0; i < node->GetChildCount(); ++i)
                traverse(node->GetChild(i));
            };

        for (int i = 0; i < rootNode->GetChildCount(); ++i)
            traverse(rootNode->GetChild(i));

        return clip;
    }
    */

    void draw() {

        Program* program = getProgram(L"advanced program");

        glBindVertexArray(VAO);

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

            auto [offset, count] = mesh_draw_ranges[i];
            glDrawArrays(GL_TRIANGLES, offset, count);
            offset += meshes[i].vertices.size();
        }

        glBindVertexArray(0);
    }
};


#endif