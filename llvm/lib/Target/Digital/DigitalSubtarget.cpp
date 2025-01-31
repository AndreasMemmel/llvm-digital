//===-- DigitalSubtarget.cpp - Digital Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Digital specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "DigitalSubtarget.h"
#include "Digital.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "digital-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "DigitalGenSubtargetInfo.inc"

void DigitalSubtarget::anchor() { }

DigitalSubtarget &DigitalSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {

  StringRef CPUName = "generic";
  ParseSubtargetFeatures(CPUName, FS);

  return *this;
}

DigitalSubtarget::DigitalSubtarget(const Triple &TT, const std::string &CPU,
                                   const std::string &FS, const TargetMachine &TM)
    : DigitalGenSubtargetInfo(TT, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(CPU, FS)), 
      InstrInfo(), RegInfo(), TLInfo(TM, *this) {}