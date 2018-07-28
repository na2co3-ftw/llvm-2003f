set(LLVM_TARGET_DEFINITIONS F2003f.td)

tablegen(LLVM F2003fGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM F2003fGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM F2003fGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM F2003fGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM F2003fGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM F2003fGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM F2003fGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM F2003fGenCallingConv.inc -gen-callingconv)
tablegen(LLVM F2003fGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(F2003fCommonTableGen)

add_llvm_target(F2003fCodeGen
  F2003fAsmPrinter.cpp
  F2003fFiMalkrz.cpp
  F2003fFrameLowering.cpp
  F2003fMachineFunctionInfo.cpp
  F2003fInstrInfo.cpp
  F2003fISelDAGToDAG.cpp
  F2003fISelLowering.cpp
  F2003fMCInstLower.cpp
  F2003fRegisterInfo.cpp
  F2003fSubtarget.cpp
  F2003fTargetMachine.cpp
  )

# add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
