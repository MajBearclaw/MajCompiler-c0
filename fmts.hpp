#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include <string>
#include <any>

namespace fmt {
	template<>
	struct formatter<cc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case cc0::ErrNoError:
				name = "No error.";
				break;
			case cc0::ErrStreamError:
				name = "Stream error.";
				break;
			case cc0::ErrEOF:
				name = "EOF. This code is uncompleted.";
				break;
			case cc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case cc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			case cc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case cc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case cc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case cc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case cc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case cc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case cc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case cc0::ErrDuplicateDeclaration:
				name = "The variable|constant|parameter|function has been declared.";
				break;
			case cc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case cc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case cc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			case cc0::ErrNeedLeftBracket:
				name = "Need '(' here.";
				break;
			case cc0::ErrNeedRightBracket:
				name = "Need ')' here.";
				break;
			case cc0::ErrNeedLeftBrace:
				name = "Need Left Brace here.";
				break;
			case cc0::ErrNeedRightBrace:
				name = "Need Right Brace here.";
				break;
			case cc0::ErrNeedComma:
				name = "Need ',' here.";
				break;
			case cc0::ErrVoidInVar:
				name = "'void' cannot be used in variable-declaration.";
				break;
			case cc0::ErrConstFunc:
				name = "'const' cannot be used to specifify function.";
				break;
			case cc0::ErrFrontZero:
				name = "The decimal Integer should not have front-zero.";
				break;
			case cc0::ErrInvalidHexInteger:
				name = "The hex Integer should have value after '0X'|'0x'.";
				break;
			case cc0::ErrNeedTypeSpecifier:
				name = "variable-declaration should have type-specifier after 'const'.";
				break;
			case cc0::ErrUnimplemented:
				name = "Unimplemented grammer, e.g. 'char','double','do','for','switch','case',...";
				break;
			case cc0::ErrInvalidStatement:
				name = "This is an invalid Statement.";
				break;
			case cc0::ErrConstVoid:
				name = "Void cannot be const.";
				break;
			case cc0::ErrExpressionNeedValue:
				name = "Expression must have value instead of 'void'.";
				break;
			case cc0::ErrNeedMain:
				name = "Program must have function main().";
				break;
			case cc0::ErrNeedReturn:
				name = "Function need return statement.";
				break;
			case cc0::ErrUnknown:
				name = "unknown error.";
				break;
			default:
				name = "unknown error.";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<cc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<cc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<cc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case cc0::NULL_TOKEN:
				name = "NullToken";
				break;
			case cc0::UNSIGNED_INTEGER:
				name = "UnsignedInteger";
				break;
			case cc0::IDENTIFIER:
				name = "Identifier";
				break;
			case cc0::CONST:
				name = "Const";
				break;
			case cc0::PRINT:
				name = "Print";
				break;
			case cc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case cc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case cc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case cc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case cc0::EQUAL_SIGN:
				name = "EqualSign";
				break;
			case cc0::SEMICOLON:
				name = "Semicolon";
				break;
			case cc0::LEFT_BRACKET:
				name = "LeftBracket";
				break;
			case cc0::RIGHT_BRACKET:
				name = "RightBracket";
				break;
			case cc0::LEFT_ANGLE_BRACKET:
				name = "LeftAngleBracket";
				break;
			case cc0::RIGHT_ANGLE_BRACKET:
				name = "RightAngleBracket";
				break;
			case cc0::LEFT_BRACE:
				name = "LeftBrace";
				break;
			case cc0::RIGHT_BRACE:
				name = "RightBrace";
				break;
			case cc0::LEFT_SQUARE_BRACKET:
				name = "LeftSquareBracket";
				break;
			case cc0::RIGHT_SQUARE_BRACKET:
				name = "RightSquareBracket";
				break;
			case cc0::EXCLAMATION:
				name = "Exclamation";
				break;
			case cc0::COMMA:
				name = "Comma";
				break;
			case cc0::SPECIFIER:
				name = "Specifier";
				break;
			case cc0::STRUCT:
				name = "Struct";
				break;
			case cc0::IF:
				name = "If";
				break;
			case cc0::ELSE:
				name = "Else";
				break;
			case cc0::LABELED:
				name = "Labeled";
				break;
			case cc0::WHILE:
				name = "While";
				break;
			case cc0::DO:
				name = "Do";
				break;
			case cc0::FOR:
				name = "For";
				break;
			case cc0::RETURN:
				name = "Return";
				break;
			case cc0::BREAK:
				name = "Break";
				break;
			case cc0::CONTINUE:
				name = "Continue";
				break;
			case cc0::SCAN:
				name = "Scan";
				break;
			case cc0::NOT_EQUAL:
				name = "NotEqual";
				break;
			case cc0::EQUAL_EQUAL:
				name = "EqualEqual";
				break;
			case cc0::LESS_OR_EQUAL:
				name = "LessOrEqual";
				break;
			case cc0::GREATER_OR_EQUAL:
				name = "GreaterOrEqual";
				break;
			default:
				name = "unknowntoken";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
}

