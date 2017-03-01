#ifndef HH_MUPARSERINTERFACE_HH
#define HH_MUPARSERINTERFACE_HH

#include <string>
#include <vector>
#include "muParser.h"
#include "Point.h"

/** Generic muParserInterface that can take into account any type of coordinates (ex: spherical, carthesian, etc)
 *
 *	It uses the library muParser. It reads and evaluate any mathematical expression.
 *
 *	\param embedded Dimension of the space
 *	\param real double or long double
 */
template <long embedded,typename real=double>
class muParserInterface {
	public:
		muParserInterface();
		~muParserInterface();
		muParserInterface(muParserInterface<embedded,real> const &);
		
		void set_expression(const std::string & e); //!< Set the expression to the parser
		/** Main virtual method, to be implemented in subclasses. Evaluates an expression in a given Point using the operator ().
		 */
		virtual real operator()(const Point<embedded,real>& inputPoint, real const & t=0) = 0;
		
	protected:
		mu::Parser M_parser;
		double M_t; //!< Time variable, whose name does not depend on the chosen type of coordinates
		std::string M_expr; //!< Expression
};

// Constructor
template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface() {
	this->M_parser.DefineVar("t",&M_t);
}

// Destructor
template <long embedded,typename real>
muParserInterface<embedded,real>::~muParserInterface()
{
	this->M_parser.ClearVar();
}

// Copy constructor
template <long embedded,typename real>
muParserInterface<embedded,real>::muParserInterface(muParserInterface<embedded,real> const & mpi):
M_parser(),M_t(mpi.M_t),M_expr(mpi.M_expr) {
	this->M_parser.SetExpr(M_expr);
	this->M_parser.DefineVar("t",&M_t);
}

// Set the expression to the muParser
template <long embedded,typename real>
void  muParserInterface<embedded,real>::set_expression(const std::string & s) {
	M_expr = s;
	this->M_parser.SetExpr(s);
}

// Print message on the standard error
void printMuException(mu::Parser::exception_type &e) {
	using std::cout;
	cout << "Content of muParser exception" << "\n";
	cout << "Message:  " << e.GetMsg() << "\n";
	cout << "Formula:  " << e.GetExpr() << "\n";
	cout << "Token:    " << e.GetToken() << "\n";
	cout << "Position: " << e.GetPos() << "\n";
	cout << "Errc:     " << e.GetCode() << "\n";
}

#endif /* HH_MUPARSERINTERFACE_HH */
