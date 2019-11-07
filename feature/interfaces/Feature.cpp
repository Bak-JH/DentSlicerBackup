#include "Feature.h"
using namespace Hix::Features;

Hix::Features::Feature::Feature()
{
}

Hix::Features::Feature::Feature(const std::unordered_set<GLModel*>& subjects):_subjects(subjects)
{
}

Hix::Features::Feature::~Feature()
{
}
