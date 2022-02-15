//
// Created by Bkg2k on 07/02/2020.
//

#include <string>
#include <utils/storage/HashMap.h>
#include <utils/Strings.h>
#include "Regions.h"

Regions::GameRegions Regions::FullNameToRegions(const std::string& region)
{
  static HashMap<std::string, GameRegions> sFullNameToRegions
    ({
       //     { "djibouti", GameRegions::DJ},
       //     { "eritrea", GameRegions::ER},
       //     { "ethiopia", GameRegions::ET},
       //     { "afghanistan", GameRegions::AF},
       //     { "namibia", GameRegions::NA},
       //     { "south africa", GameRegions::ZA},
       //     { "cameroon", GameRegions::CM},
       //     { "ghana", GameRegions::GH},
       //     { "armenia", GameRegions::AM},
       //     { "argentina", GameRegions::AR},
       { "world", GameRegions::WOR},
       //     { "united arab emirates", GameRegions::AE},
       //     { "bahrain", GameRegions::BH},
       //     { "algeria", GameRegions::DZ},
       //     { "egypt", GameRegions::EG},
       { "israel", GameRegions::IL},
       //     { "iraq", GameRegions::IQ},
       //     { "jordan", GameRegions::JO},
       //     { "comoros", GameRegions::KM},
       //     { "kuwait", GameRegions::KW},
       //     { "lebanon", GameRegions::LB},
       //     { "libya", GameRegions::LY},
       //     { "morocco", GameRegions::MA},
       //     { "mauritania", GameRegions::MR},
       //     { "oman", GameRegions::OM},
       //     { "palestinian authority", GameRegions::PS},
       //     { "qatar", GameRegions::QA},
       //     { "saudi arabia", GameRegions::SA},
       //     { "sudan", GameRegions::SD},
       //     { "somalia", GameRegions::SO},
       //     { "south sudan", GameRegions::SS},
       //     { "syria", GameRegions::SY},
       //     { "chad", GameRegions::TD},
       //     { "tunisia", GameRegions::TN},
       //     { "yemen", GameRegions::YE},
       { "chile", GameRegions::CL},
       //     { "american samoa", GameRegions::AS},
       //     { "india", GameRegions::IN},
       //     { "tanzania", GameRegions::TZ},
       { "spain", GameRegions::ES},
       //     { "azerbaijan", GameRegions::AZ},
       //     { "bosnia and herzegovina", GameRegions::BA},
       { "russia", GameRegions::RU},
       //     { "belgium", GameRegions::BE},
       //     { "belarus", GameRegions::BY},
       //     { "zambia", GameRegions::ZM},
       { "bulgaria", GameRegions::BG},
       //     { "nigeria", GameRegions::NG},
       //     { "bermuda", GameRegions::BM},
       //     { "mali", GameRegions::ML},
       //     { "brunei", GameRegions::BN},
       //     { "bangladesh", GameRegions::BD},
       //     { "bolivia", GameRegions::BO},
       { "china", GameRegions::CN},
       { "brazil", GameRegions::BR},
       { "france", GameRegions::FR},
       //     { "bahamas", GameRegions::BS},
       { "canada", GameRegions::CA},
       //     { "andorra", GameRegions::AD},
       { "italy", GameRegions::IT},
       //     { "uganda", GameRegions::UG},
       //     { "colombia", GameRegions::CO},
       { "czechia", GameRegions::CZ},
       //     { "cuba", GameRegions::CU},
       //     { "cyprus", GameRegions::CY},
       { "united kingdom", GameRegions::UK},
       { "denmark", GameRegions::DK},
       //     { "greenland", GameRegions::GL},
       //     { "kenya", GameRegions::KE},
       { "germany", GameRegions::DE},
       //     { "austria", GameRegions::AT},
       //     { "switzerland", GameRegions::CH},
       //     { "liechtenstein", GameRegions::LI},
       //     { "luxembourg", GameRegions::LU},
       //     { "niger", GameRegions::NE},
       //     { "maldives", GameRegions::MV},
       //     { "senegal", GameRegions::SN},
       //     { "bhutan", GameRegions::BT},
       //     { "estonia", GameRegions::EE},
       //     { "togo", GameRegions::TG},
       { "greece", GameRegions::GR},
       //     { "caribbean", GameRegions::CAR},
       { "europe", GameRegions::EU},
       //     { "antigua and barbuda", GameRegions::AG},
       //     { "anguilla", GameRegions::AI},
       { "australia", GameRegions::AU},
       //     { "barbados", GameRegions::BB},
       //     { "burundi", GameRegions::BI},
       //     { "botswana", GameRegions::BW},
       //     { "belize", GameRegions::BZ},
       //     { "cocos (keeling) islands", GameRegions::CC},
       //     { "cook islands", GameRegions::CK},
       //     { "christmas island", GameRegions::CX},
       //     { "dominica", GameRegions::DM},
       { "finland", GameRegions::FI},
       //     { "fiji", GameRegions::FJ},
       //     { "falkland islands", GameRegions::FK},
       //     { "micronesia", GameRegions::FM},
       //     { "grenada", GameRegions::GD},
       //     { "guernsey", GameRegions::GG},
       //     { "gibraltar", GameRegions::GI},
       //     { "gambia", GameRegions::GM},
       //     { "guam", GameRegions::GU},
       //     { "guyana", GameRegions::GY},
       //     { "hong kong sar", GameRegions::HK},
       //     { "indonesia", GameRegions::ID},
       //     { "ireland", GameRegions::IE},
       //     { "isle of man", GameRegions::IM},
       //     { "british indian ocean territory", GameRegions::IO},
       //     { "jersey", GameRegions::JE},
       //     { "jamaica", GameRegions::JM},
       //     { "kiribati", GameRegions::KI},
       //     { "saint kitts and nevis", GameRegions::KN},
       //     { "cayman islands", GameRegions::KY},
       //     { "saint lucia", GameRegions::LC},
       //     { "liberia", GameRegions::LR},
       //     { "lesotho", GameRegions::LS},
       //     { "madagascar", GameRegions::MG},
       //     { "marshall islands", GameRegions::MH},
       //     { "macao sar", GameRegions::MO},
       //     { "northern mariana islands", GameRegions::MP},
       //     { "montserrat", GameRegions::MS},
       //     { "malta", GameRegions::MT},
       //     { "mauritius", GameRegions::MU},
       //     { "malawi", GameRegions::MW},
       //     { "malaysia", GameRegions::MY},
       //     { "norfolk island", GameRegions::NF},
       { "netherlands", GameRegions::NL},
       //     { "nauru", GameRegions::NR},
       //     { "niue", GameRegions::NU},
       { "new zealand", GameRegions::NZ},
       //     { "papua new guinea", GameRegions::PG},
       //     { "philippines", GameRegions::PH},
       //     { "pakistan", GameRegions::PK},
       //     { "pitcairn islands", GameRegions::PN},
       //     { "puerto rico", GameRegions::PR},
       //     { "palau", GameRegions::PW},
       //     { "rwanda", GameRegions::RW},
       //     { "solomon islands", GameRegions::SB},
       //     { "seychelles", GameRegions::SC},
       { "sweden", GameRegions::SE},
       //     { "singapore", GameRegions::SG},
       //     { "st helena, ascension, tristan da cunha", GameRegions::SH},
       //     { "slovenia", GameRegions::SI},
       //     { "sierra leone", GameRegions::SL},
       //     { "sint maarten", GameRegions::SX},
       //     { "swaziland", GameRegions::SZ},
       //     { "turks and caicos islands", GameRegions::TC},
       //     { "tokelau", GameRegions::TK},
       //     { "tonga", GameRegions::TO},
       //     { "trinidad and tobago", GameRegions::TT},
       //     { "tuvalu", GameRegions::TV},
       //     { "u.s. outlying islands", GameRegions::UM},
       //     { "saint vincent and the grenadines", GameRegions::VC},
       //     { "british virgin islands", GameRegions::VG},
       //     { "u.s. virgin islands", GameRegions::VI},
       //     { "vanuatu", GameRegions::VU},
       //     { "samoa", GameRegions::WS},
       //     { "zimbabwe", GameRegions::ZW},
       { "latin america", GameRegions::AME},
       //     { "costa rica", GameRegions::CR},
       //     { "dominican republic", GameRegions::DO},
       //     { "ecuador", GameRegions::EC},
       //     { "equatorial guinea", GameRegions::GQ},
       //     { "guatemala", GameRegions::GT},
       //     { "honduras", GameRegions::HN},
       //     { "mexico", GameRegions::MX},
       //     { "nicaragua", GameRegions::NI},
       //     { "panama", GameRegions::PA},
       //     { "peru", GameRegions::PE},
       //     { "paraguay", GameRegions::PY},
       //     { "el salvador", GameRegions::SV},
       //     { "uruguay", GameRegions::UY},
       //     { "venezuela", GameRegions::VE},
       //     { "iran", GameRegions::IR},
       //     { "guinea", GameRegions::GN},
       //     { "faroe islands", GameRegions::FO},
       //     { "burkina faso", GameRegions::BF},
       //     { "benin", GameRegions::BJ},
       //     { "saint barthélemy", GameRegions::BL},
       //     { "congo (drc)", GameRegions::CD},
       //     { "central african republic", GameRegions::CF},
       //     { "congo", GameRegions::CG},
       //     { "côte d’ivoire", GameRegions::CI},
       //     { "gabon", GameRegions::GA},
       //     { "french guiana", GameRegions::GF},
       //     { "guadeloupe", GameRegions::GP},
       //     { "haiti", GameRegions::HT},
       //     { "monaco", GameRegions::MC},
       //     { "saint martin", GameRegions::MF},
       //     { "martinique", GameRegions::MQ},
       //     { "new caledonia", GameRegions::NC},
       //     { "french polynesia", GameRegions::PF},
       //     { "saint pierre and miquelon", GameRegions::PM},
       //     { "réunion", GameRegions::RE},
       //     { "wallis and futuna", GameRegions::WF},
       //     { "mayotte", GameRegions::YT},
       //     { "croatia", GameRegions::HR},
       { "hungary", GameRegions::HU},
       //     { "iceland", GameRegions::IS},
       //     { "san marino", GameRegions::SM},
       //     { "vatican city", GameRegions::VA},
       { "japan", GameRegions::JP},
       //     { "georgia", GameRegions::GE},
       //     { "cabo verde", GameRegions::CV},
       //     { "kazakhstan", GameRegions::KZ},
       //     { "cambodia", GameRegions::KH},
       //     { "north korea", GameRegions::KP},
       { "korea", GameRegions::KR},
       //     { "kyrgyzstan", GameRegions::KG},
       //     { "laos", GameRegions::LA},
       //     { "angola", GameRegions::AO},
       //     { "lithuania", GameRegions::LT},
       //     { "latvia", GameRegions::LV},
       //     { "mozambique", GameRegions::MZ},
       //     { "macedonia, fyro", GameRegions::MK},
       //     { "mongolia", GameRegions::MN},
       //     { "myanmar", GameRegions::MM},
       { "norway", GameRegions::NO},
       //     { "svalbard and jan mayen", GameRegions::SJ},
       //     { "nepal", GameRegions::NP},
       //     { "aruba", GameRegions::AW},
       //     { "bonaire, sint eustatius and saba", GameRegions::BQ},
       //     { "curaçao", GameRegions::CW},
       //     { "suriname", GameRegions::SR},
       { "poland", GameRegions::PL},
       { "portugal", GameRegions::PT},
       //     { "guinea-bissau", GameRegions::GW},
       //     { "são tomé and príncipe", GameRegions::ST},
       //     { "timor-leste", GameRegions::TL},
       //     { "romania", GameRegions::RO},
       //     { "moldova", GameRegions::MD},
       //     { "ukraine", GameRegions::UA},
       //     { "sri lanka", GameRegions::LK},
       //     { "slovakia", GameRegions::SK},
       //     { "albania", GameRegions::AL},
       //     { "kosovo", GameRegions::XK},
       //     { "montenegro", GameRegions::ME},
       //     { "serbia", GameRegions::RS},
       //     { "åland islands", GameRegions::AX},
       //     { "tajikistan", GameRegions::TJ},
       //     { "thailand", GameRegions::TH},
       //     { "turkmenistan", GameRegions::TM},
            { "turkey", GameRegions::TR},
       //     { "uzbekistan", GameRegions::UZ},
       //     { "vietnam", GameRegions::VN},
            { "taiwan", GameRegions::TW},
       { "usa", GameRegions::US},
       { "asia", GameRegions::ASI},
     });

  GameRegions* found = sFullNameToRegions.try_get(Strings::ToLowerASCII(region));
  if (found != nullptr)
    return *found;

  return GameRegions::Unknown;
}

