#pragma once
#include<vector>

#include <set>
#include<map>
namespace Game {
	class Room;
	class RoomManage;
	class RoomObject;


	class Camera2D
	{
	public:

		Vector::Vec2 m_Position;
		Vector::Vec2 m_ShowWide;
		Camera2D(const Vector::Vec2& position = Vector::Vec2(0)) :
			m_Position(position),
			m_ShowWide(16, 9)
		{}
		/// <summary>
		/// 缩放画面相对大小
		/// </summary>
		/// <param name="scale">缩放倍率</param>
		void Scale(float scale)
		{
			m_ShowWide *= scale;
		}
		/// <summary>
		/// 缩放画面相对大小
		/// </summary>
		/// <param name="scaleX">X缩放倍率</param>
		/// <param name="scaleY">Y缩放倍率</param>
		void Scale(float scaleX, float scaleY)
		{
			m_ShowWide.x *= scaleX;
			m_ShowWide.y *= scaleY;
		}

		void Scale(const Vector::Vec2& scale)
		{
			m_ShowWide.x *= scale.x;
			m_ShowWide.y *= scale.y;
		}
		void Move(const Vector::Vec2& move)
		{
			m_Position += move;
		}
	};
	class Rotate2D
	{
		Vector::Vec3 m_Data;
	public:
		Rotate2D(float radian = 0, const Vector::Vec2& point = Vector::Vec2()) :m_Data(point.x, point.y, radian) {}
		void SetPoint(const Vector::Vec2& point)
		{
			m_Data.x = point.x;
			m_Data.y = point.y;
		}
		void SetRadian(float radian)
		{
			m_Data.z = radian;
		}
		void SetAngle(float angle)
		{
			m_Data.z = Vector::AngleToRadian(angle);
		}
		Vector::Vec2 GetPoint()const
		{
			return Vector::Vec2(m_Data.x, m_Data.y);
		}
		float GetRadian()const
		{
			return m_Data.z;
		}
		float GetAngle()const
		{
			return Vector::RadianToAngle(m_Data.z);
		}
		Rotate2D operator*(const Rotate2D& rotate)
		{
			float newRadian = m_Data.z + rotate.m_Data.z;
			Vector::Vec2 newPos = *this * rotate.GetPoint();
			return Rotate2D(newRadian, newPos);
		}
		Vector::Vec2 operator*(const Vector::Vec2& vec)const
		{
			Vector::Vec2 pos = Vector::Vec2(m_Data.x, m_Data.y);
			Vector::Vec2 result = vec - pos;
			result.Rotate(m_Data.z);
			result += pos;
			return result;
		}
		friend Vector::Vec2 operator*(const Vector::Vec2& vec, const Rotate2D& rotate)
		{
			Vector::Vec2 pos = Vector::Vec2(rotate.m_Data.x, rotate.m_Data.y);
			Vector::Vec2 result = vec - pos;
			result.Rotate(rotate.m_Data.z);
			result += pos;
			return result;
		}
	};

	/// <summary>
	/// 场景物品
	/// 对同一窗口元素实现复用
	/// </summary>
	class RoomObject
	{
	protected:

		float m_Opacity;
	public:
		RoomObject() :m_Opacity(1.f)
		{}
		virtual ~RoomObject() { };
		virtual void Draw(Game::MainWind_D2D* wind,const Camera2D& Camera) = 0;
		virtual void Init(Game::MainWind_D2D* wind) = 0;

		void SetOpacity(float opacity)
		{
			m_Opacity = opacity;
		}
	};

	namespace RoomCallback {
		using RoomKeyCallback = void(*)(Room*, int KeyInputChar, int Frequency, KeyMode);
		using RoomMouseCallback = void(*)(Room*, int x, int y, int Frequency, MouseMessageType, KeyMode);
		using RoomSizeCallback = void(*)(Room*, int x,int y);
		using RoomUserWindowDrawCallback= void(*)(Room*, MainWind_D2D*);
	}

