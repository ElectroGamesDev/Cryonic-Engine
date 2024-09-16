#include "RaylibWrapper.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <cstring>
#include "imgui_impl_raylib.h"
#include "rlImGui.h"

#ifndef SMOOTH_CIRCLE_ERROR_RATE
#define SMOOTH_CIRCLE_ERROR_RATE    0.5f      // Circle error rate
#endif

namespace RaylibWrapper {
    void InitWindow(int width, int height, const char* title) {
        ::InitWindow(width, height, title);
    }

    bool WindowShouldClose() {
        return ::WindowShouldClose();
    }

    void CloseWindow() {
        ::CloseWindow();
    }

    bool IsWindowReady() {
        return ::IsWindowReady();
    }

    bool IsWindowFullscreen() {
        return ::IsWindowFullscreen();
    }

    bool IsWindowHidden() {
        return ::IsWindowHidden();
    }

    bool IsWindowMinimized() {
        return ::IsWindowMinimized();
    }

    bool IsWindowMaximized() {
        return ::IsWindowMaximized();
    }

    bool IsWindowFocused() {
        return ::IsWindowFocused();
    }

    bool IsWindowResized() {
        return ::IsWindowResized();
    }

    bool IsWindowState(unsigned int flag) {
        return ::IsWindowState(flag);
    }

    void SetWindowState(unsigned int flags) {
        ::SetWindowState(flags);
    }

    void ClearWindowState(unsigned int flags) {
        ::ClearWindowState(flags);
    }

    void ToggleFullscreen() {
        ::ToggleFullscreen();
    }

    void ToggleBorderlessWindowed() {
        ::ToggleBorderlessWindowed();
    }

    void MaximizeWindow() {
        ::MaximizeWindow();
    }

    void MinimizeWindow() {
        ::MinimizeWindow();
    }

    void RestoreWindow() {
        ::RestoreWindow();
    }

    void SetWindowIcon(Image image) {
        ::SetWindowIcon({image.data, image.width, image.height, image.mipmaps, image.format});
    }

    //void SetWindowIcons(Image* images, int count) {
    //    ::SetWindowIcons(images, count);
    //}

    void SetWindowTitle(const char* title) {
        ::SetWindowTitle(title);
    }

    void SetWindowPosition(int x, int y) {
        ::SetWindowPosition(x, y);
    }

    void SetWindowMonitor(int monitor) {
        ::SetWindowMonitor(monitor);
    }

    void SetWindowMinSize(int width, int height) {
        ::SetWindowMinSize(width, height);
    }

    void SetWindowSize(int width, int height) {
        ::SetWindowSize(width, height);
    }

    void SetWindowOpacity(float opacity) {
        ::SetWindowOpacity(opacity);
    }

    void* GetWindowHandle() {
        return ::GetWindowHandle();
    }

    int GetScreenWidth() {
        return ::GetScreenWidth();
    }

    int GetScreenHeight() {
        return ::GetScreenHeight();
    }

    int GetRenderWidth() {
        return ::GetRenderWidth();
    }

    int GetRenderHeight() {
        return ::GetRenderHeight();
    }

    int GetMonitorCount() {
        return ::GetMonitorCount();
    }

    int GetCurrentMonitor() {
        return ::GetCurrentMonitor();
    }

    Vector2 GetMonitorPosition(int monitor) {
        ::Vector2 pos = ::GetMonitorPosition(monitor);
        return {pos.x, pos.y};
    }

    int GetMonitorWidth(int monitor) {
        return ::GetMonitorWidth(monitor);
    }

    int GetMonitorHeight(int monitor) {
        return ::GetMonitorHeight(monitor);
    }

    int GetMonitorPhysicalWidth(int monitor) {
        return ::GetMonitorPhysicalWidth(monitor);
    }

    int GetMonitorPhysicalHeight(int monitor) {
        return ::GetMonitorPhysicalHeight(monitor);
    }

    int GetMonitorRefreshRate(int monitor) {
        return ::GetMonitorRefreshRate(monitor);
    }

    Vector2 GetWindowPosition() {
        ::Vector2 pos = ::GetWindowPosition();
        return { pos.x, pos.y };
    }

    Vector2 GetWindowScaleDPI() {
        ::Vector2 pos = ::GetWindowScaleDPI();
        return {pos.x, pos.y};
    }

    const char* GetMonitorName(int monitor) {
        return ::GetMonitorName(monitor);
    }

    void SetClipboardText(const char* text) {
        ::SetClipboardText(text);
    }

    const char* GetClipboardText() {
        return ::GetClipboardText();
    }

    void EnableEventWaiting() {
        ::EnableEventWaiting();
    }

    void DisableEventWaiting() {
        ::DisableEventWaiting();
    }


    void SetConfigFlags(unsigned int flags) {
        ::SetConfigFlags(flags);
    }

    // Screen-space-related functions
    Ray GetMouseRay(Vector2 mousePosition, Camera camera) {
        ::Ray ray = ::GetMouseRay({ mousePosition.x, mousePosition.y }, { {camera.position.x, camera.position.y, camera.position.z}, {camera.target.x, camera.target.y, camera.target.z}, {camera.up.x, camera.up.y, camera.up.z}, camera.fovy, camera.projection });
        return { {ray.position.x, ray.position.y, ray.position.z}, {ray.direction.x, ray.direction.y, ray.direction.z} };
    }