const std::string& Regions::RegionFullName(Regions::GameRegions region)
{
  static HashMap<GameRegions, const std::string> sRegionToFullName
    ({
//       { GameRegions::DJ, "Djibouti" },
//       { GameRegions::ER, "Eritrea" },
//       { GameRegions::ET, "Ethiopia" },
//       { GameRegions::AF, "Afghanistan" },
//       { GameRegions::NA, "Namibia" },
//       { GameRegions::ZA, "South Africa" },
//       { GameRegions::CM, "Cameroon" },
//       { GameRegions::GH, "Ghana" },
//       { GameRegions::AM, "Armenia" },
//       { GameRegions::AR, "Argentina" },
       { GameRegions::WOR, "World" },
//       { GameRegions::AE, "United Arab Emirates" },
//       { GameRegions::BH, "Bahrain" },
//       { GameRegions::DZ, "Algeria" },
//       { GameRegions::EG, "Egypt" },
       { GameRegions::IL, "Israel" },
//       { GameRegions::IQ, "Iraq" },
//       { GameRegions::JO, "Jordan" },
//       { GameRegions::KM, "Comoros" },
//       { GameRegions::KW, "Kuwait" },
//       { GameRegions::LB, "Lebanon" },
//       { GameRegions::LY, "Libya" },
//       { GameRegions::MA, "Morocco" },
//       { GameRegions::MR, "Mauritania" },
//       { GameRegions::OM, "Oman" },
//       { GameRegions::PS, "Palestinian Authority" },
//       { GameRegions::QA, "Qatar" },
//       { GameRegions::SA, "Saudi Arabia" },
//       { GameRegions::SD, "Sudan" },
//       { GameRegions::SO, "Somalia" },
//       { GameRegions::SS, "South Sudan" },
//       { GameRegions::SY, "Syria" },
//       { GameRegions::TD, "Chad" },
//       { GameRegions::TN, "Tunisia" },
//       { GameRegions::YE, "Yemen" },
       { GameRegions::CL, "Chile" },
//       { GameRegions::AS, "American Samoa" },
//       { GameRegions::IN, "India" },
//       { GameRegions::TZ, "Tanzania" },
       { GameRegions::ES, "Spain" },
//       { GameRegions::AZ, "Azerbaijan" },
//       { GameRegions::BA, "Bosnia and Herzegovina" },
       { GameRegions::RU, "Russia" },
//       { GameRegions::BE, "Belgium" },
//       { GameRegions::BY, "Belarus" },
//       { GameRegions::ZM, "Zambia" },
       { GameRegions::BG, "Bulgaria" },
//       { GameRegions::NG, "Nigeria" },
//       { GameRegions::BM, "Bermuda" },
//       { GameRegions::ML, "Mali" },
//       { GameRegions::BN, "Brunei" },
//       { GameRegions::BD, "Bangladesh" },
//       { GameRegions::BO, "Bolivia" },
       { GameRegions::CN, "China" },
       { GameRegions::BR, "Brazil" },
       { GameRegions::FR, "France" },
//       { GameRegions::BS, "Bahamas" },
       { GameRegions::CA, "Canada" },
//       { GameRegions::AD, "Andorra" },
       { GameRegions::IT, "Italy" },
//       { GameRegions::UG, "Uganda" },
//       { GameRegions::CO, "Colombia" },
       { GameRegions::CZ, "Czechia" },
//       { GameRegions::CU, "Cuba" },
//       { GameRegions::CY, "Cyprus" },
       { GameRegions::UK, "United Kingdom" },
       { GameRegions::DK, "Denmark" },
//       { GameRegions::GL, "Greenland" },
//       { GameRegions::KE, "Kenya" },
       { GameRegions::DE, "Germany" },
//       { GameRegions::AT, "Austria" },
//       { GameRegions::CH, "Switzerland" },
//       { GameRegions::LI, "Liechtenstein" },
//       { GameRegions::LU, "Luxembourg" },
//       { GameRegions::NE, "Niger" },
//       { GameRegions::MV, "Maldives" },
//       { GameRegions::SN, "Senegal" },
//       { GameRegions::BT, "Bhutan" },
//       { GameRegions::EE, "Estonia" },
//       { GameRegions::TG, "Togo" },
       { GameRegions::GR, "Greece" },
//       { GameRegions::CAR, "Caribbean" },
       { GameRegions::EU, "Europe" },
//       { GameRegions::AG, "Antigua and Barbuda" },
//       { GameRegions::AI, "Anguilla" },
       { GameRegions::AU, "Australia" },
//       { GameRegions::BB, "Barbados" },
//       { GameRegions::BI, "Burundi" },
//       { GameRegions::BW, "Botswana" },
//       { GameRegions::BZ, "Belize" },
//       { GameRegions::CC, "Cocos (Keeling) Islands" },
//       { GameRegions::CK, "Cook Islands" },
//       { GameRegions::CX, "Christmas Island" },
//       { GameRegions::DM, "Dominica" },
       { GameRegions::FI, "Finland" },
//       { GameRegions::FJ, "Fiji" },
//       { GameRegions::FK, "Falkland Islands" },
//       { GameRegions::FM, "Micronesia" },
//       { GameRegions::GD, "Grenada" },
//       { GameRegions::GG, "Guernsey" },
//       { GameRegions::GI, "Gibraltar" },
//       { GameRegions::GM, "Gambia" },
//       { GameRegions::GU, "Guam" },
//       { GameRegions::GY, "Guyana" },
//       { GameRegions::HK, "Hong Kong SAR" },
//       { GameRegions::ID, "Indonesia" },
//       { GameRegions::IE, "Ireland" },
//       { GameRegions::IM, "Isle of Man" },
//       { GameRegions::IO, "British Indian Ocean Territory" },
//       { GameRegions::JE, "Jersey" },
//       { GameRegions::JM, "Jamaica" },
//       { GameRegions::KI, "Kiribati" },
//       { GameRegions::KN, "Saint Kitts and Nevis" },
//       { GameRegions::KY, "Cayman Islands" },
//       { GameRegions::LC, "Saint Lucia" },
//       { GameRegions::LR, "Liberia" },
//       { GameRegions::LS, "Lesotho" },
//       { GameRegions::MG, "Madagascar" },
//       { GameRegions::MH, "Marshall Islands" },
//       { GameRegions::MO, "Macao SAR" },
//       { GameRegions::MP, "Northern Mariana Islands" },
//       { GameRegions::MS, "Montserrat" },
//       { GameRegions::MT, "Malta" },
//       { GameRegions::MU, "Mauritius" },
//       { GameRegions::MW, "Malawi" },
//       { GameRegions::MY, "Malaysia" },
//       { GameRegions::NF, "Norfolk Island" },
       { GameRegions::NL, "Netherlands" },
//       { GameRegions::NR, "Nauru" },
//       { GameRegions::NU, "Niue" },
       { GameRegions::NZ, "New Zealand" },
//       { GameRegions::PG, "Papua New Guinea" },
//       { GameRegions::PH, "Philippines" },
//       { GameRegions::PK, "Pakistan" },
//       { GameRegions::PN, "Pitcairn Islands" },
//       { GameRegions::PR, "Puerto Rico" },
//       { GameRegions::PW, "Palau" },
//       { GameRegions::RW, "Rwanda" },
//       { GameRegions::SB, "Solomon Islands" },
//       { GameRegions::SC, "Seychelles" },
       { GameRegions::SE, "Sweden" },
//       { GameRegions::SG, "Singapore" },
//       { GameRegions::SH, "St Helena, Ascension, Tristan da Cunha" },
//       { GameRegions::SI, "Slovenia" },
//       { GameRegions::SL, "Sierra Leone" },
//       { GameRegions::SX, "Sint Maarten" },
//       { GameRegions::SZ, "Swaziland" },
//       { GameRegions::TC, "Turks and Caicos Islands" },
//       { GameRegions::TK, "Tokelau" },
//       { GameRegions::TO, "Tonga" },
//       { GameRegions::TT, "Trinidad and Tobago" },
//       { GameRegions::TV, "Tuvalu" },
//       { GameRegions::UM, "U.S. Outlying Islands" },
//       { GameRegions::VC, "Saint Vincent and the Grenadines" },
//       { GameRegions::VG, "British Virgin Islands" },
//       { GameRegions::VI, "U.S. Virgin Islands" },
//       { GameRegions::VU, "Vanuatu" },
//       { GameRegions::WS, "Samoa" },
//       { GameRegions::ZW, "Zimbabwe" },
       { GameRegions::AME, "Latin America" },
//       { GameRegions::CR, "Costa Rica" },
//       { GameRegions::DO, "Dominican Republic" },
//       { GameRegions::EC, "Ecuador" },
//       { GameRegions::GQ, "Equatorial Guinea" },
//       { GameRegions::GT, "Guatemala" },
//       { GameRegions::HN, "Honduras" },
//       { GameRegions::MX, "Mexico" },
//       { GameRegions::NI, "Nicaragua" },
//       { GameRegions::PA, "Panama" },
//       { GameRegions::PE, "Peru" },
//       { GameRegions::PY, "Paraguay" },
//       { GameRegions::SV, "El Salvador" },
//       { GameRegions::UY, "Uruguay" },
//       { GameRegions::VE, "Venezuela" },
//       { GameRegions::IR, "Iran" },
//       { GameRegions::GN, "Guinea" },
//       { GameRegions::FO, "Faroe Islands" },
//       { GameRegions::BF, "Burkina Faso" },
//       { GameRegions::BJ, "Benin" },
//       { GameRegions::BL, "Saint Barthélemy" },
//       { GameRegions::CD, "Congo (DRC)" },
//       { GameRegions::CF, "Central African Republic" },
//       { GameRegions::CG, "Congo" },
//       { GameRegions::CI, "Côte d’Ivoire" },
//       { GameRegions::GA, "Gabon" },
//       { GameRegions::GF, "French Guiana" },
//       { GameRegions::GP, "Guadeloupe" },
//       { GameRegions::HT, "Haiti" },
//       { GameRegions::MC, "Monaco" },
//       { GameRegions::MF, "Saint Martin" },
//       { GameRegions::MQ, "Martinique" },
//       { GameRegions::NC, "New Caledonia" },
//       { GameRegions::PF, "French Polynesia" },
//       { GameRegions::PM, "Saint Pierre and Miquelon" },
//       { GameRegions::RE, "Réunion" },
//       { GameRegions::WF, "Wallis and Futuna" },
//       { GameRegions::YT, "Mayotte" },
//       { GameRegions::HR, "Croatia" },
       { GameRegions::HU, "Hungary" },
//       { GameRegions::IS, "Iceland" },
//       { GameRegions::SM, "San Marino" },
//       { GameRegions::VA, "Vatican City" },
       { GameRegions::JP, "Japan" },
//       { GameRegions::GE, "Georgia" },
//       { GameRegions::CV, "Cabo Verde" },
//       { GameRegions::KZ, "Kazakhstan" },
//       { GameRegions::KH, "Cambodia" },
//       { GameRegions::KP, "North Korea" },
       { GameRegions::KR, "Korea" },
//       { GameRegions::KG, "Kyrgyzstan" },
//       { GameRegions::LA, "Laos" },
//       { GameRegions::AO, "Angola" },
//       { GameRegions::LT, "Lithuania" },
//       { GameRegions::LV, "Latvia" },
//       { GameRegions::MZ, "Mozambique" },
//       { GameRegions::MK, "Macedonia, FYRO" },
//       { GameRegions::MN, "Mongolia" },
//       { GameRegions::MM, "Myanmar" },
       { GameRegions::NO, "Norway" },
//       { GameRegions::SJ, "Svalbard and Jan Mayen" },
//       { GameRegions::NP, "Nepal" },
//       { GameRegions::AW, "Aruba" },
//       { GameRegions::BQ, "Bonaire, Sint Eustatius and Saba" },
//       { GameRegions::CW, "Curaçao" },
//       { GameRegions::SR, "Suriname" },
       { GameRegions::PL, "Poland" },
       { GameRegions::PT, "Portugal" },
//       { GameRegions::GW, "Guinea-Bissau" },
//       { GameRegions::ST, "São Tomé and Príncipe" },
//       { GameRegions::TL, "Timor-Leste" },
//       { GameRegions::RO, "Romania" },
//       { GameRegions::MD, "Moldova" },
//       { GameRegions::UA, "Ukraine" },
//       { GameRegions::LK, "Sri Lanka" },
      { GameRegions::SK, "Slovakia" },
//       { GameRegions::AL, "Albania" },
//       { GameRegions::XK, "Kosovo" },
//       { GameRegions::ME, "Montenegro" },
//       { GameRegions::RS, "Serbia" },
//       { GameRegions::AX, "Åland Islands" },
//       { GameRegions::TJ, "Tajikistan" },
//       { GameRegions::TH, "Thailand" },
//       { GameRegions::TM, "Turkmenistan" },
       { GameRegions::TR, "Turkey" },
//       { GameRegions::UZ, "Uzbekistan" },
//       { GameRegions::VN, "Vietnam" },
       { GameRegions::TW, "Taiwan" },
       { GameRegions::US, "USA" },
       { GameRegions::ASI, "Asia" },
     });

  const std::string* found = sRegionToFullName.try_get(region);
  if (found != nullptr)
    return *found;

  static std::string sUnknown("Unknown Region");
  return sUnknown;
}

