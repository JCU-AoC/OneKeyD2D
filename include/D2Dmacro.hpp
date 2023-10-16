#pragma once
#include<d2d1.h>
#include<cmath>

/// <summary>
/// 释放d2d等含有Release函数的句柄
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}
/// <summary>
/// 释放new的内容
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeReleaseP(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        delete (*ppInterfaceToRelease);
        (*ppInterfaceToRelease) = NULL;
    }
}
/// <summary>
/// 释放win32gdi等使用DeleteObject函数释放资源的句柄
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeReleaseW(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        DeleteObject(*ppInterfaceToRelease);
        (*ppInterfaceToRelease) = NULL;
    }
}

class GetLastWindowError
{
    DWORD m_ErrorCode;
    LPSTR m_ErrorMessage;
public:
    GetLastWindowError()
    {
        m_ErrorCode = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&m_ErrorMessage, 0, NULL);
    }
    GetLastWindowError(HRESULT hr)
    {
        m_ErrorCode = HRESULT_CODE(hr);
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&m_ErrorMessage, 0, NULL);
    }
    ~GetLastWindowError()
    {
        if(m_ErrorMessage)
            LocalFree(m_ErrorMessage);
    }
    DWORD GetErrorCode() const
    {
        return m_ErrorCode;
    }
    LPSTR GetErrorMessage() const
    {
        return m_ErrorMessage;
    }
};
class GetLastWindowErrorW
{
    DWORD m_ErrorCode;
    LPWSTR m_ErrorMessage;
public:
    GetLastWindowErrorW()
    {
        SetConsoleOutputCP(936);
        m_ErrorCode = GetLastError();
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&m_ErrorMessage, 0, NULL);
    }
    GetLastWindowErrorW(HRESULT hr)
    {
        SetConsoleOutputCP(936);
        m_ErrorCode = HRESULT_CODE(hr);
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&m_ErrorMessage, 0, NULL);
    }
    ~GetLastWindowErrorW()
    {
        SetConsoleOutputCP(65001);
        if (m_ErrorMessage)
            LocalFree(m_ErrorMessage);
    }
    DWORD GetErrorCode() const
    {
        return m_ErrorCode;
    }
    LPWSTR GetErrorMessage() const
    {
        return m_ErrorMessage;
    }
};

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#include<string>

#ifdef UNICODE
    
typedef std::wstring STR;
typedef const std::wstring CSTR;
typedef std::wstring& QSTR;
typedef const std::wstring& CQSTR;

#else

typedef std::string STR;
typedef const std::string CSTR;
typedef std::string& QSTR;
typedef const std::string& CQSTR;

#endif // UNICODE

namespace Game {
    enum KeyMode
    {
        KM_CHAR,
        KM_DOWN,
        KM_UP,
    };
    enum MouseMessageType
    {
        MT_MOVE,
        MT_LEFT,
        MT_DOUBLE_LEFT,
        MT_RIGHT,
        MT_DOUBLE_RIGHT,
        MT_MID,
        MT_DOUBLE_MID,
        MT_WHEEL,
        MT_ENTER_WINDOW,
        MT_LEAVE_WINDOW,
    };

    enum class PenStyle
    {
        // 实线样式
        SolidLine = PS_SOLID,

        // 虚线样式
        DashedLine = PS_DASH,

        // 点线样式
        DotLine = PS_DOT,

        // 虚线点线样式
        DashDot = PS_DASHDOT,

        // 双点线样式
        DashDotDot = PS_DASHDOTDOT,

        // 空样式，不绘制轮廓
        NullLine = PS_NULL,

