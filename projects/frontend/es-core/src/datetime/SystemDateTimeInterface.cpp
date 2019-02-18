#include "SystemDateTimeInterface.h"

static SystemDateTimeImplementation SystemDateTime;

void SystemDateTimeImplementation::Initialize()
{
  DateTime::SetSystemInterface(&SystemDateTime);
}