	/// <summary>
	/// 场景
	/// </summary>
	class Room
	{
		static void StaticDraw(MainWind_D2D* m_wind)
		{
			if (!m_wind)
				return;
			Room* room = (Room*)m_wind->GetUserData();
			if (!room)
				return;
			room->Update(m_wind->GetPaintIntervalTime());
			room->Draw(m_wind);
		}
		static void StaticKeyCallback(MainWind_D2D* m_wind,int key,int v,KeyMode mode)
		{
			if (!m_wind)
				return;
			Room* room = (Room*)m_wind->GetUserData();
			if (!room)
				return;
			if (room->m_RoomKeyCallback)
				room->m_RoomKeyCallback(room, key, v, mode);
		}
		static void StaticMouseCallback(MainWind* m_wind, int x, int y, int v, MouseMessageType type, KeyMode mode)
		{
			if (!m_wind)
				return;
			Room* room = (Room*)m_wind->GetUserData();
			if (!room)
				return;
			if (room->m_MouseCallback)
				room->m_MouseCallback(room, x, y, v, type, mode);
		}
		static void StaticSizeCallback(MainWind_D2D* m_wind, int x, int y)
		{
			if (!m_wind)
				return;
			Room* room = (Room*)m_wind->GetUserData();
			if (!room)
				return;
			if (room->m_SizeCallback)
				room->m_SizeCallback(room, x, y);
		}

		std::vector<RoomObject*>m_Objects;
		COLORREF m_ClearColor;
		bool m_BKClearRun;

		long long m_RoomUserData;
		RoomCallback::RoomKeyCallback m_RoomKeyCallback;
		RoomCallback::RoomMouseCallback m_MouseCallback;
		RoomCallback::RoomSizeCallback m_SizeCallback;
		RoomCallback::RoomUserWindowDrawCallback m_UserWindowDrawCallback;
	protected:
		Camera2D m_Camera;
	public:
		Room() :m_RoomKeyCallback(nullptr), m_MouseCallback(nullptr), m_RoomUserData(0), m_SizeCallback(nullptr),m_UserWindowDrawCallback(nullptr){}
		virtual ~Room()
		{
		}
		void Draw(MainWind_D2D* m_wind)
		{
			if (m_BKClearRun)
				m_wind->ClearWindBackground(m_ClearColor);
			for (auto& obj : m_Objects)
			{
				obj->Draw(m_wind,m_Camera);
			}
			if (m_UserWindowDrawCallback)
				m_UserWindowDrawCallback(this, m_wind);
		}
		/// <summary>
		/// 查找房间对象是否在房间中
		/// 找到返回对应位置
		/// 不在返回-1
		/// 错误传参-2
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		long long FindObj(RoomObject* obj)
		{
			if (!obj)
				return -2;
			for (long long i = 0; i < m_Objects.size(); ++i)
			{
				if (m_Objects[i] == obj)
				{
					return i;
				}
			}
			return -1;
		}
		/// <summary>
		/// 直接添加房间对象，没有重复检测
		/// </summary>
		/// <param name="obj"></param>
		void DirectAddObject(RoomObject* obj)
		{
			if (!obj)
				return;
			m_Objects.push_back(obj);
		}
		/// <summary>
		/// 设置额外的窗口回调函数，用于场景对象缺失时以窗口元素绘制替代
		/// 非必要不推荐使用
		/// 此回调会在房间元素绘制完成后绘制
		/// </summary>
		/// <param name="userCallback"></param>
		void SetExtraWindowDrawCallback(RoomCallback::RoomUserWindowDrawCallback userCallback)
		{
			m_UserWindowDrawCallback = userCallback;
		}
		Camera2D& GetCamera()
		{
			return m_Camera;
		}
		const Camera2D& GetCamera()const
		{
			return m_Camera;
		}
		/// <summary>
		/// 添加房间对象
		/// </summary>
		/// <param name="obj"></param>
		void AddObject(RoomObject* obj)
		{
			if (FindObj(obj) == -1)
				m_Objects.push_back(obj);
		}
		void AddObject(RoomObject& obj)
		{
			AddObject(&obj);
		}
		/// <summary>
		/// 删除房间对象
		/// </summary>
		/// <param name="obj"></param>
		void DeleteObj(RoomObject* obj)
		{
			if (!obj)
				return;
			auto i = FindObj(obj);
			if (i >= 0)
				m_Objects.erase(m_Objects.begin() + i);
		}
		void DeleteObj(RoomObject& obj)
		{
			DeleteObj(&obj);
		}
		RoomObject* GetObj(long long id)
		{
			if (id < 0 || id >= m_Objects.size())
				return nullptr;
			return m_Objects[id];
		}
		void SetClearBackground(COLORREF color, bool open = true)
		{
			m_BKClearRun = open;
			m_ClearColor = color;
		}
		void GotoRoom(MainWind_D2D* m_wind)
		{
			for (auto& o : m_Objects)
			{
				o->Init(m_wind);
			}
			m_wind->SetUserData((LONG64)this);
			m_wind->SetPaintCallback(StaticDraw);
			m_wind->SetKeyCallback(StaticKeyCallback);
			m_wind->SetMouseCallback(StaticMouseCallback);
			m_wind->SetWindSizeCallback(StaticSizeCallback);

			Start();
		}
		void SetRoomKeyCallback(RoomCallback::RoomKeyCallback callback)
		{
			m_RoomKeyCallback = callback;
		}
		void SetRoomMouseCallback(RoomCallback::RoomMouseCallback callback)
		{
			m_MouseCallback = callback;
		}
		void SetRoomSizeCallback(RoomCallback::RoomSizeCallback callback)
		{
			m_SizeCallback = callback;
		}
		void SetRoomUserData(long long data)
		{
			m_RoomUserData = data;
		}
	 	const long long& GetRoomUserData()const
		{
			return m_RoomUserData;
		}
		long long& GetRoomUserData()
		{
			return m_RoomUserData;
		}
		void LeaveRoom(MainWind_D2D* m_wind)
		{
			m_wind->SetUserData(0);

		}

