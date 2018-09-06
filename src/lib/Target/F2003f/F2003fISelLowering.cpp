//===-- F2003fISelLowering.cpp - f2003 DAG Lowering Implementation  -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the F2003fTargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "F2003fISelLowering.h"
#include "F2003f.h"
#include "F2003fMachineFunctionInfo.h"
#include "F2003fRegisterInfo.h"
#include "F2003fSubtarget.h"
#include "F2003fTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "2003f-lower"

F2003fTargetLowering::F2003fTargetLowering(const TargetMachine &TM,
                                           const F2003fSubtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i32, &F2003f::RegRegClass);

  // Compute derived properties from the register classes
  computeRegisterProperties(STI.getRegisterInfo());

  // Provide all sorts of operation actions
  setStackPointerRegisterToSaveRestore(F2003f::F5);
  setBooleanContents(ZeroOrOneBooleanContent); // undifined
  setBooleanVectorContents(ZeroOrOneBooleanContent);

  for (auto VT : { MVT::i8, MVT::i16 }) {
    setLoadExtAction(ISD::ZEXTLOAD, MVT::i32, VT,  Expand);
  }

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1,    Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8,    Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16,   Expand);

  setOperationAction(ISD::MULHS,             MVT::i32,   Expand); // to SMUL_LOHI
  setOperationAction(ISD::MULHU,             MVT::i32,   Expand); // to UMUL_LOHI

  setOperationAction(ISD::ADDC,              MVT::i32,   Expand);
  setOperationAction(ISD::SUBC,              MVT::i32,   Expand);
  setOperationAction(ISD::ADDE,              MVT::i32,   Expand);
  setOperationAction(ISD::SUBE,              MVT::i32,   Expand);

  setOperationAction(ISD::SETCC,             MVT::i32,   Expand); // to SELECT_CC
  setOperationAction(ISD::SELECT,            MVT::i32,   Expand); // to SELECT_CC
  setOperationAction(ISD::SELECT_CC,         MVT::i32,   Custom);

  setOperationAction(ISD::BRCOND,            MVT::Other, Expand); // to BR_CC
  setOperationAction(ISD::BR_CC,             MVT::i32,   Custom);
  setOperationAction(ISD::BR_JT,             MVT::Other, Expand);

  setOperationAction(ISD::GlobalAddress,     MVT::i32,   Custom);
  setOperationAction(ISD::ExternalSymbol,    MVT::i32,   Custom);
  setOperationAction(ISD::BlockAddress,      MVT::i32,   Custom);
  setOperationAction(ISD::JumpTable,         MVT::i32,   Custom);

  // setMinFunctionAlignment(0);
  // setPrefFunctionAlignment(0);
}

SDValue F2003fTargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::SELECT_CC:      return LowerSELECT_CC(Op, DAG);
  case ISD::BR_CC:          return LowerBR_CC(Op, DAG);
  case ISD::GlobalAddress:  return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:   return LowerBlockAddress(Op, DAG);
  case ISD::ExternalSymbol: return LowerExternalSymbol(Op, DAG);
  case ISD::JumpTable:      return LowerJumpTable(Op, DAG);
  default:
    llvm_unreachable("unimplemented operand");
  }
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

// |          |          |          |12           CCInfo.getNextStackOffset()
// |0         |4         |8         |             ArgLocs[i].getLocMemOffset()
// +----------+----------+----------+-----------+
// | i32 arg1 | i32 arg2 | i32 arg3 |return addr| Stack
// +----------+----------+----------+-----------+
// |     f5+12|      f5+8|      f5+4|         f5| Address

#include "F2003fGenCallingConv.inc"

