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

#include "AsyncQueueWorker.h"
#include "AsyncWatcherOperation.h"
#include "ProducerConsumerQueue.h"
#include "Log.h"

AsyncQueueWorker::AsyncQueueWorker(ProducerConsumerQueue<AsyncWatcherOperation*>* queue)
{
    _queue = queue;
    _thread = std::thread(&AsyncQueueWorker::RunThread, this);
}

AsyncQueueWorker::~AsyncQueueWorker()
{
    _cancel = true;
    _queue->Cancel();

    if (_thread.joinable())
        _thread.join();
}

void AsyncQueueWorker::RunThread()
{
    if (!_queue)
        return;

    for (;;)
    {
        AsyncWatcherOperation* operation{ nullptr };

        _queue->WaitAndPop(operation);

        if (!operation || _cancel)
            break;

        operation->Execute();
        delete operation;
    }

    LOG_INFO("watcher", "Stop queue thread");
}
