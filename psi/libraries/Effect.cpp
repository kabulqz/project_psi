#include "Effect.hpp"
#include "Card.hpp"
#include "Hero.hpp"

std::string GameEventToString(const GameEvent gameEvent)
{
	switch (gameEvent)
	{
	case GameEvent::TURN_START: return "start of a turn";
	case GameEvent::TURN_END: return "end of a turn";
	case GameEvent::CARD_PLAYED: return "card is played";
	case GameEvent::CARD_DRAWN: return "card is drawn";
	case GameEvent::CARD_DISCARDED: return "card is discarded";
	case GameEvent::CARD_SHUFFLED: return "card is shuffled";
	case GameEvent::CARD_STOLEN: return "card is stolen";
	case GameEvent::UNIT_ATTACKED: return "unit has attacked";
	case GameEvent::UNIT_DAMAGED: return "unit has been damaged";
	case GameEvent::UNIT_HEALED: return "unit has been healed";
	case GameEvent::UNIT_DEATH: return "unit has died";
	case GameEvent::UNIT_STATUS_APPLY: return "unit had status applied";
	case GameEvent::UNIT_STATUS_REMOVE: return "unit had status removed";
	case GameEvent::UNIT_KEYWORD_ADD: return "unit had keyword added";
	case GameEvent::ITEM_EQUIPPED: return "item is equipped";
	case GameEvent::ITEM_DESTROYED: return "item is destroyed";
	}
	return "";
}

static std::string StatToString(const StatType statType)
{
	switch (statType)
	{
	case StatType::HEALTH: return "Health";
	case StatType::ATTACK: return "Attack";
	case StatType::DEFENSE: return "Defense";
	case StatType::DURABILITY: return "Durability";
	case StatType::DAMAGE: return "Damage";
	case StatType::ENERGY_COST: return "Energy Cost";
	}
	return "";
}

std::string StatusToString(Status status)
{
	switch (status)
	{
	case Status::BLEEDING: return "Bleeding";
	case Status::BURNING: return "Burning";
	case Status::POISONED: return "Poisoned";
	case Status::CONFUSED: return "Confused";
	case Status::CURSED: return "Cursed";
	case Status::ENRAGED: return "Enraged";
	case Status::FROZEN: return "Frozen";
	case Status::HEXED: return "Hexed";
	case Status::MARKED: return "Marked";
	case Status::ROCK_SKINNED: return "Rock Skinned";
	case Status::STUNNED: return "Stunned";
	}
}

std::string KeywordToString(Keyword keyword)
{
	switch (keyword)
	{
	case Keyword::BERSERK: return "BERSERK";
	case Keyword::BLOODTHIRSTY: return "BLOODTHIRSTY";
	case Keyword::DEFENDER: return "DEFENDER";
	case Keyword::DOUBLE_STRIKE: return "DOUBLE STRIKE";
	case Keyword::ICEBREAKER: return "ICEBREAKER";
	case Keyword::IMMUNE: return "IMMUNE";
	case Keyword::INSTANT_ATTACK: return "INSTANT ATTACK";
	case Keyword::INSTANT_KILLER: return "INSTANT KILLER";
	case Keyword::LIFE_STEAL: return "LIFE STEAL";
	case Keyword::MASTER_OF_ARMS: return "MASTER OF ARMS";
	case Keyword::PYROMANIAC: return "PYROMANIAC";
	case Keyword::RANGED: return "RANGED";
	case Keyword::UNSTOPPABLE: return "UNSTOPPABLE";
	}
}

void Effect::setBuffBehavior()
{// target - card(unit. item, spell)
	std::uniform_int_distribution<int> valueDistribution(2, 5);
	int value = valueDistribution(generator);

	StatType statType;
	std::unordered_map<CardType, std::vector<StatType>> selfStatsMap = {
		{CardType::UNIT, {StatType::HEALTH, StatType::ATTACK}},
		{CardType::ITEM, {StatType::DURABILITY, StatType::DEFENSE, StatType::DAMAGE}},
		{CardType::SPELL, {StatType::VALUE}} // does nothing, just to be so the program compiles
	};

	if (targetMode == TargetMode::SELF && trigger == EffectTrigger::ON_DRAW) {
		// Dodaj ENERGY_COST do mapy dla SELF, jeśli chcesz, aby koszt energii mógł być wybrany
		selfStatsMap[CardType::UNIT].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::ITEM].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::SPELL].push_back(StatType::ENERGY_COST);
	}

	std::vector<StatType> generalStats = {
		StatType::HEALTH, StatType::ATTACK, StatType::DEFENSE, StatType::DURABILITY, StatType::DAMAGE
	};

	if (targetMode != TargetMode::SELF && targetZone != TargetZone::BATTLEFIELD) {
		generalStats.push_back(StatType::ENERGY_COST);
	}

	if (targetMode == TargetMode::SELF) {
		// Jeśli targetMode == SELF, wybieramy statystykę z mapy selfStatsMap dla odpowiedniego CardType
		const auto& selfStats = selfStatsMap[cardType];
		std::uniform_int_distribution statDistribution(0, static_cast<int>(selfStats.size() - 1));
		statType = selfStats[statDistribution(generator)];
	}
	else {
		// Jeśli targetMode != SELF, wybieramy statystykę z ogólnej puli
		std::uniform_int_distribution statDistribution(0, static_cast<int>(generalStats.size() - 1));
		statType = generalStats[statDistribution(generator)];
	}

	//Sprawdzenie Triggera i ustawienie odpowiednich wartości
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<BuffBehavior>(trigger, durationType, value, statType, numberOfTargets, numberOfTurns, triggerEvent, endEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}
	if (statType == StatType::ENERGY_COST) {
		description += "reduces " + StatToString(statType) + " by " + std::to_string(value) + ", ";
	}
	else {
		description += "increases " + StatToString(statType) + " by " + std::to_string(value) + ", ";
	}
	description += "\ntargets ";

	switch (targetMode) {
	case TargetMode::SELF: description += "the card itself"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single other card"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " other cards"; break;
	}
	description += " in ";

	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ",\n";

	switch (durationType) {
	case EffectDuration::PERMANENT: description += "effect lasts forever"; break;
	case EffectDuration::TURN_BASED: description += "effect lasts for " + std::to_string(numberOfTurns.value()) + " turns"; break;
	case EffectDuration::EVENT_BASED: description += "effect lasts until " + GameEventToString(endEvent.value()); break;
	}
	description += ".";

	if (statType == StatType::VALUE) description = "";
}

