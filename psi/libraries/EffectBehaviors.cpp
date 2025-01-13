#include "EffectBehaviors.hpp"
#include "Card.hpp"
#include "Hero.hpp"

#include <openssl/asn1.h>

BuffBehavior::BuffBehavior(EffectTrigger trigger, EffectDuration duration, int value, StatType statType, int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent, std::optional<GameEvent> endEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->value = value;
	this->statType = statType;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
	this->endEvent = endEvent;
}

void BuffBehavior::execute(Target& target)
{
	std::random_device rd;
	uint_least32_t securityKey = rd();

	auto* card = dynamic_cast<Card*>(&target);
	if (statType.value() == StatType::ENERGY_COST) {
		card->reduceEnergyCost(value.value(), securityKey);
	}
	else {
		if (auto* unit = dynamic_cast<UnitCard*>(card)) {
			switch (statType.value()) {
			case StatType::HEALTH:
				unit->increaseHealth(value.value(), securityKey);
				break;
			case StatType::ATTACK:
				unit->increaseAttack(value.value(), securityKey);
				break;
			}
		}
		else if (auto* item = dynamic_cast<ItemCard*>(card)) {
			switch (statType.value()) {
			case StatType::DAMAGE:
				item->increaseDamage(value.value(), securityKey);
				break;
			case StatType::DEFENSE:
				item->increaseDefense(value.value(), securityKey);
				break;
			case StatType::DURABILITY:
				item->increaseDurability(value.value(), securityKey);
				break;
			}
		}
		else if (auto* spell = dynamic_cast<SpellCard*>(card)) {
			spell->increaseValue(value.value(), securityKey);
		}
	}
	card->applyEffect(std::make_unique<BuffBehavior>(card, securityKey, duration.value(), value.value(), statType.value(), numberOfTurns, endEvent));
}

void BuffBehavior::decrementTurn()
{
	if (securityKey.has_value() && numberOfTurns.has_value() && numberOfTurns.value() > 0) {
		numberOfTurns.value() = numberOfTurns.value() - 1;

		if (numberOfTurns.value() == 0) {
			auto* card = dynamic_cast<Card*>(effectTarget);
			card->removeEffect(this);
		}
	}
}

void BuffBehavior::checkForEndEvent(GameEvent event)
{
	if (securityKey.has_value() && endEvent.has_value() && endEvent.value() == event) {
		auto* card = dynamic_cast<Card*>(effectTarget);
		card->removeEffect(this);
	}
}

void BuffBehavior::removeEffect() const
{
	if (statType.value() == StatType::ENERGY_COST) {// Card
		auto* card = dynamic_cast<Card*>(effectTarget);

		auto it = std::ranges::find_if(card->extraEnergyCost, [this](const std::unique_ptr<EffectValue>& effect) {
			return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
			});

		if (it != card->extraEnergyCost.end()) {
			card->extraEnergyCost.erase(it);
		}
	}
	else if (statType.value() == StatType::VALUE) {// Spell
		auto* spell = dynamic_cast<SpellCard*>(effectTarget);

		auto it = std::ranges::find_if(spell->extraValue, [this](const std::unique_ptr<EffectValue>& effect) {
			return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
			});

		if (it != spell->extraValue.end()) {
			spell->extraValue.erase(it);
		}
	}
	else if (statType.value() == StatType::HEALTH || statType.value() == StatType::ATTACK) {// Unit
		auto* unit = dynamic_cast<UnitCard*>(effectTarget);
		if (statType.value() == StatType::HEALTH) {
			auto it = std::ranges::find_if(unit->extraHealth, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != unit->extraHealth.end()) {
				unit->extraHealth.erase(it);
			}
		}
		else if (statType.value() == StatType::ATTACK) {
			auto it = std::ranges::find_if(unit->extraAttack, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != unit->extraAttack.end()) {
				unit->extraAttack.erase(it);
			}
		}
	}
	else if (statType.value() == StatType::DAMAGE || statType.value() == StatType::DEFENSE || statType.value() == StatType::DURABILITY) {// Item
		auto* item = dynamic_cast<ItemCard*>(effectTarget);
		if (statType.value() == StatType::DAMAGE) {
			auto it = std::ranges::find_if(item->extraDamage, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDamage.end()) {
				item->extraDamage.erase(it);
			}
		}
		else if (statType.value() == StatType::DEFENSE) {
			auto it = std::ranges::find_if(item->extraDefense, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDefense.end()) {
				item->extraDefense.erase(it);
			}
		}
		else if (statType.value() == StatType::DURABILITY) {
			auto it = std::ranges::find_if(item->extraDurability, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDurability.end()) {
				item->extraDurability.erase(it);
			}
		}
	}
}

