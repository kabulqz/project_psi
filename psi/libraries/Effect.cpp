#include "Effect.hpp"
#include "Card.hpp"
#include "Hero.hpp"

static std::unordered_map<EffectCategory, std::vector<EffectTrigger>> triggerMap = {
	{EffectCategory::BUFF,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::DEBUFF,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::HEAL,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::DAMAGE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::STATUS_APPLY,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::KEYWORD_ADD,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::SILENCE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::STATUS_REMOVE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::SUMMON,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::CAST,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::EQUIP,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK}},
	{EffectCategory::DRAW,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::DISCARD,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::SHUFFLE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::STEAL,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::ENERGY_MODIFY,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}}
};
static std::unordered_map<EffectCategory, std::vector<EffectDuration>> durationMap = {
	{EffectCategory::BUFF, {EffectDuration::PERMANENT, EffectDuration::TURN_BASED, EffectDuration::EVENT_BASED}},
	{EffectCategory::DEBUFF, {EffectDuration::PERMANENT, EffectDuration::TURN_BASED, EffectDuration::EVENT_BASED}},
	{EffectCategory::HEAL, {EffectDuration::INSTANT, EffectDuration::TURN_BASED}},
	{EffectCategory::DAMAGE, {EffectDuration::INSTANT, EffectDuration::TURN_BASED}},
	{EffectCategory::STATUS_APPLY, {EffectDuration::PERMANENT, EffectDuration::TURN_BASED, EffectDuration::EVENT_BASED}},
	{EffectCategory::KEYWORD_ADD, {EffectDuration::PERMANENT, EffectDuration::TURN_BASED, EffectDuration::EVENT_BASED}},
	{EffectCategory::SILENCE, {EffectDuration::PERMANENT, EffectDuration::TURN_BASED, EffectDuration::EVENT_BASED}},
	{EffectCategory::STATUS_REMOVE, {EffectDuration::INSTANT}},
	{EffectCategory::SUMMON, {EffectDuration::INSTANT}},
	{EffectCategory::CAST, {EffectDuration::INSTANT}},
	{EffectCategory::EQUIP, {EffectDuration::INSTANT}},
	{EffectCategory::DRAW, {EffectDuration::INSTANT}},
	{EffectCategory::DISCARD, {EffectDuration::INSTANT}},
	{EffectCategory::SHUFFLE, {EffectDuration::INSTANT}},
	{EffectCategory::STEAL, {EffectDuration::INSTANT}},
	{EffectCategory::ENERGY_MODIFY, {EffectDuration::PERMANENT}}
};
static std::unordered_map<EffectCategory, std::vector<TargetMode>> targetModeMap = {
	{EffectCategory::BUFF,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::DEBUFF,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::HEAL,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::DAMAGE,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::STATUS_APPLY,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::KEYWORD_ADD,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::SILENCE,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::STATUS_REMOVE,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::SUMMON,{TargetMode::NONE}},
	{EffectCategory::CAST,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::EQUIP,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::DRAW,{TargetMode::NONE}},
	{EffectCategory::DISCARD,{TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::SHUFFLE,{TargetMode::SELF, TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::STEAL,{TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::ENERGY_MODIFY,{TargetMode::NONE}}
};
static std::unordered_map<EffectCategory, std::vector<TargetZone>> targetZoneMap = {
		{EffectCategory::BUFF,{TargetZone::HAND, TargetZone::DECK, TargetZone::BATTLEFIELD}},
		{EffectCategory::DEBUFF,{TargetZone::HAND, TargetZone::DECK, TargetZone::BATTLEFIELD}},
		{EffectCategory::HEAL,{TargetZone::BATTLEFIELD}},
		{EffectCategory::DAMAGE,{TargetZone::BATTLEFIELD}},
		{EffectCategory::STATUS_APPLY,{TargetZone::BATTLEFIELD}},
		{EffectCategory::KEYWORD_ADD,{TargetZone::HAND, TargetZone::DECK, TargetZone::BATTLEFIELD}},
		{EffectCategory::SILENCE,{TargetZone::BATTLEFIELD}},
		{EffectCategory::STATUS_REMOVE,{TargetZone::BATTLEFIELD}},
		{EffectCategory::SUMMON,{TargetZone::BATTLEFIELD}},
		{EffectCategory::CAST,{TargetZone::NONE}},
		{EffectCategory::EQUIP,{TargetZone::BATTLEFIELD}},
		{EffectCategory::DRAW,{TargetZone::NONE}},
		{EffectCategory::DISCARD,{TargetZone::NONE}},
		{EffectCategory::SHUFFLE,{TargetZone::HAND, TargetZone::DECK, TargetZone::BATTLEFIELD}},
		{EffectCategory::STEAL,{TargetZone::HAND, TargetZone::DECK}},
		{EffectCategory::ENERGY_MODIFY,{TargetZone::NONE}}
};

Effect::Effect(const uint_least32_t& effectSeed, const CardType cardType)
	: generator(effectSeed), cardType(cardType)
{
	// Losowanie kategorii efektu
	std::uniform_int_distribution categoryDistribution(0, static_cast<int>(EffectCategory::STEAL));
	category = static_cast<EffectCategory>(categoryDistribution(generator));

	// Losowanie triggera z mapy triggerów dla danej kategorii
	const auto& triggers = triggerMap[category];
	std::uniform_int_distribution triggerDistribution(0, static_cast<int>(triggers.size() - 1));
	trigger = triggers[triggerDistribution(generator)];

	// Losowanie typu duration
	const auto& durations = durationMap[category];
	std::uniform_int_distribution durationDistribution(0, static_cast<int>(durations.size() - 1));
	durationType = durations[durationDistribution(generator)];

	// Losowanie trybu targeta
	const auto& targetModes = targetModeMap[category];
	std::uniform_int_distribution targetModeDistribution(0, static_cast<int>(targetModes.size() - 1));
	targetMode = targetModes[targetModeDistribution(generator)];

	// Losowanie strefy targeta
	const auto& targetZones = targetZoneMap[category];
	std::uniform_int_distribution targetZoneDistribution(0, static_cast<int>(targetZones.size() - 1));
	targetZone = targetZones[targetZoneDistribution(generator)];

	// Losowanie grupy targeta
	std::uniform_int_distribution targetGroupDistribution(0, 2);
	targetGroup = static_cast<TargetGroup>(targetGroupDistribution(generator));

	using EffectBehaviorSetter = void (Effect::*)();
	std::unordered_map<EffectCategory, EffectBehaviorSetter> behaviorSetters = {
		{EffectCategory::BUFF, &Effect::setBuffBehavior},
		{EffectCategory::DEBUFF, &Effect::setDebuffBehavior},
		{EffectCategory::HEAL, &Effect::setHealBehavior},
		{EffectCategory::DAMAGE, &Effect::setDamageBehavior},
		{EffectCategory::STATUS_APPLY, &Effect::setStatusApplyBehavior},
		{EffectCategory::STATUS_REMOVE, &Effect::setStatusRemoveBehavior},
		{EffectCategory::KEYWORD_ADD, &Effect::setKeywordAddBehavior},
		{EffectCategory::SUMMON, &Effect::setSummonBehavior},
		{EffectCategory::CAST, &Effect::setCastBehavior},
		{EffectCategory::EQUIP, &Effect::setEquipBehavior},
		{EffectCategory::DRAW, &Effect::setDrawBehavior},
		{EffectCategory::DISCARD, &Effect::setDiscardBehavior},
		{EffectCategory::ENERGY_MODIFY, &Effect::setEnergyModifyBehavior},
		{EffectCategory::SILENCE, &Effect::setSilenceBehavior},
		{EffectCategory::SHUFFLE, &Effect::setShuffleBehavior},
		{EffectCategory::STEAL, &Effect::setStealBehavior}
	};

	if (const auto it = behaviorSetters.find(category); it != behaviorSetters.end())
	{
		(this->*it->second)();
	}
};

void Effect::setBuffBehavior()
{// target - card(unit. item, spell)
	if (cardType == CardType::SPELL)
	{
		if (trigger == EffectTrigger::ON_GAME_EVENT) trigger = EffectTrigger::WHEN_PLAYED;
		else if (trigger == EffectTrigger::ON_ATTACK) trigger = EffectTrigger::ON_DRAW;
		else if (trigger == EffectTrigger::ON_EQUIP) trigger = EffectTrigger::ON_DISCARD;
	}

	std::uniform_int_distribution<int> valueDistribution(2, 5);
	int value = valueDistribution(generator);

	StatType statType;
	std::unordered_map<CardType, std::vector<StatType>> selfStatsMap = {
		{CardType::UNIT, {StatType::HEALTH, StatType::ATTACK}},
		{CardType::ITEM, {StatType::DURABILITY, StatType::DEFENSE, StatType::DAMAGE}},
		{CardType::SPELL, {StatType::VALUE}}
	};

	if (targetMode == TargetMode::SELF && trigger == EffectTrigger::ON_DRAW) {
		// Dodaj ENERGY_COST do mapy dla SELF, jeœli chcesz, aby koszt energii móg³ byæ wybrany
		selfStatsMap[CardType::UNIT].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::ITEM].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::SPELL].push_back(StatType::ENERGY_COST);
	}

	std::vector<StatType> generalStats = {
		StatType::HEALTH, StatType::ATTACK, StatType::DEFENSE, StatType::DURABILITY, StatType::DAMAGE
	};

	if (targetMode != TargetMode::SELF && targetZone != TargetZone::BATTLEFIELD) {
		generalStats.push_back(StatType::VALUE);
		generalStats.push_back(StatType::ENERGY_COST);
	}

	if (targetMode == TargetMode::SELF) {
		// Jeœli targetMode == SELF, wybieramy statystykê z mapy selfStatsMap dla odpowiedniego CardType
		const auto& selfStats = selfStatsMap[cardType];
		std::uniform_int_distribution statDistribution(0, static_cast<int>(selfStats.size()) - 1);
		statType = selfStats[statDistribution(generator)];
	}
	else {
		// Jeœli targetMode != SELF, wybieramy statystykê z ogólnej puli
		std::uniform_int_distribution statDistribution(0, static_cast<int>(generalStats.size()) - 1);
		statType = generalStats[statDistribution(generator)];
	}

	//Sprawdzenie Triggera i ustawienie odpowiednich wartoœci
	if (trigger == EffectTrigger::ON_GAME_EVENT || trigger == EffectTrigger::ON_DRAW || trigger == EffectTrigger::ON_DISCARD || trigger == EffectTrigger::ON_ATTACK || trigger == EffectTrigger::ON_EQUIP)
	{
		if (trigger == EffectTrigger::ON_DISCARD)
		{
			if (targetMode == TargetMode::SELF) targetMode = TargetMode::RANDOM_SINGLE;
		}
	}

	// Losowanie liczby celów
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Ustalanie liczby tur
	std::optional<int> numberOfTurns = std::nullopt;
	if (durationType == EffectDuration::TURN_BASED) {
		std::uniform_int_distribution<int> numberOfTurnsDistribution(2, 5);
		numberOfTurns = numberOfTurnsDistribution(generator);
	}

	// Ustalanie triggerEvent
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;

		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<BuffBehavior>(trigger, durationType, value, statType, numberOfTargets, numberOfTurns, triggerEvent, endEvent);
}

void Effect::setDebuffBehavior()
{// target - card(unit. item, spell)
	if (cardType == CardType::SPELL)
	{
		if (trigger == EffectTrigger::ON_GAME_EVENT) trigger = EffectTrigger::WHEN_PLAYED;
		else if (trigger == EffectTrigger::ON_ATTACK) trigger = EffectTrigger::ON_DRAW;
		else if (trigger == EffectTrigger::ON_EQUIP) trigger = EffectTrigger::ON_DISCARD;
	}

	std::uniform_int_distribution<int> valueDistribution(2, 5);
	int value = valueDistribution(generator);

	StatType statType;
	std::unordered_map<CardType, std::vector<StatType>> selfStatsMap = {
		{CardType::UNIT, {StatType::HEALTH, StatType::ATTACK}},
		{CardType::ITEM, {StatType::DURABILITY, StatType::DEFENSE, StatType::DAMAGE}},
		{CardType::SPELL, {StatType::VALUE}}
	};

	if (targetMode == TargetMode::SELF && trigger == EffectTrigger::ON_DRAW) {
		// Dodaj ENERGY_COST do mapy dla SELF, jeœli chcesz, aby koszt energii móg³ byæ wybrany
		selfStatsMap[CardType::UNIT].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::ITEM].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::SPELL].push_back(StatType::ENERGY_COST);
	}

	std::vector<StatType> generalStats = {
		StatType::HEALTH, StatType::ATTACK, StatType::DEFENSE, StatType::DURABILITY, StatType::DAMAGE
	};

	if (targetMode != TargetMode::SELF && targetZone != TargetZone::BATTLEFIELD) {
		generalStats.push_back(StatType::VALUE);
		generalStats.push_back(StatType::ENERGY_COST);
	}

	if (targetMode == TargetMode::SELF) {
		// Jeœli targetMode == SELF, wybieramy statystykê z mapy selfStatsMap dla odpowiedniego CardType
		const auto& selfStats = selfStatsMap[cardType];
		std::uniform_int_distribution statDistribution(0, static_cast<int>(selfStats.size()) - 1);
		statType = selfStats[statDistribution(generator)];
	}
	else {
		// Jeœli targetMode != SELF, wybieramy statystykê z ogólnej puli
		std::uniform_int_distribution statDistribution(0, static_cast<int>(generalStats.size()) - 1);
		statType = generalStats[statDistribution(generator)];
	}

	//Sprawdzenie Triggera i ustawienie odpowiednich wartoœci
	if (trigger == EffectTrigger::ON_GAME_EVENT || trigger == EffectTrigger::ON_DRAW || trigger == EffectTrigger::ON_DISCARD || trigger == EffectTrigger::ON_ATTACK || trigger == EffectTrigger::ON_EQUIP)
	{
		if (trigger == EffectTrigger::ON_DISCARD)
		{
			if (targetMode == TargetMode::SELF) targetMode = TargetMode::RANDOM_SINGLE;
		}
	}

	// Losowanie liczby celów
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Ustalanie liczby tur
	std::optional<int> numberOfTurns = std::nullopt;
	if (durationType == EffectDuration::TURN_BASED) {
		std::uniform_int_distribution<int> numberOfTurnsDistribution(2, 5);
		numberOfTurns = numberOfTurnsDistribution(generator);
	}

	// Ustalanie triggerEvent
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;

		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<DebuffBehavior>(trigger, durationType, value, statType, numberOfTargets, numberOfTurns, triggerEvent, endEvent);
}

