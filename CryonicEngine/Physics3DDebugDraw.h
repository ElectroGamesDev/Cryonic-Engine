#pragma once

#define JPH_DEBUG_RENDERER
#include "Jolt/Jolt.h"
#include "Jolt/Renderer/DebugRenderer.h"

class Physics3DDebugDraw : public JPH::DebugRenderer
{
public:
    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
    virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const uint32_t* inIndices, int inIndexCount) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
    virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;
};