const std::string& Regions::SerializeRegion(Regions::GameRegions region)
{
  static HashMap<Regions::GameRegions, std::string> sRegionToRegionName
    ({
       //       { GameRegions::DJ, "dj" }, // Djibouti
       //       { GameRegions::ER, "er" }, // Eritrea
       //       { GameRegions::ET, "et" }, // Ethiopia
       //       { GameRegions::AF, "af" }, // Afghanistan
       //       { GameRegions::NA, "na" }, // Namibia
       //       { GameRegions::ZA, "za" }, // South Africa
       //       { GameRegions::CM, "cm" }, // Cameroon
       //       { GameRegions::GH, "gh" }, // Ghana
       //       { GameRegions::AM, "am" }, // Armenia
       //       { GameRegions::AR, "ar" }, // Argentina
       { GameRegions::WOR, "wor" }, // World
       //       { GameRegions::AE, "ae" }, // United Arab Emirates
       //       { GameRegions::BH, "bh" }, // Bahrain
       //       { GameRegions::DZ, "dz" }, // Algeria
       //       { GameRegions::EG, "eg" }, // Egypt
       { GameRegions::IL, "il" }, // Israel
       //       { GameRegions::IQ, "iq" }, // Iraq
       //       { GameRegions::JO, "jo" }, // Jordan
       //       { GameRegions::KM, "km" }, // Comoros
       //       { GameRegions::KW, "kw" }, // Kuwait
       //       { GameRegions::LB, "lb" }, // Lebanon
       //       { GameRegions::LY, "ly" }, // Libya
       //       { GameRegions::MA, "ma" }, // Morocco
       //       { GameRegions::MR, "mr" }, // Mauritania
       //       { GameRegions::OM, "om" }, // Oman
       //       { GameRegions::PS, "ps" }, // Palestinian Authority
       //       { GameRegions::QA, "qa" }, // Qatar
       //       { GameRegions::SA, "sa" }, // Saudi Arabia
       //       { GameRegions::SD, "sd" }, // Sudan
       //       { GameRegions::SO, "so" }, // Somalia
       //       { GameRegions::SS, "ss" }, // South Sudan
       //       { GameRegions::SY, "sy" }, // Syria
       //       { GameRegions::TD, "td" }, // Chad
       //       { GameRegions::TN, "tn" }, // Tunisia
       //       { GameRegions::YE, "ye" }, // Yemen
       { GameRegions::CL, "cl" }, // Chile
       //       { GameRegions::AS, "as" }, // American Samoa
       //       { GameRegions::IN, "in" }, // India
       //       { GameRegions::TZ, "tz" }, // Tanzania
       { GameRegions::ES, "es" }, // Spain
       //       { GameRegions::AZ, "az" }, // Azerbaijan
       //       { GameRegions::BA, "ba" }, // Bosnia and Herzegovina
       { GameRegions::RU, "ru" }, // Russia
       //       { GameRegions::BE, "be" }, // Belgium
       //       { GameRegions::BY, "by" }, // Belarus
       //       { GameRegions::ZM, "zm" }, // Zambia
       { GameRegions::BG, "bg" }, // Bulgaria
       //       { GameRegions::NG, "ng" }, // Nigeria
       //       { GameRegions::BM, "bm" }, // Bermuda
       //       { GameRegions::ML, "ml" }, // Mali
       //       { GameRegions::BN, "bn" }, // Brunei
       //       { GameRegions::BD, "bd" }, // Bangladesh
       //       { GameRegions::BO, "bo" }, // Bolivia
       { GameRegions::CN, "cn" }, // China
       { GameRegions::BR, "br" }, // Brazil
       { GameRegions::FR, "fr" }, // France
       //       { GameRegions::BS, "bs" }, // Bahamas
       { GameRegions::CA, "ca" }, // Canada
       //       { GameRegions::AD, "ad" }, // Andorra
       { GameRegions::IT, "it" }, // Italy
       //       { GameRegions::UG, "ug" }, // Uganda
       //       { GameRegions::CO, "co" }, // Colombia
       { GameRegions::CZ, "cz" }, // Czechia
       //       { GameRegions::CU, "cu" }, // Cuba
       //       { GameRegions::CY, "cy" }, // Cyprus
       { GameRegions::UK, "uk" }, // United Kingdom
       { GameRegions::DK, "dk" }, // Denmark
       //       { GameRegions::GL, "gl" }, // Greenland
       //       { GameRegions::KE, "ke" }, // Kenya
       { GameRegions::DE, "de" }, // Germany
       //       { GameRegions::AT, "at" }, // Austria
       //       { GameRegions::CH, "ch" }, // Switzerland
       //       { GameRegions::LI, "li" }, // Liechtenstein
       //       { GameRegions::LU, "lu" }, // Luxembourg
       //       { GameRegions::NE, "ne" }, // Niger
       //       { GameRegions::MV, "mv" }, // Maldives
       //       { GameRegions::SN, "sn" }, // Senegal
       //       { GameRegions::BT, "bt" }, // Bhutan
       //       { GameRegions::EE, "ee" }, // Estonia
       //       { GameRegions::TG, "tg" }, // Togo
       { GameRegions::GR, "gr" }, // Greece
       //       { GameRegions::CAR, "car" }, // Caribbean
       { GameRegions::EU, "eu" }, // Europe
       //       { GameRegions::AG, "ag" }, // Antigua and Barbuda
       //       { GameRegions::AI, "ai" }, // Anguilla
       { GameRegions::AU, "au" }, // Australia
       //       { GameRegions::BB, "bb" }, // Barbados
       //       { GameRegions::BI, "bi" }, // Burundi
       //       { GameRegions::BW, "bw" }, // Botswana
       //       { GameRegions::BZ, "bz" }, // Belize
       //       { GameRegions::CC, "cc" }, // Cocos (Keeling) Islands
       //       { GameRegions::CK, "ck" }, // Cook Islands
       //       { GameRegions::CX, "cx" }, // Christmas Island
       //       { GameRegions::DM, "dm" }, // Dominica
       { GameRegions::FI, "fi" }, // Finland
       //       { GameRegions::FJ, "fj" }, // Fiji
       //       { GameRegions::FK, "fk" }, // Falkland Islands
       //       { GameRegions::FM, "fm" }, // Micronesia
       //       { GameRegions::GD, "gd" }, // Grenada
       //       { GameRegions::GG, "gg" }, // Guernsey
       //       { GameRegions::GI, "gi" }, // Gibraltar
       //       { GameRegions::GM, "gm" }, // Gambia
       //       { GameRegions::GU, "gu" }, // Guam
       //       { GameRegions::GY, "gy" }, // Guyana
       //       { GameRegions::HK, "hk" }, // Hong Kong SAR
       //       { GameRegions::ID, "id" }, // Indonesia
       //       { GameRegions::IE, "ie" }, // Ireland
       //       { GameRegions::IM, "im" }, // Isle of Man
       //       { GameRegions::IO, "io" }, // British Indian Ocean Territory
       //       { GameRegions::JE, "je" }, // Jersey
       //       { GameRegions::JM, "jm" }, // Jamaica
       //       { GameRegions::KI, "ki" }, // Kiribati
       //       { GameRegions::KN, "kn" }, // Saint Kitts and Nevis
       //       { GameRegions::KY, "ky" }, // Cayman Islands
       //       { GameRegions::LC, "lc" }, // Saint Lucia
       //       { GameRegions::LR, "lr" }, // Liberia
       //       { GameRegions::LS, "ls" }, // Lesotho
       //       { GameRegions::MG, "mg" }, // Madagascar
       //       { GameRegions::MH, "mh" }, // Marshall Islands
       //       { GameRegions::MO, "mo" }, // Macao SAR
       //       { GameRegions::MP, "mp" }, // Northern Mariana Islands
       //       { GameRegions::MS, "ms" }, // Montserrat
       //       { GameRegions::MT, "mt" }, // Malta
       //       { GameRegions::MU, "mu" }, // Mauritius
       //       { GameRegions::MW, "mw" }, // Malawi
       //       { GameRegions::MY, "my" }, // Malaysia
       //       { GameRegions::NF, "nf" }, // Norfolk Island
       { GameRegions::NL, "nl" }, // Netherlands
       //       { GameRegions::NR, "nr" }, // Nauru
       //       { GameRegions::NU, "nu" }, // Niue
       { GameRegions::NZ, "nz" }, // New Zealand
       //       { GameRegions::PG, "pg" }, // Papua New Guinea
       //       { GameRegions::PH, "ph" }, // Philippines
       //       { GameRegions::PK, "pk" }, // Pakistan
       //       { GameRegions::PN, "pn" }, // Pitcairn Islands
       //       { GameRegions::PR, "pr" }, // Puerto Rico
       //       { GameRegions::PW, "pw" }, // Palau
       //       { GameRegions::RW, "rw" }, // Rwanda
       //       { GameRegions::SB, "sb" }, // Solomon Islands
       //       { GameRegions::SC, "sc" }, // Seychelles
       { GameRegions::SE, "se" }, // Sweden
       //       { GameRegions::SG, "sg" }, // Singapore
       //       { GameRegions::SH, "sh" }, // St Helena, Ascension, Tristan da Cunha
       //       { GameRegions::SI, "si" }, // Slovenia
       //       { GameRegions::SL, "sl" }, // Sierra Leone
       //       { GameRegions::SX, "sx" }, // Sint Maarten
       //       { GameRegions::SZ, "sz" }, // Swaziland
       //       { GameRegions::TC, "tc" }, // Turks and Caicos Islands
       //       { GameRegions::TK, "tk" }, // Tokelau
       //       { GameRegions::TO, "to" }, // Tonga
       //       { GameRegions::TT, "tt" }, // Trinidad and Tobago
       //       { GameRegions::TV, "tv" }, // Tuvalu
       //       { GameRegions::UM, "um" }, // U.S. Outlying Islands
       //       { GameRegions::VC, "vc" }, // Saint Vincent and the Grenadines
       //       { GameRegions::VG, "vg" }, // British Virgin Islands
       //       { GameRegions::VI, "vi" }, // U.S. Virgin Islands
       //       { GameRegions::VU, "vu" }, // Vanuatu
       //       { GameRegions::WS, "ws" }, // Samoa
       //       { GameRegions::ZW, "zw" }, // Zimbabwe
       { GameRegions::AME, "ame" }, // Latin America
       //       { GameRegions::CR, "cr" }, // Costa Rica
       //       { GameRegions::DO, "do" }, // Dominican Republic
       //       { GameRegions::EC, "ec" }, // Ecuador
       //       { GameRegions::GQ, "gq" }, // Equatorial Guinea
       //       { GameRegions::GT, "gt" }, // Guatemala
       //       { GameRegions::HN, "hn" }, // Honduras
       //       { GameRegions::MX, "mx" }, // Mexico
       //       { GameRegions::NI, "ni" }, // Nicaragua
       //       { GameRegions::PA, "pa" }, // Panama
       //       { GameRegions::PE, "pe" }, // Peru
       //       { GameRegions::PY, "py" }, // Paraguay
       //       { GameRegions::SV, "sv" }, // El Salvador
       //       { GameRegions::UY, "uy" }, // Uruguay
       //       { GameRegions::VE, "ve" }, // Venezuela
       //       { GameRegions::IR, "ir" }, // Iran
       //       { GameRegions::GN, "gn" }, // Guinea
       //       { GameRegions::FO, "fo" }, // Faroe Islands
       //       { GameRegions::BF, "bf" }, // Burkina Faso
       //       { GameRegions::BJ, "bj" }, // Benin
       //       { GameRegions::BL, "bl" }, // Saint Barthélemy
       //       { GameRegions::CD, "cd" }, // Congo (DRC)
       //       { GameRegions::CF, "cf" }, // Central African Republic
       //       { GameRegions::CG, "cg" }, // Congo
       //       { GameRegions::CI, "ci" }, // Côte d’Ivoire
       //       { GameRegions::GA, "ga" }, // Gabon
       //       { GameRegions::GF, "gf" }, // French Guiana
       //       { GameRegions::GP, "gp" }, // Guadeloupe
       //       { GameRegions::HT, "ht" }, // Haiti
       //       { GameRegions::MC, "mc" }, // Monaco
       //       { GameRegions::MF, "mf" }, // Saint Martin
       //       { GameRegions::MQ, "mq" }, // Martinique
       //       { GameRegions::NC, "nc" }, // New Caledonia
       //       { GameRegions::PF, "pf" }, // French Polynesia
       //       { GameRegions::PM, "pm" }, // Saint Pierre and Miquelon
       //       { GameRegions::RE, "re" }, // Réunion
       //       { GameRegions::WF, "wf" }, // Wallis and Futuna
       //       { GameRegions::YT, "yt" }, // Mayotte
       //       { GameRegions::HR, "hr" }, // Croatia
       { GameRegions::HU, "hu" }, // Hungary
       //       { GameRegions::IS, "is" }, // Iceland
       //       { GameRegions::SM, "sm" }, // San Marino
       //       { GameRegions::VA, "va" }, // Vatican City
       { GameRegions::JP, "jp" }, // Japan
       //       { GameRegions::GE, "ge" }, // Georgia
       //       { GameRegions::CV, "cv" }, // Cabo Verde
       //       { GameRegions::KZ, "kz" }, // Kazakhstan
       //       { GameRegions::KH, "kh" }, // Cambodia
       //       { GameRegions::KP, "kp" }, // North Korea
       { GameRegions::KR, "kr" }, // Korea
       //       { GameRegions::KG, "kg" }, // Kyrgyzstan
       //       { GameRegions::LA, "la" }, // Laos
       //       { GameRegions::AO, "ao" }, // Angola
       //       { GameRegions::LT, "lt" }, // Lithuania
       //       { GameRegions::LV, "lv" }, // Latvia
       //       { GameRegions::MZ, "mz" }, // Mozambique
       //       { GameRegions::MK, "mk" }, // Macedonia, FYRO
       //       { GameRegions::MN, "mn" }, // Mongolia
       //       { GameRegions::MM, "mm" }, // Myanmar
       { GameRegions::NO, "no" }, // Norway
       //       { GameRegions::SJ, "sj" }, // Svalbard and Jan Mayen
       //       { GameRegions::NP, "np" }, // Nepal
       //       { GameRegions::AW, "aw" }, // Aruba
       //       { GameRegions::BQ, "bq" }, // Bonaire, Sint Eustatius and Saba
       //       { GameRegions::CW, "cw" }, // Curaçao
       //       { GameRegions::SR, "sr" }, // Suriname
       { GameRegions::PL, "pl" }, // Poland
       { GameRegions::PT, "pt" }, // Portugal
       //       { GameRegions::GW, "gw" }, // Guinea-Bissau
       //       { GameRegions::ST, "st" }, // São Tomé and Príncipe
       //       { GameRegions::TL, "tl" }, // Timor-Leste
       //       { GameRegions::RO, "ro" }, // Romania
       //       { GameRegions::MD, "md" }, // Moldova
       //       { GameRegions::UA, "ua" }, // Ukraine
       //       { GameRegions::LK, "lk" }, // Sri Lanka
       { GameRegions::SK, "sk" }, // Slovakia
       //       { GameRegions::AL, "al" }, // Albania
       //       { GameRegions::XK, "xk" }, // Kosovo
       //       { GameRegions::ME, "me" }, // Montenegro
       //       { GameRegions::RS, "rs" }, // Serbia
       //       { GameRegions::AX, "ax" }, // Åland Islands
       //       { GameRegions::TJ, "tj" }, // Tajikistan
       //       { GameRegions::TH, "th" }, // Thailand
       //       { GameRegions::TM, "tm" }, // Turkmenistan
       //       { GameRegions::TR, "tr" }, // Turkey
       //       { GameRegions::UZ, "uz" }, // Uzbekistan
       //       { GameRegions::VN, "vn" }, // Vietnam
       { GameRegions::TW, "tw" }, // Taiwan
       { GameRegions::US, "us" }, // USA
       { GameRegions::ASI, "asi" }, // Asia
     });

  std::string* found = sRegionToRegionName.try_get(region);
  if (found != nullptr)
    return *found;

  static std::string sWorld("wor");
  return sWorld;
}

