
#pragma once
#include "Imgui/imgui_internal.h"
ID3D11ShaderResourceView* icons[5] = {};
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
extern ID3D11Device* ms_vD3dDevice;
bool LoadTextureFromFile(stbi_uc const* buffer, int len, ID3D11ShaderResourceView** out_srv)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory(buffer, len, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	ms_vD3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	ms_vD3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	//*out_width = image_width;
	//*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}
namespace ImGui
{
	bool TabEx(const char* label, int number, const bool active, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImVec2(48, 12);
		const ImVec2 second_label_size = ImVec2(112, 6);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// if (window-> & ImGuiItemFlags_ButtonRepeat)
		// 	flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);
		if (pressed)
			MarkItemEdited(id);
		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavHighlight(bb, id);
		GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 17), ImVec2(pos.x + 115, pos.y + 47), ImColor(20, 20, 20, 255), 5.5f);
		if (active || hovered)
		{

			PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
			RenderTextClipped(ImVec2(bb.Min.x - 3, bb.Min.y - 2), ImVec2(bb.Max.x - 3, bb.Max.y - 2), label, NULL, &label_size, style.ButtonTextAlign, &bb);
			PopStyleColor();

			GetWindowDrawList()->AddImage((void*)(intptr_t)icons[number], ImVec2(bb.Min.x + 11, bb.Min.y + 24), ImVec2(bb.Max.x - 123, bb.Max.y - 24), ImVec2(0, 0), ImVec2(1, 1), GetColorU32(ImVec4(1.0f, 0.54f, 0.0f, 1.00f)));
		}
		else
		{

			PushStyleColor(ImGuiCol_Text, ImVec4(0.59f, 0.59f, 0.59f, 1.f));
			RenderTextClipped(ImVec2(bb.Min.x - 3, bb.Min.y - 2), ImVec2(bb.Max.x - 3, bb.Max.y - 2), label, NULL, &label_size, style.ButtonTextAlign, &bb);
			PopStyleColor();

			GetWindowDrawList()->AddImage((void*)(intptr_t)icons[number], ImVec2(bb.Min.x + 11, bb.Min.y + 24), ImVec2(bb.Max.x - 123, bb.Max.y - 24));
		}
		return pressed;
	}
}