BuffBehavior::BuffBehavior(Card* targetOfEffect, uint_least32_t securityKey, EffectDuration duration, int value, StatType statType, std::optional<int> numberOfTurns, std::optional<GameEvent> endEvent)
{
	this->securityKey = securityKey;
	this->effectTarget = targetOfEffect;
	this->duration = duration;
	this->value = value;
	this->statType = statType;
	this->numberOfTurns = numberOfTurns;
	this->endEvent = endEvent;
}

DebuffBehavior::DebuffBehavior(EffectTrigger trigger, EffectDuration duration, int value, StatType statType, int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent, std::optional<GameEvent> endEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->value = value;
	this->statType = statType;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
	this->endEvent = endEvent;
}

void DebuffBehavior::execute(Target& target)
{
	std::random_device rd;
	uint_least32_t securityKey = rd();

	auto* card = dynamic_cast<Card*>(&target);
	if (statType.value() == StatType::ENERGY_COST) {
		card->increaseEnergyCost(value.value(), securityKey);
	}
	else {
		if (auto* unit = dynamic_cast<UnitCard*>(card)) {
			switch (statType.value()) {
			case StatType::HEALTH:
				unit->reduceHealth(value.value(), securityKey);
				break;
			case StatType::ATTACK:
				unit->reduceAttack(value.value(), securityKey);
				break;
			}
		}
		else if (auto* item = dynamic_cast<ItemCard*>(card)) {
			switch (statType.value()) {
			case StatType::DAMAGE:
				item->reduceDamage(value.value(), securityKey);
				break;
			case StatType::DEFENSE:
				item->reduceDefense(value.value(), securityKey);
				break;
			case StatType::DURABILITY:
				item->reduceDurability(value.value(), securityKey);
				break;
			}
		}
		else if (auto* spell = dynamic_cast<SpellCard*>(card)) {
			spell->reduceValue(value.value(), securityKey);
		}
	}
	card->applyEffect(std::make_unique<DebuffBehavior>(card, securityKey, duration.value(), value.value(), statType.value(), numberOfTurns, endEvent));
}

void DebuffBehavior::decrementTurn()
{
	if (securityKey.has_value() && numberOfTurns.has_value() && numberOfTurns.value() > 0) {
		numberOfTurns.value() = numberOfTurns.value() - 1;

		if (numberOfTurns.value() == 0) {
			auto* card = dynamic_cast<Card*>(effectTarget);
			card->removeEffect(this);
		}
	}
}

void DebuffBehavior::checkForEndEvent(const GameEvent event)
{
	if (securityKey.has_value() && endEvent.has_value() && endEvent.value() == event) {
		auto* card = dynamic_cast<Card*>(effectTarget);
		card->removeEffect(this);
	}
}

