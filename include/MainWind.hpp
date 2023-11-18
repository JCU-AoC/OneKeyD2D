#pragma once

#include<d2d1.h>
#include<windows.h>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<thread>
#include<dwrite.h>
#include"D2Dmacro.hpp"

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Dwrite.lib")
//0.12 Sound
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")


#define MAINWIND_HPP
namespace Game 
{
	class MainWind;
	class MainWind_GDI;
	class MainWind_D2D;



	namespace WindClassName {
		constexpr auto MAIN_WINDOW_GDI_CLASS_NAME = L"MainWindow_Game";
		constexpr auto MAIN_WINDOW_D2D_CLASS_NAME = L"d2dMainWindow_Game";
	}
	namespace WindCount
	{
		static int g_gdiWindCount = 0;
		static int g_d2dMainWindCount = 0;
	}
	namespace WindCallback
	{
		typedef void(*CharInputCallback_GDI)(MainWind_GDI*, int KeyInputChar, int Frequency, KeyMode);
		typedef void(*PaintCallback_GDI)(MainWind_GDI*);
		typedef void(*SizeCallback_GDI)(MainWind_GDI*, int width, int height);
		typedef void(*CharInputCallback_D2D)(MainWind_D2D* window, int KeyInputChar, int Frequency, KeyMode mode);
		typedef void(*PaintCallback_D2D)(MainWind_D2D*);
		typedef void(*SizeCallback_D2D)(MainWind_D2D*, int width, int height);

		typedef void(*ButtonCallback)(MainWind*,LONG64);
		typedef void(*MouseCallback)(MainWind*, int x, int y, int VirtualKey, MouseMessageType, KeyMode);
		using EditControlCallback = void(*)(MainWind*, const std::wstring&, EditMessage);
	}
	
	namespace WindElements
	{
		class d2dClickDetection
		{
		private:
			float m_X,m_Y;
			float m_RightX, m_RightY;
			LONG64 m_UserData;
			WindCallback::ButtonCallback m_Callback;
			bool m_Negation, m_Elliptic, m_Border;
		public:
			d2dClickDetection(float x = 0, float y = 0, float w = 0, float h = 0) :
				m_RightX(x + w), m_RightY(h + y), m_UserData(0),
				m_Negation(false), m_Callback(nullptr), m_Elliptic(false), m_Border(false)
			{
				m_X = x;
				m_Y = y;
			}
			void Negation(bool negation = true)
			{
				m_Negation = negation;
			}
			void Elliptic(bool elliptic = true)
			{
				m_Elliptic = elliptic;
			}
			void SetUserData(LONG64 data)
			{
				m_UserData = data;
			}
			LONG64& GetUserData()
			{
				return m_UserData;
			}
			const LONG64& GetUserData()const
			{
				return m_UserData;
			}
			void SetRectangle(const D2D_RECT_F& checkRect)
			{
				m_X = checkRect.left;
				m_Y = checkRect.top;
				m_RightX = checkRect.right;
				m_RightY = checkRect.bottom;
			}
			D2D_RECT_F GetRectangle()const
			{
				return { m_X,m_Y,m_RightX,m_RightY };
			}
			void SetPosition(float x, float y)
			{
				float wDifference = x - m_X;
				float hDifference = y - m_Y;
				m_X += wDifference;
				m_Y += hDifference;
				m_RightX += wDifference;
				m_RightY += hDifference;
			}
			void SetWide(float width, float height)
			{

				m_RightY = height + m_Y;
				m_RightX = width + m_X;
			}
			bool CheckClick(float x, float y)const
			{
				if (m_Border)
				{
					if (x <= m_X || y <= m_Y || x >= m_RightX || y >= m_RightY)
					{
						if (m_Negation)
							return true;
						return false;
					}
					if (m_Elliptic)
					{
						float halfWidth = (m_RightX - m_X) * 0.5;
						float halfHeight = (m_RightY - m_Y) * 0.5;
						float normalizedX = x - m_X - halfWidth;
						float normalizedY = y - m_Y - halfHeight;
						bool insideEllipse = (normalizedX * normalizedX) / (halfWidth * halfWidth) + (normalizedY * normalizedY) / (halfHeight * halfHeight) <= 1;
						if (m_Negation) {
							return !insideEllipse;
						}
						else {
							return insideEllipse;
						}
					}
					if (m_Negation)
						return false;
					return true;
				}
				else
				{
					if (x<m_X || y<m_Y || x>m_RightX || y> m_RightY)
					{
						if (m_Negation)
							return true;
						return false;
					}
					if (m_Elliptic)
					{
						float halfWidth = (m_RightX - m_X) * 0.5;
						float halfHeight = (m_RightY - m_Y) * 0.5;
						float normalizedX = x - m_X - halfWidth;
						float normalizedY = y - m_Y - halfHeight;
						bool insideEllipse = (normalizedX * normalizedX) / (halfWidth * halfWidth) + (normalizedY * normalizedY) / (halfHeight * halfHeight) < 1;
						if (m_Negation) {
							return !insideEllipse;
						}
						else {
							return insideEllipse;
						}
					}
					if (m_Negation)
						return false;
					return true;
				}
			}
			void SetButtonCallback(WindCallback::ButtonCallback bc)
			{
				m_Callback = bc;
			}
			void RunCallback(MainWind* mainWind)
			{
				m_Callback(mainWind, m_UserData);
			}
			friend class MainWind;
		};
	}



	///////////////////以下是窗口类///////////////////



	class MainWind
	{
	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		LPARAM m_UserData;
		LONG_PTR m_SystemData;

		WindCallback::MouseCallback m_MouseCallback;
		std::set<WindElements::d2dClickDetection*>m_Buttons;
		 
