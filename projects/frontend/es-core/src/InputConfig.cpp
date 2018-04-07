#include "InputConfig.h"
#include <string>
#include <algorithm>
#include <SDL.h>
#include <iostream>
#include "Log.h"
#include "InputManager.h"

//some util functions
std::string inputTypeToString(InputType type)
{
	switch(type)
	{
	case TYPE_AXIS:
		return "axis";
	case TYPE_BUTTON:
		return "button";
	case TYPE_HAT:
		return "hat";
	case TYPE_KEY:
		return "key";
	default:
		return "error";
	}
}

InputType stringToInputType(const std::string& type)
{
	if(type == "axis")
		return TYPE_AXIS;
	if(type == "button")
		return TYPE_BUTTON;
	if(type == "hat")
		return TYPE_HAT;
	if(type == "key")
		return TYPE_KEY;
	return TYPE_COUNT;
}


std::string toLower(std::string str)
{
	for(unsigned int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}

	return str;
}
//end util functions

InputConfig::InputConfig(const InputConfig* source) : 
	mDeviceId(source->getDeviceId()), 
	mDeviceIndex(source->getDeviceIndex()), 
	mDeviceName(source->getDeviceName()), 
	mDeviceGUID(source->getDeviceGUIDString()), 
	mDeviceNbAxes(source->getDeviceNbAxes()), 
	mDeviceNbHats(source->getDeviceNbHats()), 
	mDeviceNbButtons(source->getDeviceNbButtons())
{
	mNameMap = source->getNameMap();
}


InputConfig::InputConfig(int deviceId, int deviceIndex, const std::string& deviceName, const std::string& deviceGUID, int deviceNbAxes, int deviceNbHats, int deviceNbButtons) : mDeviceId(deviceId), mDeviceIndex(deviceIndex), mDeviceName(deviceName), mDeviceGUID(deviceGUID), mDeviceNbAxes(deviceNbAxes), mDeviceNbHats(deviceNbHats), mDeviceNbButtons(deviceNbButtons)
{
}

void InputConfig::clear()
{
	mNameMap.clear();
}

void InputConfig::loadFrom(const InputConfig* source) 
{
	mNameMap = source->getNameMap();
}

bool InputConfig::isConfigured()
{
	return mNameMap.size() > 0;
}

std::string InputConfig::getSDLPowerLevel()
{
	SDL_Joystick* joy;
	//joy = InputManager::getInstance()->getJoystickByJoystickID(getDeviceId());
	joy = SDL_JoystickOpen(getDeviceId());
	SDL_JoystickPowerLevel power = SDL_JoystickCurrentPowerLevel(joy);
	switch(power)
	{
		case SDL_JOYSTICK_POWER_EMPTY:
			return "\uF1b6";

		case SDL_JOYSTICK_POWER_FULL:
			return "\uF1b7";

		case SDL_JOYSTICK_POWER_LOW:
			return "\uF1b1";

		case SDL_JOYSTICK_POWER_MAX:
			return "\uF1ba";

		case SDL_JOYSTICK_POWER_MEDIUM:
			return "\uF1b8";

		case SDL_JOYSTICK_POWER_UNKNOWN:
			return "\uF1b9";

		case SDL_JOYSTICK_POWER_WIRED:
			return "\uF1b4";
	}

}

std::string InputConfig::getSysPowerLevel()
{
	SDL_Joystick* joy;
	//joy = InputManager::getInstance()->getJoystickByJoystickID(getDeviceId());
	joy = SDL_JoystickOpen(getDeviceId());
	return "\uF1be";
}

void InputConfig::mapInput(const std::string& name, Input input)
{
	mNameMap[toLower(name)] = input;
}

void InputConfig::unmapInput(const std::string& name)
{
	auto it = mNameMap.find(toLower(name));
	if(it != mNameMap.end())
		mNameMap.erase(it);
}

