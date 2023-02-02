#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <sstream>
#include <Windows.h>

using namespace std;

typedef unsigned int ui;
typedef unsigned long ul;
typedef unsigned long long ull;
typedef signed long sl;

enum CreatureType {
	Ant_,
	Bumblebee_,
	Butterfly_,
	Wasp_,
	CreatureType_None_,
};

map<CreatureType, string> cTypeDesc = {
	{CreatureType::Ant_, "Муравей"},
	{CreatureType::Bumblebee_, "Шмель"},
	{CreatureType::Butterfly_, "Бабочка"},
	{CreatureType::Wasp_, "Оса"},
	{CreatureType::CreatureType_None_, "Никто"}
};

enum ProfType {
	Worker_,
	Warrior_,
	Queen_,
	ProfType_None_,
	AntType_NR_
};

map<ProfType, string> pTypeDesc = {
	{ProfType::Worker_, "Рабочий"},
	{ProfType::Warrior_, "Воин"},
	{ProfType::Queen_, "Королева"},
	{ProfType::ProfType_None_, "Особый"}
};

map<ui, string> resDesc = {
	{0, "веточка"},
	{1, "камушек"},
	{2, "листик"},
	{3, "росинка"}
};


const string infoSep = "===============================================================";
const string eventSep = "------";

string mainMenu() {
	stringstream ss;
	ss << "1. Информация о колониях" << '\n';
	ss << "2. Cледующий день" << '\n';
	ss << "3. Выход из программы";

	return ss.str();
}

string coloniesMenu() {
	stringstream ss;
	ss << "1. Информация о населении колонии" << '\n';
	ss << "2. Информация о ресурсах колонии" << '\n';
	ss << "3. Информация о муравьях" << '\n';
	ss << "4. Назад";

	return ss.str();
}


class Colony;

class Heap {
public:
	string _id;
	vector<ul> _reses;
	vector<Colony*> _hiked;

	Heap(string id = "0", ul stick = 0, ul stone = 0, ul leaf = 0, ul dew = 0) : _reses(4) {
		_reses[0] = stick; _reses[1] = stone;
		_reses[2] = leaf; _reses[3] = dew;

		_id = id;
	}
	virtual ~Heap() {
	}

	bool hasSomething() {
		ull some = 0;
		for (auto r : _reses)
			some += r;

		return some > 0 ? true : false;
	}
	bool has(ui index) {
		return _reses[index] > 0;
	}

	void addHiked(Colony* colony) {
		_hiked.push_back(colony);
	}
	void clearHiked() {
		_hiked.clear();
	}

	bool operator ==(const Heap& heap) {
		return heap._id == this->_id;
	}
	bool operator !=(const Heap& heap) {
		return heap._id != this->_id;
	}
};


class QueenAnt;

class Creature {
public:
	string _name;
	ui _hp, _def, _dmg;

	string _prop;

	CreatureType _creatureType;
	ProfType _profType;

	bool isActed;

	Creature(ui hp, ui def, ui dmg, string name = "Никто", CreatureType ctype = CreatureType::CreatureType_None_, ProfType ptype = ProfType::ProfType_None_) {
		_name = name;
		_hp = hp; _def = def; _dmg = dmg;
		_creatureType = ctype; _profType = ptype;

		isActed = false;
	}
	virtual ~Creature() {

	}

	virtual string getInfo() {
		stringstream ss;
		string print_name = _prop;
		if (_name == "Жозефина")
			print_name = _name;
		ss << "Тип: " << cTypeDesc[_creatureType] << " | Профессия: " << pTypeDesc[_profType] << " | Имя: " << print_name << "\nХарактеристики: \n";
		ss << "Здоровье: " << _hp << " | Защита: " << _def << " | Урон: " << _dmg << '\n';

		return ss.str();
	}

	bool getDmg(ui dmg) {
		if (dmg >= _def) {
			ui tdef = _def;
			_def = 0;
			dmg -= tdef;
		}
		else {
			_def -= dmg;
			return true;
		}

		if (dmg >= _hp) {
			_hp = 0;
			return false;
		}
		else {
			_hp -= dmg;
			return true;
		}
	}


