#version 330 core
layout (location = 0) in vec3 aPos;
//in vec2 position;
//in vec3 color;
//in vec2 texcoord;

//out vec3 Color;
//out vec2 Texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
    //Color = color;
    //Texcoord = texcoord;
	gl_Position =  proj * view * model * vec4(aPos, 1.0);
    
}









//view:
//To create the view transformation, GLM offers the useful glm::lookAt function that 
//simulates a moving camera. The first parameter specifies the position of the camera, the second the point to be 
//centered on-screen and the third the up axis. Here up is defined as the Z axis, which implies that the XY plane is the "ground".
//glm::mat4 view = glm::lookAt(
 //   glm::vec3(1.2f, 1.2f, 1.2f),
 //   glm::vec3(0.0f, 0.0f, 0.0f),
 //   glm::vec3(0.0f, 0.0f, 1.0f)
//);
//GLint uniView = glGetUniformLocation(shaderProgram, "view");
//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));



//model: transform naszego obiektu

//GLint uniTrans = glGetUniformLocation(ourShader.ID, "model");
//auto t_now = std::chrono::high_resolution_clock::now();
//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

//glm::mat4 trans;
//trans = glm::rotate(
 //   trans,
 //   time * glm::radians(180.0f),
 //   glm::vec3(0.0f, 0.0f, 1.0f)
//);
//glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));




//proj:
//Similarly, GLM comes with the glm::perspective function to create a perspective projection matrix. 
//The first parameter is the vertical field-of-view, 
//the second parameter the aspect ratio of the screen and the last two parameters are the near and far planes.

//glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
//GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
//glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));