#include "Effect.hpp"

Effect::Effect(Game* game, int val, EffectCategory category, EffectType type, TargetType target, bool permanent, int duration, GameEvent trigger)
	: value(val), effectCategory(category), effectType(type), targetType(target), isPermanent(permanent),
	  duration(duration), triggersOn(trigger)
{
	if (effectType == EffectType::GLOBAL && targetType != TargetType::NONE) targetType = TargetType::NONE;

	if (effectType == EffectType::TARGETED && targetType == TargetType::NONE) effectType = EffectType::GLOBAL;

	if (effectCategory == EffectCategory::BUFF || effectCategory == EffectCategory::DEBUFF)
	{
		auto start = std::chrono::system_clock::now();
		auto end = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
		uint_least32_t timeSeed = static_cast<uint_least32_t>(end);

		std::default_random_engine generator(timeSeed + game->getSave().getSeed());
		std::uniform_int_distribution<int> distribution(1, 3);

		switch (distribution(generator))
		{
			case 1:
				statistic = Stat::HEALTH;
				break;
			case 2:
				statistic = Stat::ATTACK;
				break;
			case 3:
				statistic = Stat::MANA;
				if (effectCategory == EffectCategory::BUFF)
				{
					value = -value;
				}
				break;
		}
	}

	//TODO: Add description making process
}

bool Effect::isTriggered(const GameEvent& gameEvent) const
{
	return gameEvent == triggersOn;
}

int Effect::getValue() const
{
	return value;
}

EffectCategory Effect::getCategory() const
{
	return effectCategory;
}

EffectType Effect::getType() const
{
	return effectType;
}

TargetType Effect::getTargetType() const
{
	return targetType;
}

bool Effect::isPermanentEffect() const
{
	return isPermanent;
}

int Effect::getDuration() const
{
	return duration;
}

std::string Effect::getDescription() const
{
	return description;
}

void Effect::decreaseDuration()
{
	if (!isPermanent && duration > 0)
	{
		duration--;
	}
}