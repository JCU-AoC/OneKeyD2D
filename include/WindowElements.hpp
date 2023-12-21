#pragma once
#include "D2Dmacro.hpp"
#include <d2d1.h>
#include <iostream>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
namespace Game {
namespace WindElements {
    class d2dElements {
    protected:
        D2D1_RECT_F m_ShowRectangle;
        float m_Angle = 0;
        D2D1_POINT_2F m_RotateCenter = D2D1::Point2F();
        D2D1_MATRIX_3X2_F m_Rotation = D2D1::Matrix3x2F::Rotation(0);
        D2D1_MATRIX_3X2_F originalTransform = D2D1::Matrix3x2F::Rotation(0);
    public:
        d2dElements()
            : m_ShowRectangle({ 0, 0, 0, 0 })
        {
        }
        virtual bool Draw(ID2D1RenderTarget* d2dRenderTarget) = 0;
        virtual bool Draw(MainWind_D2D* d2dWind) = 0;
        /// <summary>
        /// 获取当前显示的位置所在矩形
        /// </summary>
        /// <returns></returns>
        D2D1_RECT_F GetShowRect() const
        {
            return m_ShowRectangle;
        }
        /// <summary>
        /// 获取显示宽度
        /// </summary>
        /// <returns></returns>
        D2D1_SIZE_F GetShowSize() const
        {
            return { m_ShowRectangle.right - m_ShowRectangle.left, m_ShowRectangle.bottom - m_ShowRectangle.top };
        }
        virtual void SetShowRect(const D2D1_RECT_F& rect)
        {
            m_ShowRectangle = rect;
        }
        /// <summary>
        /// 显示区域的x，y
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        virtual void SetPosition(float x, float y)
        {
            float wDifference = x - m_ShowRectangle.left;
            float hDifference = y - m_ShowRectangle.top;
            m_ShowRectangle.left += wDifference;
            m_ShowRectangle.top += hDifference;
            m_ShowRectangle.right += wDifference;
            m_ShowRectangle.bottom += hDifference;
            SetRotate(m_Angle, m_RotateCenter);
        }
        virtual D2D1_POINT_2F GetPosition() const
        {
            return D2D1::Point2F(m_ShowRectangle.left, m_ShowRectangle.top);
        }
        /// <summary>
        /// 显示区域的宽度与高度
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        virtual void SetShowWide(float width, float height)
        {
            m_ShowRectangle.bottom = height + m_ShowRectangle.top;
            m_ShowRectangle.right = width + m_ShowRectangle.left;
        }

        /// <summary>
        /// 让图像绕指定点旋转
        /// </summary>
        /// <param name="angle">旋转角度</param>
        /// <param name="center">取值为0-1，表示显示矩阵中旋转的位置</param>
        void SetRotate(float angle, const D2D1_POINT_2F& center = D2D1::Point2F())
        {
            m_Angle = angle;
            m_RotateCenter = center;
            D2D1_POINT_2F CenterR = center;
            CenterR.x = (m_ShowRectangle.right - m_ShowRectangle.left) * center.x + m_ShowRectangle.left;
            CenterR.y = (m_ShowRectangle.bottom - m_ShowRectangle.top) * center.y + m_ShowRectangle.top;
            m_Rotation = D2D1::Matrix3x2F::Rotation(angle, CenterR);
        }
    };
    class d2dPicture : public d2dElements {

