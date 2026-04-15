#include "Int128.hpp"
#include <string>
#include <map>
#include <iostream>

class Expression {
public:
	virtual ~Expression() = default;

	virtual Int128 eval(const std::map<std::string, Int128>& variables) const = 0;

	virtual Expression* clone() const = 0;

	virtual void print(std::ostream& os) const = 0;
};

std::ostream& operator<<(std::ostream& os, const Expression& expr) {
	expr.print(os);
	return os;
}

class Const : public Expression {
	Int128 value;
public:
	Const(Int128 val) : value(val) {}

	Int128 eval(const std::map<std::string, Int128>&) const override {
		return value;
	}

	Expression* clone() const override {
		return new Const(value);
	}

	void print(std::ostream& os) const override {
		os << value;
	}
};

class Variable : public Expression {
	std::string name;
public:
	Variable(std::string_view n) : name(n) {}

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return variables.at(name);
	}

	Expression* clone() const override {
		return new Variable(name);
	}

	void print(std::ostream& os) const override {
		os << name;
	}
};

class Negate : public Expression {
	Expression* expr;
public:
	Negate(const Expression& e) : expr(e.clone()) {}

	~Negate() { delete expr; }

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return -expr->eval(variables);
	}

	Expression* clone() const override {
		return new Negate(*expr);
	}

	void print(std::ostream& os) const override {
		os << "(-" << *expr << ")";
	}
};

class BinaryExpression : public Expression {
protected:
	Expression* left;
	Expression* right;
public:
	BinaryExpression(const Expression& l, const Expression& r)
		: left(l.clone()), right(r.clone()) {
	}

	~BinaryExpression() {
		delete left;
		delete right;
	}
};

class Add : public BinaryExpression {
public:
	Add(const Expression& l, const Expression& r) : BinaryExpression(l, r) {}

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return left->eval(variables) + right->eval(variables);
	}

	Expression* clone() const override {
		return new Add(*left, *right);
	}

	void print(std::ostream& os) const override {
		os << "(" << *left << " + " << *right << ")";
	}
};

class Subtract : public BinaryExpression {
public:
	Subtract(const Expression& l, const Expression& r) : BinaryExpression(l, r) {}

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return left->eval(variables) - right->eval(variables);
	}

	Expression* clone() const override {
		return new Subtract(*left, *right);
	}

	void print(std::ostream& os) const override {
		os << "(" << *left << " - " << *right << ")";
	}
};

class Multiply : public BinaryExpression {
public:
	Multiply(const Expression& l, const Expression& r) : BinaryExpression(l, r) {}

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return left->eval(variables) * right->eval(variables);
	}

	Expression* clone() const override {
		return new Multiply(*left, *right);
	}

	void print(std::ostream& os) const override {
		os << "(" << *left << " * " << *right << ")";
	}
};

class Divide : public BinaryExpression {
public:
	Divide(const Expression& l, const Expression& r) : BinaryExpression(l, r) {}

	Int128 eval(const std::map<std::string, Int128>& variables) const override {
		return left->eval(variables) / right->eval(variables);
	}

	Expression* clone() const override {
		return new Divide(*left, *right);
	}

	void print(std::ostream& os) const override {
		os << "(" << *left << " / " << *right << ")";
	}
};

Add operator+(const Expression& l, const Expression& r) { return Add(l, r); }
Subtract operator-(const Expression& l, const Expression& r) { return Subtract(l, r); }
Multiply operator*(const Expression& l, const Expression& r) { return Multiply(l, r); }
Divide operator/(const Expression& l, const Expression& r) { return Divide(l, r); }
Negate operator-(const Expression& e) { return Negate(e); }