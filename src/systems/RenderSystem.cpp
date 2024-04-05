#include "RenderSystem.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window)
{
	modelLocation = glGetUniformLocation(shader, "model");
	this->window = window;

    //Set material layers //This needs to be refactored to allow for different Shaders
    glUniform1i(glGetUniformLocation(shader, "diffuse"), 0);
    glUniform1i(glGetUniformLocation(shader, "mask"), 1);
    glUniform1i(glGetUniformLocation(shader, "normalMap"), 2);
    glUniform3f(glGetUniformLocation(shader, "directionalLightColor"), 0.8,0.8,0.8);
    glUniform3f(glGetUniformLocation(shader, "directionalLightDirection"), 0.86,0.7,0.73);
    glUniform3f(glGetUniformLocation(shader, "ambientLightColor"), 1,0,0.1);
    glUniform1f(glGetUniformLocation(shader, "ambientLightStrength"), 0.1);


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
    
    //For each Render Component
    for (std::pair<unsigned int, RenderComponent*> entity : renderComponents) {
    
        //If the mesh is bugged, do not render. TO BE REPLACED WITH BROKEN MESH MESH
        if (entity.second->mesh == nullptr) continue;

        //Get transform pair's model transform
        TransformComponent transform = *transformComponents[entity.first];
        mat4 model;
        model = transform.globalTransform;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);
    
        //For each texture with the render components material
        unsigned int materialMask = 1;
        for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
        {
            if (entity.second->material == nullptr || entity.second->material->materials[0] == 0)
            {
                //Bind missing texture if the material doesnt exist.
                glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                break;
            }

            //if the current material/texture map matches the current binary
            else if ((entity.second->material->materialMask & materialMask) == materialMask)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                if (entity.second->material->materials[i] == 0 && i == 0)
                {
                    //Bind missing texture if the diffuse doesnt exist.
                    glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, entity.second->material->materials[i]);
                }
            }
            materialMask *= 2;
        }

        //Bind mesh for drawing
        glBindVertexArray(entity.second->mesh->VAO);

        if(entity.second->mesh->IBO != 0)
            glDrawElements(GL_TRIANGLES, 3 * entity.second->mesh->triCount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0,  3 * entity.second->mesh->triCount);
    }

    glfwSwapBuffers(window);
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
