#pragma once
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)
#pragma warning(disable: 4309)
#pragma warning(disable: 4305)
#include "playerDB.h"
#include "worldDB.h"
#include "trade_system.h"
#include <thread>
#include <regex>
#include <sstream>

inline void playerRespawn(WorldInfo* world, ENetPeer* peer, const bool isDeadByTile) {
	if (static_cast<PlayerInfo*>(peer->data)->trade) end_trade(peer);
	if (static_cast<PlayerInfo*>(peer->data)->Fishing) {
		static_cast<PlayerInfo*>(peer->data)->TriggerFish = false;
		static_cast<PlayerInfo*>(peer->data)->FishPosX = 0;
		static_cast<PlayerInfo*>(peer->data)->FishPosY = 0;
		static_cast<PlayerInfo*>(peer->data)->Fishing = false;
		send_state(peer);
		Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wSit perfectly when fishing!", 0, false);
		Player::OnSetPos(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->x, static_cast<PlayerInfo*>(peer->data)->y);
	}
	auto netID = static_cast<PlayerInfo*>(peer->data)->netID;
	if (isDeadByTile == false) {
		Player::OnKilled(peer, static_cast<PlayerInfo*>(peer->data)->netID);
	}
	auto p2x = packetEnd(appendInt(appendString(createPacket(), "OnSetFreezeState"), 0));
	memcpy(p2x.data + 8, &netID, 4);
	auto respawnTimeout = 2000;
	auto deathFlag = 0x19;
	memcpy(p2x.data + 24, &respawnTimeout, 4);
	memcpy(p2x.data + 56, &deathFlag, 4);
	const auto packet2x = enet_packet_create(p2x.data, p2x.len, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet2x);
	delete p2x.data;
	const auto p5 = packetEnd(appendInt(appendString(createPacket(), "OnSetFreezeState"), 2));
	memcpy(p5.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
	const auto packet5 = enet_packet_create(p5.data, p5.len, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet5);
	GamePacket p2;
	auto x = 3040;
	auto y = 736;
	try {
		for (auto i = 0; i < world->width * world->height; i++) {
			if (world->items.at(i).foreground == 6) {
				x = (i % world->width) * 32;
				y = (i / world->width) * 32;
				break;
			}
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
	if (static_cast<PlayerInfo*>(peer->data)->ischeck) {
		p2 = packetEnd(appendFloat(appendString(createPacket(), "OnSetPos"), static_cast<PlayerInfo*>(peer->data)->checkx, static_cast<PlayerInfo*>(peer->data)->checky));
	}
	else {
		p2 = packetEnd(appendFloat(appendString(createPacket(), "OnSetPos"), x, y));
	}
	memcpy(p2.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
	respawnTimeout = 2000;
	memcpy(p2.data + 24, &respawnTimeout, 4);
	memcpy(p2.data + 56, &deathFlag, 4);
	const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet2);
	delete p2.data;
	auto p2a = packetEnd(appendString(appendString(createPacket(), "OnPlayPositioned"), "audio/teleport.wav"));
	memcpy(p2a.data + 8, &netID, 4);
	respawnTimeout = 2000;
	memcpy(p2a.data + 24, &respawnTimeout, 4);
	memcpy(p2a.data + 56, &deathFlag, 4);
	const auto packet2a = enet_packet_create(p2a.data, p2a.len, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet2a);
	delete p2a.data;
}

inline void sendTileUpdate(int x, int y, int tile, int causedBy, ENetPeer* peer, WorldInfo* world)
{
	PlayerInfo* pData = GetPeerData(peer);
	if (pData->trade) end_trade(peer);
	Player::Ping(peer);
	if (world == NULL || pData->isIn == false || pData->currentWorld == "EXIT" || tile > itemDefs.size()) return;
	auto dicenr = 0;
	PlayerMoving data{};
	data.packetType = 0x3;
	data.characterState = 0x0;
	data.x = x;
	data.y = y;
	data.punchX = x;
	data.punchY = y;
	data.XSpeed = 0;
	data.YSpeed = 0;
	data.netID = causedBy;
	data.plantingTree = tile;
	int squaresign = x + (y * world->width);
	bool isTree = false, isLock = false, isSmallLock = false, isHeartMonitor = false, isMannequin = false, isScience = false, isMagplant = false, isgateway = false, iscontains = false, VendUpdate = false, isDoor = false;
	if (world == nullptr || x < 0 || y < 0 || world->name == "HELL" && pData->isCursed || x > world->width - 1 || y > world->height - 1 || tile > itemDefs.size()) return;
	if (tile != 18 && tile != 32 && tile != 6336) {
		auto contains = false;
		SearchInventoryItem(peer, tile, 1, contains);
		if (!contains) return;
	}
	if (world->saved) world->saved = false;
	if (tile == 18) pData->totalpunch++;
	try {
		if (world->items.at(x + (y * world->width)).background == 3556 && pData->cloth_hand != 3066 && world->width == 90 && world->height == 60 && world->items.at(x + (y * world->width)).foreground == 0 || world->items.at(x + (y * world->width)).foreground == 9150 && pData->cloth_hand != 3066 && world->width == 90 && world->height == 60) {
			if (tile == 18 || getItemDef(tile).blockType == BlockTypes::BACKGROUND) return;
		}
		switch (tile) {
		case 18:
		{
			if (world->items.at(x + (y * world->width)).foreground == 1482) {
				sendTime(peer);
			}
			if (world->items.at(x + (y * world->width)).foreground == 2252) {
				sendTime(peer);
			}
			if (world->items.at(x + (y * world->width)).foreground == 7864) {
				sendTime(peer);
			}
			if (world->items.at(x + (y * world->width)).foreground == 5638) {
				bool canuseremote = true;
				if (canuseremote) {
					if (pData->inventory.items.size() == pData->currentInventorySize) {
					}
					else {
						bool iscontainss = false;
						SearchInventoryItem(peer, 5640, 1, iscontainss);
						if (!iscontainss) {
							if (isWorldOwner(peer, world) && world->items.at(x + (y * world->width)).mid != 0 && world->items.at(x + (y * world->width)).mc != 0 || world->items.at(x + (y * world->width)).rm && world->items.at(x + (y * world->width)).mid != 0 && world->items.at(x + (y * world->width)).mc != 0) {
								bool Farmable = false;
								if (world->items.at(x + (y * world->width)).mid == 7382 || world->items.at(x + (y * world->width)).mid == 4762 || world->items.at(x + (y * world->width)).mid == 5140 || world->items.at(x + (y * world->width)).mid == 5138 || world->items.at(x + (y * world->width)).mid == 5136 || world->items.at(x + (y * world->width)).mid == 5154 || world->items.at(x + (y * world->width)).mid == 340 || world->items.at(x + (y * world->width)).mid == 954 || world->items.at(x + (y * world->width)).mid == 5666) Farmable = true;
								if (world->items.at(x + (y * world->width)).foreground == 5638 && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::FOREGROUND || world->items.at(x + (y * world->width)).foreground == 5638 && Farmable || world->items.at(x + (y * world->width)).foreground == 5638 && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::BACKGROUND || world->items.at(x + (y * world->width)).foreground == 5638 && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::GROUND_BLOCK) {
									if (getItemDef(world->items.at(x + (y * world->width)).mid).properties & Property_AutoPickup) {
									}
									else {
										Player::OnTalkBubble(peer, pData->netID, "`wYou received a MAGPLANT 5000 Remote.", 0, true);
										bool success = true;
										SaveItemMoreTimes(5640, 1, peer, success);
										pData->magplantitemid = world->items.at(x + (y * world->width)).mid;
										pData->magplantx = x;
										pData->magplanty = y;
									}
								}
							}
						}
						else {
							pData->magplantitemid = world->items.at(x + (y * world->width)).mid;
							pData->magplantx = x;
							pData->magplanty = y;
						}
					}
				}
			}
			if (pData->cloth_hand == 3066) {
				if (world->items.at(x + (y * world->width)).fire) {
					int chanceofgems = 1;
					if (pData->firefighterlevel >= 4) chanceofgems = 2;
					if (pData->firefighterlevel >= 6) chanceofgems = 3;
					if (pData->firefighterlevel >= 8) chanceofgems = 4;
					if (pData->firefighterlevel >= 10) chanceofgems = 5;
					if (pData->firefighterlevel >= 2 && rand() % 100 <= chanceofgems) {
						auto Gems = (rand() % 25) + 1;
						auto currentGems = 0;
						ifstream ifs("save/gemdb/_" + pData->rawName + ".txt");
						ifs >> currentGems;
						ifs.close();
						currentGems += Gems;
						ofstream ofs("save/gemdb/_" + pData->rawName + ".txt");
						ofs << currentGems;
						ofs.close();
						Player::OnConsoleMessage(peer, "Fire King bonus obtained " + to_string(Gems) + " gems");
						Player::OnSetBux(peer, currentGems, 0);
					}
					else if (pData->firefighterlevel >= 3 && rand() % 100 <= 1) {
						bool success = true;
						SaveItemMoreTimes(4762, 1, peer, success, "");
						Player::OnConsoleMessage(peer, "Obtained Amethyst Block");
					}
					else if (pData->firefighterlevel >= 4 && rand() % 100 <= 2) {
						bool success = true;
						SaveItemMoreTimes(7156, 1, peer, success, "");
						Player::OnConsoleMessage(peer, "Obtained Fallen Pillar");
					}
					else if (pData->firefighterlevel >= 7 && rand() % 100 <= 1) {
						bool success = true;
						SaveItemMoreTimes(5138, 1, peer, success, "");
						Player::OnConsoleMessage(peer, "Obtained Diamond Stone");
					}
					else if (pData->firefighterlevel == 10 && rand() % 300 <= 1 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::BACKGROUND || pData->firefighterlevel == 10 && rand() % 300 <= 1 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::SFX_FOREGROUND || pData->firefighterlevel == 10 && rand() % 300 <= 1 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::FOREGROUND) {
						bool success = true;
						SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success, "");
						Player::OnConsoleMessage(peer, "Obtained " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "");
					}
					int targetfirelevel = 1500;
					if (pData->firefighterlevel > 0) targetfirelevel = targetfirelevel * pData->firefighterlevel;
					if (pData->firefighterlevel == 0) targetfirelevel = 750;
					if (pData->firefighterxp >= targetfirelevel && pData->firefighterlevel < 10) {
						pData->firefighterlevel++;
						pData->firefighterxp = 0;
						SyncPlayerRoles(peer, pData->firefighterlevel, "firefighter");
					}
					else {
						pData->firefighterxp++;
					}
					world->items.at(x + (y * world->width)).fire = false;
					UpdateBlockState(peer, x, y, true, world);
					ENetPeer* net_peer;
					for (net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
						if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
						if (isHere(peer, net_peer)) {
							Player::OnParticleEffect(net_peer, 149, x * 32, y * 32, 0);
						}
					}
				}
				return;
			}
			if (pData->cloth_hand == 4996 && !world->items.at(x + (y * world->width)).fire) {
				if (isSeed(world->items.at(x + (y * world->width)).foreground) || world->items.at(x + (y * world->width)).water || world->items.at(x + (y * world->width)).foreground == 6952 || world->items.at(x + (y * world->width)).foreground == 6954 || world->items.at(x + (y * world->width)).foreground == 5638 || world->items.at(x + (y * world->width)).foreground == 6946 || world->items.at(x + (y * world->width)).foreground == 6948 || world->items.at(x + (y * world->width)).foreground == 2978 || world->items.at(x + (y * world->width)).foreground == 1420 || world->items.at(x + (y * world->width)).foreground == 6214 || world->items.at(x + (y * world->width)).foreground == 1006 || world->items.at(x + (y * world->width)).foreground == 656 || world->items.at(x + (y * world->width)).foreground == 1420 || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DONATION || world->items.at(x + (y * world->width)).foreground == 3528 || world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 6864 || world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0 || world->items.at(x + (y * world->width)).foreground == 6 || world->items.at(x + (y * world->width)).foreground == 8 || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DISPLAY) {
					if (world->items.at(x + (y * world->width)).background != 6864) Player::OnTalkBubble(peer, pData->netID, "`wCan't burn that!", 0, true);
					return;
				}
				world->items.at(x + (y * world->width)).fire = true;
				for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
					if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
					if (isHere(peer, net_peer)) {
						Player::OnParticleEffect(net_peer, 150, x * 32 + 16, y * 32 + 16, 0);
					}
				}
				UpdateVisualsForBlock(peer, true, x, y, world);
				return;
			}
			if (world->items.at(x + (y * world->width)).fire) return;
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::PROVIDER) {
				if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner) || admin(peer)) {
					if (calcBanDuration(world->items.at(x + (y * world->width)).growtime) == 0) {
						if (pData->quest_active && pData->lastquest == "honor" && pData->quest_step == 13 && pData->quest_progress < 1000) {
							pData->quest_progress++;
							if (pData->quest_progress >= 1000) {
								pData->quest_progress = 1000;
								Player::OnTalkBubble(peer, pData->netID, "`9Legendary Quest step complete! I'm off to see a Wizard!", 0, false);
							}
						}
						int chanceofdouble = 1;
						int weedmagic = 1;
						if (pData->providerlevel >= 4) chanceofdouble = 2;
						if (pData->providerlevel >= 6) chanceofdouble = 3;
						if (pData->providerlevel >= 8) chanceofdouble = 4;
						if (pData->providerlevel >= 10) chanceofdouble = 5;
						else if (pData->providerlevel >= 7 && rand() % 100 <= 1) {
							bool success = true;
							SaveItemMoreTimes(2410, 1, peer, success, "");
							Player::OnConsoleMessage(peer, "Obtained Emerald Shard");
						}
						else if (pData->providerlevel == 10 && rand() % 300 <= 1) {
							Player::OnConsoleMessage(peer, "The " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + " has dropped himself");
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), world->items.at(x + (y * world->width)).foreground, 1, 0);
						}
						int targetproviderlevel = 1300;
						if (pData->providerlevel > 0) targetproviderlevel = targetproviderlevel * pData->providerlevel;
						if (pData->providerlevel == 0) targetproviderlevel = 600;
						if (pData->providerxp >= targetproviderlevel && pData->providerlevel < 10) {
							pData->providerlevel++;
							pData->providerxp = 0;
							SyncPlayerRoles(peer, pData->providerlevel, "provider");
						}
						else {
							pData->providerxp++;
						}

						if (world->items.at(x + (y * world->width)).foreground == 872) {
							if (rand() % 100 <= 25) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 874, 2 * weedmagic, 0);
							else DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 874, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 866) {
							if (rand() % 100 <= 25) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 868, 2 * weedmagic, 0);
							else DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 868, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 3888) {
							if (pData->cloth_hand == 3914) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 3890, rand() % 5 + 1 * weedmagic, 0);
							else DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 3890, rand() % 2 + 1 * weedmagic, 0);
						}
						if (world->items.at(x + (y * world->width)).foreground == 8196) {
							int ItemID = rand() % maxItems;
							while (getItemDef(ItemID).blockType == BlockTypes::CLOTHING || getItemDef(ItemID).properties & Property_Untradable || getItemDef(ItemID).name.find("Ancestral") != string::npos || getItemDef(ItemID).name.find("null_item") != string::npos || isSeed(ItemID) || ItemID == 1458) {
								ItemID = rand() % maxItems;
							}
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1, 0);
							for (ENetPeer* peer2 = server->peers; peer2 < &server->peers[server->peerCount]; ++peer2) {
								if (peer2->state != ENET_PEER_STATE_CONNECTED) continue;
								if (isHere(peer, peer2)) {
									Player::OnParticleEffect(peer2, 182, x * 32, y * 32, 0);
								}
							}
						}
						if (world->items.at(x + (y * world->width)).foreground == 10450) {
							int ItemID = rand() % maxItems;
							while (getItemDef(ItemID).blockType != BlockTypes::CLOTHING || isSeed(ItemID) || ItemID == 18 || ItemID == 32 || ItemID == 6336 || ItemID == 9510 || ItemID == 9506) {
								ItemID = rand() % maxItems;
							}
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1, 0);
							for (ENetPeer* peer2 = server->peers; peer2 < &server->peers[server->peerCount]; ++peer2) {
								if (peer2->state != ENET_PEER_STATE_CONNECTED) continue;
								if (isHere(peer, peer2)) {
									Player::OnParticleEffect(peer2, 182, x * 32, y * 32, 0);
								}
							}
						}
						if (world->items.at(x + (y * world->width)).foreground == 928) {
							vector<int> Dailyb{ 914, 916, 918, 920, 924 };
							const int Index = rand() % Dailyb.size();
							const auto ItemID = Dailyb[Index];
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1 * weedmagic, 0);
						}
						if (world->items.at(x + (y * world->width)).foreground == 1044) {
							if (rand() % 100 <= 25) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 868, 2 * weedmagic, 0);
							else DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 868, 1 * weedmagic, 0);
						}
						if (world->items.at(x + (y * world->width)).foreground == 2798) {
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 822, 1 * weedmagic, 0);
						}
						if (world->items.at(x + (y * world->width)).foreground == 1008) {
							SendFarmableDrop(peer, 500, x, y, world);
						}
						if (world->items.at(x + (y * world->width)).foreground == 1636) {
							vector<int> Dailyb{ 728, 360, 308, 306, 2966, 1646, 3170, 1644, 1642, 3524, 1640, 1638, 2582, 3198, 8838, 6794, 10110 };
							const int Index = rand() % Dailyb.size();
							const auto ItemID = Dailyb[Index];
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 3044) {
							vector<int> Dailyb{ 2914, 3012, 3014, 3016, 3018, 5528, 5526 };
							const int Index = rand() % Dailyb.size();
							const auto ItemID = Dailyb[Index];
							if (rand() % 100 <= 25) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 2 * weedmagic, 0);
							else DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 6212) {
							vector<int> Dailyb{ 1258, 1260, 1262, 1264, 1266, 1268, 1270, 4308, 4310, 4312, 4314, 4316, 4318 };
							const int Index = rand() % Dailyb.size();
							const auto ItemID = Dailyb[Index];
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 6414) {
							vector<int> Dailyb{ 6520, 6538, 6522, 6528, 6540, 6518, 6530, 6524, 6536, 6534, 6532, 6526, 6416 };
							const int Index = rand() % Dailyb.size();
							const auto ItemID = Dailyb[Index];
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), ItemID, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 5116) {
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 5114, 1 * weedmagic, 0);
						}

						if (world->items.at(x + (y * world->width)).foreground == 1632) {
							DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 1634, 1 * weedmagic, 0);
						}

						int buvoid = world->items.at(x + (y * world->width)).foreground;
						world->items.at(x + (y * world->width)).foreground = 0;
						PlayerMoving data3{};
						data3.packetType = 0x3;
						data3.characterState = 0x0;
						data3.x = x;
						data3.y = y;
						data3.punchX = x;
						data3.punchY = y;
						data3.XSpeed = 0;
						data3.YSpeed = 0;
						data3.netID = -1;
						data3.plantingTree = 0;
						ENetPeer* currentPeer;
						for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer)) {
								auto raw = packPlayerMoving(&data3);
								raw[2] = dicenr;
								raw[3] = dicenr;
								SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
							}
						}
						world->items.at(x + (y * world->width)).foreground = buvoid;
						PlayerMoving data4{};
						data4.packetType = 0x3;
						data4.characterState = 0x0;
						data4.x = x;
						data4.y = y;
						data4.punchX = x;
						data4.punchY = y;
						data4.XSpeed = 0;
						data4.YSpeed = 0;
						data4.netID = -1;
						data4.plantingTree = buvoid;
						for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer)) {
								auto raw = packPlayerMoving(&data4);
								raw[2] = dicenr;
								raw[3] = dicenr;
								SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
							}
						}
						world->items.at(x + (y * world->width)).growtime = (GetCurrentTimeInternalSeconds() + getItemDef(world->items.at(x + (y * world->width)).foreground).growTime);
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 1420 || world->items.at(x + (y * world->width)).foreground == 6214)
			{
				if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner))
				{
					auto seedexist = std::experimental::filesystem::exists("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					if (seedexist)
					{
						json j;
						ifstream fs("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						fs >> j;
						fs.close();
						bool found = false, success = false;
						if (j["clothHead"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothHead"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothHead"] = "0";
						}
						else if (j["clothHair"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothHair"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothHair"] = "0";
						}
						else if (j["clothMask"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothMask"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothMask"] = "0";
						}
						else if (j["clothNeck"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothNeck"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothNeck"] = "0";
						}
						else if (j["clothBack"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothBack"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothBack"] = "0";
						}
						else if (j["clothShirt"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothShirt"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothShirt"] = "0";
						}
						else if (j["clothPants"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothPants"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothPants"] = "0";
						}
						else if (j["clothFeet"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothFeet"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothFeet"] = "0";
						}
						else if (j["clothHand"].get<string>() != "0")
						{
							SaveItemMoreTimes(atoi(j["clothHand"].get<string>().c_str()), 1, peer, success, pData->rawName + " from Mannequin");
							found = true;
							j["clothHand"] = "0";
						}

						if (found)
						{
							updateMannequin(peer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).sign,
								atoi(j["clothHair"].get<string>().c_str()), atoi(j["clothHead"].get<string>().c_str()),
								atoi(j["clothMask"].get<string>().c_str()), atoi(j["clothHand"].get<string>().c_str()), atoi(j["clothNeck"].get<string>().c_str()),
								atoi(j["clothShirt"].get<string>().c_str()), atoi(j["clothPants"].get<string>().c_str()), atoi(j["clothFeet"].get<string>().c_str()),
								atoi(j["clothBack"].get<string>().c_str()), true, 0);

							ofstream of("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
							of << j;
							of.close();
						}
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 758) sendRoulete(peer);
			bool hassmallock = false;
			for (int i = 0; i < world->width * world->height; i++) {
				if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) {
					hassmallock = true;
					break;
				}
			}
			if (hassmallock && !admin(peer) || world->owner != "" && !isWorldOwner(peer, world)) {
				if (!admin(peer)) {
					if (!restricted_area(peer, world, x, y)) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							if (pData->rawName != whoslock) {
								try {
									ifstream read_player("save/players/_" + whoslock + ".json");
									if (!read_player.is_open()) {
										return;
									}
									json j;
									read_player >> j;
									read_player.close();
									string nickname = j["nick"];
									int adminLevel = j["adminLevel"];
									if (nickname == "") {
										nickname = role_prefix.at(adminLevel) + whoslock;
									}
									if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
									else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
									Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									return;
								}
								return;
							}
						}
					}
					else if (isWorldAdmin(peer, world)) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`2Access Granted`w)", 0, true);
								Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
							return;
						}
					}
					else if (world->isPublic)
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK)
						{
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
								Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
							return;
						}
					}
					else if (world->isEvent) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK)
						{
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							if (pData->rawName != whoslock) {
								try {
									ifstream read_player("save/players/_" + whoslock + ".json");
									if (!read_player.is_open()) {
										return;
									}
									json j;
									read_player >> j;
									read_player.close();
									string nickname = j["nick"];
									int adminLevel = j["adminLevel"];
									if (nickname == "") {
										nickname = role_prefix.at(adminLevel) + whoslock;
									}
									if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
									else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
									Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									return;
								}
								return;
							}
						}
						else if (world->items.at(x + (y * world->width)).foreground != world->publicBlock && causedBy != -1) {
							if (world->items.at(x + (y * world->width)).foreground != 0) Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							else if (world->items.at(x + (y * world->width)).background != 0) Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							return;
						}
					}
					else {
						Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
						return;
					}
				}
				if (tile == 18 && admin(peer))
				{
					if (isWorldAdmin(peer, world) && !isWorldOwner(peer, world))
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK)
						{
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`2Access Granted`w)", 0, true);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
						}
					}
					else if (world->isPublic && !isWorldOwner(peer, world))
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK)
						{
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
						}
					}
					else if (world->isEvent && !isWorldOwner(peer, world))
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK)
						{
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							if (pData->rawName != whoslock) {
								try {
									ifstream read_player("save/players/_" + whoslock + ".json");
									if (!read_player.is_open()) {
										return;
									}
									json j;
									read_player >> j;
									read_player.close();
									string nickname = j["nick"];
									int adminLevel = j["adminLevel"];
									if (nickname == "") {
										nickname = role_prefix.at(adminLevel) + whoslock;
									}
									if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
									else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									return;
								}
							}
						}
					}
				}
			}
			if (!admin(peer)) {
				if (world->items.at(x + (y * world->width)).foreground == 6 || world->items.at(x + (y * world->width)).foreground == 8 || world->items.at(x + (y * world->width)).foreground == 9380 || world->items.at(x + (y * world->width)).foreground == 8440 && pData->cloth_hand != 98 || world->items.at(x + (y * world->width)).foreground == 9146 && pData->cloth_hand != 98 || world->items.at(x + (y * world->width)).foreground == 8532 && pData->cloth_hand != 98 || world->items.at(x + (y * world->width)).foreground == 8530 && pData->cloth_hand != 98) {
					Player::OnTalkBubble(peer, pData->netID, "`wIt's too strong to break.", 0, true);
					return;
				} if (world->items.at(x + (y * world->width)).foreground == 8 || world->items.at(x + (y * world->width)).foreground == 7372 || world->items.at(x + (y * world->width)).foreground == 3760) {
				} if (tile == 9444) {
					if (pData->cloth_hand == 2952 || pData->cloth_hand == 9430 || pData->cloth_hand == 9448 || pData->cloth_hand == 9452 || pData->cloth_hand == 2592) {
					}
					else {
						Player::OnTalkBubble(peer, pData->netID, "`5This stone is too strong!", 0, true);
						return;
					}
				}
				if (tile == 6 || tile == 8 || tile == 3760 || tile == 1000 || tile == 7372 || tile == 1770 || tile == 1832 || tile == 4720) {
					Player::OnTalkBubble(peer, pData->netID, "`wIt's too heavy to place.", 0, true);
					return;
				}
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::WEATHER && isWorldOwner(peer, world) || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::WEATHER && isWorldAdmin(peer, world) || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::WEATHER && admin(peer)) SendWeather(peer, tile, world, x, y);
			if (world->items.at(x + (y * world->width)).foreground == 3694) {
				if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (world->items.at(x + (y * world->width)).activated) {
						world->items.at(x + (y * world->width)).activated = false;
						world->weather = 0;
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 0);
							}
						}
					}
					else {
						world->items.at(x + (y * world->width)).activated = true;
						world->weather = 28;
						sendHeatwave(peer, x, y, world->items.at(x + (y * world->width)).vid, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).vcount);
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 28);
							}
						}
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 3832) {
				if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (world->items.at(x + (y * world->width)).activated) {
						world->items.at(x + (y * world->width)).activated = false;
						world->weather = 0;
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 0);
							}
						}
					}
					else {
						world->items.at(x + (y * world->width)).activated = true;
						world->weather = 29;
						sendStuffweather(peer, x, y, world->items.at(x + (y * world->width)).intdata, world->items.at(x + (y * world->width)).mc, world->items.at(x + (y * world->width)).rm, world->items.at(x + (y * world->width)).opened);
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 29);
							}
						}
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 5000) {
				if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (world->items.at(x + (y * world->width)).activated) {
						world->items.at(x + (y * world->width)).activated = false;
						world->weather = 0;
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 0);
							}
						}
					}
					else {
						world->items.at(x + (y * world->width)).activated = true;
						world->weather = 34;
						sendBackground(peer, x, y, world->items.at(x + (y * world->width)).intdata);
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, currentPeer)) {
								Player::OnSetCurrentWeather(currentPeer, 34);
							}
						}
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 2946) {
				if (world->isPublic || isWorldAdmin(peer, world)) {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					return;
				}
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::RANDOM_BLOCK) {
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer)) {
						send_dice(currentPeer, rand() % 5 + 1, x, y);
					}
				}
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::TOGGLE_FOREGROUND || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::CHEMICAL_COMBINER || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::CHEST) {
				if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::CHEMICAL_COMBINER) {
						if (world->items.at(x + (y * world->width)).activated) {
							SendCombiner(peer, world, x, y);
							UpdateBlockState(peer, x, y, true, world);
							world->items.at(x + (y * world->width)).activated = false;
						}
						else {
							UpdateBlockState(peer, x, y, true, world);
							world->items.at(x + (y * world->width)).activated = true;
						}
					}
					else {
						if (world->items.at(x + (y * world->width)).activated) {
							UpdateBlockState(peer, x, y, true, world);
							world->items.at(x + (y * world->width)).activated = false;
						}
						else {
							UpdateBlockState(peer, x, y, true, world);
							world->items.at(x + (y * world->width)).activated = true;
						}
					}
				}
			}
			if (isSeed(world->items.at(x + (y * world->width)).foreground) && calcBanDuration(world->items.at(x + (y * world->width)).growtime) == 0 && pData->cloth_hand != 3066) {
				if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (world->items.at(x + (y * world->width)).foreground == 1791) { /*lwizard*/
						world->items.at(x + (y * world->width)).foreground = 1790;
						PlayerMoving data3{};
						data3.packetType = 0x3;
						data3.characterState = 0x0;
						data3.x = x;
						data3.y = y;
						data3.punchX = x;
						data3.punchY = y;
						data3.XSpeed = 0;
						data3.YSpeed = 0;
						data3.netID = -1;
						data3.plantingTree = 1790;
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer)) {
								auto raw = packPlayerMoving(&data3);
								raw[2] = dicenr;
								raw[3] = dicenr;
								Player::OnParticleEffect(peer, 48, x * 32, y * 32, 0);
								SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
							}
						}
						return;
					}
					auto chance = (rand() % 100) + 1;
					int numb = world->items.at(x + (y * world->width)).fruitcount;
					DropGem(peer, x, y, world);
					string drop_message = getItemDef(world->items.at(x + (y * world->width)).foreground).name;
					int drop_id = world->items.at(x + (y * world->width)).foreground;
					if (world->items.at(x + (y * world->width)).foreground == 1259 || world->items.at(x + (y * world->width)).foreground == 1261 || world->items.at(x + (y * world->width)).foreground == 1263 || world->items.at(x + (y * world->width)).foreground == 1265 || world->items.at(x + (y * world->width)).foreground == 1267 || world->items.at(x + (y * world->width)).foreground == 1269 || world->items.at(x + (y * world->width)).foreground == 1271 || world->items.at(x + (y * world->width)).foreground == 4309 || world->items.at(x + (y * world->width)).foreground == 4311 || world->items.at(x + (y * world->width)).foreground == 4313 || world->items.at(x + (y * world->width)).foreground == 4315 || world->items.at(x + (y * world->width)).foreground == 4317 || world->items.at(x + (y * world->width)).foreground == 4319) {
						drop_message = "Surgical Tool Tree";
						vector<int> random_surg{ 1258, 1260, 1262, 1264, 1266, 1268, 1270, 4308, 4310, 4312, 4314, 4316, 4318 };
						drop_id = random_surg.at(rand() % random_surg.size()) + 1;
					} if (chance < 15 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity != 999) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground - 1).blockType != BlockTypes::CLOTHING) {
							Player::OnTalkBubble(peer, pData->netID, "A " + drop_message + " seed falls out!", 0, false);
							DropItem(world, peer, -1, x * 32 - (rand() % 8), y * 32 + (rand() % 8), world->items.at(x + (y * world->width)).foreground, 1, 0);
						}
					} if (pData->cloth_hand == 6840) {
						auto chance1 = (rand() % 100) + 1;
						if (chance1 <= 30) {
							numb += 1;
							Player::OnParticleEffect(peer, 49, data.punchX * 32, data.punchY * 32, 0);
						}
					}
					int chanceofbuff = 1;
					if (pData->level >= 10) chanceofbuff = 1;
					if (pData->level >= 13) chanceofbuff = 2;
					if (pData->level >= 10 && rand() % 100 <= chanceofbuff) {
						numb += 1;
						Player::OnConsoleMessage(peer, "Harvester bonus extra block drop");
					}
					DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), world->items.at(x + (y * world->width)).foreground - 1, numb, 0);
					if (pData->quest_active && pData->lastquest == "honor" && pData->quest_step == 15 && pData->quest_progress < 100000) {
						pData->quest_progress += getItemDef(world->items.at(x + (y * world->width)).foreground).rarity * world->items.at(x + (y * world->width)).fruitcount;
						if (pData->quest_progress >= 100000) {
							pData->quest_progress = 100000;
							Player::OnTalkBubble(peer, pData->netID, "`9Legendary Quest step complete! I'm off to see a Wizard!", 0, false);
						}
					}
					world->items.at(x + (y * world->width)).spliced = false;
					world->items.at(x + (y * world->width)).growtime = 0;
					world->items.at(x + (y * world->width)).fruitcount = 0;
					if (HarvestEvent) {
						SendHarvestFestivalDrop(world, peer, x, y, getItemDef(world->items.at(x + (y * world->width)).foreground).rarity);
					}
					if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 25) {
						SendXP(peer, 2);
					}
					else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 25 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 40) {
						SendXP(peer, 4);
					}
					else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 40 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 60) {
						SendXP(peer, 6);
					}
					else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 60 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 80) {
						SendXP(peer, 8);
					}
					else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 80 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 100) {
						SendXP(peer, 10);
					}
					else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 100) {
						SendXP(peer, 12);
					}
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer)) {
							treeModify(currentPeer, x, y, static_cast<PlayerInfo*>(currentPeer->data)->netID);
						}
					}
					world->items.at(x + (y * world->width)).foreground = 0;
					return;
				}
				return;
			}
			if (!pData->Fishing) break;
			SyncFish(world, peer);
		}
		case 32: /*wrench*/
		{
			if (world->items.at(x + (y * world->width)).foreground == 1790) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					pData->lastPunchX = x;
					pData->lastPunchY = y;
					pData->choosing_quest = "";
					if (!admin(peer)) {
						Player::OnTalkBubble(peer, pData->netID, "Legendary Wizard is disabled.", 0, true);
						return;
					}
					if (pData->lastquest == "" || !pData->quest_active) {
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`9The Legendary Wizard``|left|1790|\nadd_textbox|`oGreetings, traveler! I am the Legendary Wizard. Should you wich to embark on a Legendary Quest, simply choose one below.|\nadd_spacer|small|\nadd_button|honor|`9Quest For Honor|noflags|0|0|\nend_dialog|legendary_wizard|No Thanks||");
					}
					else {
						send_quest_view(peer, pData, world);
					}
				}
				else if (world->owner != "") {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 3694) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					pData->wrenchedBlockLocation = x + (y * world->width);
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWeather Machine - Heatwave``|left|3694|\nadd_spacer|small|\nadd_textbox|`oAdjust the color of your heat wave here, by including 0-255 of Red, Green, and Blue.|\nadd_spacer|small|\nadd_text_input|heatwave_red|Red|" + to_string(world->items.at(x + (y * world->width)).vid) + "|3|\nadd_text_input|heatwave_green|Green|" + to_string(world->items.at(x + (y * world->width)).vprice) + "|3|\nadd_text_input|heatwave_blue|Blue|" + to_string(world->items.at(x + (y * world->width)).vcount) + "|3|\nadd_quick_exit|\nend_dialog|weatherspcl9|Cancel|Okay|");
				}
				else if (world->owner != "") {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 3832) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					pData->wrenchedBlockLocation = x + (y * world->width);
					string item_name = getItemDef(world->items.at(x + (y * world->width)).intdata).name;
					string gravity = to_string(world->items.at(x + (y * world->width)).mc);
					string spin_items = "0";
					if (world->items.at(x + (y * world->width)).rm) spin_items = "1";
					string invert_sky_colors = "0";
					if (world->items.at(x + (y * world->width)).opened) invert_sky_colors = "1";
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWeather Machine - Stuff``|left|3832|\nadd_spacer|small|\nadd_item_picker|choose|Item: `2" + item_name + "``|Select any item to rain down|\nadd_text_input|gravity|Gravity:|" + gravity + "|5|\nadd_checkbox|spin|Spin Items|" + spin_items + "\nadd_checkbox|invert|Invert Sky Colors|" + invert_sky_colors + "\nend_dialog|weatherspcl|Cancel|Okay|");
				}
				else if (world->owner != "") {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 5000) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					pData->wrenchedBlockLocation = x + (y * world->width);
					string item_name = getItemDef(world->items.at(x + (y * world->width)).intdata).name;
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWeather Machine - Background``|left|5000|\nadd_spacer|small|\nadd_textbox|You can scan any Background Block to set it up in your weather machine.|left|\nadd_item_picker|choose|Item: `2" + item_name + "``|Select any Background Block|\nend_dialog|weatherspcl3|Cancel|Okay|");
				}
				else if (world->owner != "") {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 9150) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wThe Miner``|left|9150|\nadd_textbox|Welcome, welcome young miner! Seems like you made your way to me hopefuly it wasnt too hard. What would you like to do today?|left|\nadd_button|chc0|Mineral Items|noflags|0|0|\nadd_button|chc1|Sell Minerals|noflags|0|0|\nend_dialog|phonecall|Hang Up||");
				}
				else if (world->owner != "") {
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
				}
				return;
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
				if (isWorldOwner(peer, world) || admin(peer) || pData->rawName == world->items.at(x + (y * world->width)).monitorname) {
					pData->wrenchedBlockLocation = x + (y * world->width);
					if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
						string allow_dialog = "\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|0";
						if (world->items.at(x + (y * world->width)).opened) {
							allow_dialog = "\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|1";
						}
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit Small Lock``|left|202|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|Currently, you're the only one with access.``|left\nadd_spacer|small|\nadd_player_picker|playerNetID|`wAdd``|" + allow_dialog + "\nadd_checkbox|checkbox_ignore|Ignore empty air|0\nadd_button|recalcLock|`wRe-apply lock``|noflags|0|0|\nend_dialog|lock_edit|Cancel|OK|");
						return;
					}
					int ispub = world->isPublic;
					string str = "";
					try {
						for (auto i = 0; i < world->accessed.size(); i++) {
							if (world->accessed.at(i) == "") continue;
							ifstream read_player("save/players/_" + world->accessed.at(i) + ".json");
							if (!read_player.is_open()) {
								return;
							}
							json j;
							read_player >> j;
							read_player.close();
							string nickname = j["nick"];
							int adminLevel = j["adminLevel"];
							if (nickname == "") {
								nickname = role_prefix.at(adminLevel) + world->accessed.at(i);
							}
							str += "\nadd_checkbox|checkbox_" + nickname + "|" + nickname + "``|1";
						}
					}
					catch (std::exception& e) {
						std::cout << e.what() << std::endl;
						return;
					}
					string drop_gems = "0|";
					if (world->rating == 1) drop_gems = "1|";
					int muted = world->silence;
					string wlmenu = "";
					string category_change = "\nadd_button|changecat|`wCategory: " + world->category + "``|noflags|0|0|";
					string abandon_guild = "";
					string change_guild_fg = "";
					string change_guild_bg = "";
					string clear_guild_mascot = "";
					string world_key = "";
					auto iscontains = false;
					SearchInventoryItem(peer, 1424, 1, iscontains);
					if (!iscontains) {
						world_key = "\nadd_button|getKey|Get World Key|noflags|0|0|";
					}
					if (world->category == "Guild") {
						category_change = "";
						try {
							ifstream read_player("save/players/_" + world->owner + ".json");
							if (!read_player.is_open()) {
								break;
							}
							json j;
							read_player >> j;
							read_player.close();
							string guild_name = j["guild"];

							ifstream read_guild("save/guilds/_" + guild_name + ".json");
							if (!read_guild.is_open()) {
								break;
							}
							json js;
							read_guild >> js;
							read_guild.close();
							int guild_fg = js["foreground"];
							int guild_bg = js["background"];
							string current_guild_fg = "\nadd_textbox|`oCurrent guild foreground: `$None``|";
							if (guild_fg != 0) current_guild_fg = "\nadd_label_with_icon|small|`oCurrent guild foreground: `$" + getItemDef(guild_fg).name + "``|left|" + to_string(guild_fg) + "";
							string current_guild_bg = "\nadd_textbox|`oCurrent guild background: `$None``|";
							if (guild_bg != 0) current_guild_bg = "\nadd_label_with_icon|small|`oCurrent guild background: `$" + getItemDef(guild_bg).name + "``|left|" + to_string(guild_bg) + "";
							if (guild_fg != 0 || guild_bg != 0) {
								clear_guild_mascot = "\nadd_button|clear_mascot|Clear current guild mascot``|noflags|0|0|";
							}
							abandon_guild = "\nadd_button|abandon_guild|`4Abandon " + guild_name + " guild``|noflags|0|0|";
							change_guild_fg = "\nadd_spacer|small|" + current_guild_fg + "|\nadd_item_picker|guild_fg|Choose guild foreground|Choose guild foreground|";
							change_guild_bg = current_guild_bg + "|\nadd_item_picker|guild_bg|Choose guild background|Choose guild background|\nadd_spacer|small|";
						}
						catch (std::exception& e) {
							std::cout << e.what() << std::endl;
							break;
						}
					}
					if (world->items.at(x + (y * world->width)).foreground == 4802) {
						string rainbows = "0";
						if (world->rainbow) rainbows = "1";
						if (str == "") {
							wlmenu = "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|Currently, you're the only one with access.``|left\nadd_spacer|small|\nadd_player_picker|playerNetID|`wAdd``|\nadd_button|WorldDropPickup|`wEnable/Disable Collecting Items``|\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|" + to_string(ispub) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|0|\nadd_checkbox|checkbox_disable_music_render|Make Custom Music Blocks invisible|add_checkbox|checkbox_rainbow|Enable Rainbows!|" + rainbows + "\nadd_checkbox|checkbox_muted|Silence, Peasants!|" + to_string(muted) + "|noflags|0|0|\nadd_text_input|minimum_entry_level|World Level: |1|3|\nadd_smalltext|Set minimum world entry level.|" + change_guild_fg + change_guild_bg + clear_guild_mascot + abandon_guild + category_change + world_key + "\nend_dialog|lock_edit|Cancel|OK|";
						}
						else {
							wlmenu = "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|" + str + "|left\nadd_spacer|small|\nadd_player_picker|playerNetID|`wAdd``|\nadd_button|WorldDropPickup|`wEnable/Disable Collecting Items``|\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|" + to_string(ispub) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|0|\nadd_checkbox|checkbox_disable_music_render|Make Custom Music Blocks invisible|add_checkbox|checkbox_rainbow|Enable Rainbows!|" + rainbows + "\nadd_checkbox|checkbox_muted|Silence, Peasants!|" + to_string(muted) + "|noflags|0|0|\nadd_text_input|minimum_entry_level|World Level: |1|3|\nadd_smalltext|Set minimum world entry level.|" + change_guild_fg + change_guild_bg + clear_guild_mascot + abandon_guild + category_change + world_key + "\nend_dialog|lock_edit|Cancel|OK|";
						}
						Player::OnDialogRequest(peer, wlmenu);
					}
					else {
						if (str == "") {
							wlmenu = "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|Currently, you're the only one with access.``|left\nadd_spacer|small|\nadd_player_picker|playerNetID|`wAdd``|\nadd_button|WorldDropPickup|`wEnable/Disable Collecting Items``|\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|" + to_string(ispub) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|0|\nadd_checkbox|checkbox_disable_music_render|Make Custom Music Blocks invisible|noflags|0|0|\nadd_text_input|minimum_entry_level|World Level: |" + to_string(world->entrylevel) + "|3|\nadd_smalltext|Set minimum world entry level.|" + change_guild_fg + change_guild_bg + clear_guild_mascot + abandon_guild + category_change + world_key + "\nend_dialog|lock_edit|Cancel|OK|";
						}
						else {
							wlmenu = "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|" + str + "|left\nadd_spacer|small|\nadd_player_picker|playerNetID|`wAdd``|\nadd_button|WorldDropPickup|`wEnable/Disable Collecting Items``|\nadd_checkbox|checkbox_public|Allow anyone to Build and Break|" + to_string(ispub) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|0|\nadd_checkbox|checkbox_disable_music_render|Make Custom Music Blocks invisible|noflags|0|0|\nadd_text_input|minimum_entry_level|World Level: |" + to_string(world->entrylevel) + "|3|\nadd_smalltext|Set minimum world entry level.|" + change_guild_fg + change_guild_bg + clear_guild_mascot + abandon_guild + category_change + world_key + "\nend_dialog|lock_edit|Cancel|OK|";
						}
						Player::OnDialogRequest(peer, wlmenu);
					}
				}
				else {
					if (isWorldAdmin(peer, world)) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							try {
								ifstream read_player("save/players/_" + whoslock + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + whoslock;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`2Access Granted`w)", 0, true);
								Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
							return;
						}
					}
					else if (world->isPublic) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							try {
								ifstream read_player("save/players/_" + whoslock + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + whoslock;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
								Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
							return;
						}
					}
					else if (world->isEvent) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							if (pData->rawName != whoslock) {
								try {
									ifstream read_player("save/players/_" + whoslock + ".json");
									if (!read_player.is_open()) {
										return;
									}
									json j;
									read_player >> j;
									read_player.close();
									string nickname = j["nick"];
									int adminLevel = j["adminLevel"];
									if (nickname == "") {
										nickname = role_prefix.at(adminLevel) + whoslock;
									}
									if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
									else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
									Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									return;
								}
								return;
							}
						}
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 6952) {
				if (world->isPublic) return;
				if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner) || admin(peer)) {
					pData->lastPunchX = x;
					pData->lastPunchY = y;
					string PoolDialog = "\nadd_item_picker|autobreakitem|No block type selected!|Choose the target item!|";
					string StopDialog = "";
					if (world->items.at(x + (y * world->width)).mid != 0) {
						PoolDialog = "\nadd_item_picker|autobreakitem|Target block is: `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "``|Choose the target item!|";
						StopDialog = "\nadd_button|manipulatorstop|Stop the machine|";
					}
					string AutoHarvestDialog = "\nadd_checkbox|checkbox_autoharvest|`oAuto harvest trees|0|";
					if (world->items.at(x + (y * world->width)).rm) {
						AutoHarvestDialog = "\nadd_checkbox|checkbox_autoharvest|`oAuto harvest trees|1|";
					}
					Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_textbox|`oThis machine will break blocks for you! Select one and it will start breaking it immediately! Just make sure to select the right one|left||" + PoolDialog + "|" + StopDialog + "|" + AutoHarvestDialog + "\nend_dialog|autobreak|Close|OK|");
				}
				return;
			}

			if (world->items.at(x + (y * world->width)).foreground == 6954) {
				if (world->isPublic) return;
				if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner) || admin(peer)) {
					pData->lastPunchX = x;
					pData->lastPunchY = y;
					string PoolDialog = "\nadd_item_picker|autoplaceitem|No auto place block selected!|Choose the target block!|";
					string addthemdialog = "";
					string stopdialog = "";
					if (world->items.at(x + (y * world->width)).mid != 0) {
						PoolDialog = "\nadd_item_picker|autoplaceitem|Target auto place block is: `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "`` (`2" + to_string(world->items.at(x + (y * world->width)).mc) + "/5000``)|Choose the target block!|";
						auto mtitems = 0;
						for (auto i = 0; i < pData->inventory.items.size(); i++) {
							if (pData->inventory.items.at(i).itemID == world->items.at(x + (y * world->width)).mid) {
								mtitems = pData->inventory.items.at(i).itemCount;
								break;
							}
						}
						if (mtitems != 0 && world->items.at(x + (y * world->width)).mc + mtitems <= 5000) {
							addthemdialog = "\nadd_smalltext|`oYou have " + to_string(mtitems) + " " + getItemDef(world->items.at(x + (y * world->width)).mid).name + " in your backpack.``|left|\nadd_button|addorganic|Add them to the machine|";
						}
						if (world->items.at(x + (y * world->width)).mc > 0) stopdialog = "\nadd_button|organicstop|Retrieve Items|";
					}
					string PoolDialogs = "\nadd_item_picker|autoplaceidentityitem|No ident block selected!|Choose the identification block!|";
					if (world->items.at(x + (y * world->width)).vid != 0) {
						PoolDialogs = "\nadd_item_picker|autoplaceidentityitem|Target ident block is: `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "``|Choose the identification block!|";
					}
					string MagplantsStoragesList = "";
					string UseMagDialog = "";
					if (world->items.at(x + (y * world->width)).mid != 0) {
						for (auto i = 0; i < world->width * world->height; i++) {
							if (world->items.at(i).foreground == 5638 && world->items.at(i).mid == world->items.at(x + (y * world->width)).mid) {
								MagplantsStoragesList += "\nadd_label_with_icon|small|`2" + getItemDef(world->items.at(i).mid).name + " `oMagplant (`2" + to_string(world->items.at(i).mc) + "`o)|left|" + to_string(world->items.at(i).mid) + "|";
							}
						}
						if (MagplantsStoragesList != "") {
							UseMagDialog = "\nadd_checkbox|checkbox_publicremote|`oUse magplants storage with the " + getItemDef(world->items.at(x + (y * world->width)).vid).name + "|0|";
							if (world->items.at(x + (y * world->width)).rm) {
								UseMagDialog = "\nadd_checkbox|checkbox_publicremote|`oUse magplants storage with the " + getItemDef(world->items.at(x + (y * world->width)).vid).name + "|1|";
							}
						}
					}
					Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_textbox|`oSelect the block you want this machine to automatically place for you, also you will need to select the second foreground block which will be used as identification for the first one, let's say we will select that we want to place Dirt automatically, and in the identification block we will set Wooden Platform, so that will cause our Dirt to placed above Wooden Platforms in your world!|left||" + PoolDialog + "|" + addthemdialog + "|" + PoolDialogs + "|" + MagplantsStoragesList + "|" + UseMagDialog + "|" + stopdialog + "\nend_dialog|autoplace|Close|OK|");
				}
				return;
			}

			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::SUCKER) {
				if (world->isPublic) return;
				auto squaresign = x + (y * world->width);
				auto currentworld = pData->currentWorld + "X" + std::to_string(squaresign);
				string Sucker = "";
				if (world->items.at(x + (y * world->width)).foreground == 5638) {
					Sucker = "magplant";
					pData->suckername = "magplant";
					pData->suckerid = 5638;
				}
				if (world->items.at(x + (y * world->width)).foreground == 6946) {
					Sucker = "gaiabeacon";
					pData->suckername = "gaiabeacon";
					pData->suckerid = 6946;
				}
				if (world->items.at(x + (y * world->width)).foreground == 6948) {
					Sucker = "unstabletesseract";
					pData->suckername = "unstabletesseract";
					pData->suckerid = 6948;
				}
				pData->lastPunchX = x;
				pData->lastPunchY = y;
				if (pData->rawName != world->owner && !admin(peer)) {
					if (world->items.at(x + (y * world->width)).rm) {
						string label = "`6The machine is currently empty!";
						string RemoteDialog = "";
						if (world->items.at(x + (y * world->width)).mc >= 1) {
							RemoteDialog = "\nadd_textbox|`oUse the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + " Remote to build `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `odirectly from the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "'s storage.|\nadd_button|getremote|`oGet Remote|";
							label = "`oThe machine contains " + to_string(world->items.at(x + (y * world->width)).mc) + " `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + ""; /*The message if something exists in item sucker*/
						}
						Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_spacer|small|\nadd_label_with_icon|small|`5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "|left|" + to_string(world->items.at(x + (y * world->width)).mid) + "|\nadd_textbox|" + label + "|" + RemoteDialog + "|\nend_dialog|magplantupdate|Close||");
					}
					else {
						Player::OnTalkBubble(peer, pData->netID, "This magplant public mode is disabled!", 0, false);
					}
					return;
				}
				if (world->items.at(x + (y * world->width)).mid != 0) {
					string label = "`6The machine is currently empty!";
					string RemoteDialog = "";
					string publicremotecheck = "";
					string gemDialog = "";
					bool Farmable = false;
					if (world->items.at(x + (y * world->width)).mid == 7382 || world->items.at(x + (y * world->width)).mid == 4762 || world->items.at(x + (y * world->width)).mid == 5140 || world->items.at(x + (y * world->width)).mid == 5138 || world->items.at(x + (y * world->width)).mid == 5136 || world->items.at(x + (y * world->width)).mid == 5154 || world->items.at(x + (y * world->width)).mid == 340 || world->items.at(x + (y * world->width)).mid == 954 || world->items.at(x + (y * world->width)).mid == 5666) Farmable = true;
					if (Sucker == "magplant" && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::FOREGROUND || Sucker == "magplant" && Farmable || Sucker == "magplant" && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::BACKGROUND || Sucker == "magplant" && getItemDef(world->items.at(x + (y * world->width)).mid).blockType == BlockTypes::GROUND_BLOCK) {
						if (getItemDef(world->items.at(x + (y * world->width)).mid).properties & Property_AutoPickup) {
							/*...*/
						}
						else {
							if (world->items.at(x + (y * world->width)).rm) {
								publicremotecheck = "\nadd_checkbox|checkbox_publicremote|`oAllow visitors to take remote|1|";
							}
							else {
								publicremotecheck = "\nadd_checkbox|checkbox_publicremote|`oAllow visitors to take remote|0|";
							}
							RemoteDialog = "\nadd_textbox|`oUse the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + " Remote to build `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `odirectly from the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "'s storage.|\nadd_button|getremote|`oGet Remote|";
						}
					}
					if (Sucker == "magplant") {
						if (world->items.at(x + (y * world->width)).mid == 112) {
							gemDialog = "\nadd_checkbox|checkbox_gemcollection|`oEnable gem collection|1|";
						}
						else {
							gemDialog = "\nadd_checkbox|checkbox_gemcollection|`oEnable gem collection|0|";
						}
					}
					if (world->items.at(x + (y * world->width)).mc >= 1) {
						label = "`oThe machine contains " + to_string(world->items.at(x + (y * world->width)).mc) + " `2" + getItemDef(world->items.at(x + (y * world->width)).mid).name + ""; /*The message if something exists in item sucker*/
						Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_spacer|small|\nadd_label_with_icon|small|`5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "|left|" + to_string(world->items.at(x + (y * world->width)).mid) + "|\nadd_textbox|" + label + "|\nadd_button|retrieveitem|`oRetrieve Items|" + RemoteDialog + "|" + publicremotecheck + "|\nend_dialog|magplantupdate|Close|`wUpdate|");
					}
					else {
						Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_spacer|small|\nadd_label_with_icon|small|`5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "|left|" + to_string(world->items.at(x + (y * world->width)).mid) + "|\nadd_textbox|" + label + "|\nadd_item_picker|magplantitem|Change Item|Choose an item to put in the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "!|" + publicremotecheck + "|" + gemDialog + "\nend_dialog|magplantupdate|Close|`wUpdate|");
					}
				}
				else {
					if (Sucker == "magplant") Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_spacer|small|\nadd_textbox|`6The machine is empty.|\nadd_item_picker|magplantitem|Choose Item|Choose an item to put in the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "!|\nadd_checkbox|checkbox_gemcollection|`oEnable gem collection|0|\nend_dialog|magplantcheck|Close|`wUpdate|");
					else Player::OnDialogRequest(peer, "set_default_color|\nadd_label_with_icon|big|`w" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_spacer|small|\nadd_textbox|`6The machine is empty.|\nadd_item_picker|magplantitem|Choose Item|Choose an item to put in the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "!|\nend_dialog|magplant|Close||");
				}
				return;
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::PORTAL) {
				if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y)) {
					pData->wrenchsession = x + (y * world->width);
					string DestWorldDialog = world->items.at(x + (y * world->width)).destWorld;
					if (world->items.at(x + (y * world->width)).destId != "") {
						DestWorldDialog += ":" + world->items.at(x + (y * world->width)).destId;
					}
					string IdDialog = "\nadd_text_input|door_id|ID|" + world->items.at(x + (y * world->width)).currId + "|11|\nadd_smalltext|Set a unique `2ID`` to target this door as a Destination from another!|left|";
					if (world->items.at(x + (y * world->width)).foreground == 762) {
						IdDialog = "\nadd_text_input|door_id|Password|" + world->items.at(x + (y * world->width)).password + "|23|";
					}
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_text_input|door_name|Label|" + world->items.at(x + (y * world->width)).label + "|100|\nadd_text_input|door_target|Destination|" + DestWorldDialog + "|24|\nadd_smalltext|Enter a Destination in this format: `2WORLDNAME:ID``|left|\nadd_smalltext|Leave `2WORLDNAME`` blank (:ID) to go to the door with `2ID`` in the `2Current World``.|left|" + IdDialog + "\nend_dialog|door_edit|Cancel|OK|");
				}
				return;
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::SIGN || world->items.at(x + (y * world->width)).foreground == 6214 || world->items.at(x + (y * world->width)).foreground == 1420) {
				if (world->owner == "" || isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || !restricted_area(peer, world, x, y)) {
					auto& signtext = world->items.at(x + (y * world->width)).sign;
					pData->wrenchedBlockLocation = x + (y * world->width);
					if (world->items.at(x + (y * world->width)).foreground == 6214 || world->items.at(x + (y * world->width)).foreground == 1420) {
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_textbox|To dress, select a clothing item then use on the mannequin. To remove clothes, punch it or select which item to remove.<CR><CR>It will go into your backpack if you have room.|\nadd_textbox|<CR><CR>What would you like to write on its sign?``|left|\nadd_text_input|sign_textas||" + signtext + "|128|\nend_dialog|mannequin_edit|Cancel|OK|");
						return;
					}
					Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(world->items.at(x + (y * world->width)).foreground) + "|\nadd_textbox|What would you like to write on this sign?``|left|\nadd_text_input|signtextas||" + signtext + "|128|\nend_dialog|editsign|Cancel|OK|");
				}
				return;
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::GATEWAY) {
				if (isWorldOwner(peer, world) || world->owner == "" || isWorldAdmin(peer, world) || admin(peer) || !restricted_area(peer, world, x, y)) {
					if (world->owner == "") {
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_textbox|This object has additional properties to edit if in a locked area.|left|\nend_dialog|gateway_edit|Cancel|OK|");
						return;
					}
					pData->wrenchx = x;
					pData->wrenchy = y;
					if (!world->items.at(x + (y * world->width)).opened) {
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_checkbox|checkbox_public|Is open to public|0\nend_dialog|gateway_edit|Cancel|OK|");
					}
					else {
						Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "``|left|" + to_string(getItemDef(world->items.at(x + (y * world->width)).foreground).id) + "|\nadd_checkbox|checkbox_public|Is open to public|1\nend_dialog|gateway_edit|Cancel|OK|");
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 2978)
			{
				if (x == pData->x / 32 && y == pData->y / 32)
				{
					pData->lastPunchX = x;
					pData->lastPunchY = y;
					if (world->owner != "" && !isWorldOwner(peer, world) && !admin(peer))
					{
						SendBuyerVendDialog(peer, world);
						return;
					}
					SendVendDialog(peer, world);
				}
				else {
					Player::OnTalkBubble(peer, pData->netID, "Get closer!", 0, false);
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 6016) {
				pData->lastPunchX = x;
				pData->lastPunchY = y;
				SendGScan(peer, world, x, y);
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 3898)
			{
				if (isWorldOwner(peer, world) || world->owner == "" || admin(peer) || isWorldAdmin(peer, world)) {
					Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wTelephone`|left|3898|\n\nadd_spacer|small|\nadd_label|small|`oDial a number to call somebody in Growtopia. Phone numbers have 5 digits. Most numbers are not in service!|\nadd_spacer|small|\nadd_text_input|telephonenumber|Phone #||5|\nend_dialog|usetelephone|Hang Up|`wDial|\n");
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 658)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the bulletin board.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wBulletin Board", 658, LABEL_BIG);
						myLetterBox.addSpacer(SPACER_SMALL);
						try
						{
							ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
							json j;
							ifff >> j;
							ifff.close();
							if (j["inmail"] <= 0)
							{
								myLetterBox.addTextBox("`oThe Bulletin Board is empty.");
								myLetterBox.addSpacer(SPACER_SMALL);
							}
							else
							{
								for (int i = 0; i < 20; i++)
								{
									if (j["mails"].at(i)["growid"] != "")
									{
										if (j["hidenames"] == 0)
										{
											int apos = j["mails"].at(i)["aposition"];
											myLetterBox.addLabelWithIconButton("`w" + j["mails"].at(i)["growid"].get<string>() + ":`2 " + j["mails"].at(i)["text"].get<string>() + "", 660, "removeselectedbulletin_" + to_string(squaresign) + "_" + to_string(apos));
											//myLetterBox.addSpacer(SPACER_SMALL);
										}
										else
										{
											myLetterBox.addTextBox("`2" + j["mails"].at(i)["text"].get<string>());
											//myLetterBox.addSpacer(SPACER_SMALL);
										}
									}
								}
							}

							if (j["inmail"] < 90)
							{
								myLetterBox.addTextBox("`oAdd to conversation?");
								myLetterBox.addInputBox("addbulletinletterinput", "", "", 50);
								myLetterBox.addSpacer(SPACER_SMALL);
								myLetterBox.addButton("addbulletinletter_" + to_string(squaresign), "`2Add");
								myLetterBox.addSpacer(SPACER_SMALL);
							}

							myLetterBox.addLabelWithIcon("`wOwner Options", 242, LABEL_BIG);
							myLetterBox.addSpacer(SPACER_SMALL);
							if (j["hidenames"] == 1)
							{
								myLetterBox.addTextBox("`oUncheck `5Hide names `oto enable individual comment removal options.");
								myLetterBox.addSpacer(SPACER_SMALL);
							}
							else
							{
								myLetterBox.addTextBox("`oTo remove an individual comment, press the icon to the left of it.");
								myLetterBox.addSpacer(SPACER_SMALL);
							}
							if (j["inmail"] > 0)
							{
								myLetterBox.addButton("bulletinboardclear_" + to_string(squaresign), "`4Clear Board");
								myLetterBox.addSpacer(SPACER_SMALL);
							}
							if (j["publiccanadd"] == 1)
								myLetterBox.addCheckbox("publiccanaddbulletinboard", "`oPublic can add", CHECKBOX_SELECTED);
							else
								myLetterBox.addCheckbox("publiccanaddbulletinboard", "`oPublic can add", CHECKBOX_NOT_SELECTED);

							if (j["hidenames"] == 1)
								myLetterBox.addCheckbox("hidenamesbulletinboard", "`oHide names", CHECKBOX_SELECTED);
							else
								myLetterBox.addCheckbox("hidenamesbulletinboard", "`oHide names", CHECKBOX_NOT_SELECTED);
							myLetterBox.addSpacer(SPACER_SMALL);
							myLetterBox.addButton("bulletinletterok_" + to_string(squaresign), "`wOK");
							myLetterBox.addQuickExit();
							myLetterBox.endDialog("Close", "", "Cancel");
							Player::OnDialogRequest(peer, myLetterBox.finishDialog());
						}
						catch (std::exception&)
						{
							cout << "bulletin tile failed" << endl;
							return; /*tipo jeigu nera*/
						}
					}
					else
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wBulletin Board", 658, LABEL_BIG);
						myLetterBox.addSpacer(SPACER_SMALL);
						try
						{
							ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
							json j;
							ifff >> j;
							ifff.close();

							if (j["inmail"] > 0)
							{
								for (int i = 0; i < 20; i++)
								{
									if (j["mails"].at(i)["growid"] != "")
									{
										if (j["hidenames"] == 0)
										{
											myLetterBox.addLabelWithIcon("`w" + j["mails"].at(i)["growid"].get<string>() + ":`2 " + j["mails"].at(i)["text"].get<string>() + "", 660, LABEL_SMALL);
											myLetterBox.addSpacer(SPACER_SMALL);
										}
										else
										{
											myLetterBox.addTextBox("`2" + j["mails"].at(i)["text"].get<string>());
											myLetterBox.addSpacer(SPACER_SMALL);
										}
									}
								}
							}

							if (j["publiccanadd"] == 1 && j["inmail"] < 90)
							{
								myLetterBox.addSpacer(SPACER_SMALL);
								myLetterBox.addTextBox("`oAdd to conversation?");
								myLetterBox.addInputBox("addbulletinletterinput", "", "", 50);
								myLetterBox.addSpacer(SPACER_SMALL);
								myLetterBox.addButton("addbulletinletter_" + to_string(squaresign), "`2Add");
								myLetterBox.addSpacer(SPACER_SMALL);
							}

							myLetterBox.addQuickExit();
							myLetterBox.endDialog("Close", "", "Cancel");
							Player::OnDialogRequest(peer, myLetterBox.finishDialog());
						}
						catch (std::exception&)
						{
							cout << "bulletin tile failed" << endl;
							return; /*tipo jeigu nera*/
						}
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 656)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the mailbox.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wMail Box", 656, LABEL_BIG);
						ifstream ifff("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();
						if (j["inmail"] <= 0)
						{
							myLetterBox.addTextBox("`oThe mailbox is currently empty.");
						}
						else
						{
							int donated = j["inmail"];

							myLetterBox.addTextBox("`oYou have `w" + to_string(donated) + " `oletters:");
							myLetterBox.addSpacer(SPACER_SMALL);
							for (int i = 0; i < 20; i++)
							{
								if (j["mails"].at(i)["growid"] != "")
								{
									myLetterBox.addLabelWithIcon("`5\"" + j["mails"].at(i)["text"].get<string>() + "\" - `w" + j["mails"].at(i)["growid"].get<string>() + "", 660, LABEL_SMALL);
									myLetterBox.addSpacer(SPACER_SMALL);
								}
							}
							myLetterBox.addSpacer(SPACER_SMALL);
							myLetterBox.addButton("mailempty_" + to_string(squaresign), "`4Empty mailbox");
						}
						myLetterBox.addTextBox("`oWrite a letter to yourself?");
						myLetterBox.addInputBox("addletterinput_" + to_string(squaresign), "", "", 50);
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addButton("addletter", "`2Send Letter");
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addQuickExit();
						myLetterBox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myLetterBox.finishDialog());
					}
					else
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wMail Box", 656, LABEL_BIG);
						ifstream ifff("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						myLetterBox.addTextBox("`$Want to leave a message for the owner?");
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addInputBox("addletterinput_" + to_string(squaresign), "", "", 50);
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addButton("addletter", "`2Send Letter");

						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addQuickExit();
						myLetterBox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myLetterBox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 6286)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the box.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myBox;
						myBox.addLabelWithIcon("`wStorage Box Xtreme - Level 1", 6286, LABEL_BIG);
						ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						int stored = j["instorage"];

						if (stored > 0)
						{
							myBox.addSpacer(SPACER_SMALL);
						}

						int count = 0;
						int id = 0;
						int aposition = 0;
						for (int i = 0; i < 20; i++)
						{
							if (j["storage"].at(i)["itemid"] != 0)
							{
								count = j["storage"].at(i)["itemcount"];
								id = j["storage"].at(i)["itemid"];
								aposition = j["storage"].at(i)["aposition"];

								if (i % 6 == 0 && i != 0)
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl1DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), true);
								}
								else
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl1DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), false);
								}
							}
						}

						if (stored > 0)
						{
							myBox.addNewLineAfterFrame();
						}

						myBox.addTextBox("`w" + to_string(stored) + "/20 `$items stored.");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addPicker("boxlvl1deposit_" + to_string(squaresign), "Deposit item", "Select an item");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addQuickExit();
						myBox.endDialog("Close", "", "Exit");
						Player::OnDialogRequest(peer, myBox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 6288)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the box.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myBox;
						myBox.addLabelWithIcon("`wStorage Box Xtreme - Level 2", 6288, LABEL_BIG);
						ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						int stored = j["instorage"];

						if (stored > 0)
						{
							myBox.addSpacer(SPACER_SMALL);
						}

						int count = 0;
						int id = 0;
						int aposition = 0;
						for (int i = 0; i < 40; i++)
						{
							if (j["storage"].at(i)["itemid"] != 0)
							{
								count = j["storage"].at(i)["itemcount"];
								id = j["storage"].at(i)["itemid"];
								aposition = j["storage"].at(i)["aposition"];

								if (i % 6 == 0 && i != 0)
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl2DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), true);
								}
								else
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl2DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), false);
								}
							}
						}

						if (stored > 0)
						{
							myBox.addNewLineAfterFrame();
						}

						myBox.addTextBox("`w" + to_string(stored) + "/40 `$items stored.");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addPicker("boxlvl2deposit_" + to_string(squaresign), "Deposit item", "Select an item");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addQuickExit();
						myBox.endDialog("Close", "", "Exit");
						Player::OnDialogRequest(peer, myBox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 6290)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the box.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myBox;
						myBox.addLabelWithIcon("`wStorage Box Xtreme - Level 3", 6290, LABEL_BIG);
						ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						int stored = j["instorage"];

						if (stored > 0)
						{
							myBox.addSpacer(SPACER_SMALL);
						}

						int count = 0;
						int id = 0;
						int aposition = 0;
						for (int i = 0; i < 90; i++)
						{
							if (j["storage"].at(i)["itemid"] != 0)
							{
								count = j["storage"].at(i)["itemcount"];
								id = j["storage"].at(i)["itemid"];
								aposition = j["storage"].at(i)["aposition"];

								if (i % 6 == 0 && i != 0)
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl3DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), true);
								}
								else
								{
									myBox.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "boxlvl3DepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), false);
								}
							}
						}

						if (stored > 0)
						{
							myBox.addNewLineAfterFrame();
						}

						myBox.addTextBox("`w" + to_string(stored) + "/90 `$items stored.");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addPicker("boxlvl3deposit_" + to_string(squaresign), "Deposit item", "Select an item");
						myBox.addSpacer(SPACER_SMALL);
						myBox.addQuickExit();
						myBox.endDialog("Close", "", "Exit");
						Player::OnDialogRequest(peer, myBox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 8878)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						int stored = j["insafe"];
						string password = j["password"];

						if (password != "")
						{
							GTDialog mySafeConfirm;
							mySafeConfirm.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);
							mySafeConfirm.addTextBox("Please enter your password to access the Save Vault.");
							mySafeConfirm.addInputBox("safeconfirmpassInput_" + to_string(squaresign), "", "", 18);
							mySafeConfirm.addButton("safe_confirmpass", "Enter Password");
							mySafeConfirm.addButton("saferecoverPasswordInConfirm_" + to_string(squaresign), "Recover Password");
							mySafeConfirm.addSpacer(SPACER_SMALL);
							mySafeConfirm.addQuickExit();
							mySafeConfirm.endDialog("Close", "", "Exit");
							Player::OnDialogRequest(peer, mySafeConfirm.finishDialog());
							return;
						}

						GTDialog mySafe;
						mySafe.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);

						if (stored > 0)
						{
							mySafe.addSpacer(SPACER_SMALL);
						}

						int count = 0;
						int id = 0;
						int aposition = 0;
						for (int i = 0; i < 20; i++)
						{
							if (j["safe"].at(i)["itemid"] != 0)
							{
								count = j["safe"].at(i)["itemcount"];
								id = j["safe"].at(i)["itemid"];
								aposition = j["safe"].at(i)["aposition"];

								if (i % 3 == 0 && i != 0)
								{
									mySafe.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "safeBoxDepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), true);
								}
								else
								{
									mySafe.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "safeBoxDepositedItem_" + to_string(aposition) + "_" + to_string(squaresign), false);
								}
							}
						}

						if (stored > 0)
						{
							mySafe.addNewLineAfterFrame();
						}

						mySafe.addTextBox("`w" + to_string(stored) + "/20 `$items stored.");
						mySafe.addSpacer(SPACER_SMALL);
						mySafe.addPicker("safedeposit_" + to_string(squaresign), "Deposit item", "Select an item");
						if (j["password"] == "")
						{
							mySafe.addTextBox("`$This Safe Vault is not `4password protected`$!");
						}
						else
						{
							mySafe.addTextBox("`$This Safe Vault is `2password protected`$!");
						}
						mySafe.addSpacer(SPACER_SMALL);
						mySafe.addTextBox("`$Change your password.");
						mySafe.addButton("safeupdatepass_" + to_string(squaresign), "Update Password");

						mySafe.addSpacer(SPACER_SMALL);
						mySafe.addQuickExit();
						mySafe.endDialog("Close", "", "Exit");
						Player::OnDialogRequest(peer, mySafe.finishDialog());
					}
				}
				return;
			}
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DONATION)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the donation box.", 0, true);
				}
				else
				{
					pData->lastPunchX = x;
					pData->lastPunchY = y;
					pData->lastPunchForeground = world->items.at(x + (y * world->width)).foreground;
					pData->lastPunchBackground = world->items.at(x + (y * world->width)).background;
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myDbox;
						myDbox.addLabelWithIcon("`wDonation Box", world->items.at(x + (y * world->width)).foreground, LABEL_BIG);
						ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();
						if (j["donated"] <= 0)
						{
							myDbox.addTextBox("`$The box is currently empty.");
						}
						else
						{
							int donated = j["donated"];
							int count = 0;
							myDbox.addTextBox("`oYou have `w" + to_string(donated) + " `ogifts waiting:");
							myDbox.addSpacer(SPACER_SMALL);
							for (int i = 0; i < 20; i++)
							{
								if (j["donatedItems"].at(i)["itemid"] != 0)
								{
									count = j["donatedItems"].at(i)["itemcount"];
									myDbox.addLabelWithIcon("`o" + getItemDef(j["donatedItems"].at(i)["itemid"]).name + " (`w" + to_string(count) + "`o) from `w" + j["donatedItems"].at(i)["sentBy"].get<string>() + "`#- '" + j["donatedItems"].at(i)["note"].get<string>() + "'", j["donatedItems"].at(i)["itemid"], LABEL_SMALL);
									myDbox.addSpacer(SPACER_SMALL);
								}
							}
							myDbox.addSpacer(SPACER_SMALL);
							myDbox.addButton("retrieveGifts_" + to_string(squaresign), "`4Retrieve Gifts");
						}
						myDbox.addSpacer(SPACER_SMALL);
						myDbox.addPicker("addDonationItem_" + to_string(squaresign), "`wGive Gift `o(Min rarity: `52`o)", "Select an item");
						myDbox.addSpacer(SPACER_SMALL);
						myDbox.addQuickExit();
						myDbox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myDbox.finishDialog());
					}
					else
					{
						GTDialog myDbox;
						myDbox.addLabelWithIcon("`wDonation Box", world->items.at(x + (y * world->width)).foreground, LABEL_BIG);
						ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						int donated = j["donated"];

						myDbox.addTextBox("`$You see `w" + to_string(donated) + "`$ gifts in the box!");
						myDbox.addTextBox("`$Want to leave a gift for the owner?");
						myDbox.addSpacer(SPACER_SMALL);

						myDbox.addPicker("addDonationItem_" + to_string(squaresign), "`wGive Gift `o(Min rarity: `52`o)", "Select an item");

						myDbox.addSpacer(SPACER_SMALL);
						myDbox.addQuickExit();
						myDbox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myDbox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 1006)
			{
				auto squaresign = x + (y * world->width);
				auto isdbox = std::experimental::filesystem::exists("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				if (!isdbox)
				{
					Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the mailbox.", 0, true);
				}
				else
				{
					if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || admin(peer))
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wBlue Mail Box", 1006, LABEL_BIG);
						ifstream ifff("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();
						if (j["inmail"] <= 0)
						{
							myLetterBox.addTextBox("`oThe mailbox is currently empty.");
						}
						else
						{
							int donated = j["inmail"];

							myLetterBox.addTextBox("`oYou have `w" + to_string(donated) + " `oletters:");
							myLetterBox.addSpacer(SPACER_SMALL);
							for (int i = 0; i < 20; i++)
							{
								if (j["mails"].at(i)["growid"] != "")
								{
									myLetterBox.addLabelWithIcon("`5\"" + j["mails"].at(i)["text"].get<string>() + "\" - `w" + j["mails"].at(i)["growid"].get<string>() + "", 660, LABEL_SMALL);
									myLetterBox.addSpacer(SPACER_SMALL);
								}
							}
							myLetterBox.addSpacer(SPACER_SMALL);
							myLetterBox.addButton("bluemailempty_" + to_string(squaresign), "`4Empty mailbox");
						}
						myLetterBox.addTextBox("`oWrite a letter to yourself?");
						myLetterBox.addInputBox("addblueletterinput_" + to_string(squaresign), "", "", 50);
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addButton("addblueletter", "`2Send Letter");
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addQuickExit();
						myLetterBox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myLetterBox.finishDialog());
					}
					else
					{
						GTDialog myLetterBox;
						myLetterBox.addLabelWithIcon("`wMail Box", 1006, LABEL_BIG);
						ifstream ifff("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						json j;
						ifff >> j;
						ifff.close();

						myLetterBox.addTextBox("`$Want to leave a message for the owner?");
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addInputBox("addblueletterinput_" + to_string(squaresign), "", "", 50);
						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addButton("addblueletter", "`2Send Letter");

						myLetterBox.addSpacer(SPACER_SMALL);
						myLetterBox.addQuickExit();
						myLetterBox.endDialog("Close", "", "Cancel");
						Player::OnDialogRequest(peer, myLetterBox.finishDialog());
					}
				}
				return;
			}
			if (world->items.at(x + (y * world->width)).foreground == 2946)
			{
				if (pData->rawName == world->owner || world->owner == "" || world->isPublic || admin(peer))
				{
					int itemid = world->items.at(x + (y * world->width)).foreground;
					int itembg = world->items.at(x + (y * world->width)).background;
					pData->displayfg = itemid;
					pData->displaybg = itembg;
					pData->displaypunchx = data.punchX;
					pData->displaypunchy = data.punchY;
					if (world->items.at(x + (y * world->width)).intdata != 0 && pData->rawName == world->owner)
					{
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDisplay Block|left|" + to_string(itemid) + "|\nadd_spacer|small||\nadd_label|small|`oA " + getItemDef(world->items.at(x + (y * world->width)).intdata).name + " is on display here.|\nadd_button|pickupdisplayitem|Pick it up|0|0|\nadd_quick_exit|\n");
					}
					else if (world->items.at(x + (y * world->width)).intdata != 0 && (admin(peer)))
					{
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDisplay Block|left|" + to_string(itemid) + "|\nadd_spacer|small||\nadd_label|small|`oA " + getItemDef(world->items.at(x + (y * world->width)).intdata).name + " is on display here.|\nadd_button|chc000|Okay|0|0|\nadd_quick_exit|\n");
					}
					else if (world->isPublic && world->items.at(x + (y * world->width)).intdata != 0 && pData->rawName != world->owner)
					{
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDisplay Block|left|" + to_string(itemid) + "|\nadd_spacer|small||\nadd_label|small|`oA " + getItemDef(world->items.at(x + (y * world->width)).intdata).name + " is on display here.|\nadd_button|chc000|Okay|0|0|\nadd_quick_exit|\n");
					}
					else if (world->owner == "" && world->items.at(x + (y * world->width)).intdata != 0)
					{
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDisplay Block|left|" + to_string(itemid) + "|\nadd_spacer|small||\nadd_label|small|`oA " + getItemDef(world->items.at(x + (y * world->width)).intdata).name + " is on display here.|\nadd_button|pickupdisplayitem|Pick it up|0|0|\nadd_quick_exit|\n");
					}
					else
					{
						Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDisplay Block|left|" + to_string(itemid) + "|\nadd_spacer|small||\nadd_label|small|`oThe Display Block is empty. Use an item on it to display the item!|\nend_dialog||Close||\n");
					}
					return;
				}
				else
				{
					Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					return;
				}
				return;
			}
			return;
		}
		case 6336:
		{
			SendGrowpedia(peer);
			return;
		}
		case 764:
		{
			if (pData->isZombie == true) return;
			if (pData->canWalkInBlocks == true)
			{
				pData->canWalkInBlocks = false;
				pData->skinColor = 0x8295C3FF;
				send_state(peer);
			}
			sendSound(peer, "skel.wav");
			pData->isZombie = true;
			send_state(peer);
			RemoveInventoryItem(764, 1, peer, true);
			playerconfig(peer, 1150, 130, 0x14);
			return;
		}
		case 782:
		{
			if (pData->isZombie == false) return;
			pData->isZombie = false;
			send_state(peer);
			RemoveInventoryItem(782, 1, peer, true);
			playerconfig(peer, 1150, 300, 0x14);
			return;
		}
		case 3694:
		{
			world->items.at(x + (y * world->width)).vid = 255; //r rgb value
			world->items.at(x + (y * world->width)).vprice = 128; //g rgb value
			world->items.at(x + (y * world->width)).vcount = 64; //b rgb value
			break;
		}
		case 3832:
		{
			world->items.at(x + (y * world->width)).intdata = 2;
			world->items.at(x + (y * world->width)).mc = 50;
			world->items.at(x + (y * world->width)).rm = false;
			world->items.at(x + (y * world->width)).opened = false;
			world->items.at(x + (y * world->width)).activated = false;
			break;
		}
		case 5000:
		{
			world->items.at(x + (y * world->width)).intdata = 14;
			world->items.at(x + (y * world->width)).activated = false;
			break;
		}
		case 6286: case 6288: case 6290: case 6214: case 1420: case 656: case 658: case 1006: case 8878: case 1240: case 762: case 6016:
		{
			if (world->owner == "" || isWorldOwner(peer, world) || admin(peer) || isWorldAdmin(peer, world))
			{
				// Jammers
				if (tile == 226 && tile == 1276 && tile == 1278 && tile == 4884 && tile == 4992)
					// Speical Propertie items
					if (tile == 5638 && tile == 6016 && tile == 6946 && tile == 2978)
						// Weather Machines
						if (tile == 932 && tile == 934 && tile == 946 && tile == 984 && tile == 1210 && tile == 1364 && tile == 1490 && tile == 1750 && tile == 2046 && tile == 2284 && tile == 2744 && tile == 3252 && tile == 3446 && tile == 3534 && tile == 3694 && tile == 3832 && tile == 4242 && tile == 4486 && tile == 4776 && tile == 4892 && tile == 5000 && tile == 5112 && tile == 5654 && tile == 5716 && tile == 6854 && tile == 7644 && tile == 8836 && tile == 8896 && tile == 10286)
						{
							if (world->owner == "")
							{
								Player::OnTalkBubble(peer, pData->netID, "Lock World Before Placing This!", 0, true);
								return;
							}
						}
				if (tile == 762) {
					world->items.at(x + (y * world->width)).label = getItemDef(tile).name;
					isDoor = true;
				}
				if (tile == 6286 || tile == 6288 || tile == 6290)
				{
					auto Space = 20;
					string Directory = "save/storageboxlvl1";
					if (tile == 6288)
					{
						Space = 40;
						Directory = "save/storageboxlvl2";
					}
					else if (tile == 6290)
					{
						Space = 90;
						Directory = "save/storageboxlvl3";
					}
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory(Directory + "/" + world->name) || !fs::exists(Directory + "/" + world->name))
					{
						fs::create_directory(Directory + "/" + world->name);
					}
					ofstream of(Directory + "/" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					json j;
					j["instorage"] = 0;
					auto jArray = json::array();
					json jmid;
					for (auto i = 1; i <= Space; i++)
					{
						jmid["aposition"] = i;
						jmid["itemid"] = 0;
						jmid["placedby"] = pData->rawName;
						jmid["itemcount"] = 0;
						jArray.push_back(jmid);
					}
					j["storage"] = jArray;
					of << j << std::endl;
					of.close();
				}
				if (tile == 1240) {
					isHeartMonitor = true;
				}
				if (tile == 8878)
				{
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory("save/safevault/_" + world->name) || !fs::exists("save/safevault/_" + world->name))
					{
						fs::create_directory("save/safevault/_" + world->name);
					}
					ofstream of("save/safevault/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					json j;
					j["insafe"] = 0;
					j["password"] = "";
					j["recovery"] = "";
					json jArray = json::array();
					json jmid;
					for (int i = 1; i <= 20; i++)
					{
						jmid["aposition"] = i;
						jmid["itemid"] = 0;
						jmid["placedby"] = pData->rawName;
						jmid["itemcount"] = 0;
						jArray.push_back(jmid);
					}
					j["safe"] = jArray;
					of << j << std::endl;
					of.close();
				}
				if (tile == 656)
				{
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory("save/mailbox/_" + world->name) || !fs::exists("save/mailbox/_" + world->name))
					{
						fs::create_directory("save/mailbox/_" + world->name);
					}
					ofstream of("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					json j;
					j["x"] = x;
					j["y"] = y;
					j["inmail"] = 0;
					json jArray = json::array();
					json jmid;
					for (int i = 1; i <= 90; i++)
					{
						jmid["aposition"] = i;
						jmid["growid"] = "";
						jmid["text"] = "";
						jArray.push_back(jmid);
					}
					j["mails"] = jArray;
					of << j << std::endl;
					of.close();
				}
				if (tile == 658)
				{
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory("save/bulletinboard/_" + world->name) || !fs::exists("save/bulletinboard/_" + world->name))
					{
						fs::create_directory("save/bulletinboard/_" + world->name);
					}
					ofstream of("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					json j;
					j["inmail"] = 0;
					j["publiccanadd"] = 1;
					j["hidenames"] = 0;
					json jArray = json::array();
					json jmid;
					for (int i = 1; i <= 90; i++)
					{
						jmid["aposition"] = i;
						jmid["growid"] = "";
						jmid["text"] = "";
						jArray.push_back(jmid);
					}
					j["mails"] = jArray;
					of << j << std::endl;
					of.close();
				}
				if (tile == 1006)
				{
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory("save/bluemailbox/_" + world->name) || !fs::exists("save/bluemailbox/_" + world->name))
					{
						fs::create_directory("save/bluemailbox/_" + world->name);
					}
					ofstream of("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					json j;
					j["x"] = x;
					j["y"] = y;
					j["inmail"] = 0;
					json jArray = json::array();
					json jmid;
					for (int i = 1; i <= 90; i++)
					{
						jmid["aposition"] = i;
						jmid["growid"] = "";
						jmid["text"] = "";
						jArray.push_back(jmid);
					}
					j["mails"] = jArray;

					of << j << std::endl;
					of.close();
				}
				if (tile == 6214 || tile == 1420)
				{
					isMannequin = true;
					namespace fs = std::experimental::filesystem;
					if (!fs::is_directory("save/mannequin/_" + world->name) || !fs::exists("save/mannequin/_" + world->name))
					{
						fs::create_directory("save/mannequin/_" + world->name);
					}
					json j;
					auto seedexist = std::experimental::filesystem::exists("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
					if (!seedexist)
					{
						j["clothHead"] = "0";
						j["clothHair"] = "0";
						j["clothMask"] = "0";
						j["clothNeck"] = "0";
						j["clothBack"] = "0";
						j["clothShirt"] = "0";
						j["clothPants"] = "0";
						j["clothFeet"] = "0";
						j["clothHand"] = "0";
						ofstream of("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						of << j;
						of.close();
					}
				}
				break;
			}
		}
		case 2978: /*vend*/
		{
			if (world->owner == "" || isWorldOwner(peer, world) || admin(peer) || isWorldAdmin(peer, world)) {
				VendUpdate = true;
				world->items.at(squaresign).vcount = 0;
				world->items.at(squaresign).vprice = 0;
				world->items.at(squaresign).vid = 0;
				world->items.at(squaresign).vdraw = 0;
				world->items.at(squaresign).opened = true;
				world->items.at(squaresign).rm = false;
				break;
			}
			else return;
		}
		case 2410: case 4426: case 1212: case 1234: case 3110: case 1976: case 2000: case 3122: case 10386: case 5664: case 5662: case 9644: case 5192: case 5194:
		{
			if (tile == 9644)
			{
				SearchInventoryItem(peer, 9644, 1, iscontains);
				if (!iscontains) return;
				else
				{
					if (CheckItemMaxed(peer, 1258, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1258).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1260, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1260).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1262, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1262).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1264, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1264).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1266, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1266).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1268, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1268).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 1270, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(1270).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4308, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4308).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4310, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4310).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4312, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4312).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4314, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4314).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4316, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4316).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4318, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4318).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					if (CheckItemMaxed(peer, 4296, 5))
					{
						Player::OnTalkBubble(peer, pData->netID, "" + getItemDef(4296).name + " wouldnt fit into my inventory!", 0, true);
						return;
					}

					RemoveInventoryItem(9644, 1, peer, true);
					Player::OnTalkBubble(peer, pData->netID, "`wYou received 5 Surgical Sponge, 5 Surgical Scalpel, 5 Surgical Anesthetic, 5 Surgical Antiseptic, 5 Surgical Antibiotics, 5 Surgical Splint, 1 Surgical Stitches, 5 Surgical Pins, 5 Surgical Transfusion, 5 Surgical Defibrillator, 5 Surgical Clamp, 5 Surgical Ultrasound, 5 Surgical Lab Kit and a 1 Surg-E", 0, true);
					Player::OnConsoleMessage(peer, "`oYou received 5 Surgical Sponge, 5 Surgical Scalpel, 5 Surgical Anesthetic, 5 Surgical Antiseptic, 5 Surgical Antibiotics, 5 Surgical Splint, 1 Surgical Stitches, 5 Surgical Pins, 5 Surgical Transfusion, 5 Surgical Defibrillator, 5 Surgical Clamp, 5 Surgical Ultrasound, 5 Surgical Lab Kit and a 1 Surg-E");
					bool success = true;
					SaveItemMoreTimes(1258, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1260, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1262, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1264, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1266, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1268, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(1270, 1, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4308, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4310, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4312, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4314, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4316, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4318, 5, peer, success, pData->rawName + " from surgery tool pack");
					SaveItemMoreTimes(4296, 1, peer, success, pData->rawName + " from surgery tool pack");

					ENetPeer* currentPeer;
					for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer))
						{
							SendTradeEffect(currentPeer, 1258, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1260, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1262, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1264, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1266, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1268, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 1270, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4308, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4310, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4312, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4314, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4316, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4318, pData->netID, pData->netID, 150);
							SendTradeEffect(currentPeer, 4296, pData->netID, pData->netID, 150);
						}
					}
				}
			}

			if (tile == 5662)
			{
				SearchInventoryItem(peer, 5662, 200, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6You will need more dust than that!", 0, true);
				else
				{
					RemoveInventoryItem(5662, 200, peer, true);
					bool success = true;
					SaveItemMoreTimes(5642, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5The dust stirs and begins to swirl! Cupid appears before you.", 0, true);
					pData->cloth_hand = 5642;
					sendClothes(peer);
				}
			}
			if (tile == 5664)
			{
				SearchInventoryItem(peer, 5664, 1, iscontains);
				if (!iscontains) return;
				else
				{
					RemoveInventoryItem(5664, 1, peer, true);
					bool success = true;
					SaveItemMoreTimes(5662, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
				}
			}
			if (tile == 10386)
			{
				SearchInventoryItem(peer, 10386, 1, iscontains);
				if (!iscontains) return;
				else
				{
					RemoveInventoryItem(10386, 1, peer, true);
					int itemuMas[59] = { 1216, 1218, 1992, 1982, 1994, 1972, 1980, 1988, 1984, 3116, 3102, 3106, 3110, 4160, 4162, 4164, 4154, 4158, 5224, 5222, 5232, 5240, 5248, 5238, 5256, 7116, 7108, 7110, 7128, 7112, 7114, 7130, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 1236, 1182, 1184, 1186, 1188, 1170, 10198, 10196, 10202, 10204, 10326, 10324, 10322 };
					auto randIndex = rand() % 59;
					int itemid = itemuMas[randIndex];
					bool success = true;
					SaveItemMoreTimes(itemid, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnConsoleMessage(peer, "`oYou received `21 " + getItemDef(itemid).name + " `ofrom the Gift of Growganoth.");
					Player::OnTalkBubble(peer, pData->netID, "`wYou received `21 " + getItemDef(itemid).name + " `wfrom the Gift of Growganoth.", 0, true);
				}
			}

			if (tile == 5192) {
				SearchInventoryItem(peer, 5192, 75, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough earth pigments here to make anything. Get more!", 0, true);
				else {
					RemoveInventoryItem(5192, 75, peer, true);
					bool success = true;
					SaveItemMoreTimes(7558, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The fragments forms into a earth wings!", 0, true);
					pData->cloth_back = 7558;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}

			if (tile == 5194) {
				SearchInventoryItem(peer, 5194, 75, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough lava pigments here to make anything. Get more!", 0, true);
				else {
					RemoveInventoryItem(5194, 75, peer, true);
					bool success = true;
					SaveItemMoreTimes(5196, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The fragments forms into a magma wings!", 0, true);
					pData->cloth_back = 5196;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}

			if (tile == 3122)
			{
				SearchInventoryItem(peer, 3122, 16, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough fragments here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(3122, 16, peer, true);
					bool success = true;
					SaveItemMoreTimes(3120, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The fragments forms into a teeny devil wings!", 0, true);
					pData->cloth_back = 3120;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}
			if (tile == 2000)
			{
				SearchInventoryItem(peer, 2000, 20, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough bones here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(2000, 20, peer, true);
					bool success = true;
					SaveItemMoreTimes(1998, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The bones forms into a skeletal dragon claw!", 0, true);
					pData->cloth_hand = 1998;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}
			if (tile == 1976)
			{
				SearchInventoryItem(peer, 1976, 10, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough skulls here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(1976, 10, peer, true);
					bool success = true;
					SaveItemMoreTimes(1974, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The skulls forms into a nightmare magnifying glass!", 0, true);
					pData->cloth_hand = 1974;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}
			if (tile == 1212)
			{
				SearchInventoryItem(peer, 1212, 25, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough fur here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(1212, 25, peer, true);
					bool success = true;
					SaveItemMoreTimes(1190, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The fur forms into a cuddly black cat!", 0, true);
					pData->cloth_hand = 1190;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}
			if (tile == 1234)
			{
				SearchInventoryItem(peer, 1234, 4, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough shards here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(1234, 4, peer, true);
					bool success = true;
					SaveItemMoreTimes(1206, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The shards forms into a devil wings!", 0, true);
					pData->cloth_back = 1206;
					Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
					sendClothes(peer);
				}
			}
			if (tile == 3110)
			{
				SearchInventoryItem(peer, 3110, 25, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`6There's just not enough tatters here to make anything. Get more!", 0, true);
				else
				{
					RemoveInventoryItem(3110, 25, peer, true);
					if ((rand() % 100) <= 15)
					{
						bool success = true;
						SaveItemMoreTimes(3112, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
						Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The tatters forms into a inside-out vampire cape!", 0, true);
						pData->cloth_back = 3112;
						Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
						sendClothes(peer);
					}
					else
					{
						bool success = true;
						SaveItemMoreTimes(1166, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
						Player::OnTalkBubble(peer, pData->netID, "`5SQUISH! The tatters forms into a vampire cape!", 0, true);
						pData->cloth_back = 1166;
						Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
						sendClothes(peer);
					}
				}
			}
			if (tile == 2410)
			{
				SearchInventoryItem(peer, 2410, 200, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`oYou will need more `^Emerald Shards `oFor that!", 0, true);
				else
				{
					Player::OnTalkBubble(peer, pData->netID, "`oThe power of `^Emerald Shards `oCompressed into `2Emerald Lock`o!", 0, true);
					RemoveInventoryItem(2410, 200, peer, true);
					bool success = true;
					SaveItemMoreTimes(2408, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnConsoleMessage(peer, "`o>> You received emerald lock!");
				}
			}
			if (tile == 4426)
			{
				SearchInventoryItem(peer, 4426, 200, iscontains);
				if (!iscontains) Player::OnTalkBubble(peer, pData->netID, "`oYou will need more `4Ruby Shards `oFor that!", 0, true);
				else
				{
					Player::OnTalkBubble(peer, pData->netID, "`oThe power of `4Ruby Shards `oCompressed into `4Ruby Lock`o!", 0, true);
					RemoveInventoryItem(4426, 200, peer, true);
					auto success = true;
					SaveItemMoreTimes(4428, 1, peer, success, pData->rawName + " from " + getItemDef(tile).name + "");
					Player::OnConsoleMessage(peer, "`o>> You received ruby lock!");
				}
			}
			return;
		}
		case 6856: case 6858: case 6860: case 6862: case 9266: case 8186: case 8188:
		{
			int Sub_Days = 3;
			string Sub_Type = "free";
			if (tile == 9266) Sub_Days = 1;
			if (tile == 6856) Sub_Days = 3;
			if (tile == 6858) Sub_Days = 14;
			if (tile == 6860 || tile == 8186)
			{
				Sub_Days = 30;
				Sub_Type = "premium";
			}
			if (tile == 6862 || tile == 8188)
			{
				Sub_Days = 365;
				Sub_Type = "premium";
			}
			if (x == pData->x / 32 && y == pData->y / 32)
			{
				if (pData->Subscriber)
				{
					Player::OnTalkBubble(peer, pData->netID, "You already have a Subscription!", 0, false);
					return;
				}
				pData->subtype = Sub_Type;
				pData->subdate = to_string(Sub_Days);
				RemoveInventoryItem(tile, 1, peer, true);
				SendTradeEffect(peer, tile, pData->netID, pData->netID, 150);
				pData->Subscriber = true;
				pData->haveSuperSupporterName = true;
				pData->SubscribtionEndDay = Sub_Days;
				Player::OnParticleEffect(peer, 46, pData->x, pData->y, 0);
				Player::OnAddNotification(peer, "`wYou've unlocked `5Premium Subscribtion `$benefits`w!", "audio/hub_open.wav", "interface/cash_icon_overlay.rttex");
				Player::PlayAudio(peer, "audio/thunderclap.wav", 0);
				try {
					ifstream read_player("save/players/_" + pData->rawName + ".json");
					if (!read_player.is_open()) {
						return;
					}
					json j;
					read_player >> j;
					read_player.close();
					string title = j["title"];
					string chatcolor = j["chatcolor"];
					pData->NickPrefix = title;
					if (pData->NickPrefix != "") {
						restoreplayernick(peer);
						Player::OnNameChanged(peer, pData->netID, pData->NickPrefix + ". " + pData->tankIDName);
					}
					pData->chatcolor = chatcolor;
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
					return;
				}
				send_state(peer);
				auto iscontains = false;
				SearchInventoryItem(peer, 6260, 1, iscontains);
				if (!iscontains)
				{
					bool success = true;
					SaveItemMoreTimes(6260, 1, peer, success, pData->rawName + " from subscription");
					Player::OnAddNotification(peer, "`wYou've obtained `5Amulet Of Force`w!", "audio/hub_open.wav", "interface/cash_icon_overlay.rttex");
				}
			}
			else
			{
				Player::OnTalkBubble(peer, pData->netID, "Only works when click on yourself!", 0, true);
			}
			return;
		}
		// Consummable
		case 126: case 128: case 528: case 540: case 822: case 1662: case 5750: case 6918: case 6924: case 3062: case 5706: case 9286:
		{
			if (tile == 9286 || tile == 5750) /*lucky fortune cookie*/
			{
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (isHere(peer, currentPeer)) {
						if (x == pData->x / 32 && y == pData->y / 32) {
							RemoveInventoryItem(tile, 1, peer, true);
							vector<int> lunar_new_year{ 10616, 10582, 10580, 10664, 10596, 10598, 10586, 10590, 10592, 10576, 10578, 202, 204, 206, 4994, 2978, 5766, 5768, 5744, 5756, 5758, 5760, 5762, 5754, 7688, 7690, 7694, 7686, 7692, 7698, 7696, 9286, 9272, 9290, 9280, 9282, 9292, 9284 };
							vector<string> lunar_messages{ "`5Fortune: You will only get what you have the courage to pursue.``", "`5Fortune: You will live two lives.The second will begin when you realize you only have one.``", "`5Fortune: Looking on your past is fine. Same for the future. Just don't get caught staring.``", "`5Fortune: Your reality check is about to bounce.``", "`5Fortune: Telling people their future is easy. The hard part is being right.``", "`5Fortune: Deal with the faults of others as gently as your own.``", "`5Fortune: Things could always be better. The key is knowing if they're good enough.``", "`5Fortune: You will live two lives. The second will begin when you realize you only have one.``", "`5Fortune: Things could always be better. The key is knowing if they're good enough.``", "`5Fortune: Knowledge is worthless unless put into practice.``", "`5Fortune: You can't stop the waves, but you can learn to surf.``", "`5Fortune: A block in the hand is worth two on the tree.``", "`5Fortune: If it feels like you're always arguing with idiots, consider the company you keep.``", "`5Fortune: Don't chase happiness - create it.``", "`5Fortune: Even a fish wouldn't get into trouble if it kept its mouth shut.``", "`5Fortune: One's measure is in how they treat those that cannot help them.``" };
							if (tile == 5750) lunar_new_year.push_back(9286);
							int rand_item = lunar_new_year[rand() % lunar_new_year.size()];
							string rand_message = lunar_messages[rand() % lunar_messages.size()];
							int count = 1;
							if (rand_item == 5768) count = 4;
							if (rand_item == 5766) count = 3;
							if (rand_item == 5744 || rand_item == 9290) count = 8;
							if (rand_item == 7696 || rand_item == 9272 || rand_item == 5754 || rand_item == 10576) {
								int target = 5;
								if (tile == 9286) target = 10;
								if ((rand() % 1000) <= target) {}
								else rand_item = 5744;
							}
							Player::OnTalkBubble(peer, pData->netID, "You received `2" + to_string(count) + " " + getItemDef(rand_item).name + "`` from the Lucky Fortune Cookie.", 0, false);
							Player::OnConsoleMessage(peer, "You received `2" + to_string(count) + " " + getItemDef(rand_item).name + "`` from the Lucky Fortune Cookie.");
							Player::OnTalkBubble(peer, pData->netID, rand_message, 0, false);
							Player::OnConsoleMessage(peer, rand_message);
							bool success = true;
							SaveItemMoreTimes(rand_item, count, peer, success);
							break;
						}
						else if (x == static_cast<PlayerInfo*>(currentPeer->data)->x / 32 && y == static_cast<PlayerInfo*>(currentPeer->data)->y / 32) {
							Player::OnTalkBubble(peer, pData->netID, "You can only use that on yourself.", 0, true);
							break;
						}
						else {
							Player::OnTalkBubble(peer, pData->netID, "Must be used on a person.", 0, true);
						}
					}
				}
			}
			if (tile == 540)
			{
				RemoveInventoryItem(540, 1, peer, true);
				Player::OnTalkBubble(peer, pData->netID, "`2BURRRPPP...!", 0, true);
			}
			if (tile == 3062)
			{
				if (world->items.at(x + (y * world->width)).fire == false && world->items.at(x + (y * world->width)).water == false)
				{
					if (isSeed(world->items.at(x + (y * world->width)).foreground) || world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 6864 || world->items.at(x + (y * world->width)).water || world->items.at(x + (y * world->width)).foreground == 6952 || world->items.at(x + (y * world->width)).foreground == 6954 || world->items.at(x + (y * world->width)).foreground == 5638 || world->items.at(x + (y * world->width)).foreground == 6946 || world->items.at(x + (y * world->width)).foreground == 6948 || world->items.at(x + (y * world->width)).foreground == 2978 || world->items.at(x + (y * world->width)).foreground == 1420 || world->items.at(x + (y * world->width)).foreground == 6214 || world->items.at(x + (y * world->width)).foreground == 1006 || world->items.at(x + (y * world->width)).foreground == 656 || world->items.at(x + (y * world->width)).foreground == 1420 || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DONATION || world->items.at(x + (y * world->width)).foreground == 3528 || world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0 || world->items.at(x + (y * world->width)).foreground == 6 || world->items.at(x + (y * world->width)).foreground == 8 || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DISPLAY) {
						if (world->items.at(x + (y * world->width)).background != 6864) Player::OnTalkBubble(peer, pData->netID, "`wCan't burn that!", 0, true);
						return;
					}
					if (world->items.at(x + (y * world->width)).foreground != 6 && world->items.at(x + (y * world->width)).foreground != 8 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType != BlockTypes::LOCK)
					{
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0)
						{
							Player::OnTalkBubble(peer, pData->netID, "`wTheres nothing to burn!", 0, true);
							return;
						}
						world->items.at(x + (y * world->width)).fire = !world->items.at(x + (y * world->width)).fire;
						ENetPeer* net_peer;
						for (net_peer = server->peers;
							net_peer < &server->peers[server->peerCount];
							++net_peer)
						{
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer))
							{
								Player::OnParticleEffect(net_peer, 150, x * 32 + 16, y * 32 + 16, 0);
								Player::OnTalkBubble(net_peer, pData->netID, "`7[```4MWAHAHAHA!! FIRE FIRE FIRE```7]``", 0, false);
							}
						}
						RemoveInventoryItem(3062, 1, peer, true);
						UpdateVisualsForBlock(peer, true, x, y, world);
						if (world->items.at(x + (y * world->width)).foreground == 0)
						{
							if (getItemDef(tile).rarity != 999)
							{
								int b = getGemCount(tile) + rand() % 1;
								while (b > 0)
								{
									if (b >= 100)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											100, 0);
										b -= 100;
										for (int i = 0; i < rand() % 1; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 10, 0);
										for (int i = 0; i < rand() % 4; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
									if (b >= 50)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											50, 0);
										b -= 50;
										for (int i = 0; i < rand() % 1; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 5, 0);
										for (int i = 0; i < rand() % 3; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
									if (b >= 10)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											10, 0);
										b -= 10;
										for (int i = 0; i < rand() % 4; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
									if (b >= 7)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											10, 0);
										b -= 5;
										for (int i = 0; i < rand() % 2; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
									if (b >= 5)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											5, 0);
										b -= 5;
										for (int i = 0; i < rand() % 2; i++) DropItem(world, peer, -1, x * 32 + (rand() % 16), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
									if (b >= 1)
									{
										DropItem(
											world, peer, -1,
											x * 32 + (rand() % 16),
											y * 32 + (rand() % 16),
											112,
											1, 0);
										b -= 1;
										for (int i = 0; i < rand() % 1; i++) DropItem(world, peer, -1, x * 32 + (rand() % 8), y * 32 + (rand() % 16), 112, 1, 0);
										continue;
									}
								}
							}
						}
					}
				}
				return;
			}
			if (tile == 822) {
				if (world->items.at(x + (y * world->width)).fire) {
					SendThrowEffect(peer, 822, pData->netID, -1, 150, 0, x * 32 + 16, y * 32 + 16);
					RemoveInventoryItem(822, 1, peer, true);
					world->items.at(x + (y * world->width)).fire = false;
					UpdateBlockState(peer, x, y, true, world);
					for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
						if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
						if (isHere(peer, net_peer)) {
							Player::OnParticleEffect(net_peer, 149, x * 32, y * 32, 0);
						}
					}
					return;
				}
				if (world->items.at(x + (y * world->width)).foreground != 6 && world->items.at(x + (y * world->width)).foreground != 8 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType != BlockTypes::LOCK) {
					if (isWorldOwner(peer, world) || world->owner == "" || admin(peer)) {
						if (world->items.at(x + (y * world->width)).water) {
							world->items.at(x + (y * world->width)).water = false;
							UpdateBlockState(peer, x, y, false, world);
							if ((rand() % 99) + 1 < 40) {
								bool success = true;
								SaveItemMoreTimes(822, 1, peer, success);
							}
							UpdateVisualsForBlock(peer, false, x, y, world);
						}
						else {
							world->items.at(x + (y * world->width)).water = true;
							UpdateBlockState(peer, x, y, true, world);
							RemoveInventoryItem(822, 1, peer, true);
							SendThrowEffect(peer, 822, pData->netID, -1, 150, 0, x * 32 + 16, y * 32 + 16);
							UpdateVisualsForBlock(peer, true, x, y, world);
						}
					}
				}
			}
			return;
		}
		case 1866:
		{
			if (world->owner == "" || isWorldOwner(peer, world) || admin(peer)) {
				if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
					return;
				}
				world->items.at(x + (y * world->width)).glue = !world->items.at(x + (y * world->width)).glue;
				UpdateVisualsForBlock(peer, true, x, y, world);
			}
			return;
		}
		case 3562:
		{
			Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wCave Blast`|left|3562|\nadd_textbox|This item creates a new world!  Enter a unique name for it.|left|\nadd_text_input|cavename|New World Name||24|\nend_dialog|usecaveblast|Cancel|`5Create!|\n");
			return;
		}
		case 1402:
		{
			Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wThermonuclear Blast`|left|1402|\nadd_textbox|This item creates a new world!  Enter a unique name for it.|left|\nadd_text_input|thermoname|New World Name||24|\nend_dialog|usethermoblast|Cancel|`5Create!|\n");
			return;
		}
		case 1826:
		{
			if (!isWorldOwner(peer, world)) return;
			auto iscontainsss = false;
			SearchInventoryItem(peer, 1826, 1, iscontainsss);
			if (!iscontainsss) {
				return;
			}
			else {
				auto FoundSomething = false;
				for (auto i = 0; i < world->width * world->height; i++) {
					if (isSeed(world->items.at(i).foreground)) {
						sendTileUpdate((i % world->width), (i / world->width), 18, pData->netID, peer, world);
						ENetPeer* net_peer;
						for (net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								Player::OnParticleEffect(net_peer, 182, (i % world->width) * 32, (static_cast<float>(i) / world->width) * 32, 0);
							}
						}
						FoundSomething = true;
					}
				}
				if (FoundSomething) RemoveInventoryItem(1826, 1, peer, true);
			}
			return;
		}
		case 5640: // Magplant Remote
		{
			bool aryra = false;
			for (int i = 0; i < world->width * world->height; i++)
			{
				if (world->items.at(i).foreground == 5638)
				{
					aryra = true;
				}
			}
			if (aryra == true) {
				if (pData->magplantx != 0 && pData->magplanty != 0) {
					int squaresign = pData->magplantx + (pData->magplanty * world->width);
					string currentworld = pData->currentWorld + "X" + std::to_string(squaresign);
					if (world->items.at(pData->magplantx + (pData->magplanty * world->width)).mid == pData->magplantitemid && world->items.at(pData->magplantx + (pData->magplanty * world->width)).mc > 0) {
						int magplantid = pData->magplantitemid;
						bool RotatedRight = false;
						auto xpos = x * 32;
						auto ppos = pData->x;
						if (pData->x < x * 32) RotatedRight = true;
						if (RotatedRight) ppos += 19;
						xpos = xpos / 32;
						ppos = ppos / 32;
						if (world->items.at(x + (y * world->width)).foreground != 0 && getItemDef(magplantid).blockType != BlockTypes::BACKGROUND && getItemDef(magplantid).blockType != BlockTypes::GROUND_BLOCK) return;
						ENetPeer* currentPeer;
						for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer)) {
								bool RotatedRight = false;
								auto xpos = x * 32;
								auto ppos = static_cast<PlayerInfo*>(currentPeer->data)->x;
								if (static_cast<PlayerInfo*>(currentPeer->data)->x < x * 32) RotatedRight = true;
								if (RotatedRight) ppos += 19;
								xpos = xpos / 32;
								ppos = ppos / 32;
								if (ppos == xpos && static_cast<PlayerInfo*>(currentPeer->data)->y / 32 == y && getItemDef(magplantid).properties != Property_NoSeed && getItemDef(magplantid).properties != Property_Foreground && getItemDef(magplantid).properties != Property_MultiFacing && getItemDef(magplantid).blockType != BlockTypes::SEED && getItemDef(magplantid).blockType != BlockTypes::STEAM && getItemDef(magplantid).blockType != BlockTypes::UNKNOWN && getItemDef(magplantid).blockType != BlockTypes::VENDING && getItemDef(magplantid).blockType != BlockTypes::ANIM_FOREGROUND && getItemDef(magplantid).blockType != BlockTypes::BULLETIN_BOARD && getItemDef(magplantid).blockType != BlockTypes::FACTION && getItemDef(magplantid).blockType != BlockTypes::CHEST && getItemDef(magplantid).blockType != BlockTypes::GEMS && getItemDef(magplantid).blockType != BlockTypes::MAGIC_EGG && getItemDef(magplantid).blockType != BlockTypes::CRYSTAL && getItemDef(magplantid).blockType != BlockTypes::MAILBOX && getItemDef(magplantid).blockType != BlockTypes::PORTAL && getItemDef(magplantid).blockType != BlockTypes::PLATFORM && getItemDef(magplantid).blockType != BlockTypes::SFX_FOREGROUND && getItemDef(magplantid).blockType != BlockTypes::CHEMICAL_COMBINER && getItemDef(magplantid).blockType != BlockTypes::SWITCH_BLOCK && getItemDef(magplantid).blockType != BlockTypes::TRAMPOLINE && getItemDef(magplantid).blockType != BlockTypes::TOGGLE_FOREGROUND && getItemDef(magplantid).blockType != BlockTypes::GROUND_BLOCK && getItemDef(magplantid).blockType != BlockTypes::BACKGROUND && getItemDef(magplantid).blockType != BlockTypes::MAIN_DOOR && getItemDef(magplantid).blockType != BlockTypes::SIGN && getItemDef(magplantid).blockType != BlockTypes::DOOR && getItemDef(magplantid).blockType != BlockTypes::CHECKPOINT && getItemDef(magplantid).blockType != BlockTypes::GATEWAY && getItemDef(magplantid).blockType != BlockTypes::TREASURE && getItemDef(magplantid).blockType != BlockTypes::WEATHER) return;
							}
						}
						if (world->isPublic || isWorldAdmin(peer, world) || pData->rawName == world->owner || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
							world->items.at(x + (y * world->width)).foreground = magplantid;
							world->items.at(pData->magplantx + (pData->magplanty * world->width)).mc -= 1;
							PlayerMoving data3{};
							data3.packetType = 0x3;
							data3.characterState = 0x0;
							data3.x = x;
							data3.y = y;
							data3.punchX = x;
							data3.punchY = y;
							data3.XSpeed = 0;
							data3.YSpeed = 0;
							data3.netID = -1;
							data3.plantingTree = magplantid;
							for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer)) {
									auto raw = packPlayerMoving(&data3);
									raw[2] = dicenr;
									raw[3] = dicenr;
									SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
								}
							}
							for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer)) {
									if (world->items.at(pData->magplantx + (pData->magplanty * world->width)).mc <= 0) {
										send_item_sucker(currentPeer, 5638, pData->magplantx, pData->magplanty, world->items.at(pData->magplantx + (pData->magplanty * world->width)).mid, 0, true, true, world->items.at(pData->magplantx + (pData->magplanty * world->width)).background);
									}
									else if (world->items.at(pData->magplantx + (pData->magplanty * world->width)).mc >= 5000) {
										send_item_sucker(currentPeer, 5638, pData->magplantx, pData->magplanty, world->items.at(pData->magplantx + (pData->magplanty * world->width)).mid, -1, true, true, world->items.at(pData->magplantx + (pData->magplanty * world->width)).background);
									}
									else {
										send_item_sucker(currentPeer, 5638, pData->magplantx, pData->magplanty, world->items.at(pData->magplantx + (pData->magplanty * world->width)).mid, 1, true, true, world->items.at(pData->magplantx + (pData->magplanty * world->width)).background);
									}
								}
							}
						}
					}
					else {
						Player::OnTalkBubble(peer, pData->netID, "`wThe `2MAGPLANT 5000 `wis empty!", 0, false);
					}
				}
			}
			return;
		}
		case 1360:
		{
			vector<int> common{ 242, 828, 68, 82, 5158, 98, 404, 400, 3402, 3400 };
			const int Index = rand() % common.size();
			const auto ItemID = common[Index];
			{
				if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID))
					Player::OnConsoleMessage(peer, "`oYour Inventory is too `4full`o!");
				break;
			}
			Player::OnTalkBubble(peer, pData->netID, "`wThere's 1 `2" + getItemDef(ItemID).name + " `win the package!", 0, true);
			Player::OnConsoleMessage(peer, "`wThere's 1 `2" + getItemDef(ItemID).name + " `win the package!");
			RemoveInventoryItem(1360, 1, peer, true);
			bool success = true;
			SaveItemMoreTimes(ItemID, 1, peer, success, pData->rawName + "");
		}
		case 1404:
		{
			if (static_cast<PlayerInfo*>(peer->data)->lastDM + 30000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
			{
				static_cast<PlayerInfo*>(peer->data)->lastDM = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
				if (!isWorldOwner(peer, world) && !admin(peer) && world->owner == "")
				{
					Player::OnTextOverlay(peer, "`wYou must be `9Owner `wowner Use Door Mover!");
					return;
				}
				if (world->items[x + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(y) * world->width)].foreground != 0)
				{
					Player::OnTalkBubble(peer, pData->netID, "There's no room to put the door here! You need 2 empty spaces vertically.", 0, false);
				}
				else if (world->items[x + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(y) * world->width) + 100].foreground != 0)
				{
					Player::OnTalkBubble(peer, pData->netID, "There's no room to put the door here! You need 2 empty spaces vertically.", 0, false);
				}
				else
				{
					for (int i = 0; i < world->width * world->height; i++)
					{
						if (i >= 5400)
						{
							world->items[i].foreground = 8;
						}
						else if (world->items[i].foreground == 6)
						{
							world->items[i].foreground = 0;
							world->items[static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(i) + 100].foreground = 0;
						}
						else if (world->items[i].foreground != 6)
						{
							world->items[x + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(y) * world->width)].foreground = 6;
							world->items[x + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(y) * world->width) + 100].foreground = 8;
						}
					}
					RemoveInventoryItem(1404, 1, peer, true);
					WorldInfo* wrld = getPlyersWorld(peer);
					ENetPeer* currentPeer;
					for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer))
						{
							{
								sendPlayerLeave(peer);
								string act = ((PlayerInfo*)(peer->data))->currentWorld;
								WorldInfo info = worldDB.get(act);
								sendWorldOffers(peer);
								bool success = true;
							}
						}
					}
				}
			}
			else {
				Player::OnTextOverlay(peer, "Slow down!");
			}
			return;
		}
		case 1680:
		{
			SearchInventoryItem(peer, 1680, 1, iscontains);
			SearchInventoryItem(peer, 834, 200, iscontains);
			if (!iscontains)
			{
				Player::OnTalkBubble(peer, pData->netID, "`wRequires 200 Fireworks to launch.", 0, true);
				return;
			}
			else
			{
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
							RemoveInventoryItem(1680, 1, peer, true);
							RemoveInventoryItem(834, 200, peer, true);
							vector<int> sfw{ 3764, 1664, 6308, 830, 6306, 4814, 9732, 6322, 11046, 4818, 2874, 8590, 11008, 2802, 1678, 1666, 2870, 2872, 8618, 8616, 4816, 1676, 2864, 1670, 4822, 844, 1668, 6310, 2868 };
							auto itemid = sfw[rand() % sfw.size()];
							DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, itemid, 1, 0);
							{
								Player::OnParticleEffect(peer, 90, x * 32 + rand() % 18, y * 32 + rand() % 18, 0);
							}
						}
					}
				}
			}
			return;
		}
		case 834:
		{
			RemoveInventoryItem(834, 1, peer, true);
			ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (isHere(peer, currentPeer))
				{
					std::vector<int> list{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 836 };
					int index = rand() % list.size();
					auto value = list[index];
					if (value == 836)
					{
						srand(GetTickCount()); // not needed really
						DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 836, 1, 0);
					}
					{
						Player::OnParticleEffect(peer, 40, x * 32 + rand() % 18, y * 32 + rand() % 18, 0);
					}
				}
			}
		}
		case 2480:
		{
			Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`0Megaphone`|left|2480|\n\nadd_spacer|small|\nadd_label|small|`oEnter a message you want to boardcast to every player in Growtopia! This will use up 1 Megaphone.|\nadd_spacer|small|\nadd_text_input|boardcasttext|||40|\nadd_spacer|small|\nend_dialog|boardcastDialog|Nevermind|Boardcast|\n");
			return;
		}
		case 5460: case 4520: case 382: case 3116: case 732: case 2994: case 4368: case 274: case 276: case 278: case 5708: case 5709: case 5780: case 5781: case 5782: case 5783: case 5784: case 5785: case 5710: case 5711: case 5786: case 5787: case 5788: case 5789: case 5790: case 5791: case 6146: case 6147: case 6148: case 6149: case 6150: case 6151: case 6152: case 6153: case 5670: case 5671: case 5798: case 5799: case 5800: case 5801: case 5802: case 5803: case 5668: case 5669: case 5792: case 5793: case 5794: case 5795: case 5796: case 5797: case 544: case 54600: case 1902: case 1508: case 428: case 3808: case 5132: case 7166: case 5078: case 5080: case 5082: case 5084: case 5126: case 5128: case 5130: case 5144: case 5146: case 5148: case 5150: case 5162: case 5164: case 5166: case 5168: case 5180: case 5182: case 5184: case 5186: case 7168: case 7170: case 7172: case 7174: case 9999: case 980: case 3212: case 4742: case 3496: case 3270: case 4722: case 9212: case 5134: case 5152: case 5170: case 5188: case 611:
		{
			return;
		}
		default:
		{
			if (world->items.at(x + (y * world->width)).fire && tile != 18) return;
			if (tile == 6954) {
				isMagplant = true;
			}
			if (getItemDef(tile).blockType == BlockTypes::PROVIDER) {
				isScience = true;
			}
			if (getItemDef(tile).blockType == BlockTypes::DONATION)
			{
				namespace fs = std::experimental::filesystem;
				if (!fs::is_directory("save/donationboxes/_" + world->name) || !fs::exists("save/donationboxes/_" + world->name))
				{
					fs::create_directory("save/donationboxes/_" + world->name);
				}
				ofstream of("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
				json j;
				j["donated"] = 0;
				json jArray = json::array();
				json jmid;
				for (int i = 1; i <= 20; i++)
				{
					jmid["aposition"] = i;
					jmid["sentBy"] = "";
					jmid["note"] = "";
					jmid["itemid"] = 0;
					jmid["itemcount"] = 0;
					jArray.push_back(jmid);
				}
				j["donatedItems"] = jArray;
				of << j << std::endl;
				of.close();
			}

			if (getItemDef(tile).blockType == BlockTypes::GATEWAY) {
				world->items.at(squaresign).opened = false;
				isgateway = false;
			}

			if (world->items.at(x + (y * world->width)).foreground == 1420 || world->items.at(x + (y * world->width)).foreground == 6214 && tile != 18) {
				if (getItemDef(tile).blockType == BlockTypes::CLOTHING)
				{
					if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner))
					{
						if (getItemDef(tile).properties & Property_Untradable) {
							Player::OnTalkBubble(peer, pData->netID, "You can't use untradeable items with mannequins.", 0, true);
							return;
						}
						auto seedexist = std::experimental::filesystem::exists("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
						if (seedexist)
						{
							json j;
							ifstream fs("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
							fs >> j;
							fs.close();

							int c = getItemDef(tile).clothType;
							if (c == 0) {
								//world->items.at(x + (y * world->width)).clothHead = tile;
								if (j["clothHead"].get<string>() != "0") return;
								j["clothHead"] = to_string(tile);
							}
							else if (c == 7) {
								//world->items.at(x + (y * world->width)).clothHair = tile;
								if (j["clothHair"].get<string>() != "0") return;
								j["clothHair"] = to_string(tile);
							}
							else if (c == 4) {
								//world->items.at(x + (y * world->width)).clothMask = tile;
								if (j["clothMask"].get<string>() != "0") return;
								j["clothMask"] = to_string(tile);
							}
							else if (c == 8) {
								//world->items.at(x + (y * world->width)).clothNeck = tile;
								if (j["clothNeck"].get<string>() != "0") return;
								j["clothNeck"] = to_string(tile);
							}
							else if (c == 6) {
								//world->items.at(x + (y * world->width)).clothBack = tile;
								if (j["clothBack"].get<string>() != "0") return;
								j["clothBack"] = to_string(tile);
							}
							else if (c == 1) {
								//world->items.at(x + (y * world->width)).clothShirt = tile;
								if (j["clothShirt"].get<string>() != "0") return;
								j["clothShirt"] = to_string(tile);
							}
							else if (c == 2) {
								//world->items.at(x + (y * world->width)).clothPants = tile;
								if (j["clothPants"].get<string>() != "0") return;
								j["clothPants"] = to_string(tile);
							}
							else if (c == 3) {
								//world->items.at(x + (y * world->width)).clothFeet = tile;
								if (j["clothFeet"].get<string>() != "0") return;
								j["clothFeet"] = to_string(tile);
							}
							else if (c == 5) {
								//world->items.at(x + (y * world->width)).clothHand = tile;
								if (j["clothHand"].get<string>() != "0") return;
								j["clothHand"] = to_string(tile);
							}

							if (c != 10) {
								auto iscontains = false;
								SearchInventoryItem(peer, tile, 1, iscontains);
								if (iscontains)
								{
									updateMannequin(peer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).sign,
										atoi(j["clothHair"].get<string>().c_str()), atoi(j["clothHead"].get<string>().c_str()),
										atoi(j["clothMask"].get<string>().c_str()), atoi(j["clothHand"].get<string>().c_str()), atoi(j["clothNeck"].get<string>().c_str()),
										atoi(j["clothShirt"].get<string>().c_str()), atoi(j["clothPants"].get<string>().c_str()), atoi(j["clothFeet"].get<string>().c_str()),
										atoi(j["clothBack"].get<string>().c_str()), true, 0);

									ofstream of("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									of << j;
									of.close();
									RemoveInventoryItem(tile, 1, peer, true);
								}
							}
						}
					}
				}
			}
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					bool RotatedRight = false;
					auto xpos = x * 32;
					auto ppos = static_cast<PlayerInfo*>(currentPeer->data)->x;
					if (static_cast<PlayerInfo*>(currentPeer->data)->x < x * 32) RotatedRight = true;
					if (RotatedRight) ppos += 19;
					xpos = xpos / 32;
					ppos = ppos / 32;
					if (ppos == xpos && static_cast<PlayerInfo*>(currentPeer->data)->y / 32 == y && getItemDef(tile).properties != Property_NoSeed && getItemDef(tile).properties != Property_Foreground && getItemDef(tile).properties != Property_MultiFacing && getItemDef(tile).blockType != BlockTypes::SEED && getItemDef(tile).blockType != BlockTypes::STEAM && getItemDef(tile).blockType != BlockTypes::UNKNOWN && getItemDef(tile).blockType != BlockTypes::VENDING && getItemDef(tile).blockType != BlockTypes::ANIM_FOREGROUND && getItemDef(tile).blockType != BlockTypes::BULLETIN_BOARD && getItemDef(tile).blockType != BlockTypes::FACTION && getItemDef(tile).blockType != BlockTypes::CHEST && getItemDef(tile).blockType != BlockTypes::GEMS && getItemDef(tile).blockType != BlockTypes::MAGIC_EGG && getItemDef(tile).blockType != BlockTypes::CRYSTAL && getItemDef(tile).blockType != BlockTypes::MAILBOX && getItemDef(tile).blockType != BlockTypes::PORTAL && getItemDef(tile).blockType != BlockTypes::PLATFORM && getItemDef(tile).blockType != BlockTypes::SFX_FOREGROUND && getItemDef(tile).blockType != BlockTypes::CHEMICAL_COMBINER && getItemDef(tile).blockType != BlockTypes::SWITCH_BLOCK && getItemDef(tile).blockType != BlockTypes::TRAMPOLINE && getItemDef(tile).blockType != BlockTypes::TOGGLE_FOREGROUND && getItemDef(tile).blockType != BlockTypes::GROUND_BLOCK && getItemDef(tile).blockType != BlockTypes::BACKGROUND && getItemDef(tile).blockType != BlockTypes::MAIN_DOOR && getItemDef(tile).blockType != BlockTypes::SIGN && getItemDef(tile).blockType != BlockTypes::DOOR && getItemDef(tile).blockType != BlockTypes::CHECKPOINT && getItemDef(tile).blockType != BlockTypes::GATEWAY && getItemDef(tile).blockType != BlockTypes::TREASURE && getItemDef(tile).blockType != BlockTypes::WEATHER) return;
				}
			}
			if (world->owner == "" && getItemDef(tile).blockType == BlockTypes::LOCK && tile != 202 && tile != 204 && tile != 206 && tile != 4994) {
				bool block_place = false;
				string whosowner = "";
				for (int i = 0; i < world->width * world->height; i++) {
					if (world->items.at(i).foreground == 202 && world->items.at(i).monitorname != pData->rawName || world->items.at(i).foreground == 204 && world->items.at(i).monitorname != pData->rawName || world->items.at(i).foreground == 206 && world->items.at(i).monitorname != pData->rawName || world->items.at(i).foreground == 4994 && world->items.at(i).monitorname != pData->rawName) {
						whosowner = world->items.at(i).monitorname;
						block_place = true;
						break;
					}
				}
				if (block_place) {
					try {
						ifstream read_player("save/players/_" + whosowner + ".json");
						if (!read_player.is_open()) {
							return;
						}
						json j;
						read_player >> j;
						read_player.close();
						string nickname = j["nick"];
						int adminLevel = j["adminLevel"];
						if (nickname == "") {
							nickname = role_prefix.at(adminLevel) + whosowner;
						}
						Player::OnTalkBubble(peer, pData->netID, "`wThat area is owned by " + nickname + "", 0, false);
					}
					catch (std::exception& e) {
						std::cout << e.what() << std::endl;
						return;
					}
					return;
				}
			}
			if (getItemDef(tile).blockType == BlockTypes::LOCK) {
				if (tile != 202) {
					if (world->owner != "") {
						if (!isWorldOwner(peer, world)) {
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`wThat area is owned by " + nickname + "", 0, false);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
						}
						else {
							Player::OnTalkBubble(peer, pData->netID, "`0Only one `$World Lock `0Can be placed in a world!", 0, true);
						}
						return;
					}
				}
			}
			if (getItemDef(tile).blockType == BlockTypes::LOCK && world->items.at(x + (y * world->width)).foreground == 0) {
				if (tile == 202 || tile == 204 || tile == 206 || tile == 4994) {
					if (!restricted_area_check(world, x, y)) {
						Player::OnTalkBubble(peer, pData->netID, "Cant place " + getItemDef(tile).name + " here!", 0, false);
						return;
					}
					world->items.at(x + (y * world->width)).monitorname = pData->rawName;
					isSmallLock = true;
					Player::OnTalkBubble(peer, pData->netID, "Area locked.", 0, false);
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer)) {
							Player::PlayAudio(currentPeer, "audio/use_lock.wav", 0);
						}
					}
				}
				else {
					world->owner = pData->rawName;
					world->isPublic = false;
					pData->worldsowned.push_back(pData->currentWorld);
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer)) {
							Player::OnConsoleMessage(currentPeer, "`3[`w" + world->name + " `ohas been `$World Locked `oBy " + pData->displayName + "`3]");
							Player::PlayAudio(currentPeer, "audio/use_lock.wav", 0);
						}
					}
					if (pData->displayName.find("`") != string::npos) {}
					else {
						pData->displayName = "`2" + pData->displayName;
						Player::OnNameChanged(peer, pData->netID, pData->displayName);
					}
					isLock = true;
				}
			}
			SyncFish(world, peer);
			if (tile == 2914 && isFishingRod(GetPeerData(peer)) || tile == 3016 && isFishingRod(GetPeerData(peer)))
			{
				if (world->items.at(x + (y * world->width)).water)
				{
					int PlayerPos = round(pData->x / 32);
					int PlayerPosY = round(pData->y / 32);
					if (PlayerPos != x && PlayerPos + 1 != x && PlayerPos - 1 != x || PlayerPosY != y && PlayerPosY + 1 != y)
					{
						Player::OnTalkBubble(peer, pData->netID, "Too far away...", 0, true);
						return;
					}
					if (pData->x != 0 && !pData->Fishing)
					{
						RemoveInventoryItem(tile, 1, peer, true);
						pData->FishPosX = x * 32;
						pData->FishPosY = y * 32;
						pData->Fishing = true;
						pData->LastBait = getItemDef(tile).name;
						SendFishingState(peer);
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer)) {
								SendFishing(currentPeer, pData->netID, x, y);
							}
						}
					}
					else
					{
						pData->FishPosX = 0;
						pData->FishPosY = 0;
						pData->Fishing = false;
						send_state(peer);
						Player::OnSetPos(peer, pData->netID, pData->x, pData->y);
					}
				}
				return;
			}
			if (pData->Fishing)
			{
				pData->FishPosX = 0;
				pData->FishPosY = 0;
				pData->Fishing = false;
				send_state(peer);
				Player::OnSetPos(peer, pData->netID, pData->x, pData->y);
				Player::OnTalkBubble(peer, pData->netID, "`wSit perfectly while fishing`w!", 0, true);
				return;
			}
			if (pData->cloth_hand == 3494)
			{
				if (world->owner == "" || pData->rawName == PlayerDB::getProperName(world->owner) || admin(peer))
				{
					switch (tile)
					{
					case 3478:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).red && !world->items.at(x + (y * world->width)).green && !world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3478, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3479, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = true;
						world->items.at(x + (y * world->width)).green = false;
						world->items.at(x + (y * world->width)).blue = false;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 1953289573, 168, 0);
							}
						}
						return;
					}
					case 3480:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).red && world->items.at(x + (y * world->width)).green && !world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3480, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3481, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = true;
						world->items.at(x + (y * world->width)).green = true;
						world->items.at(x + (y * world->width)).blue = false;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 1153289573, 168, 0);
							}
						}
						return;
					}
					case 3482:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (!world->items.at(x + (y * world->width)).red && world->items.at(x + (y * world->width)).green && !world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3482, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3483, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = false;
						world->items.at(x + (y * world->width)).green = true;
						world->items.at(x + (y * world->width)).blue = false;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 94634864, 168, 0);
							}
						}
						return;
					}
					case 3484:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (!world->items.at(x + (y * world->width)).red && world->items.at(x + (y * world->width)).green && world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3484, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3485, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = false;
						world->items.at(x + (y * world->width)).green = true;
						world->items.at(x + (y * world->width)).blue = true;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 3253289573, 168, 0);
							}
						}
						return;
					}
					case 3486:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (!world->items.at(x + (y * world->width)).red && !world->items.at(x + (y * world->width)).green && world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3486, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3486, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = false;
						world->items.at(x + (y * world->width)).green = false;
						world->items.at(x + (y * world->width)).blue = true;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 2553289573, 168, 0);
							}
						}
						return;
					}
					case 3488:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).red && !world->items.at(x + (y * world->width)).green && world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3488, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3489, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = true;
						world->items.at(x + (y * world->width)).green = false;
						world->items.at(x + (y * world->width)).blue = true;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 3205873253, 168, 0);
							}
						}
						return;
					}
					case 3490:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).red && world->items.at(x + (y * world->width)).green && world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "That block is already painted that color!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3490, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3491, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = true;
						world->items.at(x + (y * world->width)).green = true;
						world->items.at(x + (y * world->width)).blue = true;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, 0, 168, 0);
							}
						}
						return;
					}
					case 3492:
					{
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::MAIN_DOOR || getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK || world->items.at(x + (y * world->width)).foreground == 8)
						{
							Player::OnTalkBubble(peer, pData->netID, "That's too special to paint.", 0, false);
							return;
						}
						if (!world->items.at(x + (y * world->width)).red && !world->items.at(x + (y * world->width)).green && !world->items.at(x + (y * world->width)).blue) {
							Player::OnTalkBubble(peer, pData->netID, "Don't waste your varnish on an unpainted block!", 0, false);
							return;
						}
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background == 0) {
							Player::OnTalkBubble(peer, pData->netID, "There's nothing to paint!", 0, false);
							return;
						}
						RemoveInventoryItem(3492, 1, peer, true);
						if (world->items.at(x + (y * world->width)).foreground == 0 || isSeed(world->items.at(x + (y * world->width)).foreground)) {
							if (rand() % 100 <= 15) {
								DropItem(world, peer, -1, data.punchX * 32 + rand() % 18, data.punchY * 32 + rand() % 18, 3493, 1, 0);
							}
							else if (rand() % 100 <= 35) {
								SendFarmableDrop(peer, 5, data.punchX, data.punchY, world);
							}
						}
						world->items.at(x + (y * world->width)).red = false;
						world->items.at(x + (y * world->width)).green = false;
						world->items.at(x + (y * world->width)).blue = false;
						UpdateVisualsForBlock(peer, true, x, y, world);
						for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32 + 16, -1, 168, 0);
							}
						}
						return;
					}
					default: break;
					}
				}
			}
			bool hassmallock = false;
			for (int i = 0; i < world->width * world->height; i++) {
				if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) {
					hassmallock = true;
					break;
				}
			}
			if (hassmallock && !admin(peer) || world->owner != "" && !isWorldOwner(peer, world) && !isWorldAdmin(peer, world) && !admin(peer)) {
				if (pData->rawName == world->owner || admin(peer) || tile == world->publicBlock || causedBy == -1 || tile == 5640 || hassmallock && !restricted_area(peer, world, x, y)) {
				}
				else if (isWorldAdmin(peer, world)) {
					if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
						try {
							ifstream read_player("save/players/_" + world->owner + ".json");
							if (!read_player.is_open()) {
								return;
							}
							json j;
							read_player >> j;
							read_player.close();
							string nickname = j["nick"];
							int adminLevel = j["adminLevel"];
							if (nickname == "") {
								nickname = role_prefix.at(adminLevel) + world->owner;
							}
							Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`2Access Granted`w)", 0, true);
							Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
						}
						catch (std::exception& e) {
							std::cout << e.what() << std::endl;
							return;
						}
						return;
					}
				}
				else if (world->isPublic) {
					if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
						try {
							ifstream read_player("save/players/_" + world->owner + ".json");
							if (!read_player.is_open()) {
								return;
							}
							json j;
							read_player >> j;
							read_player.close();
							string nickname = j["nick"];
							int adminLevel = j["adminLevel"];
							if (nickname == "") {
								nickname = role_prefix.at(adminLevel) + world->owner;
							}
							Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
							Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
						}
						catch (std::exception& e) {
							std::cout << e.what() << std::endl;
							return;
						}
						return;
					}
				}
				else if (world->isEvent) {
					if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
						string whoslock = world->owner;
						if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
							whoslock = world->items.at(x + (y * world->width)).monitorname;
						}
						if (pData->rawName != whoslock) {
							try {
								ifstream read_player("save/players/_" + whoslock + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + whoslock;
								}
								if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
								else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
								Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
							return;
						}
					}
					else if (world->items.at(x + (y * world->width)).foreground != world->publicBlock && causedBy != -1) {
						Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
						return;
					}
				}
				else {
					Player::PlayAudio(peer, "audio/punch_locked.wav", 0);
					return;
				}
				if (tile == 18 && admin(peer)) {
					if (isWorldAdmin(peer, world) && !isWorldOwner(peer, world)) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`2Access Granted`w)", 0, true);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
						}
					}
					else if (world->isPublic && !isWorldOwner(peer, world) || world->items.at(x + (y * world->width)).foreground == 202 && world->items.at(x + (y * world->width)).opened || world->items.at(x + (y * world->width)).foreground == 204 && world->items.at(x + (y * world->width)).opened || world->items.at(x + (y * world->width)).foreground == 206 && world->items.at(x + (y * world->width)).opened || world->items.at(x + (y * world->width)).foreground == 4994 && world->items.at(x + (y * world->width)).opened) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							try {
								ifstream read_player("save/players/_" + world->owner + ".json");
								if (!read_player.is_open()) {
									return;
								}
								json j;
								read_player >> j;
								read_player.close();
								string nickname = j["nick"];
								int adminLevel = j["adminLevel"];
								if (nickname == "") {
									nickname = role_prefix.at(adminLevel) + world->owner;
								}
								Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								return;
							}
						}
					}
					else if (world->isEvent && !isWorldOwner(peer, world) || world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
						if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
							string whoslock = world->owner;
							if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
								whoslock = world->items.at(x + (y * world->width)).monitorname;
							}
							if (pData->rawName != whoslock) {
								try {
									ifstream read_player("save/players/_" + whoslock + ".json");
									if (!read_player.is_open()) {
										return;
									}
									json j;
									read_player >> j;
									read_player.close();
									string nickname = j["nick"];
									int adminLevel = j["adminLevel"];
									if (nickname == "") {
										nickname = role_prefix.at(adminLevel) + whoslock;
									}
									if (world->items.at(x + (y * world->width)).opened) Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`9Open to Public`w)", 0, true);
									else Player::OnTalkBubble(peer, pData->netID, "`w" + nickname + "'s `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "`w. (`4No Access`w)", 0, true);
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									return;
								}
							}
						}
					}
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 10 && tile == 3400)
			{
				if (pData->rawName == world->owner || admin(peer) || world->owner == "")
				{
					RemoveInventoryItem(3400, 1, peer, true);
					world->items.at(x + (y * world->width)).foreground = 392;
					PlayerMoving data3{};
					data3.packetType = 0x3;
					data3.characterState = 0x0;
					data3.x = x;
					data3.y = y;
					data3.punchX = x;
					data3.punchY = y;
					data3.XSpeed = 0;
					data3.YSpeed = 0;
					data3.netID = -1;
					data3.plantingTree = 392;
					ENetPeer* currentPeer;
					for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer))
						{
							auto raw = packPlayerMoving(&data3);
							raw[2] = dicenr;
							raw[3] = dicenr;
							SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
						}
					}
					for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
					}
				}
			}

			if (world->items.at(x + (y * world->width)).foreground == 2946 && tile != 18 && tile != 32 && tile > 0)
			{
				if (pData->rawName == world->owner || admin(peer))
				{
					if (pData->lastDISPLAY + 1000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
					{
						pData->lastDISPLAY = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
						auto iscontains = false;
						SearchInventoryItem(peer, tile, 1, iscontains);
						if (!iscontains) return;
						else
						{
							auto xSize = world->width;
							auto ySize = world->height;
							auto n = tile;
							if (getItemDef(n).properties & Property_Untradable || n == 6336 || n == 1424 || n == 8552 || n == 9472 || n == 5640 || n == 9482 || n == 9356 || n == 9492 || n == 9498 || n == 8774 || n == 1790 || n == 2592 || n == 1784 || n == 1792 || n == 1794 || n == 7734 || n == 8306 || n == 9458)
							{
								Player::OnTalkBubble(peer, pData->netID, "You can't display untradeable items.", 0, true);
								return;
							}
							if (getItemDef(n).blockType == BlockTypes::LOCK || n == 2946)
							{
								Player::OnTalkBubble(peer, pData->netID, "Sorry, no displaying Display Blocks or Locks.", 0, true);
								return;
							}
							if (world->items.at(x + (y * world->width)).intdata == 0)
							{
								world->items.at(x + (y * world->width)).intdata = tile;
								ENetPeer* currentPeer;
								for (currentPeer = server->peers;
									currentPeer < &server->peers[server->peerCount];
									++currentPeer)
								{
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (isHere(peer, currentPeer))
									{
										UpdateVisualsForBlock(currentPeer, true, x, y, world);
										SendThrowEffect(currentPeer, tile, pData->netID, -1, 150, 0, x * 32 + 16, y * 32 + 16);
									}
								}
								RemoveInventoryItem(n, 1, peer, true);
								updateplayerset(peer, n);
							}
							else
							{
								Player::OnTalkBubble(peer, pData->netID, "Remove what's in there first!", 0, true);
							}
							return;
						}
					}
					else
					{
						Player::OnTalkBubble(peer, pData->netID, "Slow down while using display blocks!", 0, true);;
						return;
					}
				}
				else
				{
					if (world->owner == "")
					{
						Player::OnTalkBubble(peer, pData->netID, "This area must be locked to put your item on display!", 0, true);
					}
					else if (world->isPublic)
					{
						Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					}
					else
					{
						Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					}
					return;
				}
			}
			if (world->items.at(x + (y * world->width)).foreground == 3528 && tile != 18 && tile != 32 && tile > 0) {
				if (pData->rawName == world->owner || admin(peer)) {
					auto n = tile;
					world->items.at(x + (y * world->width)).intdata = tile;
					for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
						if (isHere(peer, currentPeer)) {
							send_canvas_data(peer, world->items.at(x + (y * world->width)).foreground, world->items.at(x + (y * world->width)).background, x, y, tile, getItemDef(tile).name);
							SendThrowEffect(currentPeer, tile, pData->netID, -1, 150, 0, x * 32 + 16, y * 32 + 16);
						}
					}
					return;
				}
				else {
					if (world->owner == "")
					{
						Player::OnTalkBubble(peer, pData->netID, "This area must be locked to put your item on display!", 0, true);
					}
					else if (world->isPublic)
					{
						Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					}
					else
					{
						Player::OnTalkBubble(peer, pData->netID, "That area is owned by " + world->owner + "", 0, true);
					}
					return;
				}
			}
			if (getItemDef(tile).blockType == BlockTypes::CONSUMABLE || getItemDef(tile).blockType == BlockTypes::CLOTHING) return;
			if (getItemDef(tile).blockType == BlockTypes::SEED)
			{
				if (world->items[x + (y * world->width)].foreground != 0)
				{
					if (getItemDef(world->items[x + (y * world->width)].foreground).blockType == BlockTypes::SEED)
					{
						if (getPlyersWorld(peer)->isPublic || isWorldAdmin(peer, world) || static_cast<PlayerInfo*>(peer->data)->rawName == world->owner || world->owner == "" || admin(peer))
						{
							if (getItemDef(world->items[x + (y * world->width)].foreground).name == getItemDef(tile).name || getItemDef(world->items[x + (y * world->width)].foreground).rarity == 999 || getItemDef(tile).rarity == 999)
							{
								Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "You cant splice them", 0, true);
								return;
							}
							auto iscontains = false;
							SearchInventoryItem(peer, tile, 1, iscontains);
							if (!iscontains) return;
							auto squaresign = x + (y * 100);
							auto currentworld = static_cast<PlayerInfo*>(peer->data)->currentWorld + "X" + std::to_string(squaresign);
							auto seedexist = std::experimental::filesystem::exists("save/worlds/seed/" + currentworld + ".txt");
							if (!seedexist)
							{
								auto targetvalue = getItemDef(world->items[x + (y * world->width)].foreground).rarity + getItemDef(tile).rarity;
								for (auto i = 0; i < 10016; i++)
								{
									if (getItemDef(i).rarity == targetvalue)
									{
										RemoveInventoryItem(tile, 1, peer, true);
										ofstream seedapply("save/worlds/seed/" + currentworld + ".txt");
										seedapply << "true";
										seedapply.close();
										Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "You spliced " + getItemDef(world->items[x + (y * world->width)].foreground).name + " with " + getItemDef(tile).name + " and got " + getItemDef(i + 1).name, 0, true);
										Player::PlayAudio(peer, "audio/success.wav", 0);
										world->items[x + (y * world->width)].foreground = i + 1;
										PlayerMoving data3;
										data3.packetType = 0x3;
										data3.characterState = 0x0;
										data3.x = x;
										data3.y = y;
										data3.punchX = x;
										data3.punchY = y;
										data3.XSpeed = 0;
										data3.YSpeed = 0;
										data3.netID = -1;
										data3.plantingTree = i + 1;
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
											if (isHere(peer, currentPeer))
											{
												auto raw = packPlayerMoving(&data3);
												raw[2] = dicenr;
												raw[3] = dicenr;
												SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
											}
										}
										ifstream read_entry("config+/farmable_drops/tree_drop/rarity.json");
										json j;
										read_entry >> j;
										read_entry.close();
										int rarity1 = j["rarity_1-14"];
										int rarity15 = j["rarity_15-24"];
										int rarity25 = j["rarity_25-34"];
										int rarity35 = j["rarity_35-44"];
										int rarity45 = j["rarity_45-54"];
										int rarity55 = j["rarity_55-64"];
										int rarity65 = j["rarity_65-74"];
										int rarity75 = j["rarity_75-84"];
										int rarity85 = j["rarity_85-99"];
										int rarity100 = j["rarity_100"];
										if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 1 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 15) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity1 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 15 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 25) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity15 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 25 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 35) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity25 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 35 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 45) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity35 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 45 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 55) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity45 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 55 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 65) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity55 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 65 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 75) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity65 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 75 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 85) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity75 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 85 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 99) {
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity85 + 1;
										}
										else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity <= 99) { // (no limit, even at rarity 10,000+)
											world->items.at(x + (y * world->width)).fruitcount = rand() % rarity100 + 1;
										}
										UpdateTreeVisuals(peer, world->items[x + (y * world->width)].foreground, x, y, world->items[x + (y * world->width)].background, world->items.at(x + (y * world->width)).fruitcount, 999999999999, true, 0);
										break;
									}
								}
							}
							else
							{
								Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "This seed was already spliced!", 0, true);
								return;
							}
							return;
						}
						else
						{
							return;
						}
					}
					else
					{
						return;
					}
				}
				else
				{
					isTree = true;
				}
			}
			break;
		}
		}
		if (causedBy != -1)
		{
			if (!admin(peer))
			{
				auto iscontains = false;
				SearchInventoryItem(peer, tile, 1, iscontains);
				if (!iscontains) return;
			}
		}
		ENetPeer* currentPeer;
		bool Explosion = false;
		if (tile == 18) {
			if (pData->cloth_hand == 3066 && tile == 18) return;
			if (world->items.at(x + (y * world->width)).background == 6864 && world->items.at(x + (y * world->width)).foreground == 0) return;
			if (world->items.at(x + (y * world->width)).background == 0 && world->items.at(x + (y * world->width)).foreground == 0) return;
			if (world->items.at(x + (y * world->width)).fire) return;
			ItemDefinition brak;
			if (world->items.at(x + (y * world->width)).foreground != 0) {
				brak = getItemDef(world->items.at(x + (y * world->width)).foreground);
			}
			else {
				brak = getItemDef(world->items.at(x + (y * world->width)).background);
			}
			bool block_one_hit = false;
			if (world->width == 90 && world->height == 60) block_one_hit = true;
			data.packetType = 0x8;
			data.plantingTree = 6;
			if (break_effect.find(pData->cloth_necklace) != break_effect.end() || break_effect.find(pData->cloth_hand) != break_effect.end() || break_effect.find(pData->cloth_back) != break_effect.end()) {
				if (isWorldOwner(peer, world) || admin(peer) || world->isPublic || world->owner == "") {
					int p_break_effect = 0;
					if (break_effect.find(pData->cloth_hand) != break_effect.end()) {
						p_break_effect = break_effect.at(pData->cloth_hand);
					}
					if (p_break_effect == 0 && break_effect.find(pData->cloth_back) != break_effect.end()) {
						p_break_effect = break_effect.at(pData->cloth_back);
					}
					if (p_break_effect == 0 && break_effect.find(pData->cloth_necklace) != break_effect.end()) {
						p_break_effect = break_effect.at(pData->cloth_necklace);
					}
					if (p_break_effect != 0) {
						int particle_resize = 0;
						switch (p_break_effect) { /*recode this later*/
						case 97:
						{
							int kek = world->items.at(x + (y * world->width)).foreground;
							if (world->items.at(x + (y * world->width)).foreground != 0) kek = world->items.at(x + (y * world->width)).foreground;
							else kek = world->items.at(x + (y * world->width)).background;
							particle_resize = kek;
							break;
						}
						default:
						{
							break;
						}
						} for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
							if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
							if (isHere(peer, net_peer)) {
								SendParticleEffect(net_peer, x * 32 + 16, y * 32, particle_resize, p_break_effect, 0);
							}
						}
					}
				}
			}
			bool no_fire = false;
			bool break_background = false;
			if (tile == 18 && world->items.at(x + (y * world->width)).foreground == 9368 || world->items.at(x + (y * world->width)).foreground == 9498 && rand() % 100 <= 1 && tile == 18 || explosive.find(pData->cloth_hand) != explosive.end() || explosive.find(pData->cloth_back) != explosive.end() || explosive.find(pData->cloth_necklace) != explosive.end()) { /*explosives*/
				if (isWorldOwner(peer, world) || admin(peer) || world->isPublic || world->owner == "") {
					Explosion = true;
					if (explosive.find(pData->cloth_hand) != explosive.end() || explosive.find(pData->cloth_back) != explosive.end() || explosive.find(pData->cloth_necklace) != explosive.end()) {
						break_background = true;
						if (world->width == 90 && world->height == 60) break_background = false;
						no_fire = true;
					}
					int kek = world->items.at(x + (y * world->width)).foreground;
					if (world->items.at(x + (y * world->width)).foreground != 0) kek = world->items.at(x + (y * world->width)).foreground;
					else kek = world->items.at(x + (y * world->width)).background;
					for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
						if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
						if (isHere(peer, net_peer)) {
							SendParticleEffect(net_peer, x * 32, y * 32, kek, 97, 0);
							Player::OnParticleEffect(net_peer, 43, (x + 1) * 32, y * 32, 0);
							Player::OnParticleEffect(net_peer, 43, (x - 1) * 32, y * 32, 0);
							Player::OnParticleEffect(net_peer, 43, x * 32, (y + 1) * 32, 0);
							Player::OnParticleEffect(net_peer, 43, x * 32, (y - 1) * 32, 0);
							Player::OnParticleEffect(net_peer, 43, (x + 1) * 32, (y + 1) * 32, 0);
							Player::OnParticleEffect(net_peer, 43, (x - 1) * 32, (y - 1) * 32, 0);
							Player::OnParticleEffect(net_peer, 43, (x + 1) * 32, (y - 1) * 32, 0);
							Player::OnParticleEffect(net_peer, 43, (x - 1) * 32, (y + 1) * 32, 0);
						}
					}
				}
			}
			bool EarthMastery = false;
			int ChanceOfEarth = 0;
			if (pData->level >= 4) ChanceOfEarth = 1;
			if (pData->level >= 6) ChanceOfEarth = 2;
			if (pData->level >= 8) ChanceOfEarth = 3;
			if (world->items.at(x + (y * world->width)).foreground == 2 && !block_one_hit && tile == 18 && rand() % 100 <= ChanceOfEarth && pData->level >= 4) {
				if (isWorldOwner(peer, world) || admin(peer) || world->isPublic || world->owner == "") {
					EarthMastery = true;
					int kek = world->items.at(x + (y * world->width)).foreground;
					if (world->items.at(x + (y * world->width)).foreground != 0) kek = world->items.at(x + (y * world->width)).foreground;
					else kek = world->items.at(x + (y * world->width)).background;
					for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
						if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
						if (isHere(peer, net_peer)) {
							SendParticleEffect(net_peer, x * 32 + 16, y * 32, kek, 97, 0);
						}
					}
				}
			}
			if (pData->PunchPotion && tile == 18 && !block_one_hit || pData->cloth_necklace == 6260 && tile == 18 && !block_one_hit || pData->cloth_hand == 9164 && tile == 18 && !block_one_hit || pData->cloth_hand == 9488 && tile == 18 && !block_one_hit || pData->cloth_hand == 9496 && tile == 18 && !block_one_hit) {
				if (isWorldOwner(peer, world) || admin(peer) || world->isPublic || world->owner == "") {
					int kek = world->items.at(x + (y * world->width)).foreground;
					if (world->items.at(x + (y * world->width)).foreground != 0) kek = world->items.at(x + (y * world->width)).foreground;
					else kek = world->items.at(x + (y * world->width)).background;
					for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
						if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
						if (isHere(peer, net_peer)) {
							SendParticleEffect(net_peer, x * 32 + 16, y * 32, kek, 97, 0);
						}
					}
				}
			}
			if ((duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && !Explosion && !EarthMastery && !pData->PunchPotion && one_hit.find(pData->cloth_necklace) == one_hit.end() && one_hit.find(pData->cloth_back) == one_hit.end() && one_hit.find(pData->cloth_hand) == one_hit.end() || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && one_hit.find(pData->cloth_necklace) != one_hit.end() && one_hit.find(pData->cloth_back) != one_hit.end() && one_hit.find(pData->cloth_hand) != one_hit.end() && !Explosion && !EarthMastery || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && pData->cloth_hand == 9496 && !Explosion && !EarthMastery || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && pData->cloth_hand == 9488 && !Explosion && !EarthMastery || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && pData->cloth_hand == 9164 && !Explosion && !EarthMastery || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && pData->cloth_necklace == 6260 && !Explosion && !EarthMastery || (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items.at(x + (y * world->width)).breakTime >= 4000 && block_one_hit && world->width == 90 && world->height == 60 && pData->PunchPotion && !Explosion && !EarthMastery) {
				world->items.at(x + (y * world->width)).breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
				world->items.at(x + (y * world->width)).breakLevel = 4;
			}
			else {
				if (isPickaxe(GetPeerData(peer))) brak.breakHits -= 1;
				int break_hits = brak.breakHits;
				if (world->width == 90 && world->height == 60) {
					int white_door_level = 0;
					for (int i = 0; i < world->width * world->height; i++) {
						if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::MAIN_DOOR) {
							white_door_level = i + 100;
							break;
						}
					}
					int deep = 0;
					if (white_door_level < (x + (y * world->width))) deep = (x + (y * world->width - white_door_level)) / 100;
					break_hits += deep;
				}
				if (y < world->height && world->items.at(x + (y * world->width)).breakLevel + 4 >= break_hits * 4 || one_hit.find(pData->cloth_necklace) != one_hit.end() && tile == 18 && !block_one_hit || one_hit.find(pData->cloth_back) != one_hit.end() && tile == 18 && !block_one_hit || one_hit.find(pData->cloth_hand) != one_hit.end() && tile == 18 && !block_one_hit) {
					data.packetType = 0x3;
					data.netID = causedBy;
					data.plantingTree = 18;
					data.punchX = x;
					data.punchY = y;
					world->items.at(x + (y * world->width)).breakLevel = 0;
					auto hi = data.punchX * 32;
					auto hi2 = data.punchY * 32;
					int repeat = 1;
					if (Explosion) {
						repeat = 9;
					}
					int o_x = x;
					int o_y = y;
					for (int i = 1; i <= repeat; i++) {
						if (Explosion) { /*2x2 radius break*/
							int x_r = o_x - 1;
							int y_r = o_y - 1;
							if (i < 3) x_r += i;
							if (i > 3 && i <= 6) {
								x_r += i - 4;
								y_r += 1;
							}
							if (i > 6 && i <= 9) {
								x_r += i - 7;
								y_r += 2;
							}
							x = x_r;
							y = y_r;
						}
						if (x < 0 || y < 0 || x > world->width - 1 || y > world->height - 1) continue;
						if (Explosion && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) continue;
						if (Explosion && world->items.at(x + (y * world->width)).foreground != 8 && world->items.at(x + (y * world->width)).foreground != 0 && getItemDef(world->items.at(x + (y * world->width)).foreground).properties & Property_Mod) continue;
						if (world->items.at(x + (y * world->width)).foreground == 0 && world->items.at(x + (y * world->width)).background != 0 && Explosion && !break_background) continue;
						if (world->items.at(x + (y * world->width)).foreground == 8 && Explosion && rand() % 100 >= 5) continue;
						if (world->items.at(x + (y * world->width)).foreground == 8 && y >= world->height - 1) continue;
						if (Explosion && rand() % 100 <= 5 && world->items.at(x + (y * world->width)).background != 0 && !no_fire) { /*fire*/
							if (world->items.at(x + (y * world->width)).foreground != 3528 && !world->items.at(x + (y * world->width)).water && !world->items.at(x + (y * world->width)).fire && !isSeed(world->items.at(x + (y * world->width)).foreground) && world->items.at(x + (y * world->width)).foreground != 8 && world->items.at(x + (y * world->width)).foreground != 6 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType != BlockTypes::LOCK && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType != BlockTypes::DISPLAY && world->items.at(x + (y * world->width)).foreground != 6952 && world->items.at(x + (y * world->width)).foreground != 6954 && world->items.at(x + (y * world->width)).foreground != 5638 && world->items.at(x + (y * world->width)).foreground != 6946 && world->items.at(x + (y * world->width)).foreground != 6948 && world->items.at(x + (y * world->width)).foreground != 2978 && world->items.at(x + (y * world->width)).foreground != 1420 && world->items.at(x + (y * world->width)).foreground != 6214 && world->items.at(x + (y * world->width)).foreground != 1006 && world->items.at(x + (y * world->width)).foreground != 656 && world->items.at(x + (y * world->width)).foreground != 1420 && getItemDef(world->items.at(x + (y * world->width)).foreground).blockType != BlockTypes::DONATION) {
								world->items.at(x + (y * world->width)).fire = true;
								UpdateVisualsForBlock(peer, true, x, y, world);
							}
						}
						if (Explosion) {
							PlayerMoving data3{};
							data3.packetType = 0x3;
							data3.characterState = 0x0;
							data3.x = x;
							data3.y = y;
							data3.punchX = x;
							data3.punchY = y;
							data3.XSpeed = 0;
							data3.YSpeed = 0;
							data3.netID = causedBy;
							data3.plantingTree = tile;
							for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer)) {
									auto raw = packPlayerMoving(&data3);
									raw[2] = dicenr;
									raw[3] = dicenr;
									SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
								}
							}
						}
						if (world->items.at(x + (y * world->width)).foreground == 5140) {
							std::vector<int> list{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4426 };
							int index = rand() % list.size();
							auto value = list[index];
							if (value == 4426) {
								srand(GetTickCount());
								auto droploc = rand() % 18;
								auto droplocs = rand() % 18;
								DropItem(world, peer, -1, x * 32 + droploc, y * 32 + droplocs, 4426, 1, 0);
							}
						}
						if (world->items.at(x + (y * world->width)).foreground == 836) {
							std::vector<int> list{ 6308, 848, 3702, 8602, 8600, 11068, 832, 2860, 6328, 6324, 6330, 834, 2862, 8612, 4810, 6326, 9744, 3700, 9756, 842, 9748, 11066, 11062, 11064, 840, 850, 3704, 8592, 8594, 3698, 3740, 442, 846, 4804, 3742, 8610, 2856, 1680, 844, 11058, 2858, 11040, 822, 838, 4812 };
							int itemid = list[rand() % list.size()];
							auto value = list[itemid];
							auto amount = 1; // all items drop 1 besides....
							if (value == 832) { // coral drops 10
								amount = 10;
							}
							if (value == 834) { // firework drops 5
								amount = 5;
							}
							if (value == 850) { // ocean rock drop 10
								amount = 10;
							}
							if (value == 442) { // sand drop 10
								amount = 10;
							}
							if (value == 846) { // seaweed drop 10
								amount = 10;
							}
							if (value == 822) { // water bucket drop 10
								amount = 10;
							}
							DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, itemid, amount, 0);
						}
						else if (world->items.at(x + (y * world->width)).foreground == 3402 || world->items.at(x + (y * world->width)).foreground == 392 || world->items.at(x + (y * world->width)).foreground == 9350) {
							if (world->items.at(x + (y * world->width)).foreground == 3402) {
								pData->bootybreaken++;
								if (pData->bootybreaken >= 100) {
									gamepacket_t p;
									p.Insert("OnProgressUIUpdateValue");
									p.Insert(100);
									p.Insert(0);
									p.CreatePacket(peer);
								}
								else {
									gamepacket_t p;
									p.Insert("OnProgressUIUpdateValue");
									p.Insert(pData->bootybreaken);
									p.Insert(0);
									p.CreatePacket(peer);
								}
							}
							vector<int> list{ 362, 3398, 386, 4422, 364, 9340, 9342, 9332, 9334, 9336, 9338, 366, 2388, 7808, 7810, 4416, 7818, 7820, 5652, 7822, 7824, 5644, 390, 7826, 7830, 9324, 5658, 3396, 2384, 5660, 3400, 4418, 4412, 388, 3408, 1470, 3404, 3406, 2390, 5656, 5648, 2396, 384, 5664, 4424, 4400, 1458, 10660, 10654, 10632, 10652, 10626, 10640, 10662 };
							int itemid = list[rand() % list.size()];
							if (itemid == 1458) {
								int target = 5;
								if (world->items.at(x + (y * world->width)).foreground == 9350) target = 20;
								if ((rand() % 1000) <= target) {}
								else itemid = 7808;
							}
							DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, itemid, 1, 0);
						}
						else if (pData->cloth_hand == 8452) {
							Player::OnParticleEffect(peer, 149, data.punchX * 32, data.punchY * 32, 0);
						}
						if (world->items.at(x + (y * world->width)).foreground != 0) {
							int custom_drop = 0;
							if (custom_gem_block.find(world->items.at(x + (y * world->width)).foreground) != custom_gem_block.end()) {
								custom_drop = (rand() % custom_gem_block.at(world->items.at(x + (y * world->width)).foreground)) + 1;
							} if (custom_drop_block.find(world->items.at(x + (y * world->width)).foreground) != custom_drop_block.end()) {
								int chance_of_drop = custom_drop_chance.at(world->items.at(x + (y * world->width)).foreground);
								if (((rand() % 100) + 1) <= chance_of_drop) {
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, custom_drop_block.at(world->items.at(x + (y * world->width)).foreground), 1, 0);
								}
							}
							if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DOOR) {
								if (isWorldOwner(peer, world) || world->owner == "" || admin(peer)) {
									world->items.at(x + (y * world->width)).label = "";
									world->items.at(x + (y * world->width)).destWorld = "";
									world->items.at(x + (y * world->width)).destId = "";
									world->items.at(x + (y * world->width)).currId = "";
									world->items.at(x + (y * world->width)).password = "";
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 1680) {
								SearchInventoryItem(peer, 1680, 1, iscontains);
								SearchInventoryItem(peer, 834, 200, iscontains);
								if (!iscontains) {
									Player::OnTalkBubble(peer, pData->netID, "`wRequires 200 Fireworks to launch.", 0, true);
									return;
								}
								else
								{
									RemoveInventoryItem(1680, 1, peer, true);
									RemoveInventoryItem(834, 200, peer, true);
									vector<int> sfw{ 3764, 1664, 6308, 830, 6306, 4814, 9732, 6322, 11046, 4818, 2874, 8590, 11008, 2802, 1678, 1666, 2870, 2872, 8618, 8616, 4816, 1676, 2864, 1670, 4822, 844, 1668, 6310, 2868 };
									auto itemid = sfw[rand() % sfw.size()];
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, itemid, 1, 0);
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 8440) {
								int kiek = 0;
								if (pData->cloth_hand == 98 || Explosion) {
									kiek = 1;
								} if (kiek != 0) {
									SendXP(peer, 5);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 7960, kiek, 0);
								}
								SendFarmableDrop(peer, 10, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 8532) {
								int kiek = 0;
								if (pData->cloth_hand == 98 || Explosion) {
									kiek = 1;
								} if (kiek != 0) {
									SendXP(peer, 7);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 8428, kiek, 0);
								}
								SendFarmableDrop(peer, 25, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 9146) {
								int kiek = 0;
								if (pData->cloth_hand == 98 || Explosion) {
									kiek = 1;
								} if (kiek != 0) {
									SendXP(peer, 10);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 8430, kiek, 0);
								}
								SendFarmableDrop(peer, 50, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 8530) {
								int kiek = 0;
								if (pData->cloth_hand == 98 || Explosion) {
									kiek = 1;
								} if (kiek != 0) {
									SendXP(peer, 15);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 7962, kiek, 0);
								}
								SendFarmableDrop(peer, 100, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 5160) {
								if (pData->cloth_hand == 3932) {
									if ((rand() % 100) <= 50) {
										Player::OnTalkBubble(peer, pData->netID, "`wThe Rock was so hard to break that it broke your Rock Hammer!", 0, true);
										RemoveInventoryItem(3932, 1, peer, true);
										auto iscontains = false;
										SearchInventoryItem(peer, 3932, 1, iscontains);
										if (!iscontains) {
											pData->cloth_hand = 0;
											Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
											pData->effect = 8421376;
											sendPuncheffect(peer, pData->effect);
											send_state(peer); //here
											sendClothes(peer);
										}
									}
									SendXP(peer, 30);
									SendParticleEffect(peer, x * 32 + 16, y * 32 + 16, 3, 114, 0);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 5190, 1, 0);
								}
								SendFarmableDrop(peer, 5, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 5172) {
								if (pData->cloth_hand == 3932) {
									if ((rand() % 100) <= 50) {
										Player::OnTalkBubble(peer, pData->netID, "`wThe Rock was so hard to break that it broke your Rock Hammer!", 0, true);
										RemoveInventoryItem(3932, 1, peer, true);
										auto iscontains = false;
										SearchInventoryItem(peer, 3932, 1, iscontains);
										if (!iscontains) {
											pData->cloth_hand = 0;
											Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
											pData->effect = 8421376;
											sendPuncheffect(peer, pData->effect);
											send_state(peer); //here
											sendClothes(peer);
										}
									}
									SendXP(peer, 30);
									SendParticleEffect(peer, x * 32 + 16, y * 32 + 16, 3, 114, 0);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 5192, 1, 0);
								}
								SendFarmableDrop(peer, 5, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 5174) {
								if (pData->cloth_hand == 3932) {
									if ((rand() % 100) <= 50) {
										Player::OnTalkBubble(peer, pData->netID, "`wThe Rock was so hard to break that it broke your Rock Hammer!", 0, true);
										RemoveInventoryItem(3932, 1, peer, true);
										auto iscontains = false;
										SearchInventoryItem(peer, 3932, 1, iscontains);
										if (!iscontains) {
											pData->cloth_hand = 0;
											Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
											pData->effect = 8421376;
											sendPuncheffect(peer, pData->effect);
											send_state(peer); //here
											sendClothes(peer);
										}
									}
									SendXP(peer, 30);
									SendParticleEffect(peer, x * 32 + 16, y * 32 + 16, 3, 114, 0);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 5194, 1, 0);
								}
								SendFarmableDrop(peer, 5, x, y, world);
								world->items.at(x + (y * world->width)).fire = true;
								if (y + 1 <= 100) world->items.at(x + 1 + (y * world->width)).fire = true;
								if (x - 1 >= 0) world->items.at(x - 1 + (y * world->width)).fire = true;
								if (y - 1 >= 0) world->items.at(x + (y * world->width - 100)).fire = true;
								if (y + 1 <= 60) world->items.at(x + (y * world->width + 100)).fire = true;
								UpdateVisualsForBlock(peer, true, x, y, world);
								if (y + 1 <= 100) UpdateVisualsForBlock(peer, true, x + 1, y, world);
								if (x - 1 >= 0) UpdateVisualsForBlock(peer, true, x - 1, y, world);
								if (y - 1 >= 0) UpdateVisualsForBlock(peer, true, x, y - 1, world);
								if (y + 1 <= 60) UpdateVisualsForBlock(peer, true, x, y + 1, world);
							}
							else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::CHECKPOINT) {
								if (pData->checkx / 32 == x && pData->checky / 32 == y) {
									pData->checkx = 0;
									pData->checky = 0;
									pData->ischeck = false;
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 5176) {
								if (pData->cloth_hand == 3932) {
									if ((rand() % 100) <= 50) {
										Player::OnTalkBubble(peer, pData->netID, "`wThe Rock was so hard to break that it broke your Rock Hammer!", 0, true);
										RemoveInventoryItem(3932, 1, peer, true);
										auto iscontains = false;
										SearchInventoryItem(peer, 3932, 1, iscontains);
										if (!iscontains) {
											pData->cloth_hand = 0;
											Player::PlayAudio(peer, "audio/change_clothes.wav", 0);
											pData->effect = 8421376;
											sendPuncheffect(peer, pData->effect);
											send_state(peer); //here
											sendClothes(peer);
										}
									}
									SendXP(peer, 30);
									SendParticleEffect(peer, x * 32 + 16, y * 32 + 16, 3, 114, 0);
									DropItem(world, peer, -1, x * 32 + rand() % 18, y * 32 + rand() % 18, 5178, 1, 0);
								}
								SendFarmableDrop(peer, 5, x, y, world);
							}
							else if (world->items.at(x + (y * world->width)).foreground == 340)
							{
								ifstream read_entry("config+/farmable_drops/chandelier.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 4584)
							{
								ifstream read_entry("config+/farmable_drops/peppertree.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 526)
							{
								ifstream read_entry("config+/farmable_drops/pinballbumper.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 454)
							{
								ifstream read_entry("config+/farmable_drops/venus.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 5666)
							{
								ifstream read_entry("config+/farmable_drops/lasergrid.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 4762)
							{
								ifstream read_entry("config+/farmable_drops/amethyst.json");
								json j;
								read_entry >> j;
								read_entry.close();
								auto& custom_drop_json = j["gem_drop"];
								custom_drop = (rand() % custom_drop_json) + 1;
							}
							else if (world->items.at(x + (y * world->width)).foreground == 1420 || world->items.at(x + (y * world->width)).foreground == 6214) {
								auto squaresign = x + (y * world->width);
								auto ismannequin = std::experimental::filesystem::exists("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
								if (ismannequin) {
									remove(("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
								}
								world->items.at(x + (y * world->width)).sign = "";
							}
							else if (world->items.at(x + (y * world->width)).foreground == 656) {
								auto squaresign = x + (y * world->width);
								auto isdbox = std::experimental::filesystem::exists("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
								if (isdbox) {
									ifstream ifff("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									json j;
									ifff >> j;
									ifff.close();
									int count = j["inmail"];
									if (j["inmail"] > 0) {
										Player::OnTextOverlay(peer, "`wThere are `5" + to_string(count) + " `wletter(s) in the mailbox.");
									}
									remove(("save/mailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 658) {
								auto squaresign = x + (y * world->width);
								auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
								if (isdbox) {
									remove(("save/bulletinboard/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 1006) {
								auto squaresign = x + (y * world->width);
								auto isdbox = std::experimental::filesystem::exists("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
								if (isdbox) {
									ifstream ifff("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									json j;
									ifff >> j;
									ifff.close();
									int count = j["inmail"];
									if (j["inmail"] > 0) {
										Player::OnTextOverlay(peer, "`wThere are `5" + to_string(count) + " `wletter(s) in the mailbox.");
									}
									remove(("save/bluemailbox/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
								}
							}
							else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::DONATION) {
								auto squaresign = x + (y * world->width);
								auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
								if (isdbox) {
									ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									json j;
									ifff >> j;
									ifff.close();
									if (j["donated"] > 0) {
										Player::OnTextOverlay(peer, "Empty donation box first!");
										return;
									}
									remove(("save/donationboxes/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 1240) {
								world->items.at(x + (y * world->width)).monitorname = "";
								world->items.at(x + (y * world->width)).monitoronline = false;
							}
							else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::GATEWAY) {
								world->items.at(x + (y * world->width)).opened = false;
							}
							else if (getItemDef(world->items.at(x + (y * world->width)).foreground).properties & Property_AutoPickup) {
								bool SuccessBreak = false;
								if (world->items.at(x + (y * world->width)).foreground == 2978) {
									if (world->items.at(x + (y * world->width)).vcount != 0 || world->items.at(x + (y * world->width)).vdraw != 0) {
										Player::OnTalkBubble(peer, pData->netID, "Empty the machine before breaking it!", 0, true);
										return;
									}
									world->items.at(x + (y * world->width)).vcount = 0;
									world->items.at(x + (y * world->width)).vprice = 0;
									world->items.at(x + (y * world->width)).vid = 0;
									world->items.at(x + (y * world->width)).vdraw = 0;
									world->items.at(x + (y * world->width)).opened = false;
									world->items.at(x + (y * world->width)).rm = false;
									auto success = true;
									SaveItemMoreTimes(2978, 1, peer, success);
								}
								else if (world->items.at(x + (y * world->width)).foreground == 6286) {
									auto squaresign = x + (y * world->width);
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									if (isdbox) {
										ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
										json j;
										ifff >> j;
										ifff.close();
										if (j["instorage"] > 0) {
											Player::OnTextOverlay(peer, "Empty the box first!");
											return;
										}
										remove(("save/storageboxlvl1/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
									}
									bool success = true;
									SaveItemMoreTimes(6286, 1, peer, success);
								}
								else if (world->items.at(x + (y * world->width)).foreground == 6288) {
									auto squaresign = x + (y * world->width);
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									if (isdbox) {
										ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
										json j;
										ifff >> j;
										ifff.close();
										if (j["instorage"] > 0) {
											Player::OnTextOverlay(peer, "Empty the box first!");
											return;
										}
										remove(("save/storageboxlvl2/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
									}
									bool success = true;
									SaveItemMoreTimes(6288, 1, peer, success);
								}
								else if (world->items.at(x + (y * world->width)).foreground == 6290) {
									auto squaresign = x + (y * world->width);
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									if (isdbox) {
										ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
										json j;
										ifff >> j;
										ifff.close();
										if (j["instorage"] > 0) {
											Player::OnTextOverlay(peer, "Empty the box first!");
											return;
										}
										remove(("save/storageboxlvl3/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
									}
									bool success = true;
									SaveItemMoreTimes(6290, 1, peer, success);
								}
								else if (world->items.at(x + (y * world->width)).foreground == 8878) {
									auto squaresign = x + (y * world->width);
									auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
									if (isdbox) {
										ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json");
										json j;
										ifff >> j;
										ifff.close();
										if (j["insafe"] > 0) {
											Player::OnTextOverlay(peer, "Empty the safe first!");
											return;
										}
										remove(("save/safevault/_" + pData->currentWorld + "/X" + std::to_string(squaresign) + ".json").c_str());
									}
									bool success = true;
									SaveItemMoreTimes(8878, 1, peer, success);
								}
								else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::LOCK) {
									if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
										world->items.at(x + (y * world->width)).monitorname = "";
										world->items.at(x + (y * world->width)).opened = false;
										auto success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success);
										for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
											if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
											if (isHere(peer, net_peer)) {
												Player::PlayAudio(net_peer, "audio/metal_destroy.wav", 0);
											}
										}
									}
									else {
										if (world->category == "Guild") {
											Player::OnTalkBubble(peer, pData->netID, "Abandon your guild first before breaking the lock!", 0, true);
											return;
										}
										for (auto i = 0; i < world->width * world->height; i++) {
											if (getItemDef(world->items.at(i).foreground).properties & Property_Untradable && world->items.at(i).foreground != 0) {
												Player::OnTalkBubble(peer, pData->netID, "Take all untradeable blocks before breaking the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "", 0, true);
												return;
											}
										}
										if (pData->NickPrefix == "") updateworldremove(peer);
										auto success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success);
										for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
											if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
											if (isHere(peer, net_peer)) {
												Player::OnConsoleMessage(net_peer, "`5[`w" + world->name + " `ohas had its `$" + getItemDef(world->items.at(x + (y * world->width)).foreground).name + " `oremoved!`5]");
												Player::PlayAudio(net_peer, "audio/metal_destroy.wav", 0);
											}
										}
										bool iscontainss = false;
										SearchInventoryItem(peer, 1424, 1, iscontainss);
										if (iscontainss) {
											RemoveInventoryItem(1424, 1, peer, true);
										}
										if (world->owner != pData->rawName) {
											bool PlayerFound = false;
											for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
												if (net_peer->state != ENET_PEER_STATE_CONNECTED) continue;
												if (world->owner == static_cast<PlayerInfo*>(net_peer->data)->rawName) {
													PlayerFound = true;
													static_cast<PlayerInfo*>(net_peer->data)->worldsowned.erase(std::remove(static_cast<PlayerInfo*>(net_peer->data)->worldsowned.begin(), static_cast<PlayerInfo*>(net_peer->data)->worldsowned.end(), static_cast<PlayerInfo*>(net_peer->data)->currentWorld), static_cast<PlayerInfo*>(net_peer->data)->worldsowned.end());
													break;
												}
											} if (!PlayerFound) {
												try {
													ifstream read_player("save/players/_" + world->owner + ".json");
													if (!read_player.is_open()) {
														return;
													}
													json j;
													read_player >> j;
													read_player.close();
													string WorldOwned = j["worldsowned"];
													vector<string> editworldsowned;
													stringstream ssfs(WorldOwned);
													while (ssfs.good()) {
														string substr;
														getline(ssfs, substr, ',');
														if (substr.size() == 0) continue;
														editworldsowned.push_back(substr);
													}
													editworldsowned.erase(std::remove(editworldsowned.begin(), editworldsowned.end(), pData->currentWorld), editworldsowned.end());
													string worldstring = "";
													for (int i = 0; i < editworldsowned.size(); i++) {
														worldstring += editworldsowned.at(i) + ",";
													}
													j["worldsowned"] = worldstring;
													ofstream write_player("save/players/_" + world->owner + ".json");
													write_player << j << std::endl;
													write_player.close();
												}
												catch (std::exception& e) {
													std::cout << e.what() << std::endl;
													return;
												}
											}
										}
										else {
											pData->worldsowned.erase(std::remove(pData->worldsowned.begin(), pData->worldsowned.end(), pData->currentWorld), pData->worldsowned.end());
										}
										world->rainbow = false;
										world->entrylevel = 1;
										world->owner = "";
										world->isPublic = false;
										world->accessed.clear();
										world->silence = false;
										world->publicBlock = -1;
										world->DisableDrop = false;
									}
									if (world->items.at(x + (y * world->width)).foreground == 4802) {
										for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												send_rainbow_shit_data(currentPeer, world->items.at(x + (y * world->width)).foreground, world->items.at(x + (y * world->width)).background, x, y, false, -1);
											}
										}
									}
								}
								else if (world->items.at(x + (y * world->width)).foreground == 6954) {
									if (world->items.at(x + (y * world->width)).mc != 0) {
										Player::OnTalkBubble(peer, pData->netID, "Empty the machine before breaking it!", 0, true);
										return;
									}
									world->items.at(x + (y * world->width)).mc = 0;
									world->items.at(x + (y * world->width)).mid = 0;
									world->items.at(x + (y * world->width)).vid = 0;
									world->items.at(x + (y * world->width)).rm = false;
									auto success = true;
									SaveItemMoreTimes(6954, 1, peer, success, "");
								}
								else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::WEATHER || world->items.at(x + (y * world->width)).foreground == 3694 || world->items.at(x + (y * world->width)).foreground == 3832 || world->items.at(x + (y * world->width)).foreground == 5000) {
									auto success = true;
									SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success, "");
									if (world->weather != 0) {
										world->weather = 0;
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnSetCurrentWeather(currentPeer, 0);
											}
										}
									}
									world->items.at(x + (y * world->width)).vid = 0;
									world->items.at(x + (y * world->width)).vprice = 0;
									world->items.at(x + (y * world->width)).vcount = 0;
									world->items.at(x + (y * world->width)).intdata = 0;
									world->items.at(x + (y * world->width)).mc = 0;
									world->items.at(x + (y * world->width)).rm = false;
									world->items.at(x + (y * world->width)).opened = false;
									world->items.at(x + (y * world->width)).activated = false;
								}
								else if (getItemDef(world->items.at(x + (y * world->width)).foreground).blockType == BlockTypes::SUCKER) {
									if (world->items.at(x + (y * world->width)).mid != 0 && world->items.at(x + (y * world->width)).mc != 0) {
										if (world->items.at(x + (y * world->width)).mc == 0) {
											world->items.at(x + (y * world->width)).mc = 0;
											world->items.at(x + (y * world->width)).mid = 0;
											world->items.at(x + (y * world->width)).rm = false;
											//world->items.at(x + (y * world->width)).foreground = 0;
											auto success = true;
											SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success, "");
										}
										else {
											Player::OnTalkBubble(peer, pData->netID, "Empty the " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + " before breaking it!", 0, true);
											return;
										}
									}
									else {
										world->items.at(x + (y * world->width)).mc = 0;
										world->items.at(x + (y * world->width)).mid = 0;
										world->items.at(x + (y * world->width)).rm = false;
										//world->items.at(x + (y * world->width)).foreground = 0;
										auto success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success, "");
									}
								}
								else {
									auto success = true;
									SaveItemMoreTimes(world->items.at(x + (y * world->width)).foreground, 1, peer, success, "");
									world->items.at(x + (y * world->width)).mc = 0;
									world->items.at(x + (y * world->width)).mid = 0;
									world->items.at(x + (y * world->width)).rm = false;
									world->items.at(x + (y * world->width)).vcount = 0;
									world->items.at(x + (y * world->width)).vprice = 0;
									world->items.at(x + (y * world->width)).vid = 0;
									world->items.at(x + (y * world->width)).vdraw = 0;
									world->items.at(x + (y * world->width)).monitorname = "";
									world->items.at(x + (y * world->width)).monitoronline = false;
									world->items.at(x + (y * world->width)).opened = false;
									world->items.at(x + (y * world->width)).intdata = 0;
									//world->items.at(x + (y * world->width)).foreground = 0;
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 3528) {
								if (world->items.at(x + (y * world->width)).intdata != 0) {
									world->items.at(x + (y * world->width)).intdata = 0;
								}
							}
							else if (world->items.at(x + (y * world->width)).foreground == 2946) {
								if (world->items.at(x + (y * world->width)).intdata != 0) {
									auto success = true;
									SaveItemMoreTimes(world->items.at(x + (y * world->width)).intdata, 1, peer, success, pData->rawName + " from break " + getItemDef(world->items.at(x + (y * world->width)).foreground).name + "");
									Player::OnTalkBubble(peer, pData->netID, "You picked up 1 " + getItemDef(world->items.at(x + (y * world->width)).intdata).name + ".", 0, true);
									world->items.at(x + (y * world->width)).intdata = 0;
								}
							}
							if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity != 999 && world->items.at(x + (y * world->width)).foreground != 0) {
								SendTileData(world, peer, x, y, data.punchX, data.punchY, custom_drop);
								custom_drop = 0;
							}
							if (!isWorldOwner(peer, world) && !isWorldAdmin(peer, world) && admin(peer) && world->owner != "") {
							}
							world->items.at(x + (y * world->width)).foreground = 0;
						}
						else {
							if (getItemDef(world->items.at(x + (y * world->width)).background).rarity != 999 && world->items.at(x + (y * world->width)).background != 0 && tile == 18) {
								if (getItemDef(world->items.at(x + (y * world->width)).background).properties & Property_Dropless) return;
								SendTileData(world, peer, x, y, data.punchX, data.punchY);
							}
							SendDropSeed(world, peer, x, y, world->items.at(x + (y * world->width)).background);
							world->items.at(x + (y * world->width)).background = 0;
							if (world->items.at(x + (y * world->width)).foreground == 1008 || world->items.at(x + (y * world->width)).foreground == 1796 || world->items.at(x + (y * world->width)).foreground == 242 || world->items.at(x + (y * world->width)).foreground == 9290 || world->items.at(x + (y * world->width)).foreground == 8470 || world->items.at(x + (y * world->width)).foreground == 8 || world->items.at(x + (y * world->width)).foreground == 9308) {
								world->items.at(x + (y * world->width)).foreground = 0;
							}
						}
						if (pData->quest_active && pData->lastquest == "honor" && pData->quest_step == 3 && pData->quest_progress < 5000) {
							pData->quest_progress++;
							if (pData->quest_progress >= 5000) {
								pData->quest_progress = 5000;
								Player::OnTalkBubble(peer, pData->netID, "`9Legendary Quest step complete! I'm off to see a Wizard!", 0, false);
							}
						}
					}
				}
				else {
					if (y < world->height) {
						world->items.at(x + (y * world->width)).breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
						world->items.at(x + (y * world->width)).breakLevel += 4;
					}
				}
			}
		}
		else {
			if (world->items.at(x + (y * world->width)).foreground != 0 && getItemDef(tile).blockType != BlockTypes::BACKGROUND && getItemDef(tile).blockType != BlockTypes::GROUND_BLOCK) return;
			if (!isWorldOwner(peer, world) && !isWorldAdmin(peer, world) && admin(peer) && world->owner != "") {
			}
			for (auto i = 0; i < pData->inventory.items.size(); i++) {
				if (pData->inventory.items.at(i).itemID == tile) {
					if (static_cast<unsigned int>(pData->inventory.items.at(i).itemCount) > 1) {
						pData->inventory.items.at(i).itemCount--;
						pData->needsaveinventory = true;
					}
					else {
						pData->inventory.items.erase(pData->inventory.items.begin() + i);
						pData->needsaveinventory = true;
					}
					break;
				}
			}
			if (tile != 18 && tile != 32 && getItemDef(tile).blockType != BlockTypes::CONSUMABLE) {
				if (triple_place.find(pData->cloth_hand) != triple_place.end() || triple_place.find(pData->cloth_hair) != triple_place.end() || triple_place.find(pData->cloth_back) != triple_place.end() || triple_place.find(pData->cloth_necklace) != triple_place.end()) {
					SendPlacingEffect(peer, data.punchX, data.punchY, 229);
				} if (pData->cloth_hand == 9488) {
					SendPlacingEffect(peer, data.punchX, data.punchY, 150);
				} if (pData->cloth_back == 9152) {
					SendPlacingEffect(peer, data.punchX, data.punchY, 125);
				}
			}
			if (getItemDef(tile).blockType == BlockTypes::BACKGROUND || getItemDef(tile).blockType == BlockTypes::GROUND_BLOCK) {
				world->items.at(x + (y * world->width)).background = tile;
			}
			else if (getItemDef(tile).blockType == BlockTypes::SEED) {
				world->items.at(x + (y * world->width)).foreground = tile;
			}
			else {
				world->items.at(x + (y * world->width)).foreground = tile;
			}
			world->items.at(x + (y * world->width)).breakLevel = 0;
		}
		if (!Explosion) {
			for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					auto raw = packPlayerMoving(&data);
					raw[2] = dicenr;
					raw[3] = dicenr;
					SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
				}
			}
		} if (getItemDef(tile).properties & Property_MultiFacing) {
			if (pData->isRotatedLeft) {
				world->items.at(x + (y * world->width)).flipped = true;
				UpdateBlockState(peer, x, y, true, world);
			}
		} if (VendUpdate) {
			if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
				UpdateVend(peer, x, y, 0, false, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
			}
			else {
				UpdateVend(peer, x, y, world->items.at(x + (y * world->width)).vid, false, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
			}
		} if (isLock) {
			send_tile_data(peer, x, y, 0x10, world->items.at(x + (y * world->width)).foreground, world->items.at(x + (y * world->width)).background, lock_tile_datas(0x20, ((PlayerInfo*)(peer->data))->netID, 0, 0, false, 100));
		} if (isMannequin) {
			updateMannequin(peer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).sign, 0, 0, 0, 0, 0, 0, 0, 0, 0, true, 0);
		} if (isSmallLock) {
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					if (world->items.at(x + (y * world->width)).monitorname == pData->rawName) apply_lock_packet(world, currentPeer, x, y, world->items.at(x + (y * world->width)).foreground, pData->netID);
					else apply_lock_packet(world, currentPeer, x, y, world->items.at(x + (y * world->width)).foreground, -1);
				}
			}
		}
		if (isTree)
		{
			ifstream read_entry("config+/farmable_drops/tree_drop/rarity.json");
			json j;
			read_entry >> j;
			read_entry.close();
			int rarity1 = j["rarity_1-14"];
			int rarity15 = j["rarity_15-24"];
			int rarity25 = j["rarity_25-34"];
			int rarity35 = j["rarity_35-44"];
			int rarity45 = j["rarity_45-54"];
			int rarity55 = j["rarity_55-64"];
			int rarity65 = j["rarity_65-74"];
			int rarity75 = j["rarity_75-84"];
			int rarity85 = j["rarity_85-99"];
			int rarity100 = j["rarity_100"];
			if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 1 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 15) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity1 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 15 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 25) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity15 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 25 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 35) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity25 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 35 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 45) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity35 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 45 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 55) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity45 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 55 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 65) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity55 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 65 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 75) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity65 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 75 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 85) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity75 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity >= 85 && getItemDef(world->items.at(x + (y * world->width)).foreground).rarity < 99) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity85 + 1;
			}
			else if (getItemDef(world->items.at(x + (y * world->width)).foreground).rarity <= 99) {
				world->items.at(x + (y * world->width)).fruitcount = rand() % rarity100 + 1;
			}
			UpdateTreeVisuals(peer, world->items[x + (y * world->width)].foreground, x, y, world->items[x + (y * world->width)].background, world->items.at(x + (y * world->width)).fruitcount, 999999999999, true, 0);
		}
		if (isScience) {
			world->items.at(x + (y * world->width)).growtime = (GetCurrentTimeInternalSeconds() + getItemDef(world->items.at(x + (y * world->width)).foreground).growTime);
		} if (isHeartMonitor) {
			world->items.at(x + (y * world->width)).monitorname = pData->displayName;
			world->items.at(x + (y * world->width)).monitoronline = true;
			sendHMonitor(peer, x, y, pData->displayName, true, world->items.at(x + (y * world->width)).background);
		} if (isgateway) {
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					if (admin(currentPeer) || isWorldOwner(currentPeer, world) || isWorldAdmin(currentPeer, world)) {
						update_entrance(currentPeer, world->items.at(x + (y * world->width)).foreground, x, y, true, world->items.at(x + (y * world->width)).background);
					}
					else {
						update_entrance(currentPeer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).opened, world->items.at(x + (y * world->width)).background);
					}
				}
			}
		} if (isDoor) {
			updateDoor(peer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).background, getItemDef(world->items.at(x + (y * world->width)).foreground).name, false, true);
		} if (isMagplant) {
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					send_item_sucker(currentPeer, world->items.at(x + (y * world->width)).foreground, x, y, world->items.at(x + (y * world->width)).mid, -1, true, true, world->items.at(x + (y * world->width)).background);
				}
			}
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
}

