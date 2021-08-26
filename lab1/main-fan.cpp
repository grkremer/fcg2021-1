//     Universidade Federal do Rio Grande do Sul
//             Instituto de Inform�tica
//       Departamento de Inform�tica Aplicada
//
//    INF01047 Fundamentos de Computa��o Gr�fica
//               Prof. Eduardo Gastal
//
//                   LABORAT�RIO 1
//

// Arquivos "headers" padr�es de C podem ser inclu�dos em um
// programa C++, sendo necess�rio somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo s�o espec�ficos de C++
#include <string>
#include <fstream>
#include <sstream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Cria��o de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Cria��o de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"

// Declara��o de v�rias fun��es utilizadas em main().  Essas est�o definidas
// logo ap�s a defini��o de main() neste arquivo.
GLuint BuildTriangles(); // Constr�i tri�ngulos para renderiza��o
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Fun��o utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

// Fun��es callback para comunica��o com o sistema operacional e intera��o do
// usu�rio. Veja mais coment�rios nas defini��es das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impress�o de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL vers�o 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto �, utilizaremos somente as
    // fun��es modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 500 colunas e 500 linhas
    // de pixels, e com t�tulo "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(500, 500, "INF01047 - 00290401 - Gustavo Ribeiro Kremer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a fun��o de callback que ser� chamada sempre que o usu�rio
    // pressionar alguma tecla do teclado.
    glfwSetKeyCallback(window, KeyCallback);

    // Definimos a fun��o de callback que ser� chamada sempre que a janela for
    // redimensionada, por consequ�ncia alterando o tamanho do "framebuffer"
    // (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Indicamos que as chamadas OpenGL dever�o renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas fun��es definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Imprimimos no terminal informa��es sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de v�rtices e de fragmentos que ser�o utilizados
    // para renderiza��o. Veja slides 176-196 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" est�o fixados, sendo que assumimos a exist�ncia
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_0X/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //       |
    //       o-- ...
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Criamos um programa de GPU utilizando os shaders carregados acima
    GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Constru�mos a representa��o de um tri�ngulo
    GLuint vertex_array_object_id = BuildTriangles();

    // Ficamos em um loop infinito, renderizando, at� que o usu�rio feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as opera��es de renderiza��o

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor �
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto �:
        // Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima
        glClear(GL_COLOR_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de v�rtice e fragmentos).
        glUseProgram(program_id);

        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // v�rtices apontados pelo VAO criado pela fun��o BuildTriangles(). Veja
        // coment�rios detalhados dentro da defini��o de BuildTriangles().
        glBindVertexArray(vertex_array_object_id);

        // Pedimos para a GPU rasterizar os v�rtices apontados pelo VAO como
        // tri�ngulos.
        //
        //                +--- Veja slides 124-130 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
        //                |          +--- O array "indices[]" cont�m 6 �ndices (veja fun��o BuildTriangles()).
        //                |          |  +--- Os �ndices s�o do tipo "GLubyte" (8 bits sem sinal)
        //                |          |  |                 +--- V�rtices come�am em indices[0] (veja fun��o BuildTriangles()).
        //                |          |  |                 |
        //                V          V  V                 V
        glDrawElements(GL_TRIANGLE_FAN, 18, GL_UNSIGNED_BYTE, 0);

        // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);

        // O framebuffer onde OpenGL executa as opera��es de renderiza��o n�o
        // � o mesmo que est� sendo mostrado para o usu�rio, caso contr�rio
        // seria poss�vel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usu�rio
        // tudo que foi renderizado pelas fun��es acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma intera��o do
        // usu�rio (teclado, mouse, ...). Caso positivo, as fun��es de callback
        // definidas anteriormente usando glfwSet*Callback() ser�o chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    return 0;
}