SDValue F2003fTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeArguments(Ins, CC_F2003f);

  unsigned NumBytes = CCInfo.getNextStackOffset() + 4;

  // Create frame index for the start of the first vararg value
  if (isVarArg) {
    llvm_unreachable("Vararg is unsupported");
  }

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      switch (RegVT.getSimpleVT().SimpleTy) {
      default:
        {
#ifndef NDEBUG
          errs() << "LowerFormalArguments Unhandled argument type: "
               << RegVT.getEVTString() << "\n";
#endif
          llvm_unreachable(nullptr);
        }
      case MVT::i32:
        unsigned VReg = RegInfo.createVirtualRegister(&F2003f::RegRegClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg);
        SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);

        InVals.push_back(ArgValue);
      }
    } else {
      // Sanity check
      assert(VA.isMemLoc());

      SDValue InVal;
      ISD::ArgFlagsTy Flags = Ins[i].Flags;

      unsigned offset = NumBytes - VA.getLocMemOffset() - VA.getLocVT().getSizeInBits()/8;

      if (Flags.isByVal()) {
        int FI = MFI.CreateFixedObject(Flags.getByValSize(),
                                       offset, true);
        InVal = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      } else {
        // Load the argument to a virtual register
        unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
        // Create the frame index object for this incoming parameter...
        int FI = MFI.CreateFixedObject(ObjSize, offset, true);

        // Create the SelectionDAG nodes corresponding to a load
        //from this parameter
        SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
        InVal = DAG.getLoad(
            VA.getLocVT(), dl, Chain, FIN,
            MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI));
      }

      InVals.push_back(InVal);
    }
  }

  return Chain;
}

SDValue
F2003fTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc &dl                             = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &isTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool isVarArg                         = CLI.IsVarArg;

  // 2003f target does not yet support tail call optimization.
  isTailCall = false;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_F2003f);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset() + 4;
  auto PtrVT = getPointerTy(DAG.getDataLayout());

  // Adjust the stack pointer for the new arguments...
  // These operations are automatically eliminated by the prolog/epilog pass
  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, dl);

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SmallVector<SDValue, 12> MemOpChains;
  SDValue StackPtr;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    SDValue Arg = OutVals[i];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
        break;
    }

    // Arguments that can be passed on register must be kept at RegsToPass
    // vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());

      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, dl, F2003f::F5, PtrVT);

      unsigned offset = NumBytes - VA.getLocMemOffset() - VA.getLocVT().getSizeInBits()/8;
      SDValue PtrOff =
          DAG.getNode(ISD::ADD, dl, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(offset, dl));

      SDValue MemOp;
      ISD::ArgFlagsTy Flags = Outs[i].Flags;

      if (Flags.isByVal()) {
        SDValue SizeNode = DAG.getConstant(Flags.getByValSize(), dl, MVT::i32);
        MemOp = DAG.getMemcpy(Chain, dl, PtrOff, Arg, SizeNode,
                              Flags.getByValAlign(),
                              /*isVolatile*/false,
                              /*AlwaysInline=*/true,
                              /*isTailCall=*/false,
                              MachinePointerInfo(),
                              MachinePointerInfo());
      } else {
        MemOp = DAG.getStore(Chain, dl, Arg, PtrOff, MachinePointerInfo());
      }

      MemOpChains.push_back(MemOp);
    }
  }

  // Transform all store nodes into one single node because all store nodes are
  // independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, MemOpChains);

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emitted instructions must be stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const F2003fRegisterInfo *TRI = Subtarget->getRegisterInfo();
  Ops.push_back(DAG.getRegisterMask(TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv)));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain = DAG.getNode(F2003fISD::FENXEO, dl, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, dl, PtrVT, true),
                             DAG.getConstant(0, dl, PtrVT, true), InFlag, dl);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, dl,
                         DAG, InVals);
}

bool
F2003fTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                     MachineFunction &MF,
                                     bool IsVarArg,
                                     const SmallVectorImpl<ISD::OutputArg> &Outs,
                                     LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_F2003f);
}

SDValue
F2003fTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                  bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  const SDLoc &dl, SelectionDAG &DAG) const {

  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_F2003f);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                             OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(F2003fISD::DOSNUD, dl, MVT::Other, RetOps);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue F2003fTargetLowering::LowerCallResult(
    SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_F2003f);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}


