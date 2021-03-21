//
// Created by guillaume on 07/12/2020.
//


#include <guis/Gui.h>
#include "TextComponent.h"

class GameClipNoVideoContainer : public Gui
{
  private:

    TextComponent mWarningText;
    ImageComponent mMire;

  public:

    explicit GameClipNoVideoContainer(WindowManager& window);

    void Render(const Transform4x4f& parentTrans) override;

    bool getHelpPrompts(Help& help);
};



#ifndef EMULATIONSTATION_ALL_GAMECLIPNOVIDEOCONTAINER_H
#define EMULATIONSTATION_ALL_GAMECLIPNOVIDEOCONTAINER_H

#endif //EMULATIONSTATION_ALL_GAMECLIPNOVIDEOCONTAINER_H