namespace fmt {
	template<>
	struct formatter<cc0::Operation> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::Operation &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case cc0::NOP:
				name = "nop";
				break;
			case cc0::BIPUSH:
				name = "bipush";
				break;
			case cc0::IPUSH:
				name = "ipush";
				break;
			case cc0::POP:
				name = "pop";
				break;
			case cc0::POP2:
				name = "pop2";
				break;
			case cc0::POPN:
				name = "popN";
				break;
			case cc0::DUP:
				name = "dup";
				break;
			case cc0::DUP2:
				name = "dup2";
				break;
			case cc0::LOADC:
				name = "loadc";
				break;
			case cc0::LOADA:
				name = "loada";
				break;
			case cc0::NEW:
				name = "new";
				break;
			case cc0::SNEW:
				name = "snew";
				break;
			case cc0::ILOAD:
				name = "iload";
				break;
			case cc0::ALOAD:
				name = "aload";
				break;
			case cc0::IALOAD:
				name = "iaload";
				break;
			case cc0::AALOAD:
				name = "aaload";
				break;
			case cc0::ISTORE:
				name = "istore";
				break;
			case cc0::ASTORE:
				name = "astore";
				break;
			case cc0::IASTORE:
				name = "iastore";
				break;
			case cc0::AASTORE:
				name = "aastore";
				break;
			case cc0::IADD:
				name = "iadd";
				break;
			case cc0::ISUB:
				name = "isub";
				break;
			case cc0::IMUL:
				name = "imul";
				break;
			case cc0::IDIV:
				name = "idiv";
				break;
			case cc0::INEG:
				name = "ineg";
				break;
			case cc0::ICMP:
				name = "icmp";
				break;
			// case cc0::DADD:
			// 	name = "dadd";
			// 	break;
			// case cc0::DSUB:
			// 	name = "dsub";
			// 	break;
			// case cc0::DMUL:
			// 	name = "dmul";
			// 	break;
			// case cc0::DDIV:
			// 	name = "ddiv";
			// 	break;
			// case cc0::DNEG:
			// 	name = "dneg";
			// 	break;
			// case cc0::DCMP:
			// 	name = "dcmp";
			// 	break;
			// case cc0::I2D:
			// 	name = "i2d";
			// 	break;
			// case cc0::D2I:
			// 	name = "d2i";
			// 	break;
			// case cc0::I2C:
			// 	name = "i2c";
			// 	break;
			case cc0::JMP:
				name = "jmp";
				break;
			case cc0::JE:
				name = "je";
				break;
			case cc0::JNE:
				name = "jne";
				break;
			case cc0::JL:
				name = "jl";
				break;
			case cc0::JGE:
				name = "jge";
				break;
			case cc0::JG:
				name = "jg";
				break;
			case cc0::JLE:
				name = "jle";
				break;
			case cc0::CALL:
				name = "call";
				break;
			case cc0::RET:
				name = "ret";
				break;
			case cc0::IRET:
				name = "iret";
				break;
			// case cc0::ARET:
			// 	name = "aret";
			// 	break;
			case cc0::IPRINT:
				name = "iprint";
				break;
			case cc0::CPRINT:
				name = "cprint";
				break;
			// case cc0::SPRINT:
			// 	name = "sprint";
			// 	break;
			case cc0::PRINTL:
				name = "printl";
				break;
			case cc0::ISCAN:
				name = "iscan";
				break;
			case cc0::PCONSTANTS:
				name = ".constants:";
				break;
			case cc0::PSTART:
				name = ".start:";
				break;
			case cc0::PFUNCTION:
				name = ".functions:";
				break;
			case cc0::PFI:
				name = ".F";
				break;
			case cc0::SAVECONST:
				name = "saveconst";
				break;
			case cc0::SAVEFUNCTION:
				name = "savefunction";
				break;
			default:
				name = "unknown instruction";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
	template<>
	struct formatter<cc0::Instruction> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const cc0::Instruction &p, FormatContext &ctx) {
			std::string name;
			switch (p.GetOperation())
			{
			// 保存常量表
			case cc0::SAVECONST:
				return format_to(ctx.out(), "{} S \"{}\"", p.GetIndex(), p.GetConstType() );
			// .F0:    .F1:    .F17:    ...
			case cc0::PFI:
				return format_to(ctx.out(), ".F{}:", p.GetX() );
			case cc0::PCONSTANTS:
			case cc0::PSTART:
			case cc0::PFUNCTION:
				return format_to(ctx.out(), "{}", p.GetOperation() );
			case cc0::SAVEFUNCTION:
				return format_to(ctx.out(), "{} {} {} 1", p.GetIndex(), p.GetIndex(), p.GetX());
			case cc0::NOP:
			case cc0::POP:
			case cc0::POP2:
			case cc0::DUP:
			case cc0::DUP2:
			case cc0::NEW:		// pop (int)count: new (count) slot
			case cc0::ILOAD:
			case cc0::ALOAD:
			case cc0::IALOAD:		// array
			case cc0::AALOAD:
			case cc0::ISTORE:
			case cc0::ASTORE:
			case cc0::IASTORE:	// array
			case cc0::AASTORE:
			case cc0::IADD:
			case cc0::ISUB:
			case cc0::IMUL:
			case cc0::IDIV:
			case cc0::INEG:		// a = -a
			case cc0::ICMP:		// pop r: pop l: push sign(l-r);  1|-1|0
			case cc0::RET:
			case cc0::IRET:
			// case cc0::ARET:
			case cc0::IPRINT:
			case cc0::CPRINT:
			// case cc0::APRINT:
			case cc0::PRINTL:
			case cc0::ISCAN:
				return format_to(ctx.out(), "{} {}", p.GetIndex(), p.GetOperation());	//needn't params
			case cc0::BIPUSH:
			case cc0::IPUSH:	
			case cc0::POPN:
			case cc0::LOADC:		// load constant_index(2)
			case cc0::SNEW:		// snew count(4)					
			case cc0::JMP:		
			case cc0::JE:
			case cc0::JNE:
			case cc0::JL:
			case cc0::JGE:
			case cc0::JG:
			case cc0::JLE:
			case cc0::CALL:		
				return format_to(ctx.out(), "{} {} {}", p.GetIndex(), p.GetOperation(), p.GetX());
			case cc0::LOADA:		// loada level_diff(2), offset(4)
				return format_to(ctx.out(), "{} {} {} {}", p.GetIndex(), p.GetOperation(), p.GetX(), p.GetY());
			}
			return format_to(ctx.out(), "nop");
		}
	};
}