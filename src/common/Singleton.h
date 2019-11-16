/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_SINGLETON_H
#define CYPHESIS_SINGLETON_H

#include <cassert>
#include <boost/noncopyable.hpp>


template<typename T>
class Singleton : private boost::noncopyable
{
    protected:

        /**
         * @brief Standard constructor.
         */
        explicit Singleton()
        {
            assert(!ms_Singleton);
            ms_Singleton = static_cast<T*>( this );
        }

        virtual ~Singleton()
        {
            assert(ms_Singleton);
            ms_Singleton = nullptr;
        }

        /**
        @brief The static variable holding the singleton instance.
        Remember to instantiate this to nullptr in your implementation.
        */
        static T* ms_Singleton;

    public:

        /**
         *       @brief Gets the singleton instance.
         * @return The singleton instance.
         */
        static T& instance()
        {
            assert(ms_Singleton);
            return (*ms_Singleton);
        }

        /**
         *       @brief Gets a pointer to the singleton instance.
         * @return A pointer to the singleton instance.
         */
        static T* instancePtr()
        {
            assert(ms_Singleton);
            return ms_Singleton;
        }

        /**
         *       @brief Returns true if there's a singleton registered with the system.
         * @return True if there's a singleton available.
         */
        static bool hasInstance()
        {
            return ms_Singleton != nullptr;
        }
};
template<typename T> T* Singleton<T>::ms_Singleton = nullptr;


#endif //CYPHESIS_SINGLETON_H
