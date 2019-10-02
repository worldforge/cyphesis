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

#ifndef CYPHESIS_VARIANT_H
#define CYPHESIS_VARIANT_H

#include <boost/variant.hpp>
#include <type_traits>

namespace detail {

    template<typename... Lambdas>
    struct lambda_visitor;

    template<typename Lambda1, typename... Lambdas>
    struct lambda_visitor<Lambda1, Lambdas...>
        : public Lambda1,
          public lambda_visitor<Lambdas...>
    {

        using Lambda1::operator();
        using lambda_visitor<Lambdas...>::operator();

        lambda_visitor(Lambda1 l1, Lambdas... lambdas)
            : Lambda1(l1), lambda_visitor<Lambdas...>(lambdas...)
        {}
    };

    template<typename Lambda1>
    struct lambda_visitor<Lambda1>
        :
            public Lambda1
    {

        using Lambda1::operator();

        lambda_visitor(Lambda1 l1)
            : Lambda1(l1)
        {}
    };
}

/**
 * Allows composing a variant from lambdas.
 * @tparam Fs
 * @param fs
 * @return
 */
template<class...Fs>
auto compose(Fs&& ...fs)
{
    using visitor_type = detail::lambda_visitor<std::decay_t<Fs>...>;
    return visitor_type(std::forward<Fs>(fs)...);
};
#endif //CYPHESIS_VARIANT_H