void DebuffBehavior::removeEffect() const
{
	if (statType.value() == StatType::ENERGY_COST) {// Card
		auto* card = dynamic_cast<Card*>(effectTarget);

		auto it = std::ranges::find_if(card->extraEnergyCost, [this](const std::unique_ptr<EffectValue>& effect) {
			return effect->value == value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
			});

		if (it != card->extraEnergyCost.end()) {
			card->extraEnergyCost.erase(it);
		}
	}
	else if (statType.value() == StatType::VALUE) {// Spell
		auto* spell = dynamic_cast<SpellCard*>(effectTarget);

		auto it = std::ranges::find_if(spell->extraValue, [this](const std::unique_ptr<EffectValue>& effect) {
			return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
			});

		if (it != spell->extraValue.end()) {
			spell->extraValue.erase(it);
		}
	}
	else if (statType.value() == StatType::HEALTH || statType.value() == StatType::ATTACK) {// Unit
		auto* unit = dynamic_cast<UnitCard*>(effectTarget);
		if (statType.value() == StatType::HEALTH) {
			auto it = std::ranges::find_if(unit->extraHealth, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != unit->extraHealth.end()) {
				unit->extraHealth.erase(it);
			}
		}
		else if (statType.value() == StatType::ATTACK) {
			auto it = std::ranges::find_if(unit->extraAttack, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != unit->extraAttack.end()) {
				unit->extraAttack.erase(it);
			}
		}
	}
	else if (statType.value() == StatType::DAMAGE || statType.value() == StatType::DEFENSE || statType.value() == StatType::DURABILITY) {// Item
		auto* item = dynamic_cast<ItemCard*>(effectTarget);

		if (statType.value() == StatType::DAMAGE) {
			auto it = std::ranges::find_if(item->extraDamage, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDamage.end()) {
				item->extraDamage.erase(it);
			}
		}
		else if (statType.value() == StatType::DEFENSE) {
			auto it = std::ranges::find_if(item->extraDefense, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDefense.end()) {
				item->extraDefense.erase(it);
			}
		}
		else if (statType.value() == StatType::DURABILITY) {
			auto it = std::ranges::find_if(item->extraDurability, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
				});

			if (it != item->extraDurability.end()) {
				item->extraDurability.erase(it);
			}
		}
	}
}

DebuffBehavior::DebuffBehavior(Card* targetOfEffect, uint_least32_t securityKey, EffectDuration duration, int value, StatType statType, std::optional<int> numberOfTurns, std::optional<GameEvent> endEvent)
{
	this->securityKey = securityKey;
	this->effectTarget = targetOfEffect;
	this->duration = duration;
	this->value = value;
	this->statType = statType;
	this->numberOfTurns = numberOfTurns;
	this->endEvent = endEvent;
}

HealBehavior::HealBehavior(EffectTrigger trigger, EffectDuration duration, int value, int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->value = value;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
}

void HealBehavior::execute(Target& target)
{
	if (duration == EffectDuration::INSTANT) {
		if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
			unit->heal(value.value());
			unit->triggerGameEvent(GameEvent::UNIT_HEALED);
		}
		else if (auto* hero = dynamic_cast<Hero*>(&target)) {
			hero->restoreHealth(value.value());
		}
	}
	else if (duration == EffectDuration::TURN_BASED) {
		std::random_device rd;
		uint_least32_t securityKey = rd();

		if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
			unit->applyEffect(std::make_unique<HealBehavior>(unit, securityKey, duration.value(), value.value(), numberOfTurns));
			unit->applyStatus(Status::HEALING, numberOfTurns.value());
		}
		else if (auto* hero = dynamic_cast<Hero*>(&target)) {
			hero->applyEffect(std::make_unique<HealBehavior>(hero, securityKey, duration.value(), value.value(), numberOfTurns));
		}
	}
}

void HealBehavior::decrementTurn()
{
	if (duration == EffectDuration::TURN_BASED && numberOfTurns.has_value()) {
		numberOfTurns.value() = numberOfTurns.value() - 1;
		if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
			unit->heal(value.value());
			unit->triggerGameEvent(GameEvent::UNIT_HEALED);
		}
		if (numberOfTurns.value() == 0) {
			if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
				unit->removeEffect(this);
				unit->removeStatus(Status::HEALING);
			}
			else if (auto* hero = dynamic_cast<Hero*>(effectTarget)) {
				hero->removeEffect(this);
			}
		}
	}
}

