#include "analyser.h"

#include <climits>

namespace majc0 {
	std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseProgram();
		if (err.has_value())
			return std::make_pair(std::vector<Instruction>(), err);
		else
			return std::make_pair(_instructions, std::optional<CompilationError>());
	}

	// <C0-program> ::= {<variable-declaration>}{<function-definition>}
	std::optional<CompilationError> Analyser::analyseProgram() {
		auto err = analyseVariableDeclarationMulti();
		if(err.has_value())
			return err;
		err = analyseFunctionDefinitionMulti();
		if(err.has_value())
			return err;
		return {};
	}

	// {<variable-declaration>}
	// <variable-declaration> ::= [<const-qualifier>]<type-specifier><init-declarator-list>';'
	std::optional<CompilationError> Analyser::analyseVariableDeclarationMulti() {
		while (true){
			auto next = nextToken();
			auto isConst = 0;
			if (!next.has_value())
				return {};
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
					//////////////////////
				}else if( str == "char" || str == "double" ){
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrUnimplemented);
				}else if(str == "void"){
					unreadToken();
					return {};
				}else 
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			}else
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			
			// <init-declarator-list>
			analyseInitDeclaratorList(isConst);
			// ';'
			next = nextToken();
			if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			// 生成指令
			//////////////////////////
		}
		return {};
	}

	// <init-declarator-list> ::= <init-declarator>{','<init-declarator>}
	std::optional<CompilationError> Analyser::analyseInitDeclaratorList(int32_t isConst) {
		auto err = analyseInitDeclarator(isConst);
		if (err.has_value())
			return err;
		while (true){
			// ','
			auto next = nextToken();
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() == TokenType::COMMA){
				err = analyseInitDeclarator(isConst);
				if (err.has_value())
					return err;
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
		if (isDeclared(next.value().GetValueString()))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
		////////////////////////进表，检查同名，

		_instructions.emplace_back(Operation::SNEW, 1, 0);
		
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
			////////////////////////////生成指令

		}
		else {
			if (isConst)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstantNeedValue);
			unreadToken();
		}
		
		return {};
	}

	// <expression> ::= <additive-expression>
	std::optional<CompilationError> Analyser::analyseExpression() {
		auto err = analyseAdditiveExpression();
		if (err.has_value())
			return err;
		////////////////////////////生成指令
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
				// prefix = 1;
			}else if (next.value().GetType() == TokenType::MINUS_SIGN) {
				prefix = -1;
			}else {
				unreadToken();
				return {};
			}
			// <multiplicative-expression>
			err = analyseMultiplicativeExpression();
			if (err.has_value())
				return err;
			////////////////////////////生成指令
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
			// 生成指令, * | /

			// <cast-expression>
			err = analyseCastExpression();
			if(err.has_value())
				return err;
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
		if( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		else if (next.value().GetType() == TokenType::PLUS_SIGN){
			//////////////指令
		}else if (next.value().GetType() == TokenType::MINUS_SIGN){
			//////////////指令
		}else {
			unreadToken();
		}
		// <primary-expression>
		auto err = analysePrimaryExpression();
		if (err.has_value())
			return err;
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
			if (err.has_value())
				return err;
			next = nextToken();
			if ( ! next.has_value()  || next.value().GetType != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		}
		// |<identifier>
		// |<function-call>
		else if (next.value().GetType() == TokenType::IDENTIFIER ) {
			//////查变量表
			//////	如果在变量表，指令：变量的值放在栈上
			//////查函数表
			//////	如果在函数表，指令：调用函数?
			//////  函数return不能是void
			//////		!!!!!!!!
			//////	'(' [<expression-list>] ')'
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
			auto err = analyseExpressionList();
			if (err.has_value())
				return err;
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
			//////如果两个表都不在，则报错

		}
		// |<integer-literal>
		else if (next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
			//////指令，数据放到栈顶
		}

		return {};
	}

	// <expression-list> ::= 
    // 		<expression>{','<expression>}
	std::optional<CompilationError> Analyser::analyseExpressionList() {
		auto err = analyseExpression();
		if (err.has_value())
			return err;
		// {...}
		while (true) {
			auto next = nextToken();
			if( ! next.has_value() )
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if(next.value().GetType() != TokenType::COMMA){
				unreadToken();
				return {};
			}
			// after ',', <expression>
			err = analyseExpression();
			if (err.has_value())
				return err;
		}
		return {};
	}

	// {<function-definition>}
	// <function-definition> ::= 
    // 		<type-specifier><identifier><parameter-clause><compound-statement>
	// !!!从语义上，要求C0源代码中必须定义`main`函数。
	std::optional<CompilationError> Analyser::analyseFunctionDefinitionMulti(){
		while (true) {
			auto next = nextToken();
			if ( ! next.has_value())
				return {};
			else if (next.value().GetType() != TokenType::SPECIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			else if (next.value().GetValueString() != "void")
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
			// <identifier>
			next = nextToken();
			if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
			if (isDeclared(next.value().GetValueString()))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
			/////////////////放入函数表，生成指令
			
			// <parameter-clause>
			auto err = analyseParameterClause();
			if (err.has_value())
				return err;
			// <compound-statement>
			err = analyseCompoundStatement();
			if (err.has_value())
				return err;
			/////////////指令？？？
		}
		//////////////检查main
		return {};
	}

	// <parameter-clause>
	// 		'(' [<parameter-declaration-list>] ')'
	std::optional<CompilationError> Analyser::analyseParameterClause(){
		auto next = nextToken();
		if( ! next.has_value() )
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
		// '('
		else if (next.value().GetType() == TokenType::LEFT_BRACKET){
			next = nextToken();
			// read ')' 
			if ( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType == TokenType::RIGHT_BRACKET)
				return {};
			else 
				unreadToken();
			// <parameter-declaration-list>
			auto err = analyseParameterDeclarationList();
			if (err.has_value())
				return err;
			// ')'
			next = nextToken();
			if ( ! next.has_value()  || next.value().GetType != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		}else 
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		return {};
	}

	// <parameter-declaration-list> ::= 
    // 		<parameter-declaration>{','<parameter-declaration>}
	std::optional<CompilationError> Analyser::analyseParameterDeclarationList(){
		auto err = analyseParameterDeclaration();
		if (err.has_value())
			return err;
		while (true){
			// ','
			auto next = nextToken();
			if( ! next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() == TokenType::COMMA){
				err = analyseParameterDeclaration();
				if (err.has_value())
					return err;
			}else {
				unreadToken();
				/////////////////需要生成指令？
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
			///////////////////指令，常量修饰符
			isConst = 1;
			next = nextToken();
		}
		// <type-specifier>
		if( ! next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedTypeSpecifier);
		else if(next.value().GetType() == TokenType::SPECIFIER){
			std::string str = next.value().GetValueString();
			if(str == "int"){
				//////////////////////
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
		if ( ! next.has_value() || next.value().GetType != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		
		/////////////////对函数参数生成指令,要考虑是否被声明过吗？？？
		return {};
	}

	// <compound-statement> ::= 
    // 		'{' {<variable-declaration>} <statement-seq> '}'
	std::optional<CompilationError> Analyser::analyseCompoundStatement(){
		// '{'
		auto next = nextToken();
		if( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBrace);
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
		///////////////////////////////生成指令？？？
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
			if (next.has_value())
				return {};
			auto ttype = next.value().GetType();
			auto tvalue = next.value().GetValueString();
			if (ttype == TokenType::LEFT_BRACE){
				auto err = analyseStatementSeq();
				if (err.has_value())
					return err;
				next = nextToken();
				if ( ! next.has_value() || next.value().GetType()!= TokenType::RIGHT_BRACE)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBrace);
			}else if (ttype == TokenType::IF){
				
			}else if (ttype == TokenType::LOOP){

			}else if (ttype == TokenType::JUMP){

			}else if (ttype == TokenType::PRINT){
				analysePrintStatement();
			}else if (ttype == TokenType::SCAN){
				analyseScanStatement();
			}else if (ttype == TokenType::IDENTIFIER){
				// var
				// 		检查isConst
				// function
				// ;
				next = nextToken();
				if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			}else if (ttype == TokenType::SEMICOLON){
				_instructions.emplace_back(Operation::NOP, 0, 0);
				continue;
			}else {
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidStatement);
			}
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysePrintStatement(){
		// '('
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		// <printable> {',' <printable>}
		auto err = analyseExpression();
		if (err.has_value())
			return err;
		_instructions.emplace_back(Operation::IPRINT, 0, 0);
		while (true) {
			next = nextToken();
			if ( ! next.has_value() )
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
			else if (next.value().GetType() != TokenType::COMMA){
				unreadToken();
				break;
			}
			_instructions.emplace_back(Operation::BIPUSH, 32, 0);
			_instructions.emplace_back(Operation::CPRINT, 0, 0);
			err = analyseExpression();
			if (err.has_value())
				return err;
			_instructions.emplace_back(Operation::IPRINT, 0, 0);
		}
		// ')'
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		_instructions.emplace_back(Operation::PRINTL, 0, 0);
		return {};
	}

	std::optional<CompilationError> Analyser::analyseScanStatement(){
		auto next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedLeftBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
		/////////////检查是否是合法变量
		
		_instructions.emplace_back(Operation::LOADA, level_diff, offset);
		_instructions.emplace_back(Operation::ISCAN, 0, 0);
		_instructions.emplace_back(Operation::ISTORE, 0, 0);
		// ')'
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedRightBracket);
		next = nextToken();
		if ( ! next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		return {};
	}

	std::optional<CompilationError> Analyser::analyseAssignmentExpression(){

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

	void Analyser::_add(const Token& tk, std::map<std::string, int32_t>& mp) {
		if (tk.GetType() != TokenType::IDENTIFIER)
			DieAndPrint("only identifier can be added to the table.");
		mp[tk.GetValueString()] = _nextTokenIndex;
		_nextTokenIndex++;
	}

	void Analyser::addVariable(const Token& tk) {
		_add(tk, _vars);
	}

	void Analyser::addConstant(const Token& tk) {
		_add(tk, _consts);
	}

	void Analyser::addUninitializedVariable(const Token& tk) {
		_add(tk, _uninitialized_vars);
	}

	int32_t Analyser::getIndex(const std::string& s) {
		if (_uninitialized_vars.find(s) != _uninitialized_vars.end())
			return _uninitialized_vars[s];
		else if (_vars.find(s) != _vars.end())
			return _vars[s];
		else
			return _consts[s];
	}

	bool Analyser::isDeclared(const std::string& s) {
		return isConstant(s) || isUninitializedVariable(s) || isInitializedVariable(s);
	}

	bool Analyser::isUninitializedVariable(const std::string& s) {
		return _uninitialized_vars.find(s) != _uninitialized_vars.end();
	}
	bool Analyser::isInitializedVariable(const std::string&s) {
		return _vars.find(s) != _vars.end();
	}

	bool Analyser::isConstant(const std::string&s) {
		return _consts.find(s) != _consts.end();
	}
}