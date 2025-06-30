#ifndef Shaders_hpp
#define Shaders_hpp

const char* vertex_shader_source = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    
    out vec2 TexCoord;
    out vec4 fragPosition;

    void main()
    {
        TexCoord = aTexCoord;
        fragPosition = view * model * vec4(aPos, 1.0);
        gl_Position = projection * fragPosition;
    }
)";

const char* fragment_shader_source = R"(
    #version 330 core
        
    in vec2 TexCoord;
    in vec4 fragPosition;

    uniform sampler2D texture1; // Tekstura
    
    out vec4 FragColor;

    void main()
    {
        // Pobranie koloru tekstury
        vec3 textureColor = texture(texture1, TexCoord).rgb;

        // Finalny kolor z uwzględnieniem mgły
        FragColor = vec4(textureColor, 1.0);
    } 
)";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* vertex_shader_with_fog_source = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    
    out vec2 TexCoord;
    out vec4 fragPosition;

    void main()
    {
        TexCoord = aTexCoord;
        fragPosition = view * model * vec4(aPos, 1.0);
        gl_Position = projection * fragPosition;
    }
)";

const char* fragment_shader_with_fog_source = R"(
    #version 330 core
        
    in vec2 TexCoord;
    in vec4 fragPosition;

    uniform sampler2D texture1; // Tekstura
    uniform vec3 fogColor;
    uniform float fogStart;
    uniform float fogEnd;
    
    out vec4 FragColor;

    void main()
    {
        // Obliczenie współczynnika mgły
        float fogFactor = (fogEnd - abs(fragPosition.z)) / (fogEnd - fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        
        // Pobranie koloru tekstury
        vec3 textureColor = texture(texture1, TexCoord).rgb;

        // Finalny kolor z uwzględnieniem mgły
        FragColor = vec4(mix(fogColor, textureColor, fogFactor), 1.0);
    } 
)";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* snowflake_vertex_shader_source = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec2 texcoords;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    
    out vec2 TexCoords;
    out vec4 FragPosition;

    void main()
    {
        TexCoords = texcoords;
        FragPosition = view * model * vec4(position, 1.0);
        gl_Position = projection * FragPosition;
    }
)";

const char* snowflake_fragment_shader_source = R"(
    #version 330 core
        
    in vec2 TexCoords;
    in vec4 FragPosition;

    uniform sampler2D tex; // Tekstura płatka śniegu
    uniform vec4 fogColor;
    uniform float fogStart;
    uniform float fogEnd;

    out vec4 FragColor;

    void main()
    {
        // Pobranie koloru tekstury (z kanałem alfa)
        vec4 texColor = texture(tex, TexCoords);

        // Jeśli piksel jest prawie niewidoczny -> pomijamy go
        if (texColor.a < 0.1)
            discard;

        // Obliczenie współczynnika mgły na podstawie głębi fragmentu
        float fogFactor = (fogEnd - gl_FragCoord.z) / (fogEnd - fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);

        // Finalny kolor: mgła + tekstura + przezroczystość
        FragColor = mix(fogColor, texColor, fogFactor);
        FragColor.a = texColor.a; // Utrzymanie przezroczystości płatka
    }
)";

const char* vertex_shader_with_light_source = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    layout (location = 2) in vec3 aNormal;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;

    out vec2 TexCoord;
    out vec3 Normal;
    out vec3 FragPosition;

    void main()
    {
        TexCoord = aTexCoord;
        FragPosition = vec3(model * vec4(aPos, 1.0)); // Transformacja pozycji wierzchołka do przestrzeni świata
        //Normal = mat3(transpose(inverse(model))) * aNormal; // Transformacja normalnych do przestrzeni świata
        Normal = aNormal;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragment_shader_with_light_source = R"(
    #version 330 core

    in vec2 TexCoord;
    in vec3 Normal;
    in vec3 FragPosition;

    uniform sampler2D texture1;

    uniform vec3 LightPosition;
    uniform vec3 LightColor;
    uniform float LightPower;
    uniform float LightRange;
   
    uniform vec3 Ka;
    uniform vec3 Ks;    
    uniform vec3 Ke;    

    out vec4 FragColor;

    void main()
    {
        // Diffuse Light - światło rozproszone
        // Specular Light - światło odbite
        // Ambient - światło bazowe
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPosition - FragPosition);
        float diff = max(dot(norm, lightDir)*LightRange, 0.0f);
        vec3 diffuse = LightColor * LightPower * diff;
        
        // Specular
        vec3 specular = Ks;

        // Ambient
        vec3 ambient = Ka;
        
        // Texture
        vec3 textureColor = texture(texture1, TexCoord).rgb;

        // Finalny kolor
        vec3 result = (diffuse + specular + ambient) * textureColor;
        FragColor = vec4(result, 1.0);
    }
)";

#endif
