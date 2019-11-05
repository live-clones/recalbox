//
// Created by matthieu on 12/09/15.
//
#pragma once

#include <string>
#include <vector>
#include <map>

class RecalboxConf
{
  public:
    /*!
     * @brief Confstructor
     * @param initialConfigOnly true if only the original file has to be loaded
     */
    explicit RecalboxConf(bool initialConfigOnly);

    /*!
     * @brief Static instance
     * @return Instance
     */
    static RecalboxConf& Instance();

    /*!
     * @brief Save the configuration file and backup the current one
     * @return True if the operation is successful
     */
    bool SaveRecalboxConf();

    /*!
     * @brief Get string value from the given key
     * @param name Key
     * @return Value or empty string if the key does not exist
     */
    std::string AsString(const std::string &name);

    /*!
     * @brief Get string value from the given key or return the default value
     * @param name Key
     * @param defaultValue Default value
     * @return Value or default value if the key does not exist
     */
    std::string AsString(const std::string &name, const std::string &defaultValue);

    /*!
     * @brief Get boolean value from the given key or return the default value
     * @param name Key
     * @param defaultValue Default value (optional, false by default)
     * @return Value or default value if the key does not exist
     */
    bool AsBool(const std::string& name, bool defaultValue = false);

    /*!
     * @brief Get value as unsigned int from the given key or return the default value
     * @param name Key
     * @param defaultValue Default value (optional, 0 by default)
     * @return Value or default value if the key does not exist
     */
    unsigned int AsUInt(const std::string& name, unsigned int defaultValue = 0);

    /*!
     * @brief Get value as signed int from the given key or return the default value
     * @param name Key
     * @param defaultValue Default value (optional, 0 by default)
     * @return Value or default value if the key does not exist
     */
    int AsInt(const std::string& name, int defaultValue = 0);

    /*!
     * @brief Set the value as string of the given key
     * @param name Key
     * @param value Value to set
     */
    void SetString(const std::string &name, const std::string &value);

    /*!
     * @brief Set the value as boolean of the given key
     * @param name Key
     * @param value Value to set
     */
    void SetBool(const std::string &name, bool value);

    /*!
     * @brief Set the value as an unsigned int of the given key
     * @param name Key
     * @param value Value to set
     */
    void SetUInt(const std::string &name, unsigned int value);

    /*!
     * @brief Set the value as a signed int of the given key
     * @param name Key
     * @param value Value to set
     */
    void SetInt(const std::string &name, unsigned int value);

    /*!
     * @brief Set the value as a string list, comma separated, of the given key
     * @param name Key
     * @param values string list
     */
    void SetList(const std::string &name, const std::vector<std::string> &values);

    /*!
     * @brief Check if a value is in the given named list
     * @param name Key from which to obtain the list
     * @param value Value to seek for in the list
     * @return True if the list exists and the value is found. False otherwise
     */
    bool isInList(const std::string &name, const std::string &value);

    /*!
     * @brief Check if the given line is a valide 'key=value'
     * @param line Text line to seek for key/value
     * @param key Extracted key if found
     * @param value Extracted value if found
     * @return true if a valid key=value has been found
     */
    static bool IsValidKeyValue(const std::string& line, std::string& key, std::string& value);

  private:
    //! Configuration map: key, value
    std::map<std::string, std::string> confMap;

    /*!
     * @brief Load configuration file
     * @param initialConfigOnly if true, only the initial file in share_init is loaded if it exists
     * otherwise, the loader try both files, starting with the one in share.
     * @return True if a configuration file has been loaded successfully
     */
    bool LoadRecalboxConf(bool initialConfigOnly);
};