void Effect::setDebuffBehavior()
{// target - card(unit. item, spell)
	std::uniform_int_distribution<int> valueDistribution(2, 5);
	int value = valueDistribution(generator);

	StatType statType;
	std::unordered_map<CardType, std::vector<StatType>> selfStatsMap = {
		{CardType::UNIT, {StatType::HEALTH, StatType::ATTACK}},
		{CardType::ITEM, {StatType::DURABILITY, StatType::DEFENSE, StatType::DAMAGE}},
		{CardType::SPELL, {StatType::VALUE}} // does nothing, just to be so the program compiles
	};

	if (targetMode == TargetMode::SELF && trigger == EffectTrigger::ON_DRAW) {
		// Dodaj ENERGY_COST do mapy dla SELF, jeśli chcesz, aby koszt energii mógł być wybrany
		selfStatsMap[CardType::UNIT].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::ITEM].push_back(StatType::ENERGY_COST);
		selfStatsMap[CardType::SPELL].push_back(StatType::ENERGY_COST);
	}

	std::vector<StatType> generalStats = {
		StatType::HEALTH, StatType::ATTACK, StatType::DEFENSE, StatType::DURABILITY, StatType::DAMAGE
	};

	if (targetMode != TargetMode::SELF && targetZone != TargetZone::BATTLEFIELD) {
		generalStats.push_back(StatType::ENERGY_COST);
	}

	if (targetMode == TargetMode::SELF) {
		// Jeśli targetMode == SELF, wybieramy statystykę z mapy selfStatsMap dla odpowiedniego CardType
		const auto& selfStats = selfStatsMap[cardType];
		std::uniform_int_distribution statDistribution(0, static_cast<int>(selfStats.size() - 1));
		statType = selfStats[statDistribution(generator)];
	}
	else {
		// Jeśli targetMode != SELF, wybieramy statystykę z ogólnej puli
		std::uniform_int_distribution statDistribution(0, static_cast<int>(generalStats.size() - 1));
		statType = generalStats[statDistribution(generator)];
	}

	//Sprawdzenie Triggera i ustawienie odpowiednich wartości
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<DebuffBehavior>(trigger, durationType, value, statType, numberOfTargets, numberOfTurns, triggerEvent, endEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}
	if (statType == StatType::ENERGY_COST) {
		description += "increases " + StatToString(statType) + " by " + std::to_string(value) + ", ";
	}
	else {
		description += "reduces " + StatToString(statType) + " by " + std::to_string(value) + ", ";
	}
	description += "\ntargets ";

	switch (targetMode) {
	case TargetMode::SELF: description += "the card itself"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single other card"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " other cards"; break;
	}
	description += " in ";

	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ",\n";

	switch (durationType) {
	case EffectDuration::PERMANENT: description += "effect lasts forever"; break;
	case EffectDuration::TURN_BASED: description += "effect lasts for " + std::to_string(numberOfTurns.value()) + " turns"; break;
	case EffectDuration::EVENT_BASED: description += "effect lasts until " + GameEventToString(endEvent.value()); break;
	}
	description += ".";

	if (statType == StatType::VALUE) description = "";
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
	if (durationType == EffectDuration::TURN_BASED) {
		std::uniform_int_distribution<int> numberOfTurnsDistribution(2, 5);
		numberOfTurns = numberOfTurnsDistribution(generator);
	}

	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<HealBehavior>(trigger, durationType, value, numberOfTargets, numberOfTurns, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}
	description += "\nrestores " + std::to_string(value) + " health, ";

	switch (durationType) {
	case EffectDuration::INSTANT: description += "effect is applied instantly."; break;
	case EffectDuration::TURN_BASED: description += "effect applies every turn for " + std::to_string(numberOfTurns.value()) + " turns. "; break;
	}

	description += "\nTargets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this card"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit or hero"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units or heroes"; break;
	}
	description += " in ";

	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<DamageBehavior>(trigger, durationType, value, numberOfTargets, numberOfTurns, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}
	description += "\ndeals " + std::to_string(value) + " damage, ";

	switch (durationType) {
	case EffectDuration::INSTANT: description += "effect is applied instantly."; break;
	case EffectDuration::TURN_BASED: description += "effect applies every turn for " + std::to_string(numberOfTurns.value()) + " turns. "; break;
	}

	description += "\nTargets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this card"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit or hero"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units or heroes"; break;
	}
	description += " in ";

	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
}

