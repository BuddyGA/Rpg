#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <utility>


#define RPG_INDEX_INVALID			-1
#define RPG_INDEX_FIRST				0
#define RPG_INDEX_LAST				(INT32_MAX - 1)
#define RPG_MAX_COUNT				INT32_MAX

#define RPG_MEMORY_SIZE_KiB(n)		(n * 1024)
#define RPG_MEMORY_SIZE_MiB(n)		(RPG_MEMORY_SIZE_KiB(n) * 1024)
#define RPG_MEMORY_SIZE_GiB(n)		(RPG_MEMORY_SIZE_MiB(n) * 1024)

#define RPG_FRAME_BUFFERING			3



#define RPG_NOCOPY(type)					\
private:									\
	type(const type&) = delete;				\
	type& operator=(const type&) = delete;	


#define RPG_NOCOPYMOVE(type)				\
private:									\
	type(const type&) = delete;				\
	type(type&&) = delete;					\
	type& operator=(const type&) = delete;	\
	type& operator=(type&&) = delete;


#define RPG_SINGLETON(type)							\
RPG_NOCOPYMOVE(type)								\
type() noexcept;									\
public:												\
[[nodiscard]] static inline type& Get() noexcept	\
{													\
	static type s_instance;							\
	return s_instance;								\
}



class RpgPointInt
{
public:
	int X, Y;

public:
	RpgPointInt() noexcept
		: X(0), Y(0)
	{
	}

	RpgPointInt(int in_Value) noexcept
		: X(in_Value), Y(in_Value)
	{
	}

	RpgPointInt(int in_X, int in_Y) noexcept
		: X(in_X), Y(in_Y)
	{
	}

public:
	inline RpgPointInt operator+(const RpgPointInt& rhs) const noexcept
	{
		return RpgPointInt(X + rhs.X, Y + rhs.Y);
	}

	inline RpgPointInt& operator+=(const RpgPointInt& rhs) noexcept
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	inline RpgPointInt operator-(const RpgPointInt& rhs) const noexcept
	{
		return RpgPointInt(X - rhs.X, Y - rhs.Y);
	}

	inline RpgPointInt& operator-=(const RpgPointInt& rhs) noexcept
	{
		X -= rhs.X;
		Y -= rhs.Y;
		return *this;
	}

	inline bool operator==(const RpgPointInt& rhs) const noexcept
	{
		return X == rhs.X && Y == rhs.Y;
	}

	inline bool operator!=(const RpgPointInt& rhs) const noexcept
	{
		return !(*this == rhs);
	}

};



class RpgPointFloat
{
public:
	float X, Y;

public:
	RpgPointFloat() noexcept
		: X(0.0f), Y(0.0f)
	{
	}

	RpgPointFloat(float in_X, float in_Y) noexcept
		: X(in_X), Y(in_Y)
	{
	}


public:
	inline RpgPointFloat operator+(const RpgPointFloat& rhs) const noexcept
	{
		return RpgPointFloat(X + rhs.X, Y + rhs.Y);
	}

	inline RpgPointFloat& operator+=(const RpgPointFloat& rhs) noexcept
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	inline RpgPointFloat operator-(const RpgPointFloat& rhs) const noexcept
	{
		return RpgPointFloat(X - rhs.X, Y - rhs.Y);
	}

	inline RpgPointFloat& operator-=(const RpgPointFloat& rhs) noexcept
	{
		X -= rhs.X;
		Y -= rhs.Y;
		return *this;
	}

	inline bool operator==(const RpgPointFloat& rhs) const noexcept
	{
		return X == rhs.X && Y == rhs.Y;
	}

	inline bool operator!=(const RpgPointFloat& rhs) const noexcept
	{
		return !(*this == rhs);
	}

};



class RpgRectInt
{
public:
	int Left, Top, Right, Bottom;


public:
	RpgRectInt() noexcept
		: Left(0), Top(0), Right(0), Bottom(0)
	{
	}

	RpgRectInt(int in_Left, int in_Top, int in_Right, int in_Bottom) noexcept
		: Left(in_Left), Top(in_Top), Right(in_Right), Bottom(in_Bottom)
	{
	}

public:
	inline bool operator==(const RpgRectInt& rhs) const noexcept
	{
		return Left == rhs.Left && Top == rhs.Top && Right == rhs.Right && Bottom == rhs.Bottom;
	}

	inline bool operator!=(const RpgRectInt& rhs) const noexcept
	{
		return !(*this == rhs);
	}

public:
	inline int GetWidth() const noexcept
	{
		return Right - Left;
	}

	inline int GetHeight() const noexcept
	{
		return Bottom - Top;
	}

	inline bool TestIntersectPoint(const RpgPointInt& p) const noexcept
	{
		return (p.X >= Left && p.X <= Right && p.Y >= Top && p.Y <= Bottom);
	}

	inline bool TestIntersectRect(const RpgRectInt& r) const noexcept
	{
		return !(r.Right < Left || r.Left > Right || r.Top > Bottom || r.Bottom < Top);
	}

};



