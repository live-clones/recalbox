//
// Created by Supernature2k on 07/02/2022.
//

#include "Pi400PowerEventReader.h"
#include <utils/Log.h>
#include <linux/input.h>
#include <fcntl.h>
#include <poll.h>
#include <utils/datetime/DateTime.h>
#include <MainRunner.h>

Pi400PowerEventReader::Pi400PowerEventReader(HardwareMessageSender& messageSender)
        : mSender(messageSender)
        , mFileHandle(0)
        , mWaitFor(WaitFor::Press)
{
}

Pi400PowerEventReader::~Pi400PowerEventReader()
{
    StopReader();
}

void Pi400PowerEventReader::StartReader()
{
    { LOG(LogDebug) << "[Pi400] Power button manager requested to start."; }
    Start("Pi400Power");
}

void Pi400PowerEventReader::StopReader()
{
    { LOG(LogDebug) << "[Pi400] Power button manager requested to stop."; }
    Stop();
}

void Pi400PowerEventReader::Break()
{
    if (mFileHandle >= 0)
    {
        { LOG(LogDebug) << "[Pi400] Breaking power button thread."; }
        mFileHandle = -1;
    }
}

void Pi400PowerEventReader::Run()
{
    { LOG(LogInfo) << "[Pi400] Running background power button manager."; }
    while(IsRunning())
    {
        mFileHandle = open(sInputEventPath, O_RDONLY);
        if (mFileHandle < 0)
        {
            { LOG(LogError) << "[Pi400] Error opening " << sInputEventPath << ". Retry in 5s..."; }
            sleep(5);
            continue;
        }

        input_event pressEvent {};
        while(IsRunning())
        {
            // Poll
            struct pollfd poller { .fd = mFileHandle, .events = POLLIN, .revents = 0 };
            if (poll(&poller, 1, 100) != 1 || (poller.revents & POLLIN) == 0)
            {
                // If the button is pressed at least 2000ms
                // Just send the message and ignore release event.
                if (mWaitFor == WaitFor::Release)
                {
                    timeval now {};
                    gettimeofday(&now, nullptr);
                    long start = (pressEvent.time.tv_usec / 1000) + (pressEvent.time.tv_sec * 1000);
                    long stop = (now.tv_usec / 1000) + (now.tv_sec * 1000);
                    long elapsed = stop - start;
                    if (elapsed >= 2000)
                    {
                        mSender.Send(BoardType::Pi400, MessageTypes::PowerButtonPressed, (int)elapsed);
                        mWaitFor = WaitFor::Ignore; // Ignore releasing the button
                    }
                }
                continue;
            }

            // Read event
            struct input_event event {};
            if (read(mFileHandle, &event, sizeof(event)) != sizeof(event))
            {
                close(mFileHandle);
                // Error while the file handle is ok means a true read error
                if (mFileHandle >= 0)
                {
                    { LOG(LogError) << "[Pi400] Error reading " << sInputEventPath << ". Retrying"; }
                    continue;
                }
                // If file handle NOK, we're instructed to quit
                { LOG(LogInfo) << "[Pi400] Power event reader ordered to stop."; }
                break;
            }

            // Power button pressed?
            if ((event.type == 1) && (event.code == sPowerKeyCode))
                switch(mWaitFor)
                {
                    case WaitFor::Press:
                    {
                        if (event.value == 1) // Really pressed?
                        {
                            mWaitFor = WaitFor::Release;
                            pressEvent = event;
                        }
                        break;
                    }
                    case WaitFor::Release:
                    {
                        if (event.value == 0) // Really released
                        {
                            long start = (pressEvent.time.tv_usec / 1000) + (pressEvent.time.tv_sec * 1000);
                            long stop = (event.time.tv_usec / 1000) + (event.time.tv_sec * 1000);
                            long elapsed = stop - start;
                            if (elapsed > 20) // Debouncing
                                mSender.Send(BoardType::Pi400, MessageTypes::PowerButtonPressed, (int)elapsed);
                        }
                        break;
                    }
                    case WaitFor::Ignore: mWaitFor = WaitFor::Press; break;
                }
        }
    }
}