    public:
        d2dPicture()
            : m_Bitmap(nullptr)
            , m_Crop(D2D1::RectF(0, 0, 1, 1))
            , m_Transparency(1.f)
            , m_ImageWide(D2D1::SizeF())
        {
        }
        ~d2dPicture()
        {
            SafeRelease(&m_Bitmap);
        }
        /// <summary>
        /// 是否图片资源
        /// </summary>
        void Release()
        {
            SafeRelease(&m_Bitmap);
        }
        /// <summary>
        /// 从文件中加载图片
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="d2dWind">d2d窗口</param>
        /// <returns></returns>
        bool LoadPictureFromFile(const std::wstring& filePath, MainWind_D2D* d2dWind)
        {
            return LoadPictureFromFile(filePath, d2dWind->GetD2DTargetP());
        }
        /// <summary>
        /// 从文件中加载图片
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="d2dWind">d2d窗口</param>
        /// <returns></returns>
        bool LoadPictureFromFile(const std::wstring& filePath, ID2D1RenderTarget* d2dRenderTarget)
        {
            if (filePath.empty() || !d2dRenderTarget) {
                std::cout << "未指定文件名或窗口未创建" << std::endl;
                return false;
            }
            SafeRelease(&m_Bitmap);
            IWICImagingFactory* pWICFactory = nullptr;
            HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
            if (FAILED(hr)) {
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            IWICBitmapDecoder* pDecoder = nullptr;

            hr = pWICFactory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
            if (FAILED(hr)) {
                SafeRelease(&pWICFactory);
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            IWICBitmapFrameDecode* pFrame = nullptr;
            pDecoder->GetFrame(0, &pFrame);
            if (FAILED(hr)) {
                SafeRelease(&pWICFactory);
                SafeRelease(&pDecoder);
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            IWICFormatConverter* pConverter = nullptr;
            hr = pWICFactory->CreateFormatConverter(&pConverter);
            if (FAILED(hr)) {
                SafeRelease(&pWICFactory);
                SafeRelease(&pDecoder);
                SafeRelease(&pFrame);
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            hr = pConverter->Initialize(
                pFrame,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut);
            if (FAILED(hr)) {
                SafeRelease(&pWICFactory);
                SafeRelease(&pDecoder);
                SafeRelease(&pFrame);
                SafeRelease(&pConverter);
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            hr = d2dRenderTarget->CreateBitmapFromWicBitmap(
                pConverter,
                NULL,
                &m_Bitmap);
            SafeRelease(&pWICFactory);
            SafeRelease(&pDecoder);
            SafeRelease(&pFrame);
            SafeRelease(&pConverter);
            if (FAILED(hr)) {
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return false;
            }
            m_ImageWide = m_Bitmap->GetSize();
            SetShowWide(m_ImageWide.width, m_ImageWide.height);
            m_Crop.right = m_ImageWide.width;
            m_Crop.bottom = m_ImageWide.height;
            return true;
        }
        /// <summary>
        /// 加载像素格式的数据
        /// </summary>
        /// <param name="pData">数据指针</param>
        /// <param name="width">图片宽度</param>
        /// <param name="height">图片高度</param>
        /// <param name="pRenderTarget"></param>
        /// <param name="bitmapProperties">图片数据格式</param>
        /// <returns></returns>
        bool LoadBitmapFromMemory(const unsigned char* pData, UINT width, UINT height, ID2D1RenderTarget* pRenderTarget,
            D2D1_BITMAP_PROPERTIES bitmapProperties = D2D1::BitmapProperties())
        {
            if (!pData) {
                std::cout << "图片加载失败，未指定数据" << std::endl;
                return false;
            }
            SafeRelease(&m_Bitmap);
            HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(width, height), pData, width * 4, &bitmapProperties, &m_Bitmap);

            if (SUCCEEDED(hr)) {
                return true;
            }
            std::cout << "图片加载失败，错误码" << hr << std::endl;
            return false;
        }
        /// <summary>
        /// 加载内存中的jpg，png等格式文件
        /// </summary>
        /// <param name="pData">数据指针</param>
        /// <param name="dataSize">数据长度</param>
        /// <param name="pRenderTarget"></param>
        /// <returns></returns>
        HRESULT LoadImageFromMemory(const unsigned char* pData, UINT dataSize, ID2D1RenderTarget* pRenderTarget)
        {
            // 创建内存流对象
            IWICStream* pStream = nullptr;
            IWICImagingFactory* pWICFactory = nullptr;
            HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
            if (FAILED(hr)) {
                std::cout << "图片加载失败，错误码" << hr << std::endl;
                return S_FALSE;
            }
            hr = pWICFactory->CreateStream(&pStream);
            if (SUCCEEDED(hr)) {
                // 初始化内存流对象
                hr = pStream->InitializeFromMemory(const_cast<unsigned char*>(pData), dataSize);
                if (SUCCEEDED(hr)) {
                    // 创建解码器对象
                    IWICBitmapDecoder* pDecoder = nullptr;
                    hr = pWICFactory->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder);
                    if (SUCCEEDED(hr)) {
                        // 解码第一帧
                        IWICBitmapFrameDecode* pFrame = nullptr;
                        hr = pDecoder->GetFrame(0, &pFrame);
                        if (SUCCEEDED(hr)) {
                            // 转换为格式转换器对象
                            IWICFormatConverter* pConverter = nullptr;
                            hr = pWICFactory->CreateFormatConverter(&pConverter);
                            if (SUCCEEDED(hr)) {
                                hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
                                if (SUCCEEDED(hr)) {
                                    SafeRelease(&m_Bitmap);
                                    hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &m_Bitmap);
                                }
                                pConverter->Release();
                            }
                            pFrame->Release();
                        }
                        pDecoder->Release();
                    }
                }
                pStream->Release();
            }
            SafeRelease(&pWICFactory);
            return hr;
        }
        /// <summary>
        /// 设置不透明度
        /// </summary>
        /// <param name="t"></param>
        void SetOpacity(float t)
        {
            m_Transparency = t;
        }
        /// <summary>
        /// 相对与当前设置的显示大小进行缩放
        /// </summary>
        void Scale(float widthScale, float heightScale)
        {
            auto nowWide = GetShowSize();
            nowWide.width *= widthScale;
            nowWide.height *= heightScale;
            SetShowWide(nowWide.width, nowWide.height);
        }

        /// <summary>
        /// 设置裁切
        /// </summary>
        /// <param name="crop">单位是像素</param>
        void SetCrop(const D2D1_RECT_F& crop)
        {
            m_Crop = crop;
        }
        /// <summary>
        /// 绘制图片
        /// </summary>
        /// <param name="d2dRenderTarget">目标d2d窗口</param>
        /// <returns></returns>
        bool Draw(ID2D1RenderTarget* d2dRenderTarget) override
        {
            if (!d2dRenderTarget || !m_Bitmap)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawBitmap(m_Bitmap, m_ShowRectangle, m_Transparency, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, m_Crop);
            d2dRenderTarget->SetTransform(originalTransform);

            return true;
        }
        /// <summary>
        /// 在目标矩形绘制图片
        /// </summary>
        /// <param name="d2dRenderTarget">目标d2d窗口</param>
        /// <param name="rect">目标矩形</param>
        /// <returns></returns>
        bool DrawInRect(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& rect)
        {
            if (!d2dRenderTarget || !m_Bitmap)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawBitmap(m_Bitmap, rect, m_Transparency, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, m_Crop);
            d2dRenderTarget->SetTransform(originalTransform);

            return true;
        }
        /// <summary>
        /// 绘制目标裁切的图片
        /// </summary>
        /// <param name="d2dRenderTarget">目标d2d窗口</param>
        /// <param name="crop">裁切矩形</param>
        /// <returns></returns>
        bool Draw(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& crop)
        {
            if (!d2dRenderTarget || !m_Bitmap)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawBitmap(m_Bitmap, m_ShowRectangle, m_Transparency, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, crop);
            d2dRenderTarget->SetTransform(originalTransform);

            return true;
        }
        /// <summary>
        /// 在目标窗口绘制图片
        /// </summary>
        /// <param name="d2dWind"></param>
        /// <returns></returns>
        bool Draw(MainWind_D2D* d2dWind) override
        {
            return Draw(d2dWind->GetD2DTargetP());
        }
        /// <summary>
        /// 获取当前裁切矩形
        /// </summary>
        /// <returns></returns>
        D2D1_RECT_F GetCropRect() const
        {
            return m_Crop;
        }
        /// <summary>
        /// 获取图片文件的大小
        /// </summary>
        /// <returns></returns>
        D2D1_SIZE_F GetImageSize() const
        {
            return m_ImageWide;
        }

    private:
        float m_Transparency;
        D2D1_RECT_F m_Crop;
        D2D1_SIZE_F m_ImageWide;

        ID2D1Bitmap* m_Bitmap;
    };
    class d2dText : public d2dElements {
    private:
        std::wstring m_ShowText;

        IDWriteTextFormat* m_TextFormat;
        ID2D1SolidColorBrush* m_Color;

    public:
        d2dText(const std::wstring& showString = L"", float x = 0, float y = 0, float w = 128, float h = 32)
            : m_ShowText(showString)
            , m_Color(nullptr)
        {
            m_TextFormat = nullptr;
            m_ShowRectangle = (D2D1::RectF(x, y, x + w, y + h));
            SetTextFormat(L"仿宋", NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                h, L"");
        }
        ~d2dText()
        {
            SafeRelease(&m_TextFormat);
        }
        /// <summary>
        /// 设置文本颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="window"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, MainWind_D2D* window)
        {
            return SetColor(color, window->GetD2DTargetP());
        }
        /// <summary>
        /// 设置文本颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="renderTarget"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
        {
            if (!renderTarget)
                return false;
            SafeRelease(&m_Color);
            if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
                return true;
            return false;
        }
        /// <summary>
        /// 获取当前文本颜色
        /// </summary>
        /// <returns></returns>
        D2D1_COLOR_F GetColor() const
        {
            if (m_Color)
                return m_Color->GetColor();
            return D2D1::ColorF(0);
        }
        /// <summary>
        /// 设置不透明度
        /// </summary>
        /// <param name="t"></param>
        void SetOpacity(float t)
        {
            if (m_Color)
                m_Color->SetOpacity(t);
        }
        /// <summary>
        /// 获取当前不透明度
        /// </summary>
        /// <returns></returns>
        float GetOpacity() const
        {
            if (m_Color)
                return m_Color->GetOpacity();
            return 0.f;
        }
        /// <summary>
        /// 设置显示的文本
        /// </summary>
        /// <param name="showString"></param>
        void SetShowText(const std::wstring& showString)
        {
            m_ShowText = showString;
        }
        /// <summary>
        /// 获取当前显示的文本
        /// </summary>
        /// <returns></returns>
        const std::wstring& GetShowText() const
        {
            return m_ShowText;
        }
        /// <summary>
        /// 获取当前显示文本的引用
        /// </summary>
        /// <returns></returns>
        std::wstring& GetShowText()
        {
            return m_ShowText;
        }
        /// <summary>
        /// 设置文本水平对齐方式
        /// </summary>
        /// <param name="dta"></param>
        void SetTextAlignment(DWRITE_TEXT_ALIGNMENT dta)
        {
            if (m_TextFormat)
                m_TextFormat->SetTextAlignment(dta);
        }
        /// <summary>
        /// 设置文本垂直对齐方式
        /// </summary>
        /// <param name="dta"></param>
        void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT dta)
        {
            if (m_TextFormat)
                m_TextFormat->SetParagraphAlignment(dta);
        }
        /// <summary>
        /// 获取字体大小
        /// </summary>
        /// <returns></returns>
        float GetTextFontSize() const
        {
            return m_TextFormat->GetFontSize();
        }
        /// <summary>
        /// 设置字体大小
        /// </summary>
        /// <param name="fontSize"></param>
        void SetTextFontSize(float fontSize)
        {
            if (!m_TextFormat || fontSize < 1)
                return;

            WCHAR name[64] = {};
            m_TextFormat->GetFontFamilyName(name, 64);
            SetTextFormat(
                name,
                nullptr,
                m_TextFormat->GetFontWeight(),
                m_TextFormat->GetFontStyle(),
                m_TextFormat->GetFontStretch(),
                fontSize, L"");
            
        }
        /// <summary>
        /// 设置字体样式
        /// </summary>
        /// <param name="fontFamilyName">包含字体系列名称的字符数组</param>
        /// <param name="fontCollection">指向字体集合对象的指针。如果为 NULL，则指示系统字体集合。</param>
        /// <param name="fontWeight">一个值，该值指示此方法创建的文本对象的字体粗细。</param>
        /// <param name="fontStyle">一个值，该值指示此方法创建的文本对象的字体样式。</param>
        /// <param name="fontStretch">一个值，该值指示此方法创建的文本对象的字体拉伸。</param>
        /// <param name="fontSize">字体的逻辑大小，以 DIP（“与设备无关的像素”）为单位。DIP 等于 1/96 英寸。</param>
        /// <param name="localeName">包含区域设置名称的字符数组。</param>
        /// <returns></returns>
        HRESULT SetTextFormat(
            WCHAR const* fontFamilyName,
            IDWriteFontCollection* fontCollection,
            DWRITE_FONT_WEIGHT fontWeight,
            DWRITE_FONT_STYLE fontStyle,
            DWRITE_FONT_STRETCH fontStretch,
            FLOAT fontSize,
            WCHAR const* localeName)
        {
            if (fontSize < 1)
                return S_FALSE;
            bool haveLastTextAlignment = false;
            DWRITE_PARAGRAPH_ALIGNMENT LastParagraphAlignment;
            DWRITE_TEXT_ALIGNMENT LastAlignment;

            if(m_TextFormat)
            {
                LastParagraphAlignment = m_TextFormat->GetParagraphAlignment();
                LastAlignment = m_TextFormat->GetTextAlignment();
                haveLastTextAlignment = true;
            }
            IDWriteFactory* writeFactory = nullptr;
            HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
            if (FAILED(hr)) {
                return hr;
            }
            SafeRelease(&m_TextFormat);
            hr = writeFactory->CreateTextFormat(
                fontFamilyName,
                fontCollection,
                fontWeight,
                fontStyle,
                fontStretch,
                fontSize,
                localeName,
                &m_TextFormat);
            SafeRelease(&writeFactory);
            if(haveLastTextAlignment)
            {
                m_TextFormat->SetParagraphAlignment(LastParagraphAlignment);
                m_TextFormat->SetTextAlignment(LastAlignment);
            }
            return S_OK;
        }
        /// <summary>
        /// 获取指定位置的字符索引
        /// </summary>
        /// <param name="pos">相对位置</param>
        /// <returns></returns>
        int GetHitPositionCharPosition(const Vector::Vec2& pos) const
        {
            IDWriteFactory* writeFactory = nullptr;
            HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
            if (FAILED(hr)) {
                return -1;
            }
            IDWriteTextLayout* dwriteTextLayout;
            hr = writeFactory->CreateTextLayout(
                m_ShowText.c_str(),
                (UINT32)m_ShowText.size(),
                m_TextFormat,
                m_ShowRectangle.right - m_ShowRectangle.left,
                m_ShowRectangle.bottom - m_ShowRectangle.top,
                &dwriteTextLayout);
            if (FAILED(hr)) {
                SafeRelease(&writeFactory);
                return -1;
            }
            DWRITE_HIT_TEST_METRICS hitTestMetrics;
            BOOL isTrailingHit, isInside;
            hr = dwriteTextLayout->HitTestPoint(
                pos.x,
                pos.y,
                &isTrailingHit,
                &isInside,
                &hitTestMetrics);
            if (FAILED(hr)) {
                SafeRelease(&dwriteTextLayout);
                SafeRelease(&writeFactory);
                return -1;
            }
            SafeRelease(&dwriteTextLayout);
            SafeRelease(&writeFactory);
            return hitTestMetrics.textPosition;
        }
        /// <summary>
        /// 获取指定字符索引处的位置
        /// </summary>
        /// <param name="CharIndex">第几个字符</param>
        /// <returns></returns>
        D2D1_POINT_2F GetStringCharPosition(int CharIndex) const
        {
            IDWriteFactory* writeFactory = nullptr;
            HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
            if (FAILED(hr)) {
                return { -1, -1 };
            }
            IDWriteTextLayout* dwriteTextLayout;
            hr = writeFactory->CreateTextLayout(
                m_ShowText.c_str(),
                (UINT32)m_ShowText.size(),
                m_TextFormat,
                m_ShowRectangle.right - m_ShowRectangle.left,
                m_ShowRectangle.bottom - m_ShowRectangle.top,
                &dwriteTextLayout);
            if (FAILED(hr)) {
                SafeRelease(&writeFactory);
                return { -1, -1 };
            }
            DWRITE_HIT_TEST_METRICS hitTestMetrics;
            FLOAT charX, charY;
            hr = dwriteTextLayout->HitTestTextPosition(
                CharIndex,
                FALSE,
                &charX,
                &charY,
                &hitTestMetrics);
            if (FAILED(hr)) {
                SafeRelease(&dwriteTextLayout);
                SafeRelease(&writeFactory);
                return { -1, -1 };
            }
            SafeRelease(&dwriteTextLayout);
            SafeRelease(&writeFactory);
            return { charX, charY };
        }
        bool Draw(MainWind_D2D* d2dWind, const D2D1_RECT_F& rect)
        {
            return Draw(d2dWind->GetD2DTargetP(), rect);
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& rect)
        {
            if (!d2dRenderTarget || !m_Color)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawText(m_ShowText.c_str(), (UINT32)m_ShowText.size(), m_TextFormat, &rect, m_Color);
            d2dRenderTarget->SetTransform(originalTransform);
            return true;
        }
        bool Draw(MainWind_D2D* d2dWind)
        {
            return Draw(d2dWind->GetD2DTargetP());
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget)
        {
            if (!d2dRenderTarget || !m_Color)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawText(m_ShowText.c_str(), (UINT32)m_ShowText.size(), m_TextFormat, &m_ShowRectangle, m_Color);
            d2dRenderTarget->SetTransform(originalTransform);
            return true;
        }
    };
    class d2dRectangle : public d2dElements {