    Matrix GetCameraMatrix(Camera camera) {
        ::Matrix matrix = ::GetCameraMatrix({ {camera.position.x, camera.position.y, camera.position.z}, {camera.target.x, camera.target.y, camera.target.z}, {camera.up.x, camera.up.y, camera.up.z}, camera.fovy, camera.projection });
        return { matrix.m0, matrix.m1, matrix.m2, matrix.m3, matrix.m4, matrix.m5, matrix.m6, matrix.m7, matrix.m8, matrix.m9, matrix.m10, matrix.m11, matrix.m12, matrix.m13, matrix.m14, matrix.m15 };
    }

    Matrix GetCameraMatrix2D(Camera2D camera) {
        ::Matrix matrix = ::GetCameraMatrix2D({ {camera.offset.x, camera.offset.y}, {camera.target.x, camera.target.y}, camera.rotation, camera.zoom });
        return { matrix.m0, matrix.m1, matrix.m2, matrix.m3, matrix.m4, matrix.m5, matrix.m6, matrix.m7, matrix.m8, matrix.m9, matrix.m10, matrix.m11, matrix.m12, matrix.m13, matrix.m14, matrix.m15 };
    }

    Vector2 GetWorldToScreen(Vector3 position, Camera camera) {
        ::Vector2 vector = ::GetWorldToScreen({position.x, position.y, position.z}, { {camera.position.x, camera.position.y, camera.position.z}, {camera.target.x, camera.target.y, camera.target.z}, {camera.up.x, camera.up.y, camera.up.z}, camera.fovy, camera.projection });
        return {vector.x, vector.y};
    }

    Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera) {
        ::Vector2 vector = ::GetScreenToWorld2D({ position.x, position.y }, { {camera.offset.x, camera.offset.y}, {camera.target.x, camera.target.y}, camera.rotation, camera.zoom });
        return { vector.x, vector.y };
    }

    Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height) {
        ::Vector2 vector = ::GetWorldToScreenEx({ position.x, position.y, position.z }, { {camera.position.x, camera.position.y, camera.position.z}, {camera.target.x, camera.target.y, camera.target.z}, {camera.up.x, camera.up.y, camera.up.z}, camera.fovy, camera.projection }, width, height);
        return { vector.x, vector.y };
    }

    Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera) {
        ::Vector2 vector = ::GetWorldToScreen2D({ position.x, position.y }, { {camera.offset.x, camera.offset.y}, {camera.target.x, camera.target.y}, camera.rotation, camera.zoom });
        return { vector.x, vector.y };
    }

    // Timing-related functions
    void SetTargetFPS(int fps) {
        ::SetTargetFPS(fps);
    }

    int GetFPS() {
        return ::GetFPS();
    }

    float GetFrameTime() {
        return ::GetFrameTime();
    }

    double GetTime() {
        return ::GetTime();
    }

    // Cursor-related functions
    void ShowCursor(void) {
        ::ShowCursor();
    }

    void HideCursor(void) {
        ::HideCursor();
    }

    bool IsCursorHidden(void) {
        return ::IsCursorHidden();
    }

    void EnableCursor(void) {
        ::EnableCursor();
    }

    void DisableCursor(void) {
        ::DisableCursor();
    }

    bool IsCursorOnScreen(void) {
        return ::IsCursorOnScreen();
    }

    // Drawing-related functions
    void ClearBackground(Color color) {
        ::ClearBackground({color.r, color.g, color.b, color.a});
    }

    void BeginDrawing(void) {
        ::BeginDrawing();
    }

    void EndDrawing(void) {
        ::EndDrawing();
    }

    void BeginMode2D(Camera2D camera) {
        ::BeginMode2D({ {camera.offset.x, camera.offset.y}, {camera.target.x, camera.target.y}, camera.rotation, camera.zoom });
    }

    void EndMode2D(void) {
        ::EndMode2D();
    }

    void BeginMode3D(Camera3D camera) {
        ::BeginMode3D({ {camera.position.x, camera.position.y, camera.position.z}, {camera.target.x, camera.target.y, camera.target.z}, {camera.up.x, camera.up.y, camera.up.z}, camera.fovy, camera.projection });
    }

    void EndMode3D(void) {
        ::EndMode3D();
    }

    void BeginTextureMode(RenderTexture2D target) {
        ::BeginTextureMode({ target.id, { target.texture.id, target.texture.width, target.texture.height, target.texture.mipmaps, target.texture.format }, { target.depth.id, target.depth.width, target.depth.height, target.depth.mipmaps, target.depth.format } });
    }

    void EndTextureMode(void) {
        ::EndTextureMode();
    }

    void BeginShaderMode(Shader shader) {
        ::BeginShaderMode({shader.id, shader.locs});
    }

    void EndShaderMode(void) {
        ::EndShaderMode();
    }

    void BeginBlendMode(int mode) {
        ::BeginBlendMode(mode);
    }

    void EndBlendMode(void) {
        ::EndBlendMode();
    }

    void BeginScissorMode(int x, int y, int width, int height) {
        ::BeginScissorMode(x, y, width, height);
    }

    void EndScissorMode(void) {
        ::EndScissorMode();
    }

    //void BeginVrStereoMode(VrStereoConfig config) {
    //    ::BeginVrStereoMode(config);
    //}

    void EndVrStereoMode(void) {
        ::EndVrStereoMode();
    }

    // Texture loading functions
    Texture2D LoadTexture(const char* fileName) {
        ::Texture2D texture2D = ::LoadTexture(fileName);
        return { texture2D .id, texture2D.width, texture2D.height, texture2D.mipmaps, texture2D.format};
    }

    Texture2D LoadTextureFromImage(Image image) {
        ::Texture2D texture2D = ::LoadTextureFromImage({image.data, image.width, image.height, image.mipmaps, image.format});
        return { texture2D.id, texture2D.width, texture2D.height, texture2D.mipmaps, texture2D.format };
    }

    TextureCubemap LoadTextureCubemap(Image image, int layout) {
        ::TextureCubemap textureCubemap = ::LoadTextureCubemap({ image.data, image.width, image.height, image.mipmaps, image.format }, layout);
        return { textureCubemap.id, textureCubemap.width, textureCubemap.height, textureCubemap.mipmaps, textureCubemap.format };
    }

    RenderTexture2D LoadRenderTexture(int width, int height) {
        ::RenderTexture2D renderTexture2D = ::LoadRenderTexture(width, height);
        return { renderTexture2D.id, { renderTexture2D.texture.id, renderTexture2D.texture.width, renderTexture2D.texture.height, renderTexture2D.texture.mipmaps, renderTexture2D.texture.format }, { renderTexture2D.depth.id, renderTexture2D.depth.width, renderTexture2D.depth.height, renderTexture2D.depth.mipmaps, renderTexture2D.depth.format } };
    }

    bool IsTextureReady(Texture2D texture) {
        return ::IsTextureReady({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format });
    }

    void UnloadTexture(Texture2D texture) {
        ::UnloadTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format });
    }

    bool IsRenderTextureReady(RenderTexture2D target) {
        return ::IsRenderTextureReady({ target.id, { target.texture.id, target.texture.width, target.texture.height, target.texture.mipmaps, target.texture.format }, { target.depth.id, target.depth.width, target.depth.height, target.depth.mipmaps, target.depth.format } });
    }

    void UnloadRenderTexture(RenderTexture2D target) {
        ::UnloadRenderTexture({ target.id, { target.texture.id, target.texture.width, target.texture.height, target.texture.mipmaps, target.texture.format }, { target.depth.id, target.depth.width, target.depth.height, target.depth.mipmaps, target.depth.format } });
    }

    void UpdateTexture(Texture2D texture, const void* pixels) {
        ::UpdateTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format }, pixels);
    }

    void UpdateTextureRec(Texture2D texture, Rectangle rec, const void* pixels) {
        ::UpdateTextureRec({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format }, { rec.x, rec.y, rec.width, rec.height}, pixels);
    }

    // Texture Drawing
    void DrawTexture(Texture2D texture, int posX, int posY, Color tint) {
        return ::DrawTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format }, posX, posY, {tint.r, tint.g, tint.b, tint.a});
    }

    void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
        return ::DrawTexturePro({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format }, { source.x, source.y, source.width, source.height }, { dest.x, dest.y, dest.width, dest.height }, {origin.x, origin.y}, rotation, { tint.r, tint.g, tint.b, tint.a });
    }

    // A modified version of DrawTexturePro to flip the quad
    void DrawTextureProFlipped(Texture2D _texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
        ::Texture2D texture = { _texture.id, _texture.width, _texture.height, _texture.mipmaps, _texture.format };
        //::Rectangle source = { _source.x, _source.y, _source.width, _source.height };
        //::Rectangle dest = { dest.x, dest.y, dest.width, dest.height };
        //::Vector2 origin = { origin.x, origin.y };
        //::Color tint = { tint.r, tint.g, tint.b, tint.a };
        // Check if texture is valid
        if (texture.id > 0)
        {
            float width = (float)texture.width;
            float height = (float)texture.height;

            bool flipX = false;

            if (source.width < 0) { flipX = true; source.width *= -1; }
            if (source.height < 0) source.y -= source.height;

            ::Vector2 topLeft = { 0 };
            ::Vector2 topRight = { 0 };
            ::Vector2 bottomLeft = { 0 };
            ::Vector2 bottomRight = { 0 };

            // Only calculate rotation if needed
            if (rotation == 0.0f)
            {
                float x = dest.x - origin.x;
                float y = dest.y - origin.y;
                topLeft = { x, y };
                topRight = { x + dest.width, y };
                bottomLeft = { x, y + dest.height };
                bottomRight = { x + dest.width, y + dest.height };
            }
            else
            {
                float sinRotation = sinf(rotation * DEG2RAD);
                float cosRotation = cosf(rotation * DEG2RAD);
                float x = dest.x;
                float y = dest.y;
                float dx = -origin.x;
                float dy = -origin.y;

                topLeft.x = x + dx * cosRotation - dy * sinRotation;
                topLeft.y = y + dx * sinRotation + dy * cosRotation;

                topRight.x = x + (dx + dest.width) * cosRotation - dy * sinRotation;
                topRight.y = y + (dx + dest.width) * sinRotation + dy * cosRotation;

                bottomLeft.x = x + dx * cosRotation - (dy + dest.height) * sinRotation;
                bottomLeft.y = y + dx * sinRotation + (dy + dest.height) * cosRotation;

                bottomRight.x = x + (dx + dest.width) * cosRotation - (dy + dest.height) * sinRotation;
                bottomRight.y = y + (dx + dest.width) * sinRotation + (dy + dest.height) * cosRotation;
            }

            ::rlSetTexture(texture.id);
            ::rlBegin(RL_QUADS);

            ::rlColor4ub(tint.r, tint.g, tint.b, tint.a);
            ::rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

            // Top-left corner for texture and quad
            if (flipX) ::rlTexCoord2f((source.x + source.width) / width, source.y / height);
            else ::rlTexCoord2f(source.x / width, source.y / height);
            ::rlVertex2f(topLeft.x, topLeft.y);

            // Top-right corner for texture and quad
            if (flipX) ::rlTexCoord2f(source.x / width, source.y / height);
            else ::rlTexCoord2f((source.x + source.width) / width, source.y / height);
            ::rlVertex2f(topRight.x, topRight.y);

            // Bottom-right corner for texture and quad
            if (flipX) ::rlTexCoord2f(source.x / width, (source.y + source.height) / height);
            else ::rlTexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
            ::rlVertex2f(bottomRight.x, bottomRight.y);

            // Bottom-left corner for texture and quad
            if (flipX) ::rlTexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
            else ::rlTexCoord2f(source.x / width, (source.y + source.height) / height);
            ::rlVertex2f(bottomLeft.x, bottomLeft.y);

            ::rlEnd();
            ::rlSetTexture(0);
        }
    }

    // A modified version of DrawRectanglePro to flip the quad
    void DrawRectangleProFlipped(Rectangle rec, Vector2 origin, float rotation, Color color)
    {
        ::Vector2 topLeft = { 0 };
        ::Vector2 topRight = { 0 };
        ::Vector2 bottomLeft = { 0 };
        ::Vector2 bottomRight = { 0 };

        // Only calculate rotation if needed
        if (rotation == 0.0f)
        {
            float x = rec.x - origin.x;
            float y = rec.y - origin.y;
            topLeft = { x, y };
            topRight = { x + rec.width, y };
            bottomLeft = { x, y + rec.height };
            bottomRight = { x + rec.width, y + rec.height };
        }
        else
        {
            float sinRotation = sinf(rotation * DEG2RAD);
            float cosRotation = cosf(rotation * DEG2RAD);
            float x = rec.x;
            float y = rec.y;
            float dx = -origin.x;
            float dy = -origin.y;

            topLeft.x = x + dx * cosRotation - dy * sinRotation;
            topLeft.y = y + dx * sinRotation + dy * cosRotation;

            topRight.x = x + (dx + rec.width) * cosRotation - dy * sinRotation;
            topRight.y = y + (dx + rec.width) * sinRotation + dy * cosRotation;

            bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
            bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

            bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
            bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;
        }

#if defined(SUPPORT_QUADS_DRAW_MODE)
        ::rlSetTexture(::GetShapesTexture().id);
        ::Rectangle shapeRect = ::GetShapesTextureRectangle();

        ::rlBegin(RL_QUADS);

        ::rlNormal3f(0.0f, 0.0f, 1.0f);
        ::rlColor4ub(color.r, color.g, color.b, color.a);

        ::rlTexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
        ::rlVertex2f(topLeft.x, topLeft.y);

        ::rlTexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        ::rlVertex2f(bottomLeft.x, bottomLeft.y);

        ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        ::rlVertex2f(bottomRight.x, bottomRight.y);

        ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
        ::rlVertex2f(topRight.x, topRight.y);

        ::rlEnd();

        ::rlSetTexture(0);
#else
        ::rlBegin(RL_TRIANGLES);

        ::rlColor4ub(color.r, color.g, color.b, color.a);

        ::rlVertex2f(topLeft.x, topLeft.y);
        ::rlVertex2f(topRight.x, topRight.y);
        ::rlVertex2f(bottomLeft.x, bottomLeft.y);

        ::rlVertex2f(bottomLeft.x, bottomLeft.y);
        ::rlVertex2f(topRight.x, topRight.y);
        ::rlVertex2f(bottomRight.x, bottomRight.y);

        ::rlEnd();
#endif
    }

    void DrawRectangleOutline(Rectangle rec, Vector2 origin, float rotation, float lineThick, Color color)
    {
        if ((lineThick > rec.width) || (lineThick > rec.height))
        {
            if (rec.width > rec.height) lineThick = rec.height / 2;
            else if (rec.width < rec.height) lineThick = rec.width / 2;
        }

        Rectangle top = { rec.x, rec.y, rec.width, lineThick };
        Rectangle bottom = { rec.x, rec.y - lineThick + rec.height, rec.width, lineThick };
        Rectangle left = { rec.x, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };
        Rectangle right = { rec.x - lineThick + rec.width, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };

        DrawRectangleProFlipped(top, origin, rotation, { color.r, color.g, color.b, color.a });
        DrawRectangleProFlipped(bottom, origin, rotation, { color.r, color.g, color.b, color.a });
        DrawRectangleProFlipped(left, origin, rotation, { color.r, color.g, color.b, color.a });
        DrawRectangleProFlipped(right, origin, rotation, { color.r, color.g, color.b, color.a });
    }

    // A modified version of DrawCircleSector to flip the quad
    void DrawCircleSectorFlipped(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color)
    {
        if (radius <= 0.0f) radius = 0.1f;  // Avoid div by zero

        // Function expects (endAngle > startAngle)
        if (endAngle < startAngle)
        {
            // Swap values
            float tmp = startAngle;
            startAngle = endAngle;
            endAngle = tmp;
        }

        int minSegments = (int)ceilf((endAngle - startAngle) / 90);

        if (segments < minSegments)
        {
            // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
            float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
            segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

            if (segments <= 0) segments = minSegments;
        }

        float stepLength = (endAngle - startAngle) / (float)segments;
        float angle = startAngle;

#if defined(SUPPORT_QUADS_DRAW_MODE)
        ::rlSetTexture(GetShapesTexture().id);
        ::Rectangle shapeRect = GetShapesTextureRectangle();

        ::rlBegin(RL_QUADS);

        // NOTE: Every QUAD actually represents two segments
        for (int i = 0; i < segments / 2; i++)
        {
            ::rlColor4ub(color.r, color.g, color.b, color.a);

            ::rlTexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
            ::rlVertex2f(center.x, center.y);

            ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
            ::rlVertex2f(center.x + cosf(DEG2RAD * (angle + stepLength * 2.0f)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength * 2.0f)) * radius);

            ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
            ::rlVertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);

            ::rlTexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
            ::rlVertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);

            angle += (stepLength * 2.0f);
        }

        // NOTE: In case number of segments is odd, we add one last piece to the cake
        if (((unsigned int)segments % 2) == 1)
        {
            ::rlColor4ub(color.r, color.g, color.b, color.a);

            ::rlTexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
            ::rlVertex2f(center.x, center.y);

            ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
            ::rlVertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);

            ::rlTexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
            ::rlVertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);

            ::rlTexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
            ::rlVertex2f(center.x, center.y);
        }

        ::rlEnd();

        ::rlSetTexture(0);
