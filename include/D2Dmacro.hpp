#pragma once
#ifndef OneKeyD2DMacro_HPP
#define OneKeyD2DMacro_HPP


#include<d2d1.h>
#include<cmath>
#include<random>
#include<string>

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



#ifdef UNICODE
    
typedef std::wstring STR;
typedef const std::wstring CSTR;
typedef std::wstring& QSTR;
typedef const std::wstring& CQSTR;

template<typename T>
std::wstring ToStr(T num)
{
    return std::to_wstring(num);
}

#else

typedef std::string STR;
typedef const std::string CSTR;
typedef std::string& QSTR;
typedef const std::string& CQSTR;

template<typename T>
std::string ToStr(T num)
{
    return std::to_string(num);
}

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

        bool ShowConsoleWindow()
        {
            HWND consoleWindow = GetConsoleWindow();
            if (consoleWindow)
                return ShowWindow(consoleWindow, SW_SHOW);
            return false;
        }
        bool HideConsoleWindow()
        {
            HWND consoleWindow = GetConsoleWindow();
            if (consoleWindow)
                return ShowWindow(consoleWindow, SW_HIDE);
            return false;
        }
    };
    //向量
    namespace Vector
    {
        constexpr auto PI = 3.141592653589793;

        template<typename DataType>
        //using DataType = float;
        class Vector3
        {
        public:
            static double AngleToRadian(double angle)
            {
                return angle * PI / 180;
            }
            static double RadianToAngle(double radian)
            {
                return radian * 180 / PI;
            }
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
            Vector3  Normalize()const
            {
                auto len = Length();
                return Vector3(x / len, y / len, z / len);
            }

            static Vector3 Normalize(const Vector3& vec)
            {
                auto len = vec.Length();
                return Vector3(vec.x / len, vec.y / len, vec.z / len);
            }
            
            Vector3 Cross(const Vector3& vec)const
            {
                return Vector3((y * vec.z - z * vec.y), (z * vec.x - x * vec.z), (x * vec.y - y * vec.x));
            }
            static Vector3 Cross(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Cross(vec2);
            }

            double Dot(const Vector3& vec)const
            {
                return (x * vec.x + y * vec.y + z * vec.z);
            }
            static double Dot(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Dot(vec2);
            }
            float DotF(const Vector3& vec)const
            {
                return (x * vec.x + y * vec.y + z * vec.z);
            }
            static float DotF(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.DotF(vec2);
            }

            Vector3 Add(const Vector3& vec)const
            {
                return Vector3(vec.x + x, vec.y + y, vec.z + z);
            }
            static Vector3 Add(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Add(vec2);
            }
            Vector3 Subtract(const Vector3& vec)const
            {
                return Vector3(x - vec.x, y - vec.y, z - vec.z);
            }
            static Vector3 Subtract(const Vector3& vec1, const Vector3& vec2)
            {
                return vec1.Subtract(vec2);
            }
            Vector3 Scale(double sum)const
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
            Vector3 Project(const Vector3& aim)const
            {
                Vector3&& normalizedV = aim.Normalize();
                float dotProduct = Vector3::Dot(normalizedV, *this);
                return  normalizedV * dotProduct;
            }
            static Vector3 Project(const Vector3& vec,const Vector3& aim)
            {
                return vec.Project(aim);
            }
            Vector3 Reflect(const Vector3& normal)const
            {
                float projLen = Vector3::Dot(*this, normal);
                float factor = 2.0f * Vector3::Length(normal);
                return *this - (2.0f * projLen / factor * normal);
            }
            static Vector3 Reflect(const Vector3& vIn, const Vector3& normal)
            {
                return vIn.Reflect(normal);
            }
            Vector3 LinearInterpolation(const Vector3& vec2, double factor)const
            {
                return ((*this) * (1 - factor)) + (vec2 * (factor));
            }
            static Vector3 LinearInterpolation(const Vector3& vec1, const Vector3& vec2, double factor)
            {
                return vec1.LinearInterpolation(vec2, factor);
            }
            Vector3 operator-(const Vector3& vec)const
            {
                return Subtract(vec);
            }
            Vector3 operator-()const
            {
                return Vector3(-x, -y, -z);
            }
            void operator-=(const Vector3& vec)
            {
                x -= vec.x;
                y -= vec.y;
                z -= vec.z;
            }
            Vector3 operator+(const Vector3& vec)const
            {
                return Add(vec);
            }
            void operator+=(const Vector3& vec)
            {
                x += vec.x;
                y += vec.y;
                z += vec.z;
            }
            Vector3 operator*(double sum)const
            {
                return Scale(sum);
            }
            friend Vector3 operator*(double scalar, const Vector3& vec)
            {
                return vec.Scale(scalar);
            }
            void operator*=(double sum)
            {
                x *= sum;
                y *= sum;
                z *= sum;
            }
            Vector3 operator/(double sum)const
            {
                return Scale(1 / sum);
            }
            void operator/=(double sum)
            {
                x /= sum;
                y /= sum;
                z /= sum;
            }
            bool operator==(const Vector3& vec)const
            {
                if (x == vec.x && y == vec.y && z == vec.z)
                {
                    return true;
                }
                return false;
            }
            bool operator!=(const Vector3& vec)const
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
            static double AngleToRadian(double angle)
            {
                return angle * PI / 180;
            }
            static double RadianToAngle(double radian)
            {
                return radian * 180 / PI;
            }
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
            Vector2 Normalize()const
            {
                auto len = Length();
                return Vector2(x / len, y / len);
            }

            static Vector2 Normalize(const Vector2& vec)
            {
                auto len = vec.Length();
                return Vector2(vec.x / len, vec.y / len);
            }

            double GetAngle(const Vector2& vec)const
            {
                double dotProduct = Dot(vec);
                double&& lengthVec1 = Length();
                double&& lengthVec2 = vec.Length();
                double&& cosAngle = dotProduct / (lengthVec1 * lengthVec2);

                 return std::acos(cosAngle) * 180.0 / PI;
            }

            double Dot(const Vector2& vec)const
            {
                return (x * vec.x + y * vec.y);
            }
            static double Dot(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Dot(vec2);
            }
            float DotF(const Vector2& vec)const
            {
                return (x * vec.x + y * vec.y);
            }
            static float DotF(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.DotF(vec2);
            }

            Vector2 Add(const Vector2& vec)const
            {
                return Vector2(vec.x + x, vec.y + y);
            }
            static Vector2 Add(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Add(vec2);
            }
            Vector2 Subtract(const Vector2& vec)const
            {
                return Vector2(x - vec.x, y - vec.y);
            }
            static Vector2 Subtract(const Vector2& vec1, const Vector2& vec2)
            {
                return vec1.Subtract(vec2);
            }
            Vector2 Scale(double sum)const
            {
                return Vector2(x * sum, y * sum);
            }

            double Distance(const Vector2& vec)const
            {
                return Subtract(vec).Length();
            }
            double DistanceF(const Vector2& vec)const
            {
                return Subtract(vec).LengthF();
            }
            Vector2 Project(const Vector2& aim)const
            {
                Vector2&& normalizedV = aim.Normalize();
                double dotProduct = Vector2::Dot(normalizedV, *this);
                return  normalizedV * dotProduct;
            }
            /// <summary>
            /// 弧度
            /// </summary>
            /// <param name="radian"></param>
            /// <returns></returns>
            Vector2 Rotate(double radian)const
            {
                double s = std::sin(radian);
                double c = std::cos(radian);
                double newX = x * c - y * s;
                double newY = x * s + y * c;
                return Vector2(newX, newY);
            }
            Vector2 Reflect(const Vector2& normal)const
            {
                float projLen = Vector2::Dot(*this, normal);
                float factor = 2.0f * Vector2::Length(normal);
                return *this - (2.0f * projLen / factor * normal);
            }
            static Vector2 Reflect(const Vector2& vIn, const Vector2& normal)
            {
                return vIn.Reflect(normal);
            }
            Vector2 LinearInterpolation(const Vector2& vec2,double factor)
            {
                return ((*this) * (1 - factor)) + (vec2 * (factor));
            }
            static Vector2 LinearInterpolation(const Vector2& vec1, const Vector2& vec2, double factor)
            {
                return vec1.LinearInterpolation(vec2, factor);
            }
            Vector2 operator-(const Vector2& vec)const
            {
                return Subtract(vec);
            }
            Vector2 operator-()const
            {
                return Vector2(-x, -y);
            }
            void operator-=(const Vector2& vec)
            {
                x -= vec.x;
                y -= vec.y;
            }
            Vector2 operator+(const Vector2& vec)const
            {
                return Add(vec);
            }
            void operator+=(const Vector2& vec)
            {
                x += vec.x;
                y += vec.y;
            }
            Vector2 operator*(double sum)const
            {
                return Scale(sum);
            }
            Vector2 operator*(const Vector2& vec)const
            {
                return Vector2(x * vec.x, y * vec.y);
            }
            void operator*=(double sum)
            {
                x *= sum;
                y *= sum;
            }
            Vector2 operator/(double sum)const
            {
                return Scale(1 / sum);
            }
            void operator/=(double sum)
            {
                x /= sum;
                y /= sum;
            }
            bool operator==(const Vector2& vec)const
            {
                if (x == vec.x && y == vec.y)
                {
                    return true;
                }
                return false;
            }
            bool operator!=(const Vector2& vec)const
            {
                if (x == vec.x && y == vec.y)
                {
                    return false;
                }
                return true;
            }
            /// <summary>
            /// 用于在map中生成有序的组合
            /// 按照x为高位，y为低位比较
            /// </summary>
            /// <param name="vec"></param>
            /// <returns></returns>
            bool operator<(const Vector2& vec)const
            {
                if (x < vec.x)
                {
                    return true;
                }
                else if (x == vec.x)
                {
                    if (y < vec.y)
                        return true;
                }
                return false;
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

            D2D1_POINT_2F ToPointD2D()const
            {
                return D2D1::Point2F(x, y);
            }
            D2D1_SIZE_F ToSizeD2D()const
            {
                return D2D1::SizeF(x, y);
            }
        };

        template<typename DataType>
        //using DataType = float;
        class Vector4
        {
        public:
            static double AngleToRadian(double angle)
            {
                return angle * PI / 180;
            }
            static double RadianToAngle(double radian)
            {
                return radian * 180 / PI;
            }
            union {
                struct { DataType x, y, z, w; };
                struct { DataType r, g, b, a; };
            };
            Vector4() :x(0), y(0), z(0), w(0) {}
            Vector4(const DataType& val) :x(val), y(val), z(val), w(val) {}
            Vector4(const DataType& xx, const DataType& yy, const DataType& zz, const DataType& ww) :x(xx), y(yy), z(zz), w(ww) {}

            double Length()const
            {
                return std::sqrt(x * x + y * y + z * z + w * w);
            }
            static double Length(const Vector4& vec)
            {
                return vec.Length();
            }
            float LengthF()const
            {
                return std::sqrtf(x * x + y * y + z * z + w * w);
            }
            static float LengthF(const Vector4& vec)
            {
                return vec.LengthF();
            }

            Vector4& ToNormalize()
            {
                auto len = Length();
                x /= len;
                y /= len;
                z /= len;
                w /= len;
                return *this;
            }
            Vector4 Normalize()const
            {
                auto len = Length();
                return Vector4(x / len, y / len, z / len, w / len);
            }

            static Vector4 Normalize(const Vector4& vec)
            {
                auto len = vec.Length();
                return Vector4(vec.x / len, vec.y / len, vec.z / len, vec.w / len);
            }

            double Dot(const Vector4& vec)const
            {
                return (x * vec.x + y * vec.y + z * vec.z + w * vec.w);
            }
            static double Dot(const Vector4& vec1, const Vector4& vec2)
            {
                return vec1.Dot(vec2);
            }
            float DotF(const Vector4& vec)const
            {
                return (x * vec.x + y * vec.y + z * vec.z + w * vec.w);
            }
            static float DotF(const Vector4& vec1, const Vector4& vec2)
            {
                return vec1.DotF(vec2);
            }

            Vector4 Add(const Vector4& vec)const
            {
                return Vector4(vec.x + x, vec.y + y, vec.z + z, w + vec.w);
            }
            static Vector4 Add(const Vector4& vec1, const Vector4& vec2)
            {
                return vec1.Add(vec2);
            }
            Vector4 Subtract(const Vector4& vec)const
            {
                return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
            }
            static Vector4 Subtract(const Vector4& vec1, const Vector4& vec2)
            {
                return vec1.Subtract(vec2);
            }
            Vector4 Scale(double sum)const
            {
                return Vector4(x * sum, y * sum, z * sum, w * sum);
            }

            double Distance(const Vector4& vec)const
            {
                return Subtract(vec).Length();
            }
            double DistanceF(const Vector4& vec)const
            {
                return Subtract(vec).LengthF();
            }
            Vector4 Project(const Vector4& aim)const
            {
                Vector4&& normalizedV = aim.Normalize();
                float dotProduct = Vector4::Dot(normalizedV, *this);
                return  normalizedV * dotProduct;
            }
            static Vector4 Project(const Vector4& vec, const Vector4& aim)
            {
                return vec.Project(aim);
            }
            Vector4 Reflect(const Vector4& normal)const
            {
                float projLen = Vector4::Dot(*this, normal);
                float factor = 2.0f * Vector4::Length(normal);
                return *this - (2.0f * projLen / factor * normal);
            }
            static Vector4 Reflect(const Vector4& vIn, const Vector4& normal)
            {
                return vIn.Reflect(normal);
            }
            Vector4 LinearInterpolation(const Vector4& vec2, double factor)const
            {
                return ((*this) * (1 - factor)) + (vec2 * (factor));
            }
            static Vector4 LinearInterpolation(const Vector4& vec1, const Vector4& vec2, double factor)
            {
                return vec1.LinearInterpolation(vec2, factor);
            }
            Vector4 operator-(const Vector4& vec)const
            {
                return Subtract(vec);
            }
            Vector4 operator-()const
            {
                return Vector4(-x, -y, -z, -w);
            }
            void operator-=(const Vector4& vec)
            {
                x -= vec.x;
                y -= vec.y;
                z -= vec.z;
                w -= vec.w;
            }
            Vector4 operator+(const Vector4& vec)const
            {
                return Add(vec);
            }
            void operator+=(const Vector4& vec)
            {
                x += vec.x;
                y += vec.y;
                z += vec.z;
                w += vec.w;
            }
            Vector4 operator*(double sum)const
            {
                return Scale(sum);
            }
            friend Vector4 operator*(double scalar, const Vector4& vec)
            {
                return vec.Scale(scalar);
            }
            void operator*=(double sum)
            {
                x *= sum;
                y *= sum;
                z *= sum;
                w *= sum;
            }
            Vector4 operator/(double sum)const
            {
                return Scale(1 / sum);
            }
            void operator/=(double sum)
            {
                x /= sum;
                y /= sum;
                z /= sum;
                w /= sum;
            }
            bool operator==(const Vector4& vec)const
            {
                if (x == vec.x && y == vec.y && z == vec.z && w == vec.w)
                {
                    return true;
                }
                return false;
            }
            bool operator!=(const Vector4& vec)const
            {
                if (x == vec.x && y == vec.y && z == vec.z && w == vec.w)
                {
                    return false;
                }
                return true;
            }

            DataType& operator[](int index)
            {
                if (index > 3)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
            const DataType& operator[](int index)const
            {
                if (index > 3)
                    throw std::runtime_error("索引超出安全范围");
                return (&x)[index];
            }
        };


        using Vec3 = Vector3<float>;
        using Vec3d = Vector3<double>;
        using Vec3i = Vector3<int>;
        using Vec3l = Vector3<long long>;

        using Vec2 = Vector2<float>;
        using Vec2d = Vector2<double>;
        using Vec2i = Vector2<int>;
        using Vec2l = Vector2<long long>;

        using Vec4 = Vector4<float>;
        using Vec4d = Vector4<double>;
        using Vec4i = Vector4<int>;
        using Vec4l = Vector4<long long>;


    }
    namespace Random
    {
        using InterpolationFunction1 = float(*)(float start, float end, float factor);
        class Interpolation
        {
        public:
            static float LineInterpolation(float start, float end, float factor)
            {
                return start + (end - start) * factor;
            }
            static float Smoothstep(float start, float end, float factor)
            {

                float t = factor * factor;
                return start + (end - start) * t * (3.0f - 2.0f * t);
            }
        };
        
        class PerlinNoise1
        {
            int m_min, m_max, m_count;
            std::vector<float>m_P;
            InterpolationFunction1 m_InF;
        public:
            PerlinNoise1() :m_min(0), m_max(0), m_count(0),m_InF(Interpolation::Smoothstep) {}
            PerlinNoise1(int randomSeed, int minR, int maxR, int randomConut)
            {
                Init(randomSeed, randomConut, minR, maxR);
            }
            void SetInterpolationFunction(InterpolationFunction1 function)
            {
                m_InF = function;
            }
            void Init(int randomSeed, int randomConut,int Min,int Max)
            {
                m_max = Max;
                m_min = Min;
                m_count = randomConut;

                std::default_random_engine engine(randomSeed);
                std::uniform_real_distribution<float> dist(0, 1);
                m_P.resize(randomConut + 1);
                for (auto& i : m_P)
                {
                    i = dist(engine);
                }
            }
            float GetRandom(float pos)
            {
                if (m_P.empty())
                    return -1;
                if (pos<m_min || pos>m_max)
                    return -1;
                float interval = (m_max - m_min) / (float)m_count;
                int left = (int)((pos - m_min) / interval);
                if (left >= m_count)
                    return m_P.back();
                float leftR = m_P[left];
                float rightR = m_P[left + 1];
                return m_InF(leftR, rightR, (pos - left * interval) / interval);
            }
        };
        class PerlinNoise2
        {
            Vector::Vec2i m_min, m_max, m_count;
            std::vector<std::vector<float>>m_P;
            InterpolationFunction1 m_InF;
        public:
            PerlinNoise2() :m_min(0), m_max(0), m_count(0),m_InF(Interpolation::Smoothstep) {}
            PerlinNoise2(int randomSeed, const Vector::Vec2i& minR, const Vector::Vec2i& maxR, const Vector::Vec2i& randomConut) :m_min(minR), m_max(maxR)
            {
                Init(randomSeed, randomConut, minR, maxR);
            }
            void SetInterpolationFunction(InterpolationFunction1 function)
            {
                m_InF = function;
            }
            void Init(int randomSeed, const Vector::Vec2i& randomConut, const Vector::Vec2i& minR, const Vector::Vec2i& maxR)
            {
                m_max = maxR;
                m_min = minR;
                m_count = randomConut;

                std::default_random_engine engine(randomSeed);
                std::uniform_real_distribution<float> dist(0, 1);

                m_P.resize(randomConut.x + 1);
                for (auto& i : m_P)
                {
                    i.resize(randomConut.y + 1);
                    for (auto& j : i)
                    {
                        j = dist(engine);
                    }
                }
            }
            float GetRandom(const Vector::Vec2& pos)
            {
                if (m_P.empty())
                    return -1;
                if (pos.x<m_min.x || pos.x>m_max.x)
                    return -1;
                if (pos.y<m_min.y || pos.y>m_max.y)
                    return -1;
                Vector::Vec2 interval;
                interval.x = (m_max.x - m_min.x) / (float)m_count.x;
                interval.y = (m_max.y - m_min.y) / (float)m_count.y;
                Vector::Vec2i left;
                left.x = (int)((pos.x - m_min.x) / interval.x);
                left.y = (int)((pos.y - m_min.y) / interval.y);
                if (left.x >= m_count.x)
                    return m_P.back().back();

                float leftBottom = m_P[left.x][left.y];
                float leftTop = m_P[left.x][left.y + 1];
                float rightBottom = m_P[left.x + 1][left.y];
                float rightTop = m_P[left.x + 1][left.y + 1];

                Vector::Vec2 posLB = Vector::Vec2(left.x * interval.x, left.y * interval.y);
                Vector::Vec2 posRT = Vector::Vec2((left.x + 1) * interval.x, (left.y + 1) * interval.y);

                float xiBottom = m_InF(leftBottom, rightBottom, (pos.x - left.x * interval.x) / interval.x);
                float xiTop = m_InF(leftTop, rightTop, (pos.x - left.x * interval.x) / interval.x);

                return m_InF(xiBottom,xiTop, (pos.y - left.y * interval.y) / interval.y);
            }
        };
    }
    /// <summary>
    /// 将连续的内存转换为指定类型数据
    /// 返回转换后的结束位置指针
    /// </summary>
    /// <typeparam name="Type"></typeparam>
    /// <param name="data">数据位置</param>
    /// <param name="aim">输出</param>
    /// <returns></returns>
    template<typename Type>
    char* ToType(char* data,Type* aim)
    {
        *aim = *((Type*)data);
        return data + sizeof(*aim);
    }
}
#endif // !OneKeyD2DMacro_HPP