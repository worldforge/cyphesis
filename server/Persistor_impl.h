// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_IMPL_H
#define SERVER_PERSISTOR_IMPL_H

template <class T>
class Persistor;

template <class T>
void Persistor<T>::persist(T & t)
{
    std::cout << "Persistor::persist<" << m_class << ">(" << t.getId()
              << ")" << std::endl << std::flush;
}

#endif // SERVER_PERSISTOR_IMPL_H
