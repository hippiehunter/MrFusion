#ifndef MRFUSION_PARSER_UTILS
#define MRFUSION_PARSER_UTILS

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
      template <typename Arg, typename Context> struct result { typedef MrFusion::Ast::Line* type; };
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
    
  }
}

#endif