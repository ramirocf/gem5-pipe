#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/issue_execute_fault.hh"
#include "fi/fi_system.hh"
#include "cpu/o3/comm.hh"
#include "cpu/static_inst.hh"

using namespace std;

Issue_ExecuteFault::Issue_ExecuteFault(std::ifstream &os,bool inherited)
  : Rename_IssueFault(os,true)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::Issue_ExecuteFault);
   fi_system->issue_ExecuteFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"Issue-Execute Fault::field : "<<field<<endl;
     cout<<"Issue-Execute Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

Issue_ExecuteFault::~Issue_ExecuteFault()
{
}

const char *
Issue_ExecuteFault::description() const
{
    return "Issue-Execute Fault";
}

void Issue_ExecuteFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===Issue_ExecuteFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==Issue_ExecuteFault::dump()===\n";
  }
}

void Issue_ExecuteFault::set_field(string file_field)
{
  //First try with fields added during this stage
  if(file_field.compare("SrcReg0Ready")==0){
    field = srcReg0Ready;
    field_type = srcRegReady_type;
  }
  else if(file_field.compare("SrcReg1Ready")==0){
    field = srcReg1Ready;
    field_type = srcRegReady_type;
  }
  else if(file_field.compare("OpClass")==0){
    field = opClass;
    field_type = opClass_type;
  }
  else{
    //Look for fields in previous stages
    Rename_IssueFault::set_field(file_field);
  }
}

///Process functions
Base_PipelineFault::dynInstPtr Issue_ExecuteFault::process(dynInstPtr curInst)
{
  if(field==srcReg0Ready){
    DPRINTF(FaultInjection,"===SrcReg0Ready::process===");
    dump();
    srcRegReady_process(curInst,0);
    DPRINTF(FaultInjection,"===SrcReg0Ready::process===");
  }
  else if(field==srcReg1Ready){
    DPRINTF(FaultInjection,"===SrcRegReady::process()===");
    dump();
    srcRegReady_process(curInst,1);
    DPRINTF(FaultInjection,"===SrcRegReady::process()===");
  } 
  else if(field==opClass){
    DPRINTF(FaultInjection,"===OpClass::process()===");
    dump();
    opClass_process(curInst);
    DPRINTF(FaultInjection,"===OpClass::process()===");
  }
  else{
    //Look for the right process in previous faults
    Rename_IssueFault::process(curInst);
  }
  return curInst;
}

//Process for src register ready flag
void Issue_ExecuteFault::srcRegReady_process(dynInstPtr &curInst, int id)
{
  bool isReady = curInst->isReadySrcRegIdx(id);
  isReady = manifest_bool(isReady,getValue(),getValueType());
  curInst->set_SrcRegReady(isReady,id);
  check4reschedule();
}

//Process for op class index
void Issue_ExecuteFault::opClass_process(dynInstPtr &curInst)
{
  OpClass op = curInst->opClass();
  op = manifest(op,getValue(),getValueType());
  curInst->set_opClass(op);
  check4reschedule();
}


//Class Issue Execute Fault Queue
Issue_ExecuteFault *Issue_ExecuteFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  Issue_ExecuteFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<Issue_ExecuteFault *> (head);
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
    p = reinterpret_cast<Issue_ExecuteFault*> (p->nxt);
  }
  return(NULL);

}