		virtual void Start(){}
		virtual void Update(float IntervalTime) {};
	};
	/// <summary>
	/// 场景管理器
	/// </summary>
	class RoomManage
	{
		std::map<int, Room*> m_RoomIndex;
		Room* m_CurrentRoom;
		MainWind_D2D* m_wind;
	public:
		RoomManage(MainWind_D2D* m_wind) :m_CurrentRoom(nullptr), m_wind(m_wind)
		{
		}
		void SetWind(MainWind_D2D* m_wind)
		{
			m_wind = m_wind;
		}
		/// <summary>
		/// id返回小于0表示添加失败或
		/// </summary>
		/// <param name="room"></param>
		/// <param name="id"></param>
		/// <returns></returns>
		int AddRoom(Room* room, int id)
		{
			auto index = m_RoomIndex.find(id);
			if (index != m_RoomIndex.end())
			{
				return -1;
			}
			m_RoomIndex.insert(std::make_pair(id, room));
		}
		bool GotoRoom(int id)
		{
			if (!m_wind)
				return false;
			auto index = m_RoomIndex.find(id);
			if (index == m_RoomIndex.end())
			{
				return false;
			}
			m_wind->DeleteButten();
			index->second->GotoRoom(m_wind);
			return true;
		}
	};

	namespace RoomObj
	{
		/// <summary>
		/// 场景ui元素。
		/// 直接绘制到窗口。
		/// 坐标系是以像素为单位的窗口坐标系。
		/// 这是虚类。
		/// </summary>
		class RoomUI :public RoomObject
		{
		protected:
			D2D1_RECT_F m_ShowRectangle;
		public:
			virtual ~RoomUI() {};
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
			/// <summary>
			/// 显示区域的x，y
			/// </summary>
			/// <param name="x"></param>
			/// <param name="y"></param>
			void SetShowPosition(float x, float y)
			{
				float wDifference = x - m_ShowRectangle.left;
				float hDifference = y - m_ShowRectangle.top;
				m_ShowRectangle.left += wDifference;
				m_ShowRectangle.top += hDifference;
				m_ShowRectangle.right += wDifference;
				m_ShowRectangle.bottom += hDifference;
			}
			/// <summary>
			/// 显示区域的宽度与高度
			/// </summary>
			/// <param name="width"></param>
			/// <param name="height"></param>
			void SetShowWide(float width, float height)
			{
				m_ShowRectangle.bottom = height + m_ShowRectangle.top;
				m_ShowRectangle.right = width + m_ShowRectangle.left;
			}
		};
		/// <summary>
		/// 场景ui元素。
		/// 坐标系是场景坐标系。
		/// 这是虚类。
		/// </summary>
		class RoomScence:public RoomObject
		{
		protected:
			Vector::Vec2 m_Position;
			Vector::Vec2 m_Wide;
			Rotate2D m_Rotate;
		public:
			void SetShowPosition(const Vector::Vec2& pos)
			{
				m_Position = pos;
			}
			Vector::Vec2& GetPosition()
			{
				return m_Position;
			}
			const Vector::Vec2& GetPosition()const
			{
				return m_Position;
			}
			void SetWide(const Vector::Vec2& wide)
			{
				m_Wide = wide;
			}
			Vector::Vec2& GetWide()
			{
				return m_Wide;
			}
			const Vector::Vec2& GetWide()const
			{
				return m_Wide;
			}
			const Rotate2D& GetRotate()const
			{
				return m_Rotate;
			}
			Rotate2D& GetRotate()
			{
				return m_Rotate;
			}
			void SetRotate(const Rotate2D& rotate)
			{
				m_Rotate = rotate;
			}
			void Rotate(const Rotate2D& rotate)
			{
				m_Rotate = m_Rotate * rotate;
			}
			void SetAngle(float angle)
			{
				m_Rotate.SetAngle(angle);
			}
			static Vector::Vec2 ScenceToCamera(const Vector::Vec2& pos, const Camera2D& camera)
			{
				Vector::Vec2 right = Vector::Vec2(1, 0);
				Vector::Vec2 up = right.Rotate(Vector::PI * 0.5) * camera.m_ShowWide.y * 0.5;
				Vector::Vec2 origin = camera.m_Position - right * camera.m_ShowWide.x * 0.5 + up;
				Vector::Vec2 result = pos - origin;
				result = Vector::Vec2(result.Dot(right), result.Dot(up.Normalize()));
				result.y *= -1;
				return result;
			}
			static Vector::Vec2 CameraToWindow(const Vector::Vec2& pos, MainWind* wind, const Camera2D& camera)
			{
				Vector::Vec2 Shift = pos;
				SIZE size = wind->GetWindSize();
				float xScale = size.cx / camera.m_ShowWide.x;
				float yScale = size.cy / camera.m_ShowWide.y;

				Shift.x *= xScale;
				Shift.y *= yScale;
				return Shift;
			}
			static Vector::Vec2 ScenceToWindow(const Vector::Vec2& pos, MainWind* wind, const Camera2D& camera)
			{
				Vector::Vec2 originPoint = camera.m_ShowWide * 0.5;
				Vector::Vec2 Shift = pos - camera.m_Position;
				Shift.y = -Shift.y;
				Shift += originPoint;
				SIZE size = wind->GetWindSize();
				float xScale = size.cx / camera.m_ShowWide.x;
				float yScale = size.cy / camera.m_ShowWide.y;

				Shift.x *= xScale;
				Shift.y *= yScale;
				return Shift;
			}
		};

