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

#ifndef __INET_BONNMOTIONDAMOBILITY_H
#define __INET_BONNMOTIONDAMOBILITY_H

#include "INETDefs.h"

#include "mobility/common/LineSegmentsMobilityBase.h"
#include "BonnMotionDAFileCache.h"


/**
 * @brief Uses the BonnMotionDA native file format. See NED file for more info.
 *
 * @ingroup mobility
 * @author Andras Varga and Nuno Cruz
 */
class INET_API BonnMotionDAMobility : public LineSegmentsMobilityBase
{
  protected:
    // state
    bool is3D;
    const BonnMotionDAFile::Line *lines;
    int currentLine;
    double maxSpeed; // the possible maximum speed at any future time
    double status; // status of the radio interface of the device
    double targetStatus;
    static simsignal_t mobilityNodeRadioOn;
    static simsignal_t mobilityNodeRadioOff;

  protected:
    virtual int numInitStages() const { return 3; }

    /** @brief Initializes mobility model parameters. */
    virtual void initialize(int stage);

    /** @brief Initializes the position according to the mobility model. */
    virtual void setInitialPosition();

    /** @brief Overridden from LineSegmentsMobilityBase. */
    virtual void setTargetPosition();

    /** @brief Overridden from LineSegmentsMobilityBase. */
    virtual void move();

    virtual void computeMaxSpeed();

  public:
    BonnMotionDAMobility();

    virtual ~BonnMotionDAMobility();

    virtual double getMaxSpeed() const { return maxSpeed; }
};


#endif // ifndef __INET_BONNMOTIONDAMOBILITY_H

