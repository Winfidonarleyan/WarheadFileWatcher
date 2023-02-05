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

#include "catch2/catch.hpp"
#include "FileUtil.h"
#include <filesystem>
#include <fstream>

constexpr auto SECRETS_DIR = "secrets";
constexpr auto DEFAULT_FILE_NAME = "defaultfile";
constexpr auto SECRET_FILE_NAME = "secretsfile";
constexpr auto SECRET_WORD = "TOPSECRET";

namespace fs = std::filesystem;

TEST_CASE("Create files and dirs")
{
    SECTION("Create secrets dir")
    {
        REQUIRE(Warhead::File::CreateDirIfNeed(SECRETS_DIR));
    }

    SECTION("Create default file")
    {
        std::ofstream file(DEFAULT_FILE_NAME);
        REQUIRE(file.is_open());
        file << "Test text" << std::endl;
        file.close();
        REQUIRE(!file.is_open());
    }

    SECTION("Create secret file")
    {
        std::ofstream file(SECRET_FILE_NAME);
        REQUIRE(file.is_open());

        file << "----" << std::endl;
        file << SECRET_WORD << std::endl;
        file << "----" << std::endl;
        file.close();

        REQUIRE(!file.is_open());
    }
}

TEST_CASE("Check files")
{
    SECTION("Check empty file")
    {
        REQUIRE(fs::exists(DEFAULT_FILE_NAME));
        REQUIRE(!Warhead::File::FindWord(DEFAULT_FILE_NAME, SECRET_WORD));
    }

    SECTION("Check secret file")
    {
        REQUIRE(fs::exists(SECRET_FILE_NAME));

        auto line{ Warhead::File::FindWord(SECRET_FILE_NAME, SECRET_WORD) };
        REQUIRE(line);
    }

    SECTION("Copy secret file to secret dir")
    {
        auto copied{ Warhead::File::CopyFile(SECRET_FILE_NAME, std::string{ SECRETS_DIR } + "/" + std::string{ SECRET_FILE_NAME }) };
        REQUIRE(copied);
    }
}

TEST_CASE("Clean")
{
    SECTION("Delete empty file")
    {
        REQUIRE(fs::remove(DEFAULT_FILE_NAME));
    }

    SECTION("Delete secret file")
    {
        REQUIRE(fs::remove(SECRET_FILE_NAME));
    }

    SECTION("Delete secrets dir")
    {
        REQUIRE(fs::remove_all(SECRETS_DIR));
    }
}
