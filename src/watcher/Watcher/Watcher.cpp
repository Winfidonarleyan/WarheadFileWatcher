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

#include "Watcher.h"
#include "Config.h"
#include "Errors.h"
#include "FileUtil.h"
#include "Log.h"
#include "AsyncWatcherOperation.h"
#include "AsyncQueueWorker.h"
#include "WatcherWorker.h"
#include "ProducerConsumerQueue.h"

WH_WATCHER_API std::atomic<bool> Watcher::_cancel{ false };
WH_WATCHER_API uint8 Watcher::_exitCode = SHUTDOWN_EXIT_CODE;

Watcher::Watcher()
{
    _queue = std::make_unique<ProducerConsumerQueue<AsyncWatcherOperation*>>();
    _queueWorker = std::make_unique<AsyncQueueWorker>(_queue.get());
}

Watcher::~Watcher()
{
    Stop();
}

/*static*/ Watcher* Watcher::instance()
{
    static Watcher instance;
    return &instance;
}

void Watcher::Start()
{
    _dirWatch = sConfigMgr->GetOption<std::string>("Watcher.WatchDir", "");
    _maxFileSize = sConfigMgr->GetOption<uint64>("Watcher.Files.MaxSize", 0);
    _findWord = sConfigMgr->GetOption<std::string>("Watcher.FindWord", "TOPSECRETS");
    _dirToCopySecrets = sConfigMgr->GetOption<std::string>("Watcher.CopyPath", "secrets");

    Warhead::File::CorrectDirPath(_dirWatch);
    Warhead::File::CorrectDirPath(_dirToCopySecrets, true);
    ASSERT(Warhead::File::CreateDirIfNeed(_dirWatch), "Incorrect watch dir");
    ASSERT(Warhead::File::CreateDirIfNeed(_dirToCopySecrets), "Incorrect dir for copy secret files");
    ASSERT(_dirWatch != sLog->GetLogsDir(), "Don't use logs dir same with watch dir");
    ASSERT(_dirWatch != _dirToCopySecrets, "Don't use watch dir same with dir to copy secrets");
    ASSERT(!_findWord.empty(), "Find word cannot be empty!");

    _watcherWorker = std::make_unique<WatcherWorker>(_dirWatch);

    LOG_INFO("watcher", "Watcher config:");
    LOG_INFO("watcher", "- Watch dir: {}", _dirWatch);
    LOG_INFO("watcher", "- Max file size: {}{}", _maxFileSize, _maxFileSize > 0 ? "" : " - unlimited");
    LOG_INFO("watcher", "- Word find: {}", _findWord);
    LOG_INFO("watcher", "- Dir to copy secrets: {}", _dirToCopySecrets);
    LOG_INFO("watcher", "");
}

void Watcher::Stop()
{
    if (_cancel)
        return;

    _cancel = true;
    _queue->Cancel();
    _queueWorker.reset();
    _watcherWorker.reset();
}

void Watcher::CheckFile(std::string_view filePath)
{
    _queue->Push(new AsyncWatcherOperation(filePath, _maxFileSize));
}