void Effect::setHealBehavior()
{// target - card(unit) or hero
	std::uniform_int_distribution<int> valueDistribution(1, 7);
	int value = valueDistribution(generator);

	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	std::optional<int> numberOfTurns = std::nullopt;
	if (durationType == EffectDuration::TURN_BASED){
		std::uniform_int_distribution<int> numberOfTurnsDistribution(2, 5);
		numberOfTurns = numberOfTurnsDistribution(generator);
	}

	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<HealBehavior>(trigger, durationType, value, numberOfTargets, numberOfTurns, triggerEvent);
}

void Effect::setDamageBehavior()
{// target - card(unit) or hero
	std::uniform_int_distribution<int> valueDistribution(1, 7);
	int value = valueDistribution(generator);

	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	std::optional<int> numberOfTurns = std::nullopt;
	if (durationType == EffectDuration::TURN_BASED) {
		std::uniform_int_distribution<int> numberOfTurnsDistribution(2, 5);
		numberOfTurns = numberOfTurnsDistribution(generator);
	}

	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::HERO_HEALED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<DamageBehavior>(trigger, durationType, value, numberOfTargets, numberOfTurns, triggerEvent);
}

void Effect::setStatusApplyBehavior()
{// target - card(unit)

}

void Effect::setKeywordAddBehavior()
{// target - card(unit)

}

void Effect::setSilenceBehavior()
{// target - card(unit)

}

void Effect::setStatusRemoveBehavior()
{// target - card(unit)

}

void Effect::setSummonBehavior()
{// target - no target, summons on battlefield

}

void Effect::setCastBehavior()
{// target - no target, casts spell

}

void Effect::setEquipBehavior()
{// target - card(unit)

}

void Effect::setDrawBehavior()
{// target - no target, draws card

}

void Effect::setDiscardBehavior()
{// target - card(unit, item, spell)

}

void Effect::setShuffleBehavior()
{// target - card(unit, item, spell) to shuffle into deck

}

void Effect::setStealBehavior()
{// target - card(unit, item, spell) to steal from opponent

}

void Effect::setEnergyModifyBehavior()
{// target - hero, modifies energy of player

}