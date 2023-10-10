#pragma once
#include<d2d1.h>
#include<wincodec.h>
#include"D2Dmacro.hpp"
#include <iostream>

namespace Game
{
	namespace WindElements
	{
		class d2dElements
		{
		protected:
			D2D1_RECT_F m_ShowRectangle;
		public:
			d2dElements() : m_ShowRectangle({ 0,0,0,0 }) {}
			virtual bool Draw(ID2D1RenderTarget* d2dRenderTarget) = 0;
			virtual bool Draw(MainWind_D2D* d2dWind) = 0;
			D2D1_RECT_F GetShowRect()const
			{
				return m_ShowRectangle;
			}
			D2D1_SIZE_F GetShowSize()const
			{
				return { m_ShowRectangle.right - m_ShowRectangle.left,m_ShowRectangle.bottom - m_ShowRectangle.top };
			}
			void SetShowRect(const D2D1_RECT_F& rect)
			{
				m_ShowRectangle = rect;
			}
			void SetPosition(float x, float y)
			{
				float wDifference = x - m_ShowRectangle.left;
				float hDifference = y - m_ShowRectangle.top;
				m_ShowRectangle.left += wDifference;
				m_ShowRectangle.top += hDifference;
				m_ShowRectangle.right += wDifference;
				m_ShowRectangle.bottom += hDifference;
			}
			void SetShowWide(float width, float height)
			{
				m_ShowRectangle.bottom = height + m_ShowRectangle.top;
				m_ShowRectangle.right = width + m_ShowRectangle.left;
			}
		};
		class d2dPicture : public d2dElements
		{

