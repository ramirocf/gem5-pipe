#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/decode_rename_fault.hh"
#include "fi/fi_system.hh"
#include "cpu/o3/comm.hh"
#include "cpu/static_inst.hh"

using namespace std;

Rename_IssueFault::Rename_IssueFault(std::ifstream &os,bool inherited)
  : Decode_RenameFault(os,true)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::Rename_IssueFault);
   fi_system->rename_IssueFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"Rename-Issue Fault::field : "<<field<<endl;
     cout<<"Rename-Issue Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

Rename_IssueFault::~Rename_IssueFault()
{
}

const char *
Rename_IssueFault::description() const
{
    return "Rename-Issue Fault";
}

void Rename_IssueFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===RenameIssueFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==RenameIssueFault::dump()===\n";
  }
}

void Rename_IssueFault::set_field(string file_field)
{
  //First try with fields added during this stage
  if(file_field.compare("PhySrcReg0")==0){
    field = phySrcReg0;
    field_type = srcReg_type;
  }
  else if(file_field.compare("PhySrcReg1")==0){
    field = phySrcReg1;
    field_type = srcReg_type;
  }
  else if(file_field.compare("PhyDestReg")==0){
    field = phyDestReg;
    field_type = destReg_type;
  }
  else if(file_field.compare("PrevDestReg")==0){
    field = prevDestReg;
    field_type = destReg_type;
  }
  else{
    //Look for fields in previous stages
    Decode_RenameFault::set_field(file_field);
  }
}

///Process functions
Base_PipelineFault::dynInstPtr Rename_IssueFault::process(dynInstPtr curInst)
{
  if(field==phySrcReg0){
    DPRINTF(FaultInjection,"===PhySrcReg0::process===");
    dump();
    phySrcReg_process(curInst,0);
    DPRINTF(FaultInjection,"===PhySrcReg0::process===");
  }
  else if(field==phySrcReg1){
    DPRINTF(FaultInjection,"===PhySrcReg1::process()===");
    dump();
    phySrcReg_process(curInst,1);
    DPRINTF(FaultInjection,"===PhySrcReg1::process()===");
  } 
  else if(field==phyDestReg){
    DPRINTF(FaultInjection,"===PhyDestReg::process()===");
    dump();
    phyDestReg_process(curInst);
    DPRINTF(FaultInjection,"===PhyDestReg::process()===");
  }
  else if(field==prevDestReg){
    DPRINTF(FaultInjection,"===PrevDestReg::process()===");
    dump();
    prevDestReg_process(curInst);
    DPRINTF(FaultInjection,"===PrevDestReg::process()===");
  }
  else{
    //Look for the right process in previous faults
    //First check for static inst's fields
    StaticInstPtr staticInst = curInst->getcurInstr();
    staticInst = Decode_RenameFault::process(staticInst);
    curInst-> setCurInst(staticInst);
    //Now for dyn inst' fields
    Decode_RenameFault::process(curInst);
  }
  return curInst;
}

//Process for physical source registers assigned
void Rename_IssueFault::phySrcReg_process(dynInstPtr &curInst,int id)
{
  PhysRegIndex srcReg = curInst->renamedSrcRegIdx(id);
  srcReg = manifest(srcReg,getValue(),getValueType());
  curInst->renameSrcReg(id,srcReg);
  check4reschedule();
}

//Process for physical destination registers assigned
void Rename_IssueFault::phyDestReg_process(dynInstPtr &curInst)
{
  PhysRegIndex destReg = curInst->renamedDestRegIdx(0);
  destReg = manifest(destReg,getValue(),getValueType());
  curInst->renameDestReg(0,destReg,curInst->prevDestRegIdx(0));
  check4reschedule();
}

//Process for prevoiusly assigned physical destination register
void Rename_IssueFault::prevDestReg_process(dynInstPtr &curInst)
{
  PhysRegIndex prevReg = curInst->prevDestRegIdx(0);
  prevReg = manifest(prevReg,getValue(),getValueType());
  curInst->renameDestReg(0,curInst->renamedDestRegIdx(0),prevReg);
  check4reschedule();
}

//Class Rename Issue Fault Queue
Rename_IssueFault *Rename_IssueFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  Rename_IssueFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<Rename_IssueFault *> (head);
  //pass the list and check if a fault meets the conditions

  while(p && flag){
    if(!p->isManifested()){
      exec_time = 0;
      exec_instr = 0;
      //find how much time do I run.
      if(name().compare("DecodeStageFaultQueue") == 0 ){
        i = fi_system->get_fi_decode_counters( p , thisThread, s , &exec_instr , &exec_time );
        called++;
      }
      else if(name().compare("IEWStageFaultQueue") == 0 ){
        i = fi_system->get_fi_exec_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      else if (name().compare("LoadStoreFaultQueue") == 0 ){
        i = fi_system->get_fi_loadstore_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      else{
        i = fi_system->get_fi_fetch_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      if(i){
        switch( p->getTimingType() ){
          case (InjectedFault ::TickTiming):
            {  
              if((exec_time == p->getTiming()&&
                curBundle == p->get_bundleNum() &&
                fieldType == p->get_fieldType())){ //correct time so intend to manifest
                p->setServicedAt(exec_time);
                return(p);
              }
              else if(exec_time > p->getTiming())
                flag = 0;
            }
            break;
          case (InjectedFault ::InstructionTiming):
            {
              if(exec_instr == p->getTiming() &&
                curBundle == p->get_bundleNum() &&
                fieldType == p->get_fieldType()){
                p->setServicedAt(exec_instr);
                return(p);
              }
              else if(exec_instr > p->getTiming())
                flag = 0;
            }
            break;
          case (InjectedFault ::VirtualAddrTiming):
            {
              if(vaddr == p->getTiming() + thisThread.getMagicInstVirtualAddr() ){
                p->setServicedAt(vaddr);
                p->dump();
                return(p);
              }

            }
            break;
          default:
            {
              std::cout << "InjectedFaultQueue::scan() - getTimingType default type error\n";
              assert(0);
              break;
            }


        }
      }
    }
    p = reinterpret_cast<Rename_IssueFault*> (p->nxt);
  }
  return(NULL);

}

