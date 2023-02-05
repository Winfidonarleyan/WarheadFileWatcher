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

#ifndef ASYNC_WATCHER_OPERATION_H_
#define ASYNC_WATCHER_OPERATION_H_

#include "Define.h"
#include <string>
#include <string_view>

class WH_WATCHER_API AsyncWatcherOperation
{
public:
    explicit AsyncWatcherOperation(std::string_view filePath, std::size_t maxFileSize) :
        _filePath(filePath), _maxFileSize(maxFileSize) { }

    virtual ~AsyncWatcherOperation() = default;

    void Execute();

private:
    std::string _filePath;
    std::size_t _maxFileSize{};

    AsyncWatcherOperation(AsyncWatcherOperation const& right) = delete;
    AsyncWatcherOperation& operator=(AsyncWatcherOperation const& right) = delete;
};

#endif