	virtual pair<bool, pair<pair<CreatureType, ProfType>, pair<ui, bool> > >
		attack(Colony* ac, stringstream& events);
	virtual vector<ui> work(Heap* h, stringstream& events);

	bool operator==(const Creature& creature) {
		return creature._name == this->_name;
	}
	bool operator!=(const Creature& creature) {
		return creature._name != this->_name;
	}
};

class Ant : public Creature {
public:
	QueenAnt* _queen;
	Ant(ui hp, ui def, ui dmg, string name = "Никто", QueenAnt* queen = nullptr, ProfType ptype = ProfType::ProfType_None_) : Creature(hp, def, dmg, name, CreatureType::Ant_, ptype) {
		_queen = queen;
	}
	virtual ~Ant() {
	}
};

class WorkerAnt : public Ant {
public:
	WorkerAnt(ui hp = 10, ui def = 6, ui dmg = 0, string name = "Никто", QueenAnt* queen = nullptr) : Ant(hp, def, dmg, name, queen, Worker_) {

	}
};

class WarriorAnt : public Ant {
public:
	WarriorAnt(ui hp = 8, ui def = 4, ui dmg = 5, string name = "Никто", QueenAnt* queen = nullptr) : Ant(hp, def, dmg, name, queen, Warrior_) {

	}
};

class QueenAnt : public Ant {
public:
	ui _lavMin, _lavMax,
		_bornMin, _bornMax,
		_queensMax, _queensCount,
		_bornedCount;
	map<ProfType, pair< vector<string>, vector<vector<ui>> > > _propses;

	bool isWaitingAnt;
	ui antTimer;

	QueenAnt(ui hp = 23, ui def = 8, ui dmg = 15, string name = "Никто", QueenAnt* queen = nullptr) : Ant(hp, def, dmg, name, queen, Queen_) {
		_lavMin = 1; _lavMax = 1;
		_bornMin = 1; _bornMax = 1;
		_queensMax = 1; _queensCount = 0;
		_bornedCount = 0;

		isWaitingAnt = false;
		antTimer = 0;
	}

	Ant* bornAnt() {
		bool isPos = true;
		do {
			ui nptype = rand() % (ProfType::AntType_NR_ - 1);
			ProfType ptype = (ProfType)nptype;
			if (ptype == ProfType::Queen_) {
				if (_queensCount > _queensMax)
					isPos = false;
				else
					isPos = true;


				if (ptype == ProfType::Queen_ && isPos) {
					auto a = new QueenAnt(_hp, _def, _dmg, _name + "_" + to_string(_bornedCount), this);
					a->_lavMin = this->_lavMin; a->_lavMax = this->_lavMax;
					a->_queensMax = 0;
					a->_propses = _propses;

					_bornedCount++;
					return a;
				}
			}
			else {
				isPos = true;

				if (ptype == ProfType::Worker_) {
					ui ps = rand() % _propses[Worker_].first.size();
					string prop = _propses[Worker_].first[ps];
					vector<ui> stats = _propses[Worker_].second[ps];

					auto a = new WorkerAnt(stats[0], stats[1], stats[2]);
					a->_name = _name + "_" + to_string(_bornedCount);
					a->_queen = this;
					a->_prop = prop;

					_bornedCount++;


					return a;
				}
				else if (ptype == ProfType::Warrior_) {
					ui ps = rand() % _propses[Warrior_].first.size();
					string prop = _propses[Warrior_].first[ps];
					vector<ui> stats = _propses[Warrior_].second[ps];

					auto a = new WarriorAnt(stats[0], stats[1], stats[2]);
					a->_name = _name + "_" + to_string(_bornedCount);
					a->_queen = this;
					a->_prop = prop;

					_bornedCount++;
					return a;
				}

			}
		} while (!isPos);

		return nullptr;
	}

