#ifndef HH_CARTHESIANPARSERINTERFACE_HH
#define HH_CARTHESIANPARSERINTERFACE_HH

#include "muParserInterface.h"

/** Carthesian muParserInterface that inherits from muParserInterface.
 *
 *	It uses the library muParser. It reads and evaluate a mathematical expression with unknowns t,x,y,z.
 *
 *	\param embedded Dimension of the space
 *	\param real double or long double
 */
template <long embedded,typename real=double>
class carthesianParserInterface: public muParserInterface<embedded,real> {
	public:
		carthesianParserInterface();
		carthesianParserInterface(const std::string & s);
		carthesianParserInterface(carthesianParserInterface const &);
		carthesianParserInterface & operator=(carthesianParserInterface const &);
		
		/** Evaluates an expression with unknowns x,y,z,t in a given Point using the operator ().
		 */
		virtual real operator()(const Point<embedded,real>& inputPoint, real const & t=0);
		
	private:
		double M_x, M_y, M_z; //!< Carthesian variables
};

// Constructor
template <long embedded,typename real>
carthesianParserInterface<embedded,real>::carthesianParserInterface(): muParserInterface<embedded,real>() {
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);
	this->M_parser.DefineVar("z",&M_z);
}

// Constructor given a string expression
template <long embedded,typename real>
carthesianParserInterface<embedded,real>::carthesianParserInterface(const std::string & e): 
carthesianParserInterface() {
	muParserInterface<embedded,real>::set_expression(e);
}

// Copy constructor
template <long embedded,typename real>
carthesianParserInterface<embedded,real>::carthesianParserInterface(carthesianParserInterface const & mpi):
muParserInterface<embedded,real>(mpi),M_x(mpi.M_x),M_y(mpi.M_y),M_z(mpi.M_z) {
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);  
	this->M_parser.DefineVar("z",&M_z);  
}

// = operator
template <long embedded,typename real>
carthesianParserInterface<embedded,real> & carthesianParserInterface<embedded,real>::operator=(carthesianParserInterface<embedded,real> const & mpi) {
	if (this != &mpi) {
		this->M_parser.ClearVar();
		this->M_expr = mpi.M_expr;
		this->M_t = mpi.M_t;
		this->M_x = mpi.M_x;
		this->M_y = mpi.M_y;
		this->M_z = mpi.M_z;
		this->M_parser.SetExpr(this->M_expr);
		this->M_parser.DefineVar("t",&(this->M_t));
		this->M_parser.DefineVar("x",&M_x);
		this->M_parser.DefineVar("y",&M_y);
		this->M_parser.DefineVar("z",&M_z);
	}
	return *this;
}

// Evaluation operator, returns the value of the expression at a given Point
template <long embedded,typename real>
real carthesianParserInterface<embedded,real>::operator()(const Point<embedded,real>& inputPoint, real const & t) {
	this->M_t = t;
	this->M_x = inputPoint[0];
	if (embedded > 1) {
		this->M_y = inputPoint[1];
		if (embedded > 2)
			this->M_z = inputPoint[2];
	}
	
	return this->M_parser.Eval();
}

#endif /* HH_CARTHESIANPARSERINTERFACE_HH */