HealBehavior::HealBehavior(Target* targetOfEffect, uint_least32_t securityKey, EffectDuration duration, int value, std::optional<int> numberOfTurns)
{
	this->effectTarget = targetOfEffect;
	this->securityKey = securityKey;
	this->duration = duration;
	this->value = value;
	this->numberOfTurns = numberOfTurns;
}

DamageBehavior::DamageBehavior(EffectTrigger trigger, EffectDuration duration, int value, int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->value = value;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
}

void DamageBehavior::execute(Target& target)
{
	if (duration == EffectDuration::INSTANT) {
		if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
			unit->dealDamage(value.value());
			unit->triggerGameEvent(GameEvent::UNIT_DAMAGED);
		}
		else if (auto* hero = dynamic_cast<Hero*>(&target)) {
			hero->dealDamage(value.value());
		}
	}
	else if (duration == EffectDuration::TURN_BASED) {
		std::random_device rd;
		uint_least32_t securityKey = rd();
		if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
			unit->applyEffect(std::make_unique<DamageBehavior>(unit, securityKey, duration.value(), value.value(), numberOfTurns));
			unit->applyStatus(Status::DAMAGED, numberOfTurns.value());
		}
		else if (auto* hero = dynamic_cast<Hero*>(&target)) {
			hero->applyEffect(std::make_unique<DamageBehavior>(hero, securityKey, duration.value(), value.value(), numberOfTurns));
		}
	}
}

void DamageBehavior::decrementTurn()
{
	if (duration == EffectDuration::TURN_BASED && numberOfTurns.has_value()) {
		numberOfTurns.value() = numberOfTurns.value() - 1;
		if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
			unit->dealDamage(value.value());
			unit->triggerGameEvent(GameEvent::UNIT_DAMAGED);
		}
		if (numberOfTurns.value() == 0) {
			if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
				unit->removeEffect(this);
				unit->removeStatus(Status::DAMAGED);
			}
			else if (auto* hero = dynamic_cast<Hero*>(effectTarget)) {
				hero->removeEffect(this);
			}
		}
	}
}

DamageBehavior::DamageBehavior(Target* targetOfEffect, uint_least32_t securityKey, EffectDuration duration, int value, std::optional<int> numberOfTurns)
{
	this->effectTarget = targetOfEffect;
	this->securityKey = securityKey;
	this->duration = duration;
	this->value = value;
	this->numberOfTurns = numberOfTurns;
}

StatusApplyBehavior::StatusApplyBehavior(EffectTrigger trigger, EffectDuration duration, Status status,
	int numberOfTargets, std::optional<int> numberOfTurns,
	std::optional<GameEvent> triggerEvent, std::optional<GameEvent> endEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->status = status;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
	this->endEvent = endEvent;
}

void StatusApplyBehavior::execute(Target& target)
{
	if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
		unit->applyStatus(status.value(), numberOfTurns.value());
		unit->triggerGameEvent(GameEvent::UNIT_STATUS_APPLY);
		unit->applyEffect(std::make_unique<StatusApplyBehavior>(unit, duration.value(), status.value(), numberOfTurns.value(), endEvent.value()));
	}
}

void StatusApplyBehavior::decrementTurn()
{
	if (duration == EffectDuration::TURN_BASED && numberOfTurns.has_value()) {
		numberOfTurns.value() = numberOfTurns.value() - 1;
		if (numberOfTurns.value() == 0) {
			if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
				unit->removeEffect(this);
			}
		}
	}
}

void StatusApplyBehavior::checkForEndEvent(const GameEvent event)
{
	if (duration == EffectDuration::EVENT_BASED && endEvent.has_value() && endEvent.value() == event) {
		if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
			unit->removeEffect(this);
		}
	}
}

void StatusApplyBehavior::removeEffect() const
{
	auto* unit = dynamic_cast<UnitCard*>(effectTarget);
	unit->removeStatus(status.value());
	unit->triggerGameEvent(GameEvent::UNIT_STATUS_REMOVE);
}