static F2003fCC::CondCode getF2003fCC(ISD::CondCode CC) {
  switch(CC) {
  case ISD::SETUGT: return F2003fCC::LLONYS;
  case ISD::SETULE: return F2003fCC::XTLONYS;
  case ISD::SETUGE: return F2003fCC::XOLONYS;
  case ISD::SETULT: return F2003fCC::XYLONYS;
  case ISD::SETEQ:  return F2003fCC::CLO;
  case ISD::SETNE:  return F2003fCC::NIV;
  case ISD::SETGT:  return F2003fCC::LLO;
  case ISD::SETLE:  return F2003fCC::XTLO;
  case ISD::SETGE:  return F2003fCC::XOLO;
  case ISD::SETLT:  return F2003fCC::XYLO;
  default:
    llvm_unreachable("unimplemented CondCode");
  }
}

SDValue F2003fTargetLowering::LowerSELECT_CC(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDValue LHS    = Op.getOperand(0);
  SDValue RHS    = Op.getOperand(1);
  SDValue TrueV  = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  EVT VT         = Op.getValueType();
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc dl   (Op);

  SDValue TargetCC = DAG.getTargetConstant(getF2003fCC(CC), dl, MVT::i8);

  SDValue Ops[] = {FalseV, TrueV, LHS, RHS, TargetCC};
  return DAG.getNode(F2003fISD::SELECT_CC, dl, VT, Ops);
}

SDValue F2003fTargetLowering::LowerBR_CC(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDValue Chain  = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS    = Op.getOperand(2);
  SDValue RHS    = Op.getOperand(3);
  SDValue Dest   = Op.getOperand(4);
  EVT VT         = Op.getValueType();
  SDLoc dl   (Op);

  SDValue TargetCC = DAG.getTargetConstant(getF2003fCC(CC), dl, MVT::i8);

  SDValue Ops[] = {Chain, Dest, LHS, RHS, TargetCC};
  return DAG.getNode(F2003fISD::BR_CC, dl, VT, Ops);
}

SDValue F2003fTargetLowering::LowerGlobalAddress(SDValue Op,
                                                 SelectionDAG &DAG) const {
  SDLoc dl(Op);
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();
  auto PtrVT = getPointerTy(DAG.getDataLayout());
  SDValue Result = DAG.getTargetGlobalAddress(GV, dl, PtrVT);

  Result = DAG.getNode(F2003fISD::Wrapper, SDLoc(Op), PtrVT, Result);

  if (Offset) {
    Result = DAG.getNode(ISD::ADD, dl, PtrVT, Result, DAG.getConstant(Offset, dl, PtrVT));
  }

  return Result;
}

SDValue F2003fTargetLowering::LowerExternalSymbol(SDValue Op,
                                                  SelectionDAG &DAG) const {
  SDLoc dl(Op);
  const char *Sym = cast<ExternalSymbolSDNode>(Op)->getSymbol();
  auto PtrVT = getPointerTy(DAG.getDataLayout());
  SDValue Result = DAG.getTargetExternalSymbol(Sym, PtrVT);

  return DAG.getNode(F2003fISD::Wrapper, dl, PtrVT, Result);
}

SDValue F2003fTargetLowering::LowerBlockAddress(SDValue Op,
                                                SelectionDAG &DAG) const {
  SDLoc dl(Op);
  auto PtrVT = getPointerTy(DAG.getDataLayout());
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  SDValue Result = DAG.getTargetBlockAddress(BA, PtrVT);

  return DAG.getNode(F2003fISD::Wrapper, dl, PtrVT, Result);
}

SDValue F2003fTargetLowering::LowerJumpTable(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDLoc dl(Op);
  JumpTableSDNode *JT = cast<JumpTableSDNode>(Op);
  auto PtrVT = getPointerTy(DAG.getDataLayout());
  SDValue Result = DAG.getTargetJumpTable(JT->getIndex(), PtrVT);

  return DAG.getNode(F2003fISD::Wrapper, dl, PtrVT, Result);
}


const char *F2003fTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((F2003fISD::NodeType)Opcode) {
  case F2003fISD::FIRST_NUMBER:       break;
  case F2003fISD::Wrapper:            return "F2003fISD::Wrapper";
  case F2003fISD::FENXEO:             return "F2003fISD::FENXEO";
  case F2003fISD::DOSNUD:             return "F2003fISD::DOSNUD";
  case F2003fISD::SELECT_CC:          return "F2003fISD::SELECT_CC";
  case F2003fISD::BR_CC:              return "F2003fISD::BR_CC";
  }
  return nullptr;
}
