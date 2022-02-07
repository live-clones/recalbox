//
// Created by Bkg2k on 07/02/2020.
//
#pragma once

#include <utils/os/fs/Path.h>

class Regions
{
  public:
    enum class GameRegions : unsigned char
    {
        Unknown = 0, // Unknown/unset region
        DJ, // Djibouti
        ER, // Eritrea
        ET, // Ethiopia
        AF, // Afghanistan
        NA, // Namibia
        ZA, // South Africa
        CM, // Cameroon
        GH, // Ghana
        AM, // Armenia
        AR, // Argentina
        WOR, // World
        AE, // United Arab Emirates
        BH, // Bahrain
        DZ, // Algeria
        EG, // Egypt
        IL, // Israel
        IQ, // Iraq
        JO, // Jordan
        KM, // Comoros
        KW, // Kuwait
        LB, // Lebanon
        LY, // Libya
        MA, // Morocco
        MR, // Mauritania
        OM, // Oman
        PS, // Palestinian Authority
        QA, // Qatar
        SA, // Saudi Arabia
        SD, // Sudan
        SO, // Somalia
        SS, // South Sudan
        SY, // Syria
        TD, // Chad
        TN, // Tunisia
        YE, // Yemen
        CL, // Chile
        AS, // American Samoa
        IN, // India
        TZ, // Tanzania
        ES, // Spain
        AZ, // Azerbaijan
        BA, // Bosnia and Herzegovina
        RU, // Russia
        BE, // Belgium
        BY, // Belarus
        ZM, // Zambia
        BG, // Bulgaria
        NG, // Nigeria
        BM, // Bermuda
        ML, // Mali
        BN, // Brunei
        BD, // Bangladesh
        BO, // Bolivia
        CN, // China
        BR, // Brazil
        FR, // France
        BS, // Bahamas
        CA, // Canada
        AD, // Andorra
        IT, // Italy
        UG, // Uganda
        CO, // Colombia
        CZ, // Czechia
        CU, // Cuba
        CY, // Cyprus
        UK, // United Kingdom
        DK, // Denmark
        GL, // Greenland
        KE, // Kenya
        DE, // Germany
        AT, // Austria
        CH, // Switzerland
        LI, // Liechtenstein
        LU, // Luxembourg
        NE, // Niger
        MV, // Maldives
        SN, // Senegal
        BT, // Bhutan
        EE, // Estonia
        TG, // Togo
        GR, // Greece
        CAR, // Caribbean
        EU, // Europe
        AG, // Antigua and Barbuda
        AI, // Anguilla
        AU, // Australia
        BB, // Barbados
        BI, // Burundi
        BW, // Botswana
        BZ, // Belize
        CC, // Cocos (Keeling) Islands
        CK, // Cook Islands
        CX, // Christmas Island
        DM, // Dominica
        FI, // Finland
        FJ, // Fiji
        FK, // Falkland Islands
        FM, // Micronesia
        GD, // Grenada
        GG, // Guernsey
        GI, // Gibraltar
        GM, // Gambia
        GU, // Guam
        GY, // Guyana
        HK, // Hong Kong SAR
        ID, // Indonesia
        IE, // Ireland
        IM, // Isle of Man
        IO, // British Indian Ocean Territory
        JE, // Jersey
        JM, // Jamaica
        KI, // Kiribati
        KN, // Saint Kitts and Nevis
        KY, // Cayman Islands
        LC, // Saint Lucia
        LR, // Liberia
        LS, // Lesotho
        MG, // Madagascar
        MH, // Marshall Islands
        MO, // Macao SAR
        MP, // Northern Mariana Islands
        MS, // Montserrat
        MT, // Malta
        MU, // Mauritius
        MW, // Malawi
        MY, // Malaysia
        NF, // Norfolk Island
        NL, // Netherlands
        NR, // Nauru
        NU, // Niue
        NZ, // New Zealand
        PG, // Papua New Guinea
        PH, // Philippines
        PK, // Pakistan
        PN, // Pitcairn Islands
        PR, // Puerto Rico
        PW, // Palau
        RW, // Rwanda
        SB, // Solomon Islands
        SC, // Seychelles
        SE, // Sweden
        SG, // Singapore
        SH, // St Helena, Ascension, Tristan da Cunha
        SI, // Slovenia
        SL, // Sierra Leone
        SX, // Sint Maarten
        SZ, // Swaziland
        TC, // Turks and Caicos Islands
        TK, // Tokelau
        TO, // Tonga
        TT, // Trinidad and Tobago
        TV, // Tuvalu
        UM, // U.S. Outlying Islands
        VC, // Saint Vincent and the Grenadines
        VG, // British Virgin Islands
        VI, // U.S. Virgin Islands
        VU, // Vanuatu
        WS, // Samoa
        ZW, // Zimbabwe
        AME, // Latin America
        CR, // Costa Rica
        DO, // Dominican Republic
        EC, // Ecuador
        GQ, // Equatorial Guinea
        GT, // Guatemala
        HN, // Honduras
        MX, // Mexico
        NI, // Nicaragua
        PA, // Panama
        PE, // Peru
        PY, // Paraguay
        SV, // El Salvador
        UY, // Uruguay
        VE, // Venezuela
        IR, // Iran
        GN, // Guinea
        FO, // Faroe Islands
        BF, // Burkina Faso
        BJ, // Benin
        BL, // Saint Barthélemy
        CD, // Congo (DRC)
        CF, // Central African Republic
        CG, // Congo
        CI, // Côte d’Ivoire
        GA, // Gabon
        GF, // French Guiana
        GP, // Guadeloupe
        HT, // Haiti
        MC, // Monaco
        MF, // Saint Martin
        MQ, // Martinique
        NC, // New Caledonia
        PF, // French Polynesia
        PM, // Saint Pierre and Miquelon
        RE, // Réunion
        WF, // Wallis and Futuna
        YT, // Mayotte
        HR, // Croatia
        HU, // Hungary
        IS, // Iceland
        SM, // San Marino
        VA, // Vatican City
        JP, // Japan
        GE, // Georgia
        CV, // Cabo Verde
        KZ, // Kazakhstan
        KH, // Cambodia
        KP, // North Korea
        KR, // Korea
        KG, // Kyrgyzstan
        LA, // Laos
        AO, // Angola
        LT, // Lithuania
        LV, // Latvia
        MZ, // Mozambique
        MK, // Macedonia, FYRO
        MN, // Mongolia
        MM, // Myanmar
        NO, // Norway
        SJ, // Svalbard and Jan Mayen
        NP, // Nepal
        AW, // Aruba
        BQ, // Bonaire, Sint Eustatius and Saba
        CW, // Curaçao
        SR, // Suriname
        PL, // Poland
        PT, // Portugal
        GW, // Guinea-Bissau
        ST, // São Tomé and Príncipe
        TL, // Timor-Leste
        RO, // Romania
        MD, // Moldova
        UA, // Ukraine
        LK, // Sri Lanka
        SK, // Slovakia
        AL, // Albania
        XK, // Kosovo
        ME, // Montenegro
        RS, // Serbia
        AX, // Åland Islands
        TJ, // Tajikistan
        TH, // Thailand
        TM, // Turkmenistan
        TR, // Turkey
        UZ, // Uzbekistan
        VN, // Vietnam
        TW, // Taiwan
        US, // USA
        ASI, // Asia
        __Count
    };

