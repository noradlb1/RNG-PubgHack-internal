ImFont* m_pFont = NULL;
ImFont* Font12 = NULL;
ImFont* Font14 = NULL;
ImFont* Font16 = NULL;
ImFont* Font18 = NULL;
ImFont* Font24 = NULL;
ImFont* fontname = NULL;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
void DrawStrokeText(int x, int y, RGBA* color, const char* str, float fontsize = 12, bool center = false)
{
	if (center)
	{
		auto textisze = m_pFont->CalcTextSizeA(fontsize, FLT_MAX, 0, str);
		x -= textisze.x / 2;
		y -= textisze.y / 2;
	}
	ImFont a;
	ImGui::GetBackgroundDrawList()->AddText(m_pFont, fontsize, ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0 / 255.0, 0 / 255.0, 0 / 255.0, 255 / 255.0)), str);
	ImGui::GetBackgroundDrawList()->AddText(m_pFont, fontsize, ImVec2(x + 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0 / 255.0, 0 / 255.0, 0 / 255.0, 255 / 255.0)), str);
	ImGui::GetBackgroundDrawList()->AddText(m_pFont, fontsize, ImVec2(x, y - 0.1), ImGui::ColorConvertFloat4ToU32(ImVec4(0 / 255.0, 0 / 255.0, 0 / 255.0, 255 / 255.0)), str);
	ImGui::GetBackgroundDrawList()->AddText(m_pFont, fontsize, ImVec2(x, y + 0.1), ImGui::ColorConvertFloat4ToU32(ImVec4(0 / 255.0, 0 / 255.0, 0 / 255.0, 255 / 255.0)), str);
	ImGui::GetBackgroundDrawList()->AddText(m_pFont, fontsize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), str);
}

//void DrawStrokeText(int x, int y, RGBA *color, const char* str)
//{
//	ImFont a;
//	std::string utf_8_1 = std::string(str);
//	std::string utf_8_2 = string_To_UTF8(utf_8_1);
//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), utf_8_2.c_str());
//}

void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), str);
}

void DrawNewText2(const std::string& text, const ImVec2& position, float size, RGBA* color, bool center)
{
	ImDrawList* window = ImGui::GetBackgroundDrawList();
	float a = color->a;
	float r = color->r;
	float g = color->g;
	float b = color->b;

	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = m_pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			//window->AddText(m_pFont, size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			//window->AddText(m_pFont, size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			//window->AddText(m_pFont, size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			//window->AddText(m_pFont, size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());

			window->AddText(m_pFont, size, { position.x - textSize.x / 2.0f, position.y + textSize.y * i }, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), line.c_str());
		}
		else
		{
			window->AddText(m_pFont, size, { (position.x) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->AddText(m_pFont, size, { (position.x) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->AddText(m_pFont, size, { (position.x) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->AddText(m_pFont, size, { (position.x) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());

			window->AddText(m_pFont, size, { position.x, position.y + textSize.y * i }, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), line.c_str());
		}

		y = position.y + textSize.y * (i + 1);
		i++;
	}

	//return y;
}
void DrawNewText(int x, int y, RGBA* color, const char* str, float fontsize)
{
	ImFont a;
	ImGui::GetBackgroundDrawList()->AddText(NULL, fontsize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), str);
}
void DrawRect(int x, int y, int w, int h, RGBA* color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), 0, 0, thickness);
}
void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), 0, 0);
}
void DrawCircleFilled(int x, int y, int radius, RGBA* color)
{
	ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)));
}
void DrawCircle(int x, int y, int radius, RGBA* color, int segments, float thickness = 1.0f)
{
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), segments, thickness);
}
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color, float thickne)
{
	ImGui::GetBackgroundDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), thickne);
}
void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color)
{
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)));
}
void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->r / 255.0, color->g / 255.0, color->b / 255.0, color->a / 255.0)), thickness);
}
void DrawCornerBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	DrawFilledRect(x + borderPx, y, w / 3, borderPx, color); //top
	DrawFilledRect(x + w - w / 3 + borderPx, y, w / 3, borderPx, color); //top
	DrawFilledRect(x, y, borderPx, h / 3, color); //left
	DrawFilledRect(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color); //left
	DrawFilledRect(x + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom
	DrawFilledRect(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom
	DrawFilledRect(x + w + borderPx, y, borderPx, h / 3, color);//right
	DrawFilledRect(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color);//right
}
void DrawBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	//DrawFilledRect(x + borderPx, y, w / 3, borderPx, color); //top
	DrawFilledRect(x, y, w, borderPx, color);                                     // Top
	DrawFilledRect(x, y + h - borderPx, w, borderPx, color);                         // Bottom
	DrawFilledRect(x, y + borderPx, borderPx, h - 2 * borderPx, color);               // Left
	DrawFilledRect(x + w - borderPx, y + borderPx, borderPx, h - 2 * borderPx, color);   // Right
}