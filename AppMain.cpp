#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GLEW/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Week 3 Milestone"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };
        
    GLFWwindow* gWindow = nullptr; // Main GLFW window
    GLMesh gMesh; // Triangle mesh data
    GLuint gProgramId; // Shader program
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

out vec4 vertexColor; // variable to transfer color data to the fragment shader

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vertexColor);
}
);


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(gWindow))
    {
        UProcessInput(gWindow);
        URender(); // Render this frame
        glfwPollEvents(); // poll for and process events
    }
       
    UDestroyMesh(gMesh); // Release mesh data
    UDestroyShaderProgram(gProgramId); // Release shader program
    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //major version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); //minor version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //telling opengl to only use modern

    // GLFW: window creation
    // ---------------------
    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window); /* Make the window's context current */
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    //Initalize GLEW
    if (glewInit() != GLEW_OK)
        cout << "Failed to initalize GLEW" << endl;
    
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;  // Displays GPU OpenGL version
    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //if user press the escape key, end the program
        glfwSetWindowShouldClose(window, true); //tell the program to end
}


//allow the size of the screen to be resized and the garphics to adjust with it.
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);  
}


// Functioned called to render a frame
void URender()
{
    glEnable(GL_DEPTH_TEST);  // Enable z-depth
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the frame and z buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Wireframe Mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Scales the object
    glm::mat4 rotation = glm::rotate(90.0f, glm::vec3(0.0, 1.0f, 0.0f)); // Rotates shape by x radians in the y axis (Pitch X, Yaw Y, Roll Z)
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)); // Place object at the origin

    glm::mat4 model = translation * rotation * scale; // Model matrix: transformations are applied right-to-left order
    glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f)); // Transforms the camera: move the camera back (z axis)
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f); // Creates a perspective projection

    glUseProgram(gProgramId); // Set the shader to be used

    // Select uniform shader and variable
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
   
    // Pass transform to Shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
       
    //User-defined VAO must be called before draw
    glBindVertexArray(gMesh.vao); // Activate the VBOs contained within the mesh's VAO

    //Draw Primitative
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, 0); // Draws the triangle 

    glBindVertexArray(0); // Deactivate the Vertex Array Object

    glfwSwapBuffers(gWindow);    /* Swap front and back buffers */
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions       // Colors (r,g,b,a)
        -0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,  // v0 front left 
         0.0f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f, 1.0f,  // v1 top of pyramid
         0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,  // v2 front right
        -0.5f, -0.5f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,  // v3 back left
         0.5f, -0.5f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f   // v4 back right
    };

    // Index data to share position data
    GLushort indices[] = {
        0,1,2, // Triangle 1
        0,3,1, // Triangle 2 
        3,1,4, // Triangle 3
        4,1,2, // Triangle 4
        0,3,4, // Triangle 5
        0,4,2, // Triangle 6

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(int);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{     
    programId = glCreateProgram();  // Create a Shader program object.

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    glCompileShader(fragmentShaderId); // compile the fragment shader

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);   // links the shader program
    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
