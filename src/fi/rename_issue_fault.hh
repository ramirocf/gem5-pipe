#ifndef __RENAME_ISSUE_FAULT_HH__
#define __RENAME_ISSUE_FAULT_HH__

#include "config/the_isa.hh"
#include "base/types.hh"
#include "fi/decode_rename_fault.hh"
#include "fi/faultq.hh"

/*
 * Inject a fault into the rename-issue stage pipeline register
 */


class Rename_IssueFault : public Decode_RenameFault 
{
private:

protected:
 //Fields
 short int phySrcReg0 = 17;
 short int phySrcReg1 = 18;
 short int phyDestReg = 19;
 short int prevDestReg = 20;

public:
  //field types
  static const int srcReg_type = 4;
  static const int destReg_type = 5; 
  Rename_IssueFault(std::ifstream &os, bool inherited);
  ~Rename_IssueFault();

  virtual const char *description() const;
  
  virtual void dump() const;

  virtual void set_field(std::string file_field);

  //Process functions
  Base_PipelineFault::dynInstPtr process(dynInstPtr curInst);
  void phySrcReg_process(dynInstPtr &curInst, int id);
  void phyDestReg_process(dynInstPtr &curInst);
  void prevDestReg_process(dynInstPtr &curInst);
};

/**Fetch Decode Fault Queue
 *Its purpouse is very straightforward
 */

class Rename_IssueFaultQueue : public InjectedFaultQueue
{
  private:

  protected:

  public:
    using InjectedFaultQueue::scan;
    virtual Rename_IssueFault *scan(std::string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType);
};
#endif // __RENAME_ISSUE_FAULT_HH__
