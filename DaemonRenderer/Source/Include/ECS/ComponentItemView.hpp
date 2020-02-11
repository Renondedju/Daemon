/*
 *  MIT License
 *
 *  Copyright (c) 2019 Basile Combet, Philippe Yi
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <type_traits>

#include "Config.hpp"

#include "Meta/ValueIndexer.hpp"
#include "Types/FundamentalTypes.hpp"
#include "Containers/SOA/DataLayoutView.hpp"

BEGIN_DAEMON_NAMESPACE

template <typename TPack, typename... TTypes>
struct ComponentItemView;

template <template <DAEsize...> class TPack, DAEsize... TIndices, typename... TTypes>
struct ComponentItemView<TPack<TIndices...>, TTypes...> : public DataLayoutView<std::index_sequence<TIndices...>, TTypes...>
{
    template<DAEsize TMember>
    auto&       Fetch()       { return std::get<SelectValueIndex<TMember, TIndices...>>(*this); }

    template<DAEsize TMember>
    auto const& Fetch() const { return std::get<SelectValueIndex<TMember, TIndices...>>(*this); }

    // Making constructors available
    using DataLayoutView<std::index_sequence<TIndices...>, TTypes...>::DataLayoutView;
    using DataLayoutView<std::index_sequence<TIndices...>, TTypes...>::operator=;
};

END_DAEMON_NAMESPACE