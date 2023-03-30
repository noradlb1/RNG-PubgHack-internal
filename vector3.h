#define UCONST_Pi           3.1415926
#define RadianToURotation 180.0f / UCONST_Pi
struct Frotator
{
	float Pitch;
	float Yaw;
	//float Roll;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}
	~Vector3()
	{
	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	inline float Length()
	{
		float ls = x * x + y * y + z * z;
		return sqrt(ls);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}

	Vector3& operator/=(float fl) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	void clamp()
	{
		if (x > 75.f) x = 75.f;
		else if (x < -75.f) x = -75.f;
		if (z < -180) z += 360.0f;
		else if (z > 180) z -= 360.0f;

		y = 0.f;
	}
};

struct Vector2
{
public:
	float x;
	float y;
	inline Vector2() : x(0), y(0) {}
	inline Vector2(float x, float y) : x(x), y(y) {}
	inline float Distance(Vector2 v)
	{
		return sqrtf(((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y)));
	}
	inline Vector2 operator+(const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}
	inline Vector2 operator-(const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}
};

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};
class PlayerData
{
public:
	std::string Type;
	DWORD Address = 0;
	Vector3 Position;
	Frotator Rotation;
	int Pose = 0;
	float Health = 0;
	float NearDeathBreath = 0;
	float MaxHealth = 100;
	std::string Name;
	std::string ID;
	std::string WeaponName;
	bool IsRobot = false;
	bool isfriend = false;
	bool isaiming = false;
	int TeamID = 0;
	int IsTeam = 0;
	int CurBulletNumInClip;
	int CurMaxBulletNumInOneClip;
	uint32_t CurrentVehicle = 0;

};
class VehicleData
{
public:
	uint32_t Address;
	Vector3 Position;
	std::string Name;
	bool bIsEngineStarted;
	float HPMax;
	float HP;
	float FuelMax;
	float Fuel;
	int LatestDriverPlayerUID;
};
class ItemData
{
public:
	Vector3 Position;
	bool pickupbox;
	std::vector<std::string> items;
	std::string Name;
};
class GrenadeData
{
public:
	Vector3 Position;
	std::string Name;
};
struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
class FRotator
{
public:
	float Pitch = 0.f;
	float Yaw = 0.f;
	float Roll = 0.f;

	D3DMATRIX GetAxes()
	{
		auto tempMatrix = Matrix();
		return tempMatrix;
	}

	D3DMATRIX Matrix(Vector3 origin = Vector3(0, 0, 0))
	{
		float radPitch = (Pitch * float(UCONST_Pi) / 180.f);
		float radYaw = (Yaw * float(UCONST_Pi) / 180.f);
		float radRoll = (Roll * float(UCONST_Pi) / 180.f);
		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}
};
struct FMinimalViewInfo {
	Vector3 Location;
	Vector3 LocationLocalSpace;
	FRotator Rotation;
	float FOV;
};