	Ant* createAnt(ProfType ptype) {
		if (ptype == ProfType::Worker_) {
			ui ps = rand() % _propses[Worker_].first.size();
			string prop = _propses[Worker_].first[ps];
			vector<ui> stats = _propses[Worker_].second[ps];

			auto a = new WorkerAnt(stats[0], stats[1], stats[2]);
			a->_name = _name + "_" + to_string(_bornedCount);
			a->_queen = this;
			a->_prop = prop;

			_bornedCount++;
			return a;
		}
		else if (ptype == ProfType::Warrior_) {
			ui ps = rand() % _propses[Warrior_].first.size();
			string prop = _propses[Warrior_].first[ps];
			vector<ui> stats = _propses[Warrior_].second[ps];

			auto a = new WarriorAnt(stats[0], stats[1], stats[2]);
			a->_name = _name + "_" + to_string(_bornedCount);
			a->_queen = this;
			a->_prop = prop;

			_bornedCount++;
			return a;
		}
		else if (ptype == ProfType::Queen_) {
			auto a = new QueenAnt(_hp, _def, _dmg, _name + "_" + to_string(_bornedCount), this);

			_bornedCount++;
			return a;
		}

		return nullptr;
	}

	void setProps(ProfType ptype, vector<string> props, vector<vector<ui>> stats) {
		_propses[ptype].first = props;
		_propses[ptype].second = stats;
	}

	bool operator==(const QueenAnt& queen) {
		return queen._name == this->_name;
	}
};

class Butterfly : public Creature {
public:
	Butterfly(ui hp, ui def, ui dmg, string name = "Никто") : Creature(hp, def, dmg, name, CreatureType::Butterfly_) {

	}

};

class Wasp : public Creature {
public:
	Wasp(ui hp, ui def, ui dmg, string name = "Никто") : Creature(hp, def, dmg, name, CreatureType::Wasp_) {

	}
};


class Colony : public Heap {
public:
	string _name;
	QueenAnt* _queen;
	map<CreatureType, map<ProfType, vector<Creature*> > > _creatures;
	map<CreatureType, map<ProfType, ui> > _creaturesCount;

	Heap* _heap;

	Colony(string name, QueenAnt* queen) : _name(name), _queen(queen) {
		_creatures[queen->_creatureType][queen->_profType].push_back(queen);
		_creaturesCount[queen->_creatureType][queen->_profType] += 1;

		_heap = nullptr;
	}

	virtual ~Colony(){
	}

	void addCreature(Creature* creature) {
		_creatures[creature->_creatureType][creature->_profType].push_back(creature);
		_creaturesCount[creature->_creatureType][creature->_profType] += 1;
	}

	bool hasCreatureType(CreatureType ct) {
		return _creatures.find(ct) != _creatures.end();
	}
	bool hasProfessionType(CreatureType ct, ProfType pt) {
		return _creatures[ct].find(pt) != _creatures[ct].end();
	}
	bool existCreature(CreatureType ct) {
		if (_creatures.find(ct) != _creatures.end()) {
			for (auto pt : _creatures[ct]) {
				if (pt.second.size() != 0)
					return true;
			}
		}
		return false;
	}
	bool existCreatureProf(CreatureType ct, ProfType pt) {
		if (_creatures.find(ct) != _creatures.end()) {
			if (_creatures[ct].find(pt) != _creatures[ct].end())
				if (_creatures[ct][pt].size() != 0)
					return true;
		}
		return false;
	}
	bool hasAnyCreatureType() {
		bool res = false;
		if (_creatures.size() != 0) {
			res = true;
		}
		else {
			for (auto ct : _creatures) {
				if (ct.second.size() == 0) {
					res = false;
				}
				else {
					for (auto pt : ct.second) {
						if (pt.second.size() != 0)
							res = true;
					}
				}
			}
		}
		return res;
	}
	bool hasAnyProfessionType() {
		bool res = hasAnyCreatureType();

		for (auto ct : _creatures) {
			if (ct.second.size() == 0) {
				res = false;
			}
			else {
				for (auto pt : ct.second) {
					if (pt.second.size() != 0)
						res = true;
				}
			}
		}

		return res;
	}

	void makeHike(Heap* heap) {
		if (_heap == nullptr)
			_heap = heap;
	}
	void doneHike() {
		_heap = nullptr;
	}

	void clearIsActed() {
		for (auto ct : _creatures)
			for (auto pt : ct.second)
				for (auto c : pt.second)
					c->isActed = false;
	}

	bool operator==(const Colony& colony) {
		return colony._name == this->_name;
	}
	bool operator!=(const Colony& colony) {
		return colony._name != this->_name;
	}
};

