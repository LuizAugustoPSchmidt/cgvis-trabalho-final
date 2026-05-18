#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define SPACESHIP_MATERIAL       3
#define SPACESHIP_MOTOR          4
#define SPACESHIP_CASCO_ESCURO_1 5
#define SPACESHIP_CASCO          6
#define SPACESHIP_CSACO_ESCURO   7
#define SPACESHIP_FUNDO          8
#define SPACESHIP_TURBINA        9
#define SPACESHIP_FUNDO_2        10
#define SPACESHIP_QUEIMADO       11
#define SPACESHIP_PINTURA        12
#define SPACESHIP_LUZ_TURBINA    13
#define SPACESHIP_CABINE         14
#define SPACESHIP_PONTA          15
#define SPACESHIP_VIDRO          16
#define SPACESHIP_MATERIAL_001   17
#define BACKGROUND               18
#define ASTEROID                 19
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

	// Coeficiente de refletância difusa
	vec3 Kd0 = vec3(0.8, 0.8, 0.8);

    if ( object_id == SPHERE || object_id == BACKGROUND || object_id == ASTEROID )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
        // o slides 134-150 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // A esfera que define a projeção deve estar centrada na posição
        // "bbox_center" definida abaixo.

        // Você deve utilizar:
        //   função 'length( )' : comprimento Euclidiano de um vetor
        //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   função 'asin( )'   : seno inverso.
        //   constante M_PI
        //   variável position_model

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 d = position_model - bbox_center;

        float rho   = length(d);
        float theta = atan(d.x,d.z);
        float phi   = asin(d.y / rho);

        U = (theta + M_PI) / 2.0 / M_PI;
        V = (phi + M_PI_2) / M_PI;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		if (object_id == SPHERE)
            Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        else if (object_id == BACKGROUND)
            Kd0 = texture(TextureImage2, vec2(U*2.0, V*2.0)).rgb;
        else // ASTEROID
            Kd0 = texture(TextureImage3, vec2(U, V)).rgb;
    }
    else if ( object_id == BUNNY )
    {
        // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
        // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
        // o slides 99-104 do documento Aula_20_Mapeamento_de_Texturas.pdf,
        // e também use as variáveis min*/max* definidas abaixo para normalizar
        // as coordenadas de textura U e V dentro do intervalo [0,1]. Para
        // tanto, veja por exemplo o mapeamento da variável 'p_v' utilizando
        // 'h' no slides 158-160 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // Veja também a Questão 4 do Questionário 4 no Moodle.

        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage1
		Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == SPACESHIP_MATERIAL )
    {
        Kd0 = vec3(0.800000, 0.800000, 0.800000);
    }
    else if ( object_id == SPACESHIP_MOTOR )
    {
        Kd0 = vec3(0.175291, 0.175291, 0.175291);
    }
    else if ( object_id == SPACESHIP_CASCO_ESCURO_1 )
    {
        Kd0 = vec3(0.127605, 0.127605, 0.127605);
    }
    else if ( object_id == SPACESHIP_CASCO )
    {
        Kd0 = vec3(0.822921, 0.805622, 0.821381);
    }
    else if ( object_id == SPACESHIP_CSACO_ESCURO )
    {
        Kd0 = vec3(0.169657, 0.180242, 0.208220);
    }
    else if ( object_id == SPACESHIP_FUNDO )
    {
        Kd0 = vec3(0.085026, 0.085026, 0.085026);
    }
    else if ( object_id == SPACESHIP_TURBINA )
    {
        Kd0 = vec3(0.238216, 0.238216, 0.238216);
    }
    else if ( object_id == SPACESHIP_FUNDO_2 )
    {
        Kd0 = vec3(0.000000, 0.000000, 0.000000);
    }
    else if ( object_id == SPACESHIP_QUEIMADO )
    {
        Kd0 = vec3(0.095645, 0.095645, 0.095645);
    }
    else if ( object_id == SPACESHIP_PINTURA )
    {
        Kd0 = vec3(0.451585, 0.045113, 0.028330);
    }
    else if ( object_id == SPACESHIP_LUZ_TURBINA )
    {
        Kd0 = vec3(0.800630, 0.005851, 0.005851);
    }
    else if ( object_id == SPACESHIP_CABINE )
    {
        Kd0 = vec3(0.353214, 0.349111, 0.424897);
    }
    else if ( object_id == SPACESHIP_PONTA )
    {
        Kd0 = vec3(0.123313, 0.123313, 0.123313);
    }
    else if ( object_id == SPACESHIP_VIDRO )
    {
        Kd0 = vec3(0.455459, 0.597785, 0.760162);
    }
    else if ( object_id == SPACESHIP_MATERIAL_001 )
    {
        Kd0 = vec3(0.010219, 0.010219, 0.010219);
    }

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));

    if (object_id == BACKGROUND)
        color.rgb = Kd0;
    else
        color.rgb = Kd0 * (lambert + 0.01);

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 
