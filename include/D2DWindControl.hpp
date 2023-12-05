#pragma once
#include "WindowElements.hpp"
#include <iomanip>
#include <map>
#include <minwindef.h>
#include <sstream>
#include <vector>
namespace Game {
namespace WindControl {
    /// <summary>
    /// 以图片为背景的按钮
    /// </summary>
    class d2dImageButton {
        MainWind_D2D* m_TargetWind;
        WindElements::d2dClickDetection m_CheckRect;
        WindElements::d2dPicture m_BackgroundImage;
        WindElements::d2dText m_ShowText;

    public:
        d2dImageButton()
            : m_TargetWind(nullptr)
        {
        }
        ~d2dImageButton()
        {
            Unbind();
        }
        MainWind_D2D* GetBindedWindow() const
        {
            return m_TargetWind;
        }
        void Init(float x, float y, float w, float h,
            const std::wstring& str, const D2D1_COLOR_F& strColor,
            WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind,
            const std::wstring& backgroundImageFilePath = L"", long long UserData = 0)
        {
            m_CheckRect.SetUserData(UserData);
            m_ShowText.SetPosition(x, y);
            m_ShowText.SetShowWide(w, h);
            m_ShowText.SetShowText(str);
            m_ShowText.SetColor(strColor, TargetWind->GetD2DTargetP());
            m_ShowText.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_ShowText.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            m_CheckRect.SetPosition(x, y);
            m_CheckRect.SetWide(w, h);
            m_CheckRect.SetButtonCallback(callback);
            if (!backgroundImageFilePath.empty()) {
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
        const LONG64& GetUserData() const
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
        void Draw(MainWind_D2D* d2dWind, const D2D1_RECT_F& rect)
        {
            Draw(d2dWind->GetD2DTargetP(), rect);
        }
        void Draw(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& rect)
        {
            m_BackgroundImage.DrawInRect(d2dRenderTarget, rect);
            m_ShowText.Draw(d2dRenderTarget, rect);
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
        void SetShowText(const std::wstring& str)
        {
            m_ShowText.SetShowText(str);
        }
        std::wstring& GetShowString()
        {
            return m_ShowText.GetShowText();
        }
        const std::wstring& GetShowString() const
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
        D2D1_COLOR_F GetTextColor() const
        {
            return m_ShowText.GetColor();
        }
        D2D1_RECT_F GetShowRect() const
        {
            return m_CheckRect.GetRectangle();
        }
        D2D1_SIZE_F GetShowSize() const
        {
            auto rect = m_CheckRect.GetRectangle();
            return D2D1::SizeF(rect.right - rect.left, rect.bottom - rect.top);
        }
        D2D1_POINT_2F GetShowPosition() const
        {
            auto rect = m_CheckRect.GetRectangle();
            return D2D1::Point2F(rect.left, rect.top);
        }
    };
    /// <summary>
    /// 已色块矩形为背景的按钮
    /// </summary>
    class d2dColorButton {
        MainWind_D2D* m_TargetWind;
        WindElements::d2dClickDetection m_CheckRect;
        WindElements::d2dRectangle m_Rectangle;
        WindElements::d2dText m_ShowText;

    public:
        d2dColorButton()
            : m_TargetWind(nullptr)
        {
        }
        ~d2dColorButton()
        {
            Unbind();
        }
        MainWind_D2D* GetBindedWindow() const
        {
            return m_TargetWind;
        }
        void Init(float x, float y, float w, float h,
            const std::wstring& str,
            const D2D1_COLOR_F& textColor, const D2D1_COLOR_F& bkColor,
            WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind, long long UserData = 0)
        {
            D2D_RECT_F rect = { x, y, x + w, y + h };
            SetRect(rect);
            m_CheckRect.SetButtonCallback(callback);
            m_ShowText.SetColor(textColor, TargetWind->GetD2DTargetP());
            m_ShowText.SetShowText(str);
            m_ShowText.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_ShowText.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            m_Rectangle.SetColor(bkColor, TargetWind->GetD2DTargetP());
            m_Rectangle.FillRect();
            if (UserData != 0)
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
        const LONG64& GetUserData() const
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
        void Draw(MainWind_D2D* d2dWind, const D2D1_RECT_F& rect)
        {
            Draw(d2dWind->GetD2DTargetP(), rect);
        }
        void Draw(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& rect)
        {
            m_Rectangle.Draw(d2dRenderTarget, rect);
            m_ShowText.Draw(d2dRenderTarget, rect);
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
        void SetShowText(const std::wstring& str)
        {
            m_ShowText.SetShowText(str);
        }
        std::wstring& GetShowString()
        {
            return m_ShowText.GetShowText();
        }
        const std::wstring& GetShowString() const
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
        D2D1_COLOR_F GetTextColor() const
        {
            return m_ShowText.GetColor();
        }
        void SetBackgroundColor(const D2D1_COLOR_F& color, MainWind_D2D* wind)
        {
            SetBackgroundColor(color, wind->GetD2DTargetP());
        }
        void SetBackgroundColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* wind)
        {
            m_Rectangle.SetColor(color, wind);
        }
        D2D1_COLOR_F GetBackgroundColor() const
        {
            return m_Rectangle.GetColor();
        }
        D2D1_RECT_F GetShowRect() const
        {
            return m_CheckRect.GetRectangle();
        }
        D2D1_SIZE_F GetShowSize() const
        {
            auto rect = m_CheckRect.GetRectangle();
            return D2D1::SizeF(rect.right - rect.left, rect.bottom - rect.top);
        }
        D2D1_POINT_2F GetShowPosition() const
        {
            auto rect = m_CheckRect.GetRectangle();
            return D2D1::Point2F(rect.left, rect.top);
        }
    };
    /// <summary>
    ///  绘制折线函数图像(背景透明)
    /// </summary>
    class d2dFunctionWind {
        int m_DecimalPlaces;
        float m_Interval;
        float m_Identification;
        D2D1_COLOR_F m_TextColor = D2D1::ColorF(0, 0, 0);

        D2D1_RECT_F m_ShowRect;
        D2D1_RECT_F m_Coordinate;
        ID2D1SolidColorBrush* m_axisColor;
        std::vector<WindElements::d2dText*> m_ticksX;
        std::vector<WindElements::d2dText*> m_ticksY;
        std::map<int, std::pair<std::vector<float>, std::vector<float>>> m_LineList;
        std::map<int, WindElements::d2dFoldLine> m_Data;
        void CoordinateToWindCoordinate(std::vector<float>& xList, std::vector<float>& yList)
        {
            if (m_ticksX.empty() || m_ticksY.empty())
                return;

            float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
            float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
            float xTick = xWide / (m_Coordinate.top - m_Coordinate.left);
            float yTick = yWide / (m_Coordinate.bottom - m_Coordinate.right);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;

            for (auto& i : xList) {
                i = originX + (i - m_Coordinate.left) * xTick;
            }
            for (auto& i : yList) {
                i = originY - (i - m_Coordinate.right) * yTick;
            }
        }
        void CreateAixLineColor(ID2D1RenderTarget* tar)
        {
            SafeRelease(&m_axisColor);
            tar->CreateSolidColorBrush(m_TextColor, &m_axisColor);
        }
        void UpdateCoordinate()
        {
            for (auto& i : m_LineList) {
                auto x = i.second.first;
                auto y = i.second.second;
                CoordinateToWindCoordinate(x, y);
                m_Data[i.first].SetFoldLine(x, y);
            }
        }

    public:
        d2dFunctionWind()
            : m_ShowRect({ 0, 0, 0, 0 })
            , m_Coordinate({ 0, 0, 0, 0 })
            , m_axisColor(nullptr)
            , m_Interval(80)
            , m_Identification(5)
            , m_DecimalPlaces(2)
        {
        }
        ~d2dFunctionWind()
        {
            SafeRelease(&m_axisColor);
            for (auto& P : m_ticksY) {
                SafeReleaseP(&P);
            }
            for (auto& P : m_ticksX) {
                SafeReleaseP(&P);
            }
        }
        // 设置数字坐标轴保留的小数位数
        void SetDecimalPlaces(int dp)
        {
            m_DecimalPlaces = dp;
        }
        // 获取当前数字坐标轴保留的小数位数
        int GetDecimalPlaces() const
        {
            return m_DecimalPlaces;
        }
        // 设置轴线到边框的间距
        void SetBorderSpacing(float bs)
        {
            m_Interval = bs;
        }
        // 设置文本颜色
        void SetTextColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* tar)
        {
            m_TextColor = color;
            for (auto& i : m_ticksX) {
                i->SetColor(color, tar);
            }
            for (auto& i : m_ticksY) {
                i->SetColor(color, tar);
            }
        }
        // 设置坐标轴颜色
        void SetAxisColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* tar)
        {
            SafeRelease(&m_axisColor);
            tar->CreateSolidColorBrush(color, &m_axisColor);
        }
        // 初始化
        void Init(const D2D1_RECT_F& ShowRect, ID2D1RenderTarget* tar)
        {
            m_ShowRect = ShowRect;
            if (!m_axisColor) {
                CreateAixLineColor(tar);
            }
        }
        // 设置显示区域
        void SetShowRect(const D2D1_RECT_F& rect)
        {
            m_ShowRect = rect;
            float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
            float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
            float xTick = xWide / (m_ticksX.size() - 1);
            float yTick = yWide / (m_ticksY.size() - 1);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;
            for (int i = 0; i < m_ticksX.size(); ++i) {
                m_ticksX[i]->SetPosition(originX + i * xTick - m_Interval / 2, originY + m_Identification * 3);
            }
            for (int i = 0; i < m_ticksY.size(); ++i) {
                m_ticksY[i]->SetPosition(m_ShowRect.left, -i * yTick + originY - 10);
            }
            UpdateCoordinate();
        }

        void SetAixsCoordinateX(float minX, float maxX, float tickX, ID2D1RenderTarget* tar)
        {
            m_Coordinate = { minX, maxX, m_Coordinate.right, m_Coordinate.bottom };
            for (auto& P : m_ticksX) {
                SafeReleaseP(&P);
            }
            m_ticksX.clear();
            float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
            float xTick = xWide / (maxX - minX);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;
            for (auto x = minX; x <= maxX; x += tickX) {
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
            m_Coordinate = { m_Coordinate.left, m_Coordinate.top, minY, maxY };
            for (auto& P : m_ticksY) {
                SafeReleaseP(&P);
            }
            m_ticksY.clear();
            float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
            float yTick = yWide / (maxY - minY);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;
            for (auto y = minY; y <= maxY; y += tickY) {
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
            for (auto& P : m_ticksX) {
                SafeReleaseP(&P);
            }
            m_ticksX.clear();
            float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
            float xTick = xWide / (xTickSign.size());
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;

            for (int i = 0; i < xTickSign.size(); ++i) {
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
            for (auto& P : m_ticksY) {
                SafeReleaseP(&P);
            }
            m_ticksY.clear();
            float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
            float yTick = yWide / (yTickSign.size());
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;
            for (int i = 0; i < yTickSign.size(); ++i) {
                WindElements::d2dText* text = new WindElements::d2dText(yTickSign[i].c_str(), m_ShowRect.left, -i * yTick + originY - 10, m_Interval, 20);
                text->SetColor(m_TextColor, tar);
                m_ticksY.push_back(text);
            }
            UpdateCoordinate();
        }
        // 设置坐标轴范围及文本
        void SetCoordinate(float minX, float maxX, float minY, float maxY,
            const std::vector<std::wstring>& xTickSign, const std::vector<std::wstring>& yTickSign, ID2D1RenderTarget* tar)
        {
            m_Coordinate = { minX, maxX, minY, maxY };
            for (auto& P : m_ticksY) {
                SafeReleaseP(&P);
            }
            for (auto& P : m_ticksX) {
                SafeReleaseP(&P);
            }
            m_ticksX.clear();
            m_ticksY.clear();
            float xWide = m_ShowRect.right - m_ShowRect.left - m_Interval * 2;
            float yWide = m_ShowRect.bottom - m_ShowRect.top - m_Interval * 2;
            float xTick = xWide / (xTickSign.size() - 1);
            float yTick = yWide / (yTickSign.size() - 1);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;

            for (int i = 0; i < xTickSign.size(); ++i) {
                WindElements::d2dText* text = new WindElements::d2dText(xTickSign[i].c_str(), originX + i * xTick - m_Interval / 2, originY + m_Identification * 3, m_Interval, 20);
                text->SetColor(m_TextColor, tar);
                m_ticksX.push_back(text);
            }
            for (int i = 0; i < yTickSign.size(); ++i) {
                WindElements::d2dText* text = new WindElements::d2dText(yTickSign[i].c_str(), m_ShowRect.left, -i * yTick + originY - 10, m_Interval, 20);
                text->SetColor(m_TextColor, tar);
                m_ticksY.push_back(text);
            }
            UpdateCoordinate();
        }
        // 以纯数字设置坐标轴
        void SetCoordinate(float minX, float maxX, float tickX, float minY, float maxY, float tickY, ID2D1RenderTarget* tar)
        {
            m_Coordinate = { minX, maxX, minY, maxY };
            for (auto& P : m_ticksY) {
                SafeReleaseP(&P);
            }
            for (auto& P : m_ticksX) {
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
            for (auto x = minX; x <= maxX; x += tickX) {
                std::wstringstream wss;
                wss << std::fixed << std::setprecision(m_DecimalPlaces) << x;
                std::wstring result = wss.str();
                WindElements::d2dText* text = new WindElements::d2dText(result.c_str(), originX + (x - minX) * xTick - m_Interval / 2, originY + m_Identification * 3, m_Interval, 20);
                text->SetColor(m_TextColor, tar);
                m_ticksX.push_back(text);
            }
            for (auto y = minY; y <= maxY; y += tickY) {
                std::wstringstream wss;
                wss << std::fixed << std::setprecision(m_DecimalPlaces) << y;
                std::wstring result = wss.str();
                WindElements::d2dText* text = new WindElements::d2dText(result.c_str(), m_ShowRect.left, -(y - minY) * yTick + originY - 10, m_Interval, 20);
                text->SetColor(m_TextColor, tar);
                m_ticksY.push_back(text);
            }
            UpdateCoordinate();
        }
        // 添加函数到坐标轴
        int AddFunction(const std::vector<float>& xList, const std::vector<float>& yList, int id, const D2D1_COLOR_F& color, ID2D1HwndRenderTarget* tar)
        {
            if (xList.size() != yList.size() || id < 0 || xList.empty() || !tar) {
                std::cout << "大小不匹配" << std::endl;
                return -1;
            }
            if (!m_axisColor) {
                CreateAixLineColor(tar);
            }
            if (m_Coordinate.left == m_Coordinate.top) {
                float xMin = xList.front(), xMax = xList.front(), yMin = yList.front(), yMax = yList.front();
                for (auto&& f : xList) {
                    xMin = std::min(f, xMin);
                    xMax = std::max(f, xMax);
                }
                for (auto&& f : yList) {
                    yMin = std::min(f, yMin);
                    yMax = std::max(f, yMax);
                }
                SetCoordinate(xMin, xMax, std::max((xMax - xMin) / xList.size(), (xMax - xMin) / 5), yMin, yMax, std::max((yMax - yMin) / yList.size(), (yMax - yMin) / 5), tar);
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
        // 移除函数
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
            return true;
            ;
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
            float xTick = xWide / (m_ticksX.size() - 1);
            float yTick = yWide / (m_ticksY.size() - 1);
            float originX = m_ShowRect.left + m_Interval;
            float originY = m_ShowRect.bottom - m_Interval;
            // 画y轴轴线
            auto yTopPoint = D2D1::Point2F(originX, m_ShowRect.top + m_Interval - m_Interval / 2);
            target->DrawLine(
                D2D1::Point2F(originX, m_ShowRect.bottom - m_Interval),
                yTopPoint, m_axisColor);
            // 画箭头
            target->DrawLine(yTopPoint, D2D1::Point2F(yTopPoint.x - 3, yTopPoint.y + 3),
                m_axisColor);
            target->DrawLine(yTopPoint, D2D1::Point2F(yTopPoint.x + 3, yTopPoint.y + 3),
                m_axisColor);
            for (int i = 0; i < m_ticksY.size(); ++i) {
                target->DrawLine(
                    D2D1::Point2F(originX, originY - yTick * i),
                    D2D1::Point2F(originX - m_Identification, originY - yTick * i), m_axisColor);
                m_ticksY[i]->Draw(target);
            }
            // 画x轴线
            auto xTopPoint = D2D1::Point2F(m_ShowRect.right - m_Interval + m_Interval / 2, originY);
            target->DrawLine(
                D2D1::Point2F(m_ShowRect.left + m_Interval, originY),
                xTopPoint, m_axisColor);
            // 画箭头
            target->DrawLine(xTopPoint, D2D1::Point2F(xTopPoint.x - 3, xTopPoint.y - 3),
                m_axisColor);
            target->DrawLine(xTopPoint, D2D1::Point2F(xTopPoint.x - 3, xTopPoint.y + 3),
                m_axisColor);
            for (int i = 0; i < m_ticksX.size(); ++i) {
                target->DrawLine(
                    D2D1::Point2F(originX + xTick * i, originY),
                    D2D1::Point2F(originX + xTick * i, originY + m_Identification), m_axisColor);
                m_ticksX[i]->Draw(target);
            }
            for (auto& fl : m_Data) {
                fl.second.Draw(target);
            }
            target->PopAxisAlignedClip();
        }
    };
    /// <summary>
    /// 虚类，存储需要捕获窗口消息的控件共有的函数，
    /// 不能直接使用
    /// </summary>
    class d2dFocusControl {
    protected:
        bool m_Selected = false;
        WindElements::d2dClickDetection m_Check;
        static void Selected(MainWind* window, LONG64 data)
        {
            d2dFocusControl* control = (d2dFocusControl*)data;
            if (!control)
                return;
            control->Switch(dynamic_cast<MainWind_D2D*>(window));
        }
        virtual void End(MainWind_D2D* window) = 0;
        virtual void Begin(MainWind_D2D* window) = 0;
    public:
        void Switch(MainWind_D2D* window)
        {
            if (!window)
                return;
            auto& systemData = window->GetSystemData();
            if (m_Selected) {
                End(window);
            } else {
                if (systemData != 0) {
                    d2dFocusControl* control = (d2dFocusControl*)systemData;
                    control->Switch(window);
                }
                Begin(window);
            }
            m_Selected = !m_Selected;
            m_Check.Negation(m_Selected);
        }
    };
    /// <summary>
    /// 纯色背景编辑框
    /// </summary>
    class d2dEdit : d2dFocusControl {
    private:
        bool m_OnlyNumber = false;
        bool m_MultiLine = false;
        int m_MaxStrSize = 128;
        int m_CursorPosition;
        D2D1_COLOR_F m_SelectedColor;
        D2D1_COLOR_F m_DefColor;

        int m_TextShift = 0;

        MainWind_D2D* m_CurrentWindow = nullptr;
        WindElements::d2dRectangle m_background;
        WindElements::d2dText m_Text;

        WindElements::d2dFoldLine m_CursorLine;

        WindCallback::CharInputCallback_D2D m_DefKeyCallback;
        WindCallback::EditControlCallback m_EditCallback = nullptr;
        WindCallback::MouseCallback m_DefMouseCallback = nullptr;
        /// <summary>
        /// 字符输入回调
        /// </summary>
        /// <param name="window"></param>
        /// <param name="key"></param>
        /// <param name="vir"></param>
        /// <param name="mode"></param>
        static void CharInput(MainWind_D2D* window, int key, int vir, KeyMode mode)
        {
            d2dEdit* edit = (d2dEdit*)window->GetSystemData();
            if (!edit)
                return;
            edit->CharInput(key, mode);
        }
        /// <summary>
        /// 鼠标点击回调
        /// </summary>
        /// <param name="window"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="v"></param>
        /// <param name="type"></param>
        /// <param name="mode"></param>
        static void MouseHit(MainWind* window, int x, int y, int v, MouseMessageType type, KeyMode mode)
        {
            d2dEdit* edit = (d2dEdit*)window->GetSystemData();
            if (!edit)
                return;
            edit->Mouse(x, y, v, type, mode);
            if (edit->m_DefMouseCallback)
                edit->m_DefMouseCallback(window, x, y, v, type, mode);
        }
        /// <summary>
        /// 鼠标移动光标
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="type"></param>
        /// <param name="mode"></param>
        void Mouse(int x, int y, int v, MouseMessageType type, KeyMode mode)
        {
            switch (type) {
            case Game::MT_LEFT:
                if (mode == KM_DOWN) {
                    auto rect = m_Check.GetRectangle();
                    if (x < rect.left || y < rect.top || x > rect.right || y > rect.bottom) {
                        return;
                    }
                    auto textPos = m_Text.GetPosition();
                    SetCursorPosition(m_Text.GetHitPositionCharPosition(Vector::Vec2(x - textPos.x, y - textPos.y)) + 1);
                    SetCursor();
                }
                break;
            case Game::MT_WHEEL: {
                auto& text = m_Text;
                auto pos = text.GetPosition();
                if (mode == KM_DOWN) {
                    CursorMove(VK_DOWN);
                } else if (mode == KM_UP) {
                    CursorMove(VK_UP);
                }
                SetCursor();
            } break;
            default:
                break;
            }
        }
        /// <summary>
        /// 字符输入
        /// </summary>
        /// <param name="key"></param>
        /// <param name="mode"></param>
        void CharInput(int key, KeyMode mode)
        {
            if (mode != KM_CHAR) {
                if (mode == KM_DOWN)
                    CursorMove(key);
                return;
            }
            auto& str = m_Text.GetShowText();
            switch (key) {
            case VK_BACK: {
                if (str.empty() || m_CursorPosition <= 0)
                    break;
                str.erase(m_CursorPosition - 1, 1);
                SetCursorPosition(m_CursorPosition - 1);
                SetCursor();
                break;
            }
            case '\r': {
                if (!m_MultiLine) {
                    Switch(m_CurrentWindow);
                    break;
                }
                if (str.size() >= m_MaxStrSize)
                    break;
                if (m_OnlyNumber) {
                    if (key < '0' || key > '9') {
                        if (key != '-' && key != '.')
                            break;
                    }
                }
                str.insert(m_CursorPosition, 1, key);
                SetCursorPosition(m_CursorPosition + 1);
                SetCursor();
                if (m_EditCallback) {
                    m_EditCallback(GetBindedWindow(), m_Text.GetShowText(), EditMessage::StringChange);
                }
                break;
            }
            default:
                if (str.size() >= m_MaxStrSize)
                    break;
                if (m_OnlyNumber) {
                    if (key < '0' || key > '9') {
                        if (key != '-' && key != '.')
                            break;
                    }
                }
                str.insert(m_CursorPosition, 1, key);
                SetCursorPosition(m_CursorPosition + 1);
                SetCursor();
                if (m_EditCallback) {
                    m_EditCallback(GetBindedWindow(), m_Text.GetShowText(), EditMessage::StringChange);
                }
                break;
            }
        }
        /// <summary>
        /// 通过按键输入移动光标
        /// </summary>
        /// <param name="key"></param>
        void CursorMove(int key)
        {
            switch (key) {
            case VK_LEFT: {
                if (m_CursorPosition > 0)
                    SetCursorPosition(m_CursorPosition - 1);
                SetCursor();
                break;
            }
            case VK_RIGHT: {
                if (m_CursorPosition < m_Text.GetShowText().size())
                    SetCursorPosition(m_CursorPosition + 1);
                SetCursor();
                break;
            }
            case VK_UP: {
                auto& text = m_Text;
                auto CurrentPos = text.GetStringCharPosition(m_CursorPosition);
                auto fontSize = text.GetTextFontSize();
                CurrentPos.y -= fontSize;
                SetCursorPosition(text.GetHitPositionCharPosition(Vector::Vec2(CurrentPos.x, CurrentPos.y)));
                SetCursor();
                break;
            }
            case VK_DOWN: {
                auto& text = m_Text;
                auto CurrentPos = text.GetStringCharPosition(m_CursorPosition);
                auto fontSize = text.GetTextFontSize();
                CurrentPos.y += fontSize * 1.5;
                SetCursorPosition(text.GetHitPositionCharPosition(Vector::Vec2(CurrentPos.x, CurrentPos.y)));
                SetCursor();
                break;
            }
            default:
                break;
            }
        }
        void End(MainWind_D2D* window) override
        {
            auto& systemData = window->GetSystemData();
            m_background.SetColor(m_DefColor, window->GetD2DTargetP());
            auto* Fptr = CharInput;
            if (window->GetKeyCallback() != Fptr) {
                std::cout << "在控件捕获字符输入过程中对字符回调进行了修改\n这可能会导致未知错误！" << std::endl;
            }
            window->SetKeyCallback(m_DefKeyCallback);
            auto* MFptr = MouseHit;
            if (window->GetMouseCallback() != MFptr) {
                std::cout << "在控件捕获字符输入过程中对鼠标回调进行了修改\n这可能会导致未知错误！" << std::endl;
            }
            window->SetMouseCallback(m_DefMouseCallback);

            systemData = 0;
            m_CursorLine.SetFoldLine({});
            if (m_EditCallback) {
                m_EditCallback(window, m_Text.GetShowText(), EditMessage::LeaveControl);
            }
        }
        void Begin(MainWind_D2D* window) override
        {
            auto& systemData = window->GetSystemData();
            m_background.SetColor(m_SelectedColor, window);
            m_DefKeyCallback = window->GetKeyCallback();
            m_DefMouseCallback = window->GetMouseCallback();
            window->SetKeyCallback(CharInput);
            window->SetMouseCallback(MouseHit);
            systemData = (long long)this;
            SetCursor();
            if (m_EditCallback) {
                m_EditCallback(window, m_Text.GetShowText(), EditMessage::EnterControl);
            }
        }
        /// <summary>
        /// 更新光标的显示
        /// </summary>
        void SetCursor()
        {
            auto& text = m_Text;
            auto pos = text.GetStringCharPosition(m_CursorPosition);
            auto TextPos = m_Text.GetPosition();
            auto fontSize = text.GetTextFontSize() + 5;

            TextPos.x += pos.x;
            TextPos.y += pos.y;
            auto editPostion = m_background.GetPosition();
            auto TextShift = TextPos.y - editPostion.y;
            auto editHeight = m_background.GetShowSize().height;
            if (TextShift < 0) {
                m_TextShift -= TextShift - 1;
                UpdateTextPosition(m_background.GetShowRect());
                return SetCursor();
            } else if (TextShift > editHeight - fontSize) {
                m_TextShift -= TextShift - editHeight + fontSize + 1;
                UpdateTextPosition(m_background.GetShowRect());
                return SetCursor();
            }
            m_CursorLine.SetFoldLine({ TextPos.x, TextPos.x }, { TextPos.y, TextPos.y + fontSize });
        }

        void UpdateTextPosition(const D2D1_RECT_F& rect)
        {
            auto editPos = m_background.GetPosition();
            m_Text.SetShowRect(D2D1::RectF(rect.left, (rect.top + m_TextShift), rect.right, rect.bottom + m_TextShift));
        }

    public:
        d2dEdit()
            : m_DefColor(D2D1::ColorF(0.7f, 0.7f, 0.7f))
            , m_SelectedColor(D2D1::ColorF(0.9f, 0.9f, 0.9f))
            , m_DefKeyCallback(nullptr)
            , m_CursorPosition(0)
        {
            m_Check.SetButtonCallback(Selected);
            m_Check.SetUserData((long long)this);
            m_background.FillRect(true);
        }
        /// <summary>
        /// 是否开启只允许输入数字
        /// 包含数字和“-”“.”
        /// </summary>
        /// <param name="only"></param>
        void OnlyNumber(bool only)
        {
            m_OnlyNumber = only;
        }
        /// <summary>
        /// 是否允许多行输入
        /// </summary>
        /// <param name="ml"></param>
        void MultiLine(bool ml)
        {
            m_MultiLine = ml;
        }
        /// <summary>
        /// 设置最大字符长度
        /// </summary>
        /// <param name="size"></param>
        void SetMaxStringSize(int size)
        {
            m_MaxStrSize = size;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        /// <param name="window"></param>
        /// <param name="ShowText"></param>
        void Init(MainWind_D2D* window, const std::wstring& ShowText = L"")
        {
            m_background.SetColor(m_DefColor, window);
            m_Text.SetShowText(ShowText);
            m_Text.SetColor(D2D1::ColorF(0), window);
            m_Text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            m_Text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            m_CursorLine.SetColor(D2D1::ColorF(0), window->GetD2DTargetP());

            m_CursorLine.SetFoldLine({ D2D1::Point2F(0) });
        }
        /// <summary>
        /// 更改光标位置
        /// </summary>
        /// <param name="CuPos">对应的字符位置，允许小于0表示倒数第几个</param>
        void SetCursorPosition(int CuPos)
        {
            auto size = m_Text.GetShowText().size();
            if (CuPos < 0) {
                m_CursorPosition = 0;
            } else if (CuPos > size) {
                m_CursorPosition = (int)size;
            } else {
                m_CursorPosition = CuPos;
            }
        }
        /// <summary>
        /// 设置当前文本
        /// </summary>
        /// <param name="str"></param>
        void SetText(const std::wstring& str)
        {
            m_Text.SetShowText(str);
            SetCursorPosition((int)str.size());
        }
        /// <summary>
        /// 获取当前文本
        /// </summary>
        /// <returns></returns>
        std::wstring& GetText()
        {
            return m_Text.GetShowText();
        }
        /// <summary>
        /// 获取当前文本
        /// </summary>
        /// <returns></returns>
        const std::wstring& GetText() const
        {
            return m_Text.GetShowText();
        }
        /// <summary>
        /// 设置显示位置
        /// </summary>
        /// <param name="rect"></param>
        void SetShowRect(const D2D1_RECT_F& rect)
        {

            UpdateTextPosition(rect);
            m_background.SetShowRect(rect);
            m_Check.SetRectangle(rect);
        }
        /// <summary>
        /// 与窗口绑定
        /// </summary>
        /// <param name="window"></param>
        void Bind(MainWind_D2D* window)
        {
            window->AddButten(&m_Check);
            m_CurrentWindow = window;
        }
        /// <summary>
        /// 与窗口解绑
        /// </summary>
        void Unbind()
        {
            if (m_CurrentWindow) {
                if (m_Selected) {
                    Switch(m_CurrentWindow);
                }
                m_CurrentWindow->DeleteButten(&m_Check);
            }
        }
        /// <summary>
        /// 绘制
        /// </summary>
        /// <param name="window"></param>
        void Draw(MainWind_D2D& window)
        {
            Draw(&window);
        }
        /// <summary>
        /// 绘制
        /// </summary>
        /// <param name="window"></param>
        void Draw(MainWind_D2D* window)
        {
            auto* tar = window->GetD2DTargetP();
            tar->PushAxisAlignedClip(m_background.GetShowRect(), D2D1_ANTIALIAS_MODE_ALIASED);
            m_background.Draw(window);
            m_Text.Draw(window);
            m_CursorLine.Draw(window);
            tar->PopAxisAlignedClip();
        }
        /// <summary>
        /// 设置文本颜色
        /// </summary>
        /// <param name="color">使用D2D1::ColorF()函数创建</param>
        /// <param name="window"></param>
        void SetTextColor(const D2D1_COLOR_F& color, MainWind_D2D* window)
        {
            m_Text.SetColor(color, window);
        }
        /// <summary>
        /// 设置未选中时背景颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="window"></param>
        void SetDefBKColor(const D2D1_COLOR_F& color, MainWind_D2D* window = nullptr)
        {
            m_DefColor = color;
            if (window)
                if (!m_Selected)
                    m_background.SetColor(color, window);
        }
        /// <summary>
        /// 设置选中后的背景色
        /// </summary>
        /// <param name="color"></param>
        void SetSelectedBKColor(const D2D1_COLOR_F& color, MainWind_D2D* window = nullptr)
        {
            m_SelectedColor = color;
            if (window)
                if (m_Selected)
                    m_background.SetColor(color, window);
        }
        /// <summary>
        /// 设置光标颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="window"></param>
        void SetCursorColor(const D2D1_COLOR_F& color, MainWind_D2D* window)
        {
            m_CursorLine.SetColor(color, window->GetD2DTargetP());
        }
        /// <summary>
        /// 设置水平对齐
        /// </summary>
        /// <param name="mode"></param>
        void SetTextAlignment(DWRITE_TEXT_ALIGNMENT mode)
        {
            m_Text.SetTextAlignment(mode);
        }
        /// <summary>
        /// 设置垂直对齐
        /// </summary>
        /// <param name="mode"></param>
        void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT mode)
        {
            m_Text.SetParagraphAlignment(mode);
        }
        /// <summary>
        /// 设置回调函数
        /// </summary>
        /// <param name="callback"></param>
        void SetCallback(WindCallback::EditControlCallback callback)
        {
            m_EditCallback = callback;
        }
        /// <summary>
        /// 获取当前绑定的窗口
        /// </summary>
        /// <returns>返回窗口指针</returns>
        MainWind_D2D* GetBindedWindow() const
        {
            return m_CurrentWindow;
        }
    };

    class d2dSlider : public d2dFocusControl {
        
        float m_Left, m_Right;
        float m_CurrentNumber = 0;
        int m_NodeCount;

        bool m_vertical = false;
        WindElements::d2dGeometry m_slider;
        WindCallback::MouseCallback m_DefMouseCallback = nullptr;
        static void MouseCallback(MainWind* window,int x,int y,int v,MouseMessageType type ,KeyMode mode)
        {
            d2dSlider* sliderControl = (d2dSlider*)window->GetSystemData();
            if (!sliderControl)
                return;
            sliderControl->Mouse(window, x, y, v, type, mode);
        }
        void Mouse(MainWind* window, int x, int y, int v, MouseMessageType type, KeyMode mode)
        {
            if (GetKeyState(VK_LBUTTON) < 0) {
                if (m_Check.CheckClick(x, y)) {
                    auto rect = m_Check.GetRectangle();
                    if (m_vertical)
                    {
                        m_CurrentNumber = (y - rect.top) * (rect.bottom - rect.top) * (m_Right - m_Left);
                    } else {
                        m_CurrentNumber = (x - rect.left) * (rect.right - rect.left) * (m_Right - m_Left);
                    }
                }
            }
        }
        void End(MainWind_D2D* window)override
        {
            auto currentMouseCallback = window->GetMouseCallback();
            if (currentMouseCallback != MouseCallback) {
                std::cout << "在窗口消息被焦点控件捕获期间修改了消息回调，这可能会导致程序崩溃，请优化逻辑！" << std::endl;
            }
            window->SetMouseCallback(m_DefMouseCallback);
        }
        void Begin(MainWind_D2D* window) override
        {
            m_DefMouseCallback = window->GetMouseCallback();
            window->SetMouseCallback(MouseCallback);
        }
    public:
        d2dSlider(int NodeCount = 10, float LeftNumber = 0, float rightNumber = 1)
            : m_Left(LeftNumber)
            , m_Right(rightNumber)
            , m_NodeCount(NodeCount)
            , m_CurrentNumber(LeftNumber)
        {
        }
        void SetNodeCount(int count)
        {
            m_NodeCount = count;
        }
        void SetNumber(float LeftNumber, float rightNumber)
        {
            m_Left = LeftNumber;
            m_Right = rightNumber;
        }
        /// <summary>
        /// 绘制
        /// </summary>
        /// <param name="window"></param>
        void Draw(MainWind_D2D& window)
        {
            Draw(&window);
        }
        /// <summary>
        /// 绘制
        /// </summary>
        /// <param name="window"></param>
        void Draw(MainWind_D2D* window)
        {
            auto Pos = m_Check.GetRectangle();
            window->DrawLine(Pos.left, Pos.top, Pos.left, Pos.bottom);
            window->DrawLine(Pos.right, Pos.top, Pos.right, Pos.bottom);
            m_slider.Draw(window);
        }
    };
    /// <summary>
    /// 存储一些动画控件共有的函数，不能直接使用
    /// </summary>
    class d2dPictureAnimationBase {
    protected:
        D2D1_RECT_F m_ShowRect;
        float m_CurrentTime;
        float m_SwitchTime;
        int m_CurrentPicture;
        float m_Opacity;

    public:
        d2dPictureAnimationBase()
            : m_CurrentTime(0)
            , m_SwitchTime(1000)
            , m_CurrentPicture(0)
            , m_Opacity(1)
            , m_ShowRect(D2D1::RectF())
        {
        }
        void SetShowPosition(const Vector::Vec2& pos)
        {
            float wDifference = pos.x - m_ShowRect.left;
            float hDifference = pos.y - m_ShowRect.top;
            m_ShowRect.left += wDifference;
            m_ShowRect.top += hDifference;
            m_ShowRect.right += wDifference;
            m_ShowRect.bottom += hDifference;
        }
        void SetShowWide(const Vector::Vec2& wide)
        {
            m_ShowRect.bottom = wide.y + m_ShowRect.top;
            m_ShowRect.right = wide.x + m_ShowRect.left;
        }
        void SetShowRect(const D2D1_RECT_F& rect)
        {
            m_ShowRect = rect;
        }
        /// <summary>
        /// 显示区域的x，y
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetShowPosition(float x, float y)
        {
            float wDifference = x - m_ShowRect.left;
            float hDifference = y - m_ShowRect.top;
            m_ShowRect.left += wDifference;
            m_ShowRect.top += hDifference;
            m_ShowRect.right += wDifference;
            m_ShowRect.bottom += hDifference;
        }
        /// <summary>
        /// 显示区域的宽度与高度
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        void SetShowWide(float width, float height)
        {
            m_ShowRect.bottom = height + m_ShowRect.top;
            m_ShowRect.right = width + m_ShowRect.left;
        }
        D2D1_RECT_F GetShowRect() const
        {
            return m_ShowRect;
        }
        D2D1_SIZE_F GetShowSize() const
        {
            return { m_ShowRect.right - m_ShowRect.left, m_ShowRect.bottom - m_ShowRect.top };
        }

        void SetOpacity(float Opacity)
        {
            m_Opacity = Opacity;
        }

        virtual int GetAnimationSize()const = 0;

        void SetSwitchTime(float second)
        {
            m_SwitchTime = second * 1000;
        }
        virtual void ToPicture(int index, bool ContinueSwitch = true) = 0;
        virtual void ShowNext() = 0;
        virtual void Draw(MainWind_D2D* tar) = 0;
    };
    /// <summary>
    /// 实现对单个图片的关键帧动画播放。
    /// 不负责资源管理，图片资源（d2dPicture元素）手动添加
    /// </summary>
    class d2dPictureAnimation : public d2dPictureAnimationBase {
        WindElements::d2dPicture* m_DataPicture;
        Vector::Vec2 m_StartPosition;
        Vector::Vec2 m_CurrentPosition;
        Vector::Vec2 m_OneWide;
        Vector::Vec2 m_ShiftWide;
        int m_PictureCount;

        std::vector<Vector::Vec2> m_Crops;

    public:
        d2dPictureAnimation()
            : m_DataPicture(nullptr)
            , m_PictureCount(0)
        {
        }
        WindElements::d2dPicture* GetImageData() const
        {
            return m_DataPicture;
        }
        void Init()
        {
            m_CurrentPosition = m_StartPosition;
            m_CurrentPicture = 0;
        }
        void Init(WindElements::d2dPicture* Image, const Vector::Vec2& StartPosition, const Vector::Vec2& pictureWide, const Vector::Vec2& shiftWide, int pictureCount)
        {
            m_PictureCount = pictureCount;
            m_OneWide = pictureWide;
            m_ShiftWide = shiftWide;
            m_StartPosition = StartPosition;
            m_CurrentPosition = m_StartPosition;

            m_DataPicture = Image;

            if (!m_DataPicture)
                return;
            m_Crops.clear();
            m_Crops.push_back(m_CurrentPosition);
            for (int i = 1; i < pictureCount; ++i) {
                m_CurrentPosition.x += m_ShiftWide.x;
                if (m_CurrentPosition.x >= m_DataPicture->GetImageSize().width) {
                    m_CurrentPosition.x = m_StartPosition.x;
                    m_CurrentPosition.y += m_ShiftWide.y;
                }
                m_Crops.push_back(m_CurrentPosition);
            }
        }
        void Init(WindElements::d2dPicture* Image, int wCount, int hCount, int StartX, int StartY, int PictureCount)
        {
            auto imageSize = Image->GetImageSize();
            Vector::Vec2 size = Vector::Vec2(imageSize.width, imageSize.height);
            size.x /= wCount;
            size.y /= hCount;

            Init(Image, Vector::Vec2(size.x * StartX, size.y * StartY), size, size, PictureCount);
        }
        /// <summary>
        /// 跳转到指定帧
        /// </summary>
        /// <param name="index">指定帧</param>
        /// <param name="ContinueSwitch">默认为true，如果为false动画将暂停播放，只有重新设置大于0的切换时间以继续</param>
        void ToPicture(int index, bool ContinueSwitch = true) override
        {
            if (index >= m_PictureCount || index < 0) {
                return;
            }
            m_CurrentPicture = index;
            m_CurrentPosition = m_Crops[m_CurrentPicture];
            if (!ContinueSwitch)
                m_SwitchTime = -1;
        }

        int GetAnimationSize()const override
        {
            return m_PictureCount;
        }

        /// <summary>
        /// 设置一帧的切换时间
        /// 小于0则固定
        /// </summary>
        /// <param name="time">单位秒</param>
        void SetPictureCount(int Count)
        {
            m_PictureCount = Count;
        }
        void SetDataPicture(WindElements::d2dPicture& DataPicture)
        {
            SetDataPicture(&DataPicture);
        }
        void SetDataPicture(WindElements::d2dPicture* DataPicture)
        {
            m_DataPicture = DataPicture;
            Init();
        }

        void SetPicturePos(const Vector::Vec2& pos)
        {
            m_StartPosition = pos;
            Init();
        }
        void SetPictureWide(const Vector::Vec2& wide)
        {
            m_OneWide = wide;
            Init();
        }
        void SetPictureShiftWide(const Vector::Vec2& wide)
        {
            m_ShiftWide = wide;
            Init();
        }

        void ShowNext() override
        {
            if (!m_DataPicture)
                return;
            m_CurrentPicture++;
            if (m_CurrentPicture >= m_PictureCount)
                m_CurrentPicture = 0;
            m_CurrentPosition = m_Crops[m_CurrentPicture];
        }
        void Draw(MainWind_D2D* tar) override
        {
            if (!tar || !m_DataPicture)
                return;
            if (m_SwitchTime > 0) {
                m_CurrentTime += tar->GetPaintIntervalTime();
                while (m_CurrentTime > m_SwitchTime) {
                    ShowNext();
                    m_CurrentTime -= m_SwitchTime;
                }
            }
            Vector::Vec2 pos2 = m_CurrentPosition + m_OneWide;

            m_DataPicture->SetCrop({ m_CurrentPosition.x, m_CurrentPosition.y, pos2.x, pos2.y });
            m_DataPicture->SetShowRect(m_ShowRect);
            m_DataPicture->SetOpacity(m_Opacity);
            m_DataPicture->Draw(tar);
        }
    };
    /// <summary>
    /// 实现对多个图片的关键帧动画播放。
    /// 不负责资源管理，图片资源（d2dPicture元素）手动添加
    /// </summary>
    class d2dPicturesAnimation : public d2dPictureAnimationBase {
        std::vector<WindElements::d2dPicture*> m_DataPicture;

    public:
        d2dPicturesAnimation() { }
        int GetAnimationSize()const override
        {
            return (int)m_DataPicture.size();
        }
        void SetData(std::vector<WindElements::d2dPicture*>& data)
        {
            m_DataPicture = data;
        }
        void AddData(WindElements::d2dPicture* data)
        {
            m_DataPicture.push_back(data);
        }
        void ToPicture(int index, bool switchPicture = true) override
        {
            if (index >= m_DataPicture.size())
                return;
            m_CurrentPicture = index;
            if (!switchPicture) {
                m_SwitchTime = -1;
            }
        }
        WindElements::d2dPicture* GetPicture(int index)
        {
            if (index >= m_DataPicture.size())
                return nullptr;
            return m_DataPicture[index];
        }
        void ShowNext() override
        {
            ++m_CurrentPicture;
            if (m_CurrentPicture >= m_DataPicture.size()) {
                m_CurrentPicture = 0;
            }
        }
        void Draw(MainWind_D2D* tar) override
        {
            if (m_SwitchTime > 0) {
                m_CurrentTime += tar->GetPaintIntervalTime();
                while (m_CurrentTime > m_SwitchTime) {
                    ShowNext();
                    m_CurrentTime -= m_SwitchTime;
                }
            }
            auto picture = GetPicture(m_CurrentPicture);
            if (!picture || !tar) {
                return;
            }
            picture->SetShowRect(m_ShowRect);
            picture->SetOpacity(m_Opacity);
            picture->Draw(tar);
        }
    };

}
}