#ifndef _INPUTCONFIG_H_
#define _INPUTCONFIG_H_

#include <map>
#include <vector>
#include <string>
#include <SDL.h>
#include <sstream>
#include "pugixml/pugixml.hpp"

#define DEVICE_KEYBOARD -1
#define MAX_PLAYERS 5

enum InputType
{
	TYPE_AXIS,
	TYPE_BUTTON,
	TYPE_HAT,
	TYPE_KEY,
	TYPE_COUNT
};

struct Input
{
public:
	int device;
	InputType type;
	int id;
	int value;
	bool configured;

	Input()
	{
		device = DEVICE_KEYBOARD;
		id = -1;
		value = -999;
		type = TYPE_COUNT;
		configured = false;
	}

	Input(int dev, InputType t, int i, int val, bool conf) : device(dev), type(t), id(i), value(val), configured(conf)
	{
	}

	std::string getHatDir(int val)
	{
		if(val & SDL_HAT_UP)
			return "up";
		else if(val & SDL_HAT_DOWN)
			return "down";
		else if(val & SDL_HAT_LEFT)
			return "left";
		else if(val & SDL_HAT_RIGHT)
			return "right";
		return "neutral?";
	}

	std::string string()
	{
		std::stringstream stream;
		switch(type)
		{
			case TYPE_BUTTON:
				stream << "Button " << id;
				break;
			case TYPE_AXIS:
				stream << "Axis " << id << (value > 0 ? "+" : "-");
				break;
			case TYPE_HAT:
				stream << "Hat " << id << " " << getHatDir(value);
				break;
			case TYPE_KEY:
				stream << "Key " << SDL_GetKeyName((SDL_Keycode)id);
				break;
			default:
				stream << "Input to string error";
				break;
		}

		return stream.str();
	}
};

class InputConfig
{
public:
	InputConfig(const InputConfig* source);
	InputConfig(int deviceId, int deviceIndex, const std::string& deviceName, const std::string& deviceGUID, int deviceNbAxes, int deviceNbHats, int deviceNbButtons);

	void clear();
	void loadFrom(const InputConfig* source);
	void mapInput(const std::string& name, Input input);
	void unmapInput(const std::string& name); // unmap all Inputs mapped to this name

	inline int getDeviceId() const { return mDeviceId; };
	inline int getDeviceIndex() const { return mDeviceIndex; };
	inline const std::string& getDeviceName() const { return mDeviceName; }
	inline const std::string& getDeviceGUIDString() const { return mDeviceGUID; }
	inline int getDeviceNbAxes() const { return mDeviceNbAxes; };
	inline int getDeviceNbHats() const { return mDeviceNbHats; };
	inline int getDeviceNbButtons() const { return mDeviceNbButtons; };
	inline const std::map<std::string, Input> getNameMap() const { return mNameMap; };

	std::string getSDLPowerLevel();
	std::string getSysPowerLevel();

	//Returns true if Input is mapped to this name, false otherwise.
	bool isMappedTo(const std::string& name, Input input);
	bool isMapped(const std::string& name);

	//Returns a list of names this input is mapped to.
	std::vector<std::string> getMappedTo(Input input);

	void loadFromXML(pugi::xml_node root);
	void writeToXML(pugi::xml_node parent);

	bool isConfigured();
	// Returns true if there is an Input mapped to this name, false otherwise.
	// Writes Input mapped to this name to result if true.
	bool getInputByName(const std::string& name, Input* result);

private:
	const int mDeviceId;
	const int mDeviceIndex;
	const std::string mDeviceName;
	const std::string mDeviceGUID;
	const int mDeviceNbAxes; // number of axes of the device
	const int mDeviceNbHats;
	const int mDeviceNbButtons;
	std::map<std::string, Input> mNameMap;
};

#endif