inline void sendWorld(ENetPeer* peer, WorldInfo* worldInfo) {
	try {
		auto zero = 0;
		static_cast<PlayerInfo*>(peer->data)->joinClothesUpdated = false;
		string asdf = "0400000004A7379237BB2509E8E0EC04F8720B050000000000000000FBBB0000010000007D920100FDFDFDFD04000000040000000000000000000000070000000000";
		string worldName = worldInfo->name;
		auto xSize = worldInfo->width;
		auto ySize = worldInfo->height;
		auto square = xSize * ySize;
		auto nameLen = static_cast<__int16_t>(worldName.length());
		int payloadLen = asdf.length() / 2;
		auto dataLen = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 100;
		auto offsetData = dataLen - 100;
		int allocMem = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 640000 + 100 + (worldInfo->droppedItems.size() * 20);
		auto data = new BYTE[allocMem];
		memset(data, 0, allocMem);
		for (auto i = 0; i < asdf.length(); i += 2) {
			char x = ch2n(asdf.at(i));
			x = x << 4;
			x += ch2n(asdf[i + 1]);
			memcpy(data + (i / 2), &x, 1);
		}
		__int16_t item = 0;
		auto smth = 0;
		for (auto i = 0; i < square * 8; i += 4) memcpy(data + payloadLen + i + 14 + nameLen, &zero, 4);
		for (auto i = 0; i < square * 8; i += 8) memcpy(data + payloadLen + i + 14 + nameLen, &item, 2);
		memcpy(data + payloadLen, &nameLen, 2);
		memcpy(data + payloadLen + 2, worldName.c_str(), nameLen);
		memcpy(data + payloadLen + 2 + nameLen, &xSize, 4);
		memcpy(data + payloadLen + 6 + nameLen, &ySize, 4);
		memcpy(data + payloadLen + 10 + nameLen, &square, 4);
		BYTE* blockPtr = data + payloadLen + 14 + nameLen;
		auto sizeofblockstruct = 8;
		for (auto i = 0; i < square; i++) {
			int tile = worldInfo->items.at(i).foreground;
			sizeofblockstruct = 8;
			auto type = 0x00000000;
			if (worldInfo->items.at(i).activated) type |= 0x00400000;
			if (worldInfo->items.at(i).flipped) type |= 0x00200000;
			if (worldInfo->items.at(i).water) type |= 0x04000000;
			if (worldInfo->items.at(i).glue) type |= 0x08000000;
			if (worldInfo->items.at(i).fire) type |= 0x10000000;
			if (worldInfo->items.at(i).red) type |= 0x20000000;
			if (worldInfo->items.at(i).green) type |= 0x40000000;
			if (worldInfo->items.at(i).blue) type |= 0x80000000;
			switch (tile) {
			case 6:
			{
				memcpy(blockPtr, &tile, 2);
				memcpy(blockPtr + 4, &type, 4);
				BYTE btype = 1;
				memcpy(blockPtr + 8, &btype, 1);
				string doorText = "EXIT";
				auto doorTextChars = doorText.c_str();
				auto length = static_cast<short>(doorText.size());
				memcpy(blockPtr + 9, &length, 2);
				memcpy(blockPtr + 11, doorTextChars, length);
				sizeofblockstruct += 4 + length;
				dataLen += 4 + length;
				break;
			}
			case 2946:
			{
				memcpy(blockPtr, &worldInfo->items.at(i).foreground, 2);
				memcpy(blockPtr + 4, &type, 4);
				BYTE btype = 0x17;
				memcpy(blockPtr + 8, &btype, 1);
				auto item1 = worldInfo->items.at(i).intdata;
				memcpy(blockPtr + 9, &item1, 4);
				sizeofblockstruct += 5;
				dataLen += 5;
				break;
			}
			case 1632: case 8196: case 10450: case 5116: case 6414: case 6212: case 3044: case 1636: case 1008: case 2798: case 1044: case 872: case 866: case 3888: case 928:
			{
				BYTE btype = 9;
				auto timeIntoGrowth = getItemDef(worldInfo->items.at(i).foreground).growTime - calcBanDuration(worldInfo->items.at(i).growtime);
				memcpy(blockPtr, &worldInfo->items.at(i).foreground, 2);
				memcpy(blockPtr + 4, &type, 4);
				memcpy(blockPtr + 8, &btype, 1);
				memcpy(blockPtr + 9, &timeIntoGrowth, 4);
				sizeofblockstruct += 5;
				dataLen += 5;
				break;
			}
			default:
			{
				if ((worldInfo->items.at(i).foreground == 0) || (worldInfo->items.at(i).foreground == 2) || (worldInfo->items.at(i).foreground == 8) || (worldInfo->items.at(i).foreground == 100) || (worldInfo->items.at(i).foreground == 4) || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::FOREGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::BACKGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::TOGGLE_FOREGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::CHEMICAL_COMBINER || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::CHEST || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::SWITCH_BLOCK) {
					memcpy(blockPtr, &worldInfo->items.at(i).foreground, 2);
					memcpy(blockPtr + 4, &type, 4);
				}
				else {
					memcpy(blockPtr, &zero, 2);
				}
				break;
			}
			}
			memcpy(blockPtr + 2, &worldInfo->items.at(i).background, 2);
			blockPtr += sizeofblockstruct;
		}
		dataLen += 8;
		int itemcount = worldInfo->droppedItems.size();
		auto itemuid = worldInfo->droppedCount;
		memcpy(blockPtr, &itemcount, 4);
		memcpy(blockPtr + 4, &itemuid, 4);
		blockPtr += 8;
		auto iteminfosize = 16;
		for (auto i = 0; i < itemcount; i++) {
			auto item1 = worldInfo->droppedItems.at(i).id;
			auto count = worldInfo->droppedItems.at(i).count;
			auto uid = worldInfo->droppedItems.at(i).uid + 1;
			auto x = static_cast<float>(worldInfo->droppedItems.at(i).x);
			auto y = static_cast<float>(worldInfo->droppedItems.at(i).y);
			memcpy(blockPtr, &item1, 2);
			memcpy(blockPtr + 2, &x, 4);
			memcpy(blockPtr + 6, &y, 4);
			memcpy(blockPtr + 10, &count, 2);
			memcpy(blockPtr + 12, &uid, 4);
			blockPtr += iteminfosize;
			dataLen += iteminfosize;
		}
		dataLen += 100;
		blockPtr += 4;
		memcpy(blockPtr, &worldInfo->weather, 4);
		blockPtr += 4;
		offsetData = dataLen - 100;
		if (dataLen > 640000) {
			ColoredText("World dataLen is too big to handle " + to_string(dataLen) + " in world " + worldInfo->name, "red");
			return;
		}
		auto data2 = new BYTE[101];
		memset(data2, 0, 101);
		memcpy(data2 + 0, &zero, 4);
		auto weather = worldInfo->weather;
		memcpy(data2 + 4, &weather, 4);
		memcpy(data + dataLen - 4, &smth, 4);
		auto packet2 = enet_packet_create(data, dataLen, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet2);
		static_cast<PlayerInfo*>(peer->data)->currentWorld = worldInfo->name;
		for (auto i = 0; i < square; i++)
		{
			switch (worldInfo->items.at(i).foreground)
			{
			case 3528: //Painting Easel
			{
				int xx = i % xSize, yy = i / xSize;
				send_canvas_data(peer, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, xx, yy, worldInfo->items.at(i).intdata, getItemDef(worldInfo->items.at(i).intdata).name);
				break;
			}
			case 6952:
			{
				if (worldInfo->items.at(i).mid != 0) send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, 1, false, false, worldInfo->items.at(i).background);
				else send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, 0, false, false, worldInfo->items.at(i).background);
				break;
			}
			case 6954:
			{
				if (worldInfo->items.at(i).mid > 0 && worldInfo->items.at(i).mc > 0) {
					send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, 1, true, true, worldInfo->items.at(i).background);
				}
				else {
					send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, -1, true, true, worldInfo->items.at(i).background);
				}
				break;
			}
			case 5638: case 6946: case 6948: //MAGPLANTS
			{
				if (worldInfo->items.at(i).mc <= 0) {
					send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, 0, true, true, worldInfo->items.at(i).background);
				}
				else if (worldInfo->items.at(i).mc >= 5000 && worldInfo->items.at(i).mid != 112) {
					send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, -1, true, true, worldInfo->items.at(i).background);
				}
				else {
					send_item_sucker(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).mid, 1, true, true, worldInfo->items.at(i).background);
				}
				break;
			}
			case 2978: //Vending Machine
			{
				auto islocks = false;
				if (worldInfo->items.at(i).vdraw >= 1) {
					islocks = true;
				}
				if (worldInfo->items.at(i).vcount == 0 && worldInfo->items.at(i).vprice == 0 && worldInfo->items.at(i).vid != 0) {
					UpdateVend(peer, i % worldInfo->width, i / worldInfo->width, 0, islocks, 0, worldInfo->items.at(i).background, worldInfo->items.at(i).opened);
				}
				else if (worldInfo->items.at(i).opened && worldInfo->items.at(i).vcount < worldInfo->items.at(i).vprice) {
					UpdateVend(peer, i % worldInfo->width, i / worldInfo->width, 0, islocks, worldInfo->items.at(i).vprice, worldInfo->items.at(i).background, worldInfo->items.at(i).opened);
				}
				else UpdateVend(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).vid, islocks, worldInfo->items.at(i).vprice, worldInfo->items.at(i).background, worldInfo->items.at(i).opened);
				break;
			}
			case 1420: case 6214: //Mannequin
			{
				bool sent = false;
				auto ismannequin = std::experimental::filesystem::exists("save/mannequin/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
				if (ismannequin)
				{
					sent = true;
					json j;
					ifstream fs("save/mannequin/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
					fs >> j;
					fs.close();
					auto x = i % worldInfo->width;
					auto y = i / worldInfo->width;
					//0x00200000
					updateMannequin(peer, worldInfo->items.at(i).foreground, x, y, worldInfo->items.at(i).background, worldInfo->items.at(i).sign, atoi(j["clothHair"].get<string>().c_str()), atoi(j["clothHead"].get<string>().c_str()), atoi(j["clothMask"].get<string>().c_str()), atoi(j["clothHand"].get<string>().c_str()), atoi(j["clothNeck"].get<string>().c_str()), atoi(j["clothShirt"].get<string>().c_str()), atoi(j["clothPants"].get<string>().c_str()), atoi(j["clothFeet"].get<string>().c_str()), atoi(j["clothBack"].get<string>().c_str()), false, 0);
				}
				if (!sent) {
					PlayerMoving moving{};
					moving.packetType = 0x3;
					moving.characterState = 0x0;
					moving.x = i % worldInfo->width;
					moving.y = static_cast<float>(i) / worldInfo->height;
					moving.punchX = i % worldInfo->width;
					moving.punchY = i / worldInfo->width;
					moving.XSpeed = 0;
					moving.YSpeed = 0;
					moving.netID = -1;
					moving.plantingTree = worldInfo->items.at(i).foreground;
					SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
				}
				break;
			}
			case 1006: //Blue Mailbox
			{
				bool sent = false;
				auto isbluemail = std::experimental::filesystem::exists("save/bluemailbox/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
				if (isbluemail)
				{
					ifstream ifff("save/bluemailbox/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
					json basic_json;
					ifff >> basic_json;
					ifff.close();
					if (basic_json["inmail"] > 0)
					{
						auto x = i % worldInfo->width;
						auto y = i / worldInfo->width;
					}
				}
				if (!sent) {
					PlayerMoving moving{};
					moving.packetType = 0x3;
					moving.characterState = 0x0;
					moving.x = i % worldInfo->width;
					moving.y = static_cast<float>(i) / worldInfo->height;
					moving.punchX = i % worldInfo->width;
					moving.punchY = i / worldInfo->width;
					moving.XSpeed = 0;
					moving.YSpeed = 0;
					moving.netID = -1;
					moving.plantingTree = worldInfo->items.at(i).foreground;
					SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
				}
				break;
			}
			case 656: //Mailbox
			{
				bool sent = false;
				auto ismailbox = std::experimental::filesystem::exists("save/mailbox/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
				if (ismailbox)
				{
					ifstream ifff("save/mailbox/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
					json basic_json;
					ifff >> basic_json;
					ifff.close();
					if (basic_json["inmail"] > 0)
					{
						/*removed because started to crash when entering worlds*/
						//sent = true;
						auto x = i % worldInfo->width;
						auto y = i / worldInfo->width;
						//SendItemPacket(peer, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, x, y, 1);
					}
				}
				if (!sent) {
					PlayerMoving moving{};
					moving.packetType = 0x3;
					moving.characterState = 0x0;
					moving.x = i % worldInfo->width;
					moving.y = static_cast<float>(i) / worldInfo->height;
					moving.punchX = i % worldInfo->width;
					moving.punchY = i / worldInfo->width;
					moving.XSpeed = 0;
					moving.YSpeed = 0;
					moving.netID = -1;
					moving.plantingTree = worldInfo->items.at(i).foreground;
					SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
				}
				break;
			}
			case 1240: //Heart Monitor
			{
				bool found = false;
				for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->displayName == worldInfo->items.at(i).monitorname) {
						found = true;
						sendHMonitor(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).monitorname, true, worldInfo->items.at(i).background);
						break;
					}
				}
				if (!found) sendHMonitor(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).monitorname, false, worldInfo->items.at(i).background);
				break;
			}
			case 0: case 6: case 2946: case 8196: case 10450: case 1008: case 866: case 6414: case 6212: case 5116: case 4858: case 3888: case 3044: case 2798: case 1632: case 1636: case 1044: case 928: case 872: case 4: case 2: case 8: case 100:
			{
				if (worldInfo->items.at(i).destroy && worldInfo->items.at(i).foreground != 0) {
					for (int asd = 0; asd < getItemDef(worldInfo->items.at(i).foreground).breakHits; asd++) {
						if (worldInfo->items.at(i).foreground != 0) sendTileUpdate(i % worldInfo->width, i / worldInfo->width, 18, -1, peer, worldInfo);
					}
					worldInfo->items.at(i).destroy = false;
				}
				break;
			}
			default:
			{
				if (worldInfo->items.at(i).foreground == 3694 && worldInfo->items.at(i).activated) {
					sendHeatwave(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).vid, worldInfo->items.at(i).vprice, worldInfo->items.at(i).vcount);
					Player::OnSetCurrentWeather(peer, 28);
					break;
				}
				else if (worldInfo->items.at(i).foreground == 3832 && worldInfo->items.at(i).activated) {
					sendStuffweather(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).intdata, worldInfo->items.at(i).mc, worldInfo->items.at(i).rm, worldInfo->items.at(i).opened);
					Player::OnSetCurrentWeather(peer, 29);
					break;
				}
				else if (worldInfo->items.at(i).foreground == 5000 && worldInfo->items.at(i).activated) {
					sendBackground(peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).intdata);
					Player::OnSetCurrentWeather(peer, 34);
					break;
				}
				else if (worldInfo->items.at(i).destroy && worldInfo->items.at(i).foreground != 0) {
					for (int asd = 0; asd <= getItemDef(worldInfo->items.at(i).foreground).breakHits; asd++) {
						if (worldInfo->items.at(i).foreground != 0) sendTileUpdate(i % worldInfo->width, i / worldInfo->width, 18, -1, peer, worldInfo);
					}
					worldInfo->items.at(i).destroy = false;
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::SIGN || worldInfo->items.at(i).foreground == 1420 || worldInfo->items.at(i).foreground == 6124) {
					UpdateMessageVisuals(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).sign, worldInfo->items.at(i).background);
					break;
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::FOREGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::BACKGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::CHEST || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::SWITCH_BLOCK || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::TOGGLE_FOREGROUND || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::CHEMICAL_COMBINER || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::SIGN || getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::PORTAL) {
					break;
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::DONATION) {
					bool sent = false;
					auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
					if (isdbox)
					{
						ifstream ifff("save/donationboxes/_" + static_cast<PlayerInfo*>(peer->data)->currentWorld + "/X" + std::to_string(i) + ".json");
						json basic_json;
						ifff >> basic_json;
						ifff.close();
					}
					if (!sent) {
						PlayerMoving moving{};
						moving.packetType = 0x3;
						moving.characterState = 0x0;
						moving.x = i % worldInfo->width;
						moving.y = static_cast<float>(i) / worldInfo->height;
						moving.punchX = i % worldInfo->width;
						moving.punchY = i / worldInfo->width;
						moving.XSpeed = 0;
						moving.YSpeed = 0;
						moving.netID = -1;
						moving.plantingTree = worldInfo->items.at(i).foreground;
						SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
					}
					break;
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::SEED) {
					int growTimeSeed = getItemDef(worldInfo->items.at(i).foreground).rarity * getItemDef(worldInfo->items.at(i).foreground).rarity * getItemDef(worldInfo->items.at(i).foreground).rarity;
					growTimeSeed += 160 * getItemDef(worldInfo->items.at(i).foreground).rarity;
					UpdateTreeVisuals(peer, worldInfo->items.at(i).foreground, i % xSize, i / xSize, worldInfo->items.at(i).background, worldInfo->items.at(i).fruitcount, growTimeSeed - calcBanDuration(worldInfo->items.at(i).growtime), true, 0);
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::GATEWAY) {
					if (admin(peer) || isWorldOwner(peer, worldInfo) || isWorldAdmin(peer, worldInfo)) {
						update_entrance(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, true, worldInfo->items.at(i).background);
					}
					else {
						update_entrance(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).opened, worldInfo->items.at(i).background);
					}
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::DOOR) {
					updateDoor(peer, worldInfo->items.at(i).foreground, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).background, worldInfo->items.at(i).label == "" ? (worldInfo->items.at(i).destId == "" ? worldInfo->items.at(i).destWorld : worldInfo->items.at(i).destWorld + "...") : worldInfo->items.at(i).label, false, false);
				}
				else if (getItemDef(worldInfo->items.at(i).foreground).blockType == BlockTypes::LOCK) {
					if (worldInfo->items.at(i).foreground == 202 || worldInfo->items.at(i).foreground == 204 || worldInfo->items.at(i).foreground == 206 || worldInfo->items.at(i).foreground == 4994) {
						if (worldInfo->items.at(i).monitorname == static_cast<PlayerInfo*>(peer->data)->rawName) apply_lock_packet(worldInfo, peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).foreground, static_cast<PlayerInfo*>(peer->data)->netID);
						else if (worldInfo->items.at(i).opened) apply_lock_packet(worldInfo, peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).foreground, -3);
						else apply_lock_packet(worldInfo, peer, i % worldInfo->width, i / worldInfo->width, worldInfo->items.at(i).foreground, -1);
					}
					else if (isWorldOwner(peer, worldInfo)) send_tile_data(peer, i % worldInfo->width, i / worldInfo->width, 0x10, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, lock_tile_datas(0x20, static_cast<PlayerInfo*>(peer->data)->netID, 0, 0, false, 100));
					else if (isWorldAdmin(peer, worldInfo)) send_tile_data(peer, i % worldInfo->width, i / worldInfo->width, 0x10, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, lock_tile_datas(0x20, static_cast<PlayerInfo*>(peer->data)->netID, 0, 0, true, 100));
					else {
						PlayerMoving moving{};
						moving.packetType = 0x3;
						moving.characterState = 0x0;
						moving.x = i % worldInfo->width;
						moving.y = static_cast<float>(i) / worldInfo->height;
						moving.punchX = i % worldInfo->width;
						moving.punchY = i / worldInfo->width;
						moving.XSpeed = 0;
						moving.YSpeed = 0;
						moving.netID = -1;
						moving.plantingTree = worldInfo->items.at(i).foreground;
						SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
					}
					if (worldInfo->items.at(i).foreground == 4802 && worldInfo->rainbow && isWorldOwner(peer, worldInfo)) {
						send_rainbow_shit_data(peer, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, i % worldInfo->width, i / worldInfo->width, true, static_cast<PlayerInfo*>(peer->data)->netID);
						for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer) && !isWorldOwner(currentPeer, worldInfo)) {
								send_rainbow_shit_data(currentPeer, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, i % worldInfo->width, i / worldInfo->width, true, static_cast<PlayerInfo*>(peer->data)->netID);
							}
						}
					}
					else if (worldInfo->items.at(i).foreground == 4802 && !isWorldOwner(peer, worldInfo)) {
						for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							if (isHere(peer, currentPeer) && isWorldOwner(currentPeer, worldInfo)) {
								send_rainbow_shit_data(peer, worldInfo->items.at(i).foreground, worldInfo->items.at(i).background, i % worldInfo->width, i / worldInfo->width, true, static_cast<PlayerInfo*>(currentPeer->data)->netID);
								break;
							}
						}
					}
				}
				else {
					PlayerMoving moving{};
					moving.packetType = 0x3;
					moving.characterState = 0x0;
					moving.x = i % worldInfo->width;
					moving.y = static_cast<float>(i) / worldInfo->height;
					moving.punchX = i % worldInfo->width;
					moving.punchY = i / worldInfo->width;
					moving.XSpeed = 0;
					moving.YSpeed = 0;
					moving.netID = -1;
					moving.plantingTree = worldInfo->items.at(i).foreground;
					SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
				}
				break;
			}
			}
		}
		static_cast<PlayerInfo*>(peer->data)->lastnormalworld = static_cast<PlayerInfo*>(peer->data)->currentWorld;
		ifstream ifs("save/gemdb/_" + static_cast<PlayerInfo*>(peer->data)->rawName + ".txt");
		string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
		auto gembux = atoi(content.c_str());
		Player::OnSetBux(peer, gembux, 1);
		Player::PlayAudio(peer, "audio/door_open.wav", 0);
		delete[] data;
		delete[] data2;
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
}

