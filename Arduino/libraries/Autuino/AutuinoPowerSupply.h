#define POWER_SUPPLY_TYPE_MAIN (uint8_t)0
#define POWER_SUPPLY_TYPE_BATTERY (uint8_t)1
#define POWER_SUPPLY_TYPE_HYBRID (uint8_t)2  //can be main or battery, if main is shutdown, it will switch to battery

class AutuinoPowerSupply
{
	public:
		uint8_t getPowerSupplyType(); //what is the general type (MAIN, BATTERY or BYBRID
		uint8_t getCurrentSupplyType(); //can only be MAIN or BATTERY. It states the current situation of the power supply, not its type. Used to device can behave differently if power supply changes
		uint32_t getCurrentPowerSupplyCapacity();
		uint8_t getCurrentPowerSupplyLevel(); //if main always 100%. If battery, some calculation of battery level
};
