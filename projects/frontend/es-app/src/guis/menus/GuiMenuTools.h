//
// Created by bkg2k on 06/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#pragma once

#include <string>
#include <vector>

// Forward declarations
class SystemManager;
class SystemData;

class GuiMenuTools
{
  public:
    struct Shader
    {
      Path ShaderPath;
      std::string Displayable;
    };

    //! Shader structure list
    typedef std::vector<Shader> ShaderList;

    //! Structure used to display Emulator & Core in menus
    struct EmulatorAndCore
    {
      std::string Identifier;  //!< Identifier
      std::string Displayable; //!< Displayable emulator or emulator/core
      bool Selected;           //!< True if this is the selected item
    };

    //! List of Emulator & core structure
    typedef std::vector<EmulatorAndCore> EmulatorAndCoreList;

    /*!
     * @brief Get a displayable list of emulator and core available for the given system
     * @param systemManager System manager instance
     * @param system System to retrieve emulators & cores from
     * @param outDefaultEmulator Filled with default emeulator
     * @param outDefaultCore Filled from default core
     * @param currentEmulator Current emulator from caller (to select the right entry)
     * @param currentCore Current core from caller (to select the right entry)
     * @return Emulator & core list
     */
    static EmulatorAndCoreList ListEmulatorAndCore(SystemManager& systemManager, SystemData& system,
                                                   std::string& outDefaultEmulator, std::string& outDefaultCore,
                                                   const std::string& currentEmulator, const std::string& currentCore);

    /*!
     * @brief List shaders available in the user shader path
     * @return Shader list
     */
    static ShaderList ListShaders();

  private:
    static const Path sShadersPath;

    /*!
     * @brief Read shaders in folder recursively
     * @param root Current folder to read .glsl from
     * @param glslp Output list
     */
    static void ReadShaderFolder(const Path& root, Path::PathList& glslp);

    /*!
     * @brief Collect all abailable shaders
     * @return Shader path collection
     */
    static Path::PathList GetShaderList();
};



