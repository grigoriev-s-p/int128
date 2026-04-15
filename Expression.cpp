#include "Expression.hpp"


std::ostream& operator<<(std::ostream& os, const Expression& expr) {
	expr.print(os);
	return os;
}


Const::Const(Int128 val) : value(val) {}


Int128 Const::eval(const std::map<std::string, Int128>&) const {
	return value;
}


Expression* Const::clone() const {
	return new Const(value);
}


void Const::print(std::ostream& os) const {
	os << value;
}


Variable::Variable(std::string_view n) : name(n) {}


Int128 Variable::eval(const std::map<std::string, Int128>& variables) const {
	return variables.at(name);
}


Expression* Variable::clone() const {
	return new Variable(name);
}


void Variable::print(std::ostream& os) const {
	os << name;
}


Negate::Negate(const Expression& e) : expr(e.clone()) {}


Negate::~Negate() {
	delete expr;
}


Int128 Negate::eval(const std::map<std::string, Int128>& variables) const {
	return -expr->eval(variables);
}


Expression* Negate::clone() const {
	return new Negate(*expr);
}


void Negate::print(std::ostream& os) const {
	os << "(-" << *expr << ")";
}


BinaryExpression::BinaryExpression(const Expression& l, const Expression& r)
	: left(l.clone()), right(r.clone()) {
}


BinaryExpression::~BinaryExpression() {
	delete left;
	delete right;
}


Int128 Add::eval(const std::map<std::string, Int128>& v) const {
	return left->eval(v) + right->eval(v);
}


Expression* Add::clone() const {
	return new Add(*left, *right);
}


void Add::print(std::ostream& os) const {
	os << "(" << *left << " + " << *right << ")";
}


Int128 Subtract::eval(const std::map<std::string, Int128>& v) const {
	return left->eval(v) - right->eval(v);
}


Expression* Subtract::clone() const {
	return new Subtract(*left, *right);
}


void Subtract::print(std::ostream& os) const {
	os << "(" << *left << " - " << *right << ")";
}


Int128 Multiply::eval(const std::map<std::string, Int128>& v) const {
	return left->eval(v) * right->eval(v);
}


Expression* Multiply::clone() const {
	return new Multiply(*left, *right);
}


void Multiply::print(std::ostream& os) const {
	os << "(" << *left << " * " << *right << ")";
}


Int128 Divide::eval(const std::map<std::string, Int128>& v) const {
	return left->eval(v) / right->eval(v);
}


Expression* Divide::clone() const {
	return new Divide(*left, *right);
}


void Divide::print(std::ostream& os) const {
	os << "(" << *left << " / " << *right << ")";
}


Add operator+(const Expression& l, const Expression& r) {
	return Add(l, r);
}


Subtract operator-(const Expression& l, const Expression& r) {
	return Subtract(l, r);
}


Multiply operator*(const Expression& l, const Expression& r) {
	return Multiply(l, r);
}


Divide operator/(const Expression& l, const Expression& r) {
	return Divide(l, r);
}


Negate operator-(const Expression& e) {
	return Negate(e);
}