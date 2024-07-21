#pragma once
namespace LGP
{
    /// <summary>
    /// The threshold to which a vert will be registered as a colliding face (eg. vert * normal > threshold)
    /// </summary>
    const float COLLISION_FACE_THRESHOLD = -0.005f;

    /// <summary>
    /// An enum to help track the progress of GJK
    /// </summary>
    enum GJKEvolution
    {
        evolving = 0,
        intersecting = 1,
        notIntersecting = 2
    };

}