#include "RandomGen.h"



RandomGen::RandomGen(uint64_t pSeed): _random(pSeed)
{
}

RandomGen::RandomGen() : RandomGen(0)
{
}

void RandomGen::reset(uint64_t pSeed)
{
	_random = std::mt19937_64(pSeed);
}

float RandomGen::getFloat(float pMin, float pMax)
{
    std::uniform_real_distribution<float> distribution(pMin, pMax);

    return distribution(_random);

}

uint64_t RandomGen::getUInt()
{
    return getUInt(0, UINT64_MAX);
}

uint64_t RandomGen::getUInt(uint64_t pMin, uint64_t pMax)
{
    std::uniform_int_distribution<uint64_t> distribution(pMin, pMax);

    return distribution(_random);
}

bool RandomGen::randomChoice(float bias)
{
    //if (bias > 1)
    //    throw std::invalid_argument("randomChoice bias larger than 1");
    return getFloat() < bias;

}

float RandomGen::getFloat()
{
    return getFloat(0, 1);
}

float RandomGen::normallyDistributedSingle(float standardDeviation, float pMean)
{
    std::normal_distribution<float> distribution(pMean, standardDeviation);
    return distribution(_random);

}

float RandomGen::quickBoundNormalDistribution(float standardDeviation, float pMean, float pMin, float pMax)
{
	float result = normallyDistributedSingle(standardDeviation, pMean);
	if (result < pMin || result > pMax)
	{
		return getFloat(pMin, pMax);
	}
	else
	{
		return result;
	}
}

float RandomGen::truncatedNormalDistributedSingle(float standardDeviation, float pMean, float pMin, float pMax)
{
	float truncatedResult;
    float nMax = (pMax - pMean) / standardDeviation;
    float nMin = (pMin - pMean) / standardDeviation;
    float nRange = nMax - nMin;
    float nMaxSq = nMax * nMax;
    float nMinSq = nMin * nMin;
    float subFrom = nMinSq;
    if (nMin < 0 && 0 < nMax) subFrom = 0;
    else if (nMax < 0) subFrom = nMaxSq;

    float sigma = 0.0;
    float u;
    float z;
    do
    {
        z = nRange * getFloat() + nMin; // uniform[normMin, normMax]
        sigma = std::exp((subFrom - z * z) / 2);
        u = getFloat();
    } while (u > sigma);
	truncatedResult = z * standardDeviation + pMean;
	return truncatedResult;

}