Regions::GameRegions Regions::DeserializeRegion(const std::string& region)
{
  static HashMap<std::string, GameRegions> sRegionNameToRegion
    ({
//       { "dj", GameRegions::DJ }, // Djibouti
//       { "er", GameRegions::ER }, // Eritrea
//       { "et", GameRegions::ET }, // Ethiopia
//       { "af", GameRegions::AF }, // Afghanistan
//       { "na", GameRegions::NA }, // Namibia
//       { "za", GameRegions::ZA }, // South Africa
//       { "cm", GameRegions::CM }, // Cameroon
//       { "gh", GameRegions::GH }, // Ghana
//       { "am", GameRegions::AM }, // Armenia
//       { "ar", GameRegions::AR }, // Argentina
       { "wor", GameRegions::WOR }, // World
//       { "ae", GameRegions::AE }, // United Arab Emirates
//       { "bh", GameRegions::BH }, // Bahrain
//       { "dz", GameRegions::DZ }, // Algeria
//       { "eg", GameRegions::EG }, // Egypt
       { "il", GameRegions::IL }, // Israel
//       { "iq", GameRegions::IQ }, // Iraq
//       { "jo", GameRegions::JO }, // Jordan
//       { "km", GameRegions::KM }, // Comoros
//       { "kw", GameRegions::KW }, // Kuwait
//       { "lb", GameRegions::LB }, // Lebanon
//       { "ly", GameRegions::LY }, // Libya
//       { "ma", GameRegions::MA }, // Morocco
//       { "mr", GameRegions::MR }, // Mauritania
//       { "om", GameRegions::OM }, // Oman
//       { "ps", GameRegions::PS }, // Palestinian Authority
//       { "qa", GameRegions::QA }, // Qatar
//       { "sa", GameRegions::SA }, // Saudi Arabia
//       { "sd", GameRegions::SD }, // Sudan
//       { "so", GameRegions::SO }, // Somalia
//       { "ss", GameRegions::SS }, // South Sudan
//       { "sy", GameRegions::SY }, // Syria
//       { "td", GameRegions::TD }, // Chad
//       { "tn", GameRegions::TN }, // Tunisia
//       { "ye", GameRegions::YE }, // Yemen
       { "cl", GameRegions::CL }, // Chile
//       { "as", GameRegions::AS }, // American Samoa
//       { "in", GameRegions::IN }, // India
//       { "tz", GameRegions::TZ }, // Tanzania
       { "es", GameRegions::ES }, // Spain
//       { "az", GameRegions::AZ }, // Azerbaijan
//       { "ba", GameRegions::BA }, // Bosnia and Herzegovina
       { "ru", GameRegions::RU }, // Russia
//       { "be", GameRegions::BE }, // Belgium
//       { "by", GameRegions::BY }, // Belarus
//       { "zm", GameRegions::ZM }, // Zambia
       { "bg", GameRegions::BG }, // Bulgaria
//       { "ng", GameRegions::NG }, // Nigeria
//       { "bm", GameRegions::BM }, // Bermuda
//       { "ml", GameRegions::ML }, // Mali
//       { "bn", GameRegions::BN }, // Brunei
//       { "bd", GameRegions::BD }, // Bangladesh
//       { "bo", GameRegions::BO }, // Bolivia
       { "cn", GameRegions::CN }, // China
       { "br", GameRegions::BR }, // Brazil
       { "fr", GameRegions::FR }, // France
//       { "bs", GameRegions::BS }, // Bahamas
       { "ca", GameRegions::CA }, // Canada
//       { "ad", GameRegions::AD }, // Andorra
       { "it", GameRegions::IT }, // Italy
//       { "ug", GameRegions::UG }, // Uganda
//       { "co", GameRegions::CO }, // Colombia
       { "cz", GameRegions::CZ }, // Czechia
//       { "cu", GameRegions::CU }, // Cuba
//       { "cy", GameRegions::CY }, // Cyprus
       { "uk", GameRegions::UK }, // United Kingdom
       { "dk", GameRegions::DK }, // Denmark
//       { "gl", GameRegions::GL }, // Greenland
//       { "ke", GameRegions::KE }, // Kenya
       { "de", GameRegions::DE }, // Germany
//       { "at", GameRegions::AT }, // Austria
//       { "ch", GameRegions::CH }, // Switzerland
//       { "li", GameRegions::LI }, // Liechtenstein
//       { "lu", GameRegions::LU }, // Luxembourg
//       { "ne", GameRegions::NE }, // Niger
//       { "mv", GameRegions::MV }, // Maldives
//       { "sn", GameRegions::SN }, // Senegal
//       { "bt", GameRegions::BT }, // Bhutan
//       { "ee", GameRegions::EE }, // Estonia
//       { "tg", GameRegions::TG }, // Togo
       { "gr", GameRegions::GR }, // Greece
//       { "car", GameRegions::CAR }, // Caribbean
       { "eu", GameRegions::EU }, // Europe
//       { "ag", GameRegions::AG }, // Antigua and Barbuda
//       { "ai", GameRegions::AI }, // Anguilla
       { "au", GameRegions::AU }, // Australia
//       { "bb", GameRegions::BB }, // Barbados
//       { "bi", GameRegions::BI }, // Burundi
//       { "bw", GameRegions::BW }, // Botswana
//       { "bz", GameRegions::BZ }, // Belize
//       { "cc", GameRegions::CC }, // Cocos (Keeling) Islands
//       { "ck", GameRegions::CK }, // Cook Islands
//       { "cx", GameRegions::CX }, // Christmas Island
//       { "dm", GameRegions::DM }, // Dominica
       { "fi", GameRegions::FI }, // Finland
//       { "fj", GameRegions::FJ }, // Fiji
//       { "fk", GameRegions::FK }, // Falkland Islands
//       { "fm", GameRegions::FM }, // Micronesia
//       { "gd", GameRegions::GD }, // Grenada
//       { "gg", GameRegions::GG }, // Guernsey
//       { "gi", GameRegions::GI }, // Gibraltar
//       { "gm", GameRegions::GM }, // Gambia
//       { "gu", GameRegions::GU }, // Guam
//       { "gy", GameRegions::GY }, // Guyana
//       { "hk", GameRegions::HK }, // Hong Kong SAR
//       { "id", GameRegions::ID }, // Indonesia
//       { "ie", GameRegions::IE }, // Ireland
//       { "im", GameRegions::IM }, // Isle of Man
//       { "io", GameRegions::IO }, // British Indian Ocean Territory
//       { "je", GameRegions::JE }, // Jersey
//       { "jm", GameRegions::JM }, // Jamaica
//       { "ki", GameRegions::KI }, // Kiribati
//       { "kn", GameRegions::KN }, // Saint Kitts and Nevis
//       { "ky", GameRegions::KY }, // Cayman Islands
//       { "lc", GameRegions::LC }, // Saint Lucia
//       { "lr", GameRegions::LR }, // Liberia
//       { "ls", GameRegions::LS }, // Lesotho
//       { "mg", GameRegions::MG }, // Madagascar
//       { "mh", GameRegions::MH }, // Marshall Islands
//       { "mo", GameRegions::MO }, // Macao SAR
//       { "mp", GameRegions::MP }, // Northern Mariana Islands
//       { "ms", GameRegions::MS }, // Montserrat
//       { "mt", GameRegions::MT }, // Malta
//       { "mu", GameRegions::MU }, // Mauritius
//       { "mw", GameRegions::MW }, // Malawi
//       { "my", GameRegions::MY }, // Malaysia
//       { "nf", GameRegions::NF }, // Norfolk Island
       { "nl", GameRegions::NL }, // Netherlands
//       { "nr", GameRegions::NR }, // Nauru
//       { "nu", GameRegions::NU }, // Niue
       { "nz", GameRegions::NZ }, // New Zealand
//       { "pg", GameRegions::PG }, // Papua New Guinea
//       { "ph", GameRegions::PH }, // Philippines
//       { "pk", GameRegions::PK }, // Pakistan
//       { "pn", GameRegions::PN }, // Pitcairn Islands
//       { "pr", GameRegions::PR }, // Puerto Rico
//       { "pw", GameRegions::PW }, // Palau
//       { "rw", GameRegions::RW }, // Rwanda
//       { "sb", GameRegions::SB }, // Solomon Islands
//       { "sc", GameRegions::SC }, // Seychelles
       { "se", GameRegions::SE }, // Sweden
//       { "sg", GameRegions::SG }, // Singapore
//       { "sh", GameRegions::SH }, // St Helena, Ascension, Tristan da Cunha
//       { "si", GameRegions::SI }, // Slovenia
//       { "sl", GameRegions::SL }, // Sierra Leone
//       { "sx", GameRegions::SX }, // Sint Maarten
//       { "sz", GameRegions::SZ }, // Swaziland
//       { "tc", GameRegions::TC }, // Turks and Caicos Islands
//       { "tk", GameRegions::TK }, // Tokelau
//       { "to", GameRegions::TO }, // Tonga
//       { "tt", GameRegions::TT }, // Trinidad and Tobago
//       { "tv", GameRegions::TV }, // Tuvalu
//       { "um", GameRegions::UM }, // U.S. Outlying Islands
//       { "vc", GameRegions::VC }, // Saint Vincent and the Grenadines
//       { "vg", GameRegions::VG }, // British Virgin Islands
//       { "vi", GameRegions::VI }, // U.S. Virgin Islands
//       { "vu", GameRegions::VU }, // Vanuatu
//       { "ws", GameRegions::WS }, // Samoa
//       { "zw", GameRegions::ZW }, // Zimbabwe
       { "ame", GameRegions::AME }, // Latin America
//       { "cr", GameRegions::CR }, // Costa Rica
//       { "do", GameRegions::DO }, // Dominican Republic
//       { "ec", GameRegions::EC }, // Ecuador
//       { "gq", GameRegions::GQ }, // Equatorial Guinea
//       { "gt", GameRegions::GT }, // Guatemala
//       { "hn", GameRegions::HN }, // Honduras
//       { "mx", GameRegions::MX }, // Mexico
//       { "ni", GameRegions::NI }, // Nicaragua
//       { "pa", GameRegions::PA }, // Panama
//       { "pe", GameRegions::PE }, // Peru
//       { "py", GameRegions::PY }, // Paraguay
//       { "sv", GameRegions::SV }, // El Salvador
//       { "uy", GameRegions::UY }, // Uruguay
//       { "ve", GameRegions::VE }, // Venezuela
//       { "ir", GameRegions::IR }, // Iran
//       { "gn", GameRegions::GN }, // Guinea
//       { "fo", GameRegions::FO }, // Faroe Islands
//       { "bf", GameRegions::BF }, // Burkina Faso
//       { "bj", GameRegions::BJ }, // Benin
//       { "bl", GameRegions::BL }, // Saint Barthélemy
//       { "cd", GameRegions::CD }, // Congo (DRC)
//       { "cf", GameRegions::CF }, // Central African Republic
//       { "cg", GameRegions::CG }, // Congo
//       { "ci", GameRegions::CI }, // Côte d’Ivoire
//       { "ga", GameRegions::GA }, // Gabon
//       { "gf", GameRegions::GF }, // French Guiana
//       { "gp", GameRegions::GP }, // Guadeloupe
//       { "ht", GameRegions::HT }, // Haiti
//       { "mc", GameRegions::MC }, // Monaco
//       { "mf", GameRegions::MF }, // Saint Martin
//       { "mq", GameRegions::MQ }, // Martinique
//       { "nc", GameRegions::NC }, // New Caledonia
//       { "pf", GameRegions::PF }, // French Polynesia
//       { "pm", GameRegions::PM }, // Saint Pierre and Miquelon
//       { "re", GameRegions::RE }, // Réunion
//       { "wf", GameRegions::WF }, // Wallis and Futuna
//       { "yt", GameRegions::YT }, // Mayotte
//       { "hr", GameRegions::HR }, // Croatia
       { "hu", GameRegions::HU }, // Hungary
//       { "is", GameRegions::IS }, // Iceland
//       { "sm", GameRegions::SM }, // San Marino
//       { "va", GameRegions::VA }, // Vatican City
       { "jp", GameRegions::JP }, // Japan
//       { "ge", GameRegions::GE }, // Georgia
//       { "cv", GameRegions::CV }, // Cabo Verde
//       { "kz", GameRegions::KZ }, // Kazakhstan
//       { "kh", GameRegions::KH }, // Cambodia
//       { "kp", GameRegions::KP }, // North Korea
       { "kr", GameRegions::KR }, // Korea
//       { "kg", GameRegions::KG }, // Kyrgyzstan
//       { "la", GameRegions::LA }, // Laos
//       { "ao", GameRegions::AO }, // Angola
//       { "lt", GameRegions::LT }, // Lithuania
//       { "lv", GameRegions::LV }, // Latvia
//       { "mz", GameRegions::MZ }, // Mozambique
//       { "mk", GameRegions::MK }, // Macedonia, FYRO
//       { "mn", GameRegions::MN }, // Mongolia
//       { "mm", GameRegions::MM }, // Myanmar
       { "no", GameRegions::NO }, // Norway
//       { "sj", GameRegions::SJ }, // Svalbard and Jan Mayen
//       { "np", GameRegions::NP }, // Nepal
//       { "aw", GameRegions::AW }, // Aruba
//       { "bq", GameRegions::BQ }, // Bonaire, Sint Eustatius and Saba
//       { "cw", GameRegions::CW }, // Curaçao
//       { "sr", GameRegions::SR }, // Suriname
       { "pl", GameRegions::PL }, // Poland
       { "pt", GameRegions::PT }, // Portugal
//       { "gw", GameRegions::GW }, // Guinea-Bissau
//       { "st", GameRegions::ST }, // São Tomé and Príncipe
//       { "tl", GameRegions::TL }, // Timor-Leste
//       { "ro", GameRegions::RO }, // Romania
//       { "md", GameRegions::MD }, // Moldova
//       { "ua", GameRegions::UA }, // Ukraine
//       { "lk", GameRegions::LK }, // Sri Lanka
       { "sk", GameRegions::SK }, // Slovakia
//       { "al", GameRegions::AL }, // Albania
//       { "xk", GameRegions::XK }, // Kosovo
//       { "me", GameRegions::ME }, // Montenegro
//       { "rs", GameRegions::RS }, // Serbia
//       { "ax", GameRegions::AX }, // Åland Islands
//       { "tj", GameRegions::TJ }, // Tajikistan
//       { "th", GameRegions::TH }, // Thailand
//       { "tm", GameRegions::TM }, // Turkmenistan
       { "tr", GameRegions::TR }, // Turkey
//       { "uz", GameRegions::UZ }, // Uzbekistan
//       { "vn", GameRegions::VN }, // Vietnam
//       { "tw", GameRegions::TW }, // Taiwan
       { "us", GameRegions::US }, // usa
       { "asi", GameRegions::ASI }, // Asia

       // non iso existing regions
       { "sp", Regions::GameRegions::ES }, // ScreenScraper spanish region
       { "j", Regions::GameRegions::JP },
       { "f", Regions::GameRegions::FR },
       { "e", Regions::GameRegions::EU },
       { "u", Regions::GameRegions::US },
     });

  GameRegions* found = sRegionNameToRegion.try_get(region);
  if (found != nullptr)
    return *found;

  return GameRegions::Unknown;
}

