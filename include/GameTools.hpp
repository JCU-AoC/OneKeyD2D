
//碰撞盒
class Collider
{

public:
	using ColliderCallback = void(*)(RoomObject*, Collider*);
	//位置
	Vector::Vec2 m_Position;
	//宽度
	Vector::Vec2 m_Width = Vector::Vec2(1);
	//属于
	RoomObject* m_User = nullptr;
	//激活回调
	ColliderCallback m_Callback = nullptr;
	//圆润，静态
	bool m_elliptic = false, m_Static = false;
	/// <summary>
	/// 检测两碰撞体是否重叠
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool intersection(const Collider& other)const
	{
		auto limit = (m_Width + other.m_Width) * 0.5;
		auto distance = m_Position - other.m_Position;
		if (std::abs(distance.x) > limit.x || std::abs(distance.y) > limit.y)
			return false;
		if (m_elliptic)
		{
			if (other.m_elliptic)
			{
				if (distance.Length() > limit.x * 2)
					return false;
			}
			else
			{
				Vector::Vec2 point = Vector::Vec2(
					(limit.x < 0 ? -0.5f : 0.5f) * other.m_Width.x + other.m_Position.x,
					(limit.y < 0 ? -0.5f : 0.5f) * other.m_Width.y + other.m_Position.y);
				WindElements::d2dClickDetection click(
					m_Position.x - m_Width.x * 0.5, m_Position.y + m_Width.y * 0.5,
					m_Width.x, m_Width.y);
				click.Elliptic(true);
				return click.CheckClick(point.x, point.y);
			}
		}
		else if (other.m_elliptic)
		{
			return other.intersection(*this);
		}
		return true;
	}
	/// <summary>
	/// 指定点是否在碰撞体内
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	bool intersection(const Vector::Vec2& pos)const
	{
		auto leftBottom = m_Position - m_Width * 0.5;
		if (pos.x < leftBottom.x || pos.y < leftBottom.y)
		{
			return false;
		}
		auto rightTop = m_Position + m_Width * 0.5;
		if (pos.x > rightTop.x || pos.y > rightTop.y)
		{
			return false;
		}
		return true;
	}
	/// <summary>
	/// 激活回调函数
	/// </summary>
	/// <param name="other">来自谁</param>
	void activation(Collider& other)const
	{
		if (!m_Callback || !m_User)
			return;
		m_Callback(m_User, &other);
	}
	/// <summary>
	/// 激活回调函数
	/// </summary>
	/// <param name="other"></param>
	void activation(Collider* other)const
	{
		if (!m_Callback || !m_User)
			return;
		m_Callback(m_User, other);
	}
};
//碰撞检测管理器
class CollisionDetection
{
	//分区宽度
	int m_gridWidth = 1;
	//区块坐标对应的碰撞盒
	std::map<Vector::Vec2i, std::set<Collider*>> m_Grid;
	/// <summary>
	/// 检测并激活
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="collider"></param>
	void Check(const Vector::Vec2i& pos, Collider& collider)
	{
		auto tel = m_Grid.find(pos);
		if (tel == m_Grid.end())
			return;
		auto& ColSet = tel->second;
		for (auto& c : ColSet)
		{
			if (collider.intersection(*c))
			{
				collider.activation(*c);
			}
		}
	}

