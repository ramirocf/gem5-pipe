#ifndef __DECODE_RENAME_FAULT_HH__
#define __DECODE_RENAME_FAULT_HH__

#include "config/the_isa.hh"
#include "base/types.hh"
#include "fi/fetch_decode_fault.hh"
#include "fi/faultq.hh"
#include "cpu/static_inst.hh"

/*
 * Inject a fault into the decode-rename stage pipeline register
 */


class Decode_RenameFault : public Fetch_DecodeFault 
{
private:

protected:
 //Fields
 short int numDestRegs = 5;
 short int branch = 6;
 short int simpleALU = 7;
 short int complexALU =8;
 short int memRef = 9;
 short int load = 10;
 short int numSrcRegs = 11;
 short int srcRegId0 = 12;
 short int srcRegId1 = 13;
 short int destRegId = 14;
 short int condBranch = 15;
 short int indirBranch = 16; 

public:

  Decode_RenameFault(std::ifstream &os, bool inherited);
  ~Decode_RenameFault();

  virtual const char *description() const;
  
  virtual void dump() const;

  virtual void set_field(std::string file_field);

  //Process functions
  StaticInstPtr process(StaticInstPtr curInst);
  void numDestRegs_process(StaticInstPtr &curInst);
  void branch_process(StaticInstPtr &curInst);
  void simpleALU_process(StaticInstPtr &curInst);
  void complexALU_process(StaticInstPtr &curInst);
  void memRef_process(StaticInstPtr &curInst);
  void load_process(StaticInstPtr &curInst);
  void numSrcRegs_process(StaticInstPtr &curInst);
  void srcRegId_process(StaticInstPtr &curInst,int id);
  void destRegId_process(StaticInstPtr &curInst);
  void condBranch_process(StaticInstPtr &curInst);
  void indirBranch_process(StaticInstPtr &curInst);
  void machInst_process(StaticInstPtr &curInst);
  Base_PipelineFault::dynInstPtr process(dynInstPtr curInst);
};

/**Rename Issue Fault Queue
 *Its purpouse is very straightforward
 */

class Decode_RenameFaultQueue : public InjectedFaultQueue
{
  private:

  protected:

  public:
    using InjectedFaultQueue::scan;
    virtual Decode_RenameFault *scan(std::string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType);
};
#endif // __DECODE_RENAME_FAULT_HH__
