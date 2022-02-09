//
// Created by bkg2k on 16/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <systems/SystemSorting.h>
#include <systems/SystemData.h>

bool SortingName::operator()(const SystemData* a, const SystemData* b) const
{
  return (a->Descriptor().FullName() < b->Descriptor().FullName());
}

bool SortingReleaseDate::operator()(const SystemData* a, const SystemData* b) const
{
  return (a->Descriptor().ReleaseDate() < b->Descriptor().ReleaseDate());
}

bool Sorting1Type2Name::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Type() == b->Descriptor().Type())
  {
      return a->Descriptor().FullName() < b->Descriptor().FullName();
  }
  return (a->Descriptor().Type() < b->Descriptor().Type());
}

bool Sorting1Type2ReleaseDate::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Type() == b->Descriptor().Type())
  {
      return a->Descriptor().ReleaseDate() < b->Descriptor().ReleaseDate();
  }
  return (a->Descriptor().Type() < b->Descriptor().Type());
}

bool Sorting1Manufacturer2Name::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Manufacturer() == b->Descriptor().Manufacturer())
  {
      return a->Descriptor().FullName() < b->Descriptor().FullName();
  }
  return (a->Descriptor().Manufacturer() < b->Descriptor().Manufacturer());
}

bool Sorting1Manufacturer2ReleaseDate::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Manufacturer() == b->Descriptor().Manufacturer())
  {
      return a->Descriptor().ReleaseDate() < b->Descriptor().ReleaseDate();
  }
  return (a->Descriptor().Manufacturer() < b->Descriptor().Manufacturer());
}

bool Sorting1Type2Manufacturer3Name::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Type() == b->Descriptor().Type())
  {
    if (a->Descriptor().Manufacturer() == a->Descriptor().Manufacturer())
    {
        return (a->Descriptor().FullName() < b->Descriptor().FullName());
    }
    return (a->Descriptor().Manufacturer() < b->Descriptor().Manufacturer());
  }
  return (a->Descriptor().Type() < b->Descriptor().Type());
}

bool Sorting1Type2Manufacturer3ReleaseDate::operator()(const SystemData* a, const SystemData* b) const
{
  if (a->Descriptor().Type() == b->Descriptor().Type())
  {
    if (a->Descriptor().Manufacturer() == b->Descriptor().Manufacturer())
    {
        return (a->Descriptor().ReleaseDate() < b->Descriptor().ReleaseDate());
    }
    return (a->Descriptor().Manufacturer() < b->Descriptor().Manufacturer());
  }
  return (a->Descriptor().Type() < b->Descriptor().Type());
}
