//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2017 Daniel Liew
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
#ifndef JFS_CXX_FUZZING_BACKEND_CXX_PROGRAM_BUILDER_PASS_IMPL_H
#define JFS_CXX_FUZZING_BACKEND_CXX_PROGRAM_BUILDER_PASS_IMPL_H
#include "jfs/CXXFuzzingBackend/CXXProgram.h"
#include "jfs/CXXFuzzingBackend/CXXProgramBuilderPass.h"
#include "jfs/Core/JFSContext.h"
#include "jfs/Core/Z3ASTVisitor.h"
#include "jfs/Core/Z3Node.h"
#include "jfs/Core/Z3NodeMap.h"
namespace jfs {
namespace cxxfb {

class CXXProgramBuilderPassImpl : public jfs::core::Z3ASTVisitor {
private:
  jfs::core::JFSContext& ctx;
  std::shared_ptr<CXXProgram> program;
  std::shared_ptr<jfs::fuzzingCommon::FuzzingAnalysisInfo> info;
  CXXCodeBlockRef earlyExitBlock;
  CXXCodeBlockRef entryPointMainBlock;
  jfs::core::Z3SortMap<CXXTypeRef> sortToCXXTypeCache;

  jfs::core::Z3ASTMap<llvm::StringRef>
      exprToSymbolName; // References strings in `usedSymbols`.
  std::unordered_set<std::string> usedSymbols;
  llvm::StringRef entryPointFirstArgName;
  llvm::StringRef entryPointSecondArgName;

  CXXProgramBuilderPassImpl(
      std::shared_ptr<jfs::fuzzingCommon::FuzzingAnalysisInfo> info,
      jfs::core::JFSContext& ctx);

  void build(const jfs::core::Query& q);

  // Helpers for inserting SSA variables and types
  CXXTypeRef getOrInsertTy(jfs::core::Z3SortHandle sort);
  std::string getSanitizedVariableName(const std::string& name);
  llvm::StringRef insertSymbol(const std::string& symbolName);
  llvm::StringRef insertSSASymbolForExpr(jfs::core::Z3ASTHandle e,
                                         const std::string& symbolName);

  // Function for building various parts of the CXXProgram
  CXXFunctionDeclRef buildEntryPoint();
  void insertBufferSizeGuard(CXXCodeBlockRef cb);
  void insertFreeVariableConstruction(CXXCodeBlockRef cb);
  void insertConstantAssignments(CXXCodeBlockRef cb);
  void insertBranchForConstraint(jfs::core::Z3ASTHandle constraint);
  void insertFuzzingTarget(CXXCodeBlockRef cb);
  // Only let CXXProgramBuilderPass use the implementation.
  friend class CXXProgramBuilderPass;

  // Visitor and ConstantAssignment helper methods
  const char* getboolConstantStr(jfs::core::Z3AppHandle e) const;
  std::string getBitVectorConstantStr(jfs::core::Z3AppHandle e) const;
  std::string getFreshSymbol();
  void insertSSAStmt(jfs::core::Z3ASTHandle e, llvm::StringRef expr,
                     llvm::StringRef preferredSymbolName);
  void insertSSAStmt(jfs::core::Z3ASTHandle e, llvm::StringRef expr) {
    insertSSAStmt(e, expr, llvm::StringRef());
    return;
  }
  void doDFSPostOrderTraversal(jfs::core::Z3ASTHandle e);
  void visitIfNeccesary(jfs::core::Z3ASTHandle e);
  CXXCodeBlockRef getCurrentBlock() { return entryPointMainBlock; }
  llvm::StringRef getSymbolFor(jfs::core::Z3ASTHandle e) const;

  // Visitor methods

  // Overloaded operations
  void visitEqual(jfs::core::Z3AppHandle e) override;
  void visitDistinct(jfs::core::Z3AppHandle e) override;
  void visitIfThenElse(jfs::core::Z3AppHandle e) override;
  void visitImplies(jfs::core::Z3AppHandle e) override;
  void visitIff(jfs::core::Z3AppHandle e) override;

  // Boolean operations
  void visitAnd(jfs::core::Z3AppHandle e) override;
  void visitOr(jfs::core::Z3AppHandle e) override;
  void visitXor(jfs::core::Z3AppHandle e) override;
  void visitNot(jfs::core::Z3AppHandle e) override;

  // Arithmetic BitVector operations
  void visitBvNeg(jfs::core::Z3AppHandle e) override;
  void visitBvAdd(jfs::core::Z3AppHandle e) override;
  void visitBvSub(jfs::core::Z3AppHandle e) override;
  void visitBvMul(jfs::core::Z3AppHandle e) override;
  void visitBvSDiv(jfs::core::Z3AppHandle e) override;
  void visitBvUDiv(jfs::core::Z3AppHandle e) override;
  void visitBvSRem(jfs::core::Z3AppHandle e) override;
  void visitBvURem(jfs::core::Z3AppHandle e) override;
  void visitBvSMod(jfs::core::Z3AppHandle e) override;

  // Comparison BitVector operations
  void visitBvULE(jfs::core::Z3AppHandle e) override;
  void visitBvSLE(jfs::core::Z3AppHandle e) override;
  void visitBvUGE(jfs::core::Z3AppHandle e) override;
  void visitBvSGE(jfs::core::Z3AppHandle e) override;
  void visitBvULT(jfs::core::Z3AppHandle e) override;
  void visitBvSLT(jfs::core::Z3AppHandle e) override;
  void visitBvUGT(jfs::core::Z3AppHandle e) override;
  void visitBvSGT(jfs::core::Z3AppHandle e) override;

  // Bitwise BitVector operations
  void visitBvAnd(jfs::core::Z3AppHandle e) override;

  // Constants
  void visitBoolConstant(jfs::core::Z3AppHandle e) override;
  void visitBitVector(jfs::core::Z3AppHandle e) override;
};
}
}
#endif