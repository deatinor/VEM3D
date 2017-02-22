#ifndef HH_SPHERICALPARSERINTERFACE_HH
#define HH_SPHERICALPARSERINTERFACE_HH

#include "muParserInterface.h"

template <long embedded,typename real=double>
class sphericalParserInterface: public muParserInterface<embedded,real> {
	public:
		sphericalParserInterface();
		sphericalParserInterface(const std::string & s);
		sphericalParserInterface(sphericalParserInterface const &);
		sphericalParserInterface & operator=(sphericalParserInterface const &);
		
		virtual real operator()(const Point<embedded,real>& inputPoint, real const & t=0);
	private:
		double M_r, M_phi, M_theta;
};


template <long embedded,typename real>
sphericalParserInterface<embedded,real>::sphericalParserInterface(): muParserInterface<embedded,real>() {
	this->M_parser.DefineVar("r",&M_r);
	this->M_parser.DefineVar("phi",&M_phi);
	this->M_parser.DefineVar("theta",&M_theta);
}

template <long embedded,typename real>
sphericalParserInterface<embedded,real>::sphericalParserInterface(const std::string & e): 
sphericalParserInterface() {
	muParserInterface<embedded,real>::set_expression(e);
}

template <long embedded,typename real>
sphericalParserInterface<embedded,real>::sphericalParserInterface(sphericalParserInterface const & mpi):
muParserInterface<embedded,real>(mpi),M_r(mpi.M_r),M_phi(mpi.M_phi),M_theta(mpi.M_theta) {
	this->M_parser.DefineVar("r",&M_r);
	this->M_parser.DefineVar("phi",&M_phi);  
	this->M_parser.DefineVar("theta",&M_theta);  
}


template <long embedded,typename real>
sphericalParserInterface<embedded,real> & sphericalParserInterface<embedded,real>::operator=(sphericalParserInterface<embedded,real> const & mpi) {
	if (this != &mpi) {
		this->M_parser.ClearVar();
		this->M_expr = mpi.M_expr;
		this->M_t = mpi.M_t;
		this->M_r = mpi.M_r;
		this->M_phi = mpi.M_phi;
		this->M_theta = mpi.M_theta;
		this->M_parser.SetExpr(this->M_expr);
		this->M_parser.DefineVar("t",&(this->M_t));
		this->M_parser.DefineVar("r",&M_r);
		this->M_parser.DefineVar("phi",&M_phi);
		this->M_parser.DefineVar("theta",&M_theta);
	}
	return *this;
}

template <long embedded,typename real>
real sphericalParserInterface<embedded,real>::operator()(const Point<embedded,real>& inputPoint, real const & t) {
	this->M_t = t;
	this->M_r = inputPoint[0];
	if (embedded == 2) {
		this->M_r = sqrtl(inputPoint[0]*inputPoint[0]+inputPoint[1]*inputPoint[1]);
		if (inputPoint[0]>1e-10 && inputPoint[0]<-1e-10)
			this->M_phi = atan2l(inputPoint[1],inputPoint[0]);
		else if (inputPoint[1] >= 0)
			this->M_phi = 0;
		else 
			this->M_phi = M_PI;
	} else if (embedded > 2) {
		real r = sqrtl(inputPoint[0]*inputPoint[0]+inputPoint[1]*inputPoint[1]+inputPoint[2]*inputPoint[2]);
		this->M_r = r;
		if (inputPoint[0]>1e-10 && inputPoint[0]<-1e-10)
			this->M_phi = atan2l(inputPoint[1],inputPoint[0]);
		else if (inputPoint[1] >= 0)
			this->M_phi = 0;
		else 
			this->M_phi = M_PI;
		if (r>1e-10 && r<-1e-10)
			this->M_theta = acosl(inputPoint[2]/r); 
		else 
			this->M_theta = 0;
	}
	
	return this->M_parser.Eval();
}

#endif