		class ImageUI :public RoomUI
		{
			WindElements::d2dPicture* m_Image;
		public:
			ImageUI()
			{}
			~ImageUI()
			{}
			void Bind(WindElements::d2dPicture* picture)
			{
				m_Image = picture;
			}
			void Init(Game::MainWind_D2D* m_wind)override{}
			void Draw(MainWind_D2D* m_wind,const Camera2D&)override
			{
				m_Image->SetOpacity(m_Opacity);
				m_Image->SetShowRect(m_ShowRectangle);
				m_Image->Draw(m_wind);
			}
		};

		class TextUI :public RoomUI
		{
			Game::MainWind_D2D* m_wind;
			WindElements::d2dText m_text;
		public:
			TextUI():m_wind(nullptr){}
			void SetColor(const Vector::Vec3& color)
			{
				SetColor(D2D1::ColorF(color.r, color.g, color.b));
			}
			void SetColor(const D2D1_COLOR_F& color)
			{
				m_text.SetColor(color, m_wind->GetD2DTargetP());
			}
			void SetText(const std::wstring& str)
			{
				m_text.SetShowText(str);
			}
			void SetRotate(float angle, const Vector::Vec2& point = Vector::Vec2())
			{
				m_text.SetRotate(angle, point.ToPointD2D());
			}
			void Init(Game::MainWind_D2D* wind)override 
			{
				m_wind = wind;
				auto&& tar = m_wind->GetD2DTargetP();
				m_text.SetColor(D2D1::ColorF(0), tar);
			}
			void Draw(MainWind_D2D* wind, const Camera2D&)override
			{
				m_text.SetOpacity(m_Opacity);
				m_text.SetShowRect(m_ShowRectangle);
				m_text.Draw(wind);
			}

		};