    typedef std::vector<GameRegions> List;

    /*!
     * @brief Get full region name (english)
     * @param region Region to get full name from
     * @return Full name
     */
    static const std::string& RegionFullName(GameRegions region);

    /*!
     * @brief Get region from its full name
     * @param region Region fullname
     * @return GameRegions or Unknown
     */
    static GameRegions FullNameToRegions(const std::string& region);

    /*!
     * @brief Serialize a single region to its string representation
     * @param region Region to serialize
     * @return String
     */
    static const std::string& SerializeRegion(GameRegions region);

    /*!
     * @brief Deserialize a string representation of a region
     * @param region String representation
     * @return Region (Unknown if not reconized)
     */
    static GameRegions DeserializeRegion(const std::string& region);

    /*!
     * @brief Deserialize up to 4 regions from a comma separated string
     * @param regions Regions, comma separated
     * @return Up to 4 regions, one per byte (0 = Unknown)
     */
    static unsigned int Deserialize4Regions(const std::string& regions);

    /*!
     * @brief Serialize up to 4 regions into their string representation, comma separated
     * @param regions Compact regions (1 per byte)
     * @return String representation, comma separated
     */
    static std::string Serialize4Regions(unsigned int regions);

    /*!
     * @brief Extract one region from file name
     * @param path
     * @return
     */
    static GameRegions ExtractRegionsFromFileName(const Path& path);

    static GameRegions DeserializeCustomRegion(const std::string& region);

    /*!
     * @brief Check and clamp the value inside valid range
     * @param region Region to check
     * @return Clamped region
     */
    static GameRegions Clamp(GameRegions region)
    {
      for(GameRegions aregion : AvailableRegions())
        if (region == aregion)
          return region;
      return GameRegions::Unknown;
    }

    /*!
     * @brief Get list of available regions
     * @return Regions
     */
    static const List& AvailableRegions();

    /*!
     * @brief Check if the given region is in one of the 4 compact regions
     * @return true if region is in regions
     */
    static bool IsIn4Regions(unsigned int regions, GameRegions region);

    static GameRegions GameRegionsFromString(const std::string& gameRegions);
    static const std::string& GameRegionsFromEnum(GameRegions gameRegions);

    static unsigned int StringRegionsFromPath(const Path& path);
};

