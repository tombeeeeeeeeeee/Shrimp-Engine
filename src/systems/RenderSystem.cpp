#include "RenderSystem.h"

#include "Debug.h"




RenderSystem::RenderSystem(std::vector<unsigned int>& _shaders, unsigned int _cameraID, GLFWwindow* window)
{
    shaders = &_shaders;
    cameraID = _cameraID;

	modelLocation = glGetUniformLocation((*shaders)[Shader::defaultPBR], "model");
	this->window = window;

    //enable alpha blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //create missing texture texture
}

void RenderSystem::Start(unsigned int _skyboxTexture)
{
    skyboxTexture = _skyboxTexture;
    IBLBufferSetup(skyboxTexture);
    HDRBufferSetUp();
    CreateMissingTexture();
    BloomSetup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glUseProgram((*shaders)[defaultPBR]);
}

void RenderSystem::Update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents,
    std::unordered_map<unsigned int, LightComponent>& lightComponents,
    glm::mat4& projection, glm::mat4& view, unsigned int* renderTexture)
{
   glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    projectionMatrix = projection;
    viewMatrix = view;

    glm::vec3 cameraPos =
    {  
        transformComponents[cameraID].globalTransform[3][0], 
        transformComponents[cameraID].globalTransform[3][1], 
        transformComponents[cameraID].globalTransform[3][2],
    };
    
    //Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (skyboxTexture != 0) DrawSkyBox();

    //Check if render Component is in shader order array
    for (std::pair<unsigned int, RenderComponent> entity : renderComponents)
    {
        if(entity.second.material != nullptr)
            entityShaderOrder[entity.second.material->shaderProgram].push_back(entity.first);
    }

    //Render all components in Shader Order
    for (int i = 0; i < Shader::count; i++)
    {
        glUseProgram((*shaders)[i]);

        modelLocation = glGetUniformLocation((*shaders)[i], "model");

        //Set material layers
        glUniform1i(glGetUniformLocation((*shaders)[i], "diffuse"), 0);
        glUniform1i(glGetUniformLocation((*shaders)[i], "specular"), 1);
        glUniform1i(glGetUniformLocation((*shaders)[i], "normalMap"), 2);
        glUniform1i(glGetUniformLocation((*shaders)[i], "irradianceMap"), 3);
        glUniform1i(glGetUniformLocation((*shaders)[i], "prefilterMap"), 4);
        glUniform1i(glGetUniformLocation((*shaders)[i], "brdfLUT"), 5);

        BindLightUniform((*shaders)[i], lightComponents, transformComponents);

        glUniformMatrix4fv(glGetUniformLocation((*shaders)[i], "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation((*shaders)[i], "projection"), 1, GL_FALSE, &projection[0][0]);

        glUniform3f(glGetUniformLocation((*shaders)[i], "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        for (std::vector<unsigned int>::iterator iter = entityShaderOrder[i].begin(); iter != entityShaderOrder[i].end(); iter++)
        {
            //If the mesh is bugged, do not render. TO BE REPLACED WITH BROKEN MESH MESH
            if (renderComponents[*iter].mesh == nullptr) continue;

            //Get transform pair's model transform
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &transformComponents[*iter].globalTransform[0][0]);

            glBindTexture(GL_TEXTURE_2D, 0);

            //For each texture with the render components material
            unsigned int materialMask = 1;
            for (int i = 0; i < MATERIAL_MAPCOUNT; i++)
            {
                if (renderComponents[*iter].material == nullptr || renderComponents[*iter].material->materials[0] == 0)
                {
                    //Bind missing texture if the material doesnt exist.
                    glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                    break;
                }

                //if the current material/texture map matches the current binary
                else if ((renderComponents[*iter].material->materialMask & materialMask) == materialMask)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    if (renderComponents[*iter].material->materials[i] == 0 && i == 0)
                    {
                        //Bind missing texture if the diffuse doesnt exist.
                        glBindTexture(GL_TEXTURE_2D, missingTextureTexture);
                    }
                    else
                    {
                        glBindTexture(GL_TEXTURE_2D, renderComponents[*iter].material->materials[i]);
                    }
                }
                materialMask *= 2;
            }

            //IBL
            glActiveTexture(GL_TEXTURE0 + MATERIAL_MAPCOUNT + 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

            glActiveTexture(GL_TEXTURE0 + MATERIAL_MAPCOUNT + 1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

            glActiveTexture(GL_TEXTURE0 + MATERIAL_MAPCOUNT + 2);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

            glUniform3fv(glGetUniformLocation((*shaders)[i], "materialColour"), 1,&renderComponents[*iter].colour[0]);

            //Bind mesh for drawing
            glBindVertexArray(renderComponents[*iter].mesh->VAO);

            if (renderComponents[*iter].mesh->IBO != 0)
                glDrawElements(GL_TRIANGLES, 3 * renderComponents[*iter].mesh->triCount, GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, 3 * renderComponents[*iter].mesh->triCount);
        }
    }



    debug->lines.Draw(projectionMatrix* viewMatrix);

    RenderBloom(bloomBuffer);
	 

    if (renderTexture == nullptr)
    {
       glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
       if (outputFBO == 0) OutputBufferSetUp();

       *renderTexture = outputTexture;
       glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    }

    //FrameBuffer Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //HDR
    glUseProgram((*shaders)[Shader::hdrBloom]);

    glUniform1i(glGetUniformLocation((*shaders)[Shader::hdrBloom], "scene"), 0);
    glUniform1i(glGetUniformLocation((*shaders)[Shader::hdrBloom], "bloomBlur"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomMips[0].texture);

    glUniform1f(glGetUniformLocation((*shaders)[Shader::hdrBloom], "exposure"), exposure);

    RenderQuad();
	
    for (int i = 0; i < Shader::count; i++) entityShaderOrder[i].clear();
}

void RenderSystem::SetSkyboxTexture(unsigned int texture)
{
    skyboxTexture = texture;
    IBLBufferSetup(skyboxTexture);
}

void RenderSystem::BindLightUniform(unsigned int shaderProgram, 
    std::unordered_map<unsigned int, LightComponent>& lightComponents, 
    std::unordered_map<unsigned int, TransformComponent>& transformComponents)
{
    
    int packetCount = 0;
    std::vector<float> lightPackets;
    std::unordered_map<unsigned int, LightComponent>::iterator iter;

    for (iter = lightComponents.begin(); iter != lightComponents.end(); iter++)
    {
        //Colour
        lightPackets.push_back(iter->second.colour.x);
        lightPackets.push_back(iter->second.colour.y);
        lightPackets.push_back(iter->second.colour.z);
        //Light Type
        lightPackets.push_back((float)iter->second.lightType);

        switch(iter->second.lightType)
        {
            case LightType::ambient:
                packetCount += 1;
                break;

            case LightType::directional:
                //Direction (needs to be changed to the transfrom's forward vector)
                lightPackets.push_back(iter->second.direction.x);
                lightPackets.push_back(iter->second.direction.y);
                lightPackets.push_back(iter->second.direction.z);

                lightPackets.push_back(0);//Unused value
                packetCount += 2;
                break;

            case LightType::point:
                //Position
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][0]);
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][1]);
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][2]);
                lightPackets.push_back(0); //Unused value

                //Attenuation Constants
                lightPackets.push_back(1);
                lightPackets.push_back(iter->second.linear);
                lightPackets.push_back(iter->second.quad);
                lightPackets.push_back(iter->second.range); //unused in calculations
                packetCount += 3;
                break;

            case LightType::spot:
                //Position
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][0]);
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][1]);
                lightPackets.push_back(transformComponents[iter->first].globalTransform[3][2]);
                lightPackets.push_back(0); //Unused value


                //Direction (needs to be changed to the transfrom's forward vector)
                lightPackets.push_back(iter->second.direction.x);
                lightPackets.push_back(iter->second.direction.y);
                lightPackets.push_back(iter->second.direction.z);
                lightPackets.push_back(0); 
                
                //Attentuation and Angle calcs
                lightPackets.push_back(iter->second.cutOff);
                lightPackets.push_back(iter->second.linear);
                lightPackets.push_back(iter->second.quad);
                lightPackets.push_back(iter->second.outerCutOff);
                packetCount += 4;
        }      
    }

    unsigned int lights = glGetUniformLocation(shaderProgram, "lightPackets");
    glUniform4fv(lights, packetCount, lightPackets.data());

    glUniform1i(glGetUniformLocation(shaderProgram, "lightPacketCount"), packetCount);
}

