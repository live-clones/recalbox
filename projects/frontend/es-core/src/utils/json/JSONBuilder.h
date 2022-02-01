/*
 * JSONBuilder.h
 *
 *  Created on: 20 mai 2013
 *      Author: Bkg2k
 */
#pragma once

#include <string>
#include <map>
#include <utils/Strings.h>

/*!
 * A very simple JSON string builder that *exactly* suits our needs.
 * It is not intended to be a complete JSON builder, and yet less a serializer!
 */
class JSONBuilder : public std::string
{
  private:
    //! JSON container
    std::string mString;
    //! Do not insert CRLF when this flag is true
    bool mOptimized;

    /*!
     * Add field separator if required
     */
    void FieldCheck()
    {
      if (!empty())
        if (back() != '{' && back() != '[')
          append(1, ',');
      if (!mOptimized) append(1, '\n');
    }

    /*!
     * Add the stringized field name
     * @param name Field name
     * @return Inherited sting instance for method chaining
     */
    JSONBuilder& FieldName(const char* name)
    {
      FieldCheck();
      append(1, '\"').append(Escape(name)).append(1, '\"');
      return *this;
    }

    /*!
     * @brief Escape special chars from the gigen string
     * @param source Source string
     * @return Escaped string
     */
    static std::string Escape(const std::string& source)
    {
      #define __LENGTHY_STRING(x) x, (int)sizeof(x) - 1
      std::string result = source;
      Strings::ReplaceAllIn(result, '\\', __LENGTHY_STRING("\\\\"));
      Strings::ReplaceAllIn(result, '\b', __LENGTHY_STRING("\\b"));
      Strings::ReplaceAllIn(result, '\f', __LENGTHY_STRING("\\f"));
      Strings::ReplaceAllIn(result, '\n', __LENGTHY_STRING("\\n"));
      Strings::ReplaceAllIn(result, '\r', __LENGTHY_STRING("\\r"));
      Strings::ReplaceAllIn(result, '\t', __LENGTHY_STRING("\\t"));
      Strings::ReplaceAllIn(result, '\"', __LENGTHY_STRING("\\\""));
      return result;
      #undef __LENGTHY_STRING
    }

    /*!
     * @brief Escape special chars from the gigen string
     * @param source Source string
     * @return Escaped string
     */
    static std::string Escape(const char* source)
    {
      return Escape(std::string(source));
    }

    /*!
     * @brief Escape special chars from the gigen string
     * @param source Source string
     * @param length Source string length
     * @return Escaped string
     */
    static std::string Escape(const char* source, int length)
    {
      return Escape(std::string(source, length));
    }

  public:
    /*!
     * Return the inherited string
     * @return String reference
     */
    const std::string& GetJSONString() const { return mString; }


    explicit JSONBuilder(bool optimized = true)
      : mOptimized(optimized)
    {
    }

    explicit JSONBuilder(int reserved, bool optimized = true)
      : mOptimized(optimized)
    {
      mString.reserve(reserved);
    }

    /*!
     * Open the whole JSON builder
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Open()
    {
      std::string::operator=('{');
      return *this;
    }

    /*!
     * Close the builder
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Close()
    {
      append(1, '}');
      return *this;
    }

    /*!
     * Open a new object
     * @param name Object's name
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& OpenObject(const char* name)
    {
      if (name != nullptr) FieldName(name).append(1, ':');
      else FieldCheck();
      append(1, '{');
      return *this;
    }

    /*!
     * Close an object
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& CloseObject() { return Close(); }

    /*!
     * Open a new array
     * @param name Array name
     * @return  The current JSON Instance for method chaining.
     */
    JSONBuilder& OpenArray(const char* name)
    {
      FieldName(name).append(":[");
      return *this;
    }

    /*!
     * Close an array
     * @return The current JSON Instance for method chaining
     */
    JSONBuilder& CloseArray()
    {
      append(1, ']');
      return *this;
    }