// Constr�i tri�ngulos para futura renderiza��o
GLuint BuildTriangles()
{
    // Primeiro, definimos os atributos de cada v�rtice.

    // A posi��o de cada v�rtice � definida por coeficientes em "normalized
    // device coordinates" (NDC), onde cada coeficiente est� entre -1 e 1.
    // (Veja slides 129-132 e 138-145 do documento Aula_03_Rendering_Pipeline_Grafico.pdf).
    // Nas aulas sobre transforma��es geom�trica veremos como transformar
    // coeficientes em outros sistemas de coordenadas para coeficientes NDC.
    //
    // Note que aqui adicionamos um quarto coeficiente W (igual a 1.0).
    // Conversaremos sobre isso quando tratarmos de coordenadas homog�neas.
    //
    // Este vetor "NDC_coefficients" define a GEOMETRIA (veja slides 64-71 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat NDC_coefficients[] = {
    //    X      Y     Z     W
         0.0f, 0.0f, 0.0f, 1.0f,
         0.7f, 0.0f, 0.0f, 1.0f,
         0.646715673f, 0.267878403f, 0.0f, 1.0f,
         0.494974747f, 0.494974747f, 0.0f, 1.0f,
         0.267878403f, 0.646715673f, 0.0f, 1.0f,
         0.0f,  0.7f, 0.0f, 1.0f,
         -0.267878403f, 0.646715673f, 0.0f, 1.0f,
         -0.494974747f, 0.494974747f, 0.0f, 1.0f,
         -0.646715673f, 0.267878403f, 0.0f, 1.0f,
         -0.7f,  0.0f, 0.0f, 1.0f,
         -0.646715673f, -0.267878403f, 0.0f, 1.0f,
         -0.494974747f, -0.494974747f, 0.0f, 1.0f,
         -0.267878403f, -0.646715673f, 0.0f, 1.0f,
         0.0f,  -0.7f, 0.0f, 1.0f,
         0.267878403f, -0.646715673f, 0.0f, 1.0f,
         0.494974747f, -0.494974747f, 0.0f, 1.0f,
         0.646715673f, -0.267878403f, 0.0f, 1.0f
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO �
    // um buffer de mem�ria que ir� conter os valores de um certo atributo de
    // um conjunto de v�rtices; por exemplo: posi��o, cor, normais, coordenadas
    // de textura. Neste exemplo utilizaremos v�rios VBOs, um para cada tipo de
    // atributo.  Agora criamos um VBO para armazenarmos um atributo: posi��o
    // (coeficientes NDC definidos acima).
    GLuint VBO_NDC_coefficients_id;
    glGenBuffers(1, &VBO_NDC_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // cont�m a defini��o de v�rios atributos de um certo conjunto de v�rtices;
    // isto �, um VAO ir� conter ponteiros para v�rios VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // est� contido na vari�vel "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID est� contido na
    // vari�vel VBO_NDC_coefficients_id ser� modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer � de fato um VBO, e
    // ir� conter atributos de v�rtices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);

    // Alocamos mem�ria para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "NDC_coefficients", pedimos
    // para alocar um n�mero de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" d� uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que n�o pretendemos alterar tais dados (s�o est�ticos: "STATIC"), e
    // tamb�m dizemos que tais dados ser�o utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array NDC_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);

    // Precisamos ent�o informar um �ndice de "local" ("location"), o qual ser�
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Tamb�m, informamos a dimens�o (n�mero de
    // coeficientes) destes atributos. Como em nosso caso s�o coordenadas NDC
    // homog�neas, temos quatro coeficientes por v�rtice (X,Y,Z,W). Isto define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados est�o em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta fun��o tamb�m informa que o VBO "ligado" acima em glBindBuffer()
    // est� dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com �ndice de local
    // definido acima, na vari�vel "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isto evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada v�rtice: uma cor (veja slides 107-110 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 72 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor � definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto �: Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
    GLfloat color_coefficients[] = {
    //  R     G     B     A
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos ent�o definir dois tri�ngulos utilizando os v�rtices do array
    // NDC_coefficients. O primeiro tri�ngulo � formado pelos v�rtices 0,1,2;
    // e o segundo pelos v�rtices 2,1,3. Note que usaremos o modo de renderiza��o
    // GL_TRIANGLES na chamada glDrawElements() dentro de main(). Veja slides 124-130 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 64-71 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLubyte indices[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1 }; // GLubyte: valores entre 0 e 255 (8 bits sem sinal).

    // Criamos um buffer OpenGL para armazenar os �ndices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora � GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos mem�ria para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // N�O fa�a a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de �ndices (GL_ELEMENT_ARRAY_BUFFER) n�o pode ser "desligado",
    // caso contr�rio o VAO ir� perder a informa��o sobre os �ndices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso � tudo que ser� necess�rio para renderizar
    // os tri�ngulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo. Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos v�rtices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo. Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Fun��o auxilar, utilizada pelas duas fun��es acima. Carrega c�digo de GPU de
// um arquivo GLSL e faz sua compila��o.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela vari�vel "filename"
    // e colocamos seu conte�do em mem�ria, apontado pela vari�vel
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o c�digo do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o c�digo do shader GLSL (em tempo de execu��o)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compila��o
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos mem�ria para guardar o log de compila��o.
    // A chamada "new" em C++ � equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compila��o
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ � equivalente ao "free()" do C
    delete [] log;
}

// Esta fun��o cria um programa de GPU, o qual cont�m obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Defini��o dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos mem�ria para guardar o log de compila��o.
        // A chamada "new" em C++ � equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "== End of link log\n";

        // A chamada "delete" em C++ � equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retorna o ID gerado acima
    return program_id;
}

// Defini��o da fun��o que ser� chamada sempre que a janela do sistema
// operacional for redimensionada, por consequ�ncia alterando o tamanho do
// "framebuffer" (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda regi�o do framebuffer. A
    // fun��o "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa � a opera��o de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slides 138-145 do documento Aula_03_Rendering_Pipeline_Grafico.pdf).
    glViewport(0, 0, width, height);
}

// Defini��o da fun��o que ser� chamada sempre que o usu�rio pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // ==============
    // N�o modifique este loop! Ele � utilizando para corre��o automatizada dos
    // laborat�rios. Deve ser sempre o primeiro comando desta fun��o KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ==============

    // Se o usu�rio pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Definimos o callback para impress�o de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// vim: set spell spelllang=pt_br :