StatusApplyBehavior::StatusApplyBehavior(Target* targetOfEffect, EffectDuration duration,
										 Status status, std::optional<int> numberOfTurns, std::optional<GameEvent> endEvent)
{
	this->effectTarget = targetOfEffect;
	this->duration = duration;
	this->status = status;
	this->numberOfTurns = numberOfTurns;
	this->endEvent = endEvent;
}

KeywordAddBehavior::KeywordAddBehavior(EffectTrigger trigger, EffectDuration duration, Keyword keyword,
	int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent, std::optional<GameEvent> endEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->keyword = keyword;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
	this->endEvent = endEvent;
}

void KeywordAddBehavior::execute(Target& target)
{
	if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
		unit->addKeyword(keyword.value());
		unit->triggerGameEvent(GameEvent::UNIT_KEYWORD_ADD);
		unit->applyEffect(std::make_unique<KeywordAddBehavior>(unit, duration.value(), keyword.value(), numberOfTurns.value(), endEvent.value()));
	}
}

void KeywordAddBehavior::decrementTurn()
{
	if (duration == EffectDuration::TURN_BASED && numberOfTurns.has_value()) {
		numberOfTurns.value() = numberOfTurns.value() - 1;
		if (numberOfTurns.value() == 0) {
			if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
				unit->removeEffect(this);
			}
		}
	}
}

void KeywordAddBehavior::checkForEndEvent(GameEvent event)
{
	if (duration == EffectDuration::EVENT_BASED && endEvent.has_value() && endEvent.value() == event) {
		if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
			unit->removeEffect(this);
		}
	}
}

void KeywordAddBehavior::removeEffect() const
{
	auto* unit = dynamic_cast<UnitCard*>(effectTarget);
	unit->removeKeyword(keyword.value());
}

KeywordAddBehavior::KeywordAddBehavior(Target* targetOfEffect, EffectDuration duration, Keyword keyword,
	std::optional<int> numberOfTurns, std::optional<GameEvent> endEvent)
{
	this->effectTarget = targetOfEffect;
	this->duration = duration;
	this->keyword = keyword;
	this->numberOfTurns = numberOfTurns;
	this->endEvent = endEvent;
}

SilenceBehavior::SilenceBehavior(EffectTrigger trigger, EffectDuration duration, int numberOfTargets, std::optional<int> numberOfTurns, std::optional<GameEvent> triggerEvent, std::optional<GameEvent> endEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->numberOfTargets = numberOfTargets;
	this->numberOfTurns = numberOfTurns;
	this->triggerEvent = triggerEvent;
	this->endEvent = endEvent;
}

void SilenceBehavior::execute(Target& target)
{
	if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
		unit->applyStatus(Status::SILENCED, numberOfTurns.value());
		unit->applyEffect(std::make_unique<SilenceBehavior>(unit, duration.value(), numberOfTurns.value(), endEvent.value()));
	}
}

void SilenceBehavior::decrementTurn()
{
	if (duration == EffectDuration::TURN_BASED && numberOfTurns.has_value()) {
		numberOfTurns.value() = numberOfTurns.value() - 1;
		if (numberOfTurns.value() == 0) {
			if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
				unit->removeEffect(this);
			}
		}
	}
}

void SilenceBehavior::checkForEndEvent(GameEvent event)
{
	if (duration == EffectDuration::EVENT_BASED && endEvent.has_value() && endEvent.value() == event) {
		if (auto* unit = dynamic_cast<UnitCard*>(effectTarget)) {
			unit->removeEffect(this);
		}
	}
}

void SilenceBehavior::removeEffect() const
{
	auto* unit = dynamic_cast<UnitCard*>(effectTarget);
	unit->removeStatus(Status::SILENCED);
}

SilenceBehavior::SilenceBehavior(Target* targetOfEffect, EffectDuration duration, std::optional<int> numberOfTurns,
	std::optional<GameEvent> endEvent)
{
	this->effectTarget = targetOfEffect;
	this->duration = duration;
	this->numberOfTurns = numberOfTurns;
	this->endEvent = endEvent;
}

