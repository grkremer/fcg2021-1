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
    window = glfwCreateWindow(500, 500, "INF01047 - 290401 - Gustavo Ribeiro Kremer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    GLuint vertex_array_object_id = BuildTriangles();

    // Ficamos em um loop infinito, renderizando, at� que o usu�rio feche a janela
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program_id);
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
        int seconds = (int)glfwGetTime();
        if (seconds%2 == 0)
            glDrawElements(GL_TRIANGLE_FAN, 18, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 6));
        else
            glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 0));
        seconds = seconds/2;

        if (seconds%2 == 0)
            glDrawElements(GL_TRIANGLE_FAN, 18, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 30));
        else
            glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 24));

        seconds = seconds/2;

        if (seconds%2 == 0)
            glDrawElements(GL_TRIANGLE_FAN, 18, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 54));
        else
            glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 48));

        seconds = seconds/2;

        if (seconds%2 == 0)
            glDrawElements(GL_TRIANGLE_FAN, 18, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 78));
        else
            glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, (GLvoid*)(sizeof(GLubyte) * 72));

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
    float d0 = -0.75;
    float d1 = -0.25;
    float d2 = 0.25;
    float d3 = 0.75;

    float y1 = 2.5;
    float y2 = 0.8;


    GLfloat NDC_coefficients[] = {
    //    X      Y     Z     W
         0.0f+d3, 0.48f*y2, 0.0f, 1.0f,
         0.0f+d3, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d3, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d3, 0.6f*y2, 0.0f, 1.0f,
         0.0f+d3, 0.6f*y2, 0.0f, 1.0f,
         -0.1f+d3, 0.40f*y2, 0.0f, 1.0f,

         0.0f+d3, 0.0f*y1, 0.0f, 1.0f,
         0.2f+d3, 0.0f*y1, 0.0f, 1.0f,
         0.184775907f+d3, 0.076536686f*y1, 0.0f, 1.0f,
         0.141421356f+d3, 0.141421356f*y1, 0.0f, 1.0f,
         0.076536686f+d3, 0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d3, 0.2f*y1, 0.0f, 1.0f,
         -0.076536686f+d3, 0.184775907f*y1, 0.0f, 1.0f,
         -0.141421356f+d3, 0.141421356f*y1, 0.0f, 1.0f,
         -0.184775907f+d3, 0.076536686*y1, 0.0f, 1.0f,
         -0.2f+d3, 0.0f*y1, 0.0f, 1.0f,
         -0.184775907f+d3, -0.076536686f*y1, 0.0f, 1.0f,
         -0.141421356f+d3, -0.141421356f*y1, 0.0f, 1.0f,
         -0.076536686f+d3, -0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d3, -0.2f*y1, 0.0f, 1.0f,
         0.076536686f+d3, -0.184775907f*y1, 0.0f, 1.0f,
         0.141421356f+d3, -0.141421356f*y1, 0.0f, 1.0f,
         0.184775907f+d3, -0.076536686f*y1, 0.0f, 1.0f,

         0.0f+d2, 0.48f*y2, 0.0f, 1.0f,
         0.0f+d2, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d2, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d2, 0.6f*y2, 0.0f, 1.0f,
         0.0f+d2, 0.6f*y2, 0.0f, 1.0f,
         -0.1f+d2, 0.40f*y2, 0.0f, 1.0f,

         0.0f+d2, 0.0f*y1, 0.0f, 1.0f,
         0.2f+d2, 0.0f*y1, 0.0f, 1.0f,
         0.184775907f+d2, 0.076536686f*y1, 0.0f, 1.0f,
         0.141421356f+d2, 0.141421356f*y1, 0.0f, 1.0f,
         0.076536686f+d2, 0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d2, 0.2f*y1, 0.0f, 1.0f,
         -0.076536686f+d2, 0.184775907f*y1, 0.0f, 1.0f,
         -0.141421356f+d2, 0.141421356f*y1, 0.0f, 1.0f,
         -0.184775907f+d2, 0.076536686*y1, 0.0f, 1.0f,
         -0.2f+d2, 0.0f*y1, 0.0f, 1.0f,
         -0.184775907f+d2, -0.076536686f*y1, 0.0f, 1.0f,
         -0.141421356f+d2, -0.141421356f*y1, 0.0f, 1.0f,
         -0.076536686f+d2, -0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d2, -0.2f*y1, 0.0f, 1.0f,
         0.076536686f+d2, -0.184775907f*y1, 0.0f, 1.0f,
         0.141421356f+d2, -0.141421356f*y1, 0.0f, 1.0f,
         0.184775907f+d2, -0.076536686f*y1, 0.0f, 1.0f,

         0.0f+d1, 0.48f*y2, 0.0f, 1.0f,
         0.0f+d1, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d1, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d1, 0.6f*y2, 0.0f, 1.0f,
         0.0f+d1, 0.6f*y2, 0.0f, 1.0f,
         -0.1f+d1, 0.40f*y2, 0.0f, 1.0f,

         0.0f+d1, 0.0f*y1, 0.0f, 1.0f,
         0.2f+d1, 0.0f*y1, 0.0f, 1.0f,
         0.184775907f+d1, 0.076536686f*y1, 0.0f, 1.0f,
         0.141421356f+d1, 0.141421356f*y1, 0.0f, 1.0f,
         0.076536686f+d1, 0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d1, 0.2f*y1, 0.0f, 1.0f,
         -0.076536686f+d1, 0.184775907f*y1, 0.0f, 1.0f,
         -0.141421356f+d1, 0.141421356f*y1, 0.0f, 1.0f,
         -0.184775907f+d1, 0.076536686*y1, 0.0f, 1.0f,
         -0.2f+d1, 0.0f*y1, 0.0f, 1.0f,
         -0.184775907f+d1, -0.076536686f*y1, 0.0f, 1.0f,
         -0.141421356f+d1, -0.141421356f*y1, 0.0f, 1.0f,
         -0.076536686f+d1, -0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d1, -0.2f*y1, 0.0f, 1.0f,
         0.076536686f+d1, -0.184775907f*y1, 0.0f, 1.0f,
         0.141421356f+d1, -0.141421356f*y1, 0.0f, 1.0f,
         0.184775907f+d1, -0.076536686f*y1, 0.0f, 1.0f,

         0.0f+d0, 0.48f*y2, 0.0f, 1.0f,
         0.0f+d0, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d0, -0.6f*y2, 0.0f, 1.0f,
         0.15f+d0, 0.6f*y2, 0.0f, 1.0f,
         0.0f+d0, 0.6f*y2, 0.0f, 1.0f,
         -0.1f+d0, 0.40f*y2, 0.0f, 1.0f,

         0.0f+d0, 0.0f*y1, 0.0f, 1.0f,
         0.2f+d0, 0.0f*y1, 0.0f, 1.0f,
         0.184775907f+d0, 0.076536686f*y1, 0.0f, 1.0f,
         0.141421356f+d0, 0.141421356f*y1, 0.0f, 1.0f,
         0.076536686f+d0, 0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d0, 0.2f*y1, 0.0f, 1.0f,
         -0.076536686f+d0, 0.184775907f*y1, 0.0f, 1.0f,
         -0.141421356f+d0, 0.141421356f*y1, 0.0f, 1.0f,
         -0.184775907f+d0, 0.076536686*y1, 0.0f, 1.0f,
         -0.2f+d0, 0.0f*y1, 0.0f, 1.0f,
         -0.184775907f+d0, -0.076536686f*y1, 0.0f, 1.0f,
         -0.141421356f+d0, -0.141421356f*y1, 0.0f, 1.0f,
         -0.076536686f+d0, -0.184775907f*y1, 0.0f, 1.0f,
         0.0f+d0, -0.2f*y1, 0.0f, 1.0f,
         0.076536686f+d0, -0.184775907f*y1, 0.0f, 1.0f,
         0.141421356f+d0, -0.141421356f*y1, 0.0f, 1.0f,
         0.184775907f+d0, -0.076536686f*y1, 0.0f, 1.0f

    };

    GLuint VBO_NDC_coefficients_id;
    glGenBuffers(1, &VBO_NDC_coefficients_id);

    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);


    glBindVertexArray(vertex_array_object_id);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);

    glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);

    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(location);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat color_coefficients[] = {
    //  R     G     B     A
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f

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

    GLubyte indices[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,7,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,30,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,53,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,76,93,94,95 }; // GLubyte: valores entre 0 e 255 (8 bits sem sinal).

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