        ID2D1SolidColorBrush* m_Color;
        D2D1_ROUNDED_RECT m_Rect;

        float m_PenWide;
        bool m_Fill;
        bool m_Round;

    public:
        d2dRectangle()
            : m_Color(nullptr)
            , m_Fill(false)
            , m_Round(false)
            , m_Rect(D2D1::RoundedRect(D2D1::RectF(), 0, 0))
            , m_PenWide(1)
        {
        }
        ~d2dRectangle()
        {
            SafeRelease(&m_Color);
        }
        /// <summary>
        /// 设置是否填充
        /// </summary>
        /// <param name="fill"></param>
        void FillRect(bool fill = true)
        {
            m_Fill = fill;
        }
        /// <summary>
        /// 设置是否存在圆角
        /// </summary>
        /// <param name="round"></param>
        void Round(bool round = true)
        {
            m_Round = round;
        }
        /// <summary>
        /// 设置圆角半径
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetRound(float x, float y)
        {
            m_Rect.radiusX = x;
            m_Rect.radiusY = y;
        }
        /// <summary>
        /// 设置显示的矩形
        /// </summary>
        /// <param name="rect"></param>
        void SetShowRect(const D2D1_RECT_F& rect) override
        {
            m_ShowRectangle = rect;
            m_Rect.rect = rect;
        }
        /// <summary>
        /// 显示区域的x，y
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetPosition(float x, float y) override
        {
            float wDifference = x - m_ShowRectangle.left;
            float hDifference = y - m_ShowRectangle.top;
            m_ShowRectangle.left += wDifference;
            m_ShowRectangle.top += hDifference;
            m_ShowRectangle.right += wDifference;
            m_ShowRectangle.bottom += hDifference;
            m_Rect.rect = m_ShowRectangle;
            SetRotate(m_Angle, m_RotateCenter);
        }
        /// <summary>
        /// 显示区域的宽度与高度
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        void SetShowWide(float width, float height) override
        {
            m_ShowRectangle.bottom = height + m_ShowRectangle.top;
            m_ShowRectangle.right = width + m_ShowRectangle.left;
            m_Rect.rect = m_ShowRectangle;
        }
        /// <summary>
        /// 设置画笔宽度
        /// </summary>
        /// <param name="penWide"></param>
        void SetUnfillRectWide(float penWide)
        {
            m_PenWide = penWide;
        }
        /// <summary>
        /// 获取显示位置所在的矩形
        /// </summary>
        /// <returns></returns>
        const D2D1_RECT_F& GetShowRect() const
        {
            return m_ShowRectangle;
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="window"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, MainWind_D2D* window)
        {
            return SetColor(color, window->GetD2DTargetP());
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="renderTarget"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
        {
            if (!renderTarget)
                return false;
            SafeRelease(&m_Color);
            if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
                return true;
            return false;
        }
        /// <summary>
        /// 获取颜色
        /// </summary>
        /// <returns></returns>
        D2D1_COLOR_F GetColor() const
        {
            return m_Color->GetColor();
        }
        /// <summary>
        /// 设置不透明度
        /// </summary>
        /// <param name="t"></param>
        void SetOpacity(float t)
        {
            if (m_Color)
                m_Color->SetOpacity(t);
        }
        /// <summary>
        /// 获取不透明度
        /// </summary>
        /// <returns></returns>
        float GetOpacity() const
        {
            if (m_Color)
                return m_Color->GetOpacity();
            return 0.f;
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget) override
        {
            if (!d2dRenderTarget || !m_Color)
                return false;

            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(originalTransform * m_Rotation);
            if (m_Fill)
                if (m_Round)
                    d2dRenderTarget->FillRoundedRectangle(m_Rect, m_Color);
                else
                    d2dRenderTarget->FillRectangle(m_ShowRectangle, m_Color);
            else if (m_Round)
                d2dRenderTarget->DrawRoundedRectangle(m_Rect, m_Color);
            else
                d2dRenderTarget->DrawRectangle(m_ShowRectangle, m_Color, m_PenWide);
            d2dRenderTarget->SetTransform(originalTransform);

            return true;
        }
        bool Draw(MainWind_D2D* d2dWind) override
        {
            return Draw(d2dWind->GetD2DTargetP());
        }

        bool Draw(MainWind_D2D* d2dWind, const D2D1_RECT_F& rect)
        {
            return Draw(d2dWind->GetD2DTargetP(), rect);
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget, const D2D1_RECT_F& rect)
        {
            if (!d2dRenderTarget || !m_Color)
                return false;

            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            if (m_Fill)
                d2dRenderTarget->FillRectangle(rect, m_Color);
            else
                d2dRenderTarget->DrawRectangle(rect, m_Color, m_PenWide);
            d2dRenderTarget->SetTransform(originalTransform);

            return true;
        }
    };
    class d2dFoldLine : public d2dElements {
        std::vector<D2D1_POINT_2F> m_DrawData;
        std::vector<D2D1_POINT_2F> m_rawData;
        ID2D1SolidColorBrush* m_Color;
        ID2D1StrokeStyle* m_PenStyle;
        float m_LineWide;