class RpgRectFloat
{
public:
	float Left, Top, Right, Bottom;


public:
	RpgRectFloat() noexcept
		: Left(0.0f), Top(0.0f), Right(0.0f), Bottom(0.0f)
	{
	}

	RpgRectFloat(float in_Left, float in_Top, float in_Right, float in_Bottom) noexcept
		: Left(in_Left), Top(in_Top), Right(in_Right), Bottom(in_Bottom)
	{
	}

public:
	inline bool operator==(const RpgRectFloat& rhs) const noexcept
	{
		return Left == rhs.Left && Top == rhs.Top && Right == rhs.Right && Bottom == rhs.Bottom;
	}

	inline bool operator!=(const RpgRectFloat& rhs) const noexcept
	{
		return !(*this == rhs);
	}

public:
	inline float GetWidth() const noexcept
	{
		return Right - Left;
	}

	inline float GetHeight() const noexcept
	{
		return Bottom - Top;
	}

};



class RpgRectBorders
{
public:
	enum EBorder : uint8_t
	{
		NONE = 0,
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOTTOM,
		RIGHT_BOTTOM,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		MAX_COUNT
	};

	RpgRectInt BorderRects[MAX_COUNT];
	int BorderThickness;
	int SpaceBetweenBorder;


public:
	RpgRectBorders() noexcept
		: BorderThickness(2)
		, SpaceBetweenBorder(0)
	{
	}

	RpgRectBorders(const RpgRectInt& windowRect, int borderThickness, int spaceBetweenBorder) noexcept
		: BorderThickness(borderThickness)
		, SpaceBetweenBorder(spaceBetweenBorder)
	{
		UpdateRects(windowRect);
	}

	inline void UpdateRects(const RpgRectInt& windowRect) noexcept
	{
		RpgRectInt& bdr_LT = BorderRects[LEFT_TOP];
		bdr_LT.Left = windowRect.Left;
		bdr_LT.Top = windowRect.Top;
		bdr_LT.Right = bdr_LT.Left + BorderThickness;
		bdr_LT.Bottom = bdr_LT.Top + BorderThickness;

		RpgRectInt& bdr_RT = BorderRects[RIGHT_TOP];
		bdr_RT.Right = windowRect.Right;
		bdr_RT.Top = windowRect.Top;
		bdr_RT.Left = bdr_RT.Right - BorderThickness;
		bdr_RT.Bottom = bdr_RT.Top + BorderThickness;

		RpgRectInt& bdr_LB = BorderRects[LEFT_BOTTOM];
		bdr_LB.Left = windowRect.Left;
		bdr_LB.Bottom = windowRect.Bottom;
		bdr_LB.Right = bdr_LB.Left + BorderThickness;
		bdr_LB.Top = bdr_LB.Bottom - BorderThickness;

		RpgRectInt& bdr_RB = BorderRects[RIGHT_BOTTOM];
		bdr_RB.Right = windowRect.Right;
		bdr_RB.Bottom = windowRect.Bottom;
		bdr_RB.Left = bdr_RB.Right - BorderThickness;
		bdr_RB.Top = bdr_RB.Bottom - BorderThickness;

		RpgRectInt& bdr_L = BorderRects[LEFT];
		bdr_L.Left = bdr_LT.Left;
		bdr_L.Right = bdr_LT.Right;
		bdr_L.Top = bdr_LT.Bottom + SpaceBetweenBorder;
		bdr_L.Bottom = bdr_LB.Top - SpaceBetweenBorder;

		RpgRectInt& bdr_R = BorderRects[RIGHT];
		bdr_R.Left = bdr_RT.Left;
		bdr_R.Right = bdr_RT.Right;
		bdr_R.Top = bdr_RT.Bottom + SpaceBetweenBorder;
		bdr_R.Bottom = bdr_RB.Top - SpaceBetweenBorder;

		RpgRectInt& bdr_T = BorderRects[TOP];
		bdr_T.Top = bdr_LT.Top;
		bdr_T.Bottom = bdr_LT.Bottom;
		bdr_T.Left = bdr_LT.Right + SpaceBetweenBorder;
		bdr_T.Right = bdr_RT.Left - SpaceBetweenBorder;

		RpgRectInt& bdr_B = BorderRects[BOTTOM];
		bdr_B.Top = bdr_LB.Top;
		bdr_B.Bottom = bdr_LB.Bottom;
		bdr_B.Left = bdr_LB.Right + SpaceBetweenBorder;
		bdr_B.Right = bdr_RB.Left - SpaceBetweenBorder;
	}


	inline EBorder TestIntersectBorder(const RpgPointInt& p) const noexcept
	{
		for (int i = 1; i < MAX_COUNT; ++i)
		{
			if (BorderRects[i].TestIntersectPoint(p))
			{
				return static_cast<EBorder>(i);
			}
		}

		return NONE;
	}

};


class RpgColorRGBA
{
public:
	uint8_t R, G, B, A;

	static const RpgColorRGBA BLACK;
	static const RpgColorRGBA BLACK_TRANSPARENT;
	static const RpgColorRGBA BLUE;
	static const RpgColorRGBA GREEN;
	static const RpgColorRGBA RED;
	static const RpgColorRGBA WHITE;
	static const RpgColorRGBA WHITE_TRANSPARENT;
	static const RpgColorRGBA YELLOW;


public:
	RpgColorRGBA() noexcept
		: R(0), G(0), B(0), A(0)
	{
	}