#else
        ::rlBegin(RL_TRIANGLES);
        for (int i = 0; i < segments; i++)
        {
            ::rlColor4ub(color.r, color.g, color.b, color.a);

            ::rlVertex2f(center.x, center.y);
            ::rlVertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);
            ::rlVertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);

            angle += stepLength;
        }
        ::rlEnd();
#endif
    }

    void DrawCircleLinesV(Vector2 center, float radius, Color color)
    {
        rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
        for (int i = 0; i < 360; i += 10)
        {
            rlVertex2f(center.x + cosf(DEG2RAD * i) * radius, center.y + sinf(DEG2RAD * i) * radius);
            rlVertex2f(center.x + cosf(DEG2RAD * (i + 10)) * radius, center.y + sinf(DEG2RAD * (i + 10)) * radius);
        }
        rlEnd();
    }


    // Input-related functions: keyboard
    bool IsKeyPressed(int key) {
        return ::IsKeyPressed(key);
    }

    bool IsKeyDown(int key) {
        return ::IsKeyDown(key);
    }

    bool IsKeyReleased(int key) {
        return ::IsKeyReleased(key);
    }

    bool IsKeyUp(int key) {
        return ::IsKeyUp(key);
    }

    void SetExitKey(int key) {
        ::SetExitKey(key);
    }

    int GetKeyPressed(void) {
        return ::GetKeyPressed();
    }

    int GetCharPressed(void) {
        return ::GetCharPressed();
    }

    // Input-related functions: gamepads
    bool IsGamepadAvailable(int gamepad) {
        return ::IsGamepadAvailable(gamepad);
    }

    const char* GetGamepadName(int gamepad) {
        return ::GetGamepadName(gamepad);
    }

    bool IsGamepadButtonPressed(int gamepad, int button) {
        return ::IsGamepadButtonPressed(gamepad, button);
    }

    bool IsGamepadButtonDown(int gamepad, int button) {
        return ::IsGamepadButtonDown(gamepad, button);
    }

    bool IsGamepadButtonReleased(int gamepad, int button) {
        return ::IsGamepadButtonReleased(gamepad, button);
    }

    bool IsGamepadButtonUp(int gamepad, int button) {
        return ::IsGamepadButtonUp(gamepad, button);
    }

    int GetGamepadButtonPressed(void) {
        return ::GetGamepadButtonPressed();
    }

    int GetGamepadAxisCount(int gamepad) {
        return ::GetGamepadAxisCount(gamepad);
    }

    float GetGamepadAxisMovement(int gamepad, int axis) {
        return ::GetGamepadAxisMovement(gamepad, axis);
    }

    int SetGamepadMappings(const char* mappings) {
        return ::SetGamepadMappings(mappings);
    }

    // Input-related functions: mouse
    bool IsMouseButtonPressed(int button) {
        return ::IsMouseButtonPressed(button);
    }

    bool IsMouseButtonDown(int button) {
        return ::IsMouseButtonDown(button);
    }

    bool IsMouseButtonReleased(int button) {
        return ::IsMouseButtonReleased(button);
    }

    bool IsMouseButtonUp(int button) {
        return ::IsMouseButtonUp(button);
    }

    int GetMouseX(void) {
        return ::GetMouseX();
    }

    int GetMouseY(void) {
        return ::GetMouseY();
    }

    Vector2 GetMousePosition(void) {
        ::Vector2 pos = ::GetMousePosition();
        return {pos.x, pos.y};
    }

    Vector2 GetMouseDelta(void) {
        ::Vector2 pos = ::GetMouseDelta();
        return { pos.x, pos.y };
    }

    void SetMousePosition(int x, int y) {
        ::SetMousePosition(x, y);
    }

    void SetMouseOffset(int offsetX, int offsetY) {
        ::SetMouseOffset(offsetX, offsetY);
    }

    void SetMouseScale(float scaleX, float scaleY) {
        ::SetMouseScale(scaleX, scaleY);
    }

    float GetMouseWheelMove(void) {
        return ::GetMouseWheelMove();
    }

    Vector2 GetMouseWheelMoveV(void) {
        ::Vector2 pos = ::GetMouseWheelMoveV();
        return { pos.x, pos.y };
    }

    void SetMouseCursor(int cursor) {
        ::SetMouseCursor(cursor);
    }

    // Input-related functions: touch
    int GetTouchX(void) {
        return ::GetTouchX();
    }

    int GetTouchY(void) {
        return ::GetTouchY();
    }

    Vector2 GetTouchPosition(int index) {
        ::Vector2 pos = ::GetTouchPosition(index);
        return { pos.x, pos.y };
    }

    int GetTouchPointId(int index) {
        return ::GetTouchPointId(index);
    }

    int GetTouchPointCount(void) {
        return ::GetTouchPointCount();
    }


    // Camera management functions
    void UpdateCamera(Camera* camera, int mode) {
        ::Camera camera2 = { {camera->position.x, camera->position.y, camera->position.z}, {camera->target.x, camera->target.y, camera->target.z}, {camera->up.x, camera->up.y, camera->up.z}, camera->fovy, camera->projection };
        ::UpdateCamera(&camera2, mode);
        camera->position.x = camera2.position.x;
        camera->position.y = camera2.position.y;
        camera->position.z = camera2.position.z;
        camera->target.x = camera2.target.x;
        camera->target.y = camera2.target.y;
        camera->target.z = camera2.target.z;
        camera->up.x = camera2.up.x;
        camera->up.y = camera2.up.y;
        camera->up.z = camera2.up.z;
        camera->fovy = camera2.fovy;
        camera->projection = camera2.projection;
    }

    //void UpdateCameraPro(Camera* camera, Vector3 movement, Vector3 rotation, float zoom) {
    //    ::UpdateCameraPro(camera, movement, rotation, zoom);
    //}


    // Filesystem-related functions
    //void UnloadDirectoryFiles(FilePathList files) {
    //    for (auto const& file : files) {
    //        ::UnloadFile(file.c_str());
    //    }
    //}

    bool IsFileDropped(void) {
        return ::IsFileDropped();
    }

    FilePathList LoadDroppedFiles(void) {
        ::FilePathList filePathList = ::LoadDroppedFiles();
        return { filePathList.capacity, filePathList.count, filePathList.paths };
    }

    void UnloadDroppedFiles(FilePathList files) {
        ::UnloadDroppedFiles({ files.capacity, files.count, files.paths });
    }



    void DrawGrid(int slices, float spacing) {
        ::DrawGrid(slices, spacing);
    }


    // Basic 2D Collisions
    bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2) {
        return ::CheckCollisionRecs({rec1.x, rec1.y, rec1.width, rec1.height}, { rec2.x, rec2.y, rec2.width, rec2.height });
    }

    bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2) {
        return ::CheckCollisionCircles({ center1.x, center1.y }, radius1, { center2.x, center2.y }, radius2);
    }

    bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec) {
        return ::CheckCollisionCircleRec({center.x, center.y}, radius, { rec.x, rec.y, rec.width, rec.height });
    }


    //Vector Math
    Vector2 Vector2Add(Vector2 v1, Vector2 v2)
    {
        Vector2 result = { v1.x + v2.x, v1.y + v2.y };

        return result;
    }

    Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
    {
        Vector2 result = { v1.x - v2.x, v1.y - v2.y };

        return result;
    }

    Vector3 Vector3Add(Vector3 v1, Vector3 v2)
    {
        Vector3 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };

        return result;
    }

    Vector3 Vector3Subtract(Vector3 v1, Vector3 v2)
    {
        Vector3 result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };

        return result;
    }

    Vector3 Vector3Scale(Vector3 v, float scalar)
    {
        Vector3 result = { v.x * scalar, v.y * scalar, v.z * scalar };

        return result;
    }

    Quaternion QuaternionFromEuler(float pitch, float yaw, float roll)
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

    Vector3 QuaternionToEuler(Quaternion q)
    {
        Vector3 result = { 0 };

        // Roll (x-axis rotation)
        float x0 = 2.0f * (q.w * q.x + q.y * q.z);
        float x1 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        result.x = atan2f(x0, x1);

        // Pitch (y-axis rotation)
        float y0 = 2.0f * (q.w * q.y - q.z * q.x);
        y0 = y0 > 1.0f ? 1.0f : y0;
        y0 = y0 < -1.0f ? -1.0f : y0;
        result.y = asinf(y0);

        // Yaw (z-axis rotation)
        float z0 = 2.0f * (q.w * q.z + q.x * q.y);
        float z1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
        result.z = atan2f(z0, z1);

        return result;
    }


    // Model animations
    //ModelAnimation *LoadModelAnimations(const char *fileName, int *animCount) {
    //    ::ModelAnimation* raylibModelAnimations = ::LoadModelAnimations(fileName, animCount);

    //    ModelAnimation* wrapperModelAnimations = new ModelAnimation();

    //    wrapperModelAnimations->boneCount = raylibModelAnimations->boneCount;
    //    wrapperModelAnimations->frameCount = raylibModelAnimations->frameCount;

    //    wrapperModelAnimations->bones = new BoneInfo[raylibModelAnimations->boneCount];
    //    for (int i = 0; i < raylibModelAnimations->boneCount; ++i)
    //    {
    //        strcpy_s(wrapperModelAnimations->bones[i].name, sizeof(wrapperModelAnimations->bones[i].name), raylibModelAnimations->bones[i].name);
    //        wrapperModelAnimations->bones[i].parent = raylibModelAnimations->bones[i].parent;
    //    }

    //    wrapperModelAnimations->framePoses = new Transform * [raylibModelAnimations->frameCount];
    //    for (int i = 0; i < raylibModelAnimations->frameCount; ++i)
    //    {
    //        wrapperModelAnimations->framePoses[i] = new Transform[raylibModelAnimations->boneCount];
    //        for (int j = 0; j < raylibModelAnimations->boneCount; ++j)
    //        {
    //            wrapperModelAnimations->framePoses[i][j].translation = { raylibModelAnimations->framePoses[i][j].translation.x,
    //                raylibModelAnimations->framePoses[i][j].translation.y,
    //                raylibModelAnimations->framePoses[i][j].translation.z };
    //            wrapperModelAnimations->framePoses[i][j].rotation = {raylibModelAnimations->framePoses[i][j].rotation.x,
    //                raylibModelAnimations->framePoses[i][j].rotation.y,
    //                raylibModelAnimations->framePoses[i][j].rotation.z,
    //                raylibModelAnimations->framePoses[i][j].rotation.w };
    //            wrapperModelAnimations->framePoses[i][j].scale = { raylibModelAnimations->framePoses[i][j].scale.x,
    //                raylibModelAnimations->framePoses[i][j].scale.y,
    //                raylibModelAnimations->framePoses[i][j].scale.z };

    //        }
    //    }

    //    ::UnloadModelAnimations(raylibModelAnimations, *animCount);

    //    return wrapperModelAnimations;
    //}
    //void UpdateModelAnimation(Model model, ModelAnimation anim, int frame) { // Moved to RaylibModelWrapper
    //}
    void UnloadModelAnimation(ModelAnimation anim) {
        for (int i = 0; i < anim.frameCount; i++) RL_FREE(anim.framePoses[i]);

        RL_FREE(anim.bones);
        RL_FREE(anim.framePoses);
    }
    void UnloadModelAnimations(ModelAnimation* animations, int animCount) {
        for (int i = 0; i < animCount; i++) UnloadModelAnimation(animations[i]);
        RL_FREE(animations);
    }
    //bool IsModelAnimationValid(Model model, ModelAnimation anim) { // Moved to RaylibModelWrapper
    //    return false;
    //}


    const char* GetWorkingDirectory(void) {
        return ::GetWorkingDirectory();
    }

    void InitAudioDevice(void) {
        ::InitAudioDevice();
    }

    void CloseAudioDevice(void) {
        ::CloseAudioDevice();
    }

    bool IsAudioDeviceReady(void) {
        return ::IsAudioDeviceReady();
    }

    void SetMasterVolume(float volume) {
        ::SetMasterVolume(volume);
    }

    float GetMasterVolume(void) {
        return ::GetMasterVolume();
    }


    //Wave LoadWave(const char* fileName)
    //{
    //    ::Wave wave = ::LoadWave(fileName);
    //    return { wave.frameCount, wave.sampleRate, wave.sampleSize, wave.channels, wave.data };
    //}

    //bool IsWaveReady(Wave wave)
    //{
    //    return ::IsWaveReady({ wave.frameCount, wave.sampleRate, wave.sampleSize, wave.channels, wave.data });
    //}

    //Sound LoadSound(const char* fileName)
    //{
    //    ::Sound sound = ::LoadSound(fileName);
    //    return { { sound.stream.buffer, sound.stream.processor, sound.stream.sampleRate, sound.stream.sampleSize, sound.stream.channels }, sound.frameCount };
    //}


    //Sound LoadSoundFromWave(Wave wave)
    //{
    //    return ::LoadSoundFromWave({ wave.frameCount, wave.sampleRate, wave.sampleSize, wave.channels, wave.data });
    //}

    //bool IsSoundReady(Sound sound)
    //{
    //    return ::IsSoundReady(sound);
    //}

    //void UpdateSound(Sound sound, const void* data, int sampleCount)
    //{
    //    ::UpdateSound(sound, data, sampleCount);
    //}

    //void UnloadWave(Wave wave)
    //{
    //    ::UnloadWave(wave);
    //}

    //void UnloadSound(Sound sound)
    //{
    //    ::UnloadSound(sound);
    //}

    //void PlaySound(Sound sound)
    //{
    //    ::PlaySound(sound);
    //}

    //void StopSound(Sound sound)
    //{
    //    ::StopSound(sound);
    //}

    //void PauseSound(Sound sound)
    //{
    //    ::PauseSound(sound);
    //}

    //void ResumeSound(Sound sound)
    //{
    //    ::ResumeSound(sound);
    //}

    //bool IsSoundPlaying(Sound sound)
    //{
    //    return ::IsSoundPlaying(sound);
    //}

    //void SetSoundVolume(Sound sound, float volume)
    //{
    //    ::SetSoundVolume(sound, volume);
    //}

    //void SetSoundPitch(Sound sound, float pitch)
    //{
    //    ::SetSoundPitch(sound, pitch);
    //}

    //void SetSoundPan(Sound sound, float pan)
    //{
    //    ::SetSoundPan(sound, pan);
    //}

    //Music LoadMusicStream(const char* fileName)
    //{
    //    return ::LoadMusicStream(fileName);
    //}

    //Music LoadMusicStreamFromMemory(const char* fileType, const unsigned char* data, int dataSize)
    //{
    //    return ::LoadMusicStreamFromMemory(fileType, data, dataSize);
    //}

    //bool IsMusicReady(Music music)
    //{
    //    return ::IsMusicReady(music);
    //}

    //void UnloadMusicStream(Music music)
    //{
    //    ::UnloadMusicStream(music);
    //}

    //void PlayMusicStream(Music music)
    //{
    //    ::PlayMusicStream(music);
    //}

    //bool IsMusicStreamPlaying(Music music)
    //{
    //    return ::IsMusicStreamPlaying(music);
    //}

    //void UpdateMusicStream(Music music)
    //{
    //    ::UpdateMusicStream(music);
    //}

    //void StopMusicStream(Music music)
    //{
    //    ::StopMusicStream(music);
    //}

    //void PauseMusicStream(Music music)
    //{
    //    ::PauseMusicStream(music);
    //}

    //void ResumeMusicStream(Music music)
    //{
    //    ::ResumeMusicStream(music);
    //}

    //void SetMusicVolume(Music music, float volume)
    //{
    //    ::SetMusicVolume(music, volume);
    //}

    //void SetMusicPitch(Music music, float pitch)
    //{
    //    ::SetMusicPitch(music, pitch);
    //}

    //void SetMusicPan(Music music, float pan)
    //{
    //    ::SetMusicPan(music, pan);
    //}

    //float GetMusicTimeLength(Music music)
    //{
    //    return ::GetMusicTimeLength(music);
    //}

    //float GetMusicTimePlayed(Music music)
    //{
    //    return ::GetMusicTimePlayed(music);
    //}


    // ImGui Raylib
    bool ImGui_ImplRaylib_Init() {
        return ::ImGui_ImplRaylib_Init();
    }

    void ImGui_ImplRaylib_Shutdown() {
        ::ImGui_ImplRaylib_Shutdown();
    }

    void ImGui_ImplRaylib_NewFrame() {
        ::ImGui_ImplRaylib_NewFrame();
    }

    void ImGui_ImplRaylib_RenderDrawData(ImDrawData* draw_data) {
        ::ImGui_ImplRaylib_RenderDrawData(draw_data);
    }

    bool ImGui_ImplRaylib_ProcessEvents() {
        return ::ImGui_ImplRaylib_ProcessEvents();
    }


    void rlImGuiSetup(bool darkTheme) {
        ::rlImGuiSetup(darkTheme);
    }

    void rlImGuiBegin() {
        ::rlImGuiBegin();
    }

    void rlImGuiEnd() {
        ::rlImGuiEnd();
    }

    void rlImGuiShutdown() {
        ::rlImGuiShutdown();
    }

    void rlImGuiBeginInitImGui() {
        ::rlImGuiBeginInitImGui();
    }

    void rlImGuiEndInitImGui() {
        ::rlImGuiEndInitImGui();
    }

    void rlImGuiReloadFonts() {
        ::rlImGuiReloadFonts();
    }

    void rlImGuiBeginDelta(float deltaTime) {
        ::rlImGuiBeginDelta(deltaTime);
    }

    void rlImGuiImage(const Texture* image) {
        ImGui::Image((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
    }

    void rlImGuiImageSize(const Texture* image, int width, int height) {
        ImGui::Image((ImTextureID)image, ImVec2(float(width), float(height)));
    }

    void rlImGuiImageSizeV(const Texture* image, Vector2 size) {
        ImGui::Image((ImTextureID)image, ImVec2(size.x, size.y));
    }

    void rlImGuiImageRect(const Texture* image, int destWidth, int destHeight, Rectangle sourceRect) {
        ImVec2 uv0;
        ImVec2 uv1;

        if (sourceRect.width < 0)
        {
            uv0.x = -((float)sourceRect.x / image->width);
            uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / image->width));
        }
        else
        {
            uv0.x = (float)sourceRect.x / image->width;
            uv1.x = uv0.x + (float)(sourceRect.width / image->width);
        }

        if (sourceRect.height < 0)
        {
            uv0.y = -((float)sourceRect.y / image->height);
            uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / image->height));
        }
        else
        {
            uv0.y = (float)sourceRect.y / image->height;
            uv1.y = uv0.y + (float)(sourceRect.height / image->height);
        }

        ImGui::Image((ImTextureID)image, ImVec2(float(destWidth), float(destHeight)), uv0, uv1);
    }

    void rlImGuiImageRenderTexture(const RenderTexture* image) {
        rlImGuiImageRect(&image->texture, image->texture.width, image->texture.height, Rectangle{ 0,0, float(image->texture.width), -float(image->texture.height) });
    }

    void rlImGuiImageRenderTextureFit(const RenderTexture* image, bool center) {
        ImVec2 area = ImGui::GetContentRegionAvail();

        float scale = area.x / image->texture.width;

        float y = image->texture.height * scale;
        if (y > area.y)
        {
            scale = area.y / image->texture.height;
        }

        int sizeX = int(image->texture.width * scale);
        int sizeY = int(image->texture.height * scale);

        if (center)
        {
            ImGui::SetCursorPosX(0);
            ImGui::SetCursorPosX(area.x / 2 - sizeX / 2);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (area.y / 2 - sizeY / 2));
        }

        rlImGuiImageRect(&image->texture, sizeX, sizeY, Rectangle{ 0,0, float(image->texture.width), -float(image->texture.height) });
    }

    bool rlImGuiImageButton(const char* name, const Texture* image) {
        return ImGui::ImageButton(name, (ImTextureID)image, ImVec2(float(image->width), float(image->height)));
    }

    bool rlImGuiImageButtonSize(const char* name, const Texture* image, ImVec2 size) {
        return ImGui::ImageButton(name, (ImTextureID)image, size);
    }

    void Imgui_ImplRaylib_BuildFontAtlas() {
        ::Imgui_ImplRaylib_BuildFontAtlas();
    }
}