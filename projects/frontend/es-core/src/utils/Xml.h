//
// Created by bkg2k on 08/11/2019.
//
#pragma once

#include <pugixml/pugixml.hpp>
#include <utils/Strings.h>

typedef pugi::xml_document     XmlDocument;
typedef pugi::xml_node         XmlNode;
typedef pugi::xml_attribute    XmlAttribute;
typedef pugi::xml_parse_result XmlResult;

class Xml
{
  public:
    /*!
     * @brief Get a child value from the given xml node, or the default value if the child does not exist
     * @param root Node from which to get child
     * @param childname Children name from which to get value
     * @param defaultvalue Default value
     * @return Fetch value or default value
     */
    static std::string AsString(XmlNode root, const char* childname, const char* defaultvalue)
    {
      XmlNode child = root.child(childname);
      return child != nullptr ? child.child_value() : defaultvalue;
    }

    /*!
     * @brief Get a child value from the given xml node, or the default value if the child does not exist
     * @param root Node from which to get child
     * @param childname Children name from which to get value
     * @param defaultvalue Default value
     * @return Fetch value or default value
     */
    static std::string AsString(XmlNode root, const std::string& childname, const std::string& defaultvalue)
    {
      XmlNode child = root.child(childname.c_str());
      return child != nullptr ? child.child_value() : defaultvalue;
    }

    /*!
     * @brief Get an attribute value from the given xml node, or the default value if the attribute does not exist
     * @param root Node from which to get child
     * @param attributename Attribute name from which to get value
     * @param defaultvalue Default value
     * @return Fetch value or default value
     */
    static std::string AttributeAsString(XmlNode root, const char* attributename, const char* defaultvalue)
    {
      XmlAttribute attribute = root.attribute(attributename);
      return attribute != nullptr ? attribute.value() : defaultvalue;
    }

    /*!
     * @brief Get an attribute value from the given xml node, or the default value if the attribute does not exist
     * @param root Node from which to get child
     * @param attributename Attribute name from which to get value
     * @param defaultvalue Default value
     * @return Fetch value or default value
     */
    static int AttributeAsInt(XmlNode root, const char* attributename, int defaultvalue)
    {
      XmlAttribute attribute = root.attribute(attributename);
      int value = defaultvalue;
      if (attribute != nullptr)
        if (!Strings::ToInt(attribute.value(), value))
          value = defaultvalue;
      return value;
    }

    /*!
     * @brief Get an attribute value from the given xml node, or the default value if the attribute does not exist
     * @param root Node from which to get child
     * @param attributename Attribute name from which to get value
     * @param defaultvalue Default value
     * @return Fetch value or default value
     */
    static bool AttributeAsBool(XmlNode root, const char* attributename, bool defaultvalue)
    {
      XmlAttribute attribute = root.attribute(attributename);
      bool value = defaultvalue;
      if (attribute != nullptr)
        if (!Strings::ToBool(attribute.value(), value))
          value = defaultvalue;
      return value;
    }

    /*!
     * @brief Add new attribute
     * @param root Root node to add attribute
     * @param attributename Attribute name
     * @param value Attribute value
     */
    static void AddAttribute(XmlNode root, const char* attributename, const char* value)
    {
      XmlAttribute attribute = root.append_attribute(attributename);
      if (attribute != nullptr)
        attribute.set_value(value);
    }

    /*!
     * @brief Add new attribute
     * @param root Root node to add attribute
     * @param attributename Attribute name
     * @param value Attribute value
     */
    static void AddAttribute(XmlNode root, const char* attributename, const std::string& value)
    {
      XmlAttribute attribute = root.append_attribute(attributename);
      if (attribute != nullptr)
        attribute.set_value(value.data());
    }

    /*!
     * @brief Add new attribute
     * @param root Root node to add attribute
     * @param attributename Attribute name
     * @param value Attribute value
     */
    static void AddAttribute(XmlNode root, const char* attributename, int value)
    {
      XmlAttribute attribute = root.append_attribute(attributename);
      if (attribute != nullptr)
        attribute.set_value(value);
    }

    /*!
     * @brief Add new attribute
     * @param root Root node to add attribute
     * @param attributename Attribute name
     * @param value Attribute value
     */
    static void AddAttribute(XmlNode root, const char* attributename, unsigned int value)
    {
      XmlAttribute attribute = root.append_attribute(attributename);
      if (attribute != nullptr)
        attribute.set_value(value);
    }

    /*!
     * @brief Add new attribute
     * @param root Root node to add attribute
     * @param attributename Attribute name
     * @param value Attribute value
     */
    static void AddAttribute(XmlNode root, const char* attributename, bool value)
    {
      XmlAttribute attribute = root.append_attribute(attributename);
      if (attribute != nullptr)
        attribute.set_value(value);
    }

    /*!
     * @brief Append a child node to the given parent node and set the value
     * @param parent Parent node to which to add the child node
     * @param childname Child node name
     * @param value Value of the child node
     */
    static void AddAsString(XmlNode parent, const char* childname, const char* value)
    {
      parent.append_child(childname).text().set(value);
    }

    /*!
     * @brief Append a child node to the given parent node and set the value
     * @param parent Parent node to which to add the child node
     * @param childname Child node name
     * @param value Value of the child node
     */
    static void AddAsString(XmlNode parent, const std::string& childname, const std::string& value)
    {
      parent.append_child(childname.c_str()).text().set(value.c_str());
    }

    /*!
     * @brief Append a child node to the given parent node and set the value as integer
     * @param parent Parent node to which to add the child node
     * @param childname Child node name
     * @param value Value of the child node
     */
    static void AddAsString(XmlNode parent, const std::string& childname, int value)
    {
      parent.append_child(childname.c_str()).text().set(Strings::ToString(value).c_str());
    }

    /*!
     * @brief Append a child node to the given parent node and set the value as float
     * @param parent Parent node to which to add the child node
     * @param childname Child node name
     * @param value Value of the child node
     */
    static void AddAsString(XmlNode parent, const std::string& childname, float value)
    {
      parent.append_child(childname.c_str()).text().set(Strings::ToString(value, 5).c_str());
    }
};