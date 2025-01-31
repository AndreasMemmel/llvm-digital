//===-- DigitalTargetMachine.h - Define TargetMachine for Digital ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Digital specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETMACHINE_H
#define LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "DigitalTargetMachine.h"
#include "DigitalSubtarget.h"
#include "MCTargetDesc/DigitalMCTargetDesc.h"

namespace llvm {

class DigitalTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
	DigitalSubtarget Subtarget;
  
public:
  DigitalTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FeatureString, const TargetOptions &Options,
                     Optional<Reloc::Model> RM,
                     Optional<CodeModel::Model> CodeModel,
                     CodeGenOpt::Level OptLevel, bool JIT);
  ~DigitalTargetMachine() override;
  
  
  const DigitalSubtarget *getSubtargetImpl(const Function &F) const {
    return &Subtarget;
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  //const DigitalABIInfo &getABI() const { return ABI; }
};

} // End llvm namespace

#endif