StatusRemoveBehavior::StatusRemoveBehavior(EffectTrigger trigger, EffectDuration duration, Status status, int numberOfTargets, std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->status = status;
	this->numberOfTargets = numberOfTargets;
	this->triggerEvent = triggerEvent;
}

void StatusRemoveBehavior::execute(Target& target)
{
	if (auto* unit = dynamic_cast<UnitCard*>(&target)) {
		unit->removeStatus(status.value());
		unit->triggerGameEvent(GameEvent::UNIT_STATUS_REMOVE);
	}
}

DrawBehavior::DrawBehavior(EffectTrigger trigger, EffectDuration duration, TargetGroup who, int value,
	std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->who = who;
	this->value = value;
	this->triggerEvent = triggerEvent;
}

void DrawBehavior::execute(Target& target)
{// by target we pass the card that initiated the effect
	const auto* card = dynamic_cast<Card*>(&target);

	auto* allyHero = card->getOwner();
	auto* enemyHero = card->getEnemy();

	const int numberOfCards = value.value();
	for (int i = 0; i < numberOfCards; i++) {
		if (who == TargetGroup::ALLY) {
			allyHero->drawCard();
		}
		else if (who == TargetGroup::ENEMY) {
			enemyHero->drawCard();
		}
		else if (who == TargetGroup::BOTH) {
			allyHero->drawCard();
			enemyHero->drawCard();
		}
	}
}

DiscardBehavior::DiscardBehavior(EffectTrigger trigger, EffectDuration duration, TargetGroup who, int value,
	std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->who = who;
	this->value = value;
	this->triggerEvent = triggerEvent;
}

void DiscardBehavior::execute(Target& target)
{// by target we pass the card that initiated the effect
	const auto* card = dynamic_cast<Card*>(&target);

	auto* allyHero = card->getOwner();
	auto* enemyHero = card->getEnemy();

	const int numberOfCards = value.value();
	for (int i = 0; i < numberOfCards; i++) {
		if (who == TargetGroup::ALLY) {
			allyHero->discardCard();
		}
		else if (who == TargetGroup::ENEMY) {
			enemyHero->discardCard();
		}
		else if (who == TargetGroup::BOTH) {
			allyHero->discardCard();
			enemyHero->discardCard();
		}
	}
}

ShuffleBehavior::ShuffleBehavior(EffectTrigger trigger, EffectDuration duration, TargetGroup targetGroup,
	int value, TargetZone targetZone, std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->who = targetGroup;
	this->value = value;
	this->whichZone = targetZone;
	this->triggerEvent = triggerEvent;
}

void ShuffleBehavior::execute(Target& target)
{// by target we pass the card that initiated the effect
	auto* card = dynamic_cast<Card*>(&target);

	auto* allyHero = card->getOwner();
	auto* enemyHero = card->getEnemy();

	const int numberOfCards = value.value();
	std::random_device rd;
	std::mt19937 gen(rd());
	if (who == TargetGroup::ALLY)
	{
		if (whichZone == TargetZone::HAND)
		{
			std::uniform_int_distribution<> dis(0, static_cast<int>(allyHero->getHand().size() - 1));
			for (int i = 0; i < numberOfCards; i++)
			{
				int randomCardIndex = dis(gen);
				allyHero->shuffleCardIntoTheDeck(allyHero->getHand()[randomCardIndex]);
				allyHero->getHand().erase(allyHero->getHand().begin() + randomCardIndex);
			}
		}
		else if (whichZone == TargetZone::BATTLEFIELD)
		{
			std::uniform_int_distribution<> dis(0, static_cast<int>(allyHero->getBattlefield().size() - 1));
			for (int i = 0; i < numberOfCards; i++)
			{
				int randomCardIndex = dis(gen);
				allyHero->shuffleCardIntoTheDeck(allyHero->getBattlefield()[randomCardIndex]);
				allyHero->getBattlefield().erase(allyHero->getBattlefield().begin() + randomCardIndex);
			}
		}
	}
	else if (who == TargetGroup::ENEMY)
	{
		if (whichZone == TargetZone::HAND)
		{
			std::uniform_int_distribution<> dis(0, static_cast<int>(enemyHero->getHand().size() - 1));
			for (int i = 0; i < numberOfCards; i++)
			{
				int randomCardIndex = dis(gen);
				enemyHero->shuffleCardIntoTheDeck(enemyHero->getHand()[randomCardIndex]);
				enemyHero->getHand().erase(enemyHero->getHand().begin() + randomCardIndex);
			}
		}
		else if (whichZone == TargetZone::BATTLEFIELD)
		{
			std::uniform_int_distribution<> dis(0, static_cast<int>(enemyHero->getBattlefield().size() - 1));
			for (int i = 0; i < numberOfCards; i++)
			{
				int randomCardIndex = dis(gen);
				enemyHero->shuffleCardIntoTheDeck(enemyHero->getBattlefield()[randomCardIndex]);
				enemyHero->getBattlefield().erase(enemyHero->getBattlefield().begin() + randomCardIndex);
			}
		}
	}
}

