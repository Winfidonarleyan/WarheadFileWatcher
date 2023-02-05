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

#include "WatcherWorker.h"
#include "Log.h"
#include "Watcher.h"
#include <sys/inotify.h>

constexpr std::size_t MAX_EVENTS = 1024;
constexpr std::size_t MAX_FILE_LENGTH = 32;
constexpr std::size_t EVENT_SIZE = sizeof(inotify_event);
constexpr std::size_t BUFF_SIZE = MAX_EVENTS * (EVENT_SIZE * MAX_FILE_LENGTH);

WatcherWorker::WatcherWorker(std::string_view dirWatch)
{
    _dirWatch = dirWatch;
    _thread = std::thread(&WatcherWorker::RunThread, this);
}

WatcherWorker::~WatcherWorker()
{
    StopThread();
}

void WatcherWorker::StopThread()
{
    if (_cancel)
        return;

    _cancel = true;

    inotify_rm_watch(_fd, _watchid);
    close(_fd);

    if (_thread.joinable())
        _thread.join();
}

void WatcherWorker::RunThread()
{
    int32 length{};
    int32 i{};
    char buffer[BUFF_SIZE];

    _fd = inotify_init();
    if (_fd < 0)
    {
        LOG_FATAL("watcher.worker", "Can't initialize watcher!");
        return;
    }

    _watchid = inotify_add_watch(_fd, _dirWatch.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_TO | IN_CLOSE_WRITE);

    LOG_INFO("watcher.worker", "Start watch dir: {}. Watch id: {}", _dirWatch, _watchid);

    for (;;)
    {
        length = read(_fd, buffer, BUFF_SIZE );
        if (length < 0)
            break;

        std::string eventObject;

        while (i < length)
        {
            auto event = (inotify_event*)&buffer[i];
            if (event->len && !(event->mask & IN_ISDIR))
            {
                if (event->mask & IN_CREATE)
                    LOG_DEBUG("watcher.worker", "The file {} was created", event->name);

                if (event->mask & IN_DELETE)
                    LOG_DEBUG("watcher.worker", "The file {} was deleted", event->name);

                if (event->mask & IN_MODIFY)
                    LOG_DEBUG("watcher.worker", "The file {} was modified", event->name);

                if (event->mask & IN_MOVED_TO)
                    LOG_DEBUG("watcher.worker", "The file {} was moved to this directory", event->name);

                if (event->mask & IN_CLOSE_WRITE)
                    LOG_DEBUG("watcher.worker", "The file {} was close", event->name);

                if (!(event->mask & IN_DELETE) && event->mask & IN_CLOSE_WRITE)
                {
                    sWatcher->CheckFile(event->name);
                }
            }

            i += BUFF_SIZE + event->len;
        }

        i = 0;
    }

    LOG_INFO("watcher.worker", "Stop watcher thread");
}