    public:
        d2dFoldLine()
            : m_Color(nullptr)
            , m_LineWide(1.f)
            , m_PenStyle(nullptr)
        {
        }
        ~d2dFoldLine()
        {
            SafeRelease(&m_Color);
            SafeRelease(&m_PenStyle);
        }
        d2dFoldLine(const std::vector<D2D1_POINT_2F>& data, const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
            : m_LineWide(1.f)
            , m_PenStyle(nullptr)
        {
            SetColor(color, renderTarget);
            SetFoldLine(data);
        }
        /// <summary>
        /// 设置绘制位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetPosition(float x, float y) override
        {
            float wDifference = x - m_ShowRectangle.left;
            float hDifference = y - m_ShowRectangle.top;
            m_ShowRectangle.left += wDifference;
            m_ShowRectangle.top += hDifference;
            m_ShowRectangle.right += wDifference;
            m_ShowRectangle.bottom += hDifference;
            SetRotate(m_Angle, m_RotateCenter);
            SetFoldLine(m_rawData);
        }
        /// <summary>
        /// 设置线宽
        /// </summary>
        /// <param name="wide"></param>
        void SetLineWide(float wide)
        {
            m_LineWide = wide;
        }
        /// <summary>
        /// 获取线宽
        /// </summary>
        /// <returns></returns>
        float GetLineWide() const
        {
            return m_LineWide;
        }
        /// <summary>
        /// 设置要绘制的直线
        /// </summary>
        /// <param name="xList">直线的x数组</param>
        /// <param name="yList">直线的y数组</param>
        /// <returns></returns>
        bool SetFoldLine(const std::vector<float>& xList, const std::vector<float>& yList)
        {
            if (xList.size() != yList.size()) {
                std::cout << "列表数量不匹配" << std::endl;
                return false;
            }
            std::vector<D2D1_POINT_2F> data(xList.size());
            for (size_t p = 0; p < data.size(); ++p) {
                data[p] = D2D1::Point2F(xList[p], yList[p]);
            }
            SetFoldLine(data);
            return true;
        }
        /// <summary>
        /// 设置要绘制的直线坐标
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        bool SetFoldLine(const std::vector<D2D1_POINT_2F>& data)
        {
            m_rawData = data;
            auto size = m_rawData.size();
            m_DrawData.resize(size);
            auto position = GetPosition();
            for (size_t p = 0; p < size; ++p) {
                auto rd = m_rawData[p];
                m_DrawData[p] = D2D1::Point2F(rd.x + position.x, rd.y + position.y);
            }
            return true;
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="window"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, MainWind_D2D* window)
        {
            return SetColor(color, window->GetD2DTargetP());
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="renderTarget"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
        {
            if (!renderTarget)
                return false;
            SafeRelease(&m_Color);
            if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
                return true;
            return false;
        }
        /// <summary>
        /// 设置画笔
        /// </summary>
        /// <param name="window">d2d窗口</param>
        /// <param name="color">颜色</param>
        /// <param name="penStyle">画笔样式</param>
        /// <param name="LineWidth">线宽</param>
        /// <returns></returns>
        bool SetPen(MainWind_D2D* window, const D2D1::ColorF& color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1)
        {
            D2D1_STROKE_STYLE_PROPERTIES d2dPenStyle = D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_ROUND,
                D2D1_LINE_JOIN_MITER,
                10.0f,
                D2D1_DASH_STYLE_CUSTOM,
                0.0f);
            switch (penStyle) {
            case Game::PenStyle::DashedLine: {
                std::vector<float> point = { 5, 3 };
                if (!SetPenStyle(window, d2dPenStyle, point)) {
                    return false;
                }
            } break;
            case Game::PenStyle::DotLine: {
                std::vector<float> point = { 1, 3 };
                if (!SetPenStyle(window, d2dPenStyle, point)) {
                    return false;
                }
            } break;
            case Game::PenStyle::DashDot: {
                std::vector<float> point = { 10, 5, 1, 5 };
                if (!SetPenStyle(window, d2dPenStyle, point)) {
                    return false;
                }
            } break;
            case Game::PenStyle::DashDotDot: {
                std::vector<float> point = { 10, 5, 1, 5, 1, 5 };
                if (!SetPenStyle(window, d2dPenStyle, point)) {
                    return false;
                }
            } break;
            case Game::PenStyle::NullLine:
            case Game::PenStyle::InsideFrame:
            case Game::PenStyle::SolidLine: {
                SafeRelease(&m_PenStyle);
            } break;
            default:
                return false;
                break;
            }

            m_LineWide = (float)LineWidth;
            if (!SetColor(color, window))
                return false;
            return true;
        }
        /// <summary>
        /// 设置画笔样式
        /// </summary>
        /// <param name="window">d2d窗口</param>
        /// <param name="penStyle">画笔样式</param>
        /// <param name="dashes">样式的详细参数</param>
        /// <returns></returns>
        bool SetPenStyle(MainWind_D2D* window, const D2D1_STROKE_STYLE_PROPERTIES& penStyle, const std::vector<float>& dashes)
        {
            return SetPenStyle(window->GetD2DFactoryP(), penStyle, dashes);
        }
        /// <summary>
        /// 设置画笔样式
        /// </summary>
        /// <param name="factory">d2d工厂</param>
        /// <param name="penStyle">画笔样式</param>
        /// <param name="dashes">样式的详细参数</param>
        /// <returns></returns>
        bool SetPenStyle(ID2D1Factory* factory, const D2D1_STROKE_STYLE_PROPERTIES& penStyle, const std::vector<float>& dashes)
        {
            SafeRelease(&m_PenStyle);
            auto hr = factory->CreateStrokeStyle(penStyle,
                dashes.data(),
                dashes.size(),
                &m_PenStyle);
            if (FAILED(hr))
                return false;
            return true;
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget) override
        {
            if (!d2dRenderTarget || !m_Color || m_DrawData.size() < 2)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            for (size_t p = 0; p < m_DrawData.size() - 1; ++p) {
                if (m_PenStyle)
                    d2dRenderTarget->DrawLine(m_DrawData[p], m_DrawData[p + 1], m_Color, m_LineWide, m_PenStyle);
                else
                    d2dRenderTarget->DrawLine(m_DrawData[p], m_DrawData[p + 1], m_Color, m_LineWide);
            }
            d2dRenderTarget->SetTransform(originalTransform);
            return true;
        }
        bool Draw(MainWind_D2D* d2dWind) override
        {
            return Draw(d2dWind->GetD2DTargetP());
        }
    };
    // 绘制几何图形/多边形
    class d2dGeometry : public d2dElements {
        MainWind_D2D* m_window = nullptr;
        ID2D1SolidColorBrush* m_Color;
        ID2D1SolidColorBrush* m_FillColor = nullptr;
        ID2D1PathGeometry* pGeometry;
        ID2D1GeometrySink* pSink;
        bool m_Fill;
        std::vector<D2D1_POINT_2F> m_Path;
        std::vector<D2D1_POINT_2F> m_WindowPath;
        bool SetPath(D2D1_FILL_MODE FillMode)
        {
            if (!m_window)
                return false;
            SafeRelease(&pGeometry);
            SafeRelease(&pSink);
            auto hr = m_window->GetD2DFactoryP()->CreatePathGeometry(&pGeometry);
            if (SUCCEEDED(hr)) {
                hr = pGeometry->Open(&pSink);
                if (SUCCEEDED(hr)) {
                    pSink->SetFillMode(FillMode);
                    pSink->BeginFigure(m_WindowPath.front(), D2D1_FIGURE_BEGIN_FILLED);
                    pSink->AddLines(&m_WindowPath[1], m_WindowPath.size() - 1);
                    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    pSink->Close();

                    return true;
                } else
                    std::cout << hr << std::endl;
            } else
                std::cout << hr << std::endl;
            return false;
        }

