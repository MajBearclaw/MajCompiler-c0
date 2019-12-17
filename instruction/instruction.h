#pragma once

#include <cstdint>
#include <utility>

namespace majc0 {

	enum Operation {
		// ILL = 0,
		// LIT,

		NOP = 0,
		BIPUSH,
		IPUSH,
		POP,
		POP2,
		POPN,
		DUP,
		DUP2,
		LOADC,		// load constant
		LOADA,		// loada level_diff(2), offset(4)
		NEW,		// pop (int)count, new (count) slot
		SNEW,		// snew count(4)
		ILOAD,
		ALOAD,
		IALOAD,		// array
		AALOAD,
		ISTORE,
		ASTORE,
		IASTORE,	// array
		AASTORE,
		IADD,
		// DADD,
		ISUB,
		// DSUB,
		IMUL,
		// DMUL,
		IDIV,
		// DDIV,
		INEG,		// a = -a
		// DNEG,
		ICMP,		// pop r, pop l, push sign(l-r);  1|-1|0
		// DCMP,
		// I2D,
		// D2I,
		// I2C,
					// jxxx offset(2)
		JMP,		
		JE,
		JNE,
		JL,
		JGE,
		JG,
		JLE,
		CALL,		// call index(2)
		RET,
		IREI,
		AREI,
		IPRINT,
		CPRINT,
		APRINT,
		// SPRINT,
		PRINTL,
		ISCAN
	};
	
	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(Operation opr, int32_t x) : _opr(opr), _x(x) {}
		Instruction(Operation opr, int32_t x, int32_t y) : _opr(opr), _x(x), _y(y) {} ///////
		Instruction() : Instruction(Operation::NOP, 0){}
		Instruction(const Instruction& i) { _opr = i._opr; _x = i._x; }
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
	private:
		Operation _opr;
		int32_t _x;
		int32_t _y;	////////
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y); ///////
	}
}