void Effect::setStatusApplyBehavior()
{// target - card(unit)
	// Najpierw - jaki status ma być nałożony
	std::uniform_int_distribution<int> statusDistribution(
		static_cast<int>(Status::BLEEDING),
		static_cast<int>(Status::STUNNED)
	);
	auto status = static_cast<Status>(statusDistribution(generator));

	// Losowanie liczby celów
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Losowanie liczby tur
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<StatusApplyBehavior>(trigger, durationType, status, numberOfTargets, numberOfTurns, triggerEvent, endEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}
	description += "\napplies \"" + StatusToString(status) + "\", ";

	switch (durationType) {
	case EffectDuration::INSTANT: description += "effect is applied instantly. "; break;
	case EffectDuration::PERMANENT: description += "effect lasts forever. "; break;
	case EffectDuration::TURN_BASED: description += "effect applies every turn for " + std::to_string(numberOfTurns.value()) + " turns. "; break;
	case EffectDuration::EVENT_BASED: description += "effect lasts until " + GameEventToString(endEvent.value()) + ". "; break;
	}

	description += "\nTargets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this unit"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units"; break;
	}

	description += " in ";
	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
}

void Effect::setKeywordAddBehavior()
{// target - card(unit)
	// Najpierw - jaki keyword ma by� dodany
	std::uniform_int_distribution<int> keywordDistribution(
		static_cast<int>(Keyword::BERSERK),
		static_cast<int>(Keyword::UNSTOPPABLE)
	);
	auto keyword = static_cast<Keyword>(keywordDistribution(generator));

	// Losowanie liczby cel�w
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Losowanie liczby tur
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<KeywordAddBehavior>(trigger, durationType, keyword, numberOfTargets, numberOfTurns, triggerEvent, endEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\nadds keyword \"" + KeywordToString(keyword) + "\", ";
	switch (durationType) {
	case EffectDuration::INSTANT: description += "effect is applied instantly. "; break;
	case EffectDuration::PERMANENT: description += "effect lasts forever. "; break;
	case EffectDuration::TURN_BASED: description += "effect applies every turn for " + std::to_string(numberOfTurns.value()) + " turns. "; break;
	case EffectDuration::EVENT_BASED: description += "effect lasts until " + GameEventToString(endEvent.value()) + ". "; break;
	}

	description += "\nTargets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this unit"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units"; break;
	}

	description += " in ";
	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
}

void Effect::setSilenceBehavior()
{// target - card(unit, item)
	auto status = Status::SILENCED;

	// Losowanie liczby celów
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Losowanie liczby tur
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
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	// Ustalanie endEvent
	std::optional<GameEvent> endEvent = std::nullopt;
	if (durationType == EffectDuration::EVENT_BASED) {
		std::uniform_int_distribution<int> endEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		endEvent = static_cast<GameEvent>(endEventDistribution(generator));
	}

	behavior = std::make_unique<SilenceBehavior>(trigger, durationType, numberOfTargets, numberOfTurns, triggerEvent, endEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\nsilences the card, ";

	switch (durationType) {
	case EffectDuration::INSTANT: description += "effect is applied instantly. "; break;
	case EffectDuration::PERMANENT: description += "effect lasts forever. "; break;
	case EffectDuration::TURN_BASED: description += "effect applies every turn for " + std::to_string(numberOfTurns.value()) + " turns. "; break;
	case EffectDuration::EVENT_BASED: description += "effect lasts until " + GameEventToString(endEvent.value()) + ". "; break;
	}

	description += "\nTargets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this unit"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units"; break;
	}

	description += " in ";
	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
}

