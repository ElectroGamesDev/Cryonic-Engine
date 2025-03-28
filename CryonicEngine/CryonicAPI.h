#pragma once

#include <functional>

#include "ConsoleLogger.h"
#include "GameObject.h"
#include "Scenes/SceneManager.h"
#include "InputSystem.h"
#include "EventSystem.h"
#include "AnimationGraph.h"

#include "Components/Component.h"
#include "Components/AnimationPlayer.h"
#include "Components/AudioPlayer.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"
#include "Components/Lighting.h"
#include "Components/SpriteRenderer.h"
#include "Components/Collider2D.h"
#include "Components/Rigidbody2D.h"
#if defined(IS3D)
#include "Components/Collider3D.h"
#include "Components/Rigidbody3D.h"
#endif
#include "Components/Skybox.h"
#include "Components/Label.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasRenderer.h"