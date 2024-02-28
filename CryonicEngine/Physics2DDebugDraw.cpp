#include "Physics2DDebugDraw.h"
#include "raylib.h"
#include "ConsoleLogger.h"

void Physics2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    Color raylibColor = { static_cast<unsigned char>(color.r * 255),
                          static_cast<unsigned char>(color.g * 255),
                          static_cast<unsigned char>(color.b * 255),
                          255 };

    for (int i = 0; i < vertexCount - 1; ++i) {
        Vector2 point1 = { vertices[i].x, vertices[i].y };
        Vector2 point2 = { vertices[i + 1].x, vertices[i + 1].y };
        DrawLineV(point1, point2, raylibColor);
    }
    Vector2 lastPoint = { vertices[vertexCount - 1].x, vertices[vertexCount - 1].y };
    Vector2 firstPoint = { vertices[0].x, vertices[0].y };
    DrawLineV(lastPoint, firstPoint, raylibColor);
}

void Physics2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    Color raylibColor = { static_cast<unsigned char>(color.r * 255),
                      static_cast<unsigned char>(color.g * 255),
                      static_cast<unsigned char>(color.b * 255),
                      255 };

    Vector2* points = new Vector2[vertexCount];

    for (int i = 0; i < vertexCount; ++i) {
        points[i] = { vertices[i].x, vertices[i].y };
    }

    DrawTriangleFan(points, vertexCount, raylibColor);

    delete[] points;
}

void Physics2DDebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
}

void Physics2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
}

void Physics2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
}

void Physics2DDebugDraw::DrawTransform(const b2Transform& xf) {
}

void Physics2DDebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
}

