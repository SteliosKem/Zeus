#pragma once
#include "imgui.h"
void SetupImGuiStyle()
{
	// Zeus style from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.4000000059604645f;
	style.WindowPadding = ImVec2(8.600000381469727f, 2.599999904632568f);
	style.WindowRounding = 7.199999809265137f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 5.900000095367432f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 5.900000095367432f;
	style.PopupBorderSize = 0.0f;
	style.FramePadding = ImVec2(5.599999904632568f, 1.0f);
	style.FrameRounding = 3.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 10.19999980926514f;
	style.GrabMinSize = 11.89999961853027f;
	style.GrabRounding = 17.20000076293945f;
	style.TabRounding = 4.699999809265137f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.7843137383460999f, 0.7843137383460999f, 0.7843137383460999f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.2800000011920929f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1215686276555061f, 0.1215686276555061f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 0.9019607901573181f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1215686276555061f, 0.1215686276555061f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1215686276555061f, 0.1215686276555061f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1411764770746231f, 0.1411764770746231f, 0.1411764770746231f, 0.7490196228027344f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1215686276555061f, 0.1215686276555061f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.6470588445663452f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1215686276555061f, 0.1215686276555061f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 0.7803921699523926f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7019608020782471f, 0.7019608020782471f, 0.7019608020782471f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.7019608020782471f, 0.7019608020782471f, 0.7019608020782471f, 0.3529411852359772f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7019608020782471f, 0.7019608020782471f, 0.7019608020782471f, 0.3529411852359772f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.1411764770746231f, 0.1411764770746231f, 0.1411764770746231f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.1803921610116959f, 0.1803921610116959f, 0.1803921610116959f, 0.5882353186607361f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.2196078449487686f, 0.2196078449487686f, 0.2196078449487686f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.2196078449487686f, 0.2196078449487686f, 0.2196078449487686f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.4666666686534882f, 0.7686274647712708f, 0.8274509906768799f, 0.0470588244497776f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.7882353067398071f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.4300000071525574f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3176470696926117f, 0.3372549116611481f, 0.3568627536296844f, 0.9019607901573181f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.08235294371843338f, 0.1019607856869698f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}