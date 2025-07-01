#ifndef Textures_hpp
#define Textures_hpp

class Texture {
public:

    std::wstring path;
    unsigned int id;
    int width, height;  
    int channels;       // GL_RGBA / GL_RGB

    Texture(std::wstring path, unsigned int id) {
        
        this->path = path;
        this->id = id;

        glGenTextures(1, &this->id);
        glBindTexture(GL_TEXTURE_2D, this->id);

        // texture repeating
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // set the texture filter with mipmap support
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load the texture
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load(ConvertWideToUtf8(path + L".png").c_str(), &width, &height, &channels, 0);

        if (data) {
            GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);  // mip-map create a textures ..., 64x64, 32x32, 16x16, 8x8, 4x4, 2x2, 1x1
            stbi_image_free(data);
        }
        else
            std::wcout << L"Failed to load texture: " << path << "\n";
    };

    ~Texture() {};
    
};

std::vector < Texture* > textures;

void addTexture(std::wstring path) {
    textures.push_back(new Texture(path, textures.size()));
    //std::wcout << L"loaded texture: " << path << "\n";
}

Texture* getTexture(std::wstring path) {

    for (auto& t : textures)
        if (t->path == path)
            return t;

    return nullptr;
}
#endif