    public:
        d2dGeometry()
            : m_Color(nullptr)
            , pSink(nullptr)
            , pGeometry(nullptr)
            , m_Fill(false)
        {
        }
        ~d2dGeometry()
        {
            SafeRelease(&m_Color);
            SafeRelease(&pGeometry);
            SafeRelease(&pSink);
            SafeRelease(&m_FillColor);
        }
        /// <summary>
        /// 设置显示位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetPosition(float x, float y) override
        {
            float wDifference = x - m_ShowRectangle.left;
            float hDifference = y - m_ShowRectangle.top;
            m_ShowRectangle.left += wDifference;
            m_ShowRectangle.top += hDifference;
            m_ShowRectangle.right += wDifference;
            m_ShowRectangle.bottom += hDifference;
            SetRotate(m_Angle, m_RotateCenter);
            SetPath(m_window, m_Path);
        }
        /// <summary>
        /// 设置是否填充
        /// </summary>
        /// <param name="fill">是否填充</param>
        /// <param name="window">d2d窗口</param>
        /// <param name="color">填充颜色</param>
        void Fill(bool fill = true, MainWind_D2D* window = nullptr, const D2D1::ColorF& color = D2D1::ColorF::White)
        {
            m_Fill = fill;
            if (window) {
                SafeRelease(&m_FillColor);
                window->GetD2DTargetP()->CreateSolidColorBrush(color, &m_FillColor);
            }
        }
        /// <summary>
        /// 获取新的路径
        /// </summary>
        /// <returns></returns>
        ID2D1GeometrySink* GetNewSink()
        {
            SafeRelease(&pSink);
            if (!pGeometry)
                return nullptr;
            pGeometry->Open(&pSink);
            return pSink;
        }
        /// <summary>
        /// 获取当前路径
        /// </summary>
        /// <returns></returns>
        ID2D1GeometrySink* GetSink()
        {
            return pSink;
        }
        /// <summary>
        /// 设置路径
        /// </summary>
        /// <param name="window">d2d窗口</param>
        /// <param name="points">路径数组</param>
        /// <param name="FillMode">填充模式</param>
        /// <returns></returns>
        bool SetPath(MainWind_D2D* window, std::vector<D2D1_POINT_2F>& points,
            D2D1_FILL_MODE FillMode = D2D1_FILL_MODE::D2D1_FILL_MODE_ALTERNATE)
        {
            if (!window)
                return false;
            m_window = window;
            auto pSize = points.size();
            m_Path = points;
            m_WindowPath.resize(pSize);
            auto ShowPos = GetPosition();
            for (int i = 0; i < pSize; ++i) {
                auto& point = points[i];
                m_WindowPath[i] = D2D1::Point2F(point.x + ShowPos.x, point.y + ShowPos.y);
            }
            SetPath(FillMode);
            return true;
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color">目标颜色</param>
        /// <param name="renderTarget">d2d窗口目标</param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
        {
            if (!renderTarget)
                return false;
            SafeRelease(&m_Color);
            if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
                return true;
            return false;
        }
        bool Draw(MainWind_D2D* wind)
        {
            return Draw(wind->GetD2DTargetP());
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget) override
        {
            if (!m_Color || !pGeometry || !d2dRenderTarget)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            d2dRenderTarget->DrawGeometry(pGeometry, m_Color);
            if (m_Fill) {
                if (m_FillColor)
                    d2dRenderTarget->FillGeometry(pGeometry, m_FillColor);
                else
                    d2dRenderTarget->FillGeometry(pGeometry, m_Color);
            }
            d2dRenderTarget->SetTransform(originalTransform);
            return true;
        }
    };

