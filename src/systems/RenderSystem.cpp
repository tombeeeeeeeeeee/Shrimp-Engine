#include "RenderSystem.h"

RenderSystem::RenderSystem(std::vector<unsigned int>& _shaders, unsigned int _cameraID, GLFWwindow* window)
{
    shaders = &_shaders;
    cameraID = _cameraID;

	modelLocation = glGetUniformLocation((*shaders)[0], "model");
	this->window = window;

    //enable alpha blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //create missing texture texture
    CreateMissingTexture();
}

void RenderSystem::Update(
    std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
    std::unordered_map<unsigned int, RenderComponent*>& renderComponents,
    std::unordered_map<unsigned int, LightComponent*>& lightComponents,
    mat4& projection, mat4& view)
{
    projectionMatrix = projection;
    viewMatrix = view;

    vec3 cameraPos = transformComponents[cameraID]->Position();

    //Clear Buffers
    glClear(GL_DEPTH_BUFFER_BIT);
    
    if (skyboxTexture != 0) DrawSkyBox();

    //Check if render Component is in shader order array
    for (std::pair<unsigned int, RenderComponent*> entity : renderComponents)
    {
        if(entity.second->material != nullptr)
            entityShaderOrder[entity.second->material->shaderProgram].push_back(entity.first);
    }

    //Render all components in Shader Order
    for (int i = 0; i < SHRIMP_SHADER_PROGRAM_COUNT; i++)
    {
        glUseProgram((*shaders)[i]);

        if (i != SHRIMP_SHADER_PROGRAM_COUNT - 1)
        {
            modelLocation = glGetUniformLocation((*shaders)[i], "model");

            //Set material layers //This needs to be refactored to allow for different Shaders
            glUniform1i(glGetUniformLocation((*shaders)[i], "diffuse"), 0);
            glUniform1i(glGetUniformLocation((*shaders)[i], "specular"), 1);
            glUniform1i(glGetUniformLocation((*shaders)[i], "normalMap"), 2);

            BindLightUniform((*shaders)[i], lightComponents, transformComponents);
        }

        glUniformMatrix4fv(glGetUniformLocation((*shaders)[i], "view"), 1, GL_FALSE, view.entries);
        glUniformMatrix4fv(glGetUniformLocation((*shaders)[i], "projection"), 1, GL_FALSE, projection.entries);

        glUniform3f(glGetUniformLocation((*shaders)[i], "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        for (std::vector<unsigned int>::iterator iter = entityShaderOrder[i].begin(); iter != entityShaderOrder[i].end(); iter++)
        {
            //If the mesh is bugged, do not render. TO BE REPLACED WITH BROKEN MESH MESH
            if (renderComponents[*iter]->mesh == nullptr) continue;

            //Get transform pair's model transform
            TransformComponent transform = *transformComponents[*iter];
            mat4 model = transform.globalTransform;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);

            glBindTexture(GL_TEXTURE_2D, 0);

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
    for (int i = 0; i < SHRIMP_SHADER_PROGRAM_COUNT; i++) entityShaderOrder[i].clear();
}

void RenderSystem::BindLightUniform(unsigned int shaderProgram, 
    std::unordered_map<unsigned int, LightComponent*>& lightComponents, 
    std::unordered_map<unsigned int, TransformComponent*>& transformComponents)
{
    
    int packetCount = 0;
    std::vector<float> lightPackets;
    std::unordered_map<unsigned int, LightComponent*>::iterator iter;

    for (iter = lightComponents.begin(); iter != lightComponents.end(); iter++)
    {
        //Colour
        lightPackets.push_back(iter->second->colour.x);
        lightPackets.push_back(iter->second->colour.y);
        lightPackets.push_back(iter->second->colour.z);
        //Light Type
        lightPackets.push_back((float)iter->second->lightType);

        switch(iter->second->lightType)
        {
            case LightType::ambient:
                packetCount += 1;
                break;

            case LightType::directional:
                //Direction (needs to be changed to the transfrom's forward vector)
                lightPackets.push_back(iter->second->direction.x);
                lightPackets.push_back(iter->second->direction.y);
                lightPackets.push_back(iter->second->direction.z);

                lightPackets.push_back(0);//Unused value
                packetCount += 2;
                break;

            case LightType::point:
                //Position
                lightPackets.push_back(transformComponents[iter->first]->Position().x);
                lightPackets.push_back(transformComponents[iter->first]->Position().y);
                lightPackets.push_back(transformComponents[iter->first]->Position().z);
                lightPackets.push_back(0); //Unused value

                //Attenuation Constants
                lightPackets.push_back(1);
                lightPackets.push_back(iter->second->linear);
                lightPackets.push_back(iter->second->quad);
                lightPackets.push_back(iter->second->range); //unused in calculations
                packetCount += 3;
                break;

            case LightType::spot:
                //Position
                lightPackets.push_back(transformComponents[iter->first]->Position().x);
                lightPackets.push_back(transformComponents[iter->first]->Position().y);
                lightPackets.push_back(transformComponents[iter->first]->Position().z);
                lightPackets.push_back(0); //Unused value


                //Direction (needs to be changed to the transfrom's forward vector)
                lightPackets.push_back(iter->second->direction.x);
                lightPackets.push_back(iter->second->direction.y);
                lightPackets.push_back(iter->second->direction.z);
                lightPackets.push_back(0); 
                
                //Attentuation and Angle calcs
                lightPackets.push_back(iter->second->cutOff);
                lightPackets.push_back(iter->second->linear);
                lightPackets.push_back(iter->second->quad);
                lightPackets.push_back(iter->second->outerCutOff);
                packetCount += 4;
        }      
    }

    unsigned int lights = glGetUniformLocation(shaderProgram, "lightPackets");
    glUniform4fv(lights, packetCount, lightPackets.data());

    glUniform1i(glGetUniformLocation(shaderProgram, "lightPacketCount"), packetCount);
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

void RenderSystem::DrawSkyBox()
{
    glDisable(GL_DEPTH_TEST);

    unsigned int emptyVAO;
    glUseProgram((*shaders)[SHRIMP_SHADER_PROGRAM_COUNT - 1]);

    mat4 inversePV = GetInverse(projectionMatrix * viewMatrix);

    glUniformMatrix4fv(glGetUniformLocation((*shaders)[SHRIMP_SHADER_PROGRAM_COUNT - 1], "PV"), 1, GL_FALSE, inversePV.entries);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    glGenVertexArrays(1, &emptyVAO);
    glBindVertexArray(emptyVAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
 