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

#include "AsyncWatcherOperation.h"
#include "Log.h"
#include "StopWatch.h"
#include "Watcher.h"
#include "FileUtil.h"
#include <filesystem>

namespace fs = std::filesystem;

void AsyncWatcherOperation::Execute()
{
    StopWatch sw;

    try
    {
        fs::path path{ sWatcher->GetWatchDir() };
        path /= _filePath;

        if (!fs::exists(path))
        {
            LOG_ERROR("watcher.finder", "Not found file: {}", path.generic_string());
            return;
        }

        // Check empty file
        if (fs::is_empty(path))
            return;

        auto fileName{ path.filename().generic_string() };
        auto fileSize{ fs::file_size(path) };
        auto maxFileSize{ sWatcher->GetMaxFileSize() };
        auto findWord{ sWatcher->GetFindWord() };

        // Check file size
        if (maxFileSize && fileSize > maxFileSize)
        {
            LOG_DEBUG("watcher.finder", "Skip check file: '{}'. File size ({}) is larger than maximum ({})", fileName, fileSize, maxFileSize);
            return;
        }

        if (auto foundInLine = Warhead::File::FindWord(path.generic_string(), findWord))
        {
            LOG_WARN("watcher.finder", "Found secret '{}' in file '{}' and line {}", findWord, path.generic_string(), foundInLine);
            auto copyPath{ fs::path(sWatcher->GetSecretDir()) };
            copyPath /= fileName;

            if (Warhead::File::CopyFile(path.generic_string(), copyPath.generic_string()))
                LOG_DEBUG("watcher.finder", "File '{}' copied to '{}'", fileName, copyPath.generic_string());
        }
    }
    catch (std::exception const& exception)
    {
        LOG_ERROR("watcher.finder", "Error at check file '{}'. Message: {}", _filePath, exception.what());
    }

    LOG_DEBUG("watcher.finder", "End operation in {}", sw);
}
