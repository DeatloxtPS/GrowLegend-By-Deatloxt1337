#pragma once
#pragma warning(disable: 4018)
#pragma warning(disable: 4244)
#pragma warning(disable: 4101) // expected, sloppy work..
#pragma warning(disable: 4715) // keep an eye on this warning... (file not all control paths return a value)
#include "http.h"
#include "load_event_results.h"

int load_files() {
	try
	{
		ifstream load_config("config+/server_config/config.json");
		if (load_config.fail())
		{
			load_config.close();
			system("PAUSE");
			return -1;
		}
		json config_json;
		load_config >> config_json;
		load_config.close();
		for (int i = 0; i < config_json["creators"].size(); i++)
		{
			creators.push_back(config_json["creators"].at(i));
		}
		configPort = json_to_int(config_json["server_port"]);
		int http_port = json_to_int(config_json["http_port"]);
		server_ip = json_to_string(config_json["server_ip"]);
		server_name = json_to_string(config_json["server_name"]);
		server_discord = json_to_string(config_json["server_discord"]);
		bool debug = json_to_bool(config_json["debug"]);
		if (http_port == 0 || server_ip == "" || server_name == "" || server_discord == "")
		{
			system("PAUSE");
			return -1;
		}
		threads.push_back(std::thread(http::run, http_port, server_ip, configPort));
		if (debug) {
			cout_cch = true;
		}
	}
	catch (const std::out_of_range& e) {
		system("PAUSE");
		return -1;
	}
	try {
		string c_role_name = "";
		ifstream infile("config+/server_config/roles.xaml");
		int r_line = 0, working_with = 0;
		bool read = false, subscriber = false;
		if (infile.fail()) {
			infile.close();
			system("PAUSE");
			return -1;
		}
		for (string line; getline(infile, line);) {
			if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
				auto ex = explode("|", line);
				if (ex.at(0) == "<START>") {
					read = true;
					continue;
				}
				else if (ex.at(0) == "<SPACE>") {
					r_line = 0;
					continue;
				}
				else if (ex.at(0) == "<END>") {
					break;
				}
				if (read) {
					switch (r_line) {
					case 0:
					{
						c_role_name = ex.at(1);
						break;
					}
					case 1:
					{
						if (ex.at(1) == "none") {
							subscriber = true;
						}
						else {
							working_with = atoi(ex.at(1).c_str());
							role_name.insert({ c_role_name, working_with });
							role_nameko.insert({ working_with, c_role_name });
						}
						break;
					}
					case 2:
					{
						if (ex.at(1) == "none") {
							role_prefix.insert({ working_with, "" });
						}
						else {
							role_prefix.insert({ working_with, ex.at(1) });
						}
						break;
					}
					case 3:
					{
						if (subscriber) {
							sub_commands = explode(",", ex.at(1));
							subscriber = false;
						}
						else {
							role_commands.insert({ working_with, explode(",", ex.at(1)) });
						}
						break;
					}
					case 4:
					{
						if (ex.at(1) != "none") {
							role_chat.insert({ working_with, ex.at(1) });
						}
						break;
					}
					}
					r_line++;
				}
			}
		}infile.close();
	}
	catch (const std::out_of_range& e) {
		system("PAUSE");
		return -1;
	}
	try {
		string type = "";
		ifstream infile("config+/events/config_break_event.xaml");
		int r_line = 0, working_with = 0;
		bool read = false;
		if (infile.fail()) {
			infile.close();
			system("PAUSE");
			return -1;
		}
		for (string line; getline(infile, line);) {
			if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
				auto ex = explode("|", line);
				if (ex.at(0) == "<START>") {
					read = true;
					continue;
				}
				else if (ex.at(0) == "<SPACE>") {
					r_line = 0;
					continue;
				}
				else if (ex.at(0) == "<END>") {
					break;
				}
				if (read) {
					switch (r_line) {
					case 0:
					{
						type = ex.at(1);
						break;
					}
					case 1:
					{
						working_with = atoi(ex.at(1).c_str());
						break;
					}
					case 2:
					{if (type == "OnBreakEvent") {
						custom_gem_block.insert({ working_with, atoi(ex.at(1).c_str()) });
					}
					break;
					}
					case 3:
					{
						if (type == "OnBreakEvent") {
							if (atoi(ex.at(1).c_str()) != 0) {
								custom_drop_chance.insert({ working_with, atoi(ex.at(1).c_str()) });
							}
						}
						break;
					}
					case 4:
					{
						if (type == "OnBreakEvent") {
							if (custom_drop_chance.find(working_with) != custom_drop_chance.end()) {
								custom_drop_block.insert({ working_with, atoi(ex.at(1).c_str()) });
							}
						}break;
					}
					}
					r_line++;
				}
			}
		}infile.close();
	}
	catch (const std::out_of_range& e)
	{
		system("PAUSE");
		return -1;
	}
	try {
		ifstream infile("config+/server_config/performance.xaml");
		int r_line = 0;
		bool read = false;
		if (infile.fail()) {
			infile.close();
			system("PAUSE");
			return -1;
		}
		for (string line; getline(infile, line);)
		{
			if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
				auto ex = explode("|", line);
				if (ex.at(0) == "<START>") {
					read = true;
					continue;
				}
				else if (ex.at(0) == "<SPACE>") {
					r_line = 0;
					continue;
				}
				else if (ex.at(0) == "<END>") {
					break;
				}
				if (read) {
					switch (r_line) {
					case 0:
					{
						if (ex.at(0) == "visibleparticles" && ex.at(1) == "false") {
							visibleparticles = false;
						}
						break;
					}
					case 1:
					{
						if (ex.at(0) == "worldthreading") {
							worldthreading = atoi(ex.at(1).c_str());
							if (worldthreading != 12) ColoredText("World threading set to " + to_string(worldthreading) + " to change this edit performance.xaml", "yellow");
						}
						break;
					}
					case 2: {
						if (ex.at(0) == "autosave" && ex.at(1) == "false") {
							autosave = false;
						}
						break;
					}
					case 3: {
						if (ex.at(0) == "jammers" && ex.at(1) == "false") {
							jammers = false;
						}
						break;
					}
					}
					r_line++;
				}
			}
		}
		infile.close();
	}
	catch (const std::out_of_range& e) {
		system("PAUSE");
		return -1;
	}
	try {
		ifstream infile("config+/items.xaml");
		int r_line = 0, working_with = 0;
		bool read = false;
		if (infile.fail()) {
			infile.close();
			ColoredText("Failed to load items.xaml make sure it exists and the permissions are correct", "red");
			system("PAUSE");
			return -1;
		}
		for (string line; getline(infile, line);) {
			if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
				auto ex = explode("|", line);
				if (ex.at(0) == "<START>") {
					read = true;
					continue;
				}
				else if (ex.at(0) == "<SPACE>") {
					r_line = 0;
					continue;
				}
				else if (ex.at(0) == "<END>") {
					break;
				} if (read) {
					switch (r_line) {
					case 0:
					{
						if (ex.at(0) == "id") {
							working_with = atoi(ex.at(1).c_str());
						}
						break;
					}
					case 1:
					{
						if (ex.at(0) == "multiplegems") {
							if (atoi(ex.at(1).c_str()) > 1) {
								item_multiplier.insert({ working_with, atoi(ex.at(1).c_str()) });
							}
						}
						break;
					}
					case 2:
					{
						if (ex.at(0) == "onehit") {
							if (ex.at(1) == "true") {
								one_hit.insert({ working_with, true });
							}
						}
						break;
					}
					case 3:
					{
						if (ex.at(0) == "triplepunch") {
							if (ex.at(1) == "true") {
								triple_punch.insert({ working_with, true });
							}
						}
						break;
					}
					case 4:
					{
						if (ex.at(0) == "tripleplace") {
							if (ex.at(1) == "true") {
								triple_place.insert({ working_with, true });
							}
						}
						break;
					}
					case 5:
					{
						if (ex.at(0) == "explosive") {
							if (ex.at(1) == "true") {
								explosive.insert({ working_with, true });
								if (one_hit.find(working_with) == one_hit.end()) {
									one_hit.insert({ working_with, true });
								}
							}
						}
						break;
					}
					case 6:
					{
						if (ex.at(0) == "breakeffect") {
							break_effect.insert({ working_with, atoi(ex.at(1).c_str()) });
						}
						break;
					}
					}
					r_line++;
				}
			}
		}
		infile.close();
	}
	catch (const std::out_of_range& e) {
		system("PAUSE");
		return -1;
	}
	ifstream file("items.dat", std::ios::binary | std::ios::ate);
	int size = file.tellg();
	itemsDatSize = size;
	char* data = new char[size];
	file.seekg(0, std::ios::beg);
	if (file.read((char*)(data), size)) {
		itemsDat = new BYTE[60 + size];
		int MessageType = 0x4;
		int PacketType = 0x10;
		int NetID = -1;
		int CharState = 0x8;
		memset(itemsDat, 0, 60);
		memcpy(itemsDat, &MessageType, 4);
		memcpy(itemsDat + 4, &PacketType, 4);
		memcpy(itemsDat + 8, &NetID, 4);
		memcpy(itemsDat + 16, &CharState, 4);
		memcpy(itemsDat + 56, &size, 4);
		file.seekg(0, std::ios::beg);
		if (file.read((char*)(itemsDat + 60), size)) {
			uint8_t* pData;
			size = 0;
			const char filename[] = "items.dat";
			size = filesize(filename);
			pData = getA((string)filename, &size, false, false);
			itemdathash = HashString((unsigned char*)pData, size);
			file.close();
		}
	}
	else {
		system("PAUSE");
		return -1;
	}
	LoadEvents(true);
	{
		load_event_results();
	}
}