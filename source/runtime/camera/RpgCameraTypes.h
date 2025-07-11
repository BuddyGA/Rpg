#pragma once

#include "core/RpgPlatform.h"


RPG_LOG_DECLARE_CATEGORY_EXTERN(RpgLogCamera)


class RpgCameraComponent;
class RpgCameraWorldSubsystem;
class RpgAsyncTask_CaptureMesh;
class RpgAsyncTask_CaptureLight;


enum class RpgCameraProjectionMode : uint8_t
{
	PERSPECTIVE = 0,
	ORTHOGRAPIC
};
