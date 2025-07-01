#ifndef Materials_hpp
#define Materials_hpp

class Material {
public:

    std::wstring name;
    float Ns;               // Ns - Specular exponent (shininess)
    glm::vec3 Ka, Ks, Ke;   // Ka, Ks, Ke - Ambient, Specular, Emissive Color
    float Ni;               // Ni - Optical density
    float d;                // d - Dissolve
    
    int illum;          // illum – modele oświetlenia
                        // illum 1 – tylko kolor podstawowy i ambient(bez refleksów).
                        // illum 2 – kolor podstawowy + ambient + specular(odbicia światła).
    
    Texture* texture;   // 	map_Kd - Diffuse texture map

    Material(std::wstring name) {
        
        this->name = name;
        texture = getTexture(L"tex\\green.png");
    }

    ~Material() {};
    
};

std::vector < Material* > materials;

void addMaterial(Material* mtl) {
    materials.push_back(mtl);
}

void addLibMaterials(std::wstring path) {

    std::wifstream file(path);
    if (!file.is_open()) {
        std::cout << "Can't open file: " << ConvertWideToUtf8(path) << "\n";
        return;
    }

    std::wstring line;
    Material* mtl = nullptr;

    while (std::getline(file, line)) {
        std::wistringstream ss(line);
        std::wstring prefix;
        ss >> prefix;

        //std::wcout << line << "\n";

        if (prefix == L"newmtl") {
            if(mtl != nullptr)
                addMaterial(mtl);

            std::wstring name;
            ss >> name;
            mtl = new Material(name);
        }
        else if (prefix == L"map_Kd") {
            std::wstring tname;
            ss >> tname;
            //std::wcout << tname << L"\n";
            mtl->texture = getTexture(tname);
        }
        else if (prefix == L"Ka") {
            ss >> mtl->Ka.x;
            ss >> mtl->Ka.y;
            ss >> mtl->Ka.z;
        }
        else if (prefix == L"Ks") {
            ss >> mtl->Ks.x;
            ss >> mtl->Ks.y;
            ss >> mtl->Ks.z;
        }
    }

    addMaterial(mtl);

    file.close();

    std::wcout << "\n";
}

Material* getMaterial(std::wstring name) {
    for (auto& m : materials)
        if (m->name == name)
            return m;

    std::wcout << "mtl no exists: " << name << "\n";
    return nullptr;
}
#endif