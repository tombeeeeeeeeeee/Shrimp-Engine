#include "CameraSystem.h"

CameraSystem::CameraSystem(GLFWwindow* window)
{
    this->window = window;
    cameraTransform = 0;

    xInput.BindPair(A, D);
    yInput.BindPair(LControl, Space);
    zInput.BindPair(S, W);
}

void CameraSystem::Initialise(TransformComponent& _cameraTransform, CameraComponent& _cameraComponent)
{
    cameraTransform = &_cameraTransform;
    cameraComponent = &_cameraComponent;
}

bool CameraSystem::Update(std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    unsigned int cameraID, CameraComponent& _cameraComponent, SceneManager* scene, glm:: mat4& view, float dt)
{
    //if the camera transform hasn't been set yet, set it. (might just reset each tick)
    if (cameraTransform == nullptr) cameraTransform = &transformComponents[cameraID];
    if (cameraComponent == nullptr) cameraComponent = &_cameraComponent;

    if (inputSystem->GetKeyDown(MouseRight)) RotateCamera();

    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glm::vec3 pos = { 
        transformComponents[cameraID].globalTransform[3][0],
        transformComponents[cameraID].globalTransform[3][1],
        transformComponents[cameraID].globalTransform[3][2],
    };
    glm::quat rot = transformComponents[cameraID].rotation;

    glm::vec3& forwards = _cameraComponent.forward;
    glm::vec3& up = _cameraComponent.up;
    glm::vec3& right = _cameraComponent.right;
    
    forwards = rot * glm::vec3(1,0,0);
     
    right = glm::normalize(glm::cross(forwards, globalUp));
    up = glm::normalize(cross(right, forwards));

    view = glm::lookAt(pos, pos + forwards, up);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //Keys
    glm::vec3 dPos = {(float)xInput, 0 , (float)zInput};

    if (glm::length(dPos) > 0.1f) 
    {
        dPos = glm::normalize(dPos);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dPos *= 2;
        glm::vec3 dForward = 0.1f * dPos.z * glm::vec3(forwards.x, 0, forwards.z);
        glm::vec3 dRight = 0.1f * dPos.x * glm::vec3(right.x, 0, right.z);
        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + dForward + dRight);

    }
    if (yInput)
    {

        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + 0.1f * globalUp * (float)yInput);    
    }

    UpdateFrustum(transformComponents[cameraID], _cameraComponent);

    return false;
}

