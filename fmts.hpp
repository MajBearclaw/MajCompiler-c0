#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
	template<>
	struct formatter<miniplc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::ErrNoError:
				name = "No error.";
				break;
			case miniplc0::ErrStreamError:
				name = "Stream error.";
				break;
			case miniplc0::ErrEOF:
				name = "EOF. This code is uncompleted.";
				break;
			case miniplc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case miniplc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			
			case miniplc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case miniplc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case miniplc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case miniplc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case miniplc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case miniplc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case miniplc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case miniplc0::ErrDuplicateDeclaration:
				name = "The variable|constant|parameter|function has been declared.";
				break;
			case miniplc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case miniplc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case miniplc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			
			case miniplc0::ErrNeedLeftBracket:
				name = "Need '(' here.";
				break;
			case miniplc0::ErrNeedRightBracket:
				name = "Need ')' here.";
				break;
			case miniplc0::ErrNeedLeftBrace:
				name = "Need '{' here.";
				break;
			case miniplc0::ErrNeedRightBrace:
				name = "Need '}' here.";
				break;
			case miniplc0::ErrNeedComma:
				name = "Need ',' here.";
				break;
			case miniplc0::ErrVoidInVar:
				name = "'void' cannot be used in variable-declaration.";
				break;
			case miniplc0::ErrConstFunc:
				name = "'const' cannot be used to specifify function.";
				break;
			case miniplc0::ErrFrontZero:
				name = "The decimal Integer should not have front-zero.";
				break;
			case miniplc0::ErrInvalidHexInteger:
				name = "The hex Integer should have value after '0X'|'0x'.";
				break;
			case miniplc0::ErrNeedTypeSpecifier:
				name = "variable-declaration should have type-specifier after 'const'.";
				break;
			case miniplc0::ErrUnimplemented:
				name = "Unimplemented grammer, e.g. 'char','double','do','for','switch','case',...";
				break;
			case miniplc0::ErrInvalidStatement:
				name = "This is an invalid Statement.";
				break;
			case miniplc0::ErrConstVoid:
				name = "Void cannot be const.";
				break;
			case miniplc0::ErrExpressionNeedValue:
				name = "Expression must have value instead of 'void'.";
				break;
			case miniplc0::ErrNeedMain:
				name = "Program must have function main().";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<miniplc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<miniplc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::NULL_TOKEN:
				name = "NullToken";
				break;
			case miniplc0::UNSIGNED_INTEGER:
				name = "UnsignedInteger";
				break;
			case miniplc0::IDENTIFIER:
				name = "Identifier";
				break;
			
			case miniplc0::CONST:
				name = "Const";
				break;
			case miniplc0::PRINT:
				name = "Print";
				break;
			case miniplc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case miniplc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case miniplc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case miniplc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case miniplc0::EQUAL_SIGN:
				name = "EqualSign";
				break;
			case miniplc0::SEMICOLON:
				name = "Semicolon";
				break;
			case miniplc0::LEFT_BRACKET:
				name = "LeftBracket";
				break;
			case miniplc0::RIGHT_BRACKET:
				name = "RightBracket";
				break;
			case miniplc0::LEFT_ANGLE_BRACKET:
				name = "LeftAngleBracket";
				break;
			case miniplc0::RIGHT_ANGLE_BRACKET:
				name = "RightAngleBracket";
				break;
			case miniplc0::LEFT_BRACE:
				name = "LeftBrace";
				break;
			case miniplc0::RIGHT_BRACE:
				name = "RightBrace";
				break;
			case miniplc0::LEFT_SQUARE_BRACKET:
				name = "LeftSquareBracket";
				break;
			case miniplc0::RIGHT_SQUARE_BRACKET:
				name = "RightSquareBracket";
				break;
			case miniplc0::EXCLAMATION:
				name = "Exclamation";
				break;
			case miniplc0::COMMA:
				name = "Comma";
				break;
			case miniplc0::SPECIFIER:
				name = "Specifier";
				break;
			case miniplc0::STRUCT:
				name = "Struct";
				break;
			case miniplc0::IF:
				name = "If";
				break;
			case miniplc0::ELSE:
				name = "Else";
				break;
			case miniplc0::LABELED:
				name = "Labeled";
				break;
			case miniplc0::WHILE:
				name = "While";
				break;
			case miniplc0::DO:
				name = "Do";
				break;
			case miniplc0::FOR:
				name = "For";
				break;
			case miniplc0::RETURN:
				name = "Return";
				break;
			case miniplc0::BREAK:
				name = "Break";
				break;
			case miniplc0::CONTINUE:
				name = "Continue";
				break;
			case miniplc0::SCAN:
				name = "Scan";
				break;
			case miniplc0::NOT_EQUAL:
				name = "NotEqual";
				break;
			case miniplc0::EQUAL_EQUAL:
				name = "EqualEqual";
				break;
			case miniplc0::LESS_OR_EQUAL:
				name = "LessOrEqual";
				break;
			case miniplc0::GREATER_OR_EQUAL:
				name = "GreaterOrEqual";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Operation> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Operation &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::NOP:
				name = "nop";
				break;
			case miniplc0::BIPUSH:
				name = "bipush";
				break;
			case miniplc0::IPUSH:
				name = "ipush";
				break;
			case miniplc0::POP:
				name = "pop";
				break;
			case miniplc0::POP2:
				name = "pop2";
				break;
			case miniplc0::POPN:
				name = "popN";
				break;
			case miniplc0::DUP:
				name = "dup";
				break;
			case miniplc0::DUP2:
				name = "dup2";
				break;
			case miniplc0::LOADC:
				name = "loadc";
				break;
			case miniplc0::LOADA:
				name = "loada";
				break;
			case miniplc0::NEW:
				name = "new";
				break;
			case miniplc0::SNEW:
				name = "snew";
				break;
			case miniplc0::ILOAD:
				name = "iload";
				break;
			case miniplc0::ALOAD:
				name = "aload";
				break;
			case miniplc0::IALOAD:
				name = "iaload";
				break;
			case miniplc0::AALOAD:
				name = "aaload";
				break;
			case miniplc0::ISTORE:
				name = "istore";
				break;
			case miniplc0::ASTORE:
				name = "astore";
				break;
			case miniplc0::IASTORE:
				name = "iastore";
				break;
			case miniplc0::AASTORE:
				name = "aastore";
				break;
			case miniplc0::IADD:
				name = "iadd";
				break;
			case miniplc0::ISUB:
				name = "isub";
				break;
			case miniplc0::IMUL:
				name = "imul";
				break;
			case miniplc0::IDIV:
				name = "idiv";
				break;
			case miniplc0::INEG:
				name = "ineg";
				break;
			case miniplc0::ICMP:
				name = "icmp";
				break;
			// case miniplc0::DADD:
			// 	name = "dadd";
			// 	break;
			// case miniplc0::DSUB:
			// 	name = "dsub";
			// 	break;
			// case miniplc0::DMUL:
			// 	name = "dmul";
			// 	break;
			// case miniplc0::DDIV:
			// 	name = "ddiv";
			// 	break;
			// case miniplc0::DNEG:
			// 	name = "dneg";
			// 	break;
			// case miniplc0::DCMP:
			// 	name = "dcmp";
			// 	break;
			// case miniplc0::I2D:
			// 	name = "i2d";
			// 	break;
			// case miniplc0::D2I:
			// 	name = "d2i";
			// 	break;
			// case miniplc0::I2C:
			// 	name = "i2c";
			// 	break;
			case miniplc0::JMP:
				name = "jmp";
				break;
			case miniplc0::JE:
				name = "je";
				break;
			case miniplc0::JNE:
				name = "jne";
				break;
			case miniplc0::JL:
				name = "jl";
				break;
			case miniplc0::JGE:
				name = "jge";
				break;
			case miniplc0::JG:
				name = "jg";
				break;
			case miniplc0::JLE:
				name = "jle";
				break;
			case miniplc0::CALL:
				name = "call";
				break;
			case miniplc0::RET:
				name = "ret";
				break;
			case miniplc0::IRET:
				name = "iret";
				break;
			// case miniplc0::ARET:
			// 	name = "aret";
			// 	break;
			case miniplc0::IPRINT:
				name = "iprint";
				break;
			case miniplc0::CPRINT:
				name = "cprint";
				break;
			// case miniplc0::SPRINT:
			// 	name = "sprint";
			// 	break;
			case miniplc0::PRINTL:
				name = "printl";
				break;
			case miniplc0::ISCAN:
				name = "iscan";
				break;
			case miniplc0::PCONSTANTS:
				name = ".constants:";
				break;
			case miniplc0::PSTART:
				name = ".start:";
				break;
			case miniplc0::PFUNCTION:
				name = ".functions:";
				break;
			case miniplc0::PFI:
				name = ".F";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
	template<>
	struct formatter<miniplc0::Instruction> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Instruction &p, FormatContext &ctx) {
			std::string name;
			switch (p.GetOperation())
			{
			case miniplc0::PCONSTANTS:
			case miniplc0::PSTART:
			case miniplc0::PFUNCTION:
				return format_to(ctx.out(), "{}", p.GetOperation());	//needn't params
			case miniplc0::NOP:
			case miniplc0::POP:
			case miniplc0::POP2:
			case miniplc0::DUP:
			case miniplc0::DUP2:
			case miniplc0::NEW:		// pop (int)count: new (count) slot
			case miniplc0::ILOAD:
			case miniplc0::ALOAD:
			case miniplc0::IALOAD:		// array
			case miniplc0::AALOAD:
			case miniplc0::ISTORE:
			case miniplc0::ASTORE:
			case miniplc0::IASTORE:	// array
			case miniplc0::AASTORE:
			case miniplc0::IADD:
			case miniplc0::ISUB:
			case miniplc0::IMUL:
			case miniplc0::IDIV:
			case miniplc0::INEG:		// a = -a
			case miniplc0::ICMP:		// pop r: pop l: push sign(l-r);  1|-1|0
			case miniplc0::RET:
			case miniplc0::IRET:
			// case miniplc0::ARET:
			case miniplc0::IPRINT:
			case miniplc0::CPRINT:
			// case miniplc0::APRINT:
			case miniplc0::PRINTL:
			case miniplc0::ISCAN:
				return format_to(ctx.out(), "{} {}", p.GetIndex(), p.GetOperation());	//needn't params
			case miniplc0::BIPUSH:
			case miniplc0::IPUSH:	
			case miniplc0::POPN:
			case miniplc0::LOADC:		// load constant_index(2)
			case miniplc0::SNEW:		// snew count(4)					
			case miniplc0::JMP:		
			case miniplc0::JE:
			case miniplc0::JNE:
			case miniplc0::JL:
			case miniplc0::JGE:
			case miniplc0::JG:
			case miniplc0::JLE:
			case miniplc0::CALL:		
				return format_to(ctx.out(), "{} {}", p.GetOperation(), p.GetX());
			///////////////////////////////////////////////
			case miniplc0::LOADA:		// loada level_diff(2), offset(4)
				return format_to(ctx.out(), "{} {} {}", p.GetOperation(), p.GetX(), p.GetY());
			/////// .F0:    .F1:    .F17:    ...
			case miniplc0::PFI:
				return format_to(ctx.out(), "{}{}:", p.GetOperation(), p.GetX() );
			}
			return format_to(ctx.out(), "nop");
		}
	};
}