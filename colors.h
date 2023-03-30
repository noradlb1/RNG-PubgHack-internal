#define M_PI 3.14159265358979323846

struct RGBA {
	float r, g, b, a;
	RGBA() = default;
	RGBA(float r, float g, float b, float a = 255) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	RGBA(uint32_t color) {
		this->a = (color >> 24) & 0xff;
		this->r = (color >> 16) & 0xff;
		this->g = (color >> 8) & 0xff;
		this->b = (color & 0xff);
	}
	RGBA from_uint(uint32_t uint) {
		return RGBA(uint);
	}
	void random_color(int iTick)
	{
		this->r = sin(5.f * iTick + 0.f) * 127 + 128;
		this->g = sin(5.f * iTick + ((2.f * M_PI) / 3)) * 127 + 128;
		this->b = sin(5.f * iTick + ((4.f * M_PI) / 3)) * 127 + 128;
		this->a = 255;
	}
};

class Color
{
public:
	RGBA red = { 255,0,0,255 };
	RGBA green = { 0, 255, 0,255 };
	RGBA blue = { 0, 0, 255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA white = { 255,255,255,255 };
	RGBA black = { 0,0,0,255 };
};
Color Col;
