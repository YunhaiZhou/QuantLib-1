
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

/*! \file europeanpathpricer.cpp
    \brief path pricer for European options

    \fullpath
    ql/MonteCarlo/%europeanpathpricer.cpp

*/

// $Id$

#include <ql/MonteCarlo/europeanpathpricer.hpp>
#include <ql/Pricers/singleassetoption.hpp>

using QuantLib::Pricers::ExercisePayoff;

namespace QuantLib {

    namespace MonteCarlo {

        EuropeanPathPricer::EuropeanPathPricer(Option::Type type,
          double underlying, double strike, DiscountFactor discount,
          bool useAntitheticVariance)
        : PathPricer<Path>(discount, useAntitheticVariance), type_(type),
          underlying_(underlying), strike_(strike) {
            QL_REQUIRE(underlying>0.0,
                "EuropeanPathPricer: "
                "underlying less/equal zero not allowed");
            QL_REQUIRE(strike>0.0,
                "EuropeanPathPricer: "
                "strike less/equal zero not allowed");
        }

        double EuropeanPathPricer::operator()(const Path& path) const {
            size_t n = path.size();
            QL_REQUIRE(n>0,
                "EuropeanPathPricer: the path cannot be empty");

            double log_drift = 0.0, log_random = 0.0;
            for (size_t i = 0; i < n; i++) {
                log_drift += path.drift()[i];
                log_random += path.diffusion()[i];
            }

            if (useAntitheticVariance_)
                return (
                    ExercisePayoff(type_, underlying_ *
                        QL_EXP(log_drift+log_random), strike_) +
                    ExercisePayoff(type_, underlying_ *
                        QL_EXP(log_drift-log_random), strike_)) *
                    discount_/2.0;
            else
                return ExercisePayoff(type_, underlying_ *
                        QL_EXP(log_drift+log_random), strike_) *
                    discount_;


        }

    }

}

