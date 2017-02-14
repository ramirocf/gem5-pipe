#ifndef __BASE_PIPELINE_FAULT_HH__
#define __BASE_PIPELINE_FAULT_HH__

#include "base/refcnt.hh"
#include "cpu/o3/impl.hh"
#include "cpu/o3/dyn_inst.hh"
#include "fi/o3cpu_injfault.hh"

using namespace std;

/**Class base model for a fault in our pipeline enviroment
 *it is not supposed to be used.Just as reference.
 */

class Base_PipelineFault : public O3CPUInjectedFault 
{

private:
  
protected:

  int field_type;
  /**Field for user input fault*/
  int field;
  /**User indicated bundle number to corrupt*/
  int bundle_num;
  

public:
  typedef typename O3CPUImpl::DynInstPtr dynInstPtr;
  Base_PipelineFault(std::ifstream &os);
  ~Base_PipelineFault();

  ///Common field types
  static const int machInst_type = 1;
  static const int staticInst = 2;
  static const int dynInst = 3;

  virtual const char *description() const;

//  virtual void set_field(string file_field) {}
  int get_field() {return field;}
  int get_fieldType() {return field_type;}

  void set_bundleNum(string file_bundle) { bundle_num = strtoll(file_bundle.c_str(), NULL, 10);}
  int get_bundleNum () {return bundle_num;}
  
};

#endif // __BASE_PIPELINE_FAULT_HH__

