#pragma once
#include "tile.h"

inline void SendChat(ENetPeer* peer, const int netID, string message, WorldInfo* world, string cch) {
	try {
		if (message.length() >= 90 || message.length() == 0 || message == " " || !static_cast<PlayerInfo*>(peer->data)->isIn || static_cast<PlayerInfo*>(peer->data)->currentWorld == "EXIT" || 1 > (message.size() - countSpaces(message))) return;
		if (!static_cast<PlayerInfo*>(peer->data)->haveGrowId) {
			Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "Please create growid first!", 0, true);
			return;
		}
		if (message == "gay" || message == "nigga" || message == "dick" || message == "ass" || message == "retard") {
			Player::OnConsoleMessage(peer, "`4Anti-Toxic >> Don't use that word!");
			return;
		}
		removeExtraSpaces(message);
		string str = message;

		if (str.rfind("/", 0) == 0) {
			if (static_cast<PlayerInfo*>(peer->data)->trade) end_trade(peer);
			if (str != "/togglemod" && !has_permission(static_cast<PlayerInfo*>(peer->data)->adminLevel, str, static_cast<PlayerInfo*>(peer->data)->Subscriber)) {
				sendWrongCmd(peer);
				return;
			}
		}

		if (str == "/ghost" || str == "/mod") {
			if (world->width == 90 && world->height == 60) {
				Player::OnConsoleMessage(peer, "You can't use that command here.");
				return;
			}
			SendGhost(peer);
		}
		else if (str.substr(0, 5) == "/ans ") {
			string ban_info = str;
			size_t extra_space = ban_info.find("  ");
			if (extra_space != std::string::npos)
			{
				ban_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string ban_user;
			string ban_time;
			if ((pos = ban_info.find(delimiter)) != std::string::npos)
			{
				ban_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oUsage: /ans <user> <answer text>"));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
				return;
			}
			if ((pos = ban_info.find(delimiter)) != std::string::npos)
			{
				ban_user = ban_info.substr(0, pos);
				ban_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oUsage: /ans <user> <answer text>"));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
				return;
			}
			ban_time = ban_info;
			string playerName = PlayerDB::getProperName(ban_user);
			string answerText = ban_time;
			bool sucanswered = false;
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
					continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == playerName)
				{
					Player::OnConsoleMessage(currentPeer, "`9[`#" + server_name + " `#STAFF`9]`w: `3Administrator `2" + static_cast<PlayerInfo*>(peer->data)->rawName + "`3 just answered to your question`w:`2 " + answerText + "");
					Player::OnConsoleMessage(peer, "`2You successfully answered to `8" + playerName + "'s `2question.");
					sucanswered = true;

					string textInfo = "`1[M] `1[`o" + currentDateTime() + "`1] `6" + static_cast<PlayerInfo*>(peer->data)->tankIDName + " `4Just `2ANSWERD `8to `4player's `w" + playerName + " `6 question. `4The answer: `2 " + answerText + "";
					showModLogs(textInfo);

					break;
				}
			}
			if (!sucanswered)
			{
				Player::OnConsoleMessage(peer, "`4The player `2" + playerName + " `4 is not online.");
			}
		}
		else if (str == "/mods") {
			string x = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (mod(currentPeer) && !static_cast<PlayerInfo*>(currentPeer->data)->isinv && !static_cast<PlayerInfo*>(currentPeer->data)->isNicked || admin(peer) && mod(currentPeer)) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->isNicked && admin(peer)) {
						x.append("" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + " (" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + ")`w, ");
					}
					else {
						x.append("" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w, ");
					}
				}
			}
			x = x.substr(0, x.length() - 2);
			if (x == "") x = "(All are hidden)";
			Player::OnConsoleMessage(peer, "`oMods online: " + x);
		}

		else if (str == "/owners") {
			string x = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (mod(currentPeer) && !static_cast<PlayerInfo*>(currentPeer->data)->isinv && !static_cast<PlayerInfo*>(currentPeer->data)->isNicked || admin(peer) && mod(currentPeer)) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->isNicked && admin(peer)) {
						x.append("" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + " (" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + ")`w, ");
					}
					else {
						x.append("" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w, ");
					}
				}
			}
			x = x.substr(0, x.length() - 2);
			if (x == "") x = "(All are hidden)";
			Player::OnConsoleMessage(peer, "`oOwners online: " + x);
		}
		else if (str == "/go") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed == true) return;
			if (static_cast<PlayerInfo*>(peer->data)->lastSbbWorld == "") {
				Player::OnTextOverlay(peer, "Unable to track down the location of the message.");
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->lastSbbWorld == static_cast<PlayerInfo*>(peer->data)->currentWorld) {
				Player::OnTextOverlay(peer, "Sorry, but you are already in the world!");
				return;
			}
			handle_world(peer, static_cast<PlayerInfo*>(peer->data)->lastSbbWorld);
		}
		else if (str == "/online") {
			string online = "";
			int total = 0;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->adminLevel >= 0 && static_cast<PlayerInfo*>(currentPeer->data)->isinv == false) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->displayName == "" || static_cast<PlayerInfo*>(currentPeer->data)->rawName == "") continue;
					online += static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o ";
					total++;
				}
			}
			Player::OnConsoleMessage(peer, "`$Current Online Players (" + to_string(total) + "): `o" + online);
		}
		else if (str == "/kickall")
		{
			if (static_cast<PlayerInfo*>(peer->data)->lastDM + 60000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
			{
				static_cast<PlayerInfo*>(peer->data)->lastDM = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
				if (mod(peer) || static_cast<PlayerInfo*>(peer->data)->rawName == world->owner)
				{
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer))
						{
							if (static_cast<PlayerInfo*>(currentPeer->data)->rawName != world->owner && !isWorldAdmin(peer, world) && !mod(peer))
							{
								Player::OnConsoleMessage(currentPeer, "`@You have been kicked by `2" + static_cast<PlayerInfo*>(peer->data)->rawName + "!");
								playerRespawn(world, currentPeer, false);
							}
						}
					}
				}
				else {
					sendWrongCmd(peer);
					return;
				}
			}
			else {
				Player::OnConsoleMessage(peer, ">> `4Wait 1 Minutes before kicking everyone, again!");
			}
		}
		else if (str.substr(0, 6) == "/pull ")
		{
			if (!isWorldOwner(peer, world) && !isWorldAdmin(peer, world) && !mod(peer)) return;
			string pull_name = PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 1));
			if (pull_name.size() < 3) {
				Player::OnConsoleMessage(peer, "You'll need to enter at least the first three characters of the person's name.");
				return;
			}
			bool Found = false, Block = false;
			int Same_name = 0, Sub_worlds_name = 0;
			string Intel_sense_nick = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(pull_name)) != string::npos) Same_name++;
				}
				else if (mod(peer))
				{
					if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(pull_name)) != string::npos) Sub_worlds_name++;
				}
			}
			if (Same_name > 1)
			{
				Player::OnConsoleMessage(peer, "`oThere are more than two players in this world starting with " + pull_name + " `obe more specific!");
				return;
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer) && getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(pull_name)) != string::npos)
				{
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->rawName)
					{
						Player::OnConsoleMessage(peer, "Ouch!");
						Block = true;
						break;
					}
					Found = true;
					Intel_sense_nick = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
					PlayerMoving data{};
					data.packetType = 0x0;
					data.characterState = 0x924;
					data.x = static_cast<PlayerInfo*>(peer->data)->x;
					data.y = static_cast<PlayerInfo*>(peer->data)->y;
					data.punchX = -1;
					data.punchY = -1;
					data.XSpeed = 0;
					data.YSpeed = 0;
					data.netID = static_cast<PlayerInfo*>(currentPeer->data)->netID;
					data.plantingTree = 0x0;
					SendPacketRaw(4, packPlayerMoving(&data), 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
					GamePacket p2 = packetEnd(appendFloat(appendString(createPacket(), "OnSetPos"), static_cast<PlayerInfo*>(peer->data)->x, static_cast<PlayerInfo*>(peer->data)->y));
					memcpy(p2.data + 8, &(static_cast<PlayerInfo*>(currentPeer->data)->netID), 4);
					ENetPacket* packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet2);
					delete p2.data;
					if (isWorldOwner(peer, world)) Player::OnTextOverlay(currentPeer, "You were pulled by " + static_cast<PlayerInfo*>(peer->data)->displayName + "");
					break;
				}
			}
			if (Block) return;
			if (!Found && mod(peer))
			{
				if (Sub_worlds_name > 1)
				{
					Player::OnConsoleMessage(peer, "`oThere are more than two players in the server starting with " + pull_name + " `obe more specific!");
					return;
				}
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(pull_name)) != string::npos)
					{
						Found = true;
						Intel_sense_nick = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
						handle_world(currentPeer, static_cast<PlayerInfo*>(peer->data)->currentWorld, false, false, "", true, static_cast<PlayerInfo*>(peer->data)->x, static_cast<PlayerInfo*>(peer->data)->y);
					}
				}
			}
			if (Found)
			{
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer))
					{
						Player::PlayAudio(currentPeer, "audio/object_spawn.wav", 0);
						Player::OnConsoleMessage(currentPeer, "`o" + static_cast<PlayerInfo*>(peer->data)->displayName + " `5pulls `o" + Intel_sense_nick + "`o!");
					}
				}
			}
			else
			{
				if (mod(peer))
				{
					Player::OnConsoleMessage(peer, "`4Oops:`` There is nobody currently in this server with a name starting with `w" + pull_name + "``.");
				}
				else
				{
					Player::OnConsoleMessage(peer, "`4Oops:`` There is nobody currently in this world with a name starting with `w" + pull_name + "``.");
				}
			}
		}
		else if (str.substr(0, 4) == "/me ")
		{
			if (world->silence == true && !isWorldOwner(peer, world))
			{
				Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`1(Peasants must not to talk)", 0, true);
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == false && static_cast<PlayerInfo*>(peer->data)->haveGrowId == true)
			{
				string namer = static_cast<PlayerInfo*>(peer->data)->displayName;
				GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), static_cast<PlayerInfo*>(peer->data)->netID), "`#<`w" + namer + " `#" + str.substr(3, cch.length() - 3 - 1).c_str() + "`5>"), 0));
				ENetPacket* packet2 = enet_packet_create(p2.data,
					p2.len,
					ENET_PACKET_FLAG_RELIABLE);
				GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`w<" + namer + " `#" + str.substr(3, cch.length() - 3 - 1).c_str() + "`w>"));
				ENetPacket* packet3 = enet_packet_create(p3.data,
					p3.len,
					ENET_PACKET_FLAG_RELIABLE);
				ENetPeer* currentPeer;
				for (currentPeer = server->peers;
					currentPeer < &server->peers[server->peerCount];
					++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
						continue;
					if (isHere(peer, currentPeer))
					{
						enet_peer_send(currentPeer, 0, packet2);
						enet_peer_send(currentPeer, 0, packet3);
					}
				}
				delete p2.data;
				delete p3.data;
				return;
			}
		}
		else if (str.substr(0, 6) == "/warn ") {
			string warn_info = str;
			size_t extra_space = warn_info.find("  ");
			if (extra_space != std::string::npos) warn_info.replace(extra_space, 2, " ");
			string delimiter = " ";
			size_t pos = 0;
			string warn_user;
			string warn_message;
			if ((pos = warn_info.find(delimiter)) != std::string::npos) warn_info.erase(0, pos + delimiter.length());
			else return;
			if ((pos = warn_info.find(delimiter)) != std::string::npos) {
				warn_user = warn_info.substr(0, pos);
				warn_info.erase(0, pos + delimiter.length());
			}
			else return;
			warn_message = warn_info;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == warn_user) {
					Player::OnAddNotification(currentPeer, "`wWarning from `4System`w: " + warn_message + "", "audio/hub_open.wav", "interface/atomic_button.rttex");
					Player::OnConsoleMessage(currentPeer, "`oWarning from `4System`o: " + warn_message + "");
					break;
				}
			}
			Player::OnConsoleMessage(peer, "Warning sent (only works if the player is online)");
		}
		else if (str == "/rules") {
			Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wHelp & Rules``|left|18|\nadd_spacer|small|\nadd_textbox|`wTo keep this place fun we've got some ground rules to check out:``|left|\nadd_spacer|small|\nadd_label_with_icon|small|`wKeep your password secret. Sharing your password will result in stolen items.``|left|24|\nadd_label_with_icon|small|`wUnprotected items are at risk of being stolen. Use doors, locks, and blocks wisely to protect your items.``|left|24|\nadd_label_with_icon|small|`wWe cannot return stolen items, so BE CAREFUL!``|left|24|\nadd_label_with_icon|small|`wBe civil. Bullying, racism, excessive profanity, sexual content, and abusive behavior are not allowed.``|left|24|\nadd_label_with_icon|small|`wPlayers that are harmful to the community may be banned. This includes accounts that use lies, fake games, or trickery to mistreat other players.``|left|24|\nadd_label_with_icon|small|`wPunishments may be applied to alt accounts as well as any devices and IP addresses used by the offender.``|left|24|\nadd_label_with_icon|small|`wTrying to get punished or asking for a punishment can earn you a worse punishment.``|left|24|\nadd_label_with_icon|small|`w'Hacking' the game - including client-side manipulation, auto-clickers, speed hacks, clock manipulation, bots, macroing, and autofarming - will result in a ban.``|left|24|\nadd_label_with_icon|small|`wDon't lie about mods or fake official Growtopia system messages.``|left|24|\nadd_label_with_icon|small|`w'Drop games' are illegal scams. You can be punished for hosting, playing, advertising or even watching.``|left|24|\nadd_label_with_icon|small|`wBetting or gambling (asking players to pay for a chance to win something) is not allowed.``|left|24|\nadd_label_with_icon|small|`wIf you find a world or player name that is in violation of our rules, message a @Mod. Do not /bc or /sb from an inappropriate world.``|left|24|\nadd_label_with_icon|small|`w@Moderators are here to enforce the rules. Abusing, spamming or harassing mods will have consequences.``|left|24|\nadd_label_with_icon|small|`wAccounts, locks, or worlds may be deleted after long periods of inactivity.``|left|24|\nadd_label_with_icon|small|`wOut of game buying/selling of Growtopia accounts, items or worlds is not allowed and may result in a ban.``|left|24|\nadd_label_with_icon|small|`wIAP fraud will result in permanent suspension.``|left|24|\nadd_label_with_icon|small|`wItem function and prices are subject to change without warning.``|left|24|\nadd_label_with_icon|small|`wYour creations and items could be deleted at any time. We do everything in our power to prevent this, but data loss may happen without warning. Lost items cannot be restored.``|left|24|\nadd_spacer|small|\nadd_label_with_icon|small|`wFor technical support, IAP issues, or to report a problem, contact `6@Developers``|left|24|\nadd_spacer|small|\nend_dialog|popup||Continue|");
		}
		else if (str.substr(0, 3) == "/r ") {
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true) return;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (currentPeer->data == nullptr) {
					ColoredText("currentPeer was nullptr", "red");
					continue;
				}
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->lastMsger) {
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsger = static_cast<PlayerInfo*>(peer->data)->rawName;
					Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o)");
					Player::OnConsoleMessage(currentPeer, "CP:_PL:0_OID:_CT:[MSG]_ `c>> from (`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`c) in [`o" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "`c] > `o" + str.substr(3, cch.length() - 3 - 1));
					Player::PlayAudio(currentPeer, "audio/pay_time.wav", 0);
					break;
				}
			}
		}
		else if (str.substr(0, 4) == "/rs ") {
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true) return;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (currentPeer->data == nullptr) {
					ColoredText("currentPeer was nullptr", "red");
					continue;
				}
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->lastMsger) {
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsger = static_cast<PlayerInfo*>(peer->data)->rawName;
					Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o)");
					Player::OnConsoleMessage(currentPeer, "CP:_PL:0_OID:_CT:[MSG]_ `c>> from (`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`c) in [`4<HIDDEN>`c] > `o" + str.substr(4, cch.length() - 4 - 1));
					Player::PlayAudio(currentPeer, "audio/pay_time.wav", 0);
					break;
				}
			}
		}
		else if (str == "/rgo") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed == true) return;
			if (static_cast<PlayerInfo*>(peer->data)->lastMsgWorld == "") {
				Player::OnTextOverlay(peer, "Unable to track down the location of the message.");
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->lastMsgWorld == static_cast<PlayerInfo*>(peer->data)->currentWorld) {
				Player::OnTextOverlay(peer, "Sorry, but you are already in the world!");
				return;
			}
			handle_world(peer, static_cast<PlayerInfo*>(peer->data)->lastMsgWorld);
		}
		else if (str == "/status") {
			Player::OnConsoleMessage(peer, "`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "````'s Status:");
			Player::OnConsoleMessage(peer, "Current world: `w" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "`` (`w" + to_string(static_cast<PlayerInfo*>(peer->data)->x / 32) + "``, `w" + to_string(static_cast<PlayerInfo*>(peer->data)->y / 32) + "``) (`w" + to_string(getPlayersCountInWorld(static_cast<PlayerInfo*>(peer->data)->currentWorld)) + "`` people) Backpack slots: `w" + to_string(static_cast<PlayerInfo*>(peer->data)->currentInventorySize) + "``");
			string visited = "";
			try {
				for (int i = 0; i < static_cast<PlayerInfo*>(peer->data)->lastworlds.size(); i++) {
					if (i == static_cast<PlayerInfo*>(peer->data)->lastworlds.size() - 1) {
						visited += "`#" + static_cast<PlayerInfo*>(peer->data)->lastworlds.at(i) + "``";
					}
					else {
						visited += "`#" + static_cast<PlayerInfo*>(peer->data)->lastworlds.at(i) + "``, ";
					}
				}
			}
			catch (const std::out_of_range& e) {
				std::cout << e.what() << std::endl;
			}
			Player::OnConsoleMessage(peer, "Last visited: " + visited);
		}
		else if (str.substr(0, 9) == "/setchat ") {
			string chatcode = (str.substr(9).c_str());
			if (chatcode.size() >= 2 || chatcode.size() <= 0) return;
			if (chatcode == "o") chatcode = "";
			static_cast<PlayerInfo*>(peer->data)->chatcolor = chatcode;
			if (chatcode != "") Player::OnConsoleMessage(peer, "`oYour chat color have been changed to `" + chatcode + "color`o!");
			else Player::OnConsoleMessage(peer, "`oYour chat color have been reverted to default!");
		}
		else if (str.substr(0, 6) == "/give ") {
			string say_info = str;
			size_t extra_space = say_info.find("  ");
			if (extra_space != std::string::npos) say_info.replace(extra_space, 2, " ");
			string delimiter = " ";
			size_t pos = 0;
			string item_id;
			string item_count;
			if ((pos = say_info.find(delimiter)) != std::string::npos) {
				say_info.erase(0, pos + delimiter.length());
			} if ((pos = say_info.find(delimiter)) != std::string::npos) {
				item_id = say_info.substr(0, pos);
				say_info.erase(0, pos + delimiter.length());
			}
			item_count = say_info;
			if (item_id == "" && item_count != "") {
				bool contains_non_int2 = !std::regex_match(item_count, std::regex("^[0-9]+$"));
				if (contains_non_int2 == true) {
					return;
				}
				if (item_count.length() > 5) {
					Player::OnConsoleMessage(peer, "`oThis item does not exist");
					return;
				}
				int item_count_give = atoi(item_count.c_str());
				if (item_count_give > maxItems || item_count_give < 0 || item_count_give == 1424) {
					Player::OnConsoleMessage(peer, "`oThis item does not exist");
					return;
				}
				if (CheckItemMaxed(peer, item_count_give, 1) || static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize && CheckItemExists(peer, item_count_give) && CheckItemMaxed(peer, item_count_give, 1) || static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize && !CheckItemExists(peer, item_count_give)) {
					Player::OnConsoleMessage(peer, "`o" + getItemDef(item_count_give).name + " wont fit into my inventory!");
					return;
				}
				Player::OnConsoleMessage(peer, "`oYou got 1 '`$" + getItemDef(item_count_give).name + "`o'.");
				bool success = true;
				SaveItemMoreTimes(item_count_give, 1, peer, success);
				SendTradeEffect(peer, item_count_give, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->netID, 150);
				return;
			}
			bool contains_non_int2 = !std::regex_match(item_id, std::regex("^[0-9]+$"));
			if (contains_non_int2 == true) {
				return;
			}
			PlayerInventory inventory;
			if (item_id.length() > 5) {
				Player::OnConsoleMessage(peer, "`oThis item does not exist");
				return;
			}
			int item_id_give = atoi(item_id.c_str());
			if (item_id_give > maxItems || item_id_give < 0 || item_id_give == 1424) {
				Player::OnConsoleMessage(peer, "`oThis item does not exist");
				return;
			}
			int item_count_give = 1;
			if (item_count != "") {
				bool contains_non_int2 = !std::regex_match(item_count, std::regex("^[0-9]+$"));
				if (contains_non_int2 == true) {
					return;
				}
				item_count_give = atoi(item_count.c_str());
				if (item_count_give > 200 || item_count_give <= 0) {
					Player::OnConsoleMessage(peer, "`oItem count must be between 0 and 200");
					return;
				}
			}
			if (CheckItemMaxed(peer, item_id_give, item_count_give) || static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize && CheckItemExists(peer, item_id_give) && CheckItemMaxed(peer, item_id_give, item_count_give) || static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize && !CheckItemExists(peer, item_id_give)) {
				Player::OnConsoleMessage(peer, "`o" + getItemDef(item_id_give).name + " wont fit into my inventory!");
				return;
			}
			Player::OnConsoleMessage(peer, "`oYou got " + to_string(item_count_give) + " '`$" + getItemDef(item_id_give).name + "`o'.");
			bool success = true;
			SaveItemMoreTimes(item_id_give, item_count_give, peer, success);
			SendTradeEffect(peer, item_id_give, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->netID, 150);
		}
		else if (str.substr(0, 10) == "/gemevent ")
		{
			string multi = (str.substr(10).c_str());
			int num = atoi(multi.c_str());
			if (num < 0 || num > 5)
			{
				Player::OnConsoleMessage(peer, "Multiplier must be between 0 and 5!");
				return;
			}
			gem_multiplier = num;
			if (num == 0)
			{
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					Player::OnConsoleMessage(currentPeer, "`5Event Broadcast `o>> Gem Multiplier has ended!");
					Player::PlayAudio(currentPeer, "audio/loser.wav", 0);
				}
			}
			else
			{
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					Player::OnConsoleMessage(currentPeer, "`5Event Broadcast `o>> `2" + to_string(num) + "x `oGem Multiplier Started!");
					Player::PlayAudio(currentPeer, "audio/levelup.wav", 0);
				}
			}
		}
		else if (str == "/test1") {
			Player::OnDialogRequest(peer, "add_label_with_icon|big|`1Punish player``|left|1432|\n\nadd_spacer|small|\n\nadd_textbox|`wAre you sure you want to ban`` `2```w?``|left|\n\nadd_textbox|`rMake sure you have proof and you have discord role on discord group, wrong bans will result in demoting.``|\n\nadd_textbox|`4If you don't post proof image/video in #ban-channel in 12hours, you will be instantly demoted!``|left|\nadd_textbox|Select ban length.|left|\nmax_checks|2|\ntext_scaling_string|Ban time|\nadd_checkicon|1day|1 Day||276||0|\nadd_checkicon|7day|7 Days||276||0|\nadd_checkicon|31day|31 Days||276||0|\nadd_checkicon|a|Forever||278||0|\nadd_button_with_icon||END_LIST|noflags|0||\nadd_spacer|\n\nadd_textbox|`oReason``|\nadd_text_input|bantext|||65|\nadd_checkbox|checkbox_public|`2Allowed for ban appealing?``|1|\nend_dialog|bansubmit|Cancel|`4Ban from game``|");
		}
		else if (str == "/test") {
			ifstream test("config+/server_config/test.txt");
			stringstream buffer;
			buffer << test.rdbuf();
			string testString(buffer.str());
			Player::OnDialogRequest(peer, testString);
		}
		else if (str.substr(0, 4) == "/id ") {
			bool ScanNotFull = false;
			string item = (str.substr(4).c_str());
			vector<string> FoundItems;
			int foundkiek = 0;
			for (int i = 0; i < maxItems; i++) {
				string iname = getStrLower(getItemDef(i).name);
				if (iname.find(getStrLower(item)) != string::npos && !isSeed(i)) {
					if (foundkiek >= 100) {
						ScanNotFull = true;
						break;
					}
					foundkiek++;
					FoundItems.push_back("" + to_string(i) + ": " + getItemDef(i).name + "");
				}
			}
			Player::OnConsoleMessage(peer, "Items that matched the criteria:");
			if (FoundItems.size() == 0) {
				Player::OnConsoleMessage(peer, "no items found");
			}
			else {
				for (int i = 0; i < FoundItems.size(); i++) {
					Player::OnConsoleMessage(peer, FoundItems.at(i));
				}
			}
			if (ScanNotFull) Player::OnConsoleMessage(peer, "Aborting search because found over 100+ items");
		}
		else if (str == "/regenerate") {
			threads.push_back(std::thread(restore_prices));
		}
		else if (str == "/news") {
			std::ifstream news("config+/server_config/news.txt");
			std::stringstream buffer;
			buffer << news.rdbuf();
			std::string newsString(buffer.str());
			Player::OnDialogRequest(peer, newsString);
		}
		else if (str == "/togglemod")
		{
			try {
				ifstream read_player("save/players/_" + static_cast<PlayerInfo*>(peer->data)->rawName + ".json");
				if (!read_player.is_open())
				{
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				int adminLevel = j["adminLevel"];
				string nick = j["nick"];
				if (adminLevel < 1) {
					sendWrongCmd(peer);
					return;
				}
				if (static_cast<PlayerInfo*>(peer->data)->adminLevel < adminLevel) {
					Player::OnConsoleMessage(peer, ">> You Got your `5Permissions ``Back!");
					static_cast<PlayerInfo*>(peer->data)->adminLevel = adminLevel;
					restore_player_name(world, peer);
					if (nick != "")
					{
						static_cast<PlayerInfo*>(peer->data)->isNicked = true;
						if (static_cast<PlayerInfo*>(peer->data)->NickPrefix != "")
						{
							static_cast<PlayerInfo*>(peer->data)->displayName = static_cast<PlayerInfo*>(peer->data)->NickPrefix + ". " + nick;
						}
						else
						{
							static_cast<PlayerInfo*>(peer->data)->displayName = nick;
						}
						static_cast<PlayerInfo*>(peer->data)->OriName = nick;
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer))
							{
								Player::OnNameChanged(currentPeer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->displayName);
							}
						}
					}
				}
				else {
					Player::OnConsoleMessage(peer, "`oYou've lost your mod powers! This is more useful for testing and finding bugs as it's how most people will play. `5/togglemod `owill give them back, so don't be too sad");
					static_cast<PlayerInfo*>(peer->data)->adminLevel = 0;
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 6) == "/find ") {
			if (static_cast<PlayerInfo*>(peer->data)->haveGrowId) {
				string itemFind = str.substr(6, cch.length() - 6 - 1);
				if (itemFind.length() < 3) {
					Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wThis item name is too short!``", 0, false);
					if (itemFind.length() < 100, 50, 30, 20, 17, 19) {
						Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`4This item name is too long``", 0, false);
						return;
					}
				}
			SKIPFinds:;
				string itemLower2;
				vector<ItemDefinition> itemDefsfind;
				for (char c : itemFind) if (c < 0x20 || c > 0x7A) goto SKIPFinds;
				if (itemFind.length() < 3) goto SKIPFinds3;
				for (const ItemDefinition& item : itemDefs) {
					string itemLower;
					for (char c : item.name) if (c < 0x20 || c > 0x7A) goto SKIPFinds2;
					if (!(item.id % 2 == 0)) goto SKIPFinds2;
					itemLower2 = item.name;
					std::transform(itemLower2.begin(), itemLower2.end(), itemLower2.begin(), ::tolower);
					if (itemLower2.find(itemLower) != std::string::npos) {
						itemDefsfind.push_back(item);
					}
				SKIPFinds2:;
				}
			SKIPFinds3:;
				string listMiddle = "";
				string listFull = "";
				for (const ItemDefinition& item : itemDefsfind) {
					if (item.name != "") {
						string kys = item.name;
						std::transform(kys.begin(), kys.end(), kys.begin(), ::tolower);
						string kms = itemFind;
						std::transform(kms.begin(), kms.end(), kms.begin(), ::tolower);
						if (kys.find(kms) != std::string::npos) {
							int id = item.id;
							if (getItemDef(id).properties & Property_Untradable || id == 1790 || id == 3494 || id == 2952 || id == 112 || id == 0 || id == 6 || id == 32 || id == 18 || getItemDef(id).blockType == BlockTypes::LOCK || getItemDef(id).name.find("Ring Of") != string::npos || getItemDef(id).name.find("Paint Bucket") != string::npos || getItemDef(id).name.find("Bedrock") != string::npos || getItemDef(id).name.find("dumb") != string::npos || getItemDef(id).name.find("null") != string::npos || getItemDef(id).name.find("Subscription") != string::npos) continue;
							listMiddle += "add_button_with_icon|tool" + to_string(item.id) + "|`$" + item.name + "``|left|" + to_string(item.id) + "||\n";
						}
					}
				}
				if (itemFind.length() < 3) {
					listFull = "add_textbox|`4Word is less then 3 letters!``|\nadd_spacer|small|\n";
					Player::OnDialogRequest(peer, "add_label_with_icon|big|`9Find item: " + itemFind + "``|left|3146|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
				}
				else if (itemDefsfind.size() == 0) {
					Player::OnDialogRequest(peer, "add_label_with_icon|big|`9Find item: " + itemFind + "``|left|3146|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
				}
				else {
					if (listMiddle.size() == 0) {
						Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wNo items were found with that name!``", 0, false);
					}
					else {
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wFound item : " + itemFind + "``|left|6016|\nadd_spacer|small|\nend_dialog|findid|Cancel|\nadd_spacer|big|\n" + listMiddle + "add_quick_exit|\n");
					}
				}
			}
		}
		else if (str.substr(0, 5) == "/pay ") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed) {
				Player::OnConsoleMessage(peer, "You cannot perform this action while you are cursed");
				return;
			}
			std::ifstream ifsz("save/gemdb/_" + static_cast<PlayerInfo*>(peer->data)->rawName + ".txt");
			std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
			int b = atoi(content.c_str());
			string imie = str.substr(5, cch.length() - 5 - 1);
			int phm = 0;
			if (imie.find(" ") != std::string::npos) {
				phm = atoi(imie.substr(imie.find(" ") + 1).c_str());
				imie = imie.substr(0, imie.find(" "));
			}
			else {
				return;
			}
			if (phm > 2000000000 || static_cast<PlayerInfo*>(peer->data)->rawName == PlayerDB::getProperName(imie) || static_cast<PlayerInfo*>(peer->data)->rawName == str.substr(5, cch.length() - 5 - 1) || phm < 0) {
				return;
			}
			else if (b >= phm) {
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == imie || static_cast<PlayerInfo*>(currentPeer->data)->displayName == imie) {
						std::ifstream ifsz("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						std::string acontent((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
						int a = atoi(acontent.c_str());
						int bb = b - phm;
						int aa = a + phm;
						ofstream myfile;
						myfile.open("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						myfile << aa;
						myfile.close();
						myfile.open("save/gemdb/_" + static_cast<PlayerInfo*>(peer->data)->rawName + ".txt");
						myfile << bb;
						myfile.close();
						Player::OnConsoleMessage(peer, "`oYou've sent `$" + to_string(phm) + " `ogems to `$" + imie + "`o!");
						Player::OnSetBux(peer, bb, 0);
						Player::OnSetBux(currentPeer, aa, 0);
						Player::OnConsoleMessage(currentPeer, "`oYou've received `$" + to_string(phm) + " `ogems from `$" + static_cast<PlayerInfo*>(peer->data)->displayName + "`o!");
						Player::OnAddNotification(currentPeer, "Player`w " + static_cast<PlayerInfo*>(peer->data)->displayName + "`o paid you `2" + std::to_string(phm) + " Gems`o!", "audio/piano_nice.wav", "interface/cash_icon_overlay.rttex");
						break;
					}
				}
			}
			else if (b < phm) {
				Player::OnConsoleMessage(peer, "`oYou don't have that much (`$" + to_string(phm) + "`o) gems to pay `$" + imie);
			}
		}
		else if (str.substr(0, 7) == "/trade ") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed == true) {
				Player::OnConsoleMessage(peer, "`4You are cursed now!");
				return;
			}
			string trade_name = PlayerDB::getProperName(str.substr(7, cch.length() - 7 - 1));
			if (trade_name.size() < 3) {
				Player::OnConsoleMessage(peer, "If you want to trade Type atleast 3 Characters!");
				return;
			}
			bool Found = false, Block = false;
			int Same_name = 0, Sub_worlds_name = 0;
			string Intel_sense_nick = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) Same_name++;
				}
				else if (mod(peer)) {
					if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) Sub_worlds_name++;
				}
			}
			if (Same_name > 1) {
				Player::OnConsoleMessage(peer, "`oThere are more than two players in this world starting with " + trade_name + " `obe more specific!");
				return;
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer) && getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->rawName) {
						Player::OnConsoleMessage(peer, "`oYou trade all your stuff to yourself in exchange for all your stuff.");
						Block = true;
						break;
					}
					Block = true;
					Found = true;
					Intel_sense_nick = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
					if (static_cast<PlayerInfo*>(currentPeer->data)->trade) {
						Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wThat person is busy.", 0, false);
						break;
					}
					static_cast<PlayerInfo*>(peer->data)->trade = false;
					static_cast<PlayerInfo*>(peer->data)->trade_netid = static_cast<PlayerInfo*>(currentPeer->data)->netID;
					Player::OnStartTrade(peer, static_cast<PlayerInfo*>(currentPeer->data)->displayName, static_cast<PlayerInfo*>(currentPeer->data)->netID);
					break;
				}
			}
			if (Block) return;
			if (!Found) {
				Player::OnConsoleMessage(peer, "`4Oops:`` There is nobody currently in this world with a name starting with `w" + trade_name + "``.");
			}
		}
		else if (str == "/renderworld") {
			int space = 0;
			string append = "";
			for (auto i = 0; i < world->width * world->height; i++) {
				space += 32;
				append += "<div class='column' style='margin-left:" + to_string(space) + "px;position:absolute;'><img src='https://cdn.growstocks.xyz/item/" + to_string(world->items.at(i).foreground) + ".png'></div>";
				if ((space / 32) % world->width == 0 && i != 0) {
					append += "<br><br>";
					space = 0;
				}
			} ofstream write_map("save/render/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + ".html");
			if (!write_map.is_open()) {
				return;
			}
			write_map << append;
			write_map.close();
			Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Render Share``|left|656|\nadd_textbox|`oYour world has been rendered!|\nadd_url_button||`wView``|NOFLAGS|http://" + server_ip + "/save/render/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + ".html|Open picture in your browser?|0|0|\nend_dialog|1|Back||");
		}
		else if (str == "/help" || str == "/?") {
			string commands = "";
			for (int i = 0; i < role_commands.at(static_cast<PlayerInfo*>(peer->data)->adminLevel).size(); i++) {
				if (i + 1 == role_commands.at(static_cast<PlayerInfo*>(peer->data)->adminLevel).size()) {
					commands += "/" + role_commands.at(static_cast<PlayerInfo*>(peer->data)->adminLevel).at(i);
				}
				else {
					commands += "/" + role_commands.at(static_cast<PlayerInfo*>(peer->data)->adminLevel).at(i) + " ";
				}
			}
			if (static_cast<PlayerInfo*>(peer->data)->Subscriber) {
				for (int i = 0; i < sub_commands.size(); i++) {
					if (i + 1 == sub_commands.size()) {
						commands += "/" + sub_commands.at(i);
					}
					else {
						commands += "/" + sub_commands.at(i) + " ";
					}
				}
			}
			Player::OnConsoleMessage(peer, "`o>> Commands: " + commands);
		}
		else if (str.substr(0, 12) == "/weather ")
		{
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (isHere(peer, currentPeer))
				{
					Player::OnSetCurrentWeather(peer, atoi(str.substr(12).c_str()));
					world->weather = atoi(str.substr(12).c_str());
					return;
				}
			}
		}
		else if (str.substr(0, 8) == "/report ") {
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true)
			{
				GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4You are muted now!"));
				ENetPacket* packet0 = enet_packet_create(p0.data,
					p0.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet0);
				delete p0.data;
				return;
			}
			string reportText = str.substr(8, cch.length() - 8 - 1);
			if (reportText == "")
			{
				Player::OnConsoleMessage(peer, "`4Your report text should be not empty.");
				return;
			}
			GamePacket p6 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`1[R]: `3from player `2" + static_cast<PlayerInfo*>(peer->data)->displayName + ". `4His violation/question: `2" + reportText + ""));
			string text = "action|play_sfx\nfile|audio/siren.wav\ndelayMS|0\n";
			BYTE* data = new BYTE[5 + text.length()];
			BYTE zero = 0;
			int type = 3;
			memcpy(data, &type, 4);
			memcpy(data + 4, text.c_str(), text.length());
			memcpy(data + 4 + text.length(), &zero, 1);
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
					continue;
				if (mod(currentPeer))
				{
					if (static_cast<PlayerInfo*>(currentPeer->data)->isinv == true) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->isNicked == true) continue;
					ENetPacket* packet6 = enet_packet_create(p6.data,
						p6.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet6);
					ENetPacket* packet2 = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet2);
				}
			}
			delete[] data;
			delete p6.data;
			Player::OnConsoleMessage(peer, "`2Successfully sent a report `w(`7" + getModsOnlineCount() + "`w)");
		}
		else if (str.substr(0, 8) == "/warpto ") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed == true) return;
			if (str.substr(8, cch.length() - 8 - 1) == "") return;
			string name = str.substr(8, str.length());
			bool found = false;
			bool inExit = false;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				string name2 = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);
				std::transform(name2.begin(), name2.end(), name2.begin(), ::tolower);
				if (name == name2) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->currentWorld == "EXIT") {
						inExit = true;
						break;
					}
					handle_world(peer, static_cast<PlayerInfo*>(currentPeer->data)->currentWorld);
					found = true;
				}
			} if (found) {
				Player::OnConsoleMessage(peer, "Teleporting to " + name + "");
			}
			else if (inExit) {
				Player::OnConsoleMessage(peer, "" + name + " isnt in a world");
			}
			else {
				Player::OnConsoleMessage(peer, "" + name + " is offline");
			}
		}
		else if (str.substr(0, 6) == "/warp ") {
			if (static_cast<PlayerInfo*>(peer->data)->isCursed) return;
			string worldname = str.substr(6, str.length());
			toUpperCase(worldname);
			Player::OnConsoleMessage(peer, "Warping to world " + worldname + ".");
			handle_world(peer, worldname);
		}
		else if (str == "/nuke") {
			if (world->isNuked) {
				world->isNuked = false;
				Player::OnConsoleMessage(peer, "World has been Unnuked!");
			}
			else {
				world->isNuked = true;
				Player::OnTextOverlay(peer, "World has been Nuked!");
				string name = static_cast<PlayerInfo*>(peer->data)->displayName;
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->radio) {
						Player::OnConsoleMessage(currentPeer, "`o>>`4" + world->name + " `4was nuked from orbit`o. It's the only way to be sure. Play nice, everybody!");
						Player::PlayAudio(currentPeer, "audio/bigboom.wav", 0);
					}
					if (isHere(peer, currentPeer)) {
						if (!mod(currentPeer)) {
							sendPlayerLeave(currentPeer);
							static_cast<PlayerInfo*>(currentPeer->data)->currentWorld = "EXIT";
							sendWorldOffers(currentPeer);
						}
					}
				}
			}
		}
		else if (str.substr(0, 5) == "/ban ")
		{
			if (world == nullptr || static_cast<PlayerInfo*>(peer->data)->currentWorld == "EXIT" || serverIsFrozen) return;
			if (static_cast<PlayerInfo*>(peer->data)->haveGrowId && static_cast<PlayerInfo*>(peer->data)->rawName == world->owner || mod(peer))
			{
				if (str.substr(5, cch.length() - 5 - 1) == "") return;
				if (static_cast<PlayerInfo*>(peer->data)->rawName == str.substr(5, cch.length() - 5 - 1)) return;
				if (static_cast<PlayerInfo*>(peer->data)->currentWorld == "HELL")
				{
					Player::OnConsoleMessage(peer, "`4You can't ban players in this world.");
					return;
				}
				string name = static_cast<PlayerInfo*>(peer->data)->displayName;
				string kickname = PlayerDB::getProperName(str.substr(5, cch.length() - 5 - 1));
				ENetPeer* currentPeerp;
				for (currentPeerp = server->peers;
					currentPeerp < &server->peers[server->peerCount];
					++currentPeerp) {
					if (currentPeerp->state != ENET_PEER_STATE_CONNECTED)
						continue;
					if (static_cast<PlayerInfo*>(currentPeerp->data)->rawName == kickname)
					{
						if (admin(currentPeerp) && static_cast<PlayerInfo*>(currentPeerp->data)->rawName == kickname) {
							Player::OnConsoleMessage(peer, "`o>> You can't ban `5@Moderator `o or above!");
							return;
						}
						else
						{
							namespace fs = std::experimental::filesystem;
							if (!fs::is_directory("save/worldbans/_" + world->name) || !fs::exists("save/worldbans/_" + world->name))
							{
								fs::create_directory("save/worldbans/_" + world->name);
								std::ofstream outfile("save/worldbans/_" + world->name + "/" + static_cast<PlayerInfo*>(currentPeerp->data)->rawName);
								outfile << "worldbanned by: " + static_cast<PlayerInfo*>(peer->data)->rawName;
								outfile.close();
							}
							else
							{
								std::ofstream outfile("save/worldbans/_" + world->name + "/" + static_cast<PlayerInfo*>(currentPeerp->data)->rawName);
								outfile << "worldbanned by: " + static_cast<PlayerInfo*>(peer->data)->rawName;
								outfile.close();
							}
							sendPlayerLeave(currentPeerp);
							static_cast<PlayerInfo*>(currentPeerp->data)->currentWorld = "EXIT";
							sendWorldOffers(currentPeerp);

							ENetPeer* currentPeer;
							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
									continue;
								if (isHere(peer, currentPeer))
								{
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), name + " `4world bans " + "`o" + kickname + " from `w" + world->name + "`o!"));
									string text = "action|play_sfx\nfile|audio/repair.wav\ndelayMS|0\n";
									BYTE* data = new BYTE[5 + text.length()];
									BYTE zero = 0;
									int type = 3;
									memcpy(data, &type, 4);
									memcpy(data + 4, text.c_str(), text.length());
									memcpy(data + 4 + text.length(), &zero, 1);
									ENetPacket* packetsou = enet_packet_create(data,
										5 + text.length(),
										ENET_PACKET_FLAG_RELIABLE);
									ENetPacket* packet = enet_packet_create(p.data,
										p.len,
										ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(currentPeer, 0, packetsou);
									enet_peer_send(currentPeer, 0, packet);
									delete[] data;
									delete p.data;
								}
							}
						}
					}
				}
			}
		}
		else if (str.substr(0, 9) == "/fakeban ") {
			string username = PlayerDB::getProperName(str.substr(8, cch.length() - 8 - 1));
			bool found = false;
			string userdisplay = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == username) {
					userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
					found = true;
					break;
				}
			}
			if (userdisplay == "") userdisplay = username;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				Player::OnConsoleMessage(currentPeer, "`#** `$The Ancient Ones `ohave `4banned `w" + userdisplay + " `#** ``(`4/rules `oto see the rules!)");
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == username) {
					enet_peer_disconnect_later(currentPeer, 0);
				}
			}
			if (!found) {
				Player::OnConsoleMessage(peer, "`oPlayer was not found, so only notification was sent");
			}
			else {
				Player::OnConsoleMessage(peer, "`oUsed fake ban on " + userdisplay);
			}
		}
		else if (str.substr(0, 6) == "/pban ") {
			ifstream read_player("save/players/_" + PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6)) + ".json");
			if (!read_player.is_open()) {
				Player::OnConsoleMessage(peer, "Player does not exist!");
				return;
			}
			read_player.close();
			static_cast<PlayerInfo*>(peer->data)->last_ban_days = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_hours = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_minutes = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_reason = "";
			static_cast<PlayerInfo*>(peer->data)->lastInfo = PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6));
			send_ban_panel(peer, "");
		}
		else if (str.substr(0, 5) == "/spk ")
		{
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true) {
				Player::OnConsoleMessage(peer, "`4You are muted now!");
				return;
			}
			return;
			string say_info = str;
			size_t extra_space = say_info.find("  ");
			if (extra_space != std::string::npos) {
				say_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string say_user;
			string say_message;
			if ((pos = say_info.find(delimiter)) != std::string::npos) {
				say_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`oPlease enter a player's name.");
				return;
			}
			if ((pos = say_info.find(delimiter)) != std::string::npos) {
				say_user = say_info.substr(0, pos);
				say_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`oPlease enter a message.");
				return;
			}
			say_message = say_info;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == say_user) {
					Player::OnConsoleMessage(peer, "`$" + say_user + " `osaid >> `$" + say_message);
					SendChat(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, say_message, world, cch);
					break;
				}
			}
		}
		else if (str.substr(0, 8) == "/unmute ")
		{
			if (str.substr(8, cch.length() - 8 - 1) == "") return;
			if (static_cast<PlayerInfo*>(peer->data)->rawName == str.substr(8, cch.length() - 8 - 1)) return;
			string name = PlayerDB::getProperName(str.substr(8, cch.length() - 8 - 1));
			try {
				ifstream read_player("save/players/_" + PlayerDB::getProperName(name) + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, "Player does not exist!");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				int timemuted = j["timemuted"];
				if (timemuted == 0) {
					Player::OnConsoleMessage(peer, "" + PlayerDB::getProperName(name) + " is not muted");
					return;
				}
				j["timemuted"] = 0;
				ofstream write_player("save/players/_" + PlayerDB::getProperName(name) + ".json");
				write_player << j << std::endl;
				write_player.close();
				Player::OnConsoleMessage(peer, "Unmuted " + PlayerDB::getProperName(name));
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == name) {
						static_cast<PlayerInfo*>(currentPeer->data)->taped = false;
						static_cast<PlayerInfo*>(currentPeer->data)->isDuctaped = false;
						static_cast<PlayerInfo*>(currentPeer->data)->cantsay = false;
						static_cast<PlayerInfo*>(currentPeer->data)->lastMuted = 0;
						send_state(currentPeer);
						sendClothes(currentPeer);
						break;
					}
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 6) == "/mute ")
		{
			ifstream read_player("save/players/_" + PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6)) + ".json");
			if (!read_player.is_open()) {
				Player::OnConsoleMessage(peer, "Player does not exist!");
				return;
			}
			read_player.close();
			static_cast<PlayerInfo*>(peer->data)->last_ban_days = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_hours = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_minutes = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_reason = "";
			static_cast<PlayerInfo*>(peer->data)->lastInfo = PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6));
			send_mute_panel(peer, "");
		}
		else if (str.substr(0, 7) == "/curse ")
		{
			ifstream read_player("save/players/_" + PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6)) + ".json");
			if (!read_player.is_open()) {
				Player::OnConsoleMessage(peer, "Player does not exist!");
				return;
			}
			read_player.close();
			static_cast<PlayerInfo*>(peer->data)->last_ban_days = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_hours = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_minutes = 0;
			static_cast<PlayerInfo*>(peer->data)->last_ban_reason = "";
			static_cast<PlayerInfo*>(peer->data)->lastInfo = PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 6));
			send_curse_panel(peer, "");
		}
		else if (str == "/time")
		{
			sendTime(peer);
		}
		else if (str.substr(0, 9) == "/uncurse ")
		{
			if (str.substr(9, cch.length() - 9 - 1) == "") return;
			if (static_cast<PlayerInfo*>(peer->data)->rawName == str.substr(9, cch.length() - 9 - 1)) return;
			string cursename = str.substr(9, cch.length() - 9 - 1);
			try {
				ifstream read_player("save/players/_" + PlayerDB::getProperName(cursename) + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, "Player does not exist!");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				int timecursed = j["timecursed"];
				if (timecursed == 0) {
					Player::OnConsoleMessage(peer, "" + PlayerDB::getProperName(cursename) + " is not cursed");
					return;
				}
				j["timecursed"] = 0;
				ofstream write_player("save/players/_" + PlayerDB::getProperName(cursename) + ".json");
				write_player << j << std::endl;
				write_player.close();
				Player::OnConsoleMessage(peer, "Uncursed " + PlayerDB::getProperName(cursename));
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == cursename) {
						static_cast<PlayerInfo*>(currentPeer->data)->skinColor = 0x8295C3FF;
						sendClothes(currentPeer);
						static_cast<PlayerInfo*>(currentPeer->data)->isCursed = false;
						static_cast<PlayerInfo*>(currentPeer->data)->lastCursed = 0;
						send_state(currentPeer);
					}
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 7) == "/unban ")
		{
			string name = PlayerDB::getProperName(str.substr(7, cch.length() - 7 - 1));
			try {
				ifstream read_player("save/players/_" + PlayerDB::getProperName(name) + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, "Player does not exist!");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				int timebanned = j["timebanned"];
				bool isBanned = j["isBanned"];
				if (timebanned == 0 && !isBanned) {
					Player::OnConsoleMessage(peer, "" + PlayerDB::getProperName(name) + " is not banned");
					return;
				}
				j["timebanned"] = 0;
				j["isBanned"] = false;
				ofstream write_player("save/players/_" + PlayerDB::getProperName(name) + ".json");
				write_player << j << std::endl;
				write_player.close();
				Player::OnConsoleMessage(peer, "Unbanned " + PlayerDB::getProperName(name));
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 6) == "/msgs ")
		{
			bool found = false;
			if (static_cast<PlayerInfo*>(peer->data)->haveGrowId == false)
			{
				GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oTo prevent abuse, you `4must `obe `2registered `oin order to use this command!"));
				ENetPacket* packet0 = enet_packet_create(p0.data,
					p0.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet0);
				delete p0.data;
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true)
			{
				GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4You are muted now!"));
				ENetPacket* packet0 = enet_packet_create(p0.data,
					p0.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet0);
				delete p0.data;
				return;
			}
			string msg_info = str;
			size_t extra_space = msg_info.find("  ");
			if (extra_space != std::string::npos)
			{
				msg_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string pm_user;
			string pm_message;
			if ((pos = msg_info.find(delimiter)) != std::string::npos)
			{
				msg_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oPlease specify a `2player `oyou want your message to be delivered to."));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
			}
			if ((pos = msg_info.find(delimiter)) != std::string::npos)
			{
				pm_user = msg_info.substr(0, pos);
				msg_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oPlease enter your `2message`o."));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
			}
			pm_message = msg_info;
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
					continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->msgName == PlayerDB::getProperName(pm_user))
				{
					if (static_cast<PlayerInfo*>(currentPeer->data)->isinv == true) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->isNicked == true) continue;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsger = static_cast<PlayerInfo*>(peer->data)->rawName;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsgerTrue = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsgWorld = static_cast<PlayerInfo*>(peer->data)->currentWorld;
					GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o)"));
					ENetPacket* packet0 = enet_packet_create(p0.data,
						p0.len,
						ENET_PACKET_FLAG_RELIABLE);
					GamePacket p10 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o) `o(`4Note: `oMessage a mod `4ONLY ONCE `oabout an issue. Mods don't fix scams or replace items, they punish players who break the `5/rules`o.)"));
					ENetPacket* packet10 = enet_packet_create(p10.data,
						p10.len,
						ENET_PACKET_FLAG_RELIABLE);
					if (mod(currentPeer) && !mod(peer) && static_cast<PlayerInfo*>(currentPeer->data)->isNicked == false)
					{
						enet_peer_send(peer, 0, packet10);
					}
					else
					{
						enet_peer_send(peer, 0, packet0);
					}
					delete p0.data;
					delete p10.data;
					found = true;
					GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `c>> from (`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`c) in [`4<HIDDEN>`c] > `o" + pm_message));
					string text = "action|play_sfx\nfile|audio/pay_time.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length());
					memcpy(data + 4 + text.length(), &zero, 1);
					ENetPacket* packet2 = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet2);
					delete[] data;
					ENetPacket* packet = enet_packet_create(ps.data,
						ps.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet);
					delete ps.data;
					break;
				}
			}
			if (found == false)
			{
				Player::OnConsoleMessage(peer, "`6>> No one online who has a name starting with " + pm_user + "`8.");
			}
		}
		else if (str.substr(0, 5) == "/msg ")
		{
			bool found = false;
			if (static_cast<PlayerInfo*>(peer->data)->haveGrowId == false)
			{
				GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oTo prevent abuse, you `4must `obe `2registered `oin order to use this command!"));
				ENetPacket* packet0 = enet_packet_create(p0.data,
					p0.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet0);
				delete p0.data;
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true)
			{
				GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4You are muted now!"));
				ENetPacket* packet0 = enet_packet_create(p0.data,
					p0.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet0);
				delete p0.data;
				return;
			}
			string msg_info = str;
			size_t extra_space = msg_info.find("  ");
			if (extra_space != std::string::npos)
			{
				msg_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string pm_user;
			string pm_message;
			if ((pos = msg_info.find(delimiter)) != std::string::npos)
			{
				msg_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oPlease specify a `2player `oyou want your message to be delivered to."));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
			}
			if ((pos = msg_info.find(delimiter)) != std::string::npos)
			{
				pm_user = msg_info.substr(0, pos);
				msg_info.erase(0, pos + delimiter.length());
			}
			else
			{
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oPlease enter your `2message`o."));
				ENetPacket* packet = enet_packet_create(ps.data,
					ps.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete ps.data;
			}
			pm_message = msg_info;
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
					continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->msgName == PlayerDB::getProperName(pm_user))
				{
					if (static_cast<PlayerInfo*>(currentPeer->data)->isinv == true) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->isNicked == true) continue;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsger = static_cast<PlayerInfo*>(peer->data)->rawName;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsgerTrue = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMsgWorld = static_cast<PlayerInfo*>(peer->data)->currentWorld;
					GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o)"));
					ENetPacket* packet0 = enet_packet_create(p0.data,
						p0.len,
						ENET_PACKET_FLAG_RELIABLE);
					GamePacket p10 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `o(Sent to `$" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o) `o(`4Note: `oMessage a mod `4ONLY ONCE `oabout an issue. Mods don't fix scams or replace items, they punish players who break the `5/rules`o.)"));
					ENetPacket* packet10 = enet_packet_create(p10.data,
						p10.len,
						ENET_PACKET_FLAG_RELIABLE);
					if (mod(currentPeer) && !mod(peer) && static_cast<PlayerInfo*>(currentPeer->data)->isNicked == false)
					{
						enet_peer_send(peer, 0, packet10);
					}
					else
					{
						enet_peer_send(peer, 0, packet0);
					}
					delete p0.data;
					delete p10.data;
					found = true;
					GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[MSG]_ `c>> from (`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`c) in [`o" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "`c] > `o" + pm_message));
					string text = "action|play_sfx\nfile|audio/pay_time.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length());
					memcpy(data + 4 + text.length(), &zero, 1);
					ENetPacket* packet2 = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet2);
					delete[] data;
					ENetPacket* packet = enet_packet_create(ps.data,
						ps.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet);
					delete ps.data;
					break;
				}
			}
			if (found == false)
			{
				Player::OnConsoleMessage(peer, "`6>> No one online who has a name starting with " + pm_user + "`8.");
			}
		}
		else if (str == "/uba")
		{
			if (world == nullptr || static_cast<PlayerInfo*>(peer->data)->currentWorld == "EXIT" || serverIsFrozen) return;
			if (static_cast<PlayerInfo*>(peer->data)->haveGrowId && static_cast<PlayerInfo*>(peer->data)->rawName == world->owner || mod(peer))
			{
				namespace fs = std::experimental::filesystem;
				fs::remove_all("save/worldbans/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld);
				Player::OnConsoleMessage(peer, "`oYou unbanned everyone from the world!");
			}
		}
		else if (str.substr(0, 5) == "/eff ")
		{
			if (static_cast<PlayerInfo*>(peer->data)->lastEff + 4000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
			{
				static_cast<PlayerInfo*>(peer->data)->lastEff = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();

				Player::OnConsoleMessage(peer, "`oEffect spawned (" + str.substr(5, cch.length() - 5 - 1) + ")");
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer))
					{
						Player::OnParticleEffect(currentPeer, atoi(str.substr(5, cch.length() - 5 - 1).c_str()), static_cast<PlayerInfo*>(peer->data)->x, static_cast<PlayerInfo*>(peer->data)->y, 0);
					}
				}
			}
			else {
				Player::OnConsoleMessage(peer, "`wAnti-Lag >> Slow down! (Cooldown: 4 Seconds)");
			}
		}
		else if (str.substr(0, 3) == "/p ") {
			Player::OnConsoleMessage(peer, "`oPunch Effect changed to " + str.substr(3, cch.length() - 3 - 1));
			static_cast<PlayerInfo*>(peer->data)->effect = atoi(str.substr(3, cch.length() - 3 - 1).c_str());
			sendPuncheffect(peer, static_cast<PlayerInfo*>(peer->data)->effect);
			send_state(peer);
		}
		else if (str.substr(0, 5) == "/gsm ")
		{
			string name = static_cast<PlayerInfo*>(peer->data)->displayName;
			GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4Global System Message: `o" + str.substr(4, cch.length() - 4 - 1)));
			string text = "action|play_sfx\nfile|audio/sungate.wav\ndelayMS|0\n";
			BYTE* data = new BYTE[5 + text.length()];
			BYTE zero = 0;
			int type = 3;
			memcpy(data, &type, 4);
			memcpy(data + 4, text.c_str(), text.length());
			memcpy(data + 4 + text.length(), &zero, 1);
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
					continue;
				if (!static_cast<PlayerInfo*>(currentPeer->data)->radio)
					continue;
				ENetPacket* packet = enet_packet_create(p.data,
					p.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(currentPeer, 0, packet);
				ENetPacket* packet2 = enet_packet_create(data,
					5 + text.length(),
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(currentPeer, 0, packet2);
				//enet_host_flush(server);
			}
			delete[] data;
			delete p.data;
		}
		else if (str.substr(0, 9) == "/unbanip ")
		{
			string playerCalled = PlayerDB::getProperName(str.substr(9, cch.length() - 9 - 1));

			string ipid;
			string getmac;
			string getrid;
			string getsid;
			string getgid;
			string getvid;
			string getaid;
			string getip;

			try {
				ifstream read_player("save/players/_" + PlayerDB::getProperName(playerCalled) + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, "Player does not exist!");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				string ipid = j["ipID"];
				string getmac = j["mac"];
				string getrid = j["rid"];
				string getsid = j["sid"];
				string getgid = j["gid"];
				string getvid = j["vid"];
				string getaid = j["aid"];
				string getip = j["ip"];
				string isipbanned = "No.";
				string macremoved = getmac;
				Remove(macremoved, ":");
				bool existx = std::experimental::filesystem::exists("save/ipbans/mac/" + macremoved + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/mac/" + macremoved + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/rid/" + getrid + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/rid/" + getrid + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/gid/" + getgid + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/gid/" + getgid + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/ip/" + getip + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/ip/" + getip + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/sid/" + getsid + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/sid/" + getsid + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/aid/" + getaid + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/aid/" + getaid + ".txt").c_str());
				}
				existx = std::experimental::filesystem::exists("save/ipbans/ip_id/" + ipid + ".txt");
				if (existx) {
					isipbanned = "Yes.";
					remove(("save/ipbans/ip_id/" + ipid + ".txt").c_str());
				}
				if (isipbanned == "No.") {
					Player::OnConsoleMessage(peer, username + " is not ip banned!");
				}
				else {
					Player::OnConsoleMessage(peer, username + " ip ban was removed!");
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 8) == "/infoex ") {
			string checknick = PlayerDB::getProperName(str.substr(8, cch.length() - 8 - 1));
			try {
				ifstream read_player("save/players/_" + checknick + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, checknick + " does not exist");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				int playerid = j["playerid"];
				string email = j["email"];
				string ipID = j["ipID"];
				int receivedwarns = j["receivedwarns"];
				string mac = j["mac"];
				string rid = j["rid"];
				string sid = j["sid"];
				string gid = j["gid"];
				string vid = j["vid"];
				string aid = j["aid"];
				string ip = j["ip"];
				int adminLevel = j["adminLevel"];
				string subtype = j["subtype"];
				string subdate = j["subdate"];
				string nick = j["nick"];
				int level = j["level"];
				if (mac == "02:00:00:00:00:00") mac = "N/A";
				if (rid == "" || rid == "none") rid = "N/A";
				if (sid == "" || sid == "none") sid = "N/A";
				if (gid == "" || gid == "none") gid = "N/A";
				if (vid == "" || vid == "none") vid = "N/A";
				if (aid == "" || aid == "none") aid = "N/A";
				if (ip == "127.0.0.1") ip = "localhost";
				if (ip == "" || ip == "none") ip = "N/A";
				{
					Player::OnConsoleMessage(peer, checknick + " does not exist");
					return;
				}
				Player::OnConsoleMessage(peer, "Found 1 matches:");
				Player::OnConsoleMessage(peer, username + " (ID: " + to_string(playerid));
				if (nick == "") nick = "N/A";
				string status = role_nameko.at(adminLevel);
				if (subdate == "") subdate = "N/A";
				Player::OnConsoleMessage(peer, "Status: " + status + " Nickname: " + nick + " Email: " + email + " Level: " + to_string(level) + " Subscription: " + subdate + "");
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str.substr(0, 6) == "/info ") {
			SendPunishView(peer, PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 1)));
		}
		else if (str.substr(0, 7) == "/banip ")
		{
			string warn_info = str;
			size_t extra_space = warn_info.find("  ");
			if (extra_space != std::string::npos)
			{
				warn_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string banip_user;
			string banip_reason;
			if ((pos = warn_info.find(delimiter)) != std::string::npos)
			{
				warn_info.erase(0, pos + delimiter.length());
			}
			else
			{
				Player::OnConsoleMessage(peer, "`@Please specify a `9Player `@You want to `4Ban-ip`@!");
				return;
			}
			if ((pos = warn_info.find(delimiter)) != std::string::npos)
			{
				banip_user = warn_info.substr(0, pos);
				warn_info.erase(0, pos + delimiter.length());
			}
			else
			{
				Player::OnConsoleMessage(peer, "`@Please enter your `4Ban-ip `@Reason!");
				return;
			}
			banip_reason = warn_info;
			int kiekDabarTuriWarns = 0;
			if (banip_reason.length() < 5)
			{
				AutoDemote(static_cast<PlayerInfo*>(peer->data)->rawName, to_string(static_cast<PlayerInfo*>(peer->data)->adminLevel), to_string(peer->address.host), "warn reason length < 5. He wrote: " + banip_reason);
				return;
			}
			try {
				ifstream read_player("save/players/_" + PlayerDB::getProperName(banip_user) + ".json");
				if (!read_player.is_open()) {
					Player::OnConsoleMessage(peer, PlayerDB::getProperName(banip_user) + " does not exist");
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["username"];
				string ipID = j["ipID"];
				string macmysql = j["mac"];
				string ridmysql = j["rid"];
				string sidmysql = j["sid"];
				string gidmysql = j["gid"];
				string vidmysql = j["vid"];
				string aidmysql = j["aid"];
				string ipmysql = j["ip"];
				bool arRado = false;
				GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`#** `$The Ancient Ones `oapplied `4device ban `oon `2" + banip_user + " `ofor `2" + banip_reason + "`#** `o(`4/rules `oto see the rules!)"));
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == PlayerDB::getProperName(banip_user)) {
						string name = static_cast<PlayerInfo*>(peer->data)->displayName;
						arRado = true;
						string registermac = static_cast<PlayerInfo*>(currentPeer->data)->registermac;
						string registerrid = static_cast<PlayerInfo*>(currentPeer->data)->registerrid;
						string registersid = static_cast<PlayerInfo*>(currentPeer->data)->registersid;
						string registergid = static_cast<PlayerInfo*>(currentPeer->data)->registergid;
						string registervid = static_cast<PlayerInfo*>(currentPeer->data)->registervid;
						string registeraid = static_cast<PlayerInfo*>(currentPeer->data)->registeraid;
						string registerIP = static_cast<PlayerInfo*>(currentPeer->data)->registerIP;
						if (registermac != "02:00:00:00:00:00" && registermac != "" && registermac != "none")
						{
							Remove(registermac, ":");
							std::ofstream outfile2("save/ipbans/mac/" + registermac + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registerrid != "" && registerrid != "none")
						{
							std::ofstream outfile2("save/ipbans/rid/" + registerrid + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registersid != "" && registersid != "none")
						{
							std::ofstream outfile2("save/ipbans/sid/" + registersid + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registergid != "" && registergid != "none")
						{
							std::ofstream outfile2("save/ipbans/gid/" + registergid + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registervid != "" && registervid != "none")
						{
							std::ofstream outfile2("save/ipbans/vid/" + registervid + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registeraid != "" && registeraid != "none")
						{
							std::ofstream outfile2("save/ipbans/aid/" + registeraid + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						if (registerIP != "" && registerIP != "none")
						{
							std::ofstream outfile2("save/ipbans/ip/" + registerIP + ".txt");
							outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
							outfile2 << "Ban-ip reason: " + banip_reason << endl;
							outfile2 << "Banned user name is: " + static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							outfile2.close();
						}
						Player::OnAddNotification(currentPeer, "`4Your device got banned by `2" + static_cast<PlayerInfo*>(peer->data)->rawName + ". `4Reason: `2 " + banip_reason + "", "audio/hub_open.wav", "interface/atomic_button.rttex");
						Player::OnConsoleMessage(currentPeer, "`4Your device got banned by `2" + static_cast<PlayerInfo*>(peer->data)->rawName + ". `4Reason: `2 " + banip_reason + "");
						Player::OnConsoleMessage(peer, "`2You successfully `5BANNED `4device `5on `w" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + " !! `1(device bans cannot be removed from the game)");
						enet_peer_disconnect_later(currentPeer, 0);
						ENetPeer* currentPeer6;
						for (currentPeer6 = server->peers;
							currentPeer6 < &server->peers[server->peerCount];
							++currentPeer6)
						{
							if (currentPeer6->state != ENET_PEER_STATE_CONNECTED)
								continue;
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(currentPeer6, 0, packet);
						}
						delete ps.data;
						break;
					}
				}
				if (arRado == false) {
					if (macmysql != "02:00:00:00:00:00" && macmysql != "" && macmysql != "none")
					{
						Remove(macmysql, ":");
						std::ofstream outfile2("save/ipbans/mac/" + macmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (ridmysql != "" && ridmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/rid/" + ridmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (sidmysql != "" && sidmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/sid/" + sidmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (gidmysql != "" && gidmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/gid/" + gidmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (vidmysql != "" && vidmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/vid/" + vidmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (aidmysql != "" && aidmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/aid/" + aidmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}
					if (ipmysql != "" && ipmysql != "none")
					{
						std::ofstream outfile2("save/ipbans/ip/" + ipmysql + ".txt");
						outfile2 << "user who banned this ID: " + static_cast<PlayerInfo*>(peer->data)->rawName << endl;
						outfile2 << "Ban-ip reason: " + banip_reason << endl;
						outfile2 << "Banned user name is: " + PlayerDB::getProperName(banip_user);
						outfile2.close();
					}

					ENetPeer* currentPeer6;
					for (currentPeer6 = server->peers;
						currentPeer6 < &server->peers[server->peerCount];
						++currentPeer6)
					{
						if (currentPeer6->state != ENET_PEER_STATE_CONNECTED)
							continue;
						ENetPacket* packet = enet_packet_create(ps.data,
							ps.len,
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(currentPeer6, 0, packet);
					}
					delete ps.data;
					Player::OnConsoleMessage(peer, "`2User was device-banned in `boffline`2!");
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		else if (str == "/nick")
		{
			restore_player_name(world, peer);
		}
		else if (str.substr(0, 11) == "/givelevel ") {
			string ban_info = str;
			size_t extra_space = ban_info.find("  ");
			if (extra_space != std::string::npos)
			{
				ban_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string ban_user;
			string ban_time;
			if ((pos = ban_info.find(delimiter)) != std::string::npos)
			{
				ban_info.erase(0, pos + delimiter.length());
			}
			else
			{
				Player::OnConsoleMessage(peer, "`o>> Usage: /givelevels <user> <amount>");
				return;
			}
			if ((pos = ban_info.find(delimiter)) != std::string::npos)
			{
				ban_user = ban_info.substr(0, pos);
				ban_info.erase(0, pos + delimiter.length());
			}
			else
			{
				Player::OnConsoleMessage(peer, "`o>> Usage: /givelevels <user> <amount>");
				return;
			}
			ban_time = ban_info;
			string user = ban_user;
			string levelnum = ban_time;
			if (levelnum.length() > 9)
			{
				Player::OnConsoleMessage(peer, "`o>> Something went wrong with the length!");
				return;
			}
			bool contains_non_int = !std::regex_match(levelnum, std::regex("^[0-9]+$"));
			if (contains_non_int == true)
			{
				Player::OnConsoleMessage(peer, "`o>> `4Something went wrong. Maybe typed in something wrong!");
				return;
			}
			else
			{
				int konvertuotasInt = stoi(levelnum);
				if (konvertuotasInt <= 0 || konvertuotasInt > 125)
				{
					Player::OnConsoleMessage(peer, "`o>> Type in a number below or equal to 125!");
					return;
				}
				else
				{
					bool existxx = std::experimental::filesystem::exists("save/players/_" + PlayerDB::getProperName(user) + ".json");
					if (!existxx)
					{
						Player::OnConsoleMessage(peer, "`o>> `4User Does not exist, Make sure the name is all lowercase and correct!");
						return;
					}
					ifstream fg("save/players/_" + PlayerDB::getProperName(user) + ".json");
					json j;
					fg >> j;
					fg.close();
					j["level"] = konvertuotasInt;
					ofstream fs("save/players/_" + PlayerDB::getProperName(user) + ".json");
					fs << j;
					fs.close();
					ENetPeer* currentPeer;
					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;
						if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == PlayerDB::getProperName(user))
						{
							static_cast<PlayerInfo*>(currentPeer->data)->level = konvertuotasInt;
							send_state(currentPeer);
							sendClothes(currentPeer);
						}
					}
					Player::OnConsoleMessage(peer, ">> You successfully gave `2" + to_string(konvertuotasInt) + "`` level to `2" + user + "`` user.");
				}
			}
		}
		else if (str.substr(0, 10) == "/givegems ") {
			if (str.substr(10, cch.length() - 10 - 1) == "") return;
			string ban_info = str;
			size_t extra_space = ban_info.find("  ");
			if (extra_space != std::string::npos) {
				ban_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string ban_user;
			string ban_time;
			if ((pos = ban_info.find(delimiter)) != std::string::npos) {
				ban_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`o>> Usage: /givegems <user> <amount>");
				return;
			}
			if ((pos = ban_info.find(delimiter)) != std::string::npos) {
				ban_user = ban_info.substr(0, pos);
				ban_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`oUsage: /givegems <user> <amount>");
				return;
			}
			ban_time = ban_info;
			string playerName = ban_user;
			string howmuchgems = ban_time;
			bool contains_non_int2 = !std::regex_match(howmuchgems, std::regex("^[0-9]+$"));
			if (contains_non_int2 == true) {
				return;
			}
			int kiek_gems = atoi(howmuchgems.c_str());
			transform(playerName.begin(), playerName.end(), playerName.begin(), ::tolower);
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == playerName) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->haveGrowId) {
						Player::OnConsoleMessage(currentPeer, "`oYou received " + to_string(kiek_gems) + " gems");
						Player::OnConsoleMessage(peer, "`oSent " + to_string(kiek_gems) + " gems to " + static_cast<PlayerInfo*>(currentPeer->data)->displayName);
						std::ifstream ifsz("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
						int gembux = atoi(content.c_str());
						int fingembux = gembux + kiek_gems;
						ofstream myfile;
						myfile.open("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						myfile << fingembux;
						myfile.close();
						int gemcalc = gembux + kiek_gems;
						Player::OnSetBux(currentPeer, gemcalc, 0);
						break;
					}
				}
			}
		}
		else if (str.substr(0, 12) == "/removegems ") {
			if (str.substr(12, cch.length() - 12 - 1) == "") return;
			string ban_info = str;
			size_t extra_space = ban_info.find("  ");
			if (extra_space != std::string::npos) {
				ban_info.replace(extra_space, 2, " ");
			}
			string delimiter = " ";
			size_t pos = 0;
			string ban_user;
			string ban_time;
			if ((pos = ban_info.find(delimiter)) != std::string::npos) {
				ban_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`o>> Usage: /removegems <user> <amount>");
				return;
			}
			if ((pos = ban_info.find(delimiter)) != std::string::npos) {
				ban_user = ban_info.substr(0, pos);
				ban_info.erase(0, pos + delimiter.length());
			}
			else {
				Player::OnConsoleMessage(peer, "`oUsage: /removegems <user> <amount>");
				return;
			}
			ban_time = ban_info;
			string playerName = ban_user;
			string howmuchgems = ban_time;
			bool contains_non_int2 = !std::regex_match(howmuchgems, std::regex("^[0-9]+$"));
			if (contains_non_int2 == true) {
				return;
			}
			int kiek_gems = atoi(howmuchgems.c_str());
			transform(playerName.begin(), playerName.end(), playerName.begin(), ::tolower);
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == playerName) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->haveGrowId) {
						Player::OnConsoleMessage(currentPeer, "`oYou were `4removed `o" + to_string(kiek_gems) + " gems!");
						Player::OnConsoleMessage(peer, "`4Removed `o " + to_string(kiek_gems) + " gems from " + static_cast<PlayerInfo*>(currentPeer->data)->displayName);
						std::ifstream ifsz("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
						int gembux = atoi(content.c_str());
						int fingembux = gembux - kiek_gems;
						ofstream myfile;
						myfile.open("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
						myfile << fingembux;
						myfile.close();
						int gemcalc = gembux - kiek_gems;
						Player::OnSetBux(currentPeer, gemcalc, 0);
						break;
					}
				}
			}
		}
		else if (str.substr(0, 10) == "/giverank ") {
			if (std::find(creators.begin(), creators.end(), static_cast<PlayerInfo*>(peer->data)->rawName) != creators.end()) {
				if (str.substr(10, cch.length() - 10 - 1) == "") return;
				string ban_info = str;
				size_t extra_space = ban_info.find("  ");
				if (extra_space != std::string::npos) {
					ban_info.replace(extra_space, 2, " ");
				}
				string delimiter = " ";
				size_t pos = 0;
				string ban_user;
				string ban_time;
				if ((pos = ban_info.find(delimiter)) != std::string::npos) {
					ban_info.erase(0, pos + delimiter.length());
				}
				else {
					Player::OnConsoleMessage(peer, "`oUsage: /giverank <user> <rankname>");
					return;
				}
				if ((pos = ban_info.find(delimiter)) != std::string::npos) {
					ban_user = ban_info.substr(0, pos);
					ban_info.erase(0, pos + delimiter.length());
				}
				else {
					Player::OnConsoleMessage(peer, "`oUsage: /giverank <user> <rankname>");
					return;
				}
				ban_time = ban_info;
				string playerName = ban_user;
				string rankName = ban_time;
				bool success = false;
				transform(rankName.begin(), rankName.end(), rankName.begin(), ::tolower);
				transform(playerName.begin(), playerName.end(), playerName.begin(), ::tolower);
				GiveRank(rankName, playerName, success);
				if (success) {
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == playerName) {
							if (static_cast<PlayerInfo*>(currentPeer->data)->haveGrowId) {
								Player::OnConsoleMessage(currentPeer, ">> Your now rank, " + rankName);
								enet_peer_disconnect_later(currentPeer, 0);
								Player::OnConsoleMessage(peer, "`2Successfully changed.");
								break;
							}
						}
					}
				}
				else {
					Player::OnConsoleMessage(peer, ">> You typed in wrong `2GrowID ``or `2Rank``!");
				}
			}
		}
		else if (str.substr(0, 6) == "/nick ") {
			string name2 = str.substr(6, cch.length() - 6 - 1);
			if ((str.substr(6, cch.length() - 6 - 1).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`!@#$%^&*.+=-_ ") != string::npos)) return;
			if (name2.length() < 3) return;
			if (name2.length() > 24) return;
			static_cast<PlayerInfo*>(peer->data)->msgName = PlayerDB::getProperName(str.substr(6, cch.length() - 6 - 1));
			static_cast<PlayerInfo*>(peer->data)->OriName = name2;
			if (static_cast<PlayerInfo*>(peer->data)->NickPrefix != "")
			{
				static_cast<PlayerInfo*>(peer->data)->displayName = static_cast<PlayerInfo*>(peer->data)->NickPrefix + ". " + str.substr(6, cch.length() - 6 - 1);
				name2 = static_cast<PlayerInfo*>(peer->data)->NickPrefix + ". " + name2;
			}
			else static_cast<PlayerInfo*>(peer->data)->displayName = str.substr(6, cch.length() - 6 - 1);
			if (static_cast<PlayerInfo*>(peer->data)->ltitle && static_cast<PlayerInfo*>(peer->data)->ltitleunlocked && static_cast<PlayerInfo*>(peer->data)->displayName.find(" of Legend``") == string::npos) {
				static_cast<PlayerInfo*>(peer->data)->displayName += " of Legend``";
			}
			static_cast<PlayerInfo*>(peer->data)->isNicked = true;
			if (isWorldOwner(peer, world))
			{
				static_cast<PlayerInfo*>(peer->data)->displayName = "`2" + static_cast<PlayerInfo*>(peer->data)->displayName;
				Player::OnNameChanged(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`2" + static_cast<PlayerInfo*>(peer->data)->displayName);
			}
			else {
				Player::OnNameChanged(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->displayName);
			}
		}
		else if (str == "/invis" || str == "/invisible")
		{
			if (static_cast<PlayerInfo*>(peer->data)->isinv == false)
			{
				static_cast<PlayerInfo*>(peer->data)->isinv = true;
				Player::OnConsoleMessage(peer, "`oYour now Invisible, no one can see you!");
				Player::PlayAudio(peer, "audio/boo_ghost_be_gone.wav", 0);
				gamepacket_t p(0, static_cast<PlayerInfo*>(peer->data)->netID);
				p.Insert("OnInvis");
				p.Insert(1);
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer))
					{
						p.CreatePacket(currentPeer);
					}
				}
			}
			else {
				Player::OnConsoleMessage(peer, "`oPeople can now see you!");
				static_cast<PlayerInfo*>(peer->data)->isinv = false;
				Player::PlayAudio(peer, "audio/boo_proton_glove.wav", 0);
				gamepacket_t p(0, static_cast<PlayerInfo*>(peer->data)->netID);
				p.Insert("OnInvis");
				p.Insert(0);
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer)) {
						p.CreatePacket(currentPeer);
						Player::PlayAudio(currentPeer, "audio/magic.wav", 0);
						for (int i = 0; i < 14; i++)
						{
							if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
							if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
							if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
							if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
							if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 16, 3, i * 300);
							if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 16, 3, i * 300);
							if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 16, 3, i * 300);
							if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 16, 3, i * 300);
						}
					}
				}
			}
		}
		else if (str == "/hidestatus")
		{
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->haveSuperSupporterName = false)
				{
					Player::OnConsoleMessage(peer, "Supporter name has been restored!");
					static_cast<PlayerInfo*>(currentPeer->data)->haveSuperSupporterName = true;
				}
				else
				{
					Player::OnConsoleMessage(peer, "You have lost your supporter name, to get it back use command again!");
					static_cast<PlayerInfo*>(currentPeer->data)->haveSuperSupporterName = false;
				}
			}
		}
		else if (str.substr(0, 9) == "/copyset ")
		{
			string name = str.substr(9, cch.length() - 9 - 1);
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;

				if (((PlayerInfo*)(currentPeer->data))->rawName == name || ((PlayerInfo*)(currentPeer->data))->tankIDName == name) {
					((PlayerInfo*)(peer->data))->cloth_hair = ((PlayerInfo*)(currentPeer->data))->cloth_hair;
					((PlayerInfo*)(peer->data))->cloth_shirt = ((PlayerInfo*)(currentPeer->data))->cloth_shirt;
					((PlayerInfo*)(peer->data))->cloth_pants = ((PlayerInfo*)(currentPeer->data))->cloth_pants;
					((PlayerInfo*)(peer->data))->cloth_feet = ((PlayerInfo*)(currentPeer->data))->cloth_feet;
					((PlayerInfo*)(peer->data))->cloth_face = ((PlayerInfo*)(currentPeer->data))->cloth_face;
					((PlayerInfo*)(peer->data))->cloth_hand = ((PlayerInfo*)(currentPeer->data))->cloth_hand;
					((PlayerInfo*)(peer->data))->cloth_back = ((PlayerInfo*)(currentPeer->data))->cloth_back;
					((PlayerInfo*)(peer->data))->cloth_mask = ((PlayerInfo*)(currentPeer->data))->cloth_mask;
					((PlayerInfo*)(peer->data))->cloth_necklace = ((PlayerInfo*)(currentPeer->data))->cloth_necklace;
					((PlayerInfo*)(peer->data))->cloth_ances = ((PlayerInfo*)(currentPeer->data))->cloth_ances;
					((PlayerInfo*)(peer->data))->skinColor = ((PlayerInfo*)(currentPeer->data))->skinColor;
					sendClothes(peer);
					Player::OnAddNotification(peer, "`wYou Copy Player " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w's Sets!", "audio/double_chance.wav", "interface/large/epic.rttex");
					Player::PlayAudio(peer, "change_clothes.wav", 0);
				}
			}
		}

		else if (str.substr(0, 5) == "/stop") {
		string userdisplay = "";
		for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
		{
			if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
			{
				userdisplay = static_cast<PlayerInfo*>(peer->data)->displayName;
				Player::OnConsoleMessage(currentPeer, "`4Global System Message From " + userdisplay + "`9Server Stoping Maintenance!");
				Player::OnAddNotification(currentPeer, "Restart in 3...", "", "");
				Sleep(598000);
				Player::OnAddNotification(currentPeer, "Restart in 2...", "", "");
				Sleep(599000);
				Player::OnAddNotification(currentPeer, "Restart in 1...", "", "");
				exit(0);
				return;
			}
		}
		
}


		else if (str.substr(0, 4) == "/sb ")
		{
			auto price = 2000;
			auto cooldown_sec = 360;
			if (static_cast<PlayerInfo*>(peer->data)->lastSB != 0)
			{
				Player::OnConsoleMessage(peer, "`4Super Broadcast Cooldown! `owait a few minutes and /sb again");
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->isDuctaped == true)
			{
				Player::OnConsoleMessage(peer, "`oSorry, you can't broadcast while your ducttaped / Muted!");
				return;
			}
			if (static_cast<PlayerInfo*>(peer->data)->level < 3 && !static_cast<PlayerInfo*>(peer->data)->Subscriber)
			{
				Player::OnConsoleMessage(peer, "`o>> `4OOPS: `oTo cut down on `4spam `othe broadcast features are only available to `5Supporters `owho are level `53 `oand higher. To become a `5Supporter`o, you must have a subscripion active - you'll get new skin colors, and `5Recycle `otool, and more!");
				return;
			}
			ifstream ifsz("save/gemdb/_" + static_cast<PlayerInfo*>(peer->data)->rawName + ".txt");
			string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
			int b = atoi(content.c_str());
			if (b > price)
			{
				ENetPeer* currentPeer;
				for (currentPeer = server->peers;
					currentPeer < &server->peers[server->peerCount];
					++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					ifstream read_player("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
					json j;
					read_player >> j;
					read_player.close();
					j["timesb"] = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(cooldown_sec)));
					ofstream write_player("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
					write_player << j << std::endl;
					write_player.close();
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(currentPeer->data)->rawName)
					{
						static_cast<PlayerInfo*>(currentPeer->data)->isSBcooldown = true;
						static_cast<PlayerInfo*>(currentPeer->data)->lastSB = (GetCurrentTimeInternalSeconds()) + (static_cast<long long>(cooldown_sec));
					}
					int gembux = b -= price;
					ofstream myfile;
					myfile.open("save/gemdb/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".txt");
					myfile << gembux;
					myfile.close();
					Player::OnSetBux(peer, gembux, 0);
					string sb_text = str.substr(4, cch.length() - 4 - 1);
					string worldname = static_cast<PlayerInfo*>(peer->data)->currentWorld;
					if (jammers)
					{
						for (auto i = 0; i < world->width * world->height; i++)
						{
							if (world->items.at(i).foreground == 226 && world->items.at(i).activated)
							{
								worldname = "`4JAMMED!";
								break;
							}
						}
					}
					GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[SB]_ `#** `#from (`2" + static_cast<PlayerInfo*>(peer->data)->displayName + "`#) in [`o" + worldname + "`#] ** :`o " + sb_text));
					string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length());
					memcpy(data + 4 + text.length(), &zero, 1);
					if (!static_cast<PlayerInfo*>(currentPeer->data)->radio) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->isIn == false) continue;
					ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet);
					ENetPacket* packet2 = enet_packet_create(data, 5 + text.length(), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(currentPeer, 0, packet2);
					static_cast<PlayerInfo*>(currentPeer->data)->lastSbbWorld = static_cast<PlayerInfo*>(peer->data)->currentWorld;
					delete[] data;
					delete p.data;
				}
			}
		}
		else if (str == "/sb") {
			int total = 0;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->adminLevel >= 0 && static_cast<PlayerInfo*>(currentPeer->data)->isinv == false) {
					total++;
				}
				GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`o>> This lets you send a message to Everyone (" + to_string(total) + ") online and make an annoying beep noise. Costs `92,000 Gems `oto use. Example: /sb Hey guys, come to my world! Your allowed to SB Every Few minutes! (`4IF FOUND SB SPAMMING YOU WILL BE BANNED FOR BUG ABUSE`o)"));
				ENetPacket* packet = enet_packet_create(p.data,
					p.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete p.data;
			}
		}
		else if (str.substr(0, 4) == "/gc ")
		{
			if (static_cast<PlayerInfo*>(peer->data)->guild != "")
			{
				string name = static_cast<PlayerInfo*>(peer->data)->displayName;
				GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`w[`5GUILD CHAT`w] [`4" + static_cast<PlayerInfo*>(peer->data)->tankIDName + "`w]  = " + str.substr(3, cch.length() - 3 - 1)));
				ENetPeer* currentPeer;
				for (currentPeer = server->peers;
					currentPeer < &server->peers[server->peerCount];
					++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
						continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->currentWorld == "EXIT")
						continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->isIn == false)
						continue;
					if (find(static_cast<PlayerInfo*>(peer->data)->guildMembers.begin(), static_cast<PlayerInfo*>(peer->data)->guildMembers.end(), static_cast<PlayerInfo*>(currentPeer->data)->rawName) != static_cast<PlayerInfo*>(peer->data)->guildMembers.end())
					{
						ENetPacket* packet = enet_packet_create(p.data,
							p.len,
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(currentPeer, 0, packet);
					}
				}
				delete p.data;
			}
			else
			{
				Player::OnConsoleMessage(peer, "You won't see broadcasts anymore.");
				GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "You can't send a broadcast sense your in no guild!"));
				ENetPacket* packet = enet_packet_create(p.data,
					p.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				delete p.data;
				return;
			}
		}
		else if (str.substr(0, 6) == "/radio") {
			if (static_cast<PlayerInfo*>(peer->data)->radio) {
				Player::OnConsoleMessage(peer, "You won't see broadcasts anymore.");
				static_cast<PlayerInfo*>(peer->data)->radio = false;
			}
			else {
				Player::OnConsoleMessage(peer, "You will now see broadcasts again.");
				static_cast<PlayerInfo*>(peer->data)->radio = true;
			}
		}
		else if (str.substr(0, 7) == "/color ") {
			if (str.substr(7, cch.length() - 7 - 1).size() >= 20 || str.substr(7, cch.length() - 7 - 1).size() <= 0) return;
			int color = atoi(str.substr(7, cch.length() - 7 - 1).c_str());
			static_cast<PlayerInfo*>(peer->data)->skinColor = color;
			sendClothes(peer);
		}
		else if (str.substr(0, 4) == "/who") {
			sendWho(peer);
		}
		else if (str == "/credits") {
			std::ifstream credits("config+/server_config/credits.txt");
			std::stringstream buff;
			buff << credits.rdbuf();
			std::string creditsString(buff.str());
			Player::OnDialogRequest(peer, creditsString);
		}

		else if (str == "/daily") {
		std::ifstream daily("config+/server_config/daily.txt");
		std::stringstream buff;
		buff << daily.rdbuf();
		std::string creditsString(buff.str());
		Player::OnDialogRequest(peer, creditsString);
}
		else if (str.substr(0, 8) == "/muteall")
		{
			string userdisplay = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
				}
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				Player::OnConsoleMessage(currentPeer, "`#** `$The Gods `ohave duct-tape `2" + userdisplay + "`o's mouth `#**`o (`4/rules`o to see the rules!)");
				if (isHere(peer, currentPeer))
				{
					Player::OnAddNotification(currentPeer, "`wWarning from `4System`w: You've been muted by an `4Owner `wfor 10 minutes", "audio/hub_open.wav", "interface/atomic_button.rttex");
					Player::OnConsoleMessage(currentPeer, "`oDuct tape has covered your mouth! (`$Duct Tape`o mod added, `$10 minutes`o left)");
					Player::OnConsoleMessage(currentPeer, "`oWarning from `4System`o: You've been `4duct-taped `ofor 10 minutes");
					Player::PlayAudio(currentPeer, "audio/already_used.wav", 0);
					static_cast<PlayerInfo*>(currentPeer->data)->taped = true;
					static_cast<PlayerInfo*>(currentPeer->data)->isDuctaped = true;
					static_cast<PlayerInfo*>(currentPeer->data)->cantsay = true;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMuted = (GetCurrentTimeInternalSeconds()) + (10 * 60);
					send_state(currentPeer);
					sendClothes(currentPeer);
				}
			}
			static_cast<PlayerInfo*>(peer->data)->taped = false;
			static_cast<PlayerInfo*>(peer->data)->isDuctaped = false;
			static_cast<PlayerInfo*>(peer->data)->cantsay = false;
			static_cast<PlayerInfo*>(peer->data)->lastMuted = 0;
			send_state(peer);
			sendClothes(peer);
		}
		else if (str == "/unmuteall")
		{
			string userdisplay = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
				}
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					static_cast<PlayerInfo*>(currentPeer->data)->taped = false;
					static_cast<PlayerInfo*>(currentPeer->data)->isDuctaped = false;
					static_cast<PlayerInfo*>(currentPeer->data)->cantsay = false;
					static_cast<PlayerInfo*>(currentPeer->data)->lastMuted = 0;
					send_state(currentPeer);
					sendClothes(currentPeer);
				}
			}
			static_cast<PlayerInfo*>(peer->data)->taped = false;
			static_cast<PlayerInfo*>(peer->data)->isDuctaped = false;
			static_cast<PlayerInfo*>(peer->data)->cantsay = false;
			static_cast<PlayerInfo*>(peer->data)->lastMuted = 0;
			send_state(peer);
			sendClothes(peer);
		}
		else if (str == "/update" || str == "/restart")
		{
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				{
					Player::OnConsoleMessage(currentPeer, "** Global System Message: `4Server Restart for update!");
					Player::OnConsoleMessage(currentPeer, "`4Global System Message``: Restarting server for update in `410 ``minute");
					Sleep(60000);
					Player::OnConsoleMessage(currentPeer, "`4Global System Message``: Restarting server for update in `410 ``minute");
					Sleep(360000);
					Player::OnConsoleMessage(currentPeer, "`4Global System Message``: Restarting server for update in `45 ``minute");
					Sleep(540000);
					Player::OnConsoleMessage(currentPeer, "`4Global System Message``: Restarting server for update in `41 ``minute");
					Sleep(597000);
					Player::OnAddNotification(currentPeer, "Restart in 3...", "", "");
					Sleep(598000);
					Player::OnAddNotification(currentPeer, "Restart in 2...", "", "");
					Sleep(599000);
					Player::OnAddNotification(currentPeer, "Restart in 1...", "", "");
					exit(0);
					return;
				}
			}
		}
		else if (str.substr(0, 5) == "/fsb ")
		{
			string userdisplay = "";
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					userdisplay = static_cast<PlayerInfo*>(peer->data)->displayName;
					Player::OnAddNotification(currentPeer, userdisplay + "`0: " + str.substr(5, cch.length() - 5 - 1), "audio/double_chance.wav", "");
				}
			}
		}
		else if (str == "/mods")
		{
			string online = "";
			int total = 0;
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (static_cast<PlayerInfo*>(currentPeer->data)->isinv == false && static_cast<PlayerInfo*>(currentPeer->data)->adminLevel == 1)
				{
					if (static_cast<PlayerInfo*>(currentPeer->data)->displayName == "" || static_cast<PlayerInfo*>(currentPeer->data)->rawName == "") continue;
					online += static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o ";
					total++;
				}
			}
			Player::OnConsoleMessage(peer, "Mods online (" + to_string(total) + "): `o" + online);
		}
		else if (str.substr(0, 5) == "/bugs" || str == "/BUGS" || str == "/BUGLIST" || str == "/bugslist") {
			std::ifstream bugs("config+/server_config/bugs.txt");
			std::stringstream buff;
			buff << bugs.rdbuf();
			std::string bugsString(buff.str());
			Player::OnDialogRequest(peer, bugsString);
		}


		else if (str == "/howgay" || str == "/HOWGAY") {
			int random = 0;
			(random = rand() % 100 + 1);
			Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`oYou are " + to_string(random) + " % gay.", 0, false);
			return;
		}
		else if (str == "/magic") {
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
				if (isHere(peer, currentPeer))
				{
					Player::PlayAudio(currentPeer, "audio/magic.wav", 0);
					for (int i = 0; i < 14; i++)
					{
						if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
						if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
						if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
						if (rand() % 100 <= 75) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 6 + 1, 2, i * 300);
						if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 16, 3, i * 300);
						if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y - 15 * (rand() % 6), rand() % 16, 3, i * 300);
						if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x + 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 16, 3, i * 300);
						if (rand() % 100 <= 25) SendParticleEffect(currentPeer, static_cast<PlayerInfo*>(peer->data)->x - 15 * (rand() % 6), static_cast<PlayerInfo*>(peer->data)->y + 15 * (rand() % 6), rand() % 16, 3, i * 300);
					}
				}
			}
		}
		else if (str.rfind("/", 0) == 0 && str != "/cheer" && str != "/dance" && str != "/cry" && str != "/troll" && str != "/sleep" && str != "/dance2" && str != "/love" && str != "/dab" && str != "/wave" && str != "/furious" && str != "/fp" && str != "/yes" && str != "/no" && str != "/omg" && str != "/idk" && str != "/rolleyes" && str != "/fold" && str != "/sassy") {
			sendWrongCmd(peer);
		}
		else {
			if (str.rfind("/", 0) == 0) return;
			message = trimString(message);
			string check_msg = message;
			toUpperCase(check_msg);
			if (check_msg == ":)" || check_msg == ":(" || check_msg == ":*" || check_msg == ":'(" || check_msg == ":D" || check_msg == ":O" || check_msg == ";)" || check_msg == ":O.O" || check_msg == ":p") {
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer)) {
						Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(peer->data)->netID, check_msg, 0, false);
					}
				}
				return;
			}
			string ccode = "", chatcode = "";
			ccode = role_chat.at(static_cast<PlayerInfo*>(peer->data)->adminLevel);
			chatcode = role_chat.at(static_cast<PlayerInfo*>(peer->data)->adminLevel);
			if (ccode == "w") chatcode = "$";
			if (static_cast<PlayerInfo*>(peer->data)->isNicked && mod(peer)) {
				ccode = "w";
				chatcode = "$";
			}  if (static_cast<PlayerInfo*>(peer->data)->Subscriber && static_cast<PlayerInfo*>(peer->data)->chatcolor != "" || admin(peer) && static_cast<PlayerInfo*>(peer->data)->chatcolor != "") {
				ccode = static_cast<PlayerInfo*>(peer->data)->chatcolor;
				chatcode = static_cast<PlayerInfo*>(peer->data)->chatcolor;
			}
			if (allCharactersSame(message)) return;
			for (auto c : message) {
				if (c < 0x18 || std::all_of(message.begin(), message.end(), static_cast<int(*)(int)>(isspace))) return;
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer))
				{
					Player::OnConsoleMessage(currentPeer, "CP:_PL:0_OID:_CT:[W]_ `o<`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`o> " "`" + ccode + message);
					Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(peer->data)->netID, "`" + ccode + message, 0, false);
				}
			}
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
}