    class d2dElliptic : public d2dElements {
        D2D1_ELLIPSE m_Ellipse;
        ID2D1SolidColorBrush* m_Color;
        float m_Wide;
        bool m_Fill;

    public:
        d2dElliptic()
            : m_Color(nullptr)
            , m_Ellipse({ { 0, 0 }, 0, 0 })
            , m_Wide(1)
            , m_Fill(false)
        {
        }
        ~d2dElliptic()
        {
            SafeRelease(&m_Color);
        }
        /// <summary>
        /// 设置显示位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetPosition(float x, float y) override
        {
            float wDifference = x - m_ShowRectangle.left;
            float hDifference = y - m_ShowRectangle.top;
            m_ShowRectangle.left += wDifference;
            m_ShowRectangle.top += hDifference;
            m_ShowRectangle.right += wDifference;
            m_ShowRectangle.bottom += hDifference;
            m_Ellipse.point.x = (m_ShowRectangle.left + m_ShowRectangle.right) * 0.5f;
            m_Ellipse.point.y = (m_ShowRectangle.top + m_ShowRectangle.bottom) * 0.5f;
            SetRotate(m_Angle, m_RotateCenter);
        }
        /// <summary>
        /// 设置显示宽度
        /// </summary>
        /// <param name="w"></param>
        /// <param name="h"></param>
        void SetShowWide(float w, float h) override
        {
            auto pos = GetPosition();
            SetShowRect(D2D1::RectF(pos.x, pos.y, pos.x + w, pos.y + h));
        }
        /// <summary>
        /// 设置显示所在位置的矩形
        /// </summary>
        /// <param name="rect"></param>
        void SetShowRect(const D2D1_RECT_F& rect) override
        {
            m_ShowRectangle = rect;
            auto size = GetShowSize();
            m_Ellipse.point.x = m_ShowRectangle.left + size.width * 0.5f;
            m_Ellipse.point.y = m_ShowRectangle.top + size.height * 0.5f;
            m_Ellipse.radiusX = size.width * 0.5f;
            m_Ellipse.radiusY = size.height * 0.5f;
        }
        /// <summary>
        /// 所在椭圆中心点位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetEllipticalPosition(float x, float y)
        {
            auto size = GetShowSize();
            size.width *= 0.5;
            size.height *= 0.5;
            SetShowRect(D2D1::RectF(x - size.width, y - size.height, x + size.width, y + size.height));
        }
        /// <summary>
        /// 设置半径
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        void SetRadius(float x, float y)
        {
            auto pos = GetPosition();
            SetShowRect(D2D1::RectF(pos.x - x, pos.y - y, pos.x + x, pos.y + y));
        }
        /// <summary>
        /// 设置颜色
        /// </summary>
        /// <param name="color"></param>
        /// <param name="renderTarget"></param>
        /// <returns></returns>
        bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
        {
            if (!renderTarget)
                return false;
            SafeRelease(&m_Color);
            if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
                return true;
            return false;
        }
        /// <summary>
        /// 设置是否填充
        /// </summary>
        /// <param name="fill"></param>
        void Fill(bool fill = true)
        {
            m_Fill = fill;
        }
        /// <summary>
        /// 设置线宽
        /// </summary>
        /// <param name="wide"></param>
        void SetWide(float wide)
        {
            m_Wide = wide;
        }

        bool Draw(MainWind_D2D* wind)
        {
            return Draw(wind->GetD2DTargetP());
        }
        bool Draw(ID2D1RenderTarget* d2dRenderTarget) override
        {
            if (!m_Color || !d2dRenderTarget)
                return false;
            d2dRenderTarget->GetTransform(&originalTransform);
            d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
            if (m_Fill)
                d2dRenderTarget->FillEllipse(m_Ellipse, m_Color);
            else
                d2dRenderTarget->DrawEllipse(m_Ellipse, m_Color, m_Wide);
            d2dRenderTarget->SetTransform(originalTransform);
            return true;
        }
    };
}

}