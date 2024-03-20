#include "RenderSystem.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window)
{
	modelLocation = glGetUniformLocation(shader, "model");
	this->window = window;

    //Set material layers
    glUniform1i(glGetUniformLocation(shader, "diffuse"), 0);
    glUniform1i(glGetUniformLocation(shader, "mask"), 1);
    glUniform1i(glGetUniformLocation(shader, "normalMap"), 2);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), 0.86,0.7,0.73);


    //enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //create missing texture texture
    CreateMissingTexture();
}

void RenderSystem::Update(unordered_map<unsigned int, TransformComponent>& transformComponents, unordered_map<unsigned int, RenderComponent>& renderComponents)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (std::pair<unsigned int, RenderComponent> entity : renderComponents) {
    
        //if (entity.second.mesh == nullptr) continue;
        TransformComponent transform = transformComponents[entity.first];
        mat4 model;
        model = createTranslationMatrix(transform.position);
        model *= rotationZAxisMatrix(transform.eulers.z);
        model *= rotationYAxisMatrix(transform.eulers.y);
        model *= rotationXAxisMatrix(transform.eulers.x);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);
    
        unsigned int materialMask = 1;
        for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
        {
            if (entity.second.material == nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                break;
            }

            else if ((entity.second.material->materialMask & materialMask) == materialMask)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                if (entity.second.material->materials[i] == 0 && i == 0)
                {
                    glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                }
                else
                {
                  glBindTexture(GL_TEXTURE_2D, entity.second.material->materials[i]);
                }
            }
            materialMask *= 2;
        }
        glBindVertexArray(entity.second.mesh->VAO);
        glDrawArrays(GL_TRIANGLES, 0, entity.second.mesh->vertexCount);
    }

    glfwSwapBuffers(window);
}

void RenderSystem::CreateMissingTexture()
{
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
