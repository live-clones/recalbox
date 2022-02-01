//
// Created by gugue_u on 10/02/2022.
//



#include <vector>
#include "SmartCollection.h"

class Collections
{
  public:
    //! Constructor
    explicit Collections();

    std::vector<SmartCollection> Deserialize();

  private:
    std::vector<SmartCollection> mSmartCollections;
};


