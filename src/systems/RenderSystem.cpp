#include "RenderSystem.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window)
{
	modelLocation = glGetUniformLocation(shader, "model");
	this->window = window;

    //Set material layers
    glUniform1i(glGetUniformLocation(shader, "material"), 0);
    glUniform1i(glGetUniformLocation(shader, "mask"), 1);

    //enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::Update(unordered_map<unsigned int, TransformComponent>& transformComponents, unordered_map<unsigned int, RenderComponent>& renderComponents)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (std::pair<unsigned int, RenderComponent> entity : renderComponents) {
    
        TransformComponent& transform = transformComponents[entity.first];
        mat4 model;
        model = createTranslationMatrix(transform.position);
        model *= rotationYAxisMatrix(transform.eulers.z);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);
    
        unsigned int materialMask = 1;
        for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
        {
            if (entity.second.materials[i] != 0 && (entity.second.materialMask & materialMask == materialMask))
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, entity.second.materials[i]);
            }
            materialMask *= 2;
        }

        glBindVertexArray(entity.second.mesh);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
}
