//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

enum class ScrapingMethod
{
  All,                        //<! Scrape all
  AllIfNoithingExists,        //<! Scrape all if no metadata at all
  CompleteAndKeepExisting,    //<! Scrape only missing information, keeping information already here
  CompleteAndReplaceExisting, //<! Scrape only missing information, and update information already here
};