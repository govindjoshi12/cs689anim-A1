#include "TimeControl.h"
#include <iostream>

using namespace std;

TimeControlFunction::TimeControlFunction() {
	// Functions written such that it can be generalized
	// to any 8 constraints with minimum additional effort.
	coeffs = Eigen::VectorXf(8);
	solveCustomConstraints();
}

TimeControlFunction::~TimeControlFunction() {

}

float TimeControlFunction::computeCubic(float x) {

	int addIdx = x >= xmid ? 4 : 0;

	float sum = 0.0f;
	for(int i = 0; i < 4; i++) {
		sum += coeffs(i + addIdx) * pow(x, 3 - i);
	}
	return sum;
}

Eigen::Vector4f TimeControlFunction::coefficientBlock(float tVal, bool deriv, bool negate) {
	Eigen::Vector4f block;
	block.setZero();
	int sign = negate ? -1 : 1;

	if(deriv) {
		block << sign * 3*pow(tVal, 2), 
			   sign * 2*tVal, 
			   sign, 
			   0;
	} else {
		block << sign * pow(tVal, 3), 
			   sign * pow(tVal, 2),
			   sign * tVal, 
			   sign * 1;
	}

	return block;
}

Eigen::VectorXf TimeControlFunction::createCoefficientRow(float tVal, bool deriv, bool append) {
	Eigen::VectorXf row(8);
	row.setZero();

	if(append) {
		row.segment<4>(0) = coefficientBlock(tVal, deriv);
	} else {
		row.segment<4>(4) = coefficientBlock(tVal, deriv);
	}


	return row;
}

Eigen::VectorXf TimeControlFunction::createJoinedRow(float leftTval, float rightTval, bool leftDeriv, bool rightDeriv) {
	
	Eigen::VectorXf row(8);
	row.setZero();

	row.segment<4>(0) = coefficientBlock(leftTval, leftDeriv);
	row.segment<4>(4) = coefficientBlock(rightTval, rightDeriv, NEGATE);
	return row;

}

void TimeControlFunction::solveCustomConstraints() {

	Eigen::VectorXf t(8);
	t << 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.4f, 0.4f;
	Eigen::VectorXf b(8);
	b << 0.0f, 0.0f, 0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f; 

	Eigen::MatrixXf A(8, 8);

	A.block<1, 8>(0, 0) = createCoefficientRow(t(0), OG, APPEND);
	A.block<1, 8>(1, 0) = createCoefficientRow(t(1), DERIV, APPEND);
	A.block<1, 8>(2, 0) = createCoefficientRow(t(2), OG, PREPEND);
	A.block<1, 8>(3, 0) = createCoefficientRow(t(3), DERIV, PREPEND);
	A.block<1, 8>(4, 0) = createCoefficientRow(t(4), OG, PREPEND);
	A.block<1, 8>(5, 0) = createCoefficientRow(t(5), DERIV, PREPEND);
	A.block<1, 8>(6, 0) = createJoinedRow(t(6), t(6), OG, OG);
	A.block<1, 8>(7, 0) = createJoinedRow(t(7), t(7), DERIV, DERIV);

	coeffs = A.colPivHouseholderQr().solve(b);
}