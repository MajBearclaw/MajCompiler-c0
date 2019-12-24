#include "analyser.h"

#include <climits>
#include <stack>

namespace miniplc0 {
	
	// 当前层级，每多一层嵌套{}, ++;
	// 函数声明不用++, 只有遇到函数参数之后的{, 才++
	static int32_t current_level = 0;
	// 每调用一个函数, pop (func_current_level)
	// 当前函数下标(函数表的大小)  .F0的0, .F1的1
	static int32_t current_func_index = 0;
	// 下述表达式只在 函数声明 开始有效
	// func_current_level := current_level+1
	static int32_t current_func_level = 0;
	// 指令下标 from 0 ~ n-1
	static int32_t current_instruction_index = 0;
	// 变量声明的下标
	static int32_t current_var_index = 0;
	
	// static std::stack <int32_t> func_level_st ;
	static std::stack <int32_t> instruction_index_st ;
	
	std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseProgram();
		if (err.has_value())
			return std::make_pair(std::vector<Instruction>(), err);
		else
			return std::make_pair(_instructions, std::optional<CompilationError>());
	}

	// <C0-program> ::= {<variable-declaration>}{<function-definition>}
	std::optional<CompilationError> Analyser::analyseProgram() {
		_instructions.emplace_back(0, Operation::PSTART, 0, 0);
		auto err = analyseVariableDeclarationMulti();
		if(err.has_value())
			return err;
		err = analyseFunctionDefinitionMulti();
		if(err.has_value())  return err;
		bool inclmain = false;
		for (int i=_functionsTable.size()-1; i>=0;i--){
			if(_functionsTable[i].getFuncName()=="main"){
				inclmain = true;
				break;
			}
		}
		if ( ! inclmain)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedMain);
		return {};
	}

	// {<variable-declaration>}
	// <variable-declaration> ::= [<const-qualifier>]<type-specifier><init-declarator-list>';'
	std::optional<CompilationError> Analyser::analyseVariableDeclarationMulti() {
		while (true){
			auto next = nextToken();
			auto isConst = 0;
			if (!next.has_value()) return {};
			// [<const-qualifier>]
			else if (next.value().GetType() == TokenType::CONST ){
				isConst = 1;
				next = nextToken();
			}
			// <type-specifier>
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			else if(next.value().GetType() == TokenType::SPECIFIER){
				std::string str = next.value().GetValueString();
				if(str == "int"){
					///
				}else if( str == "char" || str == "double" ){
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrUnimplemented);
				}else if(str == "void"){
					if (isConst)
						return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstVoid);
					unreadToken();
					return {};
				}else 
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			}
			// 不是类型修饰符，因此不是变量声明
			else{
				if (isConst)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
				unreadToken();
				return {};
			}
			
			// 检查是否是函数, <identifier>'('
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() == TokenType::LEFT_BRACKET){
				if (isConst)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstFunc);
				unreadToken();	// unrd '('
				unreadToken();	// unrd <identifier>
				unreadToken();	// unrd <type-specifier>
				return {};
			}
			unreadToken();	// unrd '('
			unreadToken();	// unrd <identifier>

			// <init-declarator-list>
			analyseInitDeclaratorList(isConst);
			// ';'
			next = nextToken();
			if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		return {};
	}

	// <init-declarator-list> ::= <init-declarator>{','<init-declarator>}
	std::optional<CompilationError> Analyser::analyseInitDeclaratorList(int32_t isConst) {
		auto err = analyseInitDeclarator(isConst);
		if (err.has_value()) return err;
		current_var_index++;
		while (true){
			// ','
			auto next = nextToken();
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() == TokenType::COMMA){
				err = analyseInitDeclarator(isConst);
				if (err.has_value()) return err;
				current_var_index++;
			}else {
				unreadToken();
				return {};
			}
		}
		return {};
	}

	// <init-declarator> ::= <identifier>[<initializer>]
	// <initializer> ::= '='<expression>   
	std::optional<CompilationError> Analyser::analyseInitDeclarator(int32_t isConst) {
		// <identifier>
		auto next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		if (isDeclaredSameLevel(next.value().GetValueString(), current_level))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
		C0Var tmpvar(next.value().GetValueString(), "int", current_level, current_var_index);
		addGlobalVariable(tmpvar);		
		// [<initializer>]
		// 		[ '='<expression>  ]
		next = nextToken();
		if( ! next.has_value() )
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		else if( next.value().GetType() == TokenType::EQUAL_SIGN){
			// <expression>
			auto err = analyseExpression();
			if (err.has_value())
				return err;
			tmpvar.setInitialized();
		}
		else {
			if (isConst)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstantNeedValue);
			_instructions.emplace_back(current_instruction_index++, Operation::SNEW, 1, 0);
			unreadToken();
		}
		if (isConst)
			tmpvar.setConst();
		return {};
	}

	// <expression> ::= <additive-expression>
	std::optional<CompilationError> Analyser::analyseExpression() {
		auto err = analyseAdditiveExpression();
		if (err.has_value()) return err;
		return {};
	}

	// <additive-expression> ::= <multiplicative-expression>{<additive-operator><multiplicative-expression>}
	std::optional<CompilationError> Analyser::analyseAdditiveExpression() {
		auto prefix = 1; // 1 => '+'; -1 => '-'
		// <multiplicative-expression>
		auto err = analyseMultiplicativeExpression();
		if (err.has_value())
			return err;
		// {...}
		while (true){
			// <additive-operator>
			auto next = nextToken();
			if ( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if ( next.value().GetType() == TokenType::PLUS_SIGN) {
				prefix = 1;
			}else if (next.value().GetType() == TokenType::MINUS_SIGN) {
				prefix = -1;
			}else {
				unreadToken();
				return {};
			}
			// <multiplicative-expression>
			err = analyseMultiplicativeExpression();
			if (err.has_value()) return err;
			if (prefix == -1)
				_instructions.emplace_back(current_instruction_index++, Operation::ISUB, 0, 0);
			else
				_instructions.emplace_back(current_instruction_index++, Operation::IADD, 0, 0);
		}
		return {};
	}

	// <multiplicative-expression> ::= 
	// 		<cast-expression>{<multiplicative-operator><cast-expression>}
	std::optional<CompilationError> Analyser::analyseMultiplicativeExpression() {
		// <cast-expression>
		auto err = analyseCastExpression();
		if(err.has_value())
			return err;
		auto prefixMul = 1; // 1 => '*'; -1 => '/'
		// {...}
		while (true) {
			// <multiplicative-operator>
			auto next = nextToken();
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() == TokenType::MULTIPLICATION_SIGN){
				//prefixMul = 1;
			}else if (next.value().GetType() == TokenType::DIVISION_SIGN){
				prefixMul = -1;
			}else {
				unreadToken();
				break;
			}
			// <cast-expression>
			err = analyseCastExpression();
			if(err.has_value())
				return err;
			if (prefixMul == -1)
				_instructions.emplace_back(current_instruction_index++, Operation::IDIV, 0, 0);
			else 
				_instructions.emplace_back(current_instruction_index++, Operation::IMUL, 0, 0);
		}
		return {};
	}

	// <cast-expression>
	// 			{'('<type-specifier>')'}<unary-expression>
	std::optional<CompilationError> Analyser::analyseCastExpression() {
		/* 未实现类型转换, 直接一元表达式
		auto next = nextToken();
		// '('
		if( ! next.has_value() ){
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		}else if( next.value().GetType() == TokenType::LEFT_BRACKET){
			
		}else {
			return 
		}
		*/
		// <unary-expression>
		auto err = analyseUnaryExpression();
		if (err.has_value())
			return err;
		return {};
	}

	// <unary-expression>
	// 		[<unary-operator>]<primary-expression>
	std::optional<CompilationError> Analyser::analyseUnaryExpression() {
		// [<unary-operator>]
		// 		'+'|'-'
		auto next = nextToken();
		auto prefix = 1; // 1 == '+'; -1 == '-'
		if( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		else if (next.value().GetType() == TokenType::PLUS_SIGN){
			// prefix = 1;
		}else if (next.value().GetType() == TokenType::MINUS_SIGN){
			prefix = -1;
		}else {
			unreadToken();
		}
		// <primary-expression>
		auto err = analysePrimaryExpression();
		if (err.has_value())
			return err;
		if (prefix == -1)
			_instructions.emplace_back(current_instruction_index++, Operation::INEG, 0, 0);
		return {};
	}

	// <primary-expression>
	//		 '('<expression>')' 
    // 		|<identifier>
    // 		|<integer-literal>
    // 		|<function-call>
	std::optional<CompilationError> Analyser::analysePrimaryExpression() {
		auto next = nextToken();
		if( ! next.has_value() )
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		// '('<expression>')'
		else if (next.value().GetType() == TokenType::LEFT_BRACKET){
			auto err = analyseExpression();
			if (err.has_value()) return err;
			next = nextToken();
			if ( ! next.has_value()  || next.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		}
		// |<identifier>
		// |<function-call>
		else if (next.value().GetType() == TokenType::IDENTIFIER ) {
			auto identiName = next.value().GetValueString();
			if (isVariable(identiName, current_func_level)){
				C0Var * ptmpvar = getVar(identiName, current_func_level);
				_instructions.emplace_back(current_instruction_index++, Operation::LOADA, !(ptmpvar->getLevel()), ptmpvar->getOffset());
				_instructions.emplace_back(current_instruction_index++, Operation::ILOAD, 0, 0);
			}else if (isFunction(identiName)){
				C0Function * pfunc = getFunc(identiName);
				// 检查函数返回值
				if(pfunc->getRetType() == "void")
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrExpressionNeedValue);
				unreadToken();
				auto err = analyseFunctionCall();
				if (err.has_value()) return err;
			}
			else
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
		}
		// |<integer-literal>
		else if (next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
			_instructions.emplace_back(current_instruction_index++, Operation::IPUSH, std::any_cast<std::int32_t>(next.value().GetValue()), 0);
		}
		else 
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
		return {};
	}

	// {<function-definition>}
	// <function-definition> ::= 
    // 		<type-specifier><identifier><parameter-clause><compound-statement>
	std::optional<CompilationError> Analyser::analyseFunctionDefinitionMulti(){
		while (true) {
			auto next = nextToken();
			std::string tmptype;
			if ( ! next.has_value())
				return {};
			else if (next.value().GetType() != TokenType::SPECIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			else if (next.value().GetValueString() == "void")
				tmptype = "void";
			else if (next.value().GetValueString() == "int")
				tmptype = "int";
			else 
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			// <identifier>
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
			if (isDeclaredSameLevel(next.value().GetValueString(), 0))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
			C0Function tmpfunc(next.value().GetValueString(), tmptype, current_func_index);
			addFunction(tmpfunc);
			// <parameter-clause>
			auto err = analyseParameterClause();
			if (err.has_value()) return err;
			// 变量声明的下标从参数表长度开始
			current_var_index = tmpfunc.getParamsNum();
			// 符号表中的层级+1，函数层级对应+1，符号表层级的递增在analyseCompoundStatement()函数中完成
			current_func_level++;
			// <compound-statement>
			err = analyseCompoundStatement();
			if (err.has_value()) return err;
			current_func_index++;
		}
		return {};
	}

	// <parameter-clause>
	// 		'(' [<parameter-declaration-list>] ')'
	std::optional<CompilationError> Analyser::analyseParameterClause(){
		auto next = nextToken();
		if( ! next.has_value() )
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		// '('
		else if (next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		// read ')' 
		next = nextToken();
		if ( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		else if (next.value().GetType() == TokenType::RIGHT_BRACKET)
			return {};
		else 
			unreadToken();
		// <parameter-declaration-list>
		auto err = analyseParameterDeclarationList();
		if (err.has_value()) return err;
		// ')'
		next = nextToken();
		if ( ! next.has_value()  || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		return {};
	}

	// <parameter-declaration-list> ::= 
    // 		<parameter-declaration>{','<parameter-declaration>}
	std::optional<CompilationError> Analyser::analyseParameterDeclarationList(){
		auto err = analyseParameterDeclaration();
		if (err.has_value()) return err;
		current_var_index++;
		while (true){
			// ','
			auto next = nextToken();
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() == TokenType::COMMA){
				err = analyseParameterDeclaration();
				if (err.has_value()) return err;
				current_var_index++;
			}else {
				unreadToken();
				return {};
			}
		}
		return {};
	}

	// <parameter-declaration>
	// 		[<const-qualifier>]<type-specifier><identifier>
	std::optional<CompilationError> Analyser::analyseParameterDeclaration(){
		auto next = nextToken();
		auto isConst = 0;
		if ( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		// [<const-qualifier>]
		else if (next.value().GetType() == TokenType::CONST ){
			isConst = 1;
			next = nextToken();
		}
		// <type-specifier>
		if( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
		else if(next.value().GetType() == TokenType::SPECIFIER){
			std::string str = next.value().GetValueString();
			if(str == "int"){
				///
			}else if( str == "char" || str == "double" ){
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrUnimplemented);
			}else if(str == "void"){
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidInVar);
			}else 
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
		}else
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
		// <identifier>
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		std::string tmpParaName;
		if (isDeclaredSameLevel(tmpParaName, current_level+1))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
		C0Var tmpparam(tmpParaName, "int", current_level+1, current_var_index);
		tmpparam.setInitialized();
		if (isConst)
			tmpparam.setConst();
		// 加入函数的参数表
		_functionsTable[current_func_index].getParamsList()->push_back(tmpparam);
		return {};
	}

	// <compound-statement> ::= 
    // 		'{' {<variable-declaration>} <statement-seq> '}'
	std::optional<CompilationError> Analyser::analyseCompoundStatement(){
		// '{'
		auto next = nextToken();
		if( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBrace);
		current_level++;
		// {<variable-declaration>}
		auto err = analyseVariableDeclarationMulti();
		if (err.has_value())
			return err;
		// <statement-seq>
		err = analyseStatementSeq();
		if (err.has_value())
			return err;
		// '}'
		next = nextToken();
		if( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBrace);
		crushVar(current_level);
		current_level--;
		return {};
	}

	// <statement-seq> ::= 
	// 		{<statement>}
	// <statement> ::= 
    // 		 '{' <statement-seq> '}'		{
    // 		|<condition-statement>			if
    // 		|<loop-statement>				while
	//		|<jump-statement>				return
	//		|<print-statement>				print
	//		|<scan-statement>				scan
	//		|<assignment-expression>';'		<identifier>
	//		|<function-call>';'				<identifier>
	//		|';'							;
	std::optional<CompilationError> Analyser::analyseStatementSeq(){
		while (true){
			auto next = nextToken();
			if (next.has_value()) return {};
			auto ttype = next.value().GetType();
			auto tvalue = next.value().GetValueString();
			if (ttype == TokenType::LEFT_BRACE){
				current_level++;
				auto err = analyseStatementSeq();
				if (err.has_value()) return err;
				next = nextToken();
				if ( ! next.has_value() || next.value().GetType()!= TokenType::RIGHT_BRACE)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBrace);
				crushVar(current_level);
				current_level--;
			}else if (ttype == TokenType::IF){
				unreadToken();
				auto err = analyseConditionStatement();
				if (err.has_value()) return err;
			}else if (ttype == TokenType::WHILE){
				unreadToken();
				auto err = analyseLoopStatement();
				if (err.has_value()) return err;
			}else if (ttype == TokenType::RETURN){
				unreadToken();
				auto err = analyseReturnStatement();
				if (err.has_value()) return err;
			}else if (ttype == TokenType::PRINT){
				unreadToken();
				auto err = analysePrintStatement();
				if (err.has_value()) return err;
			}else if (ttype == TokenType::SCAN){
				unreadToken();
				auto err = analyseScanStatement();
				if (err.has_value()) return err;
			}else if (ttype == TokenType::IDENTIFIER){
				// var 
				if (isVariable(tvalue,current_func_level)){
					unreadToken();
					auto err = analyseAssignmentExpression();
					if (err.has_value()) return err;
				}
				// function
				else if (isFunction(tvalue)){
					unreadToken();
					auto err = analyseFunctionCall();
					if (err.has_value()) return err; 
				}
				else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
				// ;
				next = nextToken();
				if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			}else if (ttype == TokenType::SEMICOLON){
				_instructions.emplace_back(current_instruction_index++, Operation::NOP, 0, 0);
			}else {
				return {};
			}
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseStatement(){
		auto next = nextToken();
		if (next.has_value()) return {};
		auto ttype = next.value().GetType();
		auto tvalue = next.value().GetValueString();
		if (ttype == TokenType::LEFT_BRACE){
			current_level++;
			auto err = analyseStatementSeq();
			if (err.has_value()) return err;
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType()!= TokenType::RIGHT_BRACE)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBrace);
			crushVar(current_level);
			current_level--;
		}else if (ttype == TokenType::IF){
			unreadToken();
			auto err = analyseConditionStatement();
			if (err.has_value()) return err;
		}else if (ttype == TokenType::WHILE){
			unreadToken();
			auto err = analyseLoopStatement();
			if (err.has_value()) return err;
		}else if (ttype == TokenType::RETURN){
			unreadToken();
			auto err = analyseReturnStatement();
			if (err.has_value()) return err;
		}else if (ttype == TokenType::PRINT){
			unreadToken();
			auto err = analysePrintStatement();
			if (err.has_value()) return err;
		}else if (ttype == TokenType::SCAN){
			unreadToken();
			auto err = analyseScanStatement();
			if (err.has_value()) return err;
		}else if (ttype == TokenType::IDENTIFIER){
			// var 
			if (isVariable(tvalue,current_func_level)){
				unreadToken();
				auto err = analyseAssignmentExpression();
				if (err.has_value())
					return err;
			}
			// function
			else if (isFunction(tvalue)){
				unreadToken();
				auto err = analyseFunctionCall();
				if (err.has_value()) return err; 
			}
			else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
			// ;
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}else if (ttype == TokenType::SEMICOLON){
			_instructions.emplace_back(current_instruction_index++, Operation::NOP, 0, 0);
		}else {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		}
		return {};
	}

	// <condition-statement>
	// 		'if' '(' <condition> ')' <statement> ['else' <statement>]
	std::optional<CompilationError> Analyser::analyseConditionStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::IF)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		Operation tmpop;
		auto err = analyseCondition(tmpop);
		if (err.has_value()) return err;
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		int32_t ifindex = _instructions.size();
		_instructions.emplace_back(current_instruction_index++, tmpop, 0, 0);
		err = analyseStatement();
		next = nextToken();
		_instructions[ifindex].SetX(current_instruction_index);
		if( ! next.has_value() || next.value().GetType() != TokenType::ELSE){
			unreadToken();
			return {};
		}
		err = analyseStatement();
		if (err.has_value()) return err;
		return {};
	}

	// <condition>
	// 		<expression>[<relational-operator><expression>]
	std::optional<CompilationError> Analyser::analyseCondition(Operation& tmpop){
		auto err = analyseExpression();
		if (err.has_value()) return err;
		auto next = nextToken();
		if ( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		else if (next.value().GetType() == TokenType::EQUAL_EQUAL)
			tmpop = Operation::JNE;
		else if (next.value().GetType() == TokenType::NOT_EQUAL)
			tmpop = Operation::JE;
		else if (next.value().GetType() == TokenType::GREATER_OR_EQUAL)
			tmpop = Operation::JL;
		else if (next.value().GetType() == TokenType::LESS_OR_EQUAL)
			tmpop = Operation::JG;
		else if (next.value().GetType() == TokenType::LEFT_ANGLE_BRACKET)
			tmpop = Operation::JGE;
		else if (next.value().GetType() == TokenType::RIGHT_ANGLE_BRACKET)
			tmpop = Operation::JLE;
		else {
			unreadToken();
			tmpop = Operation::JE;
			return {};
		}
		err = analyseExpression();
		if (err.has_value()) return err;
		return {};
	}

	// <loop-statement>
	// 		'while' '(' <condition> ')' <statement>
	std::optional<CompilationError> Analyser::analyseLoopStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::WHILE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		Operation tmpop;
		auto err = analyseCondition(tmpop);
		if (err.has_value()) return err;
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		int32_t whileindex = _instructions.size();
		_instructions.emplace_back(current_instruction_index++, tmpop, 0, 0);
		err = analyseStatement();
		if (err.has_value()) return err;
		_instructions[whileindex].SetX(current_instruction_index);
		return {};
	}

	// <return-statement>
	// 		'return' [<expression>] ';'
	std::optional<CompilationError> Analyser::analyseReturnStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RETURN)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		std::string rettype = _functionsTable[current_func_index].getRetType();
		if (rettype == "int"){
			auto err = analyseExpression();
			if (err.has_value()) return err;
		}
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		if (rettype == "int")
			_instructions.emplace_back(current_instruction_index++, Operation::IRET, 0, 0);
		else 
			_instructions.emplace_back(current_instruction_index++, Operation::RET, 0, 0);
		return {};
	}

	// <print-statement>
	// 		'print' '(' [<printable-list>] ')' ';'
	std::optional<CompilationError> Analyser::analysePrintStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::PRINT)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		// '('
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		// );          print '\n' directly
		next = nextToken();
		if ( !next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		else if (next.value().GetType() == TokenType::RIGHT_BRACKET){
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			_instructions.emplace_back(current_instruction_index++, Operation::PRINTL, 0, 0);
			return {};
		}else unreadToken();
		// <printable> {',' <printable>}
		auto err = analyseExpression();
		if (err.has_value()) return err;
		_instructions.emplace_back(current_instruction_index++, Operation::IPRINT, 0, 0);
		while (true) {
			next = nextToken();
			if ( ! next.has_value() )
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() != TokenType::COMMA){
				unreadToken();
				break;
			}
			// 输出空格
			_instructions.emplace_back(current_instruction_index++, Operation::BIPUSH, 32, 0);
			_instructions.emplace_back(current_instruction_index++, Operation::CPRINT, 0, 0);
			err = analyseExpression();
			if (err.has_value()) return err;
			_instructions.emplace_back(current_instruction_index++, Operation::IPRINT, 0, 0);
		}
		// ')'
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		_instructions.emplace_back(current_instruction_index++, Operation::PRINTL, 0, 0);
		return {};
	}

	// <scan-statement> ::= 
    // 		'scan' '(' <identifier> ')' ';'
	std::optional<CompilationError> Analyser::analyseScanStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SCAN)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		if ( ! isVariable(next.value().GetValueString(), current_func_level))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
		C0Var * ptmpvar = getVar(next.value().GetValueString(),current_func_level);
		if (ptmpvar->isConst())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
		_instructions.emplace_back(current_instruction_index++, Operation::LOADA, !(ptmpvar->getLevel()), ptmpvar->getOffset());
		_instructions.emplace_back(current_instruction_index++, Operation::ISCAN, 0, 0);
		_instructions.emplace_back(current_instruction_index++, Operation::ISTORE, 0, 0);
		if ( ! ptmpvar->isInitialized())
			ptmpvar->setInitialized();
		// ')'
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		return {};
	}

	// <assignment-expression>
	// 		<identifier><assignment-operator><expression>
	std::optional<CompilationError> Analyser::analyseAssignmentExpression(){
		// <identifier>
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER || !isVariable(next.value().GetValueString(),current_func_level) )
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		C0Var * pvar = getVar(next.value().GetValueString(),current_func_level);
		if (pvar->isConst())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
		// <assignment-operator>
		// '='
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::EQUAL_SIGN)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidAssignment);
		// <expression>
		auto err = analyseExpression();
		if (err.has_value()) return err;
		_instructions.emplace_back(current_instruction_index++, Operation::ISTORE, 0, 0);
		return {};
	}

	// <function-call>
	// 		<identifier> '(' [<expression-list>] ')'
	std::optional<CompilationError> Analyser::analyseFunctionCall(){
		// <identifier> 
		// 父调用函数检查过是函数标识符了
		auto next = nextToken();
		C0Function * pfunc = getFunc(next.value().GetValueString());
		auto paramsNum = (*pfunc).getParamsNum();
		// '(' 
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		// [<expression-list>] 
		auto err = analyseExpressionList(paramsNum);
		if (err.has_value()) return err;
		// ')'
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		// call
		_instructions.emplace_back(current_instruction_index++, Operation::CALL, pfunc->getOffset(), 0);
		return {};
	}

	// <expression-list>
	// 		<expression>{','<expression>}
	std::optional<CompilationError> Analyser::analyseExpressionList(int32_t paraNum) {
		if (paraNum==0)
			return {};
		for(;paraNum>0;paraNum--) {
			auto err = analyseExpression();
			if (err.has_value()) return err;
			if (paraNum == 1)
				break;
			// ','
			auto next = nextToken();
			if( ! next.has_value() || next.value().GetType() != TokenType::COMMA)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedComma);
		}
		return {};
	}

	std::optional<Token> Analyser::nextToken() {
		if (_offset == _tokens.size())
			return {};
		// 考虑到 _tokens[0..._offset-1] 已经被分析过了
		// 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
		_current_pos = _tokens[_offset].GetEndPos();
		return _tokens[_offset++];
	}

	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}

	void Analyser::addGlobalVariable(C0Var& c0var) {
		_globalVariablesTable.push_back(c0var);
	}

	// void Analyser::addGlobalConstant(C0Var& c0var) {
	// 	_globalConstantsTable.push_back(c0var);
	// }

	void Analyser::addFunction(C0Function& func) {
		_functionsTable.push_back(func);
	}

	void Analyser::addVariable(C0Var& c0var) {
		_variablesTable.push_back(c0var);
	}

	// bool Analyser::isDeclared(const std::string& s, int32_t level) {
	// 	if (level < 0)
	// 		return false;
	// 	if (level == 0)
	// 		return isFunction(s) || isGlobalVariable(s);
	// 	else 
	// 		return isVariable(s,level);
	// }

	// 同级作用域下变量是否已经声明
	bool Analyser::isDeclaredSameLevel(const std::string& s, int32_t level) {
		if (level < 0)
			return false;
		else if (level == 0)
			return (isFunction(s)||isGlobalVariable(s));
		int len = _variablesTable.size();
		for (int i=len-1; i>=0; i--){
			if (_variablesTable[i].getLevel() < level)
				return false;
			else if (s == _variablesTable[i].getName() )
				return true;
		}
		return false;
	}

	// bool Analyser::isGlobalConstant(const std::string&s) {
	// 	int len = _globalConstantsTable.size();
	// 	for (int i = 0; i<len; i++){
	// 		if (s == _globalConstantsTable[i].getName() )
	// 			return true;
	// 	}
	// 	return false;
	// }

	bool Analyser::isFunction(const std::string&s) {
		int len = _functionsTable.size();
		for (int i = 0; i<len; i++){
			if (s == _functionsTable[i].getFuncName() )
				return true;
		}
		return false;
	}

	bool Analyser::isGlobalVariable(const std::string&s) {
		int len = _globalVariablesTable.size();
		for (int i = 0; i<len; i++){
			if (s == _globalVariablesTable[i].getName() )
				return true;
		}
		return false;
	}

	bool Analyser::isVariable(const std::string&s, int32_t level) {
		int len = _variablesTable.size();
		for (int i=len-1; i>=0; i--){
			if (_variablesTable[i].getLevel() < level)
				break;
			else if (s == _variablesTable[i].getName() )
				return true;
		}
		return isGlobalVariable(s);
	}

	C0Var* Analyser::getVar(const std::string& s,int32_t level){
		int32_t len = _variablesTable.size();
		C0Var * pVar;
		for (int i=len-1; i>=0; i--){
			if (_variablesTable[i].getLevel()<level)
				break;
			else if (s == _variablesTable[i].getName())
				return &_variablesTable[i];
		}
		pVar = getGlobalVar(s);
		// if (pVar != nullptr)
			return pVar;
		// else if ((pVar = getGlobalConst(s)) != nullptr)
		// 	return pVar;
		// return nullptr;
	}

	C0Var * Analyser::getGlobalVar(const std::string& s){
		int32_t len = _globalVariablesTable.size();
		for (int i=len-1; i>=0; i--){
			if (s == _globalVariablesTable[i].getName())
				return &_globalVariablesTable[i];
		}
		return nullptr;
	}

	// C0Var * Analyser::getGlobalConst(const std::string& s) {
	// 	int32_t len = _globalConstantsTable.size();
	// 	for (int i=len; i>=0; i--){
	// 		if (s == _globalConstantsTable[i].getName())
	// 			return &_globalConstantsTable[i];
	// 	}
	// 	return nullptr;
	// }

	C0Function* Analyser::getFunc(const std::string& s){
		int32_t len = _functionsTable.size();
		for (int i=0; i<len; i++){
			if (s == _functionsTable[i].getFuncName())
				return &_functionsTable[i];
		}
		return nullptr;
	}

	void Analyser::crushVar(int32_t level){
		if (level <= 0)
			return;
		while(true){
			if (!_variablesTable.empty() && _variablesTable.back().getLevel() == level) 
				_variablesTable.pop_back();
			else 
				break;
		}
		return ;
	}

}