pair<pair<CreatureType, ProfType>, pair<ui, Creature*> > getRnd(Colony* ac, set<CreatureType> sct, set<ProfType> spt) {
	CreatureType ctrand;
	ProfType ptrand;

	bool cStop = false, pStop = false;
	map<CreatureType, bool> cCounter;
	map<ProfType, bool> pCounter;
	vector<CreatureType> vC;
	vector<ProfType> vP;

	for (auto sc : sct) {
		vC.push_back(sc);
		cCounter[sc] = false;
	}
	for (auto sp : spt) {
		vP.push_back(sp);
		pCounter[sp] = false;
	}
	if (!ac->hasAnyProfessionType())
		return { {CreatureType_None_, ProfType_None_}, {0, nullptr} };

	do {
		if (cStop)
			break;

		ctrand = vC[rand() % vC.size()];

		if (!ac->existCreature(ctrand)) {
			cCounter[ctrand] = true;

			bool isBr = true;
			for (auto cC : cCounter)

				if (!cC.second)
					isBr = false;
			if (isBr)
				cStop = true;
		}
		else {
			break;
		}


		if (cStop)
			break;

	} while (1);
	do {
		if (cStop) {
			pStop = true;
			break;
		}

		ptrand = vP[rand() % vP.size()];

		if (!ac->existCreatureProf(ctrand, ptrand)) {
			pCounter[ptrand] = true;
			bool isBr = true;
			for (auto pC : pCounter)
				if (!pC.second)
					isBr = false;
			if (isBr)
				pStop = true;
		}
		else {
			break;
		}

		if (pStop)
			break;

	} while (1);


	if (pStop)
		return { {CreatureType_None_, ProfType_None_}, {0, nullptr} };


	auto creatures = ac->_creatures[ctrand][ptrand];
	ui idx = rand() % creatures.size();
	return { {ctrand, ptrand}, {idx, creatures[idx]} };
}