StealBehavior::StealBehavior(EffectTrigger trigger, EffectDuration duration, int value, TargetZone targetZone,
	std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->value = value;
	this->whichZone = targetZone;
	this->triggerEvent = triggerEvent;
}

void StealBehavior::execute(Target& target)
{// by target we pass the card that initiated the effect
	auto* card = dynamic_cast<Card*>(&target);

	auto* allyHero = card->getOwner();
	auto* enemyHero = card->getEnemy();
	// we steal number of cards from whichZone from enemy and shuffle the cards into the deck of the ally
	const int numberOfCards = value.value();
	std::random_device rd;
	std::mt19937 gen(rd());
	if (whichZone == TargetZone::HAND)
	{
		std::uniform_int_distribution<> dis(0, static_cast<int>(enemyHero->getHand().size() - 1));
		for (int i = 0; i < numberOfCards; i++)
		{
			int randomCardIndex = dis(gen);
			allyHero->shuffleCardIntoTheDeck(enemyHero->getHand()[randomCardIndex]);
			enemyHero->getHand()[randomCardIndex]->triggerGameEvent(GameEvent::CARD_STOLEN);
			enemyHero->getHand().erase(enemyHero->getHand().begin() + randomCardIndex);
		}
	}
	else if (whichZone == TargetZone::DECK)
	{
		std::uniform_int_distribution<> dis(0, static_cast<int>(enemyHero->getDeck().size() - 1));
		for (int i = 0; i < numberOfCards; i++)
		{
			int randomCardIndex = dis(gen);
			allyHero->shuffleCardIntoTheDeck(enemyHero->getDeck()[randomCardIndex]);
			enemyHero->getHand()[randomCardIndex]->triggerGameEvent(GameEvent::CARD_STOLEN);
			enemyHero->getDeck().erase(enemyHero->getDeck().begin() + randomCardIndex);
		}
	}
}

EnergyModifyBehavior::EnergyModifyBehavior(EffectTrigger trigger, EffectDuration duration, TargetGroup who, int value,
	std::optional<GameEvent> triggerEvent)
{
	this->trigger = trigger;
	this->duration = duration;
	this->who = who;
	this->value = value;
	this->triggerEvent = triggerEvent;
}

void EnergyModifyBehavior::execute(Target& target)
{// by target we pass the card that initiated the effect
	auto* card = dynamic_cast<Card*>(&target);

	auto* allyHero = card->getOwner();
	auto* enemyHero = card->getEnemy();

	const int numberOfEnergy = value.value();
	if (who == TargetGroup::ALLY || who == TargetGroup::BOTH)
	{
		allyHero->modifyEnergy(numberOfEnergy);
	}
	if (who == TargetGroup::ENEMY || who == TargetGroup::BOTH)
	{
		enemyHero->modifyEnergy(numberOfEnergy);
	}
}