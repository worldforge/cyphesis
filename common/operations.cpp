// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "Add.h"
#include "Attack.h"
#include "Eat.h"
#include "Burn.h"
#include "Drop.h"
#include "Nourish.h"
#include "Pickup.h"
#include "Setup.h"
#include "Tick.h"
#include "Unseen.h"
#include "Update.h"
#include "Monitor.h"
#include "Connect.h"

/// \brief Operation classes not defined in the Atlas spec
///
/// Operation classes used by Cyphesis as extensions of the
/// original Atlas specification used to generate the classes in
/// the Atlas-C++ library.
/// \defgroup CustomOperations Custom Operation Classes

namespace Atlas { namespace Objects { namespace Operation {

int ACTUATE_NO = -1;
int ADD_NO = -1;
int ATTACK_NO = -1;
int BURN_NO = -1;
int CONNECT_NO = -1;
int DROP_NO = -1;
int MONITOR_NO = -1;
int EAT_NO = -1;
int UNSEEN_NO = -1;
int UPDATE_NO = -1;
int NOURISH_NO = -1;
int PICKUP_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;

} } }
