#pragma once

#include "core/RpgThreadPool.h"


class RpgWorld;
class RpgRenderComponent_Camera;



class RpgAsyncTask_CaptureMesh : public RpgThreadTask
{
public:
	RpgWorld* World;
	RpgRenderComponent_Camera* Camera;


public:
	RpgAsyncTask_CaptureMesh() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;


	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_CaptureMesh";
	}

};



class RpgAsyncTask_CaptureLight : public RpgThreadTask
{
public:
	RpgWorld* World;
	RpgRenderComponent_Camera* Camera;


public:
	RpgAsyncTask_CaptureLight() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;


	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_CaptureLight";
	}

};
