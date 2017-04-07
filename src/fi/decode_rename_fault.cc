#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/decode_rename_fault.hh"
#include "fi/fi_system.hh"
#include "cpu/static_inst.hh"
using namespace std;

Decode_RenameFault::Decode_RenameFault(std::ifstream &os,bool inherited)
  : Fetch_DecodeFault(os,true)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::Decode_RenameFault);
   fi_system->decode_RenameFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"Decode-Rename Fault::field : "<<field<<endl;
     cout<<"Decode-Rename Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

Decode_RenameFault::~Decode_RenameFault()
{
}

void Decode_RenameFault::set_field(string file_field)
{
 //Fisr searh for fields in this stage
 if(file_field.compare("NumDestRegs")==0){
   field = numDestRegs;
   field_type = staticInst;
 }
 else if(file_field.compare("Branch")==0){
   field = branch;
   field_type = staticInst;
 }
 else if(file_field.compare("SimpleALU")==0){
   field = simpleALU;
   field_type = staticInst;
 }
 else if(file_field.compare("ComplexALU")==0){
   field = complexALU;
   field_type = staticInst;
 }
 else if(file_field.compare("SimpleALU")==0){
   field = simpleALU;
   field_type = staticInst;
 }
 else if(file_field.compare("MemRef")==0){
   field = memRef;
   field_type = staticInst;
 }
 else if(file_field.compare("Load")==0){
   field = load;
   field_type = staticInst;
 }
 else if(file_field.compare("NumSrcRegs")==0){
   field = numSrcRegs;
   field_type = staticInst;
 }
 else if(file_field.compare("DestRegId")==0){
   field = destRegId;
   field_type = staticInst;
 }
 else if(file_field.compare("SrcRegId0")==0){
   field = srcRegId0;
   field_type = staticInst;
 }      
 else if(file_field.compare("SrcRegId1")==0){
   field = srcRegId1;
   field_type = staticInst;
 }
 else if(file_field.compare("DestRegId")==0){
   field = destRegId;
   field_type = staticInst;
 }
 else if(file_field.compare("CondBranch")==0){
   field = condBranch;
   field_type = staticInst;
 }
 else if(file_field.compare("IndirBranch")==0){
   field = indirBranch;
   field_type = staticInst;  
  }
 else{
   //Look for fields in previous faults
   Fetch_DecodeFault::set_field(file_field);
 	 field_type = staticInst;
	}
}
const char *
Decode_RenameFault::description() const
{
    return "Decode_RenameFault";
}

void Decode_RenameFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===Decode_RenameFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==Decode_RenameFault::dump()===\n";
  }
}

///Process functions
///Injecting a fault after decode stage
StaticInstPtr Decode_RenameFault::process(StaticInstPtr curInst)
{

  if(field == numDestRegs){
    DPRINTF(FaultInjection,"===NumDestRegs::process===");
    dump();
    numDestRegs_process(curInst);
    DPRINTF(FaultInjection,"===NumDestRegs::process===");
  }
  else if(field == branch){
    DPRINTF(FaultInjection,"===Branch::process===");    
    dump();
    branch_process(curInst);
    DPRINTF(FaultInjection,"===Branch::process===");
  }
 else if(field == simpleALU){
    DPRINTF(FaultInjection,"===SimpleALU::process===");    
    dump();
    simpleALU_process(curInst);
    DPRINTF(FaultInjection,"===SimpleALU::process===");
  }
 else if(field == complexALU){
    DPRINTF(FaultInjection,"===ComplexALU::process===");    
    dump();
    complexALU_process(curInst);
    DPRINTF(FaultInjection,"===ComplexALU::process===");
  }
 else if(field == memRef){
    DPRINTF(FaultInjection,"===MemRef::process===");    
    dump();
    memRef_process(curInst);
    DPRINTF(FaultInjection,"===MemRef::process===");
  }
 else if(field == load){
    DPRINTF(FaultInjection,"===Load::process===");    
    dump();
    load_process(curInst);
    DPRINTF(FaultInjection,"===Load::process===");
  }
 else if(field == numSrcRegs){
    DPRINTF(FaultInjection,"===NumSrcRegs::process===");    
    dump();
    numSrcRegs_process(curInst);
    DPRINTF(FaultInjection,"===NumSrcRegs::process===");
  } 
 else if(field == srcRegId0){
    DPRINTF(FaultInjection,"===SrcRegId0::process===");    
    dump();
    srcRegId_process(curInst,0);
    DPRINTF(FaultInjection,"===SrcRegId0::process===");
  } 
 else if(field == srcRegId1){
    DPRINTF(FaultInjection,"===SrcRegId1::process===");    
    dump();
    srcRegId_process(curInst,1);
    DPRINTF(FaultInjection,"===SrcRegId1::process===");
  }
 else if(field == destRegId){
    DPRINTF(FaultInjection,"===DestRegId::process===");    
    dump();
    destRegId_process(curInst);
    DPRINTF(FaultInjection,"===DestRegId::process===");
  }
 else if(field == condBranch){
    DPRINTF(FaultInjection,"===CondBranch::process===");    
    dump();
    condBranch_process(curInst);
    DPRINTF(FaultInjection,"===CondBranch::process===");
  }
 else if(field == indirBranch){
    DPRINTF(FaultInjection,"===IndirectBranch::process===");    
    dump();
    indirBranch_process(curInst);
    DPRINTF(FaultInjection,"===IndirectBranch::process===");
  }
 else if(field == machInst){
   DPRINTF(FaultInjection,"===MachInst::process()===");
   dump();
   machInst_process(curInst);
   DPRINTF(FaultInjection,"===MachInst::process()===");
 }
 else{
    if(DTRACE(FaultInjection)){
      cout<<"Really I don't know why, but you are here and this field IS NOT VALID"<<endl;
    }
 }   
  return curInst;
}