		class AnimationUI:public RoomUI
		{
			WindControl::d2dPictureAnimationBase* m_Animation;
		public:
			AnimationUI(WindControl::d2dPictureAnimationBase* data = nullptr) :m_Animation(data) {}
			void SetData(WindControl::d2dPictureAnimationBase* AnimationData)
			{
				m_Animation = AnimationData;
			}
			WindControl::d2dPictureAnimationBase* GetCurrentData()const
			{
				return m_Animation;
			}
		};
		/// <summary>
		/// 设置图片到场景
		/// </summary>
		class Image :public RoomScence
		{
			D2D1_RECT_F m_Crop;
			WindElements::d2dPicture* m_Image;
		public:
			Image(){}
			void SetMapData(WindElements::d2dPicture* image)
			{
				m_Image = image;
				auto size = image->GetImageSize();
				m_Crop = D2D1::RectF(0, 0, size.width, size.height);
			}
			void SetCrop(const D2D1_RECT_F& crop)
			{
				m_Crop = crop;
			}

			void Draw(Game::MainWind_D2D* wind, const Camera2D& Camera)override
			{
				Vector::Vec2 Point1 = ScenceToCamera(m_Position, Camera);
				if (Point1.x > Camera.m_ShowWide.x+2 || Point1.y >  Camera.m_ShowWide.y+2)
					return;
				Vector::Vec2 Point2 = ScenceToCamera(m_Position + m_Wide, Camera);
				if (Point2.x < -2 || Point2.y < -2)
					return;
				Point2 = Point1 + m_Wide;
				Point1 = CameraToWindow(Point1, wind, Camera);

				Point2 = CameraToWindow(Point2, wind, Camera);
				m_Image->SetShowRect({ Point1.x, Point1.y,Point2.x,Point2.y });
				m_Image->SetOpacity(m_Opacity);
				m_Image->Draw(wind->GetD2DTargetP(), m_Crop);
			}
			void Init(MainWind_D2D*)override{}
		};
		/// <summary>
		/// 铺设瓦片地图到场景。
		/// 这个会忽略旋转
		/// </summary>
		class TileMap :public RoomScence
		{
			WindControl::d2dPictureAnimationBase* m_Data;
			std::map<Vector::Vector2<int>, int>m_TileIndex;
			Vector::Vec2 m_TileWide;
			Vector::Vec2 m_ImageWide;
		public:
			TileMap():m_Data(nullptr){}
			void CreateTileMap(const Vector::Vec2& Size, const Vector::Vec2& TileWide, const Vector::Vec2& ImageWide, int defIndex = -1)
			{
				m_TileWide = TileWide;
				m_ImageWide = ImageWide;
				m_TileIndex.clear();
				for (int x = 0; x < Size.x; ++x)
				{
					for (int y = 0; y < Size.y; ++y)
					{
						m_TileIndex.insert(std::make_pair(Vector::Vector2<int>(x, y), defIndex));
					}
				}
			}
			int GetIndex(const Vector::Vector2<int>& pos)
			{
				auto p = m_TileIndex.find(pos);
				if (p == m_TileIndex.end())
					return -2;
				return p->second;
			}
			/// <summary>
			/// 添加指定位置的瓦片
			/// </summary>
			/// <param name="pos"></param>
			/// <param name="index"></param>
			void SetTileMap(const Vector::Vector2<int>& pos,int index)
			{
				if (index < 0)
					return;
				m_TileIndex[pos] = index;
			}
			/// <summary>
			/// 添加指定矩形区域的瓦片
			/// </summary>
			/// <param name="pos1">矩形左下角的点（场景坐标）</param>
			/// <param name="pos2">矩形右上角的点（场景坐标）</param>
			/// <param name="index"></param>
			void SetTileMap(const Vector::Vector2<int>& pos1, const Vector::Vector2<int>& pos2, int index)
			{
				for (int x = pos1.x; x <= pos2.x; ++x)
				{
					for (int y = pos1.y; y <= pos2.y; ++y)
					{
						SetTileMap(Vector::Vector2<int>(x, y), index);
					}
				}
			}

