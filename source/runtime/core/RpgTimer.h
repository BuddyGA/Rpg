#pragma once

#include "RpgPlatform.h"



class RpgTimer
{
private:
	float PerformanceFrequency;
	uint64_t StartTickCounter;
	uint64_t PrevTickCounter;
	float DeltaTime;

public:
	// Micro seconds
	static constexpr int TIME_UNIT = 1000000;


public:
	RpgTimer() noexcept
	{
		PerformanceFrequency = static_cast<float>(SDL_GetPerformanceFrequency());
		StartTickCounter = 0;
		PrevTickCounter = 0;
		DeltaTime = 0.0f;
	}


	// Reset and stop the timer
	inline void Reset() noexcept
	{
		StartTickCounter = 0;
		PrevTickCounter = 0;
		DeltaTime = 0.0f;
	}


	// Start the timer if it hasn't started
	inline void Start() noexcept
	{
		if (StartTickCounter == 0)
		{
			StartTickCounter = SDL_GetPerformanceCounter();
			PrevTickCounter = StartTickCounter;
		}
	}


	// Tick the timer
	// @returns Delta time in micro seconds
	inline float Tick() noexcept
	{
		const uint64_t currentTickCounter = SDL_GetPerformanceCounter();
		DeltaTime = static_cast<float>((currentTickCounter - PrevTickCounter) * TIME_UNIT) / PerformanceFrequency;
		PrevTickCounter = currentTickCounter;

		return DeltaTime;
	}


	[[nodiscard]] inline uint64_t GetCurrentTickCounter() const noexcept
	{
		return PrevTickCounter;
	}


	// Get delta time in seconds
	// @returns Delta time in seconds
	[[nodiscard]] inline float GetDeltaTimeSeconds() const noexcept
	{
		return DeltaTime / TIME_UNIT;
	}

};