inline void joinWorld(WorldInfo info, ENetPeer* peer, string act, int x2, int y2) {
	try {
		if (static_cast<PlayerInfo*>(peer->data)->lastJoinReq + 8000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
			static_cast<PlayerInfo*>(peer->data)->lastJoinReq = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
		}
		else {
			Player::OnConsoleMessage(peer, "`4Anti-Lag `o>> Cooldown, wait 8 seconds!");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		string upsd = act;
		transform(upsd.begin(), upsd.end(), upsd.begin(), ::toupper);
		if (static_cast<PlayerInfo*>(peer->data)->isCursed) act = "HELL";
		if (act.length() > 24 || act.length() < 0) {
			Player::OnConsoleMessage(peer, "`4Sorry, but world names with more than 24 characters are not allowed!");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		else if (static_cast<PlayerInfo*>(peer->data)->haveGrowId) {
			DailyRewardCheck(peer);
			auto iscontains = false;
			SearchInventoryItem(peer, 6336, 1, iscontains);
			if (!iscontains) {
				auto success = true;
				SaveItemMoreTimes(6336, 1, peer, success, static_cast<PlayerInfo*>(peer->data)->rawName + " from system");
			}
			auto iscontainss = false;
			SearchInventoryItem(peer, 5640, 1, iscontainss);
			if (iscontainss) {
				static_cast<PlayerInfo*>(peer->data)->magplantitemid = 0;
				RemoveInventoryItem(5640, 1, peer, true);
			}
			iscontainss = false;
			SearchInventoryItem(peer, 1424, 1, iscontainss);
			if (iscontainss) {
				RemoveInventoryItem(1424, 1, peer, true);
			}
		}
		static_cast<PlayerInfo*>(peer->data)->netID = cId;
		int x = 3040;
		int y = 736;
		string Definitions = " `0[";
		if (info.category != "None") {
			if (info.category == "Guild") {
				Definitions += "`2GUILD";
			}
			else {
				Definitions += "`9" + info.category;
			}
		}
		for (auto jss = 0; jss < info.width * info.height; jss++) {
			if (jammers) {
				if (info.items.at(jss).foreground == 226 && info.items.at(jss).activated) {
					if (Definitions == " `0[") Definitions += "`4JAMMED";
					else Definitions += "``, `4JAMMED";
				}
				if (info.items.at(jss).foreground == 1276 && info.items.at(jss).activated) {
					if (Definitions == " `0[") Definitions += "`2NOPUNCH";
					else Definitions += "``, `2NOPUNCH";
				}
				if (info.items.at(jss).foreground == 1278 && info.items.at(jss).activated) {
					if (Definitions == " `0[") Definitions += "`2IMMUNE";
					else Definitions += "``, `2IMMUNE";
				}
				if (info.items.at(jss).foreground == 4992 && info.items.at(jss).activated) {
					if (Definitions == " `0[") Definitions += "`2ANTIGRAVITY";
					else Definitions += "``, `2ANTIGRAVITY";
				}
				if (info.items.at(jss).foreground == 4758 && info.items.at(jss).activated) {
					if (Definitions == " `0[") Definitions += "`4CAN'T DROP";
					else Definitions += "``, `4CAN'T DROP";
				}
			}
			if (info.items.at(jss).foreground == 6) {
				x = (jss % info.width) * 32;
				y = (jss / info.width) * 32;
				if (!jammers) break;
			}
		}
		if (info.isNuked) {
			if (Definitions == " `0[") Definitions += "`4Nuked World";
			else Definitions += "``, `4Nuked World";
		}
		Definitions += "`0]";
		if (Definitions == " `0[`0]") Definitions = "";
		Player::OnConsoleMessage(peer, "World `w" + info.name + "``" + Definitions + " `oentered. There are `w" + to_string(getPlayersCountInWorld(info.name)) + "`` other people here, `w" + GetPlayerCountServer() + "`` online.");
		size_t pos;
		while ((pos = static_cast<PlayerInfo*>(peer->data)->displayName.find("`2")) != string::npos) {
			static_cast<PlayerInfo*>(peer->data)->displayName.replace(pos, 2, "");
		}
		if (info.owner != "") {
			try {
				ifstream read_player("save/players/_" + info.owner + ".json");
				if (!read_player.is_open()) {
					return;
				}
				json j;
				read_player >> j;
				read_player.close();
				string username = j["nick"];
				int adminLevel = j["adminLevel"];
				if (username == "") {
					username = role_prefix.at(adminLevel) + info.owner;
				}
				if (info.owner == static_cast<PlayerInfo*>(peer->data)->rawName || username == static_cast<PlayerInfo*>(peer->data)->displayName || isWorldAdmin(peer, info)) {
					Player::OnConsoleMessage(peer, "`5[`w" + info.name + "`$ World Locked`o by " + username + " ``(`2ACCESS GRANTED``)`5]");
					if (info.notification != "") {
						Player::OnConsoleMessage(peer, info.notification);
						info.notification = "";
					}
					if (info.owner == static_cast<PlayerInfo*>(peer->data)->rawName || username == static_cast<PlayerInfo*>(peer->data)->displayName) {
						if (static_cast<PlayerInfo*>(peer->data)->displayName.find("`") != string::npos) {}
						else {
							static_cast<PlayerInfo*>(peer->data)->displayName = "`2" + static_cast<PlayerInfo*>(peer->data)->displayName;
							Player::OnNameChanged(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->displayName);
						}
					}
				}
				else if (info.publicBlock != -1 && info.publicBlock != 0) {
					Player::OnConsoleMessage(peer, "`5[`w" + info.name + "`$ World Locked`o by " + username + " ``(`^BFG``)`5]");
				}
				else {
					Player::OnConsoleMessage(peer, "`5[`w" + info.name + "`$ World Locked`o by " + username + "`5]");
				}
				if (isWorldAdmin(peer, info)) {
					size_t pos;
					while ((pos = static_cast<PlayerInfo*>(peer->data)->displayName.find("`w")) != string::npos) {
						static_cast<PlayerInfo*>(peer->data)->displayName.replace(pos, 2, "");
					}
					static_cast<PlayerInfo*>(peer->data)->displayName = "`^" + static_cast<PlayerInfo*>(peer->data)->displayName;
					Player::OnNameChanged(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->displayName);
				}
				else if (info.owner != static_cast<PlayerInfo*>(peer->data)->rawName && username != static_cast<PlayerInfo*>(peer->data)->displayName) {
					if (static_cast<PlayerInfo*>(peer->data)->displayName.find("`2") != string::npos) {
						size_t pos;
						while ((pos = static_cast<PlayerInfo*>(peer->data)->displayName.find("`2")) != string::npos) {
							static_cast<PlayerInfo*>(peer->data)->displayName.replace(pos, 2, "");
						}
						Player::OnNameChanged(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->displayName);
					}
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				return;
			}
		}
		sendWorld(peer, &info);
		SendInventory(peer, static_cast<PlayerInfo*>(peer->data)->inventory);
		if (x2 != 0 && y2 != 0) {
			x = x2;
			y = y2;
		}
		static_cast<PlayerInfo*>(peer->data)->x = x;
		static_cast<PlayerInfo*>(peer->data)->x = y;
		static_cast<PlayerInfo*>(peer->data)->checkx = x;
		static_cast<PlayerInfo*>(peer->data)->checky = y;
		int smstate = 0, is_invis = 0;
		if (static_cast<PlayerInfo*>(peer->data)->adminLevel == 5) smstate = 1;
		if (static_cast<PlayerInfo*>(peer->data)->isinv) is_invis = 1;
		/*spawn|avatar\nnetID|" + std::to_string(cId) + "\nuserID|" + std::to_string(cId) + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(x) + "|" + std::to_string(y) + "\nname|" + static_cast<PlayerInfo*>(peer->data)->displayName + "``\ncountry|" + static_cast<PlayerInfo*>(peer->data)->country + "\ninvis|2\nmstate|0\nsmstate|" + to_string(smstate) + "\nonlineID|\ntype|local*/
		auto p = packetEnd(appendString(appendString(createPacket(), "OnSpawn"), "spawn|avatar\nnetID|" + std::to_string(cId) + "\nuserID|" + std::to_string(cId) + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(x) + "|" + std::to_string(y) + "\nname|" + static_cast<PlayerInfo*>(peer->data)->displayName + "``\ncountry|" + static_cast<PlayerInfo*>(peer->data)->country + "\ninvis|" + to_string(is_invis) + "\nmstate|0\nsmstate|" + to_string(smstate) + "\nonlineID|\ntype|local\n"));
		const auto packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
		delete p.data;
		static_cast<PlayerInfo*>(peer->data)->netID = cId;
		onPeerConnect(peer);
		cId++;
		if (!static_cast<PlayerInfo*>(peer->data)->isinv) {
			for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					if (static_cast<PlayerInfo*>(currentPeer->data)->currentWorld == "EXIT") continue;
					if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->rawName) continue;
					Player::OnConsoleMessage(currentPeer, "`5<`w" + static_cast<PlayerInfo*>(peer->data)->displayName + " `5entered, `w" + std::to_string(getPlayersCountInWorld(static_cast<PlayerInfo*>(peer->data)->currentWorld) - 1) + "`` `5others here>```w");
					Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(peer->data)->netID, "`5<`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`` `5entered, `w" + std::to_string(getPlayersCountInWorld(static_cast<PlayerInfo*>(peer->data)->currentWorld) - 1) + "`` `5others here>```w", 0, true);
					Player::PlayAudio(currentPeer, "audio/door_open.wav", 0);
				}
			}
		}
		if (ValentineEvent) {
			gamepacket_t p;
			p.Insert("OnProgressUISet");
			p.Insert(1);
			p.Insert(3402);
			p.Insert(static_cast<PlayerInfo*>(peer->data)->bootybreaken);
			p.Insert(100);
			p.Insert("");
			p.Insert(1);
			p.CreatePacket(peer);
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
}

inline void handle_world(ENetPeer* peer, string act, bool sync = false, bool door = false, string destId = "", bool animations = true, int x = 0, int y = 0) {
	try {
		if (!static_cast<PlayerInfo*>(peer->data)->HasLogged && static_cast<PlayerInfo*>(peer->data)->haveGrowId) return;
		toUpperCase(act);
		if (act.size() > 24) return;
		if (act.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) {
			Player::OnConsoleMessage(peer, "Sorry, spaces and special characters are not allowed in world or door names.  Try again.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (act == "EXIT" && static_cast<PlayerInfo*>(peer->data)->currentWorld == "EXIT") {
			if (!static_cast<PlayerInfo*>(peer->data)->isCursed) act = "HELL";
			Player::OnConsoleMessage(peer, "Exit from what? Press back if you're done playing.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		else if (act == "EXIT") {
			if (!static_cast<PlayerInfo*>(peer->data)->isCursed) act = "HELL";
			Player::OnConsoleMessage(peer, "Where would you like to go? (`w" + GetPlayerCountServer() + " `oonline)");
			sendPlayerLeave(peer);
			static_cast<PlayerInfo*>(peer->data)->currentWorld = "EXIT";
			sendWorldOffers(peer);
			return;
		}
		if (act == "ANAL" || act == "ANUS" || act == "ARSE" || act == "ASS" || act == "BALLSACK" || act == "BALLS" || act == "BITCH" || act == "BOOB" || act == "BUTT" || act == "CLITORIS" || act == "COCK" || act == "CUNT" || act == "DICK" || act == "FAG" || act == "FECK" || act == "FELLATE" || act == "FELLATIO" || act == "FELCHING" || act == "FUCK" || act == "FUDGEPACKER" || act == "FLANGE" || act == "GODDAMN" || act == "HOMO" || act == "JERK" || act == "JIZZ" || act == "KNOBEND" || act == "LABIA" || act == "LMAO" || act == "LMFAO" || act == "MUFF" || act == "NIGGER" || act == "NIGGA" || act == "OMG" || act == "PENIS" || act == "PISS" || act == "POOP" || act == "PRICK" || act == "PUBE" || act == "PUSSY" || act == "QUEER" || act == "SCROTUM" || act == "SEX" || act == "SHIT" || act == "SH1T" || act == "SLUT" || act == "SMEGMA" || act == "SPUNK" || act == "TIT" || act == "TOSSER" || act == "TURD" || act == "TWAT" || act == "VAGINA" || act == "WANK" || act == "WHORE" || act == "WTF" || act == "SEBIA" || act == "ADMIN" || act == "SETH" || act == "HAMUMU" || act == "GOD" || act == "SATAN" || act == "RTSOFT" || act == "HEROMAN" || act == "SYSTEM" || act == "MIKEHOMMEL" || act == "SKIDS" || act == "MODERATOR" || act == "GODS" || act == "THEGODS" || act == "ALMANTAS" || act == "Chris" || act == "Leom" || act == "FocusedOVP") {
			Player::OnConsoleMessage(peer, "`4To reduce confusion, that is not a valid world name.``  Try another?");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (static_cast<PlayerInfo*>(peer->data)->isCursed) act = "HELL";
		if (static_cast<PlayerInfo*>(peer->data)->currentWorld != "EXIT" && !door) {
			sendPlayerLeave(peer);
		}
		if (!static_cast<PlayerInfo*>(peer->data)->haveGrowId) act = "START";
		if (static_cast<PlayerInfo*>(peer->data)->currentWorld != "EXIT" && !door) {
			sendPlayerLeave(peer);
		}
		if (std::experimental::filesystem::exists("save/worldbans/_" + act + "/" + static_cast<PlayerInfo*>(peer->data)->rawName)) {
			Player::OnConsoleMessage(peer, "`4Oh no! ``You've been banned from that world by its owner! Try again later after ban wears off.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		WorldInfo info;
		if (door && act == "") info = worldDB.get(static_cast<PlayerInfo*>(peer->data)->currentWorld);
		else info = worldDB.get(act);
		if (act == "" && !door) {
			joinWorld(info, peer, "START", 0, 0);
			return;
		}
		if (info.name == "error") {
			Player::OnConsoleMessage(peer, "An error occured while trying to enter this world, please try again!");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (getPlayersCountInWorldSave(info.name) > 25) {
			Player::OnConsoleMessage(peer, "Oops, `5" + info.name + "`` already has `425`` people in it. Try again later.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (act == "GROWGANOTH" && !GrowganothEvent && !admin(peer)) {
			Player::OnConsoleMessage(peer, "Growganoth is currently closed.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (info.isNuked && !mod(peer)) {
			Player::OnConsoleMessage(peer, "That world is inaccessible.");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (info.entrylevel > static_cast<PlayerInfo*>(peer->data)->level && info.owner != static_cast<PlayerInfo*>(peer->data)->rawName && !mod(peer)) {
			Player::OnConsoleMessage(peer, "Players lower than level " + to_string(info.entrylevel) + " can't enter " + info.name + ".");
			Player::OnFailedToEnterWorld(peer);
			return;
		}
		if (info.width == 90 && info.height == 60 && static_cast<PlayerInfo*>(peer->data)->canWalkInBlocks) {
			SendGhost(peer);
		}
		if (door) {
			if (destId != "") {
				for (auto i = 0; i < info.width * info.height; i++) {
					if (getItemDef(info.items.at(i).foreground).blockType == BlockTypes::DOOR || getItemDef(info.items.at(i).foreground).blockType == BlockTypes::PORTAL) {
						if (info.items.at(i).currId == destId) {
							if (act == static_cast<PlayerInfo*>(peer->data)->currentWorld || act == "") {
								DoCancelTransitionAndTeleport(peer, (i % info.width), (i / info.width), false, animations);
							}
							else {
								joinWorld(info, peer, act, (i % info.width) * 32, (i / info.width) * 32);
							}
							return;
						}
					}
				}
			}
			for (auto s = 0; s < info.width * info.height; s++) {
				if (info.items.at(s).foreground == 6) {
					if (act == static_cast<PlayerInfo*>(peer->data)->currentWorld || act == "") {
						DoCancelTransitionAndTeleport(peer, (s % info.width), (s / info.width), false, animations);
					}
					else {
						joinWorld(info, peer, act, 0, 0);
					}
					return;
				}
			}
			return;
		}
		joinWorld(info, peer, act, x, y);
		if (sync) Player::PlayAudio(peer, "audio/choir.wav", 200);
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
}

inline void DoEnterDoor(ENetPeer* peer, WorldInfo* world, int x, int y, bool animations = true) {
	if (static_cast<PlayerInfo*>(peer->data)->Fishing) {
		static_cast<PlayerInfo*>(peer->data)->TriggerFish = false;
		static_cast<PlayerInfo*>(peer->data)->FishPosX = 0;
		static_cast<PlayerInfo*>(peer->data)->FishPosY = 0;
		static_cast<PlayerInfo*>(peer->data)->Fishing = false;
		send_state(peer);
		Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wSit perfectly when fishing!", 0, false);
		Player::OnSetPos(peer, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(peer->data)->x, static_cast<PlayerInfo*>(peer->data)->y);
	}
	handle_world(peer, world->items.at(x + (y * world->width)).destWorld, false, true, world->items.at(x + (y * world->width)).destId, animations);
}

inline void SendSacrifice(WorldInfo* world, ENetPeer* peer, int itemid, int count)
{
	int pradinis = itemid;
	int Rarity = getItemDef(itemid).rarity;
	if (getItemDef(itemid).rarity >= 10)
	{
		Rarity = getItemDef(itemid).rarity * count;
	}
	if (itemid == 242) itemid = 1212;
	else if (itemid == 1190) itemid = 1214;
	else if (itemid == 882) itemid = 1232;
	else if (itemid == 592 || itemid == 1018) itemid = 1178;
	else if (itemid == 362) itemid = 1234;
	else if (itemid == 910) itemid = 1250;
	else if (itemid == 274 || itemid == 276) itemid = 1956;
	else if (itemid == 1474) itemid = 1990;
	else if (itemid == 1506) itemid = 1968;
	else if (itemid == 1746) itemid = 1960;
	else if (itemid == 1252) itemid = 1948;
	else if (itemid == 1830) itemid = 1966;
	else if (itemid == 2722) itemid = 3114;
	else if (itemid == 2984) itemid = 3118;
	else if (itemid == 3040) itemid = 3100;
	else if (itemid == 2390) itemid = 3122;
	else if (itemid == 1934) itemid = 3124;
	else if (itemid == 1162) itemid = 3126;
	else if (itemid == 604 || itemid == 2636) itemid = 3108;
	else if (itemid == 3020) itemid = 3098;
	else if (itemid == 914 || itemid == 916 || itemid == 918 || itemid == 920 || itemid == 924) itemid = 1962;
	else if (itemid == 900 || itemid == 1378 || itemid == 1576 || itemid == 7136 || itemid == 7754 || itemid == 7752 || itemid == 7758 || itemid == 7760) itemid = 2000;
	else if (itemid == 1460) itemid = 1970;
	else if (itemid == 3556 && rand() % 100 <= 50) itemid = 4186;
	else if (itemid == 3556) itemid = 4188;
	else if (itemid == 2914 || itemid == 3012 || itemid == 3014) itemid = 4246;
	else if (itemid == 3016 || itemid == 3018) itemid = 4248;
	else if (itemid == 414 || itemid == 416 || itemid == 418 || itemid == 420 || itemid == 422 || itemid == 424 || itemid == 426 || itemid == 4634 || itemid == 4636 || itemid == 4638 || itemid == 4640 || itemid == 4642) itemid = 4192;
	else if (itemid == 1114) itemid = 4156;
	else if (itemid == 366) itemid = 4136;
	else if (itemid == 1950) itemid = 4152;
	else if (itemid == 2386) itemid = 4166;
	else if (itemid == 762) itemid = 4190;
	else if (itemid == 2860 || itemid == 2268) itemid = 4172;
	else if (itemid == 2972) itemid = 4182;
	else if (itemid == 3294) itemid = 4144;
	else if (itemid == 3296) itemid = 4146;
	else if (itemid == 3298) itemid = 4148;
	else if (itemid == 3290) itemid = 4140;
	else if (itemid == 3288) itemid = 4138;
	else if (itemid == 3292) itemid = 4142;
	else if (itemid == 1198) itemid = 5256;
	else if (itemid == 4960) itemid = 5208;
	else if (itemid == 1242) itemid = 5216;
	else if (itemid == 1244) itemid = 5218;
	else if (itemid == 1248) itemid = 5220;
	else if (itemid == 1246) itemid = 5214;
	else if (itemid == 5018) itemid = 5210;
	else if (itemid == 1408) itemid = 5254;
	else if (itemid == 4334) itemid = 5250;
	else if (itemid == 4338) itemid = 5252;
	else if (itemid == 3792) itemid = 5244;
	else if (itemid == 1294) itemid = 5236;
	else if (itemid == 6144) itemid = 7104;
	else if (itemid == 4732) itemid = 7124;
	else if (itemid == 4326) itemid = 7122;
	else if (itemid == 6300) itemid = 7102;
	else if (itemid == 2868) itemid = 7100;
	else if (itemid == 6798) itemid = 7126;
	else if (itemid == 6160) itemid = 7104;
	else if (itemid == 6176) itemid = 7124;
	else if (itemid == 5246) itemid = 7122;
	else if (itemid == 5246) itemid = 7102;
	else if (itemid == 5246) itemid = 7100;
	else if (itemid == 7998) itemid = 9048;
	else if (itemid == 6196) itemid = 9056;
	else if (itemid == 2392) itemid = 9114;
	else if (itemid == 8018) itemid = 9034;
	else if (itemid == 8468) itemid = 10232;

	else if (itemid == 4360) itemid = 10194;
	else if (itemid == 9364) itemid = 10206;
	else if (itemid == 9322) itemid = 10184;

	else if (itemid == 3818) itemid = 10192;
	else if (itemid == 3794) itemid = 10190;
	else if (itemid == 7696) itemid = 10186;

	else if (itemid == 10212) itemid = 10212;

	else if (itemid == 10328)
	{
		int itemuMas[154] = { 10232, 10194, 10206, 10184, 10192, 10190, 10186, 10212, 1212, 1190, 1206, 1166, 1964, 1976, 1998, 1946, 2002, 1958, 1952, 2030, 3104, 3112, 3120, 3092, 3094, 3096, 4184, 4178, 4174, 4180, 4170, 4168, 4150, 1180, 1224, 5226, 5228, 5230, 5212, 5246, 5242, 5234, 7134, 7118, 7132, 7120, 7098, 9018, 9038, 9026, 9066, 9058, 9044, 9024, 9032, 9036, 9028, 9030, 9110, 9112, 10386, 10326, 10324, 10322, 10328, 10316, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 1236, 1182, 1184, 1186, 1188, 1170, 1212, 1214, 1232, 1178, 1234, 1250, 1956, 1990, 1968, 1960, 1948, 1966, 3114, 3118, 3100, 3122, 3124, 3126, 3108, 3098, 1962, 2000, 1970, 4186, 4188, 4246, 4248, 4192, 4156, 4136, 4152, 4166, 4190, 4172, 4182, 4144, 4146, 4148, 4140, 4138, 4142, 5256, 5208, 5216, 5218, 5220, 5214, 5210, 5254, 5250, 5252, 5244, 5236, 7104, 7124, 7122, 7102, 7100, 7126, 7104, 7124, 7122, 7102, 7100, 9048, 9056, 9114, 9034 };
		auto randIndex = rand() % 154;
		itemid = itemuMas[randIndex];
		playerRespawn(world, peer, false);
		Player::OnConsoleMessage(peer, "`2" + getItemDef(pradinis).name + " `owas devoursed by Growganoth!");
		Player::OnConsoleMessage(peer, "`4Growganoth retrieves a very special artifact from his tomb on your behalf and kills you for the service!");
		Player::OnConsoleMessage(peer, "`oA `2" + getItemDef(itemid).name + " `owas bestoved upon you!");
		if (getItemDef(itemid).name.find("Wings") != string::npos || getItemDef(itemid).name.find("Cape") != string::npos || getItemDef(itemid).name.find("Cloak") != string::npos)
		{
			ENetPeer* currentPeer;
			for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				Player::OnConsoleMessage(currentPeer, "`4Growganoth `ohonors `w" + static_cast<PlayerInfo*>(peer->data)->displayName + " `owith a `5Rare " + getItemDef(itemid).name + "");
			}
		}
		auto success = true;
		SaveItemMoreTimes(itemid, 1, peer, success, static_cast<PlayerInfo*>(peer->data)->rawName + " from growganoth");
		if (count >= 2)
		{
			auto success = true;
			SaveItemMoreTimes(10328, count - 1, peer, success, static_cast<PlayerInfo*>(peer->data)->rawName + " from growganoth");
		}
		return;
	}

	else if (getItemDef(itemid).blockType == BlockTypes::WEATHER && itemid != 932) itemid = 1210;

	else if (Rarity < 10)
	{
		int itemuMas[2] = { 1208, 5256 };
		auto randIndex = rand() % 2;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 15)
	{
		int itemuMas[5] = { 1222, 1198, 1992, 5256, 1208 };
		auto randIndex = rand() % 5;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 20)
	{
		int itemuMas[7] = { 1200, 1992, 1982, 5256, 1198, 1208, 1222 };
		auto randIndex = rand() % 7;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 25)
	{
		int itemuMas[9] = { 1220, 1992, 1982, 5256, 1198, 1208, 1222, 1200, 10198 };
		auto randIndex = rand() % 9;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 30)
	{
		int itemuMas[11] = { 1202, 1992, 1982, 5240, 5256, 1198, 1208, 1222, 1200, 1220, 10198 };
		auto randIndex = rand() % 11;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 35)
	{
		int itemuMas[17] = { 1238, 1992, 1982, 4160, 4162, 5240, 5238, 5256, 7116, 1198, 1208, 1222, 1200, 1220, 1202, 10198, 10196 };
		auto randIndex = rand() % 17;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 40)
	{
		int itemuMas[18] = { 1168, 1992, 1982, 4160, 4162, 5240, 5238, 5256, 7116, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 10198, 10196 };
		auto randIndex = rand() % 18;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 45)
	{
		int itemuMas[21] = { 1172, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 10198, 10196 };
		auto randIndex = rand() % 21;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 50)
	{
		int itemuMas[22] = { 1230, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 10198, 10196 };
		auto randIndex = rand() % 22;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 55)
	{
		int itemuMas[25] = { 1194, 1192, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 10198, 10196, 10202 };
		auto randIndex = rand() % 25;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 60)
	{
		int itemuMas[27] = { 1226, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 7108, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 10198, 10196, 10202 };
		auto randIndex = rand() % 27;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 65)
	{
		int itemuMas[28] = { 1196, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 7108, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 10198, 10196, 10202 };
		auto randIndex = rand() % 28;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 70)
	{
		int itemuMas[29] = { 1236, 1992, 1982, 3116, 4160, 4162, 4164, 5240, 5238, 5256, 7116, 7108, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 10198, 10196, 10202 };
		auto randIndex = rand() % 29;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 80)
	{
		int itemuMas[44] = { 1182, 1184, 1186, 1188, 1992, 1982, 1994, 1972, 1980, 1988, 3116, 3102, 4160, 4162, 4164, 4154, 5224, 5222, 5232, 5240, 5238, 5256, 7116, 7108, 7110, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 1236, 10198, 10196, 10202, 10204 };
		auto randIndex = rand() % 44;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity < 90)
	{
		int itemuMas[48] = { 1170, 1992, 1982, 1994, 1972, 1980, 1988, 1984, 3116, 3102, 4160, 4162, 4164, 4154, 4158, 5224, 5222, 5232, 5240, 5238, 5256, 7116, 7108, 7110, 7128, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 1236, 1182, 1184, 1186, 1188, 10198, 10196, 10202, 10204 };
		auto randIndex = rand() % 48;
		itemid = itemuMas[randIndex];
		count = 1;
	}
	else if (Rarity >= 90 && Rarity != 999)
	{
		int itemuMas[56] = { 1216, 1218, 1992, 1982, 1994, 1972, 1980, 1988, 1984, 3116, 3102, 3106, 3110, 4160, 4162, 4164, 4154, 4158, 5224, 5222, 5232, 5240, 5248, 5238, 5256, 7116, 7108, 7110, 7128, 7112, 7114, 7130, 1198, 1208, 1222, 1200, 1220, 1202, 1238, 1168, 1172, 1230, 1194, 1192, 1226, 1196, 1236, 1182, 1184, 1186, 1188, 1170, 10198, 10196, 10202, 10204 };
		auto randIndex = rand() % 56;
		itemid = itemuMas[randIndex];
		count = 1;
	}

	if (pradinis == itemid)
	{
		playerRespawn(world, peer, false);
		Player::OnConsoleMessage(peer, "`4Growganoth rejects your puny offering and eats you instead!");
		Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`4Growganoth rejects your puny offering and eats you instead!", 0, true);
		auto success = true;
		SaveItemMoreTimes(pradinis, count, peer, success, static_cast<PlayerInfo*>(peer->data)->rawName + " from growganoth");
		return;
	}

	playerRespawn(world, peer, false);
	Player::OnConsoleMessage(peer, "`2" + getItemDef(pradinis).name + " `owas devoursed by Growganoth!");
	Player::OnConsoleMessage(peer, "`oA `2" + getItemDef(itemid).name + " `owas bestoved upon you!");
	auto success = true;
	SaveItemMoreTimes(itemid, count, peer, success, static_cast<PlayerInfo*>(peer->data)->rawName + " from growganoth");
}
