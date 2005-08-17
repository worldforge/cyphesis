// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <string>

#ifndef COMMON_CUSTOM_OP_H
#define COMMON_CUSTOM_OP_H

template <class ParentOpData, const char * ClassName, int ClassNo>
class CustomOpData : public ParentOpData
{
protected:
    /// Construct a CustomOpData class definition.
    CustomOpData(CustomOpData *defaults = NULL) : ParentOpData((ParentOpData*)defaults)
    {
        this->m_class_no = ClassNo;
    }
    /// Default destructor.
    virtual ~CustomOpData();

public:
    /// Copy this object.
    virtual CustomOpData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ClassNo) current_class = -1; ParentOpData::iterate(current_class, attr);}

    //freelist related things
public:
    static CustomOpData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual CustomOpData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static CustomOpData *getDefaultObjectInstance();
private:
    static CustomOpData *defaults_CustomOpData;
    static CustomOpData *begin_CustomOpData;
};

#endif // COMMON_CUSTOM_OP_H
