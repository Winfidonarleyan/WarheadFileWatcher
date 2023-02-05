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

#ifndef WARHEAD_FILE_WATCHER_H_
#define WARHEAD_FILE_WATCHER_H_

#include "Define.h"
#include <atomic>
#include <memory>

enum ShutdownExitCode : uint8
{
    SHUTDOWN_EXIT_CODE,
    ERROR_EXIT_CODE
};

template <typename T>
class ProducerConsumerQueue;

class AsyncQueueWorker;
class AsyncWatcherOperation;
class WatcherWorker;

class WH_WATCHER_API Watcher
{
    Watcher();
    ~Watcher();

public:
    static Watcher* instance();

    void Start();
    void Stop();

    [[nodiscard]] static bool IsStopped() { return _cancel; }
    static uint8 GetExitCode() { return _exitCode; }
    static void StopNow(uint8 exitcode) { _cancel = true; _exitCode = exitcode; }

    void CheckFile(std::string_view filePath);

    [[nodiscard]] inline std::string_view GetWatchDir() const { return _dirWatch; }
    [[nodiscard]] inline std::string_view GetSecretDir() const { return _dirToCopySecrets; }
    [[nodiscard]] inline std::string_view GetFindWord() const { return _findWord; }
    [[nodiscard]] inline std::size_t GetMaxFileSize() const { return _maxFileSize; }

private:
    std::unique_ptr<ProducerConsumerQueue<AsyncWatcherOperation*>> _queue;
    std::unique_ptr<AsyncQueueWorker> _queueWorker;
    std::unique_ptr<WatcherWorker> _watcherWorker;
    std::size_t _maxFileSize{};
    std::string _dirWatch;
    std::string _dirToCopySecrets;
    std::string _findWord;

    static std::atomic<bool> _cancel;
    static uint8 _exitCode;
};

#define sWatcher Watcher::instance()

#endif
