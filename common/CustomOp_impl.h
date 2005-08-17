// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CUSTOM_OP_IMPL_H
#define COMMON_CUSTOM_OP_IMPL_H

#include "common/CustomOp.h"

#include <list>

template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo>::~CustomOpData()
{
}
// <ParentOpData, ClassName, ClassNo>

template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> * CustomOpData<ParentOpData, ClassName, ClassNo>::copy() const
{
    CustomOpData<ParentOpData, ClassName, ClassNo> * copied = CustomOpData<ParentOpData, ClassName, ClassNo>::alloc();
    *copied = *this;
    return copied;
}

template<class ParentOpData, const char * ClassName, int ClassNo>
bool CustomOpData<ParentOpData, ClassName, ClassNo>::instanceOf(int classNo) const
{
    if(ClassNo == classNo) return true;
    return ParentOpData::instanceOf(classNo);
}

//freelist related methods specific to this class
template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> *CustomOpData<ParentOpData, ClassName, ClassNo>::defaults_CustomOpData = 0;

template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> *CustomOpData<ParentOpData, ClassName, ClassNo>::begin_CustomOpData = 0;

template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> *CustomOpData<ParentOpData, ClassName, ClassNo>::alloc()
{
    if(begin_CustomOpData) {
        CustomOpData *res = begin_CustomOpData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_CustomOpData = (CustomOpData *)begin_CustomOpData->m_next;
        return res;
    }
    return new CustomOpData<ParentOpData, ClassName, ClassNo>(CustomOpData<ParentOpData, ClassName, ClassNo>::getDefaultObjectInstance());
}

template<class ParentOpData, const char * ClassName, int ClassNo>
void CustomOpData<ParentOpData, ClassName, ClassNo>::free()
{
    this->m_next = begin_CustomOpData;
    begin_CustomOpData = this;
}


template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> *CustomOpData<ParentOpData, ClassName, ClassNo>::getDefaultObjectInstance()
{
    if (defaults_CustomOpData == 0) {
        defaults_CustomOpData = new CustomOpData;
        defaults_CustomOpData->attr_objtype = "op";
        defaults_CustomOpData->attr_serialno = 0;
        defaults_CustomOpData->attr_refno = 0;
        defaults_CustomOpData->attr_seconds = 0.0;
        defaults_CustomOpData->attr_future_seconds = 0.0;
        defaults_CustomOpData->attr_stamp = 0.0;
        defaults_CustomOpData->attr_parents = std::list<std::string>(1, ClassName);
    }
    return defaults_CustomOpData;
}

template<class ParentOpData, const char * ClassName, int ClassNo>
CustomOpData<ParentOpData, ClassName, ClassNo> *CustomOpData<ParentOpData, ClassName, ClassNo>::getDefaultObject()
{
    return CustomOpData<ParentOpData, ClassName, ClassNo>::getDefaultObjectInstance();
}

#endif // COMMON_CUSTOM_OP_IMPL_H
