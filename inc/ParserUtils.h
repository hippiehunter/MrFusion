#ifndef MRFUSION_PARSER_UTILS
#define MRFUSION_PARSER_UTILS

#include "AST.h"

namespace MrFusion
{
  namespace ParserUtils
  {
    struct make_line_impl
    {
      MrFusion::Ast::GlobalContext* _context;
      make_line_impl(MrFusion::Ast::GlobalContext* context)
      {
	_context = context;
      }
      
      template <typename Sig>
      struct result;
      
      template <typename This, typename Arg>
      struct result<This(Arg const &)>
      {
	typedef MrFusion::Ast::Line* type;
      };
      template <typename Arg>
      MrFusion::Ast::Line* operator()(Arg const & content)
      {
	return _context->makeLine(_context->currentFile(), _context->nextLineNumber(), content);
      }
    };
    
    struct make_line_empty_impl
    {
      MrFusion::Ast::GlobalContext* _context;
      make_line_empty_impl(MrFusion::Ast::GlobalContext* context)
      {
	_context = context;
      }
      
      template <typename Sig>
      struct result;
      
      template <typename This>
      struct result<This()>
      {
	typedef MrFusion::Ast::Line* type;
      };
      MrFusion::Ast::Line* operator()()
      {
	return _context->makeLine(_context->currentFile(), _context->nextLineNumber());
      }
    };
    
    //attaches a statement as the target of a label
    struct attach_statement_impl
    {      
      template <typename Sig>
      struct result;
      
      template <typename This, typename Arg1, typename Arg2>
      struct result<This(Arg1 const &, Arg2 const&)>
      {
	typedef void type;
      };
      template <typename Arg1, typename Arg2>
      void operator()(Arg1 const& label, Arg2 const& statement)
      {
	label->target = statement;
      }
    };
    
  }
}

#endif