//Injecting a fault in fetch's primary fields
Base_PipelineFault::dynInstPtr Decode_RenameFault::process(dynInstPtr curInst)
{
  //This process only calls Fetch_Decode Fault process method.This because targeted fields are fully contained in that function.
  curInst = Fetch_DecodeFault::process(curInst);
  return curInst;
}

//Process for number of destination registers
void Decode_RenameFault::numDestRegs_process(StaticInstPtr &curInst)
{
  int destRegs = curInst->numDestRegs();
  destRegs = manifest(destRegs,getValue(),getValueType());
  curInst->set_numDestRegs(destRegs);
  check4reschedule();
}

//Process for branch flag
void Decode_RenameFault::branch_process(StaticInstPtr &curInst)
{
  bool isControl = curInst->isControl();
  isControl = manifest_bool(isControl,getValue(),getValueType());
  curInst->set_isControl(isControl);
  check4reschedule();
}

//Process for simple ALU flag
void Decode_RenameFault::simpleALU_process(StaticInstPtr &curInst)
{
  bool isSimple = curInst->isInteger();
  isSimple = manifest_bool(isSimple,getValue(),getValueType());
  curInst->set_isInteger(isSimple); 
  check4reschedule();
}

//Process for complex ALU flag
void Decode_RenameFault::complexALU_process(StaticInstPtr &curInst)
{
  bool isComplex = curInst->isFloating();
  isComplex = manifest_bool(isComplex,getValue(),getValueType());
  curInst->set_isFloating(isComplex); 
  check4reschedule();
}

//Process for memory referene flag
void Decode_RenameFault::memRef_process(StaticInstPtr &curInst)
{
  bool isMemRef = curInst->isMemRef();
  isMemRef = manifest_bool(isMemRef,getValue(),getValueType());
  curInst->set_isMemRef(isMemRef); 
  check4reschedule();
}

//Process for load flag
void Decode_RenameFault::load_process(StaticInstPtr &curInst)
{
  bool isLoad = curInst->isLoad();
  isLoad = manifest_bool(isLoad,getValue(),getValueType());
  curInst->set_isLoad(isLoad); 
  check4reschedule();
}

//Process for number of source register field 
void Decode_RenameFault::numSrcRegs_process(StaticInstPtr &curInst)
{
  int srcRegs = curInst->numSrcRegs();
  srcRegs = manifest(srcRegs,getValue(),getValueType());
  curInst->set_numSrcRegs(srcRegs); 
  check4reschedule();
}

//Process for source register id  
void Decode_RenameFault::srcRegId_process(StaticInstPtr &curInst,int id)
{
  TheISA::RegIndex index = curInst->srcRegIdx(id);
  index = manifest(index,getValue(),getValueType());
  curInst->set_srcRegIdx(index,id); 
  check4reschedule();
}

//Process for destination register id field
void Decode_RenameFault::destRegId_process(StaticInstPtr &curInst)
{
  TheISA::RegIndex index = curInst->destRegIdx(0);
  index = manifest(index,getValue(),getValueType());
  curInst->set_destRegIdx(index,0);
  check4reschedule();
}

//Process for conditional branch flag
void Decode_RenameFault::condBranch_process(StaticInstPtr &curInst)
{
  bool isCondCtrl = curInst->isCondCtrl();
  isCondCtrl = manifest_bool(isCondCtrl,getValue(),getValueType());
  curInst->set_isCondCtrl(isCondCtrl);
  check4reschedule();
}

//Process for indirect control inst flag
void Decode_RenameFault::indirBranch_process(StaticInstPtr &curInst)
{
   bool isIndirectCtrl = curInst->isIndirectCtrl();
   isIndirectCtrl = manifest_bool(isIndirectCtrl,getValue(),getValueType());
   curInst->set_isIndirectCtrl(isIndirectCtrl);
   check4reschedule();
}

//Process for machine instruction got from dynamic instruction
void Decode_RenameFault::machInst_process(StaticInstPtr &curInst)
{
 TheISA::ExtMachInst inst = curInst->getMachInst();
 inst = manifest(inst,getValue(),getValueType());
 curInst->set_machInst(inst); 
 check4reschedule();
}

//Class Decode Rename Fault Queue
Decode_RenameFault *Decode_RenameFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  Decode_RenameFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<Decode_RenameFault *> (head);
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
              if(exec_time == p->getTiming()&&
                curBundle == p->get_bundleNum() &&
                fieldType == p->get_fieldType()){ //correct time so intend to manifest
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
    p = reinterpret_cast<Decode_RenameFault*> (p->nxt);
  }
  return(NULL);

}

