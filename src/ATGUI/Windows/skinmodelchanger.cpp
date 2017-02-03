#include "skinmodelchanger.h"

bool SkinModelChanger::showWindow = false;

static int page = 0;

static int originalModelCT = 42;
static int replacementModelCT = 42;
static char filterOriginalModelsCT[18];
static char filterReplacementModelsCT[18];

static int originalModelT = 59;
static int replacementModelT = 59;
static char filterOriginalModelsT[18];
static char filterReplacementModelsT[18];

static int modelCT = 1;
static int modelSkinCT = 1;

static int modelT = 1;
static int modelSkinT = 1;

bool isKnife(ItemDefinitionIndex index)
{
	switch(index)
	{
		case ItemDefinitionIndex::WEAPON_KNIFE:
		case ItemDefinitionIndex::WEAPON_KNIFE_T:
		case ItemDefinitionIndex::WEAPON_KNIFE_BAYONET:
		case ItemDefinitionIndex::WEAPON_KNIFE_FLIP:
		case ItemDefinitionIndex::WEAPON_KNIFE_GUT:
		case ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT:
		case ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET:
		case ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL:
		case ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY:
		case ItemDefinitionIndex::WEAPON_KNIFE_FALCHION:
		case ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE:
		case ItemDefinitionIndex::WEAPON_KNIFE_PUSH:
			return true;
		default:
			return false;
	}
}

bool isNotWeapon(ItemDefinitionIndex index)
{
	switch(index)
	{
		case ItemDefinitionIndex::INVALID:
		case ItemDefinitionIndex::WEAPON_C4:
		case ItemDefinitionIndex::WEAPON_FLASHBANG:
		case ItemDefinitionIndex::WEAPON_HEGRENADE:
		case ItemDefinitionIndex::WEAPON_INCGRENADE:
		case ItemDefinitionIndex::WEAPON_MOLOTOV:
		case ItemDefinitionIndex::WEAPON_SMOKEGRENADE:
		case ItemDefinitionIndex::WEAPON_DECOY:
			return true;
		default:
			return false;
	}
}

void TabButtons()
{
	const char* tabs[] = {
			"Models",
			"Skins",
	};

	int tabs_size = sizeof(tabs) / sizeof(tabs[0]);
	for (int i = 0; i < tabs_size; i++)
	{
		int distance = i == page ? 0 : i > page ? i - page : page - i;

		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
				Settings::UI::mainColor.Value.x - (distance * 0.06f),
				Settings::UI::mainColor.Value.y - (distance * 0.06f),
				Settings::UI::mainColor.Value.z - (distance * 0.06f),
				Settings::UI::mainColor.Value.w
		);

		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / tabs_size - 9, 0)))
			page = i;

		ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::mainColor;

		if (i < tabs_size - 1)
			ImGui::SameLine();
	}
}