bool InputConfig::getInputByName(const std::string& name, Input* result)
{
	auto it = mNameMap.find(toLower(name));
	if(it != mNameMap.end())
	{
		*result = it->second;
		return true;
	}

	return false;
}

bool InputConfig::isMapped(const std::string& name)
{
	auto it = mNameMap.find(toLower(name));
	return it != mNameMap.end();
}

bool InputConfig::isMappedTo(const std::string& name, Input input)
{
	Input comp;
	if(!getInputByName(name, &comp))
		return false;
	
	if(comp.configured && comp.type == input.type && comp.id == input.id)
	{
		if(comp.type == TYPE_HAT)
		{
			return (input.value == 0 || input.value & comp.value);
		}

		if(comp.type == TYPE_AXIS)
		{
			return input.value == 0 || comp.value == input.value;
		}else{
			return true;
		}
	}
	return false;
}

std::vector<std::string> InputConfig::getMappedTo(Input input)
{
	std::vector<std::string> maps;

	typedef std::map<std::string, Input>::iterator it_type;
	for(it_type iterator = mNameMap.begin(); iterator != mNameMap.end(); iterator++)
	{
		Input chk = iterator->second;

		if(!chk.configured)
			continue;

		if(chk.device == input.device && chk.type == input.type && chk.id == input.id)
		{
			if(chk.type == TYPE_HAT)
			{
				if(input.value == 0 || input.value & chk.value)
				{
					maps.push_back(iterator->first);
				}
				continue;
			}

			if(input.type == TYPE_AXIS)
			{
				if(input.value == 0 || chk.value == input.value)
					maps.push_back(iterator->first);
			}else{
				maps.push_back(iterator->first);
			}
		}
	}

	return maps;
}

void InputConfig::loadFromXML(pugi::xml_node node)
{
	clear();

	for(pugi::xml_node input = node.child("input"); input; input = input.next_sibling("input"))
	{
		std::string name = input.attribute("name").as_string();
		std::string type = input.attribute("type").as_string();
		InputType typeEnum = stringToInputType(type);

		if(typeEnum == TYPE_COUNT)
		{
			LOG(LogError) << "InputConfig load error - input of type \"" << type << "\" is invalid! Skipping input \"" << name << "\".\n";
			continue;
		}

		int id = input.attribute("id").as_int();
		int value = input.attribute("value").as_int();

        if(value == 0) {
			LOG(LogWarning) << "WARNING: InputConfig value is 0 for " << type << " " << id << "!\n";
        }

		mNameMap[toLower(name)] = Input(mDeviceId, typeEnum, id, value, true);
	}
}

void InputConfig::writeToXML(pugi::xml_node parent)
{
	pugi::xml_node cfg = parent.append_child("inputConfig");

	if(mDeviceId == DEVICE_KEYBOARD)
	{
		cfg.append_attribute("type") = "keyboard";
		cfg.append_attribute("deviceName") = "Keyboard";
	}else{
		cfg.append_attribute("type") = "joystick";
		cfg.append_attribute("deviceName") = mDeviceName.c_str();
	}

	cfg.append_attribute("deviceGUID") = mDeviceGUID.c_str();
    cfg.append_attribute("deviceNbAxes") = mDeviceNbAxes;
    cfg.append_attribute("deviceNbHats") = mDeviceNbHats;
    cfg.append_attribute("deviceNbButtons") = mDeviceNbButtons;

	typedef std::map<std::string, Input>::iterator it_type;
	for(it_type iterator = mNameMap.begin(); iterator != mNameMap.end(); iterator++)
	{
		if(!iterator->second.configured)
			continue;

		pugi::xml_node input = cfg.append_child("input");
		input.append_attribute("name") = iterator->first.c_str();
		input.append_attribute("type") = inputTypeToString(iterator->second.type).c_str();
		input.append_attribute("id").set_value(iterator->second.id);
		input.append_attribute("value").set_value(iterator->second.value);
	}
}
