//===-- DigitalTargetTransformInfo.h - Digital specific TTI ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file a TargetTransformInfo::Concept conforming object specific to the
// Digital target machine. It uses the target's detailed information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETTRANSFORMINFO_H

#include "Digital.h"
#include "DigitalSubtarget.h"
#include "DigitalTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Support/MathExtras.h"

namespace llvm {
class DigitalTTIImpl : public BasicTTIImplBase<DigitalTTIImpl> {
  typedef BasicTTIImplBase<DigitalTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const DigitalSubtarget *ST;
  const DigitalTargetLowering *TLI;

  const DigitalSubtarget *getST() const { return ST; }
  const DigitalTargetLowering *getTLI() const { return TLI; }

public:
  explicit DigitalTTIImpl(const DigitalTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  bool shouldBuildLookupTables() const { return false; }

  TargetTransformInfo::PopcntSupportKind getPopcntSupport(unsigned TyWidth) {
    if (TyWidth == 32)
      return TTI::PSK_FastHardware;
    return TTI::PSK_Software;
  }

  int getIntImmCost(const APInt &Imm, Type *Ty) {
    assert(Ty->isIntegerTy());
    if (Imm == 0)
      return TTI::TCC_Free;
    if (isInt<16>(Imm.getSExtValue()))
      return TTI::TCC_Basic;
    if (isInt<21>(Imm.getZExtValue()))
      return TTI::TCC_Basic;
    if (isInt<32>(Imm.getSExtValue())) {
      if ((Imm.getSExtValue() & 0xFFFF) == 0)
        return TTI::TCC_Basic;
      return 2 * TTI::TCC_Basic;
    }

    return 4 * TTI::TCC_Basic;
  }

  int getIntImmCost(unsigned Opc, unsigned Idx, const APInt &Imm, Type *Ty) {
    return getIntImmCost(Imm, Ty);
  }

  int getIntImmCost(Intrinsic::ID IID, unsigned Idx, const APInt &Imm,
                    Type *Ty) {
    return getIntImmCost(Imm, Ty);
  }

  unsigned getArithmeticInstrCost(
      unsigned Opcode, Type *Ty,
      TTI::OperandValueKind Opd1Info = TTI::OK_AnyValue,
      TTI::OperandValueKind Opd2Info = TTI::OK_AnyValue,
      TTI::OperandValueProperties Opd1PropInfo = TTI::OP_None,
      TTI::OperandValueProperties Opd2PropInfo = TTI::OP_None,
      ArrayRef<const Value *> Args = ArrayRef<const Value *>()) {
    int ISD = TLI->InstructionOpcodeToISD(Opcode);

    switch (ISD) {
    default:
      return BaseT::getArithmeticInstrCost(Opcode, Ty, Opd1Info, Opd2Info,
                                           Opd1PropInfo, Opd2PropInfo);
    case ISD::MUL:
    case ISD::SDIV:
    case ISD::UDIV:
    case ISD::UREM:
      // This increases the cost associated with multiplication and division
      // to 64 times what the baseline arithmetic cost is. The arithmetic
      // instruction cost was arbitrarily chosen to reduce the desirability
      // of emitting arithmetic instructions that are emulated in software.
      // TODO: Investigate the performance impact given specialized lowerings.
      return 64 * BaseT::getArithmeticInstrCost(Opcode, Ty, Opd1Info, Opd2Info,
                                                Opd1PropInfo, Opd2PropInfo);
    }
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETTRANSFORMINFO_H