pair<bool, pair<pair<CreatureType, ProfType>, pair<ui, bool> > >
Creature::attack(Colony* ac, stringstream& ss) {
	if (isActed)
		return { true, { {}, {0, true} } };
	ui tC;
	ui bC;

	if (_prop == "Продвинутый_3") {
		tC = 2;
		bC = 1;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
			else {
				if (!getDmg(a->_dmg)) {
					ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
						<< eventSep << '\n';
					return { false, { {ctrand, ptrand}, {idx, true} } };
				}
			}
		}
	}
	else if (_prop == "Старший") {
		tC = 1;
		bC = 1;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
			else {
				if (!getDmg(a->_dmg)) {
					ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
						<< eventSep << '\n';
					return { false, { {ctrand, ptrand}, {idx, true} } };
				}
			}
		}
	}
	else if (_prop == "Легендарный мифический") {
		tC = 3;
		bC = 1;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC * 100)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
		}
	}
	else if (_prop == "Обычный") {
		tC = 1;
		bC = 1;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
			else {
				if (!getDmg(a->_dmg)) {
					ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
						<< eventSep << '\n';
					return { false, { {ctrand, ptrand}, {idx, true} } };
				}
			}
		}
	}
	else if (_prop == "Элитный_2") {
		tC = 2;
		bC = 2;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
			else {
				if (!getDmg(a->_dmg)) {
					ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
						<< eventSep << '\n';
					return { false, { {ctrand, ptrand}, {idx, true} } };
				}
			}
		}
	}
	else if (_prop == "Легендарный сосредоточенный") {
	tC = 3;
	bC = 1;

	for (ui i = 0; i < tC; i++) {
		auto ainfo = getRnd(ac, { Ant_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
		auto ctrand = ainfo.first.first;
		auto ptrand = ainfo.first.second;
		auto idx = ainfo.second.first;
		auto a = ainfo.second.second;
		if (!a)
			return { true, { {}, {0, true} } };
		if (!a->getDmg(_dmg * bC * 2)) {
			ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
				<< eventSep << '\n';

			auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
			ac->_creatures[ctrand][ptrand].erase(ait);
			return { true, { {ctrand, ptrand}, {idx, false} } };
		}
		else {
			if (!getDmg(a->_dmg)) {
				ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
					<< eventSep << '\n';
				return { false, { {ctrand, ptrand}, {idx, true} } };
			}
		}
	}
	}

	else if (_prop == "Трудолюбивый неуязвимый агрессивный щитоносец опытный") {
		tC = 1;
		bC = 3;

		for (ui i = 0; i < tC; i++) {
			auto ainfo = getRnd(ac, { Ant_, Butterfly_, Wasp_ }, { Worker_, Warrior_, ProfType_None_ });
			auto ctrand = ainfo.first.first;
			auto ptrand = ainfo.first.second;
			auto idx = ainfo.second.first;
			auto a = ainfo.second.second;
			if (!a)
				return { true, { {}, {0, true} } };
			if (!a->getDmg(_dmg * bC)) {
				ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
					<< eventSep << '\n';

				auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
				ac->_creatures[ctrand][ptrand].erase(ait);
				return { true, { {ctrand, ptrand}, {idx, false} } };
			}
			else {
				if (!getDmg(a->_dmg)) {
					ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
						<< eventSep << '\n';
					return { false, { {ctrand, ptrand}, {idx, true} } };
				}
			}
		}
	}
	else if (_prop == "Трудолюбивый обычный агрессивный дурной ветеран") {
	tC = 2;
	bC = 2;

	for (ui i = 0; i < tC; i++) {
		auto ainfo = getRnd(ac, { Ant_, Wasp_, Butterfly_ }, { Worker_, Warrior_, ProfType_None_ });
		auto ctrand = ainfo.first.first;
		auto ptrand = ainfo.first.second;
		auto idx = ainfo.second.first;
		auto a = ainfo.second.second;
		if (!a)
			return { true, { {}, {0, true} } };
		if (!a->getDmg(_dmg * bC * 2)) {
			ss << "<" << a->_prop << "> " << a->_name << " был убит " << "<" << _prop << "> " << _name << '\n'
				<< eventSep << '\n';

			auto ait = ac->_creatures[ctrand][ptrand].begin() + idx;
			ac->_creatures[ctrand][ptrand].erase(ait);
			return { true, { {ctrand, ptrand}, {idx, false} } };
		}
		else {
			if (!getDmg(a->_dmg)) {
				ss << "<" << _prop << "> " << _name << " был убит " << "<" << a->_prop << "> " << a->_name << '\n'
					<< eventSep << '\n';
				return { false, { {ctrand, ptrand}, {idx, true} } };
			}
		}
	}

	}

	isActed = true;
	return { true, { {}, {0, true} } };
}

vector<ui> Creature::work(Heap* h, stringstream& ss) {
	ui rC;
	ui count_take = 0;
	vector<ui> res(4);

	if (_prop == "Легендарный") {
		rC = 3;
		if (h->has(0)) {
			h->_reses[0]--;
			res[0]++;
		}
		if (h->has(0)) {
			h->_reses[0]--;
			res[0]++;
		}
		if (h->has(0)) {
			h->_reses[0]--;
			res[0]++;
		}

		return res;
	}
	else if (_prop == "Элитный_1") {
		rC = 2;
		if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}
		if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}

		return res;
	}
	else if (_prop == "Старший сильный") {
		rC = 2;
		if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}

		return res;
	}
	else if (_prop == "Продвинутый_1") {
		rC = 2;
		if (h->has(1) && count_take < rC) {
			h->_reses[1]--;
			res[1]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}
		if (h->has(1) && count_take < rC) {
			h->_reses[1]--;
			res[1]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}

		return res;
	}
	else if (_prop == "Продвинутый_2") {
		rC = 2;
		if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}
		if (h->has(0) && count_take < rC) {
			h->_reses[0]--;
			res[0]++;
			count_take++;
		}
		if (h->has(3) && count_take < rC) {
			h->_reses[3]--;
			res[3]++;
			count_take++;
		}

		return res;
	}
	else if (_prop == "Элитный любимчик королевы") { //ПО ВОЗМОЖНОСТИ РЕАЛИЗВАТЬ ПРОВЕРКУ НА НАЛИЧИЕ ДРУГОЙ АРМИИ
		rC = 2;

		if (true) {
			if (h->has(0)) {
				h->_reses[0]--;
				res[0]++;
			}
			if (h->has(0)) {
				h->_reses[0]--;
				res[0]++;
			}
		}
		else {
			ss << _prop << " игнорируйте поход\n"
				<< eventSep << endl;
		}

		return res;
	}
	else if (_prop == "Tрудолюбивый неуязвимый агрессивный щитоносец опытный") {
	rC = 2;

	if (h->has(0) && count_take < rC) {
		h->_reses[0]--;
		res[0]++;
		count_take++;
	}
	if (h->has(1) && count_take < rC) {
		h->_reses[1]--;
		res[1]++;
		count_take++;
	}
	if (h->has(3) && count_take < rC) {
		h->_reses[3]--;
		res[3]++;
		count_take++;
	}
	if (h->has(0) && count_take < rC) {
		h->_reses[0]--;
		res[0]++;
		count_take++;
	}
	if (h->has(1) && count_take < rC) {
		h->_reses[1]--;
		res[1]++;
		count_take++;
	}
	if (h->has(3) && count_take < rC) {
		h->_reses[3]--;
		res[3]++;
		count_take++;
	}
	return res;
	}
}


