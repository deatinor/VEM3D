#ifndef HH_CARTESIANPARSERINTERFACE_HH
#define HH_CARTESIANPARSERINTERFACE_HH

#include "muParserInterface.h"

/** Cartesian muParserInterface that inherits from muParserInterface.
 *
 *	It uses the library muParser. It reads and evaluate a mathematical expression with unknowns t,x,y,z.
 *
 *	\param embedded Dimension of the space
 *	\param real double or long double
 */
template <long embedded,typename real=double>
class cartesianParserInterface: public muParserInterface<embedded,real> {
	public:
		cartesianParserInterface();
		cartesianParserInterface(const std::string & s);
		cartesianParserInterface(cartesianParserInterface const &);
		cartesianParserInterface & operator=(cartesianParserInterface const &);
		
		/** Evaluates an expression with unknowns x,y,z,t in a given Point using the operator ().
		 */
		virtual real operator()(const Point<embedded,real>& inputPoint, real const & t=0);
		
	private:
		double M_x, M_y, M_z; //!< Cartesian variables
};

// Constructor
template <long embedded,typename real>
cartesianParserInterface<embedded,real>::cartesianParserInterface(): muParserInterface<embedded,real>() {
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);
	this->M_parser.DefineVar("z",&M_z);
}

// Constructor given a string expression
template <long embedded,typename real>
cartesianParserInterface<embedded,real>::cartesianParserInterface(const std::string & e): 
cartesianParserInterface() {
	muParserInterface<embedded,real>::set_expression(e);
}

// Copy constructor
template <long embedded,typename real>
cartesianParserInterface<embedded,real>::cartesianParserInterface(cartesianParserInterface const & mpi):
muParserInterface<embedded,real>(mpi),M_x(mpi.M_x),M_y(mpi.M_y),M_z(mpi.M_z) {
	this->M_parser.DefineVar("x",&M_x);
	this->M_parser.DefineVar("y",&M_y);  
	this->M_parser.DefineVar("z",&M_z);  
}

// = operator
template <long embedded,typename real>
cartesianParserInterface<embedded,real> & cartesianParserInterface<embedded,real>::operator=(cartesianParserInterface<embedded,real> const & mpi) {
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
real cartesianParserInterface<embedded,real>::operator()(const Point<embedded,real>& inputPoint, real const & t) {
	this->M_t = t;
	this->M_x = inputPoint[0];
	if (embedded > 1) {
		this->M_y = inputPoint[1];
		if (embedded > 2)
			this->M_z = inputPoint[2];
	}
	
	return this->M_parser.Eval();
}

#endif /* HH_CARTESIANPARSERINTERFACE_HH */
