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

#ifndef WARHEAD_WORKER_H_
#define WARHEAD_WORKER_H_

#include "Define.h"
#include <atomic>
#include <string_view>
#include <thread>

class WH_WATCHER_API WatcherWorker
{
public:
    explicit WatcherWorker(std::string_view dirWatch);
    ~WatcherWorker();

    void StopThread();
    inline std::string_view GetWatchDir() { return _dirWatch; }

private:
    void RunThread();

    int _watchid{};
    int _fd{};
    std::string _dirWatch;
    std::thread _thread;
    std::atomic<bool> _cancel{ false };

    WatcherWorker(WatcherWorker const& right) = delete;
    WatcherWorker& operator=(WatcherWorker const& right) = delete;
};

#endif