void Effect::setStatusRemoveBehavior()
{// target - card(unit)

	// Najpierw - jaki status ma być usuwany
	std::uniform_int_distribution<int> statusDistribution(
		static_cast<int>(Status::BLEEDING),
		static_cast<int>(Status::STUNNED)
	);
	Status status = static_cast<Status>(statusDistribution(generator));

	// Losowanie liczby celów
	int numberOfTargets;
	if (targetMode == TargetMode::SELF || targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfTargets = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfTargetsDistribution(2, 3);
		numberOfTargets = numberOfTargetsDistribution(generator);
	}

	// Ustalanie triggerEvent
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		targetZone = TargetZone::BATTLEFIELD;
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<StatusRemoveBehavior>(trigger, durationType, status, numberOfTargets, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\nremoves \"" + StatusToString(status) + "\", ";
	description += "targets ";
	switch (targetMode) {
	case TargetMode::SELF: description += "this unit"; break;
	case TargetMode::RANDOM_SINGLE: description += "a random single unit"; break;
	case TargetMode::RANDOM_MULTIPLE: description += "a random " + std::to_string(numberOfTargets) + " units"; break;
	}

	description += " in ";
	switch (targetZone) {
	case TargetZone::HAND: description += "the hand"; break;
	case TargetZone::DECK: description += "the deck"; break;
	case TargetZone::BATTLEFIELD: description += "the battlefield"; break;
	}
	description += ".";
}

void Effect::setDrawBehavior()
{// target - no target, draws card
	std::uniform_int_distribution<int> valueDistribution(1, 3);
	int numberOfCards = valueDistribution(generator);

	//trigger
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<DrawBehavior>(trigger, durationType, targetGroup, numberOfCards, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\n";

	switch (targetGroup) {
	case TargetGroup::ALLY: description += "Allied hero draws " + std::to_string(numberOfCards) + " cards."; break;
	case TargetGroup::ENEMY: description += "Enemy hero draws " + std::to_string(numberOfCards) + " cards."; break;
	case TargetGroup::BOTH: description += "Both heroes draw " + std::to_string(numberOfCards) + " cards."; break;
	}
}

void Effect::setDiscardBehavior()
{// target - card(unit, item, spell) in hand
	if (trigger == EffectTrigger::ON_DISCARD) trigger = EffectTrigger::WHEN_PLAYED;

	int numberOfCards;
	if (targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfCards = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfCardsDistribution(2, 3);
		numberOfCards = numberOfCardsDistribution(generator);
	}

	//trigger
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<DiscardBehavior>(trigger, durationType, targetGroup, numberOfCards, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\n";

	switch (targetGroup) {
	case TargetGroup::ALLY: description += "Allied hero discards " + std::to_string(numberOfCards) + " cards from hand."; break;
	case TargetGroup::ENEMY: description += "Enemy hero discards " + std::to_string(numberOfCards) + " cards from hand."; break;
	case TargetGroup::BOTH: description += "Both heroes discard " + std::to_string(numberOfCards) + " cards from hand."; break;
	}
}

void Effect::setShuffleBehavior()
{// target - card(unit, item, spell) to shuffle into deck
	int numberOfCards;
	if (targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfCards = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfCardsDistribution(2, 3);
		numberOfCards = numberOfCardsDistribution(generator);
	}

	if (targetGroup == TargetGroup::BOTH) {
		targetGroup = TargetGroup::ALLY;
	}

	//trigger
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<ShuffleBehavior>(trigger, durationType, targetGroup, numberOfCards, targetZone, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\n";

	switch (targetGroup) {
	case TargetGroup::ALLY: description += "Allied hero shuffles " + std::to_string(numberOfCards) + " cards into deck "; break;
	case TargetGroup::ENEMY: description += "Enemy hero shuffles " + std::to_string(numberOfCards) + " cards into deck "; break;
	}

	switch (targetZone) {
	case TargetZone::HAND: description += "from their hand."; break;
	case TargetZone::BATTLEFIELD: description += "from their battlefield."; break;
	}
}

void Effect::setStealBehavior()
{// target - card(unit, item, spell) to steal from opponent
	int numberOfCards;
	if (targetMode == TargetMode::RANDOM_SINGLE) {
		numberOfCards = 1;
	}
	else if (targetMode == TargetMode::RANDOM_MULTIPLE) {
		std::uniform_int_distribution<int> numberOfCardsDistribution(2, 3);
		numberOfCards = numberOfCardsDistribution(generator);
	}

	// trigger
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<StealBehavior>(trigger, durationType, numberOfCards, targetZone, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\nsteals " + std::to_string(numberOfCards) + " cards from opponents ";

	switch (targetZone) {
	case TargetZone::HAND: description += "hand."; break;
	case TargetZone::DECK: description += "deck."; break;
	}
}

void Effect::setEnergyModifyBehavior()
{// target - hero, modifies energy of player
	std::uniform_int_distribution<int> valueDistribution(1, 2);
	int value = valueDistribution(generator);

	if (trigger == EffectTrigger::ON_GAME_EVENT || trigger == EffectTrigger::ON_ATTACK) {
		value = 1; // ustawiamy na jeden bo te triggery są wywoływane częściej
	}

	// trigger
	std::optional<GameEvent> triggerEvent = std::nullopt;
	if (trigger == EffectTrigger::ON_GAME_EVENT) {
		std::uniform_int_distribution<int> triggerEventDistribution(
			static_cast<int>(GameEvent::TURN_START),
			static_cast<int>(GameEvent::ITEM_DESTROYED)
		);
		triggerEvent = static_cast<GameEvent>(triggerEventDistribution(generator));
	}

	behavior = std::make_unique<EnergyModifyBehavior>(trigger, durationType, targetGroup, value, triggerEvent);

	switch (trigger) {
	case EffectTrigger::WHEN_PLAYED: description += "Triggered immediately when the card is played, "; break;
	case EffectTrigger::ON_DRAW: description += "Triggered when a card is drawn, "; break;
	case EffectTrigger::ON_GAME_EVENT: description += "Triggered when " + GameEventToString(triggerEvent.value()) + ", "; break;
	case EffectTrigger::ON_DISCARD: description += "Triggered when a card is discarded, "; break;
	case EffectTrigger::ON_ATTACK: description += "Triggered when a unit attacks, "; break;
	case EffectTrigger::ON_EQUIP: description += "Triggered when an item is equipped, "; break;
	}

	description += "\nadds " + std::to_string(value) + " to max energy.";
}

static std::unordered_map<EffectCategory, std::vector<EffectTrigger>> triggerMap = {
	{EffectCategory::BUFF,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::DEBUFF,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::HEAL,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::DAMAGE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::STATUS_APPLY,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::KEYWORD_ADD,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::SILENCE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
	{EffectCategory::STATUS_REMOVE,{EffectTrigger::WHEN_PLAYED, EffectTrigger::ON_GAME_EVENT, EffectTrigger::ON_DRAW, EffectTrigger::ON_DISCARD, EffectTrigger::ON_ATTACK, EffectTrigger::ON_EQUIP}},
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
	{EffectCategory::DRAW,{TargetMode::NONE}},
	{EffectCategory::DISCARD,{TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
	{EffectCategory::SHUFFLE,{TargetMode::RANDOM_SINGLE, TargetMode::RANDOM_MULTIPLE}},
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
		{EffectCategory::DRAW,{TargetZone::NONE}},
		{EffectCategory::DISCARD,{TargetZone::HAND}},
		{EffectCategory::SHUFFLE,{TargetZone::HAND, TargetZone::BATTLEFIELD}},
		{EffectCategory::STEAL,{TargetZone::HAND, TargetZone::DECK}},
		{EffectCategory::ENERGY_MODIFY,{TargetZone::NONE}}
};

Effect::Effect(const uint_least32_t& effectSeed, const CardType cardType)
	: generator(effectSeed), cardType(cardType)
{
	description = "";

	// Losowanie kategorii efektu
	std::uniform_int_distribution categoryDistribution(0, static_cast<int>(EffectCategory::STEAL));
	category = static_cast<EffectCategory>(categoryDistribution(generator));

	// Losowanie triggera z mapy triggerów dla danej kategorii
	const auto& triggers = triggerMap[category];
	std::uniform_int_distribution triggerDistribution(0, static_cast<int>(triggers.size() - 1));
	trigger = triggers[triggerDistribution(generator)];

	// Jeśli karta jest zaklęciem, zmień trigger na odpowiedni
	if (cardType == CardType::SPELL)
	{
		if (trigger == EffectTrigger::ON_GAME_EVENT) trigger = EffectTrigger::WHEN_PLAYED;
		else if (trigger == EffectTrigger::ON_ATTACK) trigger = EffectTrigger::ON_DRAW;
		else if (trigger == EffectTrigger::ON_EQUIP) trigger = EffectTrigger::ON_DISCARD;
	}
	// Jeśli karta jest przedmiotem oraz jeśli trigger jest ON_ATTACK, zmień go na ON_EQUIP
	if (cardType == CardType::ITEM)
	{
		if (trigger == EffectTrigger::ON_ATTACK) trigger = EffectTrigger::ON_EQUIP;
	}
	// Jeśli karta jest jednostką oraz jeśli trigger jest ON_EQUIP, zmień go na ON_ATTACK
	if (cardType == CardType::UNIT)
	{
		if (trigger == EffectTrigger::ON_EQUIP) trigger = EffectTrigger::ON_ATTACK;
	}

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

	// Jesli kategoria to SHUFFLE lub STEAL, zmien grupe targeta na odpowiednia
	if (targetGroup == TargetGroup::BOTH) {
		if (category == EffectCategory::SHUFFLE) {
			targetGroup = TargetGroup::ALLY;
		}
		if (category == EffectCategory::STEAL) {
			targetGroup = TargetGroup::ENEMY;
		}
	}

	using EffectBehaviorSetter = void (Effect::*)();
	std::unordered_map<EffectCategory, EffectBehaviorSetter> behaviorSetters = {
		{EffectCategory::BUFF, &Effect::setBuffBehavior},
		{EffectCategory::DEBUFF, &Effect::setDebuffBehavior},
		{EffectCategory::HEAL, &Effect::setHealBehavior},
		{EffectCategory::DAMAGE, &Effect::setDamageBehavior},
		{EffectCategory::STATUS_APPLY, &Effect::setStatusApplyBehavior},
		{EffectCategory::STATUS_REMOVE, &Effect::setStatusRemoveBehavior},
		{EffectCategory::KEYWORD_ADD, &Effect::setKeywordAddBehavior},
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
}

void Effect::executeEffect(Card* card, EffectTrigger trigger, std::optional<GameEvent> event)
{
	if (trigger != EffectTrigger::ON_GAME_EVENT) {
		std::optional<GameEvent> triggerEvent = std::nullopt;
		switch (trigger) {
		case EffectTrigger::WHEN_PLAYED: triggerEvent = GameEvent::CARD_PLAYED; break;
		case EffectTrigger::ON_DRAW: triggerEvent = GameEvent::CARD_DRAWN; break;
		case EffectTrigger::ON_DISCARD: triggerEvent = GameEvent::CARD_DISCARDED; break;
		case EffectTrigger::ON_ATTACK: triggerEvent = GameEvent::UNIT_ATTACKED; break;
		case EffectTrigger::ON_EQUIP: triggerEvent = GameEvent::ITEM_EQUIPPED; break;
		}
		switch (category) {
		case EffectCategory::DRAW: behavior->execute(*card); break;
		case EffectCategory::DISCARD: behavior->execute(*card); break;
		case EffectCategory::SHUFFLE: behavior->execute(*card); break;
		case EffectCategory::STEAL: behavior->execute(*card); break;
		case EffectCategory::ENERGY_MODIFY: behavior->execute(*card); break;
		}
		if (behavior->getNumberOfTargets().has_value())
		{
			if (this->targetMode == TargetMode::SELF)
			{
				// if the target is self, we call the behavior on the card
				behavior->execute(*card);
			}
			else if (this->targetMode == TargetMode::RANDOM_SINGLE || this->targetMode == TargetMode::RANDOM_MULTIPLE)
			{
				int numberOfTargets = behavior->getNumberOfTargets().value();
				// if the target is random single, we look for 1 random target
				// if the target is random multiple, we look for 2-3 random targets
				// vector of possible targets
				std::vector<Target*> possibleTargets;
				// for HEAL and DAMAGE, we look for units and heroes
				if (category == EffectCategory::HEAL || category == EffectCategory::DAMAGE) {
					// we look for units on battlefield and heroes
					// if target group ALLY we pass to possible targets all ally units and ally hero
					// if target group ENEMY we pass to possible targets all enemy units and enemy hero
					// if target group BOTH we pass to possible targets all units and heroes from both groups
					if (this->targetGroup == TargetGroup::ALLY) {
						// we pass to possible
						for (const auto allyUnit : card->getOwner()->getBattlefield()) {
							possibleTargets.push_back(allyUnit);
						}
						possibleTargets.push_back(card->getOwner());
					}
					else if (this->targetGroup == TargetGroup::ENEMY) {
						for (const auto enemyUnit : card->getEnemy()->getBattlefield()) {
							possibleTargets.push_back(enemyUnit);
						}
						possibleTargets.push_back(card->getEnemy());
					}
					else if (this->targetGroup == TargetGroup::BOTH) {
						for (const auto allyUnit : card->getOwner()->getBattlefield()) {
							possibleTargets.push_back(allyUnit);
						}
						for (const auto enemyUnit : card->getEnemy()->getBattlefield()) {
							possibleTargets.push_back(enemyUnit);
						}
						possibleTargets.push_back(card->getOwner());
						possibleTargets.push_back(card->getEnemy());
					}
				}
				else if (category == EffectCategory::BUFF || category == EffectCategory::DEBUFF) {
					// if targetGroup is ALLY we look for cards from ally hero
					// if targetGroup is ENEMY we look for cards from enemy hero
					// if targetGroup is BOTH we look for cards from both heroes
					if (behavior->getStatType().has_value()) {
						// we look for cards in all zones depending on what stat we want to buff or debuff
						StatType statType = behavior->getStatType().value();
						// if statType is ENERGY_COST we look for all cards in hand and deck
						// if statType is VALUE we look for all spell cards in hand and deck
						// if statType is HEALTH, ATTACK we look for all units in hand, deck and battlefield
						// if statType is DURABILITY, DAMAGE, DEFENSE we look for all items in hand, deck and battlefield

						if (this->targetGroup == TargetGroup::ALLY)
						{
							if (statType == StatType::ENERGY_COST) {
								for (const auto card : card->getOwner()->getHand()) {
									possibleTargets.push_back(card);
								}
								for (const auto card : card->getOwner()->getDeck()) {
									possibleTargets.push_back(card);
								}
							}
							else if (statType == StatType::VALUE) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
							}
							else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto unit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
							else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getOwner()->getBattlefield()) {
									if (const auto unit = dynamic_cast<UnitCard*>(card)) {
										if (unit->hasItem()) {
											possibleTargets.push_back(unit->getItem());
										}
									}
								}
							}
						}
						else if (this->targetGroup == TargetGroup::ENEMY)
						{
							if (statType == StatType::ENERGY_COST)
							{
								for (const auto card : card->getEnemy()->getHand()) {
									possibleTargets.push_back(card);
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									possibleTargets.push_back(card);
								}
							}
							else if (statType == StatType::VALUE) {
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
							}
							else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto unit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
							else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getEnemy()->getBattlefield()) {
									if (const auto unit = dynamic_cast<UnitCard*>(card)) {
										if (unit->hasItem()) {
											possibleTargets.push_back(unit->getItem());
										}
									}
								}
							}
						}
						else if (this->targetGroup == TargetGroup::BOTH)
						{
							if (statType == StatType::ENERGY_COST) {
								for (const auto card : card->getOwner()->getHand()) {
									possibleTargets.push_back(card);
								}
								for (const auto card : card->getEnemy()->getHand()) {
									possibleTargets.push_back(card);
								}
								for (const auto card : card->getOwner()->getDeck()) {
									possibleTargets.push_back(card);
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									possibleTargets.push_back(card);
								}
							}
							else if (statType == StatType::VALUE) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto spell = dynamic_cast<SpellCard*>(card)) {
										possibleTargets.push_back(spell);
									}
								}
							}
							else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto unit = dynamic_cast<UnitCard*>(card)) {
										possibleTargets.push_back(unit);
									}
								}
								for (const auto unit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
								for (const auto unit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
							else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
								for (const auto card : card->getOwner()->getHand()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getEnemy()->getHand()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getOwner()->getDeck()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getEnemy()->getDeck()) {
									if (auto item = dynamic_cast<ItemCard*>(card)) {
										possibleTargets.push_back(item);
									}
								}
								for (const auto card : card->getOwner()->getBattlefield()) {
									if (const auto unit = dynamic_cast<UnitCard*>(card)) {
										if (unit->hasItem()) {
											possibleTargets.push_back(unit->getItem());
										}
									}
								}
								for (const auto card : card->getEnemy()->getBattlefield()) {
									if (const auto unit = dynamic_cast<UnitCard*>(card)) {
										if (unit->hasItem()) {
											possibleTargets.push_back(unit->getItem());
										}
									}
								}
							}
						}
					}
				}
				else if (category == EffectCategory::KEYWORD_ADD) {
					// if targetGroup is ALLY then we look for units in ally deck, hand, battlefield
					// if targetGroup is ENEMY then we look for units in enemy deck, hand, battlefield
					// if targetGroup is BOTH then we look for units in both decks, hands, battlefields
					if (behavior->getKeyword().has_value()) {
						if (targetGroup == TargetGroup::ALLY) {
							for (const auto card : card->getOwner()->getHand()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto card : card->getOwner()->getDeck()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto unit : card->getOwner()->getBattlefield()) {
								possibleTargets.push_back(unit);
							}
						}
						else if (targetGroup == TargetGroup::ENEMY) {
							for (const auto card : card->getEnemy()->getHand()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto card : card->getEnemy()->getDeck()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto unit : card->getEnemy()->getBattlefield()) {
								possibleTargets.push_back(unit);
							}
						}
						else if (targetGroup == TargetGroup::BOTH) {
							for (const auto card : card->getOwner()->getHand()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto card : card->getEnemy()->getHand()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto card : card->getOwner()->getDeck()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto card : card->getEnemy()->getDeck()) {
								if (auto unit = dynamic_cast<UnitCard*>(card)) {
									possibleTargets.push_back(unit);
								}
							}
							for (const auto unit : card->getOwner()->getBattlefield()) {
								possibleTargets.push_back(unit);
							}
							for (const auto unit : card->getEnemy()->getBattlefield()) {
								possibleTargets.push_back(unit);
							}
						}
					}
				}
				else if (category == EffectCategory::STATUS_APPLY || category == EffectCategory::STATUS_REMOVE || category == EffectCategory::SILENCE) {
					// if targetGroup is ALLY then we look for units in ally battlefield
					// if targetGroup is ENEMY then we look for units in enemy battlefield
					// if targetGroup is BOTH then we look for units in both battlefields
					if (targetGroup == TargetGroup::ALLY) {
						for (const auto unit : card->getOwner()->getBattlefield()) {
							possibleTargets.push_back(unit);
						}
					}
					else if (targetGroup == TargetGroup::ENEMY) {
						for (const auto unit : card->getEnemy()->getBattlefield()) {
							possibleTargets.push_back(unit);
						}
					}
					else if (targetGroup == TargetGroup::BOTH) {
						for (const auto unit : card->getOwner()->getBattlefield()) {
							possibleTargets.push_back(unit);
						}
						for (const auto unit : card->getEnemy()->getBattlefield()) {
							possibleTargets.push_back(unit);
						}
					}
				}

				// at the end go through the possible targets and call the behavior on them
				std::ranges::shuffle(possibleTargets, generator);
				for (auto target : possibleTargets){
					behavior->execute(*target);
				}
			}
		}

		// After everything, we check for the ally battlefield, and call OGE for the cards that are on the battlefield
		for (const auto* allyHero = dynamic_cast<Hero*>(card); const auto& u : allyHero->getBattlefield()) {
			const auto unit = dynamic_cast<UnitCard*>(u);
			unit->triggerGameEvent(triggerEvent.value());
			if (unit->hasItem()) {
				unit->getItem()->triggerGameEvent(triggerEvent.value());
			}
		}
	}
	else { // this is situation when the trigger is OGE and we check for the event from behavior
		if (behavior->getTriggerEvent().has_value()) {
			if (behavior->getTriggerEvent().value() == event.value()) {
				switch (category) {
				case EffectCategory::DRAW: behavior->execute(*card); break;
				case EffectCategory::DISCARD: behavior->execute(*card); break;
				case EffectCategory::SHUFFLE: behavior->execute(*card); break;
				case EffectCategory::STEAL: behavior->execute(*card); break;
				case EffectCategory::ENERGY_MODIFY: behavior->execute(*card); break;
				}
				if (behavior->getNumberOfTargets().has_value()) {
					if (this->targetMode == TargetMode::SELF) {
						// if the target is self, we call the behavior on the card
						behavior->execute(*card);
					}
					else if (this->targetMode == TargetMode::RANDOM_SINGLE || this->targetMode == TargetMode::RANDOM_MULTIPLE){
						int numberOfTargets = behavior->getNumberOfTargets().value();
						// if the target is random single, we look for 1 random target
						// if the target is random multiple, we look for 2-3 random targets
						// vector of possible targets
						std::vector<Target*> possibleTargets;
						// for HEAL and DAMAGE, we look for units and heroes
						if (category == EffectCategory::HEAL || category == EffectCategory::DAMAGE) {
							// we look for units on battlefield and heroes
							// if target group ALLY we pass to possible targets all ally units and ally hero
							// if target group ENEMY we pass to possible targets all enemy units and enemy hero
							// if target group BOTH we pass to possible targets all units and heroes from both groups
							if (this->targetGroup == TargetGroup::ALLY) {
								// we pass to possible
								for (const auto allyUnit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(allyUnit);
								}
								possibleTargets.push_back(card->getOwner());
							}
							else if (this->targetGroup == TargetGroup::ENEMY) {
								for (const auto enemyUnit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(enemyUnit);
								}
								possibleTargets.push_back(card->getEnemy());
							}
							else if (this->targetGroup == TargetGroup::BOTH) {
								for (const auto allyUnit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(allyUnit);
								}
								for (const auto enemyUnit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(enemyUnit);
								}
								possibleTargets.push_back(card->getOwner());
								possibleTargets.push_back(card->getEnemy());
							}
						}
						else if (category == EffectCategory::BUFF || category == EffectCategory::DEBUFF) {
							// if targetGroup is ALLY we look for cards from ally hero
							// if targetGroup is ENEMY we look for cards from enemy hero
							// if targetGroup is BOTH we look for cards from both heroes
							if (behavior->getStatType().has_value()) {
								// we look for cards in all zones depending on what stat we want to buff or debuff
								StatType statType = behavior->getStatType().value();
								// if statType is ENERGY_COST we look for all cards in hand and deck
								// if statType is VALUE we look for all spell cards in hand and deck
								// if statType is HEALTH, ATTACK we look for all units in hand, deck and battlefield
								// if statType is DURABILITY, DAMAGE, DEFENSE we look for all items in hand, deck and battlefield

								if (this->targetGroup == TargetGroup::ALLY) {
									if (statType == StatType::ENERGY_COST) {
										for (const auto card : card->getOwner()->getHand()) {
											possibleTargets.push_back(card);
										}
										for (const auto card : card->getOwner()->getDeck()) {
											possibleTargets.push_back(card);
										}
									}
									else if (statType == StatType::VALUE) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
									}
									else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto unit : card->getOwner()->getBattlefield()) {
											possibleTargets.push_back(unit);
										}
									}
									else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getOwner()->getBattlefield()) {
											if (const auto unit = dynamic_cast<UnitCard*>(card)) {
												if (unit->hasItem()) {
													possibleTargets.push_back(unit->getItem());
												}
											}
										}
									}
								}
								else if (this->targetGroup == TargetGroup::ENEMY) {
									if (statType == StatType::ENERGY_COST) {
										for (const auto card : card->getEnemy()->getHand()) {
											possibleTargets.push_back(card);
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											possibleTargets.push_back(card);
										}
									}
									else if (statType == StatType::VALUE) {
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
									}
									else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto unit : card->getEnemy()->getBattlefield()) {
											possibleTargets.push_back(unit);
										}
									}
									else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getEnemy()->getBattlefield()) {
											if (const auto unit = dynamic_cast<UnitCard*>(card)) {
												if (unit->hasItem()) {
													possibleTargets.push_back(unit->getItem());
												}
											}
										}
									}
								}
								else if (this->targetGroup == TargetGroup::BOTH) {
									if (statType == StatType::ENERGY_COST) {
										for (const auto card : card->getOwner()->getHand()) {
											possibleTargets.push_back(card);
										}
										for (const auto card : card->getEnemy()->getHand()) {
											possibleTargets.push_back(card);
										}
										for (const auto card : card->getOwner()->getDeck()) {
											possibleTargets.push_back(card);
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											possibleTargets.push_back(card);
										}
									}
									else if (statType == StatType::VALUE) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto spell = dynamic_cast<SpellCard*>(card)) {
												possibleTargets.push_back(spell);
											}
										}
									}
									else if (statType == StatType::HEALTH || statType == StatType::ATTACK) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto unit = dynamic_cast<UnitCard*>(card)) {
												possibleTargets.push_back(unit);
											}
										}
										for (const auto unit : card->getOwner()->getBattlefield()) {
											possibleTargets.push_back(unit);
										}
										for (const auto unit : card->getEnemy()->getBattlefield()) {
											possibleTargets.push_back(unit);
										}
									}
									else if (statType == StatType::DURABILITY || statType == StatType::DAMAGE || statType == StatType::DEFENSE) {
										for (const auto card : card->getOwner()->getHand()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getEnemy()->getHand()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getOwner()->getDeck()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getEnemy()->getDeck()) {
											if (auto item = dynamic_cast<ItemCard*>(card)) {
												possibleTargets.push_back(item);
											}
										}
										for (const auto card : card->getOwner()->getBattlefield()) {
											if (const auto unit = dynamic_cast<UnitCard*>(card)) {
												if (unit->hasItem()) {
													possibleTargets.push_back(unit->getItem());
												}
											}
										}
										for (const auto card : card->getEnemy()->getBattlefield()) {
											if (const auto unit = dynamic_cast<UnitCard*>(card)) {
												if (unit->hasItem()) {
													possibleTargets.push_back(unit->getItem());
												}
											}
										}
									}
								}
							}
						}
						else if (category == EffectCategory::KEYWORD_ADD) {
							// if targetGroup is ALLY then we look for units in ally deck, hand, battlefield
							// if targetGroup is ENEMY then we look for units in enemy deck, hand, battlefield
							// if targetGroup is BOTH then we look for units in both decks, hands, battlefields
							if (behavior->getKeyword().has_value()) {
								if (targetGroup == TargetGroup::ALLY) {
									for (const auto card : card->getOwner()->getHand()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto card : card->getOwner()->getDeck()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto unit : card->getOwner()->getBattlefield()) {
										possibleTargets.push_back(unit);
									}
								}
								else if (targetGroup == TargetGroup::ENEMY) {
									for (const auto card : card->getEnemy()->getHand()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto card : card->getEnemy()->getDeck()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto unit : card->getEnemy()->getBattlefield()) {
										possibleTargets.push_back(unit);
									}
								}
								else if (targetGroup == TargetGroup::BOTH) {
									for (const auto card : card->getOwner()->getHand()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto card : card->getEnemy()->getHand()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto card : card->getOwner()->getDeck()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto card : card->getEnemy()->getDeck()) {
										if (auto unit = dynamic_cast<UnitCard*>(card)) {
											possibleTargets.push_back(unit);
										}
									}
									for (const auto unit : card->getOwner()->getBattlefield()) {
										possibleTargets.push_back(unit);
									}
									for (const auto unit : card->getEnemy()->getBattlefield()) {
										possibleTargets.push_back(unit);
									}
								}
							}
						}
						else if (category == EffectCategory::STATUS_APPLY || category == EffectCategory::STATUS_REMOVE || category == EffectCategory::SILENCE) {
							// if targetGroup is ALLY then we look for units in ally battlefield
							// if targetGroup is ENEMY then we look for units in enemy battlefield
							// if targetGroup is BOTH then we look for units in both battlefields
							if (targetGroup == TargetGroup::ALLY) {
								for (const auto unit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
							else if (targetGroup == TargetGroup::ENEMY) {
								for (const auto unit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
							else if (targetGroup == TargetGroup::BOTH) {
								for (const auto unit : card->getOwner()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
								for (const auto unit : card->getEnemy()->getBattlefield()) {
									possibleTargets.push_back(unit);
								}
							}
						}

						// at the end go through the possible targets and call the behavior on them
						std::ranges::shuffle(possibleTargets, generator);
						for (auto target : possibleTargets) {
							behavior->execute(*target);
						}
					}
				}
			}
		}
	}
};