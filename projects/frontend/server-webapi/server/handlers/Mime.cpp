//
// Created by bkg2k on 03/04/2020.
//

#include "Mime.h"

Pistache::Http::Mime::MediaType Mime::Json(Pistache::Http::Mime::Type::Application, Pistache::Http::Mime::Subtype::Json);
Pistache::Http::Mime::MediaType Mime::BinaryFile(Pistache::Http::Mime::Type::Application, Pistache::Http::Mime::Subtype::OctetStream);
Pistache::Http::Mime::MediaType Mime::FilePdf(Pistache::Http::Mime::Type::Application, Pistache::Http::Mime::Subtype::Pdf);
Pistache::Http::Mime::MediaType Mime::PlainText(Pistache::Http::Mime::Type::Text, Pistache::Http::Mime::Subtype::Plain);
Pistache::Http::Mime::MediaType Mime::Html(Pistache::Http::Mime::Type::Text, Pistache::Http::Mime::Subtype::Html);
Pistache::Http::Mime::MediaType Mime::JavaScript(Pistache::Http::Mime::Type::Text, Pistache::Http::Mime::Subtype::Javascript);
Pistache::Http::Mime::MediaType Mime::Css(Pistache::Http::Mime::Type::Text, Pistache::Http::Mime::Subtype::Css);
Pistache::Http::Mime::MediaType Mime::Icon(Pistache::Http::Mime::Type::Image, Pistache::Http::Mime::Subtype::XIcon);
Pistache::Http::Mime::MediaType Mime::ImagePng(Pistache::Http::Mime::Type::Image, Pistache::Http::Mime::Subtype::Png);
Pistache::Http::Mime::MediaType Mime::ImageJpg(Pistache::Http::Mime::Type::Image, Pistache::Http::Mime::Subtype::Jpeg);
Pistache::Http::Mime::MediaType Mime::ImageGif(Pistache::Http::Mime::Type::Image, Pistache::Http::Mime::Subtype::Gif);
Pistache::Http::Mime::MediaType Mime::ImageSvg(Pistache::Http::Mime::Type::Image, Pistache::Http::Mime::Subtype::Svg);
Pistache::Http::Mime::MediaType Mime::VideoMkv(Pistache::Http::Mime::Type::Video, Pistache::Http::Mime::Subtype::Mkv);
Pistache::Http::Mime::MediaType Mime::VideoMp4(Pistache::Http::Mime::Type::Video, Pistache::Http::Mime::Subtype::Mp4);
Pistache::Http::Mime::MediaType Mime::VideoAvi(Pistache::Http::Mime::Type::Video, Pistache::Http::Mime::Subtype::Avi);
Pistache::Http::Mime::MediaType Mime::Zip(Pistache::Http::Mime::Type::Application, Pistache::Http::Mime::Subtype::OctetStream, Pistache::Http::Mime::Suffix::Zip);
Pistache::Http::Mime::MediaType Mime::FontTtf(Pistache::Http::Mime::Type::Font, Pistache::Http::Mime::Subtype::Ttf);
Pistache::Http::Mime::MediaType Mime::FontWoff(Pistache::Http::Mime::Type::Font, Pistache::Http::Mime::Subtype::Woff);
Pistache::Http::Mime::MediaType Mime::FontWoff2(Pistache::Http::Mime::Type::Font, Pistache::Http::Mime::Subtype::Woff2);

HashMap<std::string, Pistache::Http::Mime::MediaType> Mime::ExtToMIME
({
  { ".html"  , Mime::Html       },
  { ".htm"   , Mime::Html       },
  { ".css"   , Mime::Css        },
  { ".json"  , Mime::Json       },
  { ".ico"   , Mime::Icon       },
  { ".js"    , Mime::JavaScript },
  { ".png"   , Mime::ImagePng   },
  { ".gif"   , Mime::ImageGif   },
  { ".jpg"   , Mime::ImageJpg   },
  { ".jpeg"  , Mime::ImageJpg   },
  { ".svg"   , Mime::ImageSvg   },
  { ".zip"   , Mime::Zip        },
  { ".ttf"   , Mime::FontTtf    },
  { ".woff"  , Mime::FontWoff   },
  { ".woff2" , Mime::FontWoff2  },
  { ".pdf"   , Mime::FilePdf    },
  { ".mkv"   , Mime::VideoMkv   },
  { ".mp4"   , Mime::VideoMp4   },
  { ".avi"   , Mime::VideoAvi   },
});

