/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"
#include "Timer.h"
#include "Errors.h"
#include "Watcher.h"
#include "Log.h"
#include <csignal>
#include <filesystem>
#include <thread>

#ifndef _WARHEAD_WATCHER_CONFIG
#define _WARHEAD_WATCHER_CONFIG "WarheadFileWatcher.conf"
#endif

namespace fs = std::filesystem;

void TerminateHandler(int sigval);
void WatcherUpdateLoop();

/// Launch the server
int main()
{
    signal(SIGTERM, &TerminateHandler);
    signal(SIGABRT, &Warhead::AbortHandler);

    auto configFile = fs::path(sConfigMgr->GetConfigPath() + std::string(_WARHEAD_WATCHER_CONFIG));

    // Add file and args in config
    sConfigMgr->Configure(configFile.generic_string());
    if (!sConfigMgr->LoadAppConfigs())
        return 1;

    // Init logging
    sLog->Initialize();

    LOG_INFO("watcher", "> Using configuration file:       {}", sConfigMgr->GetFilename());
    LOG_INFO("watcher", "> Using logs directory:           {}", sLog->GetLogsDir());
    LOG_INFO("watcher", "");

    sWatcher->Start();

    WatcherUpdateLoop();

    LOG_INFO("server", "Halting process...");

    // 0 - normal shutdown
    // 1 - shutdown at error
    return Watcher::GetExitCode();
}

void WatcherUpdateLoop()
{
    auto realCurrTime = 0ms;
    auto realPrevTime = GetTimeMS();

    while (!Watcher::IsStopped())
    {
        realCurrTime = GetTimeMS();

        auto diff = GetMSTimeDiff(realPrevTime, realCurrTime);
        if (diff == 0ms)
        {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        realPrevTime = realCurrTime;
    }

    LOG_INFO("watcher", "Stop update loop");
}

void TerminateHandler(int sigval)
{
    LOG_WARN("watcher", "Caught signal: {}. Stop process", sigval);
    sWatcher->StopNow(SHUTDOWN_EXIT_CODE);
}