	Collider* CheckPoint(const Vector::Vec2i& key, const Collider& collider)
	{
		auto it = m_Grid.find(key);
		if (it == m_Grid.end())
			return nullptr;
		for (auto c : it->second)
		{
			if (c == &collider)
				continue;
			
			if (c->intersection(collider))
			{
				return c;
			}
		}
		return nullptr;
	}
	bool DeleteCollider(const Vector::Vec2i& key, Collider* aim)
	{
		auto it = m_Grid.find(key);
		if (it == m_Grid.end())
			return false;
		auto P = it->second.find(aim);
		if (P == it->second.end())return false;
		it->second.erase(P);
		if (it->second.empty())
		{
			m_Grid.erase(it);
		}
		return true;
	}
public:
	CollisionDetection(int gridWidth = 10) :m_gridWidth(gridWidth) {}
	Vector::Vec2i GetGridPos(const Collider& collider)const
	{
		return Vector::Vec2i::ToType((collider.m_Position / m_gridWidth) + 1);
	}
	Vector::Vec2i GetGridPos(const Vector::Vec2& pos)const
	{
		return Vector::Vec2i::ToType((pos / m_gridWidth) + 1);
	}
	void AddCollider(std::vector<Collider>& data)
	{
		for (auto& c : data)
		{
			AddCollider(c);
		}
	}
	void AddCollider(Collider& collider)
	{
		auto wide = collider.m_Width;
		Vector::Vec2i key(GetGridPos(collider));
		auto& ColliderSet = m_Grid[key];
		ColliderSet.insert(&collider);
		int MaxWide = wide.x > wide.y ? wide.x : wide.y;
		if (MaxWide > m_gridWidth)
		{
			m_gridWidth = MaxWide;
		}
	}
	void DeleteAllCollider()
	{
		m_Grid.clear();
	}
	void DeleteCollider(Collider& collider)
	{
		Collider* aim = &collider;
		Vector::Vec2i key(GetGridPos(collider));
		if (DeleteCollider(Vector::Vec2i(key.x, key.y), aim))return;

		if (DeleteCollider(Vector::Vec2i(key.x + 1, key.y), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x - 1, key.y), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x, key.y + 1), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x, key.y - 1), aim))return;

		if (DeleteCollider(Vector::Vec2i(key.x + 1, key.y + 1), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x + 1, key.y - 1), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x - 1, key.y + 1), aim))return;
		if (DeleteCollider(Vector::Vec2i(key.x - 1, key.y - 1), aim))return;

		//周围区块未找到，开始遍历数据
		for (auto& grid : m_Grid)
		{
			for (auto& c : grid.second)
			{
				if (c == &collider)
				{
					grid.second.erase(c);
					if (grid.second.empty())
						m_Grid.erase(grid.first);
					return;
				}
			}
		}
	}
	/// <summary>
	/// 检测碰撞
	/// </summary>
	void Check()
	{
		for (auto& cs : m_Grid)
		{
			for (auto& c : cs.second)
			{
				auto pos = cs.first;
				Check(pos, *c);
				Check(Vector::Vec2i(pos.x, pos.y + 1), *c);
				Check(Vector::Vec2i(pos.x, pos.y - 1), *c);
				Check(Vector::Vec2i(pos.x + 1, pos.y + 1), *c);
				Check(Vector::Vec2i(pos.x + 1, pos.y - 1), *c);
				Check(Vector::Vec2i(pos.x - 1, pos.y + 1), *c);
				Check(Vector::Vec2i(pos.x - 1, pos.y - 1), *c);
				Check(Vector::Vec2i(pos.x + 1, pos.y), *c);
				Check(Vector::Vec2i(pos.x - 1, pos.y), *c);
			}
		}
	}
	/// <summary>
	/// 更新位置
	/// 静态碰撞不会更新
	/// </summary>
	void Update()
	{
		auto p = m_Grid.begin();
		while (p != m_Grid.end())
		{
			auto col = p->second.begin();
			while (col != p->second.end())
			{
				if (!(*col)->m_Static)
				{
					auto nowPos = GetGridPos(*(*col));
					if (nowPos != p->first)
					{
						m_Grid[nowPos].insert(*col);
						col = p->second.erase(col);
						continue;
					}
				}
				++col;
			}
			if (p->second.empty())
			{
				p = m_Grid.erase(p);
				continue;
			}
			++p;
		}
	}

	void MoveCollider(Collider* collider, const Vector::Vec2& move)
	{
		auto it = m_Grid.find(GetGridPos(*collider));
		if (it == m_Grid.end())
			return;
		for (auto& c : it->second)
		{
			if (c == collider)
			{
				it->second.erase(c);
				if (it->second.empty())
					m_Grid.erase(it);
				collider->m_Position += move;
				AddCollider(*collider);
				return;
			}
		}

	}

	/// <summary>
	/// 检查指定位置是否有碰撞体
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	Collider* CheckPoint(const Vector::Vec2& pos)
	{
		auto it = m_Grid.find(GetGridPos(pos));
		if (it == m_Grid.end())
			return nullptr;
		for (auto c : it->second)
		{
			if (c->intersection(pos))
			{
				return c;
			}
		}
		return nullptr;
	}
	/// <summary>
	/// 检测是否存在碰撞
	/// </summary>
	/// <param name="collider"></param>
	/// <returns></returns>
	Collider* CheckPoint(const Collider& collider)
	{
		auto key = GetGridPos(collider);
		auto col = CheckPoint(key, collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x + 1, key.y), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x - 1, key.y), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x, key.y + 1), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x, key.y - 1), collider);
		if (col)return col;

		col = CheckPoint(Vector::Vec2i(key.x + 1, key.y + 1), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x + 1, key.y - 1), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x - 1, key.y + 1), collider);
		if (col)return col;
		col = CheckPoint(Vector::Vec2i(key.x - 1, key.y - 1), collider);
		if (col)return col;

		return nullptr;
	}
};