        // 内框样式
        InsideFrame = PS_INSIDEFRAME,
    };
    enum class ControlMessage
    {
        EnterControl,
        LeaveControl,
        ControlMessage,
    };
    class AutoInit
    {
    public:
        AutoInit()
        {
            std::wcout.imbue(std::locale("chs"));
            SetConsoleOutputCP(65001);
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                std::cerr << "初始化失败" << std::endl;
                return;
            }
            return;
        }
        ~AutoInit()
        {
            CoUninitialize();
        }
    };
    //向量
    namespace Vector
    {
        template<typename DataType>
        //using DataType = float;
        class Vector3
        {
        public:
            union {
                struct { DataType x, y, z; };
                struct { DataType r, g, b; };
            };
            Vector3() :x(0), y(0), z(0) {}
            Vector3(const DataType& val) :x(val), y(val), z(val) {}
            Vector3(const DataType& xx, const DataType& yy, const DataType& zz) :x(xx), y(yy), z(zz) {}

            double Length()const
            {
                return std::sqrt(x * x + y * y + z * z);
            }
            static double Length(const Vector3& vec)
            {
                return vec.Length();
            }
            float LengthF()const
            {
                return std::sqrtf(x * x + y * y + z * z);
            }
            static float LengthF(const Vector3& vec)
            {
                return vec.LengthF();
            }

            Vector3& ToNormalize()
            {
                auto len = Length();
                x /= len;
                y /= len;
                z /= len;
                return *this;
            }
            Vector3&&  Normalize()const
            {
                auto len = Length();
                return Vector3(x / len, y / len, z / len);
            }

            static Vector3&& Normalize(const Vector3& vec)
            {
                auto len = vec.Length();
                return Vector3(vec.x / len, vec.y / len, vec.z / len);
            }
            
            Vector3&& Cross(const Vector3& vec)const
            {
                return Vector3((y * vec.z - z * vec.y), (z * vec.x - x * vec.z), (x * vec.y - y * vec.x));
            }
            static Vector3&& Cross(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Cross(vec2);
            }

            double Dot(const Vector3& vec)const
            {
                return (x * vec.x, y * vec.y, z * vec.z);
            }
            static double Dot(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Dot(vec2);
            }
            float DotF(const Vector3& vec)const
            {
                return (x * vec.x, y * vec.y, z * vec.z);
            }
            static float DotF(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.DotF(vec2);
            }

            Vector3&& Add(const Vector3& vec)const
            {
                return Vector3(vec.x + x, vec.y + y, vec.z + z);
            }
            static Vector3&& Add(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Add(vec2);
            }
            Vector3&& Subtract(const Vector3& vec)const
            {
                return Vector3(x - vec.x, y - vec.y, z - vec.z);
            }
            static Vector3&& Subtract(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Subtract(vec2);
            }
            Vector3&& Scale(double sum)const
            {
                return Vector3(x * sum, y * sum, z * sum);
            }

            double Distance(const Vector3& vec)const
            {
                return Subtract(vec).Length();
            }
            double DistanceF(const Vector3& vec)const
            {
                return Subtract(vec).LengthF();
            }
            Vector3&& Project(const Vector3& aim)const
            {
                Vector3&& normalizedV = aim.Normalize();
                float dotProduct = Vector3::Dot(normalizedV, *this);
                return  normalizedV * dotProduct;
            }
            static Vector3&& Project(const Vector3& vec,const Vector3& aim)
            {
                return vec.Project(aim);
            }
            Vector3&& Reflect(const Vector3& normal)const
            {
                float projLen = Vector3::Dot(*this, normal);
                float factor = 2.0f * Vector3::Length(normal);
                return *this - (2.0f * projLen / factor * normal);
            }
            static Vector3&& Reflect(const Vector3& vIn, const Vector3& normal)
            {
                return vIn.Reflect(normal);
            }
            Vector3&& LinearInterpolation(Vector3& vec2, double factor)
            {
                return ((*this) * (1 - factor)) + (vec2 * (factor));
            }
            static Vector3&& LinearInterpolation(Vector3& vec1, Vector3& vec2, double factor)
            {
                return vec1.LinearInterpolation(vec2, factor);
            }
            Vector3&& operator-(const Vector3& vec)const
            {
                return Subtract(vec);
            }
            Vector3&& operator-()const
            {
                return Vector3(-x, -y, -z);
            }
            void operator-=(const Vector3& vec)
            {
                *this = Subtract(vec);
            }
            Vector3&& operator+(const Vector3& vec)const
            {
                return Add(vec);
            }
            void operator+=(const Vector3& vec)
            {
                *this = Add(vec);
            }
            Vector3&& operator*(double sum)const
            {
                return Scale(sum);
            }
            friend Vector3&& operator*(double scalar, const Vector3& vec)
            {
                return vec.Scale(scalar);
            }
            void operator*=(double sum)
            {
                *this = Scale(sum);
            }
            Vector3&& operator/(double sum)const
            {
                return Scale(1 / sum);
            }
            friend Vector3&& operator/(double scalar, const Vector3& vec)
            {
                return vec.Scale(1/scalar);
            }
            void operator/=(double sum)
            {
                *this = Scale(1 / sum);
            }
            bool operator==(const Vector3& vec)
            {
                if (x == vec.x && y == vec.y && z == vec.z)
                {
                    return true;
                }
                return false;
            }
            bool operator!=(const Vector3& vec)
            {
                if (x == vec.x && y == vec.y && z == vec.z)
                {
                    return false;
                }
                return true;
            }

            DataType& operator[](int index)
            {
                if (index > 2)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
            const DataType& operator[](int index)const
            {
                if (index > 2)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
        };
        template<typename DataType>
        //using DataType = float;
        class Vector2
        {
        public:
            union {
                struct { DataType x, y; };
            };
            Vector2() :x(0), y(0) {}
            Vector2(const DataType& val) :x(val), y(val) {}
            Vector2(const DataType& xx, const DataType& yy) :x(xx), y(yy) {}

            double Length()const
            {
                return std::sqrt(x * x + y * y);
            }
            static double Length(const Vector2& vec)
            {
                return vec.Length();
            }
            float LengthF()const
            {
                return std::sqrtf(x * x + y * y);
            }
            static float LengthF(const Vector2& vec)
            {
                return vec.LengthF();
            }

            Vector2& ToNormalize()
            {
                auto len = Length();
                x /= len;
                y /= len;
                return *this;
            }
            Vector2&& Normalize()const
            {
                auto len = Length();
                return Vector2(x / len, y / len);
            }

            static Vector2&& Normalize(const Vector2& vec)
            {
                auto len = vec.Length();
                return Vector2(vec.x / len, vec.y / len);
            }


            double Dot(const Vector2& vec)const
            {
                return (x * vec.x, y * vec.y);
            }
            static double Dot(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Dot(vec2);
            }
            float DotF(const Vector2& vec)const
            {
                return (x * vec.x, y * vec.y);
            }
            static float DotF(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.DotF(vec2);
            }

            Vector2&& Add(const Vector2& vec)const
            {
                return Vector2(vec.x + x, vec.y + y);
            }
            static Vector2&& Add(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Add(vec2);
            }
            Vector2&& Subtract(const Vector2& vec)const
            {
                return Vector2(x - vec.x, y - vec.y);
            }
            static Vector2&& Subtract(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Subtract(vec2);
            }
            Vector2&& Scale(double sum)const
            {
                return Vector2(x * sum, y * sum);
            }

            double Distance(const Vector2& vec)
            {
                return Subtract(vec).Length();
            }
            double DistanceF(const Vector2& vec)
            {
                return Subtract(vec).LengthF();
            }
            Vector2&& Project(const Vector2& aim)
            {
                Vector2&& normalizedV = aim.Normalize();
                double dotProduct = Vector2::Dot(normalizedV, *this);
                return  normalizedV * dotProduct;
            }
            Vector2&& Rotate(double angle)
            {
                double s = std::sin(angle);
                double c = std::cos(angle);
                double newX = x * c - y * s;
                double newY = x * s + y * c;
                return Vector2(newX, newY);
            }
            Vector2&& Reflect(const Vector2& normal)const
            {
                float projLen = Vector2::Dot(*this, normal);
                float factor = 2.0f * Vector2::Length(normal);
                return *this - (2.0f * projLen / factor * normal);
            }
            static Vector2&& Reflect(const Vector2& vIn, const Vector2& normal)
            {
                return vIn.Reflect(normal);
            }
            Vector2&& LinearInterpolation(Vector2& vec2,double factor)
            {
                return ((*this) * (1 - factor)) + (vec2 * (factor));
            }
            static Vector2&& LinearInterpolation(Vector2& vec1, Vector2& vec2, double factor)
            {
                return vec1.LinearInterpolation(vec2, factor);
            }
            Vector2&& operator-(const Vector2& vec)const
            {
                return Subtract(vec);
            }
            Vector2&& operator-()const
            {
                return Vector2(-x, -y);
            }
            void operator-=(const Vector2& vec)
            {
                *this = Subtract(vec);
            }
            Vector2&& operator+(const Vector2& vec)const
            {
                return Add(vec);
            }
            void operator+=(const Vector2& vec)
            {
                *this = Add(vec);
            }
            Vector2&& operator*(double sum)const
            {
                return Scale(sum);
            }
            void operator*=(double sum)
            {
                *this = Scale(sum);
            }
            Vector2&& operator/(double sum)const
            {
                return Scale(1 / sum);
            }
            void operator/=(double sum)
            {
                *this = Scale(1 / sum);
            }
            bool operator==(const Vector2& vec)
            {
                if (x == vec.x && y == vec.y)
                {
                    return true;
                }
                return false;
            }
            bool operator!=(const Vector2& vec)
            {
                if (x == vec.x && y == vec.y)
                {
                    return false;
                }
                return true;
            }
            DataType& operator[](int index)
            {
                if (index > 1)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
            const DataType& operator[](int index)const
            {
                if (index > 1)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
        };
        using Vec3 = Vector3<float>;
        using Vec3d = Vector3<double>;
        using Vec2 = Vector2<float>;
        using Vec2d = Vector2<double>;
    }

}