		public:
			d2dPicture() :m_Bitmap(nullptr), m_Crop({ 0,0,1,1 }), m_Transparency(1.f)
			{
				m_Rotation = D2D1::Matrix3x2F::Rotation(0);
			}
			~d2dPicture()
			{
				SafeRelease(&m_Bitmap);
			}
			void Release()
			{
				SafeRelease(&m_Bitmap);
			}
			bool LoadPictureFromFile(const std::wstring& filePath, MainWind_D2D* d2dWind)
			{
				return LoadPictureFromFile(filePath, d2dWind->GetD2DTargetP());
			}
			bool LoadPictureFromFile(const std::wstring& filePath, ID2D1HwndRenderTarget* d2dRenderTarget)
			{
				if (filePath.empty() || !d2dRenderTarget)
					return false;
				SafeRelease(&m_Bitmap);
				IWICImagingFactory* pWICFactory = nullptr;
				HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
				if (FAILED(hr))
				{
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				IWICBitmapDecoder* pDecoder = nullptr;
				hr = pWICFactory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
				if (FAILED(hr))
				{
					SafeRelease(&pWICFactory);
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				IWICBitmapFrameDecode* pFrame = nullptr;
				pDecoder->GetFrame(0, &pFrame);
				if (FAILED(hr))
				{
					SafeRelease(&pWICFactory);
					SafeRelease(&pDecoder);
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				IWICFormatConverter* pConverter = nullptr;
				hr = pWICFactory->CreateFormatConverter(&pConverter);
				if (FAILED(hr))
				{
					SafeRelease(&pWICFactory);
					SafeRelease(&pDecoder);
					SafeRelease(&pFrame);
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				hr = pConverter->Initialize(
					pFrame,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);
				if (FAILED(hr))
				{
					SafeRelease(&pWICFactory);
					SafeRelease(&pDecoder);
					SafeRelease(&pFrame);
					SafeRelease(&pConverter);
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				hr = d2dRenderTarget->CreateBitmapFromWicBitmap(
					pConverter,
					NULL,
					&m_Bitmap
				);
				SafeRelease(&pWICFactory);
				SafeRelease(&pDecoder);
				SafeRelease(&pFrame);
				SafeRelease(&pConverter);
				if (FAILED(hr))
				{
					std::cout << "错误码" << hr << std::endl;
					return false;
				}
				m_ImageWide = m_Bitmap->GetSize();
				SetShowWide(m_ImageWide.width,m_ImageWide.height);
				m_Crop.right = m_ImageWide.width;
				m_Crop.bottom = m_ImageWide.height;
				return true;
			}

			void SetTransparency(float t)
			{
				m_Transparency = t;
			}

			/// <summary>
			/// 相对与当前设置的显示大小进行缩放
			/// </summary>
			void Scale(float widthScale,float heightScale)
			{
				auto nowWide = GetShowSize();
				nowWide.width *= widthScale;
				nowWide.height *= heightScale;
				SetShowWide(nowWide.width, nowWide.height);
			}
			/// <summary>
			/// 让图像绕指定点旋转
			/// </summary>
			/// <param name="angle"></param>
			/// <param name="center">取值为0-1，表示显示矩阵中旋转的位置</param>
			void SetRotate(float angle, const D2D1_POINT_2F& center = D2D1::Point2F())
			{
				D2D1_POINT_2F CenterR = center;
				CenterR.x = (m_ShowRectangle.right - m_ShowRectangle.left) * center.x + m_ShowRectangle.left;
				CenterR.y = (m_ShowRectangle.bottom - m_ShowRectangle.top) * center.y + m_ShowRectangle.top;
				m_Rotation = D2D1::Matrix3x2F::Rotation(angle, CenterR);
			}
			void SetCrop(const D2D1_RECT_F& crop)
			{
				m_Crop = crop;
			}
			bool Draw(ID2D1RenderTarget* d2dRenderTarget)override
			{
				if (!d2dRenderTarget || !m_Bitmap)
					return false;
				d2dRenderTarget->GetTransform(&originalTransform);
				d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
				d2dRenderTarget->DrawBitmap(m_Bitmap, m_ShowRectangle, m_Transparency, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, m_Crop);
				d2dRenderTarget->SetTransform(originalTransform);

				return true;
			}
			bool Draw(MainWind_D2D* d2dWind)override
			{
				return Draw(d2dWind->GetD2DTargetP());
			}


			D2D1_RECT_F GetCropRect()const
			{
				return m_Crop;
			}
			D2D1_SIZE_F GetImageRect()const
			{
				return m_ImageWide;
			}
		private:
			float m_Transparency;
			D2D1_RECT_F m_Crop;
			D2D1_SIZE_F m_ImageWide;

			ID2D1Bitmap* m_Bitmap;
			D2D1::Matrix3x2F m_Rotation;
			D2D1_MATRIX_3X2_F originalTransform;
		};
		class d2dText :public d2dElements
		{
		private:

			std::wstring m_ShowText;

			IDWriteTextFormat* m_TextFormat;
			ID2D1SolidColorBrush* m_Color;

			D2D1::Matrix3x2F m_Rotation;
			D2D1_MATRIX_3X2_F originalTransform;
		public:
			d2dText(const std::wstring& showString = L"", float x = 0, float y = 0, float w = 128, float h = 32) :
				m_ShowText(showString),  m_Rotation(D2D1::Matrix3x2F::Rotation(0)),m_Color(nullptr)
			{
				m_TextFormat=nullptr;
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
			bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
			{
				if (!renderTarget)
					return false;
				SafeRelease(&m_Color);
				if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
					return true;
				return false;
			}
			D2D1_COLOR_F&& GetColor()const
			{
				return m_Color->GetColor();
			}
			void SetOpacity(float t)
			{
				if (m_Color)
					m_Color->SetOpacity(t);
			}
			float GetOpacity()const
			{
				if (m_Color)
					return m_Color->GetOpacity();
				return 0.f;
			}
			void SetShowText(const std::wstring& showString)
			{
				m_ShowText = showString;
			}
			const std::wstring& GetShowText()const
			{
				return m_ShowText;
			}
			std::wstring& GetShowText()
			{
				return m_ShowText;
			}
			void SetRotate(float angle, const D2D1_POINT_2F& center = D2D1::Point2F())
			{
				D2D1_POINT_2F CenterR = center;
				CenterR.x = (m_ShowRectangle.right - m_ShowRectangle.left) * center.x + m_ShowRectangle.left;
				CenterR.y = (m_ShowRectangle.bottom - m_ShowRectangle.top) * center.y + m_ShowRectangle.top;
				m_Rotation = D2D1::Matrix3x2F::Rotation(angle, CenterR);
			}
			void SetTextAlignment(DWRITE_TEXT_ALIGNMENT dta)
			{
				if (m_TextFormat)
					m_TextFormat->SetTextAlignment(dta);
			}
			void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT dta)
			{
				if (m_TextFormat)
					m_TextFormat->SetParagraphAlignment(dta);
			}
			void SetTextFontSize(float fsize)
			{
				if (!m_TextFormat || fsize < 1)
					return;
				WCHAR name[64] = {};
				m_TextFormat->GetFontFamilyName(name, 64);
				SetTextFormat(
					name,
					nullptr,
					m_TextFormat->GetFontWeight(),
					m_TextFormat->GetFontStyle(),
					m_TextFormat->GetFontStretch(),
					fsize, L""
				);
			}
			HRESULT SetTextFormat(
				WCHAR const* fontFamilyName,
				IDWriteFontCollection* fontCollection,
				DWRITE_FONT_WEIGHT    fontWeight,
				DWRITE_FONT_STYLE     fontStyle,
				DWRITE_FONT_STRETCH   fontStretch,
				FLOAT                 fontSize,
				WCHAR const* localeName)
			{
				if (fontSize < 1)
					return S_FALSE;
				IDWriteFactory* writeFactory = nullptr;
				HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
				if (FAILED(hr))
				{
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
					&m_TextFormat 
				);
				m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				SafeRelease(&writeFactory);
				return S_OK;
			}

			D2D1_POINT_2F GetStringCharPosition(int CharIndex)const
			{
				IDWriteFactory* writeFactory = nullptr;
				HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
				if (FAILED(hr))
				{
					return { -1, -1 };
				}
				IDWriteTextLayout* dwriteTextLayout;
				hr= writeFactory->CreateTextLayout(
					m_ShowText.c_str(),
					m_ShowText.size(),
					m_TextFormat,
					FLT_MAX,
					FLT_MAX,
					&dwriteTextLayout
				);
				if (FAILED(hr))
				{
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
					&hitTestMetrics
				);
				if (FAILED(hr))
				{
					SafeRelease(&dwriteTextLayout);
					SafeRelease(&writeFactory);
					return { -1, -1 };
				}
				SafeRelease(&dwriteTextLayout);
				SafeRelease(&writeFactory);
				return { charX,charY };
			}
			bool Draw(MainWind_D2D* d2dWind)
			{
				return Draw(d2dWind->GetD2DTargetP());
			}
			bool Draw(ID2D1RenderTarget* d2dRenderTarget)
			{
				if (!d2dRenderTarget||!m_Color)
					return false;
				d2dRenderTarget->PushAxisAlignedClip(m_ShowRectangle, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				d2dRenderTarget->GetTransform(&originalTransform);
				d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
				
				d2dRenderTarget->DrawText(m_ShowText.c_str(), m_ShowText.size(), m_TextFormat, &m_ShowRectangle, m_Color);
				d2dRenderTarget->SetTransform(originalTransform);
				d2dRenderTarget->PopAxisAlignedClip();
				return true;
			}
		};
		class d2dRectangle :d2dElements
		{
			D2D1::Matrix3x2F m_Rotation;
			D2D1_MATRIX_3X2_F originalTransform;

			ID2D1SolidColorBrush* m_Color;

			float m_PenWide;
			bool m_Fill;
		public:
			d2dRectangle() :m_Color(nullptr), m_Fill(false), m_PenWide(1), m_Rotation(D2D1::Matrix3x2F::Rotation(0))
			{

			}
			~d2dRectangle()
			{
				SafeRelease(&m_Color);
			}
			void SetShowRect(const D2D1_RECT_F& rect)
			{
				m_ShowRectangle = rect;
			}
			void FillRect(bool fill = true)
			{
				m_Fill = true;
			}
			void SetUnfillRectWide(float penWide)
			{
				m_PenWide = penWide;
			}
			const D2D1_RECT_F& GetShowRect()const
			{
				return m_ShowRectangle;
			}
			void SetRotate(float angle, const D2D1_POINT_2F& center = D2D1::Point2F())
			{
				D2D1_POINT_2F CenterR = center;
				CenterR.x = (m_ShowRectangle.right - m_ShowRectangle.left) * center.x + m_ShowRectangle.left;
				CenterR.y = (m_ShowRectangle.bottom - m_ShowRectangle.top) * center.y + m_ShowRectangle.top;
				m_Rotation = D2D1::Matrix3x2F::Rotation(angle, CenterR);
			}
			bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
			{
				if (!renderTarget)
					return false;
				SafeRelease(&m_Color);
				if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
					return true;
				return false;
			}
			D2D1_COLOR_F&& GetColor()const
			{
				return m_Color->GetColor();
			}
			void SetOpacity(float t)
			{
				if (m_Color)
					m_Color->SetOpacity(t);
			}
			float GetOpacity()const
			{
				if (m_Color)
					return m_Color->GetOpacity();
				return 0.f;
			}
			bool Draw(ID2D1RenderTarget* d2dRenderTarget)override
			{
				if (!d2dRenderTarget || !m_Color)
					return false;

				d2dRenderTarget->GetTransform(&originalTransform);
				d2dRenderTarget->SetTransform(m_Rotation * originalTransform);
				if (m_Fill)
					d2dRenderTarget->FillRectangle(m_ShowRectangle, m_Color);
				else
					d2dRenderTarget->DrawRectangle(m_ShowRectangle, m_Color, m_PenWide);
				d2dRenderTarget->SetTransform(originalTransform);

				return true;
			}
			bool Draw(MainWind_D2D* d2dWind)override
			{
				return Draw(d2dWind->GetD2DTargetP());
			}
		};
		class d2dFoldLine:public d2dElements
		{
			std::vector<D2D1_POINT_2F>m_data;
			ID2D1SolidColorBrush* m_Color;
			ID2D1StrokeStyle* m_PenStyle;
			float m_LineWide;
		public:
			d2dFoldLine() :m_Color(nullptr), m_LineWide(1.f), m_PenStyle(nullptr)
			{}
			~d2dFoldLine()
			{
				SafeRelease(&m_Color);
				SafeRelease(&m_PenStyle);
			}
			d2dFoldLine(const std::vector<D2D1_POINT_2F>& data, const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget) :m_LineWide(1.f), m_PenStyle(nullptr)
			{
				SetColor(color, renderTarget);
				m_data = data;
			}
			void SetLineWide(float wide)
			{
				m_LineWide = wide;
			}
			float GetLineWide()const
			{
				return m_LineWide;
			}
			bool SetFoldLine(const std::vector<float>& xList, const std::vector<float>& yList)
			{
				if (xList.size() != yList.size())
				{
					std::cout << "列表数量不匹配" << std::endl;
					return false;
				}
				m_data.resize(xList.size());
				for (size_t p = 0; p < m_data.size(); ++p)
				{
					m_data[p] = D2D1::Point2F(xList[p], yList[p]);
				}
				return true;
			}
			bool SetFoldLine(const std::vector<D2D1_POINT_2F>&data)
			{
				m_data = data;
				return true;
			}
			bool SetColor(const D2D1_COLOR_F& color, ID2D1RenderTarget* renderTarget)
			{
				if (!renderTarget)
					return false;
				SafeRelease(&m_Color);
				if (SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &m_Color)))
					return true;
				return false;
			}
			bool SetPenStyle(const D2D1_STROKE_STYLE_PROPERTIES& penStyle, const std::vector<float>dashes)
			{
				SafeRelease(&m_PenStyle);
				ID2D1Factory* d2dFactory = nullptr;
				if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory)))
					return false;
				HRESULT hr;
				if (dashes.empty())
				{
					hr = d2dFactory->CreateStrokeStyle(penStyle,
						nullptr,
						0,
						&m_PenStyle);
				}
				else
				{
					hr = d2dFactory->CreateStrokeStyle(penStyle,
						dashes.data(),
						dashes.size(),
						&m_PenStyle);
				}
				SafeRelease(&d2dFactory);
				if (FAILED(hr))
					return false;
				return true;
			}
			bool Draw(ID2D1RenderTarget* d2dRenderTarget)override
			{
				if (!d2dRenderTarget || !m_Color || m_data.size() < 2)
					return false;
				for (size_t p = 0; p < m_data.size() - 1; ++p)
				{
					if(m_PenStyle)
						d2dRenderTarget->DrawLine(m_data[p], m_data[p + 1], m_Color, m_LineWide,m_PenStyle);
					else
						d2dRenderTarget->DrawLine(m_data[p], m_data[p + 1], m_Color, m_LineWide);
				}
				return true;
			}
			bool Draw(MainWind_D2D* d2dWind)override
			{
				return Draw(d2dWind->GetD2DTargetP());
			}
		};
		//绘制几何图形/多边形
		class d2dGeometry:public d2dElements
		{
			ID2D1SolidColorBrush* m_Color;
			ID2D1PathGeometry* pGeometry;
			ID2D1GeometrySink* pSink;
		public:
			d2dGeometry():m_Color(nullptr),pSink(nullptr),pGeometry(nullptr){}
			~d2dGeometry()
			{
				SafeRelease(&m_Color);
				SafeRelease(&pGeometry);
				SafeRelease(&pSink);
			}
			bool Init(ID2D1Factory* fac)
			{
				if (!fac)
					return false;
				HRESULT hr = fac->CreatePathGeometry(&pGeometry);
				if (FAILED(hr))
					return false;
			}
			ID2D1GeometrySink* GetNewSink()
			{
				SafeRelease(&pSink);
				if (!pGeometry)
					return nullptr;
				pGeometry->Open(&pSink);
				return pSink;
			}
			ID2D1GeometrySink* GetSink()
			{
				return pSink;
			}
			bool SetPath(const std::vector<D2D1_POINT_2F>& points,
				D2D1_FILL_MODE FillMode = D2D1_FILL_MODE::D2D1_FILL_MODE_ALTERNATE)
			{
				SafeRelease(&pSink);
				pGeometry->Open(&pSink);
				pSink->SetFillMode(FillMode);
				pSink->BeginFigure(points.front(), D2D1_FIGURE_BEGIN_FILLED);
				for (UINT32 i = 1; i < points.size(); i++) {
					pSink->AddLine(points[i]);
				}
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				pSink->Close();
			}

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
			bool Draw(ID2D1RenderTarget* wind)override
			{
				if (!m_Color || !pGeometry || !wind)
					return false;
				wind->DrawGeometry(pGeometry, m_Color);
				return true;
			}
		};
	}

}