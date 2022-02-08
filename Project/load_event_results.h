#pragma once
#include "core.h"

inline void load_event_results() {
	{
		ifstream read_entry("config+/events/SummerFestEvent.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool SummerFestEvent_json = j;
		SummerFestEvent = SummerFestEvent_json;
	}
	{
		ifstream read_entry("config+/events/ValentineEvent.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool ValentineEvent_json = j;
		ValentineEvent = ValentineEvent_json;
	}
	{
		ifstream read_entry("config+/events/LunarEvent.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool LunarEvent_json = j;
		LunarEvent = LunarEvent_json;
	}
	{
		ifstream read_entry("config+/events/GrowganothEvent.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool GrowganothEvent_json = j;
		GrowganothEvent = GrowganothEvent_json;
	}
	{
		ifstream read_entry("config+/events/SurgeryDay.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool SurgeryDay_json = j;
		SurgeryDay = SurgeryDay_json;
	}
	{
		ifstream read_entry("config+/events/GeigerDay.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool GeigerDay_json = j;
		GeigerDay = GeigerDay_json;
	}
	{
		ifstream read_entry("config+/events/HarvestEvent.json");
		json j;
		read_entry >> j;
		read_entry.close();
		bool HarvestEvent_json = j;
		HarvestEvent = HarvestEvent_json;
	}
}