void RenderSystem::HDRBufferSetUp()
{
    glGenFramebuffers(1, &hdrFBO);
    glGenTextures(1, &colorBuffer);
    glGenRenderbuffers(1, &rboDepth);
    
    HDRBufferUpdate();
}

void RenderSystem::OutputBufferSetUp()
{
   glGenFramebuffers(1, &outputFBO);
   glGenTextures(1, &outputTexture);

   OutputBufferUpdate();
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

void RenderSystem::IBLBufferSetup(unsigned int skybox)
{
    SetIrradianceMap(skybox);

    SetPrefilteredMap(skybox);
   
    glGenTextures(1, &brdfLUTTexture);
    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);

    unsigned int currShader = (*shaders)[Shader::brdf];
    glUseProgram(currShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glUseProgram((*shaders)[Shader::defaultPBR]);


    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
}

void RenderSystem::BloomSetup()
{
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glm::vec2 mipSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    glm::ivec2 mipIntSize((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

    for (GLuint i = 0; i < bloomMipMapCount; i++)
    {
        bloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
            (int)mipSize.x, (int)mipSize.y,
            0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        bloomMips.push_back(mip);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, bloomMips[0].texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    glUseProgram((*shaders)[Shader::downSample]);
    glUniform1i(glGetUniformLocation((*shaders)[Shader::downSample], "srcTexture"), 0);
    glUseProgram((*shaders)[Shader::upSample]);
    glUniform1i(glGetUniformLocation((*shaders)[Shader::upSample], "srcTexture"), 0);
    glUseProgram(0);
}

void RenderSystem::SetIrradianceMap(unsigned int skybox)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //Convert skybox into an irradance version!
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    unsigned int currShader = (*shaders)[Shader::irradiance];
    glUseProgram(currShader);
    glUniform1i(glGetUniformLocation(currShader, "environmentMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(currShader, "projection"), 1, GL_FALSE, &captureProjection[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);


    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(currShader, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
}

void RenderSystem::SetPrefilteredMap(unsigned int skybox)
{
    glViewport(0, 0, 32, 32);
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    unsigned int currShader = (*shaders)[Shader::prefilter];
    glUseProgram(currShader);

    glUniform1i(glGetUniformLocation(currShader, "environmentMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(currShader, "projection"), 1, GL_FALSE, &captureProjection[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < 5; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(currShader, "roughness"), roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            //ColouredOutput("view in prefilter is: ", Colour::green);
            //ColouredOutput(glGetUniformLocation(currShader, "view") == -1, Colour::red, false);
            glUniformMatrix4fv(glGetUniformLocation(currShader, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

}

void RenderSystem::HDRBufferUpdate()
{
    // create floating point color buffer
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create bloom buffer
    glGenTextures(1, &bloomBuffer);
    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create depth buffer (renderbuffer)
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OutputBufferUpdate();
}

void RenderSystem::OutputBufferUpdate()
{
   if (outputFBO == 0) return;

   // create unsigned int color buffer
   glBindTexture(GL_TEXTURE_2D, outputTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // attach buffer
   glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::RenderBloom(unsigned int srcTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    
    RenderDownSamples(srcTexture);
    
    RenderUpSamples((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void RenderSystem::RenderDownSamples(unsigned int srcTexture)
{

    glUseProgram((*shaders)[Shader::downSample]);

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glm::vec2 inverseRes = { 1.0f / (float)SCREEN_WIDTH, 1.0f / (float)SCREEN_HEIGHT };

    glUniform1i(glGetUniformLocation((*shaders)[Shader::downSample], "mipLevel"), 0);

    glUniform2fv(glGetUniformLocation((*shaders)[Shader::downSample], "srcResolution"), 1, &inverseRes[0]);
    
    glDisable(GL_BLEND);

    // Progressively downsample through the mip chain
    for (int i = 0; i < (int)bloomMips.size(); i++)
    {
        const bloomMip& mip = bloomMips[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        inverseRes = 1.0f / mip.size;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        RenderQuad();

        // Set current mip resolution as srcResolution for next iteration
        glUniform2fv(glGetUniformLocation((*shaders)[Shader::downSample], "srcResolution"), 1,  &inverseRes[0]);

        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Disable Karis average for consequent downsamples
        if (i == 0) glUniform1i(glGetUniformLocation((*shaders)[Shader::downSample], "mipLevel"), 1); 
    }

    glUseProgram(0);
}

void RenderSystem::RenderUpSamples(float aspectRatio)
{
    glUseProgram((*shaders)[Shader::upSample]);
    glUniform1f(glGetUniformLocation((*shaders)[Shader::upSample],"aspectRatio"), aspectRatio);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = (int)bloomMips.size() - 1; i > 0; i--)
    {
        const bloomMip& mip = bloomMips[i];
        const bloomMip& nextMip = bloomMips[i - 1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        RenderQuad();
    }
    glDisable(GL_BLEND); 
    glUseProgram(0);
}

void RenderSystem::RenderQuad()
{
    glDisable(GL_CULL_FACE);
    
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

}

void RenderSystem::DrawSkyBox()
{
    glDisable(GL_DEPTH_TEST);

    unsigned int emptyVAO;
    glUseProgram((*shaders)[Shader::skybox]);

    glm::mat4 PV = projectionMatrix * viewMatrix;
    glm::mat4 iPV = glm::inverse(PV);

    glUniformMatrix4fv(glGetUniformLocation((*shaders)[Shader::skybox], "PV"), 1, GL_FALSE, &iPV[0][0]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    glGenVertexArrays(1, &emptyVAO);
    glBindVertexArray(emptyVAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
 