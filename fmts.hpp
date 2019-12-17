#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
	template<>
	struct formatter<majc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case majc0::ErrNoError:
				name = "No error.";
				break;
			case majc0::ErrStreamError:
				name = "Stream error.";
				break;
			case majc0::ErrEOF:
				name = "EOF. This code is uncompleted.";
				break;
			case majc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case majc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			
			case majc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case majc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case majc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case majc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case majc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case majc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case majc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case majc0::ErrDuplicateDeclaration:
				name = "The variable or constant has been declared.";
				break;
			case majc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case majc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case majc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			
			case majc0::ErrNeedLeftBracket:
				name = "Need '(' here."
				break;
			case majc0::ErrNeedRightBracket:
				name = "Need ')' here."
				break;
			case majc0::ErrNeedLeftBrace:
				name = "Need '{' here."
				break;
			case majc0::ErrNeedRightBrace:
				name = "Need '}' here."
				break;
			case majc0::ErrVoidInVar:
				name = "'void' cannot be used in variable-declaration."
				break;
			case majc0::ErrFrontZero:
				name = "The decimal Integer should not have front-zero.";
				break;
			case majc0::ErrInvalidHexInteger:
				name = "The hex Integer should have value after '0X'|'0x'.";
				break;
			case majc0::ErrNeedTypeSpecifier:
				name = "variable-declaration should have type-specifier after 'const'.";
				break;
			case majc0::ErrUnimplemented:
				name = "Unimplemented grammer, e.g. 'char','double','do','for','switch','case',...";
				break;
			case majc0::ErrInvalidStatement:
				name = "It is not a valid statement.";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<majc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<majc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<majc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case majc0::NULL_TOKEN:
				name = "NullToken";
				break;
			case majc0::UNSIGNED_INTEGER:
				name = "UnsignedInteger";
				break;
			case majc0::IDENTIFIER:
				name = "Identifier";
				break;
			
			case majc0::CONST:
				name = "Const";
				break;
			case majc0::PRINT:
				name = "Print";
				break;
			case majc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case majc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case majc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case majc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case majc0::EQUAL_SIGN:
				name = "EqualSign";
				break;
			case majc0::SEMICOLON:
				name = "Semicolon";
				break;
			case majc0::LEFT_BRACKET:
				name = "LeftBracket";
				break;
			case majc0::RIGHT_BRACKET:
				name = "RightBracket";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
}

