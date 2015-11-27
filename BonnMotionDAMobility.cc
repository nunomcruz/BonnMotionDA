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

#include "BonnMotionDAMobility.h"
#include "BonnMotionDAFileCache.h"
#include "FWMath.h"


Define_Module(BonnMotionDAMobility);

simsignal_t BonnMotionDAMobility::mobilityNodeRadioOn = cComponent::registerSignal("mobilityNodeRadioOn");
simsignal_t BonnMotionDAMobility::mobilityNodeRadioOff = cComponent::registerSignal("mobilityNodeRadioOff");

BonnMotionDAMobility::BonnMotionDAMobility()
{
    is3D = false;
    lines = NULL;
    currentLine = -1;
    maxSpeed = 0;
}

void BonnMotionDAMobility::computeMaxSpeed()
{
    const BonnMotionDAFile::Line& vec = *lines;
    double lastTime = vec[0];
    Coord lastPos(vec[1],vec[2],(is3D ? vec[3] : 0));
    unsigned int step = (is3D ? 5: 4);
    for (unsigned int i = step; i < vec.size(); i += step)
    {
        double elapsedTime = vec[i] - lastTime;
        Coord currPos(vec[i+1], vec[i+2], (is3D ? vec[i+3] : 0));
        double distance = currPos.distance(lastPos);
        double speed = distance / elapsedTime;
        if (speed > maxSpeed)
            maxSpeed = speed;
        lastPos.x = currPos.x;
        lastPos.y = currPos.y;
        lastPos.z = currPos.z;
        lastTime = vec[i];
    }
}

BonnMotionDAMobility::~BonnMotionDAMobility()
{
    BonnMotionDAFileCache::deleteInstance();
}

void BonnMotionDAMobility::initialize(int stage)
{
    LineSegmentsMobilityBase::initialize(stage);

    EV_TRACE << "initializing BonnMotionDAMobility stage " << stage << endl;
    if (stage == 0) {
        is3D = par("is3D").boolValue();
        int nodeId = par("nodeId");
        if (nodeId == -1)
            nodeId = getContainingNode(this)->getIndex();
        const char *fname = par("traceFile");
        const BonnMotionDAFile *bmFile = BonnMotionDAFileCache::getInstance()->getFile(fname);
        lines = bmFile->getLine(nodeId);
        if (!lines)
            throw cRuntimeError("Invalid nodeId %d -- no such line in file '%s'", nodeId, fname);
        currentLine = 0;
        computeMaxSpeed();
    }
}

void BonnMotionDAMobility::setInitialPosition()
{
    const BonnMotionDAFile::Line& vec = *lines;
    if (lines->size() >= 3) {
        lastPosition.x = vec[1];
        lastPosition.y = vec[2];
    }
    status=is3D ? vec[4] : vec[3];
    if(status==2.0)
        emit(mobilityNodeRadioOff, this);
    else
        emit(mobilityNodeRadioOn, this);
}

void BonnMotionDAMobility::setTargetPosition()
{
    const BonnMotionDAFile::Line& vec = *lines;
    if (currentLine + (is3D ? 3 : 2) >= (int)vec.size()) {
        nextChange = -1;
        stationary = true;
        targetPosition = lastPosition;
        return;
    }
    nextChange = vec[currentLine];
    targetPosition.x = vec[currentLine + 1];
    targetPosition.y = vec[currentLine + 2];
    targetPosition.z = is3D ? vec[currentLine + 3] : 0;
    targetStatus=is3D ? vec[currentLine+4] : vec[currentLine+3];

    currentLine += (is3D ? 5 : 4);
}

void BonnMotionDAMobility::move()
{
    simtime_t now = simTime();
    if (now == nextChange) {
        if (targetStatus!=status) {
           //std::cout<<"EMIT status: "<<status<<" ("<<targetPosition.x<<","<<targetPosition.y<<")"<<endl;
            if(targetStatus==2.0)
                emit(mobilityNodeRadioOff, this);
            else
                emit(mobilityNodeRadioOn, this);

       }
       status=targetStatus;
    }
    LineSegmentsMobilityBase::move();
    raiseErrorIfOutside();
}

