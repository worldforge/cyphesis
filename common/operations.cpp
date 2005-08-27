// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "Add.h"
#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Burn.h"
#include "Delve.h"
#include "Dig.h"
#include "Mow.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"
#include "Unseen.h"
#include "Update.h"
#include "Monitor.h"
#include "Connect.h"

#include "CustomOp_impl.h"

#include <Atlas/Objects/Operation.h>

namespace Atlas { namespace Objects { namespace Operation {

template <>
int AddData::class_no = -1;

template class CustomOpData<SetData, AddProxy>;
template class CustomOpData<SetData, BurnProxy>;
template class CustomOpData<SetData, ChopProxy>;
template class CustomOpData<SetData, CutProxy>;
template class CustomOpData<SetData, DelveProxy>;
template class CustomOpData<SetData, DigProxy>;
template class CustomOpData<SetData, MowProxy>;
template class CustomOpData<SetData, EatProxy>;
template class CustomOpData<SetData, UnseenProxy>;
template class CustomOpData<SetData, TickProxy>;
template class CustomOpData<SetData, UpdateProxy>;
template class CustomOpData<SetData, NourishProxy>;
template class CustomOpData<SetData, SetupProxy>;
template class CustomOpData<SetData, MonitorProxy>;
template class CustomOpData<SetData, ConnectProxy>;

// template <>
// const char * const CustomOpData<SetData>::class_name = "add";

#warning Currently there is no difference between different children of a given type. There needs to be another template argument to differentiate.

} } }
