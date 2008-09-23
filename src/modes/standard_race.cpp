//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "modes/standard_race.hpp"
#include "user_config.hpp"
#include "unlock_manager.hpp"
#include "gui/menu_manager.hpp"

//-----------------------------------------------------------------------------
StandardRace::StandardRace() : World(), Clock::ClockListener()
{
    m_clock.registerEventListener(this);
    m_clock.setMode(CHRONO);
}

//-----------------------------------------------------------------------------
StandardRace::~StandardRace()
{
}
    
#pragma mark -
#pragma mark clock events

//-----------------------------------------------------------------------------
void StandardRace::countdownReachedZero() { }
//-----------------------------------------------------------------------------
void StandardRace::onGo()
{
    // Reset the brakes now that the prestart 
    // phase is over (braking prevents the karts 
    // from sliding downhill)
    for(unsigned int i=0; i<m_kart.size(); i++) 
    {
        m_kart[i]->resetBrakes();
    }
}
//-----------------------------------------------------------------------------
void StandardRace::onTerminate()
{
    World::terminateRace();
}

#pragma mark -
#pragma mark overridden from World

//-----------------------------------------------------------------------------
void StandardRace::restartRace()
{
    World::restartRace();
}
//-----------------------------------------------------------------------------
void StandardRace::update(float delta)
{
    m_clock.updateClock(delta);
    
    World::update(delta);
    if(!m_clock.isRacePhase()) return;
    
    // All karts are finished
    if(race_manager->getFinishedKarts() >= race_manager->getNumKarts() )
    {
        m_clock.raceOver();
	    if(user_config->m_profile<0) printProfileResultAndExit();
        unlock_manager->raceFinished();
    }   // if all karts are finished
    
    // All player karts are finished, but computer still racing
    // ===========================================================
    else if(race_manager->allPlayerFinished())
    {
        // Set delay mode to have time for camera animation, and
        // to give the AI some time to get non-estimated timings
        m_clock.raceOver(true /* delay */);
    }
}

//-----------------------------------------------------------------------------
void StandardRace::getDefaultCollectibles(int& collectible_type, int& amount)
{
    // in time trial mode, give zippers
    if(race_manager->getMinorMode() == RaceManager::MINOR_MODE_TIME_TRIAL)
    {
        collectible_type = COLLECT_ZIPPER;
        amount = race_manager->getNumLaps();
    }
    else World::getDefaultCollectibles(collectible_type, amount);
}
//-----------------------------------------------------------------------------
bool StandardRace::useRedHerring()
{
    // in time trial mode, don't use "red herrings"
    return race_manager->getMinorMode() != RaceManager::MINOR_MODE_TIME_TRIAL;
}
//-----------------------------------------------------------------------------
std::string StandardRace::getInternalCode() const
{
    if(race_manager->getMinorMode() == RaceManager::MINOR_MODE_TIME_TRIAL)
        return "STD_TIMETRIAL";
    else
        return "STANDARD";
}