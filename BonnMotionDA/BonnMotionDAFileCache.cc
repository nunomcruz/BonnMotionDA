//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include <fstream>
#include <sstream>

#include "BonnMotionDAFileCache.h"

const BonnMotionDAFile::Line *BonnMotionDAFile::getLine(int nodeId) const
{
    LineList::const_iterator it = lines.begin();
    for (int i = 0; i < nodeId && it != lines.end(); i++)
        it++;
    return (it == lines.end()) ? NULL : &(*it);
}

BonnMotionDAFileCache *BonnMotionDAFileCache::inst;

BonnMotionDAFileCache *BonnMotionDAFileCache::getInstance()
{
    if (!inst)
        inst = new BonnMotionDAFileCache;
    return inst;
}

void BonnMotionDAFileCache::deleteInstance()
{
    if (inst) {
        delete inst;
        inst = NULL;
    }
}

const BonnMotionDAFile *BonnMotionDAFileCache::getFile(const char *filename)
{
    // if found, return it from cache
    BMFileMap::iterator it = cache.find(std::string(filename));
    if (it != cache.end())
        return &(it->second);

    // load and store in cache
    BonnMotionDAFile& bmFile = cache[filename];
    parseFile(filename, bmFile);
    return &bmFile;
}

void BonnMotionDAFileCache::parseFile(const char *filename, BonnMotionDAFile& bmFile)
{
    std::ifstream in(filename, std::ios::in);
    if (in.fail())
        throw cRuntimeError("Cannot open file '%s'", filename);

    std::string line;
    int i = 0;
    while (std::getline(in, line)) {
        std::stringstream linestream(line);
        double d;
        switch(i%4) {
            case 0:
                linestream >> d;
                if(d==0.0) {
                    bmFile.lines.push_back(BonnMotionDAFile::Line());
                }
                bmFile.lines.back().push_back(d);
                break;
            case 1:
                linestream >> d;
                bmFile.lines.back().push_back(d);
                break;
            case 2:
                linestream >> d;
                bmFile.lines.back().push_back(d);
                break;
            case 3:
                linestream >> d;
                bmFile.lines.back().push_back(d);
                break;
            default:
                break;
        }
        i++;
    }
    in.close();
}