void ModelsTab()
{
	ImGui::Checkbox("Enabled", &Settings::Skinchanger::Models::enabled);
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Text("Counter Terrorist Models");
		ImGui::BeginChild("CTColumn", ImVec2(0, 0), true);
		{
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Original");
				ImGui::PushItemWidth(-1);
					ImGui::InputText("##filterOriginalModelCT", filterOriginalModelsCT, IM_ARRAYSIZE(filterOriginalModelsCT));
				ImGui::PopItemWidth();
				ImGui::ListBoxHeader("##originalModelsCT", ImVec2(-1, 300));
					for (auto model : ItemDefinitionIndexMap)
					{
						if (!Util::Contains(Util::ToLower(std::string(filterOriginalModelsCT)), Util::ToLower(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())))))
							continue;
						if (model.first != ItemDefinitionIndex::WEAPON_KNIFE && model.first != ItemDefinitionIndex::GLOVE_CT_SIDE)
							continue;
						const bool item_selected = ((int) model.first == originalModelCT);
						ImGui::PushID((int)model.first);
							if (ImGui::Selectable(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())).c_str(), item_selected))
							{
								originalModelCT = (int)model.first;
								replacementModelCT = (int)Settings::Skinchanger::skinsCT.at(model.first).itemDefinitionIndex;
							}
						ImGui::PopID();
					}
				ImGui::ListBoxFooter();
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Replacement");
				ImGui::PushItemWidth(-1);
					ImGui::InputText("##filterReplacementModelCT", filterReplacementModelsCT, IM_ARRAYSIZE(filterReplacementModelsCT));
				ImGui::PopItemWidth();
				ImGui::ListBoxHeader("##replacementModelsCT", ImVec2(-1, 300));
					for (auto model : ItemDefinitionIndexMap)
					{
						if (!Util::Contains(Util::ToLower(std::string(filterReplacementModelsCT)), Util::ToLower(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())))))
							continue;
						switch(originalModelCT)
						{
							case (int) ItemDefinitionIndex::WEAPON_KNIFE:
								if(!isKnife(model.first))
								continue;
								break;
							case (int) ItemDefinitionIndex::GLOVE_CT_SIDE:
								if (model.first < ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND || model.first == ItemDefinitionIndex::GLOVE_T_SIDE)
									continue;
								break;
							default:
								break;
						}
						const bool item_selected = ((int) model.first == replacementModelCT);
						ImGui::PushID((int)model.first);
							if (ImGui::Selectable(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())).c_str(), item_selected))
								replacementModelCT = (int)model.first;
						ImGui::PopID();
					}
				ImGui::ListBoxFooter();
			}
			ImGui::Columns(1);
			if(ImGui::Button("Apply", ImVec2(-1, 0)))
			{
				if (Settings::Skinchanger::skinsCT.find((ItemDefinitionIndex)originalModelCT) == Settings::Skinchanger::skinsCT.end())
					Settings::Skinchanger::skinsCT[(ItemDefinitionIndex)originalModelCT] = AttribItem_t();

				Settings::Skinchanger::skinsCT.at((ItemDefinitionIndex)originalModelCT).itemDefinitionIndex = (ItemDefinitionIndex) replacementModelCT;

				SkinChanger::forceFullUpdate = true;
			}
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::Text("Terrorist Models");
		ImGui::BeginChild("TColumn", ImVec2(0, 0), true);
		{
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Original");
				ImGui::PushItemWidth(-1);
					ImGui::InputText("##filterOriginalModelT", filterOriginalModelsT, IM_ARRAYSIZE(filterOriginalModelsT));
				ImGui::PopItemWidth();
				ImGui::ListBoxHeader("##originalModelsT", ImVec2(-1, 300));
					for (auto model : ItemDefinitionIndexMap)
					{
						if (!Util::Contains(Util::ToLower(std::string(filterOriginalModelsT)), Util::ToLower(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())))))
							continue;
						if (model.first != ItemDefinitionIndex::WEAPON_KNIFE_T && model.first != ItemDefinitionIndex::GLOVE_T_SIDE)
							continue;
						const bool item_selected = ((int) model.first == originalModelT);
						ImGui::PushID((int)model.first);
							if (ImGui::Selectable(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())).c_str(), item_selected))
							{
								originalModelT = (int)model.first;
								replacementModelT = (int)Settings::Skinchanger::skinsT.at(model.first).itemDefinitionIndex;
							}
						ImGui::PopID();
					}
				ImGui::ListBoxFooter();
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Replacement");
				ImGui::PushItemWidth(-1);
					ImGui::InputText("##filterReplacementModelT", filterReplacementModelsT, IM_ARRAYSIZE(filterReplacementModelsT));
				ImGui::PopItemWidth();
				ImGui::ListBoxHeader("##replacementModelsT", ImVec2(-1, 300));
					for (auto model : ItemDefinitionIndexMap)
					{
						if (!Util::Contains(Util::ToLower(std::string(filterReplacementModelsT)), Util::ToLower(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())))))
							continue;
						switch(originalModelT)
						{
							case (int) ItemDefinitionIndex::WEAPON_KNIFE_T:
								if(!isKnife(model.first))
									continue;
								break;
							case (int) ItemDefinitionIndex::GLOVE_T_SIDE:
								if (model.first < ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND || model.first == ItemDefinitionIndex::GLOVE_CT_SIDE)
									continue;
								break;
							default:
								break;
						}
						const bool item_selected = ((int) model.first == replacementModelT);
						ImGui::PushID((int)model.first);
						if (ImGui::Selectable(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())).c_str(), item_selected))
							replacementModelT = (int)model.first;
						ImGui::PopID();
					}
				ImGui::ListBoxFooter();
			}
			ImGui::Columns(1);
			if(ImGui::Button("Apply", ImVec2(-1, 0)))
			{
				if (Settings::Skinchanger::skinsT.find((ItemDefinitionIndex)originalModelT) == Settings::Skinchanger::skinsT.end())
					Settings::Skinchanger::skinsT[(ItemDefinitionIndex)originalModelT] = AttribItem_t();

				Settings::Skinchanger::skinsT.at((ItemDefinitionIndex)originalModelT).itemDefinitionIndex = (ItemDefinitionIndex) replacementModelT;

				SkinChanger::forceFullUpdate = true;
			}
			ImGui::EndChild();
		}
	}
}