unsigned int Regions::Deserialize4Regions(const std::string& regions)
{
  unsigned int result = 0;
  for(int start = 0; start < (int)regions.size(); )
  {
    int pos = regions.find(',', start + 1);
    if (pos == (int)std::string::npos)
      pos = (int)regions.size();
    std::string subRegion = Strings::Replace(regions.substr(start, pos - start), ",", "");
    start = pos;
    GameRegions region = FullNameToRegions(subRegion);
    if (region != GameRegions::Unknown)
    {
      result = (result << 8) | (unsigned int) region;
      continue;
    }
    region = DeserializeRegion(subRegion);
    if (region != GameRegions::Unknown)
    {
      result = (result << 8) | (unsigned int) region;
      continue;
    }
  }
  return result;
}

std::string Regions::Serialize4Regions(unsigned int regions)
{
  std::string result;
  for(int i = 4; --i >= 0;)
  {
    GameRegions region = (GameRegions)(regions & 0xFF);
    if (region != GameRegions::Unknown)
    {
      if (!result.empty()) result.append(1, ',');
      result.append(SerializeRegion(region));
    }
    regions >>= 8;
  }
  return result;
}

Regions::GameRegions Regions::ExtractRegionsFromFileName(const Path& path)
{
  const std::string& pathString = path.ToString();
  for(int start = 0;;)
  {
    // Lookup (...)
    start = pathString.find('(', start);
    if (start == (int)std::string::npos) break;
    int stop = pathString.find(')', start);
    if (stop == (int)std::string::npos) break;

    // Match long/short name?
    std::string region = Strings::ToLowerASCII(pathString.substr(start + 1, stop - (start + 1)));
    GameRegions result = FullNameToRegions(region);
    if (result != GameRegions::Unknown) return result;
    result = DeserializeRegion(region);
    if (result != GameRegions::Unknown) return result;

    // Try next
    start = stop + 1;
  }
  return GameRegions::Unknown;
}

