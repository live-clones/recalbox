//
// Created by bkg2k on 04/12/2019.
//
#pragma once

//! Result type
enum class ScrapeResult
{
    Ok,           //!< Game found
    NotScraped,   //!< Game does not need to be scraped
    NotFound,     //!< Game not found
    QuotaReached, //!< Quota reached - Stop scraping!
    DiskFull,     //!< Disk limit reached - Stop scraping!
    FatalError,   //!< Fatal error - Stop scraping!
};