#include "pch.h"

std::unordered_map<xuid_t, unsigned short> thirstyList;
std::unordered_map<xuid_t, unsigned short> thirstyTime;
std::unordered_map<xuid_t, bool> isNether;

unsigned short addThirst(xuid_t xuid, unsigned short thirsty) {
	unsigned short thirsty1 = thirstyList[xuid];
	if ((thirsty1 + thirsty) > 100) {
		thirstyList[xuid] = 100;
		return 100 - thirsty1;
	}
	thirstyList[xuid] = thirsty1 + thirsty;
	return thirsty;
}

unsigned short delThirst(xuid_t xuid, unsigned short thirsty) {
	short thirsty1 = thirstyList[xuid];
	if ((thirsty1 - thirsty) < 0) {
		thirstyList[xuid] = 0;
		return 100 - thirsty1;
	}
	thirstyList[xuid] = thirsty1 - thirsty;
	return thirsty;
}

void onPlayerUseItem(PlayerUseItemEV ev) { //ˮƿ
	ItemStack item = ev.Player->getCarriedItem();
	WPlayer wp = WPlayer(*ev.Player);
	string itemName;
	if (item.getId() != 0) {
		SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			void, const Item*, string*)(item.getItem(), &itemName);
	}
	else {
		itemName = "air";
	}
	if (itemName == "minecraft:potion") {
		if (item.getAuxValue() != 0) {
			wp.sendText(u8"�Ǵ���ˮ���ܲ���TP", JUKEBOX_POPUP);
			return;
		}
		xuid_t plXuid = stoull(offPlayer::getXUID(ev.Player));
		string playerReal = offPlayer::getRealName(ev.Player);
		if (liteloader::runcmdEx("clear \"" + playerReal + "\" potion 0 1").first) {
			liteloader::runcmdEx("give \"" + playerReal + "\" glass_bottle 1");
			wp.sendText(u8"��b���ƿ�Ӻ���һ��ˮ��TP+" + std::to_string(addThirst(plXuid, 20)));;
		}
		else {
			wp.sendText(u8"��bˮ��������");
		}
	}
}

void onPlayerDestroy(PlayerDestroyEV ev) { //ˮͰ�����ֺ�ˮ
	ItemStack item = ev.Player->getCarriedItem();
	string itemName;
	if (item.getId() != 0) {
		SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			void, const Item*, string*)(item.getItem(), &itemName);
	}
	else {
		itemName = "air";
	}

	//ev.getPlayer().sendText(itemName, JUKEBOX_POPUP);
	string playerReal = offPlayer::getRealName(ev.Player);
	WPlayer wp = WPlayer(*ev.Player);
	if (itemName == "minecraft:water_bucket") {
		if (liteloader::runcmdEx("clear \"" + playerReal + "\" water_bucket 0 1").first) {
			liteloader::runcmdEx("give \"" + playerReal + "\" bucket 1");
			//add
			xuid_t plXuid = stoull(offPlayer::getXUID(ev.Player));
			wp.sendText(u8"��b���ں���һͰˮ��TP+" + std::to_string(addThirst(plXuid, 50)));
		}
		else {
			wp.sendText(u8"��bˮ��������");
		}
		//ev.setCancelled();
	}
	if (ev.Player->isInWater() && itemName == "air") {
		xuid_t plXuid = stoull(offPlayer::getXUID(ev.Player));
		wp.sendText(u8"��b�㽫��ˮ�������ϣ�һ����������ȥ��TP+" + std::to_string(addThirst(plXuid, 15)));
		//ev.setCancelled();
	}
}

/*void onPlayerPreJoin(PlayerPreJoinEvent& ev) {
	xuid_t xuid = stoull(ExtendedCertificate::getXuid(ev.cert));
	if (thirstyList.find(xuid) == thirstyList.end()) {
		thirstyList[xuid] = 100;
	}
}*/

THook(void, "?_onClientAuthenticated@ServerNetworkHandler@@AEAAXAEBVNetworkIdentifier@@AEBVCertificate@@@Z", void* snh, NetworkIdentifier& neti, Certificate& cert) {
	original(snh, neti, cert);
	xuid_t xuid = offPlayer::getXUIDbyCert(&cert);
	if (thirstyList.find(xuid) == thirstyList.end()) {
		thirstyList[xuid] = 100;
	}
}

void onPlayerJoin(JoinEV ev) {
	xuid_t xuid = stoull(offPlayer::getXUID(ev.Player));
	if (ev.Player->getDimensionId() == 1) {
		isNether[xuid] = true;
	}
	else {
		isNether[xuid] = false;
	}
	thirstyTime[xuid] = 0;
}

