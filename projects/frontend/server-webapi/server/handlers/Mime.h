//
// Created by bkg2k on 03/04/2020.
//
#pragma once

#include <pistache/mime.h>
#include <utils/storage/HashMap.h>

class Mime
{
  public:
    //! JSON MIME type
    static Pistache::Http::Mime::MediaType Json;
    //! Binary MIME type
    static Pistache::Http::Mime::MediaType BinaryFile;
    //! Plain text MIME type
    static Pistache::Http::Mime::MediaType PlainText;
    //! Html MIME Type
    static Pistache::Http::Mime::MediaType Html;
    //! css MIME Type
    static Pistache::Http::Mime::MediaType Css;
    //! JavaScript MIME Type
    static Pistache::Http::Mime::MediaType JavaScript;
    //! Icon MIME Type
    static Pistache::Http::Mime::MediaType Icon;
    //! Png MIME Type
    static Pistache::Http::Mime::MediaType ImagePng;
    //! Jpeg MIME Type
    static Pistache::Http::Mime::MediaType ImageJpg;
    //! Gif MIME Type
    static Pistache::Http::Mime::MediaType ImageGif;
    //! SVG MIME Type
    static Pistache::Http::Mime::MediaType ImageSvg;
    //! MKV MIME Type
    static Pistache::Http::Mime::MediaType VideoMkv;
    //! MP4 MIME Type
    static Pistache::Http::Mime::MediaType VideoMp4;
    //! AVI MIME Type
    static Pistache::Http::Mime::MediaType VideoAvi;
    //! PDF MIME Type
    static Pistache::Http::Mime::MediaType FilePdf;
    //! ttf MIME Type
    static Pistache::Http::Mime::MediaType FontTtf;
    //! zip MIME Type
    static Pistache::Http::Mime::MediaType Zip;
    //! woff MIME Type
    static Pistache::Http::Mime::MediaType FontWoff;
    //! woff2 MIME Type
    static Pistache::Http::Mime::MediaType FontWoff2;

    //! Extension to MIME Type
    static HashMap<std::string, Pistache::Http::Mime::MediaType> ExtToMIME;
};