const Regions::List& Regions::AvailableRegions()
{
  //! Sorted regions, from most to least important
  static List sRegions
  ({
     GameRegions::Unknown, // No region
     GameRegions::WOR, // World
     GameRegions::US, // USA
     GameRegions::EU,  // Europe
     GameRegions::AME, // Latin America
     GameRegions::ASI, // Asia
     GameRegions::AU,  // Australia
     GameRegions::BR,  // Brazil
     GameRegions::BG,  // Bulgaria
     GameRegions::CA,  // Canada
     GameRegions::CL,  // Chile
     GameRegions::CN,  // China
     GameRegions::CZ,  // Czechia
     GameRegions::DK,  // Denmark
     GameRegions::FI,  // Finland
     GameRegions::FR,  // France
     GameRegions::DE,  // Germany
     GameRegions::GR,  // Greece
     GameRegions::HU,  // Hungary
     GameRegions::IL,  // Israel
     GameRegions::KR,  // Korea
     GameRegions::IT,  // Italy
     GameRegions::JP,  // Japan
     GameRegions::NL,  // Netherlands
     GameRegions::NZ,  // New Zealand
     GameRegions::NO,  // Norway
     GameRegions::PL,  // Poland
     GameRegions::PT,  // Portugal
     GameRegions::RU,  // Russia
     GameRegions::ES,  // Spain
     GameRegions::SE,  // Sweden
     GameRegions::UK,  // United Kingdom
  });

  return sRegions;
}

