#include "PluginSDK.h"
#include "Color.h"

PluginSetup("Garen Tutorial");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* HarassMenu;
IMenu* Drawings;
IMenu* MiscMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* WHealth;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassE;
IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("Garen Tutorial");
	ComboMenu = MainMenu->AddMenu("Combo");
	HarassMenu = MainMenu->AddMenu("Harass");
	Drawings = MainMenu->AddMenu("Drawings");
	MiscMenu = MainMenu->AddMenu("Miscs");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboE = ComboMenu->CheckBox("Use E", true);
	ComboR = ComboMenu->CheckBox("Use R", true);

	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassW = HarassMenu->CheckBox("Use W", true);
	HarassE = HarassMenu->CheckBox("Use E", true);

	WHealth = MiscMenu->AddFloat("Use W when X Percent HP", 0, 100, 65);

	DrawReady = Drawings->CheckBox("Draw Ready Spells", true);
	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawW = Drawings->CheckBox("Draw W", true);
	DrawE = Drawings->CheckBox("Draw E", true);
	DrawR = Drawings->CheckBox("Draw R", true);

}
void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	E->SetOverrideRange(325);
	W->SetOverrideRange(600);
	R->SetOverrideRange(400);
}
void Combo()
{
	auto player = GEntityList->Player();
	auto t = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	if (Q->IsReady() && ComboQ->Enabled() && player->IsValidTarget(t, Q->Range()))
	{
		Q->CastOnPlayer();
	}
	if (W->IsReady() && ComboW->Enabled() && player->IsValidTarget(t, W->Range()) && (player->HealthPercent() <= WHealth->GetFloat()) );
	{
		W->CastOnPlayer();
	}
	if (E->IsReady() && ComboE->Enabled() && player->IsValidTarget(t, E->Range()))
	{
		//E->CastOnPosition(GGame->CursorPosition());
		E->CastOnPlayer();
		GOrbwalking->SetAttacksAllowed(false);
	}
	GOrbwalking->SetAttacksAllowed(true);
}
void Harass()
{
	auto player = GEntityList->Player();
	auto t = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
	if (Q->IsReady() && HarassQ->Enabled() && player->IsValidTarget(t, Q->Range()))
	{
		Q->CastOnPlayer();
	}
	if (W->IsReady() && HarassW->Enabled() && player->IsValidTarget(t, W->Range()) && (player->HealthPercent() <= WHealth->GetFloat()));
	{
		W->CastOnPlayer();
	}
	if (E->IsReady() && HarassE->Enabled() && player->IsValidTarget(t, E->Range()))
	{
		E->CastOnPosition(GGame->CursorPosition());
		//E->CastOnPlayer();
	}
}
void AutoR()
{
	auto player = GEntityList->Player();
	if (!R->IsReady() && !ComboR->Enabled())
		return;
	for (auto enemy : GEntityList->GetAllHeros(false, true))
	{
		auto RDamage = GHealthPrediction->GetKSDamage(enemy, kSlotR, 0, false);
		if (player->IsValidTarget(enemy, R->Range()) && enemy->GetHealth() <= RDamage)
		{
			R->CastOnTarget(enemy);
		}
	}
	
}



void Drawingss()
{
	auto player = GEntityList->Player();
	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range()); }
		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range()); }
		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range()); }
		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), R->Range()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), Q->Range()); }
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), W->Range()); }
		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), E->Range()); }
		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(player->GetPosition(), Vec4(225, 225, 0, 225), R->Range()); }
	}
	
}
PLUGIN_EVENT(void) OnRender()
{
	Drawingss();
}
PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
		
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	AutoR();
	
	
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GRender->NotificationEx(Color::LightBlue().Get(), 2, true, true, "Welcome to Garen Tutorial");
}
PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
}