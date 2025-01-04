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

void BuffBehavior::removeBuff() const
{
	if (statType.value() == StatType::ENERGY_COST) {// Card
		auto* card = dynamic_cast<Card*>(effectTarget);

		auto it = std::ranges::find_if(card->extraEnergyCost, [this](const std::unique_ptr<EffectValue>& effect) {
				return effect->value == -value.value() && effect->securityKey == securityKey.value() && effect->statType == statType.value();
			});

		if (it!=card->extraEnergyCost.end()) {
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

void DebuffBehavior::removeDebuff() const
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

void HealBehavior::checkForEndEvent(const GameEvent event)
{
	// Not used
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

void DamageBehavior::checkForEndEvent(const GameEvent event)
{
	// Not used
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

void StatusApplyBehavior::removeStatus() const
{
	auto* unit = dynamic_cast<UnitCard*>(effectTarget);
	unit->removeStatus(status.value());
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

void SilenceBehavior::removeSilence() const
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
	}
}

void StatusRemoveBehavior::decrementTurn()
{
	// Not used
}

void StatusRemoveBehavior::checkForEndEvent(GameEvent event)
{
	// Not used
}