			void DeleteTileMap(const Vector::Vector2<int>& pos)
			{
				m_TileIndex.erase(pos);
			}
			void DeleteTileMap(const Vector::Vector2<int>& pos1, const Vector::Vector2<int>& pos2)
			{
				for (int x = pos1.x; x <= pos2.x; ++x)
				{
					for (int y = pos1.y; y <= pos2.y; ++y)
					{
						DeleteTileMap(Vector::Vector2<int>(x, y));
					}
				}
			}
			void SetImageWide(const Vector::Vec2& ImageWide)
			{
				m_ImageWide = ImageWide;
			}
			/// <summary>
			/// 绑定单图片动画组件作为数据。
			/// 多图片组件绑定失效
			/// </summary>
			/// <param name="data"></param>
			void SetMapData(WindControl::d2dPictureAnimationBase* data)
			{
				m_Data = data;
			}			
			/// <summary>
			/// 绑定图片动画组件作为数据。
			/// </summary>
			/// <param name="data"></param>
			void SetMapData(WindControl::d2dPictureAnimationBase& data)
			{
				m_Data = &data;
			}
			void Draw(Game::MainWind_D2D* wind, const Camera2D& Camera)override
			{
				if (!wind || (!m_Data))
					return;
				m_Data->SetSwitchTime(-1);

				m_Data->SetShowWide(CameraToWindow(m_ImageWide, wind, Camera));
				Vector::Vec2 rightTop(Camera.m_Position + Camera.m_ShowWide * 0.5 - m_Position);
				Vector::Vec2 leftBottom(Camera.m_Position - Camera.m_ShowWide * 0.5 - m_Position);


				for (int x = leftBottom.x - 1; x < rightTop.x + 1; ++x)
				{
					for (int y = leftBottom.y - 1; y < rightTop.y + 1; ++y)
					{
						int index = GetIndex(Vector::Vector2<int>(x, y));
						if (index < 0)
							continue;

						auto Point1 = Vector::Vec2(x, y) + m_Position;
						Point1 = ScenceToWindow(Point1, wind, Camera);
						m_Data->SetShowPosition(Point1);
						m_Data->SetOpacity(m_Opacity);
						m_Data->ToPicture(index);
						m_Data->Draw(wind);
					}
				}
			}
			void Init(MainWind_D2D*)override {}
		};

		class Animation :public RoomScence
		{
			WindControl::d2dPictureAnimationBase* m_AnimationData;
		public:
			Animation():m_AnimationData(nullptr){}
			void SetData(WindControl::d2dPictureAnimationBase* data)
			{
				m_AnimationData = data;
			}

