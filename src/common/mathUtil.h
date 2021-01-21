#pragma once

#include <cmath>
#include <limits>
#include <algorithm>
namespace Hix
{
	namespace Common
	{
		namespace MathUtil
		{
            template<typename NumberType>
            size_t smallestDecimal(NumberType x)
            {
                static constexpr NumberType NumberType = 10.0 * std::numeric_limits<NumberType>::epsilon();
                static constexpr size_t MAX_DIGIT_DECIMAL = std::numeric_limits<NumberType>::max_digits10; // 2^52 = 4,503,599,627,370,496
                int m = std::floor(std::log10(std::abs(x)));
                NumberType pow10i;
                for (size_t i = 0; i < MAX_DIGIT_DECIMAL; ++i)
                {
                    pow10i = std::pow(10, m);
                    NumberType y = std::round(x / pow10i) * pow10i;
                    if (std::abs(x - y) < std::abs(x) * ERROR_EPS)
                    {
                        break;
                    }
                    m--;
                }
                return std::max(0, -m);
            }
		}
	}
}

