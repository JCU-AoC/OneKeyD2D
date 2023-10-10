#pragma once
#include"WindowElements.hpp"
#include<map>
#include <iomanip>
#include<vector>
#include <sstream>
#include <minwindef.h>
namespace Game {
	namespace WindControl{
		/// <summary>
		/// 以图片为背景的按钮
		/// </summary>
		class d2dImageButton
		{
			MainWind_D2D* m_TargetWind;
			WindElements::d2dClickDetection m_CheckRect;
			WindElements::d2dPicture m_BackgroundImage;
			WindElements::d2dText m_ShowText;
		public:
			d2dImageButton():m_TargetWind(nullptr){}
			~d2dImageButton()
			{
				Unbind();
			}
			void Init(float x, float y, float w, float h,
				const std::wstring& str,const D2D1_COLOR_F& strColor,
				WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind,
				const std::wstring& backgroundImageFilePath = L"",long long UserData=0)
			{
				m_CheckRect.SetUserData(UserData);
				m_ShowText.SetPosition(x, y);
				m_ShowText.SetShowWide(w, h);
				m_ShowText.SetShowText(str);
				m_ShowText.SetColor(strColor,TargetWind->GetD2DTargetP());
				m_CheckRect.SetPosition(x, y);
				m_CheckRect.SetWide(w, h);
				m_CheckRect.SetButtonCallback(callback);
				if (!backgroundImageFilePath.empty())
				{
					m_BackgroundImage.LoadPictureFromFile(backgroundImageFilePath, TargetWind);
					m_BackgroundImage.SetPosition(x, y);
					m_BackgroundImage.SetShowWide(w, h);
				}
			}
			/// <summary>
			/// 设置字体大小
			/// </summary>
			void SetFontSize(float size)
			{
				m_ShowText.SetTextFontSize(size);
			}
			WindElements::d2dText& GetTextElement()
			{
				return m_ShowText;
			}
			void SetUserData(LONG64 data)
			{
				m_CheckRect.SetUserData(data);
			}
			LONG64& GetUserData()
			{
				return m_CheckRect.GetUserData();
			}
			void Negation(bool negation = true)
			{
				m_CheckRect.Negation(negation);
			}
			void Bind(MainWind_D2D* TargetWind)
			{
				Unbind();
				m_TargetWind = TargetWind;
				TargetWind->AddButten(&m_CheckRect);
			}
			void Unbind()
			{
				if (m_TargetWind)
					m_TargetWind->DeleteButten(&m_CheckRect);
				m_TargetWind = nullptr;
			}
			void SetRect(const D2D1_RECT_F& rect)
			{
				m_CheckRect.SetRectangle(rect);
				m_BackgroundImage.SetShowRect(rect);
				m_ShowText.SetShowRect(rect);
			}
			void Draw(MainWind_D2D* wind)
			{
				m_BackgroundImage.Draw(wind);
				m_ShowText.Draw(wind);
			}
			void Draw(ID2D1RenderTarget* wind)
			{
				m_BackgroundImage.Draw(wind);
				m_ShowText.Draw(wind);
			}
			void LoadBackgroundImage(const std::wstring& filePath, MainWind_D2D* TargetWind)
			{
				m_BackgroundImage.LoadPictureFromFile(filePath, TargetWind);
			}
			void SetCallback(WindCallback::ButtonCallback cb)
			{
				m_CheckRect.SetButtonCallback(cb);
			}
			void LinkWindow(MainWind_D2D* wind)
			{
				wind->AddButten(&m_CheckRect);
			}
			void DisconnectWindow(MainWind_D2D* wind)
			{
				wind->DeleteButten(&m_CheckRect);
			}
			void SetShowText(const std::wstring& str)
			{
				m_ShowText.SetShowText(str);
			}
			std::wstring& GetShowString()
			{
				return m_ShowText.GetShowText();
			}
			const std::wstring& GetShowString()const
			{
				return m_ShowText.GetShowText();
			}
			void SetTextColor(const D2D1_COLOR_F& color, MainWind_D2D* wind)
			{
				SetTextColor(color, wind->GetD2DTargetP());
			}
			void SetTextColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* wind)
			{
				m_ShowText.SetColor(color, wind);
			}
			D2D1_COLOR_F&& GetTextColor()const
			{
				return m_ShowText.GetColor();
			}
		};
		/// <summary>
		/// 已色块矩形为背景的按钮
		/// </summary>
		class d2dColorButton
		{
			MainWind_D2D* m_TargetWind;
			WindElements::d2dClickDetection m_CheckRect;
			WindElements::d2dRectangle m_Rectangle;
			WindElements::d2dText m_ShowText;
		public:
			d2dColorButton() :m_TargetWind(nullptr) {}
			~d2dColorButton()
			{
				Unbind();
			}
			void Init(float x, float y, float w, float h, 
				const std::wstring& str, 
				const D2D1_COLOR_F& textColor,const D2D1_COLOR_F& bkColor,
				WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind,long long UserData=0)
			{
				D2D_RECT_F rect = { x,y,x + w,y + h };
				SetRect(rect);
				m_CheckRect.SetButtonCallback(callback);
				m_ShowText.SetColor(textColor,TargetWind->GetD2DTargetP());
				m_ShowText.SetShowText(str);
				m_Rectangle.SetColor(bkColor,TargetWind->GetD2DTargetP());
				m_Rectangle.FillRect();
				m_CheckRect.SetUserData(UserData);
			}
			WindElements::d2dText& GetTextElement()
			{
				return m_ShowText;
			}
			/// <summary>
			/// 设置字体大小
			/// </summary>
			void SetFontSize(float size)
			{
				m_ShowText.SetTextFontSize(size);
			}
			void SetBakegroundFill(bool fill = true)
			{
				m_Rectangle.FillRect(fill);
			}
			void SetUserData(LONG64 data)
			{
				m_CheckRect.SetUserData(data);
			}
			LONG64& GetUserData()
			{
				return m_CheckRect.GetUserData();
			}
			void Negation(bool negation = true)
			{
				m_CheckRect.Negation(negation);
			}
			void Bind(MainWind_D2D* TargetWind)
			{
				Unbind();
				m_TargetWind = TargetWind;
				TargetWind->AddButten(&m_CheckRect);
			}
			void Unbind()
			{
				if (m_TargetWind)
					m_TargetWind->DeleteButten(&m_CheckRect);
				m_TargetWind = nullptr;
			}
			void SetRect(const D2D1_RECT_F& rect)
			{
				m_CheckRect.SetRectangle(rect);
				m_Rectangle.SetShowRect(rect);
				m_ShowText.SetShowRect(rect);
			}
			void Draw(MainWind_D2D* wind)
			{
				m_Rectangle.Draw(wind);
				m_ShowText.Draw(wind);
			}
			void Draw(ID2D1RenderTarget* wind)
			{
				m_Rectangle.Draw(wind);
				m_ShowText.Draw(wind);
			}
			void SetCallback(WindCallback::ButtonCallback cb)
			{
				m_CheckRect.SetButtonCallback(cb);
			}
			void LinkWindow(MainWind_D2D* wind)
			{
				wind->AddButten(&m_CheckRect);
			}
			void DisconnectWindow(MainWind_D2D* wind)
			{
				wind->DeleteButten(&m_CheckRect);
			}
			void SetShowText(const std::wstring& str)
			{
				m_ShowText.SetShowText(str);
			}
			std::wstring& GetShowString()
			{
				return m_ShowText.GetShowText();
			}
			const std::wstring& GetShowString()const
			{
				return m_ShowText.GetShowText();
			}
			void SetTextColor(const D2D1_COLOR_F& color, MainWind_D2D* wind)
			{
				SetTextColor(color, wind->GetD2DTargetP());
			}
			void SetTextColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* wind)
			{
				m_ShowText.SetColor(color, wind);
			}
			D2D1_COLOR_F&& GetTextColor()const
			{
				return m_ShowText.GetColor();
			}
			void SetBackgroundColor(const D2D1_COLOR_F& color, MainWind_D2D* wind)
			{
				SetBackgroundColor(color, wind->GetD2DTargetP());
			}
			void SetBackgroundColor(const D2D1_COLOR_F& color,ID2D1RenderTarget* wind)
			{
				m_Rectangle.SetColor(color, wind);
			}
			D2D1_COLOR_F&& GetBackgroundColor()const
			{
				return m_Rectangle.GetColor();
			}
		};

		/// <summary>
		///  绘制折线函数图像(背景透明)
		/// 吃内存大户
		/// </summary>
		class d2dFunctionWind
		{
			int m_DecimalPlaces;
			float m_Interval;
			float m_Identification;
			D2D1_COLOR_F m_TextColor= D2D1::ColorF(0, 0, 0);

			D2D1_RECT_F m_ShowRect;
			D2D1_RECT_F m_Coordinate;
			ID2D1SolidColorBrush* m_axisColor;
			std::vector<WindElements::d2dText*> m_ticksX;
			std::vector<WindElements::d2dText*> m_ticksY;
			std::map<int, std::pair<std::vector<float>, std::vector<float>>> m_LineList;
			std::map<int, WindElements::d2dFoldLine>m_Data;
			void CoordinateToWindCoordinate(std::vector<float>& xList, std::vector<float>& yList)
			{
				if (m_ticksX.empty() || m_ticksY.empty())
					return;

				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval*2;
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float xTick = xWide / (m_Coordinate.top - m_Coordinate.left);
				float yTick = yWide / (m_Coordinate.bottom - m_Coordinate.right);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;

				for (auto& i : xList)
				{
					i = originX + (i - m_Coordinate.left) * xTick;
				}
				for (auto& i : yList)
				{
					i = originY - (i - m_Coordinate.right) * yTick;
				}
			}
			void CreateAixLineColor(ID2D1RenderTarget* tar)
			{
				tar->CreateSolidColorBrush(m_TextColor, &m_axisColor);
			}
			void UpdateCoordinate()
			{
				for (auto& i : m_LineList)
				{
					auto x = i.second.first;
					auto y = i.second.second;
					CoordinateToWindCoordinate(x, y);
					m_Data[i.first].SetFoldLine(x, y);
				}
			}
		public:
			d2dFunctionWind() : m_ShowRect({ 0,0,0,0 }), m_Coordinate({ 0,0,0,0 }),\
				m_axisColor(nullptr),m_Interval(80),m_Identification(5),m_DecimalPlaces(2){}
			~d2dFunctionWind()
			{
				SafeRelease(&m_axisColor);
				for (auto& P : m_ticksY)
				{
					SafeReleaseP(&P);
				}
				for (auto& P : m_ticksX)
				{
					SafeReleaseP(&P);
				}
			}
			//设置数字坐标轴保留的小数位数
			void SetDecimalPlaces(int dp)
			{
				m_DecimalPlaces = dp;
			}
			//获取当前数字坐标轴保留的小数位数
			int GetDecimalPlaces()const
			{
				return m_DecimalPlaces;
			}
			//设置轴线到边框的间距
			void SetBorderSpacing(float bs)
			{
				m_Interval = bs;
			}
			//设置文本颜色
			void SetTextColor(const D2D1_COLOR_F& color,ID2D1RenderTarget* tar)
			{
				m_TextColor = color;
				for (auto& i : m_ticksX)
				{
					i->SetColor(color, tar);
				}
				for (auto& i : m_ticksY)
				{
					i->SetColor(color, tar);
				}
			}
			//设置坐标轴颜色
			void SetAxisColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* tar)
			{
				SafeRelease(&m_axisColor);
				tar->CreateSolidColorBrush(color, &m_axisColor);
			}
			//初始化
			void Init(const D2D1_RECT_F& ShowRect,ID2D1RenderTarget* tar)
			{
				m_ShowRect = ShowRect;
				if (!m_axisColor)
				{
					CreateAixLineColor(tar);
				}
			}
			//设置显示区域
			void SetShowRect(const D2D1_RECT_F& rect)
			{
				m_ShowRect = rect;
				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float xTick = xWide / (m_ticksX.size() - 1);
				float yTick = yWide / (m_ticksY.size() - 1);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				for (int i = 0; i < m_ticksX.size(); ++i)
				{
					m_ticksX[i]->SetPosition(originX + i * xTick - m_Interval / 2, originY + m_Identification * 3);
				}
				for (int i = 0; i < m_ticksY.size(); ++i)
				{
					m_ticksY[i]->SetPosition(m_ShowRect.left, -i * yTick + originY - 10);
				}
				UpdateCoordinate();
			}

			void SetAixsCoordinateX(float minX, float maxX, float tickX, ID2D1RenderTarget* tar)
			{
				m_Coordinate = { minX,maxX,m_Coordinate.right,m_Coordinate.bottom };
				for (auto& P : m_ticksX)
				{
					SafeReleaseP(&P);
				}
				m_ticksX.clear();
				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float xTick = xWide / (maxX - minX);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				for (auto x = minX; x <= maxX; x += tickX)
				{
					std::wstringstream wss;
					wss << std::fixed << std::setprecision(m_DecimalPlaces) << x;
					std::wstring result = wss.str();
					WindElements::d2dText* text = new WindElements::d2dText(result.c_str(), originX + (x - minX) * xTick - m_Interval / 2, originY + m_Identification * 3, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksX.push_back(text);
				}
				UpdateCoordinate();
			}
			void SetAixsCoordinateY(float minY, float maxY, float tickY, ID2D1RenderTarget* tar)
			{
				m_Coordinate = {m_Coordinate.left,m_Coordinate.top,minY,maxY };
				for (auto& P : m_ticksY)
				{
					SafeReleaseP(&P);
				}
				m_ticksY.clear();
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float yTick = yWide / (maxY - minY);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				for (auto y = minY; y <= maxY; y += tickY)
				{
					std::wstringstream wss;
					wss << std::fixed << std::setprecision(m_DecimalPlaces) << y;
					std::wstring result = wss.str();
					WindElements::d2dText* text = new WindElements::d2dText(result.c_str(), m_ShowRect.left, -(y - minY) * yTick + originY - 10, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksY.push_back(text);
				}
				UpdateCoordinate();
			}
			void SetAixsCoordinateX(float minX, float maxX, const std::vector<std::wstring>& xTickSign, ID2D1RenderTarget* tar)
			{
				m_Coordinate.left = minX;
				m_Coordinate.top = maxX;
				for (auto& P : m_ticksX)
				{
					SafeReleaseP(&P);
				}
				m_ticksX.clear();
				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float xTick = xWide / (xTickSign.size());
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;

				for (int i = 0; i < xTickSign.size(); ++i)
				{
					WindElements::d2dText* text = new WindElements::d2dText(xTickSign[i].c_str(), originX + i * xTick - m_Interval / 2, originY + m_Identification * 3, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksX.push_back(text);
				}
				UpdateCoordinate();
			}
			void SetAixsCoordinateY(float minY, float maxY, const std::vector<std::wstring>& yTickSign, ID2D1RenderTarget* tar)
			{
				m_Coordinate.right = minY;
				m_Coordinate.bottom = maxY;
				for (auto& P : m_ticksY)
				{
					SafeReleaseP(&P);
				}
				m_ticksY.clear();
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float yTick = yWide / (yTickSign.size());
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				for (int i = 0; i < yTickSign.size(); ++i)
				{
					WindElements::d2dText* text = new WindElements::d2dText(yTickSign[i].c_str(), m_ShowRect.left, -i * yTick + originY - 10, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksY.push_back(text);
				}
				UpdateCoordinate();
			}
			//设置坐标轴范围及文本
			void SetCoordinate(float minX, float maxX,float minY, float maxY,
				const std::vector<std::wstring>& xTickSign, const std::vector<std::wstring>& yTickSign, ID2D1RenderTarget* tar)
			{
				m_Coordinate = { minX,maxX,minY,maxY };
				for (auto& P : m_ticksY)
				{
					SafeReleaseP(&P);
				}
				for (auto& P : m_ticksX)
				{
					SafeReleaseP(&P);
				}
				m_ticksX.clear();
				m_ticksY.clear();
				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float xTick = xWide / (xTickSign.size());
				float yTick = yWide / (yTickSign.size());
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;

				for (int i = 0; i < xTickSign.size(); ++i)
				{
					WindElements::d2dText* text = new WindElements::d2dText(xTickSign[i].c_str(), originX + i * xTick - m_Interval / 2, originY + m_Identification * 3, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksX.push_back(text);
				}
				for (int i = 0; i < yTickSign.size(); ++i)
				{
					WindElements::d2dText* text = new WindElements::d2dText(yTickSign[i].c_str(), m_ShowRect.left, -i * yTick + originY - 10, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksY.push_back(text);
				}
				UpdateCoordinate();
			}
			//以纯数字设置坐标轴
			void SetCoordinate(float minX, float maxX, float tickX, float minY, float maxY, float tickY,ID2D1RenderTarget* tar)
			{
				m_Coordinate = { minX,maxX,minY,maxY };
				for (auto& P : m_ticksY)
				{
					SafeReleaseP(&P);
				}
				for (auto& P : m_ticksX)
				{
					SafeReleaseP(&P);
				}
				m_ticksX.clear();
				m_ticksY.clear();
				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float xTick = xWide / (maxX - minX);
				float yTick = yWide / (maxY - minY);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				for (auto x = minX; x <= maxX; x += tickX)
				{
					std::wstringstream wss;
					wss << std::fixed << std::setprecision(m_DecimalPlaces) << x;
					std::wstring result = wss.str();
					WindElements::d2dText* text = new WindElements::d2dText(result.c_str(), originX + (x-minX)*xTick - m_Interval / 2, originY + m_Identification*3, m_Interval, 20);
					text->SetColor(m_TextColor, tar);
					m_ticksX.push_back(text);
				}
				for (auto y = minY; y <= maxY; y += tickY)
				{
					std::wstringstream wss;
					wss << std::fixed << std::setprecision(m_DecimalPlaces) << y;
					std::wstring result = wss.str();
					WindElements::d2dText* text= new WindElements::d2dText(result.c_str(), m_ShowRect.left, -(y-minY) * yTick + originY - 10, m_Interval, 20);
					text->SetColor(m_TextColor,tar);
					m_ticksY.push_back(text);
				}
				UpdateCoordinate();
			}
			//添加函数到坐标轴
			int AddFunction(const std::vector<float>& xList, const std::vector<float>& yList,int id,const D2D1_COLOR_F& color,ID2D1HwndRenderTarget* tar)
			{
				if (xList.size() != yList.size() || id < 0 || xList.empty()||!tar)
				{
					std::cout << "大小不匹配" << std::endl;
					return -1;
				}
				if (!m_axisColor)
				{
					CreateAixLineColor(tar);
				}
				if(m_Coordinate.left==m_Coordinate.top)
				{
					float xMin = xList.front(), xMax = xList.front(), yMin = yList.front(), yMax = yList.front();
					for (auto&& f : xList)
					{
						xMin = min(f, xMin);
						xMax = max(f, xMax);
					}
					for (auto&& f : yList)
					{
						yMin = min(f, yMin);
						yMax = max(f, yMax);
					}
					SetCoordinate(xMin, xMax, max((xMax - xMin) / xList.size(), (xMax - xMin) / 5), yMin, yMax, max((yMax - yMin) / yList.size(), (yMax - yMin) / 5), tar);
				}
				m_LineList[id] = std::make_pair(xList, yList);
				m_Data[id] = WindElements::d2dFoldLine();
				auto& fl = m_Data[id];
				fl.SetColor(color, tar);
				auto XLIST = xList;
				auto YLIST = yList;
				CoordinateToWindCoordinate(XLIST, YLIST);
				fl.SetFoldLine(XLIST, YLIST);
				return id;
			}
			//移除函数
			bool DeleteFunction(int id)
			{
				auto result = m_Data.find(id);
				if (result == m_Data.end())
					return false;
				m_Data.erase(result);
				auto Lresult = m_LineList.find(id);
				if (Lresult == m_LineList.end())
					return false;
				m_LineList.erase(id);
				return true;;
			}
			WindElements::d2dFoldLine* GetFunctionWindControl(int id)
			{
				auto result = m_Data.find(id);
				if (result == m_Data.end())
					return nullptr;
				return &result->second;
			}
			auto* GetFunction(int id)
			{
				auto result = m_LineList.find(id);
				if (result == m_LineList.end())
					return (std::pair<std::vector<float>, std::vector<float>>*)(nullptr);
				return &result->second;
			}
			void Draw(MainWind_D2D* target)
			{
				Draw(target->GetD2DTargetP());
			}
			void Draw(ID2D1HwndRenderTarget* target)
			{
				if (!m_axisColor)
					return;
				target->PushAxisAlignedClip(m_ShowRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

				target->DrawRectangle(m_ShowRect, m_axisColor);

				float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
				float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
				float xTick = xWide / (m_ticksX.size()-1);
				float yTick = yWide / (m_ticksY.size()-1);
				float originX = m_ShowRect.left + m_Interval;
				float originY = m_ShowRect.bottom - m_Interval;
				//画y轴轴线
				auto yTopPoint = D2D1::Point2F(originX, m_ShowRect.top + m_Interval - m_Interval / 2);
				target->DrawLine(
					D2D1::Point2F(originX, m_ShowRect.bottom - m_Interval),
					yTopPoint, m_axisColor);
				//画箭头
				target->DrawLine(yTopPoint, D2D1::Point2F(yTopPoint.x - 3, yTopPoint.y + 3),
					m_axisColor);
				target->DrawLine(yTopPoint, D2D1::Point2F(yTopPoint.x + 3, yTopPoint.y + 3),
					m_axisColor);
				for(int i = 0; i < m_ticksY.size(); ++i)
				{
					target->DrawLine(
						D2D1::Point2F(originX, originY - yTick * i),
						D2D1::Point2F(originX - m_Identification, originY - yTick * i), m_axisColor);
					m_ticksY[i]->Draw(target);
				}
				//画x轴线
				auto xTopPoint = D2D1::Point2F(m_ShowRect.right - m_Interval + m_Interval / 2, originY);
				target->DrawLine(
					D2D1::Point2F(m_ShowRect.left + m_Interval, originY),
					xTopPoint, m_axisColor);
				//画箭头
				target->DrawLine(xTopPoint, D2D1::Point2F(xTopPoint.x - 3, xTopPoint.y - 3),
					m_axisColor);
				target->DrawLine(xTopPoint, D2D1::Point2F(xTopPoint.x - 3, xTopPoint.y + 3),
					m_axisColor);
				for (int i = 0; i < m_ticksX.size(); ++i)
				{
					target->DrawLine(
						D2D1::Point2F(originX + xTick * i, originY),
						D2D1::Point2F(originX + xTick * i, originY + m_Identification), m_axisColor);
					m_ticksX[i]->Draw(target);
				}
				for (auto& fl : m_Data)
				{
					fl.second.Draw(target);
				}
				target->PopAxisAlignedClip();
			}
		};


		/// <summary>
		/// 编辑框，用于编辑文字
		/// 当用户点击进入编辑框，离开编辑框，或者完成编辑时会调用指定消息循环
		/// 当开始输入时，窗口的字符回调会被替换并延迟执行（如果存在）
		/// 不要在编辑期间修改字符回调，可能导致难以预测的错误
		/// </summary>

		class d2dEdit
		{
			static d2dEdit* g_CurrentFocusEdit;

			const D2D1_COLOR_F DefultColor = D2D1::ColorF(0.7, 0.7, 0.7);
			const D2D1_COLOR_F SelectedColor = D2D1::ColorF(0.8, 0.8, 0.8);
			const D2D1_COLOR_F m_CursorColorF = D2D1::ColorF(0);

			MainWind_D2D* m_BindedWind;
			WindCallback::EditControlCallback m_EditCallback;
			WindControl::d2dColorButton m_CheckRect;

			WindCallback::CharInputCallback_D2D m_LastCharCallback;
			D2D1_POINT_2F m_Indicator;
			ID2D1SolidColorBrush* m_CursorColor;

			int m_StringShiftPtr;
			bool m_Focus;

			static void StartInput(MainWind* wind,long long ptr)
			{
				d2dEdit* thisP = (d2dEdit*)ptr;
				if (!thisP)
					return;
				thisP->UserClicks();
			}
			static void DetectionInput(MainWind_D2D*,int input,int Fa,KeyMode mode)
			{
				if (mode == KM_UP || !g_CurrentFocusEdit)
					return;
				g_CurrentFocusEdit->SetStr(mode, input);
			}
			void SetStr(KeyMode mode,int input)
			{
				auto& showStr = m_CheckRect.GetShowString();
				switch (input)
				{
				case VK_BACK:
				{
					auto& strPtr = m_StringShiftPtr;
					if (showStr.empty() || mode != KM_CHAR || strPtr >= showStr.size())
						return;
					showStr.erase(showStr.end() - strPtr - 1);
					m_Indicator = m_CheckRect.GetTextElement().GetStringCharPosition(showStr.size() - strPtr);
					break;
				}
				case VK_LEFT:
				{
					auto& strPtr = m_StringShiftPtr;
					strPtr++;
					break;
				}
				case VK_RIGHT:
				{
					auto& strPtr = m_StringShiftPtr;
					strPtr--;
					if (strPtr < 0)
					{
						strPtr = 0;
					}
					break;
				}
				default:
				{
					if (mode == KM_CHAR)
					{
						showStr.push_back(input);
						m_Indicator = m_CheckRect.GetTextElement().GetStringCharPosition(showStr.size() - m_StringShiftPtr);
					}
				}
				break;
				}
			}
			void UserClicks()
			{
				if (m_Focus)
				{
					m_BindedWind->SetKeyCallback(m_LastCharCallback);

					m_CheckRect.Negation(false);
					m_CheckRect.SetBackgroundColor(DefultColor, m_BindedWind);
					if (m_EditCallback)
						m_EditCallback(m_BindedWind, m_CheckRect.GetShowString(), ControlMessage::LeaveControl);

					g_CurrentFocusEdit = nullptr;
				}
				else
				{
					g_CurrentFocusEdit = this;

					m_LastCharCallback = m_BindedWind->GetKeyCallback();
					m_BindedWind->SetKeyCallback(DetectionInput);

					m_CheckRect.Negation(true);
					m_CheckRect.SetBackgroundColor(SelectedColor, m_BindedWind);
					if (m_EditCallback)
						m_EditCallback(m_BindedWind, m_CheckRect.GetShowString(), ControlMessage::EnterControl);
				}
				m_Focus = !m_Focus;
			}

		public:
			d2dEdit() :m_BindedWind(nullptr), m_Focus(false), m_LastCharCallback(nullptr), m_StringShiftPtr(0), m_CursorColor(nullptr)
			{
				m_CheckRect.SetCallback(StartInput);
				m_CheckRect.SetUserData((LONG64)this);
				m_CheckRect.SetBakegroundFill();
			};
			~d2dEdit()
			{
				UnBind();
				SafeRelease(&m_CursorColor);
			}
			const std::wstring& GetShowString()const
			{
				return m_CheckRect.GetShowString();
			}
			std::wstring& GetShowString()
			{
				return m_CheckRect.GetShowString();
			}
			void SetShowString(const std::wstring& str)
			{
				m_CheckRect.SetShowText(str);
			}
			void SetRect(const D2D1_RECT_F& rect)
			{
				m_CheckRect.SetRect(rect);
			}
			void SetEditCallback(WindCallback::EditControlCallback callback)
			{
				m_EditCallback = callback;
			}
			void RunCallback()const
			{
				if (m_EditCallback)
					m_EditCallback(m_BindedWind, m_CheckRect.GetShowString(), ControlMessage::ControlMessage);
			}
			void Draw(MainWind_D2D* wind)
			{
				Draw(wind->GetD2DTargetP());
			}
			void Draw(ID2D1RenderTarget* wind)
			{
				m_CheckRect.Draw(wind);
				if (m_CursorColor)
					wind->DrawLine(m_Indicator, D2D1::Point2F(m_Indicator.x, m_Indicator.y + 10),m_CursorColor);
			}
			void Bind(MainWind_D2D* wind)
			{
				UnBind();
				m_BindedWind = wind;
				m_CheckRect.Bind(wind);
				m_CheckRect.SetBackgroundColor(DefultColor, wind);
				m_CheckRect.SetTextColor(D2D1::ColorF(0), wind);
				wind->GetD2DTargetP()->CreateSolidColorBrush(m_CursorColorF, &m_CursorColor);
			}
			void UnBind()
			{
				if(!m_BindedWind)
					return;
				m_CheckRect.Unbind();
			}
		};
		d2dEdit* d2dEdit::g_CurrentFocusEdit = nullptr;
	}
}