		bool m_ButtonChange;

		virtual void OnPaint() = 0;
		virtual void OnSize(int width,int height) = 0;
		void OnMouse(UINT msg, int x, int y, int VirtualKey)
		{
			if (msg == WM_LBUTTONDOWN)
			{
				for (auto& bt : m_Buttons)
				{
					if (bt->CheckClick((float)x, (float)y))
						bt->RunCallback(this);
					if (m_ButtonChange)
						break;
				}
				m_ButtonChange = false;
			}
			if (!m_MouseCallback)
				return;
			switch (msg)
			{
			case WM_MOUSEMOVE:
				m_MouseCallback(this, x, y, VirtualKey, MT_MOVE, KM_CHAR);
				break;
			case WM_LBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_LEFT, KM_DOWN);
				break;
			case WM_LBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEFT, KM_DOWN);
				break;
			case WM_LBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEFT, KM_UP);
				break;
			case WM_RBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_RIGHT, KM_DOWN);
				break;
			case WM_RBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_RIGHT, KM_DOWN);
				break;
			case WM_RBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_RIGHT, KM_UP);
				break;
			case WM_MBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_MID, KM_DOWN);
				break;
			case WM_MBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_MID, KM_DOWN);
				break;
			case WM_MBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_MID, KM_UP);
				break;
			case WM_MOUSEWHEEL:
				if (GET_WHEEL_DELTA_WPARAM(VirtualKey) > 0)
					m_MouseCallback(this, x, y, VirtualKey, MT_WHEEL, KM_UP);
				else
					m_MouseCallback(this, x, y, VirtualKey, MT_WHEEL, KM_DOWN);
				break;
			case WM_MOUSEACTIVATE:
				m_MouseCallback(this, x, y, VirtualKey, MT_ENTER_WINDOW, KM_CHAR);
				break;
			case WM_MOUSELEAVE:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEAVE_WINDOW, KM_CHAR);
				break;
			default:
				break;
			}
		}

		static LONG_PTR GetWindLongPtr(HWND hWnd)
		{
			return (
				::GetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA
				));
		}
	public:

		MainWind() : m_hWnd(nullptr), m_hInstance(nullptr), m_UserData(0),m_MouseCallback(nullptr),m_ButtonChange(false),m_SystemData(0) {}
		virtual ~MainWind(){}
		virtual HRESULT CreateWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT) = 0;

		/// <summary>
		/// 由系统调用，不开放使用
		/// 有需要使用UserData
		/// </summary>
		/// <returns></returns>
		LONG_PTR& GetSystemData()
		{
			return m_SystemData;
		}

		void SetMouseCallback(WindCallback::MouseCallback mcb)
		{
			m_MouseCallback = mcb;
		}
		const WindCallback::MouseCallback& GetMouseCallback()const
		{
			return m_MouseCallback;
		}
		/// <summary>
		/// 设置默认画笔,当宽度大于1时，样式无效
		/// </summary>
		/// <param name="color"></param>
		/// <param name="penStyle"></param>
		/// <param name="LineWidth"></param>
		/// <returns></returns>
		virtual bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1) = 0;
		virtual bool ClearWindBackground(COLORREF color = RGB(255, 255, 255)) = 0;
		virtual bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true) = 0;
		/// <summary>
		/// 绘制矩形，颜色由画笔决定,仅在绘制过程函数调用
		/// </summary>
		/// <param name="left">左上角x</param>
		/// <param name="top">左上角y</param>
		/// <param name="right">右下角x</param>
		/// <param name="bottom">右下角y</param>
		/// <returns></returns>
		virtual bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom) = 0;
		virtual bool DrawFillRectangle(
			int left,
			int top,
			int right,
			int bottom, COLORREF fillColor) = 0;
		/// <summary>
		/// 画线,颜色为画笔颜色
		/// </summary>
		/// <param name="startX"></param>
		/// <param name="startY"></param>
		/// <param name="endX"></param>
		/// <param name="endY"></param>
		/// <returns></returns>
		virtual bool DrawLine(int startX, int startY, int endX, int endY) = 0;
		/// <summary>
		/// 画线,颜色为画笔颜色
		/// 从上一次结束的地方开始，默认为（0，0）开始
		/// </summary>
		/// <param name="toX"></param>
		/// <param name="toY"></param>
		/// <returns></returns>
		virtual bool DrawLineTo(int toX, int toY) = 0;
		//绘制字符串
		virtual bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h) = 0;
		virtual bool DrawTextWide(const std::wstring& ShowText, int x, int y, int w, int h) = 0;
		//绘制多边形
		virtual bool DrawGeometry(POINT* pointArr,int pointCount)=0;
		//绘制多边形
		virtual bool DrawGeometry(std::vector<POINT>&points)=0;
		//绘制填充多边形
		virtual bool DrawFillGeometry(POINT* pointArr, int pointCount, COLORREF fillColor) = 0;
		//绘制填充多边形
		virtual bool DrawFillGeometry(std::vector<POINT>& points, COLORREF fillColor) = 0;

		/// <summary>
		/// 获取窗口句柄（如果对win32编程不熟悉不建议直接使用句柄）
		/// </summary>
		/// <returns></returns>
		HWND GethWnd()const
		{
			return m_hWnd;
		}
		void CloseWind()
		{
			if (m_hWnd)
				DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}
		//启动消息循环，直到窗口关闭
		static void StaticWindowMessageLoop()
		{
			MSG msg = { };
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		/// <summary>
		/// 强制重绘窗口
		/// </summary>
		void PaintWind()
		{
			if (m_hWnd)
				InvalidateRect(m_hWnd, NULL, false);
		}

		SIZE GetWindSize()const
		{
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			return { rect.right,rect.bottom };
		}
		RECT GetWindRect()const
		{
			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			return rect;
		}
		void MoveWind(int x, int y, int w, int h)const
		{
			MoveWindow(m_hWnd, x, y, w, h, true);
		}
		void SetText(const std::wstring& text)const
		{
			SetWindowTextW(m_hWnd, text.c_str());
		}
		void SetText(const std::string& text)const
		{
			SetWindowTextA(m_hWnd, text.c_str());
		}
		void ShowWind()const
		{
			ShowWindow(m_hWnd, SW_SHOW);
		}
		void HideWind()const
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
		void AddButten(WindElements::d2dClickDetection* db)
		{
			m_Buttons.insert(db);
			m_ButtonChange = true;
		}
		/// <summary>
		/// 清除指定检测区块
		/// 若为空则清空所有
		/// </summary>
		/// <param name="db">指定区块指针</param>
		void DeleteButten(WindElements::d2dClickDetection* db = nullptr)
		{
			if (db)
				m_Buttons.erase(db);
			else
				m_Buttons.clear();
			m_ButtonChange = true;
		}
		void SetUserData(LPARAM data)
		{
			m_UserData = data;
		}
		const LPARAM& GetUserData()const
		{
			return m_UserData;
		}
		LPARAM& GetUserData()
		{
			return m_UserData;
		}
		/// <summary>
		/// 固定窗口大小
		/// </summary>
		/// <param name="width">目标宽度，小于0时为当前大小</param>
		/// <param name="height">目标高度，小于0时为当前大小</param>
		void FixedWindowSize(int width = -1, int height = -1)
		{
			DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
			dwStyle &= ~WS_THICKFRAME; // 移除窗口大小调整边框
			dwStyle &= ~WS_MAXIMIZEBOX; // 移除窗口的最大化按钮
			SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
			auto rect = GetWindRect();
			if (width < 0)
			{
				width = rect.right - rect.left;
			}
			if (height < 0)
			{
				height = rect.bottom - rect.top;
			}
			MoveWind( rect.left, rect.top, width, height);
		}

	};

	class MainWind_GDI :public MainWind
	{
	private:
		HBRUSH m_BackgroundColor;
		HDC m_hdc;
		RECT m_WindWide;

		HPEN m_CurrentPen;
		HPEN m_OldPen;
		WindCallback::CharInputCallback_GDI m_CharCallback;
		WindCallback::PaintCallback_GDI m_PaintCallback;
		WindCallback::SizeCallback_GDI m_WindSizeCallback;

		void OnPaint()override
		{
			PAINTSTRUCT ps;
			m_hdc = BeginPaint(m_hWnd, &ps);
			SetBkMode(m_hdc, TRANSPARENT);
			
			if (m_CurrentPen)
				m_OldPen = (HPEN)SelectObject(m_hdc, m_CurrentPen);
			if (m_BackgroundColor)
			{
				SelectObject(m_hdc, m_BackgroundColor);
				FillRect(m_hdc, &m_WindWide, m_BackgroundColor);
			}
			if (m_PaintCallback)
				m_PaintCallback(this);
			if (m_CurrentPen)
				SelectObject(m_hdc, m_OldPen);
			EndPaint(m_hWnd, &ps);
			m_hdc = nullptr;
		}
		void OnSize(int width, int height)override
		{
			m_WindWide.right = width;
			m_WindWide.bottom = height;
			if (m_WindSizeCallback)
				m_WindSizeCallback(this, width, height);
		}
		static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_CREATE)
			{
				LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
				MainWind_GDI* mainWind = (MainWind_GDI*)pcs->lpCreateParams;

				::SetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(mainWind)
				);
				return 1;
			}
			MainWind_GDI* mainWind = reinterpret_cast<MainWind_GDI*>(GetWindLongPtr(hWnd));
			if (!mainWind)
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST||uMsg==WM_MOUSEACTIVATE||uMsg== WM_MOUSELEAVE)
			{
				mainWind->OnMouse(uMsg, LOWORD(lParam), HIWORD(lParam), (int)wParam);
				return 0;
			}
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			{
				if (mainWind->m_CharCallback)
				{
					switch (uMsg)
					{
					case WM_CHAR:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_CHAR);
						break;
					case WM_KEYDOWN:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_DOWN);
						break;
					case WM_KEYUP:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_UP);
						break;
					default:
						break;
					}
				}
				break;
			}
			case WM_PAINT:
			{
				mainWind->OnPaint();
				break;
			}
			case WM_SIZE:
			{
				mainWind->OnSize(LOWORD(lParam), HIWORD(lParam));
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
				break;
			}
			return 0;
		}
	public:
		MainWind_GDI() :m_hdc(nullptr), m_BackgroundColor(nullptr), m_CurrentPen(nullptr), m_OldPen(nullptr),
			m_CharCallback(nullptr), m_PaintCallback(nullptr), m_WindSizeCallback(nullptr)
		{ 
			WindCount::g_gdiWindCount++;
			m_WindWide = { 0,0,0,0 };
		}
		~MainWind_GDI()
		{
			SafeReleaseW(&m_CurrentPen);
			if (m_BackgroundColor)
				DeleteObject(m_BackgroundColor);
			WindCount::g_gdiWindCount--;
			CloseWind();
			if (WindCount::g_gdiWindCount == 0)
			{
				UnregisterClassW(WindClassName::MAIN_WINDOW_GDI_CLASS_NAME, m_hInstance);
			}
		}
		HRESULT CreateWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT)override
		{
			if (m_hWnd)
			{
				std::cerr << "请使用新的实例创建窗口" << std::endl;
				return S_FALSE;
			}
			m_hInstance = hInstance;

			WNDCLASSW windowClass = {};
			if (GetClassInfoW(hInstance, WindClassName::MAIN_WINDOW_GDI_CLASS_NAME, &windowClass) == 0)
			{
				WNDCLASSEXW wc = { 0 };
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = MainWindowProc;
				wc.hInstance = hInstance;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.lpszClassName = WindClassName::MAIN_WINDOW_GDI_CLASS_NAME;

				// 注册窗口类
				if (!RegisterClassExW(&wc)) {
					GetLastWindowError error;
					std::string errorOutput = "窗口类注册失败!";
					errorOutput += error.GetErrorMessage();
					MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
					return S_FALSE;
				}
			}

			if (parent)
			{
				RECT parentRect;
				GetClientRect(parent, &parentRect);
				m_hWnd = CreateWindowW(
					WindClassName::MAIN_WINDOW_GDI_CLASS_NAME,
					(L"gdi窗口"),
					WS_CHILD | WS_VISIBLE,
					0, 0, parentRect.right, parentRect.bottom,
					parent, NULL,
					hInstance, this);
			}
			else
			{
				m_hWnd = CreateWindowW(
					WindClassName::MAIN_WINDOW_GDI_CLASS_NAME,
					(L"gdi窗口"),
					WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					100, 100, 800, 600,
					NULL, NULL,
					hInstance, this);
			}
			if (!m_hWnd)
			{
				GetLastWindowError error;
				std::string errorOutput = "窗口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return S_FALSE;
			}
			GetClientRect(m_hWnd, &m_WindWide);
			return S_OK;
		}


		/// <summary>
		/// 设置字符消息的处理函数
		/// </summary>
		/// <param name="charCallback"></param>
		void SetKeyCallback(WindCallback::CharInputCallback_GDI charCallback)
		{
			m_CharCallback = charCallback;
		}
		/// <summary>
		/// 设置窗口绘制过程函数
		/// </summary>
		/// <param name="paintCallback"></param>
		void SetPaintCallback(WindCallback::PaintCallback_GDI paintCallback)
		{
			m_PaintCallback = paintCallback;
		}
		/// <summary>
		/// 窗口大小改变时调用
		/// </summary>
		/// <param name="sizeCallback"></param>
		void SetWindSizeCallback(WindCallback::SizeCallback_GDI sizeCallback)
		{
			m_WindSizeCallback = sizeCallback;
		}
		const WindCallback::CharInputCallback_GDI& GetKeyCallback()const
		{
			return m_CharCallback;
		}
		const WindCallback::PaintCallback_GDI& GetPaintCallback()const
		{
			return m_PaintCallback;
		}
		const WindCallback::SizeCallback_GDI& GetWindSizeCallback()const
		{
			return m_WindSizeCallback;
		}

		bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom)override
		{
			if (m_hdc)
				return Rectangle(m_hdc, left, top, right, bottom);
			return false;
		}
		bool DrawFillRectangle(
			int left,
			int top,
			int right,
			int bottom, COLORREF fillColor)override
		{
			if (m_hdc)
			{
				HBRUSH color = CreateSolidBrush(fillColor);
				RECT rect = { left, top, right, bottom };
				auto result = FillRect(m_hdc, &rect, color);
				SafeReleaseW(&color);
				return result;
			}
			return false;
		}
		bool ClearWindBackground(COLORREF color = RGB(255, 255, 255))override
		{
			if (!m_hdc)
				return false;
			RECT clientRect;
			GetClientRect(m_hWnd, &clientRect);
			HBRUSH hBrush = CreateSolidBrush(color);
			FillRect(m_hdc, &clientRect, hBrush);
			DeleteObject(hBrush);
			return true;
		}
		bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true)override
		{
			if (!run)
			{
				if (m_BackgroundColor)
				{
					DeleteObject(m_BackgroundColor);
				}
				m_BackgroundColor = nullptr;
				return true;
			}
			if (m_BackgroundColor)
			{
				DeleteObject(m_BackgroundColor);
			}
			m_BackgroundColor = CreateSolidBrush(color);
			if (m_BackgroundColor)
				return false;
			return true;
		}
		bool DrawLine(int startX, int startY, int endX, int endY)override
		{
			if (!m_hdc)
				return false;
			MoveToEx(m_hdc, startX, startY, nullptr);
			LineTo(m_hdc, endX, endY);
			return true;
		}
		bool DrawLineTo(int toX, int toY)override
		{
			if (!m_hdc)
				return false;
			LineTo(m_hdc,toX, toY);
			return true;
		}
		bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h)override
		{
			if (!m_hdc)
				return false;
			RECT rect = { x,y,x + w,y + h };
			DrawTextA(m_hdc, ShowText.c_str(), (int)ShowText.size(), &rect, DT_CENTER);
		}
		bool DrawTextWide(const std::wstring& ShowText, int x, int y, int w, int h)override
		{
			if (!m_hdc)
				return false;
			RECT rect = { x,y,x + w,y + h };
			DrawTextW(m_hdc, ShowText.c_str(), (int)ShowText.size(), &rect, DT_CENTER);
		}
		//绘制多边形
		bool DrawGeometry(POINT* pointArr,int pointCount)override
		{
			if (!m_hdc)
				return false;
			Polygon(m_hdc, pointArr, pointCount);
			return true;
		}
		//绘制多边形
		bool DrawGeometry(std::vector<POINT>&points)override
		{
			return DrawGeometry(points.data(),points.size());
		}
		//绘制填充多边形
		bool DrawFillGeometry(POINT* pointArr, int pointCount, COLORREF fillColor)override
		{
			if (!m_hdc)
				return false;
			auto color = CreateSolidBrush(fillColor);
			if (!color)
				return false;
			auto oldColor = SelectObject(m_hdc, color);
			Polygon(m_hdc, pointArr, pointCount);
			SelectObject(m_hdc, oldColor);
			return true;
		}
		//绘制填充多边形
		bool DrawFillGeometry(std::vector<POINT>& points, COLORREF fillColor)override
		{
			return DrawFillGeometry(points.data(), points.size(), fillColor);
		}
		bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1)override
		{
			auto lastPen = m_CurrentPen;
			m_CurrentPen = CreatePen((int)penStyle, LineWidth, color);
			if (!m_CurrentPen)
			{
				m_CurrentPen = lastPen;
				return false;
			}
			SafeReleaseW(&lastPen);
			if (m_hdc)
			{
				SelectObject(m_hdc, m_CurrentPen);
			}
			else
			{
				if (m_hWnd)
				{
					m_hdc = GetDC(m_hWnd);
					SelectObject(m_hdc, m_CurrentPen);
					DeleteDC(m_hdc);
					m_hdc = nullptr;
				}
				else
					return false;
			}
			return true;
		}
	};
	
	class MainWind_D2D :public MainWind
	{
	private:
		ID2D1Factory* m_d2dFactory;
		ID2D1HwndRenderTarget* m_d2dRenderTarget;

		D2D1_POINT_2F m_LastPoint;
		int m_PenWidth;
		ID2D1SolidColorBrush* m_PenBrush;
		ID2D1StrokeStyle* m_PenStyle;

		IDWriteTextFormat* m_TextFormat;

		D2D1_COLOR_F* m_BackgroundColor;


		FILETIME m_LastPaintTime;
		FILETIME m_NowTime;
		float m_IntervalTime;

		WindCallback::CharInputCallback_D2D m_CharCallback;
		WindCallback::PaintCallback_D2D m_PaintCallback;
		WindCallback::SizeCallback_D2D m_WindSizeCallback;
		void OnPaint()override
		{
			if (m_d2dFactory==nullptr || m_d2dRenderTarget==nullptr)
			{
				PAINTSTRUCT ps;
				BeginPaint(m_hWnd, &ps);
				EndPaint(m_hWnd, &ps);
				return;
			}
			GetSystemTimePreciseAsFileTime(&m_NowTime);
			m_IntervalTime = ((*reinterpret_cast<const ULONGLONG*>(&m_NowTime)) - *reinterpret_cast<const ULONGLONG*>(&m_LastPaintTime)) / 10000.f;
			m_d2dRenderTarget->BeginDraw();
			if (m_BackgroundColor!=nullptr)
				m_d2dRenderTarget->Clear(m_BackgroundColor);
			if (m_PaintCallback)
				m_PaintCallback(this);
			// 如果你的程序卡住了，暂停跳转到了这里，
			// 多半是出现了逻辑错误（而且指针非法访问的错误可能也不会报，要自己找）
			// 特别注意对容器的操作
			m_d2dRenderTarget->EndDraw();
			m_LastPaintTime = m_NowTime;
			return;
		}
		void OnSize(int width, int height)override
		{
			if (!m_d2dFactory)
				return;
			HRESULT hr = m_d2dRenderTarget->Resize(D2D1::SizeU(width, height));
			if (FAILED(hr))
			{
				GetLastWindowError error;
				std::cerr << "d2d窗口输出对象创建失败！在窗口大小调整过程中\n" <<error.GetErrorMessage()<< std::endl;
				return;
			}
			if (m_WindSizeCallback)
				m_WindSizeCallback(this, width, height);
		}
		static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_CREATE)
			{
				LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
				MainWind_D2D* mainWind = (MainWind_D2D*)pcs->lpCreateParams;

				::SetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(mainWind)
				);
				return 1;
			}
			MainWind_D2D* mainWind = reinterpret_cast<MainWind_D2D*>(GetWindLongPtr(hWnd));
			if (!mainWind)
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST || uMsg == WM_MOUSEACTIVATE || uMsg == WM_MOUSELEAVE)
			{
				mainWind->OnMouse(uMsg, LOWORD(lParam), HIWORD(lParam), (int)wParam);
				return 0;
			}
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			{
				if (mainWind->m_CharCallback)
				{
					switch (uMsg)
					{
					case WM_CHAR:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_CHAR);
						break;
					case WM_KEYDOWN:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_DOWN);
						break;
					case WM_KEYUP:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_UP);
						break;
					default:
						break;
					}
				}
				break;
			}
			case WM_PAINT:
			{
				mainWind->OnPaint();
				break;
			}
			case WM_SIZE:
			{
				mainWind->OnSize(LOWORD(lParam), HIWORD(lParam));
				break;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
				break;
			}
			return 0;
		}
	public:
		MainWind_D2D() :m_d2dFactory(nullptr), m_d2dRenderTarget(nullptr), m_PenBrush(nullptr), m_LastPoint({ 0,0 }),
			m_PenStyle(nullptr), m_PenWidth(1), m_CharCallback(nullptr), m_PaintCallback(nullptr), m_WindSizeCallback(nullptr),
			m_TextFormat(nullptr), m_LastPaintTime(), m_NowTime(), m_BackgroundColor(nullptr), m_IntervalTime(0)
		{
			WindCount::g_d2dMainWindCount++;
		}
		~MainWind_D2D()
		{
			WindCount::g_d2dMainWindCount--;
			SafeRelease(&m_d2dFactory);
			SafeRelease(&m_d2dRenderTarget);
			SafeRelease(&m_PenBrush);
			SafeRelease(&m_PenStyle);
			SafeReleaseP(&m_BackgroundColor);
			SafeRelease(&m_TextFormat);
			CloseWind();
			if (WindCount::g_d2dMainWindCount == 0)
			{
				UnregisterClassW(WindClassName::MAIN_WINDOW_D2D_CLASS_NAME, m_hInstance);
			}
		}
		ID2D1HwndRenderTarget* GetD2DTargetP()const
		{
			return m_d2dRenderTarget;
		}
		ID2D1Factory* GetD2DFactoryP()const
		{
			return m_d2dFactory;
		}
		HRESULT CreateWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT)override
		{
			if (m_hWnd)
			{ 
				std::cerr << "请使用新的实例创建窗口" << std::endl;
				return S_FALSE;
			}
			m_hInstance = hInstance;

			WNDCLASSW windowClass = {};
			if (GetClassInfoW(hInstance, WindClassName::MAIN_WINDOW_D2D_CLASS_NAME, &windowClass) == 0)
			{
				WNDCLASSEXW wc = { 0 };
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = MainWindowProc;
				wc.hInstance = hInstance;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.lpszClassName = WindClassName::MAIN_WINDOW_D2D_CLASS_NAME;

				// 注册窗口类
				if (!RegisterClassExW(&wc)) {
					GetLastWindowError error;
					std::string errorOutput = "窗口类注册失败!";
					errorOutput += error.GetErrorMessage();
					MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
					return S_FALSE;
				}
			}

			if (parent)
			{
				RECT parentRect;
				GetClientRect(parent, &parentRect);
				m_hWnd = CreateWindowW(
					WindClassName::MAIN_WINDOW_D2D_CLASS_NAME,
					L"d2d窗口",
					WS_CHILD|WS_VISIBLE,
					0, 0, parentRect.right, parentRect.bottom,
					parent, NULL,
					hInstance, this);
			}
			else
			{
				m_hWnd = CreateWindowW(
					WindClassName::MAIN_WINDOW_D2D_CLASS_NAME,
					L"d2d窗口",
					WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					100, 100, 800, 600,
					NULL, NULL,
					hInstance, this);
			}
			if (!m_hWnd)
			{
				GetLastWindowError error;
				std::string errorOutput = "窗口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return S_FALSE;
			}
			HRESULT hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				&m_d2dFactory
			);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d设备创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			hr = m_d2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(
					m_hWnd,
					D2D1::SizeU(
						rc.right - rc.left,
						rc.bottom - rc.top)
				), &m_d2dRenderTarget);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d设备关联窗口失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			hr = m_d2dRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_PenBrush);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d画笔创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}

			IDWriteFactory* writeFactory = nullptr;
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d文本接口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			hr = writeFactory->CreateTextFormat(
				L"Gabriola",
				NULL,
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				30.0f,
				L"",
				&m_TextFormat
			);
			SafeRelease(&writeFactory);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d字体创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			GetSystemTimePreciseAsFileTime(&m_LastPaintTime);
			return S_OK;
		}

		static COLORREF D2DColorFToGdiColor(D2D1::ColorF colorF)
		{
			BYTE redByte = static_cast<BYTE>(colorF.r * 255);
			BYTE greenByte = static_cast<BYTE>(colorF.g * 255);
			BYTE blueByte = static_cast<BYTE>(colorF.b * 255);
			return RGB(redByte, greenByte, blueByte);
		}
		static D2D1::ColorF GdiColorToD2DColor(COLORREF colorRef)
		{
			float red = GetRValue(colorRef) / 255.0f;
			float green = GetGValue(colorRef) / 255.0f;
			float blue = GetBValue(colorRef) / 255.0f;
			return D2D1::ColorF(red, green, blue);
		}

		/// <summary>
		/// 设置字符消息的处理函数
		/// </summary>
		/// <param name="charCallback"></param>
		void SetKeyCallback(WindCallback::CharInputCallback_D2D charCallback)
		{
			m_CharCallback = charCallback;
		}
		/// <summary>
		/// 设置窗口绘制过程函数
		/// </summary>
		/// <param name="paintCallback"></param>
		void SetPaintCallback(WindCallback::PaintCallback_D2D paintCallback)
		{
			m_PaintCallback = paintCallback;
		}
		/// <summary>
		/// 窗口大小改变时调用
		/// </summary>
		/// <param name="sizeCallback"></param>
		void SetWindSizeCallback(WindCallback::SizeCallback_D2D sizeCallback)
		{
			m_WindSizeCallback = sizeCallback;
		}
		const WindCallback::CharInputCallback_D2D& GetKeyCallback()const
		{
			return m_CharCallback;
		}
		const WindCallback::PaintCallback_D2D& GetPaintCallback()const
		{
			return m_PaintCallback;
		}
		const WindCallback::SizeCallback_D2D& GetWindSizeCallback()const
		{
			return m_WindSizeCallback;
		}
		bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom)override
		{
			if(m_d2dRenderTarget)
			{
				D2D1_RECT_F rect = D2D1::RectF((float)left, (float)top, (float)right, (float)bottom);
				m_d2dRenderTarget->DrawRectangle(rect, m_PenBrush,m_PenWidth,m_PenStyle);
				return true;
			}
			return false;
		}
		bool DrawFillRectangle(
			int left,
			int top,
			int right,
			int bottom, COLORREF fillColor)override
		{
			if (m_d2dRenderTarget)
			{
				D2D1_RECT_F rect = D2D1::RectF((float)left, (float)top, (float)right, (float)bottom);
				m_d2dRenderTarget->FillRectangle(rect, m_PenBrush);
				return true;
			}
			return false;
		}
		bool ClearWindBackground(COLORREF color = RGB(255, 255, 255))override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_d2dRenderTarget->Clear(GdiColorToD2DColor(color));
			return true;
		}
		bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true)override
		{
			SafeReleaseP(&m_BackgroundColor);
			if (!run)
			{
				return true;
			}

			m_BackgroundColor = new D2D1_COLOR_F(GdiColorToD2DColor(color));
			if (m_BackgroundColor)
				return true;
			return false;
		}
		bool DrawLine(int startX, int startY, int endX, int endY)override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_LastPoint = D2D1::Point2F((float)endX, (float)endY);
			m_d2dRenderTarget->DrawLine(D2D1::Point2F((float)startX, (float)startY), m_LastPoint, m_PenBrush, m_PenWidth, m_PenStyle);
			return true;
		}
		bool DrawLineTo(int toX, int toY)override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_d2dRenderTarget->DrawLine(m_LastPoint, D2D1::Point2F((float)toX, (float)toY), m_PenBrush, m_PenWidth, m_PenStyle);
			return true;
		}
		bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h)override
		{
			return false;
		}
		bool DrawTextWide(const std::wstring& ShowText, int x, int y, int w, int h)override
		{
			if (!m_d2dRenderTarget)
				return false;
			auto rect = D2D1::RectF((float)x, (float)y, (float)x + w, (float)y + h);
			m_d2dRenderTarget->DrawText(ShowText.c_str(), (int)ShowText.size(), m_TextFormat, rect, m_PenBrush);
			return true;
		}
		bool DrawGeometry(D2D1_POINT_2F* pointArr, int pointCount, bool Fill = true)
		{
			if (!m_d2dRenderTarget||!m_PenBrush)
				return false;
			ID2D1GeometrySink* pSink;
			ID2D1PathGeometry* pathGeometry;
			HRESULT hr= m_d2dFactory->CreatePathGeometry(&pathGeometry);
			if (SUCCEEDED(hr))
			{
				// Write to the path geometry using the geometry sink.
				hr = pathGeometry->Open(&pSink);

				if (SUCCEEDED(hr))
				{
					pSink->BeginFigure(
						*pointArr,
						D2D1_FIGURE_BEGIN_FILLED
					);

					pSink->AddLines(&pointArr[1], pointCount-1);
					pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

					hr = pSink->Close();
				}
				SafeRelease(&pSink);
			}
			if (Fill)
				m_d2dRenderTarget->FillGeometry(pathGeometry, m_PenBrush);
			else
				m_d2dRenderTarget->DrawGeometry(pathGeometry, m_PenBrush, m_PenWidth, m_PenStyle);
			return true;
		}
		//绘制多边形
		bool DrawGeometry(POINT* pointArr,int pointCount)override
		{
			if (!m_d2dRenderTarget)
				return false;
			D2D1_POINT_2F* arr=new D2D1_POINT_2F[pointCount];
			if(!arr)
				return false;
			for (UINT32 i = 0; i < pointCount; i++) 
			{
				POINT poi=pointArr[i];
				arr[i]=D2D1::Point2F(poi.x,poi.y);
			}
			return DrawGeometry(arr, pointCount, false);
		}
		//绘制多边形
		bool DrawGeometry(std::vector<POINT>&points)override
		{
			return DrawGeometry(points.data(),points.size());
		}
		float GetPaintIntervalTime()const
		{
			return m_IntervalTime;
		}

		bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1)override
		{
			D2D1_STROKE_STYLE_PROPERTIES d2dPenStyle = D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_CUSTOM,
				0.0f);
			switch (penStyle)
			{
			case Game::PenStyle::DashedLine:
			{
				std::vector<float> point = { 5,3 };
				if (!SetPenStyle(d2dPenStyle, point))
				{
					return false;
				}
			}
				break;
			case Game::PenStyle::DotLine:
			{
				std::vector<float> point = { 1,3 };
				if (!SetPenStyle(d2dPenStyle, point))
				{
					return false;
				}
			}
				break;
			case Game::PenStyle::DashDot:
			{
				std::vector<float> point = { 10,5,1,5 };
				if (!SetPenStyle(d2dPenStyle, point))
				{
					return false;
				}
			}
				break;
			case Game::PenStyle::DashDotDot:
			{
				std::vector<float> point = { 10,5,1,5,1,5 };
				if (!SetPenStyle(d2dPenStyle, point))
				{
					return false;
				}
			}
				break;
			case Game::PenStyle::NullLine:
			case Game::PenStyle::InsideFrame:
			case Game::PenStyle::SolidLine:
			{
				SafeRelease(&m_PenStyle);
			}
			break;
			default:
				return false;
				break;
			}

			m_PenWidth = LineWidth;
			if (!SetPenColor(color))
				return false;
			return true;
		}
		bool SetPenStyle(const D2D1_STROKE_STYLE_PROPERTIES& penStyle, std::vector<float>&dashes)
		{
			SafeRelease(&m_PenStyle);
			auto hr = m_d2dFactory->CreateStrokeStyle(penStyle,
				dashes.data(),
				dashes.size(),
				&m_PenStyle);
			if (FAILED(hr))
				return false;
			return true;
		}
		bool SetPenColor(D2D1::ColorF color)
		{
			SafeRelease(&m_PenBrush);
			if ((m_d2dRenderTarget->CreateSolidColorBrush(color, &m_PenBrush))<0)
			{
				return false;
			}
			return true;
		}
		bool SetPenColor(COLORREF color)
		{
			return SetPenColor(GdiColorToD2DColor(color));
		}

		//绘制填充多边形
		bool DrawFillGeometry(POINT* pointArr, int pointCount, COLORREF fillColor)override
		{
			return true;
		}
		//绘制填充多边形
		bool DrawFillGeometry(std::vector<POINT>& points, COLORREF fillColor)override
		{
			return true;
		}
	};


	///////////////////以下是音频类///////////////////

	/// <summary>
	/// 仅能播放wav文件
	/// </summary>
	class SimpleSound
	{
	public:
		/// <summary>
		/// 指定播放模式
		/// </summary>
		enum PlayMode
		{
			PM_ONECE = 0,
			/// <summary>
			/// 异步播放，不会阻塞
			/// </summary>
			PM_ASYNC = SND_ASYNC,
			/// <summary>
			/// 循环播放
			/// </summary>
			PM_LOOP = (SND_LOOP | SND_ASYNC),

		};
		SimpleSound(){}
		~SimpleSound()
		{
			Stop();
		}
		/// <summary>
		/// 从内存加载wav数据
		/// </summary>
		/// <param name="WavData"></param>
		void LoadMusicFormMemery(const std::vector<BYTE>& WavData)
		{
			m_Data = WavData;
		}
		/// <summary>
		/// 从文件加载wav数据
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		bool LoadMusicFile(CQSTR filePath)
		{
			HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == nullptr)
			{
				std::cout << "文件打开失败" << std::endl;
				return false;
			}
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			m_Data.resize(dwFileSize);
			DWORD dwBytesRead;
			if (ReadFile(hFile, m_Data.data(), dwFileSize, &dwBytesRead, NULL))
			{
				CloseHandle(hFile);
				return true;
			}
			CloseHandle(hFile);
			return false;
		}
		
		bool Play(PlayMode mode = PM_ASYNC)
		{
			if (m_Data.empty())
			{
				std::cout << "兄弟，你还没有加载文件呢" << std::endl;
				return false;
			}
			else
				return PlaySound((TCHAR*)m_Data.data(), nullptr, SND_MEMORY | SND_APPLICATION | (int)mode);
		}
		bool Play(CQSTR filePath, PlayMode mode = PM_ASYNC)
		{
			return PlaySound(filePath.c_str(), nullptr, SND_APPLICATION | (int)mode );
		}
		void Stop()
		{

			PlaySound(NULL, NULL, 0);
			return;
		}
	private:
		std::vector<BYTE>m_Data;
	};
	class Sound
	{
		STR m_id;
	public:

		Sound() :m_id(StrTool::ToStr((long long)this)) {}
		~Sound()
		{
			Close();
		}
		/// <summary>
		/// 打开音频文件
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		bool Open(CQSTR filePath)const
		{
			if (0 != mciSendString((TEXT("open ") + filePath + TEXT(" alias ") + m_id).c_str(), NULL, 0, NULL))
			{
				std::cout << "打开音频失败" << std::endl;
				return false;
			}
			return true;
		}
		/// <summary>
		/// 重新播放音频。
		/// </summary>
		/// <returns></returns>
		bool Play()const
		{
			Seek(0);
			if (0 != mciSendString((TEXT("play ") + m_id).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 循环播放音频
		/// </summary>
		/// <returns></returns>
		bool RepeatPlay()const
		{
			if (0 != mciSendString((TEXT("play ") + m_id + TEXT(" repeat")).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 跳转到指定时刻，单位毫秒ms
		/// </summary>
		/// <param name="ms">毫秒</param>
		/// <returns></returns>
		bool Seek(int ms)const
		{
			if (0 != mciSendString((TEXT("seek ") + m_id + TEXT(" to ")+StrTool::ToStr(ms,0)).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 停止播放
		/// </summary>
		/// <returns></returns>
		bool Stop()const
		{
			if (0 != mciSendString((TEXT("stop ") + m_id).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 关闭并清空
		/// </summary>
		/// <returns></returns>
		bool Close()const
		{
			if (0 != mciSendString((TEXT("close ") + m_id).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 暂停播放
		/// </summary>
		/// <returns></returns>
		bool Pause()const
		{
			if (0 != mciSendString((TEXT("pause ") + m_id).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 继续播放
		/// </summary>
		/// <returns></returns>
		bool Resume()const
		{
			if (0 != mciSendString((TEXT("resume ") + m_id).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 调节音量
		/// </summary>
		/// <param name="v">范围是0-1000，表示调成系统音量的v/1000，比如800是80%</param>
		/// <returns></returns>
		bool SetAudioVolume(int v)const
		{
			if (0 != mciSendString((TEXT("setaudio ") + m_id + TEXT(" volume to ") + StrTool::ToStr(v,0)).c_str(), NULL, 0, NULL))
			{
				return false;
			}
			return true;
		}
		/// <summary>
		/// 获取当前音量
		/// </summary>
		/// <returns></returns>
		int GetVolume()const
		{
			WCHAR str[32];
			mciSendStringW(L"status music volume", str, 32, 0);
			return std::wcstol(str, nullptr, 32);
		}
		/// <summary>
		/// 获取当前音乐长度
		/// </summary>
		/// <returns>单位是毫秒</returns>
		int GetLegth()const
		{
			WCHAR str[32];
			mciSendStringW(L"status music length", str, 32, 0);
			return std::wcstol(str, nullptr, 32);
		}
		/// <summary>
		/// 获取当前播放位置
		/// </summary>
		/// <returns></returns>
		int GetPosition()const
		{
			WCHAR str[32];
			mciSendStringW(L"status music position", str, 32, 0);
			return std::wcstol(str, nullptr, 32);
		}
		/// <summary>
		/// 获取标识符，用于对mciSendString进行更多操作
		/// </summary>
		/// <returns></returns>
		const auto& GetSoundID()const
		{
			return m_id;
		}
	};
}