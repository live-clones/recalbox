//
// Created by bkg2k on 03/04/2020.
//
#pragma once

#include <bios/BiosList.h>
#include "utils/json/JSONBuilder.h"
#include "Validator.h"
#include <utils/os/fs/Path.h>
#include <utils/Strings.h>
#include <utils/IniFile.h>
#include <emulators/EmulatorList.h>
#include <pistache/include/pistache/http.h>

class RequestHandlerTools
{
  private:
    /*!
     * @brief Get suported syetem list
     * @return Systems' short names list
     */
    static const Strings::Vector& GetSupportedSystemList();

    static const Strings::Vector& GetSupportedRatioList();

    static const std::string& GetArchitecture();

    static std::string GetCommandOutput(const std::string& command);

    static HashMap<std::string, std::string> GetAvailableResolutions();

    static HashMap<std::string, std::string> GetAvailableLanguages();

    static HashMap<std::string, std::string> GetAvailableKeyboardLayout();

    static Strings::Vector GetAvailableSoundDevices();

    static const Strings::Vector& GetAvailableTimeZone();

    static void GetAvailableShadersIn(const Path& path, Strings::Vector& results);

    static void GetEmbeddedBios(const Path& base, HashMap<std::string, bool>& results);

    static void GetAvailableBios(const Path& path, Path::PathList& results, const HashMap<std::string, bool>& embedded);

    static Strings::Vector GetAvailableShaders();

    static void SetHeaders(Pistache::Http::ResponseWriter& response);

  public:
    //! Configuration file
    static constexpr const char* sConfiguration = "/recalbox/share/system/recalbox.conf";
    //! Region-based System resource path
    static constexpr const char* sSystemResourceRegionPath = "/recalbox/share_init/system/.emulationstation/themes/recalbox-next/%SYSTEM%/data/%REGION%/%FILE%";
    //! Basic System resource path
    static constexpr const char* sSystemResourceBasePath = "/recalbox/share_init/system/.emulationstation/themes/recalbox-next/%SYSTEM%/data/%FILE%";

    //! Device information structure
    struct DeviceInfo
    {
      std::string Mount;          //!< Mount point
      std::string FileSystem;     //!< File system / device
      std::string FileSystemType; //!< File system name
      std::string Bus;            //!< Device bus
      std::string Type;           //!< Device type
      std::string Model;          //!< Device Model name
      long long Size;             //!< Total space in byte
      long long Used;             //!< Used space

      //! Constructor - Store info from df command
      DeviceInfo(const std::string& mount, const std::string& filesystem, const std::string& filesystemtype,
                 const std::string& size, const std::string& used)
        : Mount(mount)
        , FileSystem(filesystem)
        , FileSystemType(filesystemtype)
        , Size(0)
        , Used(0)
      {
        Strings::ToLong(size, Size);
        Strings::ToLong(used, Used);
      }
    };

    /*!
     * @brief Get HTTP method name
     * @param method Method
     * @return Method name
     */
    static const char* MethodToString(Pistache::Http::Method method);

    /*!
     * @brief Log
     * @param request Request to information from
     * @param methodName Method called
     */
    static void LogRoute(const Pistache::Rest::Request& request, const char* methodName);

    /*!
     * @brief Send back a response
     * @param response Response object
     * @param code Http code
     * @param body Response Body
     * @param mime MIME type
     */
    static void Send(Pistache::Http::ResponseWriter& response, Pistache::Http::Code code, const std::string& body, const Pistache::Http::Mime::MediaType& mime);

    /*!
     * @brief Send back a response
     * @param response Response object
     * @param code Http code
     */
    static void Send(Pistache::Http::ResponseWriter& response, Pistache::Http::Code code);

    /*!
     * @brief Serialize a partition object got from "df" line
     * @param parts Split lines. Must contains 7 items
     * @return JSON object
     */
    static JSONBuilder BuildPartitionObject(const DeviceInfo& deviceInfo, const std::string& recalboxtype);

    /*!
     * @brief Get device propertie: *Device name, type and bus
     * @param mount Device
     * @param name Output name
     * @param type Output type
     * @param bus Output bus
     */
    static void GetDevicePropertiesOf(DeviceInfo& deviceInfo);

    /*!
     * @brief Build system resource path, both region-based and basic
     * @param regionPath Output region-based path
     * @param basePath Output basci path
     * @param resourceFileName Target filename
     */
    static void GetSystemResourcePath(Path& regionPath, Path& basePath, const std::string& system, const std::string& region, const char* resourceFileName);

    /*!
     * @brief Execute external command
     * @param command Command to execute
     * @return Output lines
     */
    static Strings::Vector OutputLinesOf(const std::string& command);

