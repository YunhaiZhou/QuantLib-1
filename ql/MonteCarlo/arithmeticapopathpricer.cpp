
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.org/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, please email quantlib-users@lists.sourceforge.net
 * The license is also available at http://quantlib.org/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.org/group.html
*/

/*! \file arithmeticapopathpricer.cpp
    \brief arithmetic average price option path pricer

    \fullpath
    ql/MonteCarlo/%arithmeticapopathpricer.cpp

*/

// $Id$

#include <ql/MonteCarlo/arithmeticapopathpricer.hpp>
#include <ql/Pricers/singleassetoption.hpp>

using QuantLib::Pricers::ExercisePayoff;

namespace QuantLib {

    namespace MonteCarlo {

        ArithmeticAPOPathPricer::ArithmeticAPOPathPricer(
          Option::Type type, double underlying, double strike,
          DiscountFactor discount, bool useAntitheticVariance)
        : PathPricer<Path>(discount, useAntitheticVariance), type_(type),
          underlying_(underlying), strike_(strike) {
            QL_REQUIRE(underlying>0.0,
                "ArithmeticAPOPathPricer: "
                "underlying less/equal zero not allowed");
            QL_REQUIRE(strike>0.0,
                "ArithmeticAPOPathPricer: "
                "strike less/equal zero not allowed");
        }

        double ArithmeticAPOPathPricer::operator()(const Path& path) const {

            size_t n = path.size();
            QL_REQUIRE(n>0,
                "ArithmeticAPOPathPricer: the path cannot be empty");

            double price1 = underlying_;
            double averagePrice1 = 0.0;
            size_t i;
            for (i=0; i<n; i++) {
                price1 *= QL_EXP(path.drift()[i]+path.diffusion()[i]);
                averagePrice1 += price1;
            }
            averagePrice1 = averagePrice1/n;

            if (useAntitheticVariance_) {
                double price2 = underlying_;
                double averagePrice2 = 0.0;

                for (i=0; i<n; i++) {
                    price2 *= QL_EXP(path.drift()[i]-path.diffusion()[i]);
                    averagePrice2 += price2;
                }
                averagePrice2 = averagePrice2/n;
                return discount_/2.0*(ExercisePayoff(type_, averagePrice1, strike_)
                    +ExercisePayoff(type_, averagePrice2, strike_));
            } else
                return discount_*ExercisePayoff(type_, averagePrice1, strike_);
        }

    }

}
