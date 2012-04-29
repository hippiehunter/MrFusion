#include "AST.h"

using MrFusion::Ast::Opcode;
using MrFusion::Ast::Register;
using MrFusion::Ast::SpecialOperandType;
using MrFusion::Ast::ExpressionKind;

using std::map;

map<Opcode, const char*> MrFusion::Ast::opcodeNames = {
  {Opcode::SET, "SET" },
  {Opcode::ADD, "ADD" },
  {Opcode::SUB, "SUB" },
  {Opcode::MUL, "MUL" },
  {Opcode::DIV, "DIV" },
  {Opcode::MOD, "MOD" },
  {Opcode::SHL, "SHL" },
  {Opcode::SHR, "SHR" },
  {Opcode::AND, "AND" },
  {Opcode::BOR, "BOR" },
  {Opcode::XOR, "XOR" },
  {Opcode::IFE, "IFE" },
  {Opcode::IFN, "IFN" },
  {Opcode::IFG, "IFG" },
  {Opcode::IFB, "IFB" },
  {Opcode::JSR, "JSR" }};
  
map<Register, const char*> MrFusion::Ast::registerNames = {
  {Register::A, "A" },
  {Register::B, "B" },
  {Register::C, "C" },
  {Register::I, "I" },
  {Register::J, "J" },
  {Register::X, "X" },
  {Register::Y, "Y" },
  {Register::Z, "Z" },
  {Register::PC, "PC" },
  {Register::SP, "SP" },
  {Register::OF, "OF" }};
  
map<SpecialOperandType, const char*> MrFusion::Ast::specialOperandTypeNames = {
  {SpecialOperandType::PEEK, "PEEK" },
  {SpecialOperandType::POP, "POP" },
  {SpecialOperandType::PUSH, "PUSH" }};
  
map<ExpressionKind, const char*> MrFusion::Ast::expressionKindNames = {
  {ExpressionKind::Plus, "+" },
  {ExpressionKind::Minus, "-" }};