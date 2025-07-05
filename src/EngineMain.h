#pragma once

//Core
#include <Core/Application.h>
#include <Core/Scene.h>
#include <Core/SceneManager.h>
#include <Core/Window.h>
#include <Core/ISystem.h>

// Render Components
#include <Graphics/Camera.h>
#include <Components/SpriteRenderer.h>
#include <Components/Transform.h>

//Physics Components
#include <Components/BoxCollider.h>
#include <Components/CircleCollider.h>
#include <Components/Rigidbody2D.h>

//Graphics Utils
#include <Graphics/Gizmos.h>

//Other Utils
#include <Utils/Time.h>
#include <Utils/Input.h>
#include <Utils/Debug.h>
#include <Utils/randomr.h>

#include <Experimental/CameraSystem.h>
#include <Graphics/GizmosRenderSystem.h>

using namespace engine;
using namespace graphics;