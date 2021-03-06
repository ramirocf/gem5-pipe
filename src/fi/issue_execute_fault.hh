#ifndef __ISSUE_EXECUTE_FAULT_HH__
#define __ISSUE_EXECUTE_FAULT_HH__

#include "config/the_isa.hh"
#include "base/types.hh"
#include "fi/rename_issue_fault.hh"
#include "fi/faultq.hh"
#include "cpu/op_class.hh"

/*
 * Inject a fault into the issue to execute stage pipeline register
 */


class Issue_ExecuteFault : public Rename_IssueFault 
{
private:

protected:
 //Fields
 short int srcReg0Ready = 21;
 short int srcReg1Ready = 22;
 short int opClass = 23;

public:
  //field types
  static const int srcRegReady_type = 6;
  static const int opClass_type = 7;
 
  Issue_ExecuteFault(std::ifstream &os, bool inherited);
  ~Issue_ExecuteFault();

  virtual const char *description() const;
  
  virtual void dump() const;

  virtual void set_field(std::string file_field);

  //Process functions
  Base_PipelineFault::dynInstPtr process(dynInstPtr curInst);
  void srcRegReady_process(dynInstPtr &curInst, int id);
  void opClass_process(dynInstPtr &curInst);
};

/**Issue Execute Fault Queue
 *Its purpouse is very straightforward
 */

class Issue_ExecuteFaultQueue : public InjectedFaultQueue
{
  private:

  protected:

  public:
    using InjectedFaultQueue::scan;
    virtual Issue_ExecuteFault *scan(std::string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType);
};
#endif // __ISSUE_EXECUTE_FAULT_HH__