			void Draw(Game::MainWind_D2D* wind, const Camera2D& Camera)override
			{
				if (!wind || (!m_AnimationData))
					return;
				m_AnimationData->SetShowPosition(ScenceToWindow(m_Position, wind, Camera));
				m_AnimationData->SetShowWide(CameraToWindow(m_Wide, wind, Camera));
				m_AnimationData->SetOpacity(m_Opacity);
				m_AnimationData->Draw(wind);

			}
			void Init(Game::MainWind_D2D* m_wind)override
			{

			}
		};
	};
	namespace RoomObjTool
	{
		class TileMapType
		{
		public:
			bool lt, mt, rt;
			bool lm, mm, rm;
			bool lb, mb, rb;
			char activation;
			int result;
			TileMapType() :
				lt(false), mt(false), rt(false),
				lm(false), mm(false), rm(false),
				lb(false), mb(false), rb(false),
				result(0), activation(0b01011010) {}
			TileMapType(
				bool LT, bool MT, bool RT,
				bool LM, bool MM, bool RM,
				bool LB, bool MB, bool RB,
				int Result, char Activation = 0b01011010) :
				lt(LT), mt(MT), rt(RT),
				lm(LM), mm(MM), rm(RM),
				lb(LB), mb(MB), rb(RB),
				result(Result), activation(Activation) {}
			TileMapType(char Rule, bool MM, int Result, char Activation = 0b01011010) :result(Result), mm(MM), activation(Activation)
			{
				for (int i = 0; i < 8; ++i)
				{
					operator[](i) = (Rule >> (7 - i)) & 1;
				}

			}
			bool& operator[](int num)
			{
				if (num > 8)
				{
					std::runtime_error("传入了无法处理的参数");
					return rb;
				}
				switch (num)
				{
				case 0:
					return lt;
				case 1:
					return mt;
				case 2:
					return rt;
				case 3:
					return lm;
				case 4:
					return rm;
				case 5:
					return lb;
				case 6:
					return mb;
				case 7:
					return rb;
				case 8:
					return mm;
				default:
					break;
				}
				return rb;
			}
			const bool& operator[](int num)const
			{
				if (num > 8)
				{
					std::runtime_error("传入了无法处理的参数");
					return rb;
				}
				switch (num)
				{
				case 0:
					return lt;
				case 1:
					return mt;
				case 2:
					return rt;
				case 3:
					return lm;
				case 4:
					return rm;
				case 5:
					return lb;
				case 6:
					return mb;
				case 7:
					return rb;
				case 8:
					return mm;
				default:
					break;
				}
				return rb;
			}
			bool operator==(const TileMapType& tmt)const
			{
				for (int i = 0; i <= 8; i++)
				{
					if (i >= 8 || ((activation >> (7 - i)) & 1))
						if (operator[](i) != tmt[i])
						{
							return false;
						}
				}
				return true;
			}
		};
		class TileMapTool
		{
			std::map<Vector::Vec2i, bool>m_MapData;
			std::vector<TileMapType>m_Rules;
		public:

			TileMapTool() { }
			void SetMap(const std::map<Vector::Vec2i, bool>& Map)
			{
				m_MapData = Map;
			}
			void AddRule(const TileMapType& tmt)
			{
				m_Rules.push_back(tmt);
			}
			void DeleteRule(const TileMapType& tmt)
			{
				int i = 0;
				for (auto& t : m_Rules)
				{
					if (t == tmt)
					{
						break;
					}
					++i;
				}
				m_Rules.erase(m_Rules.begin() + i);
			}
			bool GetMapInformation(const Vector::Vec2i& pos)
			{
				auto r = m_MapData.find(pos);
				if (r == m_MapData.end())
					return false;
				return r->second;
			}
			int GetRules(const TileMapType& tmt)
			{
				int result = -1;
				for (auto& i : m_Rules)
				{
					if (i == tmt)
						result = i.result;
				}
				return result;
			}
			void ToTileMap(RoomObj::TileMap& map)
			{
				for (auto& mt : m_MapData)
				{
					auto& pos = mt.first;

					auto result = GetRules(TileMapType(
						GetMapInformation(Vector::Vec2i(pos.x - 1, pos.y + 1)),
						GetMapInformation(Vector::Vec2i(pos.x, pos.y + 1)),
						GetMapInformation(Vector::Vec2i(pos.x + 1, pos.y + 1)),

						GetMapInformation(Vector::Vec2i(pos.x - 1, pos.y)),
						GetMapInformation(Vector::Vec2i(pos.x, pos.y)),
						GetMapInformation(Vector::Vec2i(pos.x + 1, pos.y)),

						GetMapInformation(Vector::Vec2i(pos.x - 1, pos.y - 1)),
						GetMapInformation(Vector::Vec2i(pos.x, pos.y - 1)),
						GetMapInformation(Vector::Vec2i(pos.x + 1, pos.y - 1)),
						0
					));
					map.SetTileMap(pos, result);
				}
			}
		};
	}
};