	RpgColorRGBA(uint8_t in_R, uint8_t in_G, uint8_t in_B, uint8_t in_A = 255) noexcept
		: R(in_R), G(in_G), B(in_B), A(in_A)
	{
	}

};



class RpgColorLinear
{
public:
	float R, G, B, A;

	static const RpgColorLinear BLACK;
	static const RpgColorLinear BLACK_TRANSPARENT;
	static const RpgColorLinear BLUE;
	static const RpgColorLinear GREEN;
	static const RpgColorLinear RED;
	static const RpgColorLinear WHITE;
	static const RpgColorLinear WHITE_TRANSPARENT;
	static const RpgColorLinear YELLOW;


public:
	RpgColorLinear() noexcept
		: R(0), G(0), B(0), A(0)
	{
	}

	RpgColorLinear(float in_R, float in_G, float in_B, float in_A = 1.0f) noexcept
		: R(in_R), G(in_G), B(in_B), A(in_A)
	{
	}

};



class RpgInt3
{
public:
	int X, Y, Z;

public:
	RpgInt3() noexcept : X(0), Y(0), Z(0) {}
	RpgInt3(int in_Value) noexcept : X(in_Value), Y(in_Value), Z(in_Value) {}
	RpgInt3(int in_X, int in_Y, int in_Z) noexcept : X(in_X), Y(in_Y), Z(in_Z) {}

};




namespace RpgType
{
	template<typename T> struct IsIntegral { static constexpr bool Value = false; };
	template<> struct IsIntegral<int8_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<int16_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<int32_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<int64_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<uint8_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<uint16_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<uint32_t> { static constexpr bool Value = true; };
	template<> struct IsIntegral<uint64_t> { static constexpr bool Value = true; };


	template<typename T> struct IsFloat { static constexpr bool Value = false; };
	template<> struct IsFloat<float> { static constexpr bool Value = true; };
	template<> struct IsFloat<double> { static constexpr bool Value = true; };


	template<typename T>
	struct IsArithmetic
	{
		static constexpr bool Value = IsIntegral<T>::Value || IsFloat<T>::Value;
	};


	template<typename T>
	constexpr inline void BitSetCondition(T& out_Flags, T bitFlags, bool bCondition) noexcept
	{
		static_assert(IsIntegral<T>::Value, "RpgType::BitSetCondition type of <T> must be integral type!");
		out_Flags = (static_cast<T>(out_Flags) & ~static_cast<T>(bitFlags)) | (-static_cast<T>(bCondition) & static_cast<T>(bitFlags));
	}


	constexpr inline uint32_t Align(uint32_t offset, uint32_t alignment) noexcept
	{
		return (offset + (alignment - 1) & ~(alignment - 1));
	}

	constexpr inline int Align(int offset, int alignment) noexcept
	{
		return (offset + (alignment - 1) & ~(alignment - 1));
	}

};



[[nodiscard]] inline uint64_t Rpg_GetHash(int value) noexcept
{
	return static_cast<uint64_t>(value);
}

template<typename T>
[[nodiscard]] inline uint64_t Rpg_GetHash(T* value) noexcept
{
	return reinterpret_cast<uint64_t>(value);
}



enum class RpgAxis : uint8_t
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS
};



namespace RpgTextureFormat
{
	enum EType : uint8_t
	{
		NONE = 0,

		TEX_2D_R,		// Uncompressed, Red8 (Ex: Font, Mask, Specular, AO, Metallic, Roughness)
		TEX_2D_RG,		// Uncompressed, Red8-Green8
		TEX_2D_RGBA,	// Uncompressed, Red8-Green8-Blue8-Alpha8 (Ex: Base color, UI)
		TEX_2D_BC3U,	// Compressed, Red-Green-Blue-(Alpha) (Ex: Base color)
		TEX_2D_BC4U,	// Compressed, Red (Ex: Mask, Specular, AO, Metallic, Roughness)
		TEX_2D_BC5S,	// Compressed, Red-Green (Normal, Motion)
		TEX_2D_BC6H,	// Compressed, HDR
		TEX_2D_BC7U,	// Compressed, Red-Green-Blue-(Alpha)

		TEX_RT_RGBA,
		TEX_RT_BGRA,

		TEX_DS_16,
		TEX_DS_24_8,
		TEX_DS_32,

		MAX_COUNT
	};


	constexpr const char* NAMES[MAX_COUNT] =
	{
		"NONE",
		"TEX_2D_R",
		"TEX_2D_RGBA",
		"TEX_2D_SRGB",
		"TEX_2D_BC3",
		"TEX_2D_BC4",
		"TEX_2D_BC5",
		"TEX_2D_BC6H",
		"TEX_2D_BC7",
		"TEX_RT_RGBA",
		"TEX_RT_BGRA",
		"TEX_DS_16",
		"TEX_DS_24_8",
		"TEX_DS_32",
	};

};
