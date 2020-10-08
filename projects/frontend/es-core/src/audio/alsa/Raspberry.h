//
// Created by bkg2k on 08/10/2020.
//

class Raspberry
{
  public:
    //! Selected output
    enum class Output
    {
      None = 0,       //!< No output
      Headphones = 1, //!< Headphones (jack)
      HDMI = 2,       //!< HDMI
    };

    //! Constructor
    Raspberry();

    //! Route sound to the given output
    void SetRoute(Output output);
};

