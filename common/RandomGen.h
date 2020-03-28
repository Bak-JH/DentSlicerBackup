#pragma once
#include <random>




class RandomGen
{
private:
    //mersenne_twister_engine with uint64_t as seed. btw, default_random_engine is mt19937 with 32 bit seed
    std::mt19937_64 _random;
public:
    RandomGen(uint64_t pSeed);
	RandomGen();
	//reset randomGen with a new seed
	void reset(uint64_t pSeed);
    float getFloat();
    float getFloat(float pMin, float pMax);
    uint64_t getUInt();
    uint64_t getUInt(uint64_t pMin, uint64_t pMax);
    int64_t getInt();
    int64_t getInt(int64_t pMin, int64_t pMax);

    bool randomChoice(float bias = 0.5);

    float normallyDistributedSingle(float standardDeviation, float pMean);
	float quickBoundNormalDistribution(float standardDeviation, float pMean, float pMin, float pMax);
	//truncated normal distribution, closer to normal distribtuion than quickBoundNormalDistribution, but slower
    float truncatedNormalDistributedSingle(float standardDeviation, float pMean, float pMin, float pMax);


};
