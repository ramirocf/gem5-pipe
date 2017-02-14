#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/wb_commit_fault.hh"
#include "fi/fi_system.hh"
#include "cpu/static_inst.hh"

using namespace std;

WB_CommitFault::WB_CommitFault(std::ifstream &os,bool inherited)
  : Issue_ExecuteFault(os,true)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::WB_CommitFault);
   fi_system->wb_CommitFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"WB-Commit Fault::field : "<<field<<endl;
     cout<<"WB-Commit Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

WB_CommitFault::~WB_CommitFault()
{
}

const char *
WB_CommitFault::description() const
{
    return "WB-Commit Fault";
}

void WB_CommitFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===WB_CommitFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==WB_CommitFault::dump()===\n";
  }
}

void WB_CommitFault::set_field(string file_field)
{
  //First try with fields added during this stage
  if(file_field.compare("MisPredAddr")==0){
    field = misPredAddr;
    field_type = misPredAddr_type;
  }
  else if(file_field.compare("RedPC")==0){
    field = redPC;
    field_type = redPC_type;
  }
  else if(file_field.compare("SquashSeqNum")==0){
    field = squashSeqNum;
    field_type = squashSeqNum_type;
  }
  else if(file_field.compare("NumDest")==0){
    field = numDest;
    field_type = dynInst;
  }
  else if(file_field.compare("Load")==0){
    field = load;
    field_type = dynInst;
  }
  else if(file_field.compare("Squash")==0){
    field = squash;
    field_type = squash_type;
  }
  else if(file_field.compare("BranchMisPred")==0){
    field = branchMisPred;
    field_type = branchMisPred_type;
  }
  else if(file_field.compare("DestReg")==0){
    field = destReg;
    field_type = dynInst;
  }
  else if(file_field.compare("PC")==0){
    field = pC;
    field_type = dynInst;
  }
  else if(file_field.compare("PhyDestReg")==0){
    field = phyDestReg;
    field_type = dynInst;
  }
  else if(file_field.compare("PrevDestReg")==0){
    field = prevDestReg;
    field_type = dynInst;
  }
  else if(file_field.compare("BrTaken")==0){
    field = brTaken;
    field_type = brTaken_type;
  }
  else if(file_field.compare("MemRef")==0){
   field = memRef;
   field_type = dynInst;
  }
  else
    cout<<"No Such field: "<<file_field<<endl;
}

///Process functions

//Process for boolean values
bool WB_CommitFault::process(bool val)
{
  if(DTRACE(FaultInjection)){
		if(field == squash){
		  cout <<"===Squash::process()==="<<endl;
		}
		else if(field == branchMisPred){
		  cout<<"===BranchMisPred::process()==="<<endl;
		}
		else if(field == brTaken){
		  cout<<"===BranchTaken::process()==="<<endl;
		}
  }

  val = manifest_bool(val,getValue(),getValueType());
  check4reschedule();

  if(DTRACE(FaultInjection)){
	 if(field == squash){
		  cout <<"===Squash::process()==="<<endl;
		}
  
   else if(field == branchMisPred){
     cout<<"===BranchMisPred::process()==="<<endl;
   }
   else if(field == brTaken){
     cout<<"===BranchTaken::process()==="<<endl;
   }
  }

  return val;
}

//Process dor dynamic instruction's fields
Base_PipelineFault::dynInstPtr WB_CommitFault::process(dynInstPtr curInst)
{
  if(field==numDest){
    DPRINTF(FaultInjection,"===NumDest::process===\n");
    dump();
    StaticInstPtr staticInst = curInst->getcurInstr();
    Decode_RenameFault::numDestRegs_process(staticInst);
    curInst->setCurInst(staticInst);
    DPRINTF(FaultInjection,"===NumDest::process===\n");
  }
  else if(field==load){
    DPRINTF(FaultInjection,"===Load::process()===\n");
    dump();
    StaticInstPtr staticInst = curInst->getcurInstr();
    Decode_RenameFault::load_process(staticInst);
    curInst->setCurInst(staticInst);    
    DPRINTF(FaultInjection,"===Load::process()===\n");
  } 
  else if(field==destReg){
    DPRINTF(FaultInjection,"===DestRegId::process()===\n");
    dump();
    StaticInstPtr staticInst = curInst->getcurInstr();
    Decode_RenameFault::destRegId_process(staticInst);
    curInst->setCurInst(staticInst);    
    DPRINTF(FaultInjection,"===DestRegId::process()===\n");
  }
  else if(field==pC){
    DPRINTF(FaultInjection,"===PC::process()===\n");
    dump();
    Fetch_DecodeFault::pC_process(curInst);
    DPRINTF(FaultInjection,"===PC::process()===\n");
  }
  else if(field==phyDestReg){
    DPRINTF(FaultInjection,"===PhyDestReg::process()===\n");
    dump();
    Rename_IssueFault::phyDestReg_process(curInst);
    DPRINTF(FaultInjection,"===PhyDestReg::process()===\n");
  }
  else if(field==prevDestReg){
    DPRINTF(FaultInjection,"===PrevDestReg::process()===\n");
    dump();
    Rename_IssueFault::prevDestReg_process(curInst);
    DPRINTF(FaultInjection,"===PrevDestReg::process()===\n");
  }
  else if(field == memRef){
    DPRINTF(FaultInjection,"===MemRef::process()===\n");
    dump();
    StaticInstPtr staticInst = curInst->getcurInstr();
    Decode_RenameFault::memRef_process(staticInst);
    curInst->setCurInst(staticInst);    
    DPRINTF(FaultInjection,"===MemRef::process()===\n");
  }
  return curInst;
}

//Process for miscelaneous types
PCState WB_CommitFault::process(PCState pC)
{
    if(field==redPC){
     if(DTRACE(FaultInjection)){ 
       cout<<"===RedirectionPC::process()==="<<endl;
     }
     Addr curPC =  pC.pc();
     curPC = manifest(curPC,getValue(),getValueType());
     pC.pc(curPC);
     check4reschedule();
     if(DTRACE(FaultInjection)){ 
       cout<<"===RedirectionPC::process()==="<<endl;
     }
    }
    return pC;
}

//Class WriteBack Commit Fault Queue
WB_CommitFault *WB_CommitFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  WB_CommitFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<WB_CommitFault *> (head);
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
    p = reinterpret_cast<WB_CommitFault*> (p->nxt);
  }
  return(NULL);

}

