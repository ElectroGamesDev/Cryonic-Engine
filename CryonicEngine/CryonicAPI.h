#pragma once

// Core
#include "ConsoleLogger.h"
#include "GameObject.h"
#include "Scenes/SceneManager.h"
#include "InputSystem.h"
#include "EventSystem.h"
#include "AnimationGraph.h"

// Rendering
#include "Components/MeshRenderer.h"
#include "Components/SpriteRenderer.h"
#include "Components/Lighting.h"
#include "Components/Skybox.h"

// Physics
#include "Components/Collider2D.h"
#include "Components/Rigidbody2D.h"
#if defined(IS3D)
#include "Components/Collider3D.h"
#include "Components/Rigidbody3D.h"
#endif

// UI
#include "Components/Label.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasRenderer.h"

// Misc
#include "Components/Component.h"
#include "Components/AnimationPlayer.h"
#include "Components/AudioPlayer.h"
#include "Components/CameraComponent.h"
#include <functional>