    /*!
     * Add a string field
     * @param name Field name
     * @param value Field value
     * @param length Field value length
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const char* value, int length)
    {
      if (name != nullptr) FieldName(name).append(":\"").append(Escape(value, length)).append(1, '"');
      return *this;
    }

    /*!
     * Add a string field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const char* value)
    {
      if (name != nullptr) FieldName(name).append(":\"").append(Escape(value)).append(1, '"');
      return *this;
    }

    /*!
     * Add a string field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const std::string& value)
    {
      if (name != nullptr) FieldName(name).append(":\"").append(Escape(value)).append(1, '"');
      return *this;
    }

    /*!
     * Add an integer field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, int value)
    {
      if (name != nullptr) FieldName(name).append(1, ':').append(Strings::ToString(value));
      return *this;
    }

    /*!
     * Add a 64-bit integer field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, long long value)
    {
      if (name != nullptr) FieldName(name).append(1, ':').append(Strings::ToString(value));
      return *this;
    }

    /*!
     * Add a float field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, float value)
    {
      if (name != nullptr) FieldName(name).append(1, ':').append(Strings::ToString(value, 2));
      return *this;
    }

    /*!
     * Add a boolean field
     * @param name Field name
     * @param value Field value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, bool value)
    {
      if (name != nullptr) FieldName(name).append(1, ':').append(value ? "true" : "false");
      return *this;
    }

    /*!
     * Add a map as a subobject
     * @param name Field name
     * @param value Map
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const std::map<std::string, std::string>& values)
    {
      if (name != nullptr) FieldName(name).append(":{");
      for(const auto& value : values)
        Field(value.first.c_str(), value.second);
      append(1, '}');
      return *this;
    }

    /*!
     * Add a string array field
     * @param name Field name
     * @param value Field array
     * @param count value count
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const std::vector<std::string>& values)
    {
      if (name != nullptr) FieldName(name).append(":[");
      for(const std::string& value : values)
      {
        if (back() != '[') append(1, ',');
        append(1, '"').append(value).append(1, '"');
      }
      append(1, ']');
      return *this;
    }

    /*!
     * Add a float array field
     * @param name Field name
     * @param value Field array
     * @param count value count
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const float value[], int count)
    {
      if (name != nullptr) FieldName(name).append(":[");
      for(int i = 0; --count >= 0; ++i)
      {
        append(Strings::ToString(value[i], 2));
        if (count != 0) append(1, ',');
      }
      append(1, ']');
      return *this;
    }

    /*!
     * Add a long long array field
     * @param name Field name
     * @param value Field array
     * @param count value count
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const long long value[], int count)
    {
      if (name != nullptr) FieldName(name).append(":[");
      for(int i = 0; --count >= 0; i++)
      {
        append(Strings::ToString(value[i]));
        if (count != 0) append(1, ',');
      }
      append(1, ']');
      return *this;
    }

    /*!
     * Add an int array field
     * @param name Field name
     * @param value Field array
     * @param count value count
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const int value[], int count)
    {
      if (name != nullptr) FieldName(name).append(":[");
      for(int i = 0; --count >= 0; i++)
      {
        append(Strings::ToString(value[i]));
        if (count != 0) append(1, ',');
      }
      append(1, ']');
      return *this;
    }

    /*!
     * Add a hexadecimal array field
     * @param name Field name
     * @param value Field array
     * @param count value count
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& FieldHexa(const char* name, int value[], int count)
    {
      if (name != nullptr)
      {
        OpenArray(name);
        for(int i = 0; --count >= 0; i++)
        {
          append(1, '\"').append(Strings::ToHexa(value[i])).append(1, '\"');
          if (count != 0) append(1, ',');
        }
        CloseArray();
      }
      return *this;
    }

    /*!
     * Add ab object from another JSON Builder
     * @param name Field name
     * @param object Object value
     * @return The current JSON Instance for method chaining.
     */
    JSONBuilder& Field(const char* name, const JSONBuilder& object)
    {
      if (name != nullptr) FieldName(name).append(1, ':').append(object);
      return *this;
    }
};
