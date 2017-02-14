#include "fi/base_pipeline_fault.hh"

using namespace std;

Base_PipelineFault::Base_PipelineFault(std::ifstream &os):O3CPUInjectedFault(os)
{
}

Base_PipelineFault::~Base_PipelineFault()
{

}

const char *Base_PipelineFault::description() const
{
  return "Base_PipelineFault";
}
