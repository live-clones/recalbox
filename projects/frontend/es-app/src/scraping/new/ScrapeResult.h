//
// Created by bkg2k on 04/12/2019.
//
#pragma once

//! Result type
enum class ScrapeResult
{
    Ok,           //!< Game found
    NotFound,     //!< Game not found
    QuotaReached, //!< Quota reached - Stop scrapping!
    DiskFull,     //!< Disk limit reached - Stop scrapping!
    FatalError,   //!< Fatal error - Stop scrapping!
};