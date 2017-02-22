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
		~muParserInterface();
		muParserInterface(muParserInterface<embedded,real> const &);
		
		void set_expression(const std::string & e);
		virtual real operator()(const Point<embedded,real>& inputPoint, real const & t=0) = 0;
	protected:
		mu::Parser M_parser;
		double M_t;
		std::string M_expr;
};

template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface() {
	this->M_parser.DefineVar("t",&M_t);
}

template <long embedded,typename real>
muParserInterface<embedded,real>::~muParserInterface()
{
	this->M_parser.ClearVar();
}

template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface(muParserInterface<embedded,real> const & mpi):
M_parser(),M_t(mpi.M_t),M_expr(mpi.M_expr) {
	this->M_parser.SetExpr(M_expr);
	this->M_parser.DefineVar("t",&M_t);
}

template <long embedded,typename real>
void  muParserInterface<embedded,real>::set_expression(const std::string & s) {
	M_expr = s;
	this->M_parser.SetExpr(s);
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