std::unordered_map<unsigned int, RenderComponent> CameraSystem::CheckOnFrustum(Frustum frustum, std::unordered_map<unsigned int, RenderComponent>& renderComponents, std::unordered_map<unsigned int, TransformComponent>& transformComponents)
{
    std::unordered_map<unsigned int, RenderComponent> culledComponents = std::unordered_map<unsigned int, RenderComponent>();

    for (std::pair<unsigned int, RenderComponent> entity : renderComponents)
    {
        glm::mat4 model = transformComponents[entity.first].globalTransform;
        glm::vec3 min = entity.second.mesh->bottomCorner;
        glm::vec3 max = entity.second.mesh->topCorner;

        glm::vec3 OOBB[8] =
        {
            model * glm::vec4(min, 1),
            model * glm::vec4(min.x, min.y, max.z, 1),
            model * glm::vec4(min.x, max.y, min.z, 1),
            model * glm::vec4(min.x, max.y, max.z, 1),
            model * glm::vec4(max.x, min.y, min.z, 1),
            model * glm::vec4(max.x, min.y, max.z, 1),
            model * glm::vec4(max.x, max.y, min.z, 1),
            model * glm::vec4(max, 1),
        };

        if (IsOnFrustum(frustum, OOBB)) culledComponents[entity.first] = entity.second;
    }

    return culledComponents;
}

 void CameraSystem::UpdateFrustum(TransformComponent& cameraTransform, CameraComponent& cameraComponent)
 {
     Frustum frustum;

     glm::vec3 pos = {
        cameraTransform.globalTransform[3][0],
        cameraTransform.globalTransform[3][1],
        cameraTransform.globalTransform[3][2],
     };

     float fov = cameraComponent.fov;
     float aspect = cameraComponent.aspectRatio;
     float nearClip = cameraComponent.nearClip;
     float farClip = cameraComponent.farClip;

     frustum = CreateFrustum(pos, fov, aspect, nearClip, farClip,
         cameraComponent.up, cameraComponent.forward, cameraComponent.right);

     cameraComponent.frustum = frustum;
}

 Frustum CameraSystem::CreateFrustum(glm::vec3 pos, float fov, float aspect, float nearClip, float farClip,
     glm::vec3 up, glm::vec3 forward, glm::vec3 right)
 {
     Frustum frustum;

     const float halfVSide = farClip * tanf(fov / 2.0f);
     const float halfHSide = halfVSide * aspect;
     const glm::vec3 frontMultFar = farClip * forward;

     glm::vec3 rightNormal = glm::normalize(glm::cross(frontMultFar - right * halfHSide, up));
     glm::vec3 leftNormal = glm::normalize(glm::cross(up, frontMultFar + right * halfHSide));
     glm::vec3 topNormal = glm::normalize(glm::cross(right, frontMultFar - up * halfVSide));
     glm::vec3 bottomNormal = glm::normalize(glm::cross(frontMultFar + up * halfVSide, right));

     frustum.nearFace = { forward, glm::dot(pos + nearClip * forward ,forward) };
     frustum.farFace = { -forward, glm::dot(pos + frontMultFar, -forward) };
     frustum.rightFace = { rightNormal, glm::dot(pos, rightNormal) };
     frustum.leftFace = { leftNormal, glm::dot(pos, leftNormal) };
     frustum.topFace = { topNormal, glm::dot(pos, topNormal) };
     frustum.bottomFace = { bottomNormal, glm::dot(pos, bottomNormal) };


     return frustum;
 }

 bool CameraSystem::IsOnFrustum(Frustum frustum, glm::vec3 OOBB[8])
 {
     if (!IsInfrontOfPlane(frustum.leftFace, OOBB)) return false;
     if (!IsInfrontOfPlane(frustum.rightFace, OOBB)) return false;
     if (!IsInfrontOfPlane(frustum.topFace, OOBB)) return false;
     if (!IsInfrontOfPlane(frustum.bottomFace, OOBB)) return false;
     if (!IsInfrontOfPlane(frustum.farFace, OOBB)) return false;
     if (!IsInfrontOfPlane(frustum.nearFace, OOBB)) return false;
     return true;
 }

 bool CameraSystem::IsInfrontOfPlane(Plane plane, glm::vec3 OOBB[8])
 {
    /*
    *  for all verts
    *    compare with  face
    *        if closer than face return true;
    *  return false
    */

     for (int i = 0; i < 8; i++)
     {
         float distance = glm::dot(plane.normal, OOBB[i]);
         if (distance - plane.distance >= 0) return true;
     }
     return false;
 }

void CameraSystem::RotateCamera()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    
    glm::vec3 dEulers = { 0.0f, 0.0f, 0.0f };
    double mouse_x, mouse_y; 
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, w / 2, h / 2); 
    
    dEulers.y = -0.0005f * static_cast<float>(mouse_x - w / 2); 
    dEulers.z = -0.0005f * static_cast<float>(mouse_y - h / 2);

    glm::quat zRot = glm::quat(glm::vec3(0, 0, dEulers.z));
    if(abs(glm::dot(zRot * cameraComponent->forward, globalUp)) < 0.95f)
        cameraTransform->rotation *= zRot;

    glm::quat yRot = glm::rotate(glm::identity<glm::quat>(), dEulers.y, globalUp);
    cameraTransform->rotation = yRot * cameraTransform->rotation;
}

