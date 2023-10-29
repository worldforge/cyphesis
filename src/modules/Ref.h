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

#ifndef CYPHESIS_REF_H
#define CYPHESIS_REF_H

#include <memory>
#include <iostream>

template<typename T>
class Ref
{
    public:
        template<class U> friend
        class Ref;

        constexpr Ref();

        constexpr Ref(const Ref& rhs);

        constexpr Ref(Ref&& rhs) noexcept;

        /* implicit */
        constexpr Ref(T* entity);

        /**
         * This allows us to create a new Ref from a Ref for a subclass.
         */
        /* implicit */
        template<class TSubclass>
        constexpr operator Ref<TSubclass>() const noexcept
        {
            return Ref<TSubclass>(m_inner);
        }

        /**
         * This allows us to create a new Ref from a Ref for a subclass.
         */
        /* implicit */
        template<class TSubclass>
        constexpr operator Ref<const TSubclass>() const noexcept
        {
            return Ref<const TSubclass>(m_inner);
        }

        ~Ref();

        constexpr Ref<T>& operator=(T* rhs);

        constexpr Ref& operator=(const Ref&);

        constexpr void reset(T* ptr = nullptr);

        /**
         * This operator allows us to assign a Ref for a subclass to this.
         *
         * Like this:
         * Ref<SuperClass> r;
         * r = Ref<SubClass>();
         *
         */
        template<class TSubclass>
        constexpr Ref& operator=(const Ref<TSubclass>& rhs)
        {
            if (rhs.get()) {
                rhs.get()->incRef();
            }
            decRef();
            this->m_inner = rhs.m_inner;
            return *this;
        }

        constexpr Ref& operator=(Ref&&) noexcept;

        /**
         * This operator allows us to move a Ref for a subclass to this.
         *
         * Like this:
         * Ref<SuperClass> r;
         * r = std::move(Ref<SubClass>());
         *
         */
        template<class TSubclass>
        constexpr Ref& operator=(Ref<TSubclass>&& rhs) noexcept
        {

            decRef();
            this->m_inner = rhs.m_inner;
            rhs.m_inner = nullptr;

            return *this;
        }

        constexpr T& operator*() const
        {
            return *m_inner;
        }

        constexpr T* operator->() const
        {
            return m_inner;
        }

        constexpr T* get() const
        {
            return m_inner;
        }

        constexpr explicit operator bool() const
        {
            return (m_inner != nullptr);
        }

        constexpr bool operator!() const
        {
            return (m_inner == nullptr);
        }

        constexpr bool operator==(const Ref& e) const
        {
            return (m_inner == e.m_inner);
        }

        template<class TSubclass>
        constexpr bool operator==(const Ref<TSubclass>& e) const
        {
            return (m_inner == e.get());
        }

        constexpr bool operator==(const T* e) const
        {
            return (m_inner == e);
        }

        constexpr bool operator!=(const Ref& e) const
        {
            return (m_inner != e.m_inner);
        }

        template<class TSubclass>
        constexpr bool operator!=(const Ref<TSubclass>& e) const
        {
            return (m_inner != e.get());
        }

        constexpr bool operator!=(const T* e) const
        {
            return (m_inner != e);
        }

        constexpr bool operator<(const Ref& e) const
        {
            return (m_inner < e.m_inner);
        }

        constexpr explicit operator T*() const;

    private:
        T* m_inner;

        void decRef() {
            if (this->m_inner) {
                if (this->m_inner->decRef() == 0) {
                    delete this->m_inner;
                    this->m_inner = nullptr;
                }
            }
        }

};

template<typename T>
std::ostream& operator<<(std::ostream& s, const Ref<T>& d);


template<typename T>
constexpr Ref<T>::Ref()
    : m_inner(nullptr)
{

}

template<typename T>
constexpr Ref<T>::Ref(const Ref& rhs)
    : m_inner(rhs.m_inner)
{
    if (this->m_inner) {
        this->m_inner->incRef();
    }
}


template<typename T>
constexpr Ref<T>::Ref(Ref&& rhs) noexcept
    : m_inner(rhs.m_inner)
{
    rhs.m_inner = nullptr;
}

template<typename T>
constexpr Ref<T>::Ref(T* entity)
    : m_inner(entity)
{
    if (this->m_inner) {
        this->m_inner->incRef();
    }
}

template<typename T>
Ref<T>::~Ref()
{
    decRef();
}

template<typename T>
constexpr Ref<T>& Ref<T>::operator=(T* rhs)
{
    reset(rhs);
    return *this;
}

template<typename T>
constexpr void Ref<T>::reset(T* ptr)
{
    if (ptr) {
        ptr->incRef();
    }
    decRef();
    this->m_inner = ptr;
}



template<typename T>
constexpr Ref<T>& Ref<T>::operator=(const Ref<T>& rhs)
{
    if (rhs.m_inner) {
        rhs.m_inner->incRef();
    }
    decRef();
    this->m_inner = rhs.m_inner;
    return *this;
}

template<typename T>
constexpr Ref<T>& Ref<T>::operator=(Ref<T>&& rhs) noexcept
{
    if (this != &rhs) {
        decRef();
        this->m_inner = rhs.m_inner;
        rhs.m_inner = nullptr;
    }
    return *this;
}


template<typename T>
constexpr Ref<T>::operator T*() const
{
    return m_inner;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const Ref<T>& d) {
    if (d) {
        s << d->describeEntity();
    } else {
        s << "[none]";
    }
    return s;
}



#endif //CYPHESIS_REF_H