namespace fmt {
	template<>
	struct formatter<majc0::Operation> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::Operation &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case majc0::NOP:
				name = "nop";
				break;
			case majc0::BIPUSH:
				name = "bipush";
				break;
			case majc0::IPUSH:
				name = "ipush";
				break;
			case majc0::POP:
				name = "pop";
				break;
			case majc0::POP2:
				name = "pop2";
				break;
			case majc0::POPN:
				name = "popN";
				break;
			case majc0::DUP:
				name = "dup";
				break;
			case majc0::DUP2:
				name = "dup2";
				break;
			case majc0::LOADC:
				name = "loadc";
				break;
			case majc0::LOADA:
				name = "loada";
				break;
			case majc0::NEW:
				name = "new";
				break;
			case majc0::SNEW:
				name = "snew";
				break;
			case majc0::ILOAD:
				name = "iload";
				break;
			case majc0::ALOAD:
				name = "aload";
				break;
			case majc0::IALOAD:
				name = "iaload";
				break;
			case majc0::AALOAD:
				name = "aaload";
				break;
			case majc0::ISTORE:
				name = "istore";
				break;
			case majc0::ASTORE:
				name = "astore";
				break;
			case majc0::IASTORE:
				name = "iastore";
				break;
			case majc0::AASTORE:
				name = "aastore";
				break;
			case majc0::IADD:
				name = "iadd";
				break;
			case majc0::ISUB:
				name = "isub";
				break;
			case majc0::IMUL:
				name = "imul";
				break;
			case majc0::IDIV:
				name = "idiv";
				break;
			case majc0::INEG:
				name = "ineg";
				break;
			case majc0::ICMP:
				name = "icmp";
				break;
			// case majc0::DADD:
			// 	name = "dadd";
			// 	break;
			// case majc0::DSUB:
			// 	name = "dsub";
			// 	break;
			// case majc0::DMUL:
			// 	name = "dmul";
			// 	break;
			// case majc0::DDIV:
			// 	name = "ddiv";
			// 	break;
			// case majc0::DNEG:
			// 	name = "dneg";
			// 	break;
			// case majc0::DCMP:
			// 	name = "dcmp";
			// 	break;
			// case majc0::I2D:
			// 	name = "i2d";
			// 	break;
			// case majc0::D2I:
			// 	name = "d2i";
			// 	break;
			// case majc0::I2C:
			// 	name = "i2c";
			// 	break;
			case majc0::JMP:
				name = "jmp";
				break;
			case majc0::JE:
				name = "je";
				break;
			case majc0::JNE:
				name = "jne";
				break;
			case majc0::JL:
				name = "jl";
				break;
			case majc0::JGE:
				name = "jge";
				break;
			case majc0::JG:
				name = "jg";
				break;
			case majc0::JLE:
				name = "jle";
				break;
			case majc0::CALL:
				name = "call";
				break;
			case majc0::RET:
				name = "ret";
				break;
			case majc0::IRET:
				name = "iret";
				break;
			case majc0::ARET:
				name = "aret";
				break;
			case majc0::IPRINT:
				name = "iprint";
				break;
			case majc0::CPRINT:
				name = "cprint";
				break;
			// case majc0::SPRINT:
			// 	name = "sprint";
			// 	break;
			case majc0::PRINTL:
				name = "printl";
				break;
			case majc0::ISCAN:
				name = "iscan";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
	template<>
	struct formatter<majc0::Instruction> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const majc0::Instruction &p, FormatContext &ctx) {
			std::string name;
			switch (p.GetOperation())
			{
			
			case majc0::NOP:
			case majc0::POP:
			case majc0::POP2:
			case majc0::DUP:
			case majc0::DUP2:
			case majc0::NEW:		// pop (int)count: new (count) slot
			case majc0::ILOAD:
			case majc0::ALOAD:
			case majc0::IALOAD:		// array
			case majc0::AALOAD:
			case majc0::ISTORE:
			case majc0::ASTORE:
			case majc0::IASTORE:	// array
			case majc0::AASTORE:
			case majc0::IADD:
			case majc0::ISUB:
			case majc0::IMUL:
			case majc0::IDIV:
			case majc0::INEG:		// a = -a
			case majc0::ICMP:		// pop r: pop l: push sign(l-r);  1|-1|0
			case majc0::RET:
			case majc0::IREI:
			case majc0::AREI:
			case majc0::IPRINT:
			case majc0::CPRINT:
			case majc0::APRINT:
			case majc0::PRINTL:
			case majc0::ISCAN:
				return format_to(ctx.out(), "{}", p.GetOperation());	//needn't params
			case majc0::BIPUSH:
			case majc0::IPUSH:	
			case majc0::POPN:
			case majc0::LOADC:		// load constant_index(2)
			case majc0::SNEW:		// snew count(4)					
			case majc0::JMP:		
			case majc0::JE:
			case majc0::JNE:
			case majc0::JL:
			case majc0::JGE:
			case majc0::JG:
			case majc0::JLE:
			case majc0::CALL:		
				return format_to(ctx.out(), "{} {}", p.GetOperation(), p.GetX());
			///////////////////////////////////////////////
			case majc0::LOADA:		// loada level_diff(2), offset(4)
				return format_to(ctx.out(), "{} {} {}", p.GetOperation(), p.GetX(), p.GetY());
			}
			return format_to(ctx.out(), "nop");
		}
	};
}