bool Regions::IsIn4Regions(unsigned int regions, Regions::GameRegions region)
{
  if ((GameRegions)((regions >>  0) & 0xFF) == region) return true;
  if ((GameRegions)((regions >>  8) & 0xFF) == region) return true;
  if ((GameRegions)((regions >> 16) & 0xFF) == region) return true;
  return (GameRegions) ((regions >> 24) & 0xFF) == region;
}

Regions::GameRegions Regions::GameRegionsFromString(const std::string& gameRegion)
{
  return DeserializeRegion(gameRegion);
}

const std::string& Regions::GameRegionsFromEnum(Regions::GameRegions gameRegions)
{
  return SerializeRegion(gameRegions);
}

//https://datomatic.no-intro.org/stuff/The%20Official%20No-Intro%20Convention%20(20071030).pdf
// [BIOS flag] Title (Region) (Languages) (Version) (Devstatus) (Additional)(Special) (License) [Status]
// Region is mandatory
unsigned int Regions::ExtractRegionsFromNoIntroName(const Path& path)
{
  std::string fileName = Strings::ToLowerASCII(path.FilenameWithoutExtension());
  const size_t strBegin = fileName.find_first_of('(') + 1;const size_t strEnd   = fileName.find_first_of(')');
  std::string regions = fileName.substr(strBegin , strEnd - strBegin);
  regions = Strings::Replace(regions, " ", "");

  return Deserialize4Regions(regions);
}

