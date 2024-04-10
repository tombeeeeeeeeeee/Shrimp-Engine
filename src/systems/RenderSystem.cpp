#include "RenderSystem.h"

RenderSystem::RenderSystem(std::vector<unsigned int> _shaders, GLFWwindow* window)
{
    shaders = _shaders;

	modelLocation = glGetUniformLocation(shaders[1], "model");
	this->window = window;

    //Set material layers //This needs to be refactored to allow for different Shaders
    glUniform1i(glGetUniformLocation(shaders[1], "diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaders[1], "mask"), 1);
    glUniform1i(glGetUniformLocation(shaders[1], "normalMap"), 2);
    glUniform3f(glGetUniformLocation(shaders[1], "directionalLightColor"), 0.8,0.8,0.8);
    glUniform3f(glGetUniformLocation(shaders[1], "directionalLightDirection"), 0.86,0.7,0.73);
    glUniform3f(glGetUniformLocation(shaders[1], "ambientLightColor"), 1,0,0.1);
    glUniform1f(glGetUniformLocation(shaders[1], "ambientLightStrength"), 0.1);


    //enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //create missing texture texture
    CreateMissingTexture();
}

void RenderSystem::Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents, std::unordered_map<unsigned int, RenderComponent*>& renderComponents)
{
    //Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Check if render Component is in shader order array
    for (std::pair<unsigned int, RenderComponent*> entity : renderComponents)
    {
        entityShaderOrder[entity.second->material->shaderProgram].push_back(entity.first);
    }

    //Render all components In Shader Order
    for (int i = 0; i < shaderProgramCount; i++)
    {
        for (std::vector<unsigned int>::iterator iter = entityShaderOrder[i].begin(); iter != entityShaderOrder[i].end(); iter++)
        {
            //If the mesh is bugged, do not render. TO BE REPLACED WITH BROKEN MESH MESH
            if (renderComponents[*iter]->mesh == nullptr) continue;

            //Get transform pair's model transform
            TransformComponent transform = *transformComponents[*iter];
            mat4 model;
            model = transform.globalTransform;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);

            //For each texture with the render components material
            unsigned int materialMask = 1;
            for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
            {
                if (renderComponents[*iter]->material == nullptr || renderComponents[*iter]->material->materials[0] == 0)
                {
                    //Bind missing texture if the material doesnt exist.
                    glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                    break;
                }

                //if the current material/texture map matches the current binary
                else if ((renderComponents[*iter]->material->materialMask & materialMask) == materialMask)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    if (renderComponents[*iter]->material->materials[i] == 0 && i == 0)
                    {
                        //Bind missing texture if the diffuse doesnt exist.
                        glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                    }
                    else
                    {
                        glBindTexture(GL_TEXTURE_2D, renderComponents[*iter]->material->materials[i]);
                    }
                }
                materialMask *= 2;
            }

            //Bind mesh for drawing
            glBindVertexArray(renderComponents[*iter]->mesh->VAO);

            if (renderComponents[*iter]->mesh->IBO != 0)
                glDrawElements(GL_TRIANGLES, 3 * renderComponents[*iter]->mesh->triCount, GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, 3 * renderComponents[*iter]->mesh->triCount);
        }
    }

    glfwSwapBuffers(window);
    for (int i = 0; i < shaderProgramCount; i++) entityShaderOrder[i].clear;
}

void RenderSystem::CreateMissingTexture()
{
              //MAGENTA
    int data = 0xff00ff;

    //make the texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //load data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    missingTextureTexture = texture;
}