bool isNumber(const string& sNum) {
	std::string::const_iterator it = sNum.begin();
	if (*it == '-') it++;
	while (it != sNum.end() && std::isdigit(*it)) ++it;
	return !sNum.empty() && it == sNum.end();
}
bool isNegNumber(const std::string& sNum) {
	if (sNum[0] == '-' && isNumber(sNum))
		return true;
	return false;
}

bool numParse(std::string number, sl a, sl b) {
	if (isNumber(number)) {
		sl n = atol(number.c_str());
		if (a <= n && n <= b)
			return true;
	}
	return false;
}


ui days = 11;

ui workBlackC = 14, warBlackC = 8,
workRedC = 18, warRedC = 6,
workGreenC = 19, warGreenC = 6;

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	srand(time(NULL));
	vector<Heap*> heaps = {
		new Heap("1", 39, 0, 0, 0),
		new Heap("2", 26, 0, 0, 15),
		new Heap("3", 37, 45, 0, 26),
		new Heap("4", 43, 39, 0, 38),
		new Heap("5", 43, 0, 0, 23)
	};
	vector<Colony*> colonies;

	auto qRed = new QueenAnt(21, 7, 28, "Жозефина", nullptr);
	qRed->setProps(ProfType::Worker_, { "Продвинутый_1", "Продвинутый_2","Элитный любимчик королевы" }, { {6,2,0},{6,2,0},{8,4,0} });
	qRed->setProps(ProfType::Warrior_, { "Продвинутый_3", "Старший", "Легендарный мифический" }, { {6,2,3},{2,1,2}, {10,6,4} });
	qRed->_bornMin = 2; qRed->_bornMax = 3;
	qRed->_queensMax = 4;
	auto colonyRed = new Colony("Рыжий", qRed);
	for (ui i = 0; i < workRedC; i++)
		colonyRed->addCreature(qRed->createAnt(ProfType::Worker_));
	for (ui i = 0; i < warRedC; i++)
		colonyRed->addCreature(qRed->createAnt(ProfType::Warrior_));
	auto waspRed = new Wasp(27, 24, 14, "Oca");
	waspRed->_prop = "Трудолюбивый неуязвимый агрессивный щитоносец опытный";
	colonyRed->addCreature(waspRed);

	colonies.push_back(colonyRed);

	auto qGreen = new QueenAnt(27, 7, 27, "Жозефина", nullptr);
	qGreen->setProps(ProfType::Worker_, { "Элитный_1", "Легендарный","Старший сильный" }, { {8,4,0},{10,6,0},{2,1,0} });
	qGreen->setProps(ProfType::Warrior_, { "Обычный", "Элитный_2", "Легендарный сосредоточенный" }, { {1,0,1},{8,4,5}, {10,6,4} });
	qGreen->_bornMin = 2; qGreen->_bornMax = 3;
	qGreen->_queensMax = 4;
	auto colonyGreen = new Colony("Зеленый", qGreen);
	for (ui i = 0; i < workGreenC; i++)
		colonyGreen->addCreature(qGreen->createAnt(ProfType::Worker_));
	for (ui i = 0; i < warGreenC; i++)
		colonyGreen->addCreature(qGreen->createAnt(ProfType::Warrior_));
	auto butGreen = new Butterfly(26, 6, 10, "Бабочка");
	butGreen->_prop = "Трудолюбивый обычный агрессивный дурной ветеран";
	colonyGreen->addCreature(butGreen);

	colonies.push_back(colonyGreen);

	ui currDay = 1;

	string sInput = "0";
	ui nInput = 0;
	bool flag_doll = true; //доп задание
	auto rnd = rand() % 3 + 1; //доп задание

	do {
		cout << "Текущий День: " << currDay << endl;
		cout << mainMenu() << endl;
		cout << ">> "; cin >> sInput;

		if (sInput == "1") {
			cout << "Текущий День: " << currDay << endl;
			cout << coloniesMenu() << endl;
			cout << ">> "; cin >> sInput;

			if (sInput == "1") {
				cout << infoSep << endl;
				for (auto colony : colonies) {
					cout << "Название колонии: " << colony->_name << " | Информация: " << endl;
					for (auto ct : colony->_creatures) {
						cout << "\tТип существа: " << cTypeDesc[ct.first] << endl;
						for (auto pt : ct.second) {
							cout << "\t\tТип профессии: " << pTypeDesc[pt.first] << " | Количество: " << pt.second.size() << endl;
						}
					}
				}
				cout << infoSep << endl;
			}
			else if (sInput == "2") {
				cout << infoSep << endl;
				for (auto colony : colonies) {
					cout << "Название колонии: " << colony->_name << " | Информация: " << endl;
					cout << "\tВеточка: " << colony->_reses[0] <<
						"\n\tКамушек: " << colony->_reses[1] <<
						"\n\tЛистик: " << colony->_reses[2] <<
						"\n\tРосинка: " << colony->_reses[3] << endl;
				}
				cout << infoSep << endl;
			}
			else if (sInput == "3") {
				do {
					for (ui i = 0; i < colonies.size(); i++) {
						cout << i + 1 << ". Название колонии: " << colonies[i]->_name << endl;
					}

					cout << ">> "; cin >> sInput;
					if (!numParse(sInput, 1, colonies.size()))
						continue;

					nInput = atoi(sInput.c_str());

					cout << infoSep << endl;
					for (auto ct : colonies[nInput - 1]->_creatures) {
						for (auto pt : ct.second) {
							for (auto c : pt.second) {
								cout << c->getInfo() << endl;
								cout << "------" << endl;
							}
						}
					}
					break;

				} while (true);
				cout << infoSep << endl;
			}
		}
		else if (sInput == "2") {
			currDay++;
			stringstream events;

			vector<Colony*>newColonies;

			for (auto c : colonies) {
				auto q = c->_queen;
				if (!q->isWaitingAnt) {
					q->antTimer = rand() % (q->_bornMax - q->_bornMin + 1) + q->_bornMin;
					q->isWaitingAnt = true;
					events << "Королева " << q->_name << " ожидает рождения кого-то, мы будем знать, кто родится через <" << q->antTimer << "> дня." << endl
						<< eventSep << endl;
					q->antTimer--;
				}
				else {
					if (q->antTimer > 0) {
						q->antTimer--;
					}
					else {
						auto a = q->bornAnt();

						if (a == nullptr)
							continue;
						if (a->_profType == ProfType::Queen_) {
							q->_queensCount++;
							QueenAnt* qa = static_cast<QueenAnt*>(a);
							Colony* nc = new Colony(qa->_name + " Колония", qa);

							newColonies.push_back(nc);

							q->isWaitingAnt = false;

							events << "Новая королева (" << qa->_name << ") был рожден " << q->_name << endl
								<< eventSep << '\n';
							events << "Новая колония (" << nc->_name << ") был создан " << qa->_name << endl
								<< eventSep << '\n';
						}
						else {
							c->addCreature(a);
							q->isWaitingAnt = false;
							events << "Новый муравей (<" << a->_prop << "> " << pTypeDesc[a->_profType] << ") был рожден " << q->_name << ". И был добавлен к " << c->_name << " колония" << endl
								<< eventSep << endl;
						}
					}
				}
			}

			for (auto c : colonies) {
				auto h = heaps[rand() % heaps.size()];
				h->addHiked(c);
				c->makeHike(h);

				events << c->_name << " выбрал для похода в " << h->_id << endl
					<< eventSep << endl;
			}

			for (auto col : colonies) {
				bool isWar = false;

				for (auto ct : col->_creatures) {
					for (auto pt : ct.second)
						for (auto c = pt.second.begin(); c != pt.second.end(); ) {
							if ((*c)->isActed) {
								c++;
								continue;
							}
							if (pt.first == Worker_ || pt.first == ProfType_None_) {
								auto reses = (*c)->work(col->_heap, events);
								for (ui i = 0; i < reses.size(); i++) {
									col->_reses[i] += reses[i];
									if (reses[i] > 0) {
										events << "<" <<(*c)->_prop << "> " << (*c)->_name << " принес " << reses[i] << ' ' << resDesc[i] << endl
											<< eventSep << endl;
									}
								}
								c++;
							}
							else if (pt.first != Queen_) {
								bool killed = false;
								for (auto ac : col->_heap->_hiked) {
									if (ac == col || ac->_queen->_queen == col->_queen) {
										continue;
									}
									else {
										bool k = true;
										if (c != pt.second.end()) {
											auto ares = (*c)->attack(ac, events);
											k = ares.first;
										}
										else {
										}
										if (!k) {

											c = pt.second.erase(c);
											killed = true;
										}
									}
								}
								if (!killed)
									c++;
							}
							else {
								c++;
							}
						}
				}
			}

			for (auto col : colonies) {
				col->_heap->clearHiked();
				col->doneHike();
			}

			for (auto nc : newColonies)
				colonies.push_back(nc);

			auto rndColonyPos = 0;
			if (flag_doll) {
				events << "Появилась неизвестная куколка через <" << rnd << "> дня мы узнаем в какую колонию она поселилась." << endl
					<< eventSep << endl;
				flag_doll = false;
			}
			if (currDay == 2 + rnd) {
				rndColonyPos = rand() % colonies.size();
				events << "Неизвестная куколка поселилась в колонию: " << colonies[rndColonyPos]->_name << " через <2> дня в этой колонии появится особое насекомое." << endl
					<< eventSep << endl;
			}
			if (currDay == rnd + 4) {
				auto rndSpecial = rand() % 100 + 1;
				if (rndSpecial > 50) {
					colonies[rndColonyPos]->addCreature(butGreen);
					events << "В колонии " << colonies[rndColonyPos]->_name << " появилось особое насекомое - Бабочка." << endl
						<< eventSep << endl;
				}
				else {
					colonies[rndColonyPos]->addCreature(waspRed);
					events << "В колонии " << colonies[rndColonyPos]->_name << " появилось особое насекомое - Оса." << endl
						<< eventSep << endl;
				}
			}


			cout << events.str();
		}

		if (currDay > days) {
			ui winIdx = 0;

			ui lastSum = 0;
			for (ui i = 0; i < colonies.size(); i++) {
				auto col = colonies[i];

				ui sum = 0;
				for (ui r = 0; r < col->_reses.size(); r++) {
					sum += col->_reses[r];
				}
				if (sum > lastSum) {
					winIdx = i;
					lastSum = sum;
				}
			}

			auto winner = colonies[winIdx];

			cout << "----------------------------------" << endl;
			cout << "---------Наступила засуха---------" << endl;
			cout << "--------------Выжила--------------" << endl;
			cout << "Название колонии: " << winner->_name << " | Информация: " << endl;
			for (auto ct : winner->_creatures) {
				cout << "\tТип существа: " << cTypeDesc[ct.first] << endl;
				for (auto pt : ct.second) {
					cout << "\t\tТип профессии: " << pTypeDesc[pt.first] << " | Количество: " << pt.second.size() << endl;
				}
			}
			cout << "Ресурсы: " << endl;
			cout << "\tВеточки: " << winner->_reses[0] <<
				"\n\tКамни: " << winner->_reses[1] <<
				"\n\tЛистья: " << winner->_reses[2] <<
				"\n\tРосы: " << winner->_reses[3] << endl;

			break;
		}

	} while (sInput != "3");

	return 0;
}