// https://www.tosecdev.org/tosec-naming-convention
// Legend of TOSEC, The (1986)(Devstudio)(JP)(en) - set released in Japan and in English.
unsigned int Regions::ExtractRegionsFromTosecName(const Path& path)
{
  unsigned int result = 0;
  std::string fileName = Strings::ToLowerASCII(path.FilenameWithoutExtension());

  for(;;)
  {
    const size_t strBegin = fileName.find_first_of('(') + 1;
    const size_t strEnd = fileName.find_first_of(')');
    std::string regions = fileName.substr(strBegin, strEnd - strBegin);
    regions = Strings::Replace(regions, " ", "");

    for (int start = 0; start < (int) regions.size();)
    {
      int pos = regions.find('-', start + 1);
      // if splitter not found -> only one loop
      if (pos == (int) std::string::npos)
        pos = (int) regions.size();

      std::string subRegion = Strings::Replace(regions.substr(start, pos - start), "-", "");
      start = pos;
      GameRegions region = FullNameToRegions(subRegion);
      if (region != GameRegions::Unknown)
      {
        result = (result << 8) | (unsigned int) region;
        continue;
      }
      region = DeserializeRegion(subRegion);
      if (region != GameRegions::Unknown)
      {
        result = (result << 8) | (unsigned int) region;
        continue;
      }
    }

    if(result != 0 || strEnd == fileName.size() -1)
      break;
    
    fileName = fileName.substr(strEnd + 1, fileName.size() -1);
  }
  return result;
}

unsigned int Regions::ExtractRegionsFromName(const std::string& string)
{
  const size_t strBegin = string.find_first_of('[') + 1;const size_t strEnd   = string.find_first_of(']');
  std::string regions = string.substr(strBegin , strEnd - strBegin);
  regions = Strings::Replace(regions, " ", "");

  return Deserialize4Regions(regions);
}