    /*!
     * @brief Execute external command
     * @param command Command to execute
     * @return Output lines
     */
    static std::string OutputOf(const std::string& command);

    /*!
     * @brief Send a 404 error
     */
    static void Error404(Pistache::Http::ResponseWriter& response);

    /*!
     * @brief Serialize the given Bios object
     * @param bios Bios object to serialize
     * @return Serialized Bios object
     */
    static JSONBuilder SerializeBiosToJSON(const Bios& bios);

    /*!
     * @brief Serialize the given BiosList object
     * @param biosList BiosList object to serialize
     * @return Serialized BiosList object
     */
    static JSONBuilder SerializeBiosListToJSON(const BiosList& biosList);

    /*!
     * @brief Check and send target resource if it exists or return 404
     * @param resourcepath Resource path
     * @param response Response object
     */
    static void SendResource(const Path& resourcepath, Pistache::Http::ResponseWriter& response, const Pistache::Http::Mime::MediaType& mimeType);

    /*!
     * @brief Check and send target resource if it exists or return 404
     * check and send preferedpath first, then fallback
     * @param preferedpath Prefered resource path
     * @param fallbackpath Fallback resource path if Prefered path does not exists
     * @param response Response object
     * @param mimetype Mime Type
     */
    static void SendResource(const Path& preferedpath, const Path& fallbackpath, Pistache::Http::ResponseWriter& response, const Pistache::Http::Mime::MediaType& mimetype);

    /*!
     * @brief Serialize an emulatortree to json
     * @param emulatorlist Emulator tree
     * @return JSON
     */
    static JSONBuilder SerializeEmulatorsAndCoreToJson(const EmulatorList& emulatorlist);

    /*!
     * @brief Get configuration key list for given namespace
     * @return Key set & configuraion. Empty set if the namespace is unknown
     */
    static const HashMap<std::string, Validator>& SelectConfigurationKeySet(const std::string& _namespace);

    /*!
     * @brief Load recalbox configuration
     * @return Configuration object
     */
    static IniFile LoadConfiguration();

    /*!
     * @brief Save recalbox configuration
     * @param configuration Configuration object
     */
    static void SaveConfiguration(IniFile& configuration);

    /*!
     * @brief Get all configuration values from the given keys of the given domain and build
     * a JSON object. Then send them all back to the caller using the response object
     * @param domain Namespace/Domain
     * @param keys Key/Validator collection
     * @param response Response object
     */
    static void GetKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, Pistache::Http::ResponseWriter& response);

    /*!
     * @brief Get all configuration options from the given keys and build
     * a JSON object. Then send them all back to the caller using the response object
     * @param keys Key/Validator collection
     * @param response Response object
     */
    static void GetKeyValueOptions(const HashMap<std::string, Validator>& keys, Pistache::Http::ResponseWriter& response);

    /*!
     * @brief Get the key/values from the given JSON, try to validate them all using the keys collection, then write all
     * values to the recalbox configuration file.
     * Any unknown value or non-validated value send back a BAD_REQUEST HTTP code.
     * @param domain Namespace/Domain
     * @param keys Key/Validator collection
     * @param json Key/value collection
     * @param response Response object
     */
    static void SetKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, const std::string& json, Pistache::Http::ResponseWriter& response);

    /*!
     * @brief Get the keys from the given JSON, and try to delete them from the given domain, then write all
     * values to the recalbox configuration file.
     * Any unknown value or non-validated value send back a BAD_REQUEST HTTP code.
     * @param domain Namespace/Domain
     * @param keys Key/Validator collection
     * @param json Key/value collection
     * @param response Response object
     */
    static void DeleteKeyValues(const std::string& domain, const HashMap<std::string, Validator>& keys, const std::string& json, Pistache::Http::ResponseWriter& response);

    /*!
     * @brief Compress files in /recalbox/share/bios in zip format en return the zip content
     */
    static Path GetCompressedBiosFolder();

    /*!
     * @brief Check if the given system is in the supported system list
     * @param system System short name
     * @return True if the given system is in the supported system list, false otherwise
     */
    static bool IsValidSystem(const std::string& system);

    /*!
     * @brief Basic but efficient file extractor
     * @param request Request
     * @param filename Extracted filename
     * @param startOffset Start offset of the extracted file in request's body
     * @param size Size of the extracted file
     * @return True if a valid file has been extracted, false otherwize
     */
    static bool GetUploadedFile(const Pistache::Rest::Request& request, std::string& filename, int& startOffset, int& size);

    /*!
     * @brief Extract values from the JSON array from the given request body
     * @param request Requets to extract JSON array from
     * @param result List to fill with array values
     * @return True if the request body is a valid JSON array, false otherwise
     */
    static bool ExtractArray(const Pistache::Rest::Request& request, Strings::Vector& result);
};