void SkinsTab()
{
	if(ImGui::Checkbox("Enabled", &Settings::Skinchanger::Skins::enabled))
		SkinChanger::forceFullUpdate = true;

	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Text("Counter Terrorist Skins");
		ImGui::BeginChild("CTColumn", ImVec2(0, 0), true);
		{
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Model");
				ImGui::ListBoxHeader("##modelsCT", ImVec2(-1, 300));
				for (auto model : ItemDefinitionIndexMap)
				{
//					if (!Util::Contains(Util::ToLower(std::string(filterOriginalModelsCT)), Util::ToLower(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())))))
//						continue;
					if (isNotWeapon(model.first))
						continue;
					const bool item_selected = ((int) model.first == modelCT);
					ImGui::PushID((int)model.first);
					if (ImGui::Selectable(Util::WstringToString(localize->FindSafe(Util::Items::GetItemDisplayName(model.first).c_str())).c_str(), item_selected))
					{
						modelCT = (int)model.first;
						modelSkinCT = (int)Settings::Skinchanger::skinsCT.at(model.first).fallbackPaintKit;
					}
					ImGui::PopID();
				}
				ImGui::ListBoxFooter();
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Skins");
				ImGui::ListBoxHeader("##modelSkinsCT", ImVec2(-1, 300));
					for (auto skin : weaponSkins)
					{
	//					if (!Util::Contains(Util::ToLower(std::string(filterSkins)), Util::ToLower(std::string(skin.second))))
	//						continue;
						const bool item_selected = (skin.first == modelSkinCT);
						ImGui::PushID(skin.first);
							if (ImGui::Selectable(skin.second, item_selected))
								modelSkinCT = skin.first;
						ImGui::PopID();
					}
				ImGui::ListBoxFooter();
			}
			ImGui::Columns(1);
			if(ImGui::Button("Apply", ImVec2(-1, 0)))
			{
				if (Settings::Skinchanger::skinsCT.find((ItemDefinitionIndex)modelCT) == Settings::Skinchanger::skinsCT.end())
					Settings::Skinchanger::skinsCT[(ItemDefinitionIndex)modelCT] = AttribItem_t();

				Settings::Skinchanger::skinsCT.at((ItemDefinitionIndex)modelCT).fallbackPaintKit = modelSkinCT;

				SkinChanger::forceFullUpdate = true;
			}

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::Text("Terrorist Skins");
		ImGui::BeginChild("TColumn", ImVec2(0, 0), true);
		{
			ImGui::Text("Skin");



			ImGui::EndChild();
		}
	}
}

void SkinModelChanger::RenderWindow()
{
	if(!SkinModelChanger::showWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(960, 645), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Skin And Model Changer", &SkinModelChanger::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
	{
		TabButtons();
		ImGui::Separator();
		switch (page)
		{
			case 0:
				ModelsTab();
				break;
			case 1:
				SkinsTab();
				break;
		}
		ImGui::End();
	}
};