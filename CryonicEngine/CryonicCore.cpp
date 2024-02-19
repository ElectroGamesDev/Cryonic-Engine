#include "CryonicCore.h"
#include <cmath>

Vector3 RotateVector3ByQuaternion(Vector3 vector, Quaternion quaternion)
{
    Vector3 result = { 0 };

    result.x = vector.x * (quaternion.x * quaternion.x + quaternion.w * quaternion.w - quaternion.y * quaternion.y - quaternion.z * quaternion.z) + vector.y * (2 * quaternion.x * quaternion.y - 2 * quaternion.w * quaternion.z) + vector.z * (2 * quaternion.x * quaternion.z + 2 * quaternion.w * quaternion.y);
    result.y = vector.x * (2 * quaternion.w * quaternion.z + 2 * quaternion.x * quaternion.y) + vector.y * (quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z) + vector.z * (-2 * quaternion.w * quaternion.x + 2 * quaternion.y * quaternion.z);
    result.z = vector.x * (-2 * quaternion.w * quaternion.y + 2 * quaternion.x * quaternion.z) + vector.y * (2 * quaternion.w * quaternion.x + 2 * quaternion.y * quaternion.z) + vector.z * (quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);

    return result;
}

Quaternion EulerToQuaternion(float pitch, float yaw, float roll)
{
    Quaternion result = { 0 };

    float x0 = cosf(pitch * 0.5f);
    float x1 = sinf(pitch * 0.5f);
    float y0 = cosf(yaw * 0.5f);
    float y1 = sinf(yaw * 0.5f);
    float z0 = cosf(roll * 0.5f);
    float z1 = sinf(roll * 0.5f);

    result.x = x1 * y0 * z0 - x0 * y1 * z1;
    result.y = x0 * y1 * z0 + x1 * y0 * z1;
    result.z = x0 * y0 * z1 - x1 * y1 * z0;
    result.w = x0 * y0 * z0 + x1 * y1 * z1;

    return result;
}


Vector3 QuaternionToEuler(Quaternion quaternion)
{
    Vector3 result = { 0 };

    // Roll (x-axis rotation)
    float x0 = 2.0f * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
    float x1 = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
    result.x = atan2f(x0, x1);

    // Pitch (y-axis rotation)
    float y0 = 2.0f * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
    y0 = y0 > 1.0f ? 1.0f : y0;
    y0 = y0 < -1.0f ? -1.0f : y0;
    result.y = asinf(y0);

    // Yaw (z-axis rotation)
    float z0 = 2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
    float z1 = 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
    result.z = atan2f(z0, z1);

    return result;
}


//void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint)
//{
//    rlPushMatrix();
//
//    // get the camera view matrix
//    Matrix mat = MatrixInvert(MatrixLookAt(camera.position, camera.target, camera.up));
//    // peel off just the rotation
//    Quaternion quat = QuaternionFromMatrix(mat);
//    mat = QuaternionToMatrix(quat);
//
//    // apply just the rotation
//    rlMultMatrixf(MatrixToFloat(mat));
//
//    // translate backwards in the inverse rotated matrix to put the item where it goes in world space
//    position = Vector3Transform(position, MatrixInvert(mat));
//    rlTranslatef(position.x, position.y, position.z);
//
//    // draw the billboard
//    float width = size.x / 2;
//    float height = size.y / 2;
//
//    Color color = WHITE;
//
//    rlCheckRenderBatchLimit(6);
//
//    rlSetTexture(texture.id);
//
//    // draw quad
//    rlBegin(RL_QUADS);
//    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
//    // Front Face
//    rlNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing Towards Viewer
//
//    rlTexCoord2f((float)source.x / texture.width, (float)(source.y + source.height) / texture.height);
//    rlVertex3f(-width, -height, 0);  // Bottom Left Of The Texture and Quad
//
//    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)(source.y + source.height) / texture.height);
//    rlVertex3f(+width, -height, 0);  // Bottom Right Of The Texture and Quad
//
//    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)source.y / texture.height);
//    rlVertex3f(+width, +height, 0);  // Top Right Of The Texture and Quad
//
//    rlTexCoord2f((float)source.x / texture.width, (float)source.y / texture.height);
//    rlVertex3f(-width, +height, 0);  // Top Left Of The Texture and Quad
//
//    rlEnd();
//    rlSetTexture(0);
//    rlPopMatrix();
//}
//
//void Draw3DBillboard(Camera camera, Texture2D texture, Vector3 position, float size, Color tint)
//{
//    Draw3DBillboardRec(camera, texture, { 0, 0, (float)texture.width, (float)texture.height }, position, { size, size }, tint);
//}