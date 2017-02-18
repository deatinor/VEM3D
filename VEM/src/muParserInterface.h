#ifndef HH_MUPARSERINTERFACE_HH
#define HH_MUPARSERINTERFACE_HH

#include <string>
#include <vector>
#include "muParser.h"
#include "Point.h"

template <long embedded,typename real=double>
class muParserInterface {
	public:
		muParserInterface();
		muParserInterface(const std::string & s);
		~muParserInterface();
		muParserInterface(muParserInterface const &);
		muParserInterface & operator=(muParserInterface const &);
		
		void set_expression(const std::string & e);
		real operator()(const Point<embedded,real>& inputPoint, real const & t=0);
	private:
		mu::Parser M_parser;
		double M_t, M_x, M_y, M_z;
		std::string M_expr;
};

template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface() {
	this->M_parser.DefineVar("t",&M_t);
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);
	this->M_parser.DefineVar("z",&M_z);
}

template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface(const std::string & e): 
muParserInterface() { 
	M_expr = e;
	this->M_parser.SetExpr(e);
}

template <long embedded,typename real>
muParserInterface<embedded,real>::~muParserInterface()
{
	this->M_parser.ClearVar();
}

template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface(muParserInterface const & mpi):
M_parser(),M_t(mpi.M_t),M_x(mpi.M_x),M_y(mpi.M_y),M_z(mpi.M_z),M_expr(mpi.M_expr) {
	this->M_parser.SetExpr(M_expr);
	this->M_parser.DefineVar("t",&M_t);
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);  
	this->M_parser.DefineVar("z",&M_z);  
}

template <long embedded,typename real>
muParserInterface<embedded,real> & muParserInterface<embedded,real>::operator=(muParserInterface<embedded,real> const & mpi) {
	if (this != &mpi) {
		this->M_parser.ClearVar();
		this->M_expr = mpi.M_expr;
		this->M_t = mpi.M_t;
		this->M_x = mpi.M_x;
		this->M_y = mpi.M_y;
		this->M_z = mpi.M_z;
		this->M_parser.SetExpr(M_expr);
		this->M_parser.DefineVar("t",&M_t);
		this->M_parser.DefineVar("x",&M_x);
		this->M_parser.DefineVar("y",&M_y);
		this->M_parser.DefineVar("z",&M_z);
	}
	return *this;
}

template <long embedded,typename real>
void  muParserInterface<embedded,real>::set_expression(const std::string & s) {
	M_expr = s;
	this->M_parser.SetExpr(s);
}

template <long embedded,typename real>
real muParserInterface<embedded,real>::operator()(const Point<embedded,real>& inputPoint, real const & t) {
	this->M_t = t;
	this->M_x = inputPoint[0];
	if (embedded > 1) {
		this->M_y = inputPoint[1];
		if (embedded > 2)
			this->M_z = inputPoint[2];
	}
	
	return this->M_parser.Eval();
}

//! prints message on the standard error
void printMuException(mu::Parser::exception_type &e) {
	using std::cout;
	cout << "Content of muParser exception" << "\n";
	cout << "Message:  " << e.GetMsg() << "\n";
	cout << "Formula:  " << e.GetExpr() << "\n";
	cout << "Token:    " << e.GetToken() << "\n";
	cout << "Position: " << e.GetPos() << "\n";
	cout << "Errc:     " << e.GetCode() << "\n";
}


#endif
