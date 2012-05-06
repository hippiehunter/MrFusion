#ifndef MRFUSION_PARSER_UTILS
#define MRFUSION_PARSER_UTILS

#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/include/at.hpp>

#include "AST.h"

namespace MrFusion
{
  namespace ParserUtils
  {
    struct make_line_impl
    {
      template <typename Arg, typename Context>
      MrFusion::Ast::Line* operator()(Arg const & content, Context const& context) const
      {
	return context->makeLine(context->currentFile(), context->nextLineNumber(), content);
      }
      
      template <typename Context>
      MrFusion::Ast::Line* operator()(const MrFusion::Ast::Label* const & content, Context const& context) const
      {
	auto madeLine = context->makeLine(context->currentFile(), context->nextLineNumber(), content);
	content->line = madeLine;
	return madeLine;
      }
      template <typename Arg, typename Context> struct result { typedef MrFusion::Ast::Line* type; };
      template <typename Context> struct result<MrFusion::Ast::Label*, Context> { typedef MrFusion::Ast::Line* type; };
    };
    
    struct make_line_empty_impl
    {
      template <typename Context>
      MrFusion::Ast::Line* operator()(Context const& context) const
      {
	return context->makeLine(context->currentFile(), context->nextLineNumber());
      }
      template <typename Context> struct result { typedef MrFusion::Ast::Line* type; };
    };
    
    //attaches a statement as the target of a label
    struct attach_statement_impl
    {      
      template <typename Arg1, typename Arg2>
      const Arg1& operator()(Arg1 const& label, Arg2 const& statement) const
      {
	label->target = statement;
	return label;
      }
      template <typename Arg1, typename Arg2> struct result { typedef Arg1 type; };
    };
    
    struct make_label_impl
    {
      template <typename Arg1, typename Context>
      MrFusion::Ast::Label* operator()(Arg1 const& symbolName, Context const& context) const
      {
	return context->makeLabel(symbolName);
      }
      template <typename Arg1, typename Context> struct result { typedef MrFusion::Ast::Label* type; };
    };
    
    struct make_binary_instruction_impl
    {
      template <typename Opration, typename First, typename Second, typename Context>
      MrFusion::Ast::Instruction* operator()(Opration const& op, First const& first, Second const& second, Context const& context) const
      {
	return context->makeInstruction(op, first, second);
      }
      template <typename Opration, typename First, typename Second, typename Context> 
      struct result { typedef MrFusion::Ast::Instruction* type; };
    };
    
    struct make_unary_instruction_impl
    {
      template <typename Opration, typename First, typename Context>
      MrFusion::Ast::Instruction* operator()(Opration const& op, First const& first, Context const& context) const
      {
	return context->makeInstruction(op, first);
      }
      template <typename Opration, typename First, typename Context> 
      struct result { typedef MrFusion::Ast::Instruction* type; };
    };
    
    template<typename RetType>
    struct ProcExpr : public boost::static_visitor<RetType>
    {
      RetType operator()(uint16_t const& data) const
      {
	return MrFusion::Ast::LiteralOperand { data };
      }
      
      RetType operator()(std::string const& name) const
      {
	return MrFusion::Ast::LabelOperand { name };
      }
    };
    
    struct make_operand_impl
    {
      MrFusion::Ast::Operand operator()(MrFusion::Ast::Register const& reg) const
      {
	return MrFusion::Ast::RegisterOperand { reg };
      }
      
      MrFusion::Ast::Operand operator()(MrFusion::Ast::SpecialOperandType const& sOp) const
      {
	return MrFusion::Ast::SpecialOperand { sOp };
      }
      
      MrFusion::Ast::Operand operator()(boost::variant<uint16_t, std::string> const& expr) const
      {
	return boost::apply_visitor(ProcExpr<MrFusion::Ast::Operand>(), expr);
      }
      
      
      template <typename Arg1> struct result { typedef MrFusion::Ast::Operand type; };
    };
    
    struct make_deref_operand_impl
    {
      MrFusion::Ast::DerefOperand operator()(MrFusion::Ast::Register const& reg) const
      {
	return MrFusion::Ast::DerefOperand { MrFusion::Ast::RegisterOperand { reg } };
      }
      
      MrFusion::Ast::DerefOperand operator()(MrFusion::Ast::ExpressionOperand const& expOp) const
      {
	return MrFusion::Ast::DerefOperand { expOp };
      }
      
      MrFusion::Ast::DerefOperand operator()(boost::variant<uint16_t, std::string> const& expr) const
      {
	return MrFusion::Ast::DerefOperand { 
	  boost::apply_visitor(ProcExpr<typename MrFusion::Ast::DerefOperand::element_type>(), expr) };
      }
      
      
      template <typename Arg1> struct result { typedef MrFusion::Ast::DerefOperand type; };
    };
    
    struct make_compound_expr_impl
    {
      MrFusion::Ast::ExpressionOperand operator()(MrFusion::Ast::ExpressionKind const& kind,
						  boost::variant<uint16_t, std::string> const& left,
						  MrFusion::Ast::Register const& right) const
      {
	return MrFusion::Ast::ExpressionOperand
	{
	  boost::apply_visitor(ProcExpr<typename MrFusion::Ast::ExpressionOperand::element_type>(), left),
	  MrFusion::Ast::RegisterOperand { right },
	  kind
	};
      }
      
      
      template <typename Arg1, typename Arg2, typename Arg3>
      struct result { typedef MrFusion::Ast::ExpressionOperand type; };
    };
  }
}

#endif