void onPlayerDeath(PlayerDeathEV ev) {
	xuid_t plXuid = stoull(offPlayer::getXUID(ev.Player));
	thirstyList[plXuid] = 100;
	thirstyTime[plXuid] = 0;
}

short ticks = 0;
THook(void, "?normalTick@Player@@UEAAXXZ", Player* player) {
	if (ticks > 0) {
		WPlayer wp = WPlayer(*player);
		//ItemStack* itemStack = SymCall("?getSelectedItem@Player@@QEBAAEBVItemStack@@XZ", ItemStack*, Player*)(player);
		//if (thirstyList[plXuid] != 0) thirstyTime[plXuid] = thirstyTime[plXuid] + 1;
		int thirsty = thirstyList[stoull(offPlayer::getXUID(player))];
		string popup;
		if (thirsty <= 20) {
			if (thirsty == 0) player->setOnFire(1);
			//std::cout << SymCall("?getSpeed@Player@@UEBAMXZ", float, Player*)(player) << "\n";
			//SymCall("?setSpeed@Player@@UEAAXM@Z", void, Player*, float)(player, 0.05);
			liteloader::runcmdEx("effect \"" + offPlayer::getRealName(player) + "\" slowness 1");
			popup = u8"��cTP:" + std::to_string(thirsty) + u8"%%";
		}
		else {
			if (wp.getDimID() == 1) {
				popup = u8"��6TP:" + std::to_string(thirsty) + u8"%%";
			}
			else {
				popup = u8"��bTP: " + std::to_string(thirsty) + u8"%%";
			}
		}
		wp.sendText(popup, TIP);
		ticks = 0;
	}
	return original(player);
}

THook(void, "?eat@Player@@QEAAXHM@Z", Player* player, int a1, float a2) {
	//std::cout << player->getNameTag() << " " << a1 << " " << a2 << "\n";
	WPlayer wp = WPlayer(*player);
	wp.sendText(u8"��b��ʳ������ȡˮ�֣�TP+" + std::to_string(addThirst(stoull(offPlayer::getXUID(player)), (a1 / 2))));
	return original(player, a1, a2);
}

void timer() {
	for (; true; Sleep(1000)) {
		ticks++;
		for (auto i : thirstyTime) {
			//if (thirstyList.find(i.first) == thirstyList.end()) continue;
			xuid_t xuid = i.first;
			thirstyTime[xuid] = i.second + 1;
			if (thirstyTime[xuid] >= 10) {
				if (isNether[xuid]) {
					delThirst(xuid, 2);
					thirstyTime[xuid] = 0;
				}
				else {
					delThirst(xuid, 1);
					thirstyTime[xuid] = 0;
				}
			}
		}
	}
}

/*
void popupTimer() {
	for (; true; Sleep(1500)) {
		for (auto pl : players) {
			WPlayer wp = pl.second;
			if (!wp) continue;
			int thirsty = thirstyList[wp.getXuid()];
			string popup;
			if (thirsty <= 20) {
				if (thirsty == 0) wp.get().setOnFire(2);
				liteloader::runcmdEx("effect \"" + wp.getRealName() + "\" slowness 2");
				popup = u8"��cTP:" + std::to_string(thirsty) + u8"%%";
			}
			else {
				if (isNether[wp.getXuid()]) {
					popup = u8"��6TP:" + std::to_string(thirsty) + u8"%%";
				}
				else {
					popup = u8"��bTP: " + std::to_string(thirsty) + u8"%%";
				}
			}
			wp.sendText(popup, TIP);
		}
	}
}*/

void entry() {
	Event::addEventListener(onPlayerDestroy);
	Event::addEventListener(onPlayerUseItem);
	Event::addEventListener(onPlayerDeath);
	Event::addEventListener([](ChangeDimEV ev) {
		xuid_t xuid = stoull(offPlayer::getXUID(ev.Player));
		if (ev.Player->getDimensionId() == 1) {
			isNether[xuid] = true;
			WPlayer wp = WPlayer(*ev.Player);
			wp.sendText(u8"��b[URSystem] �����������ȣ�TP�����ٶ��������������");
		}
		else {
			isNether[xuid] = false;
		}
		});
	Event::addEventListener(onPlayerJoin);
	//Event::addEventListener(onPlayerPreJoin);
	Event::addEventListener([](LeftEV ev) {
		thirstyTime.erase(ev.xuid);
		});
	std::thread t(timer);
	t.detach();
	std::cout << "[ThirstPoint] loaded!\n";
}