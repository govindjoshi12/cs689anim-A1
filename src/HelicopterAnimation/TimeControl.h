#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class TimeControlFunction 
{
public: EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    TimeControlFunction();
    virtual ~TimeControlFunction();
    float computeCubic(float x);

private:
    bool DERIV = true;
    bool OG = false;
    bool APPEND = true;
    bool PREPEND = false;
    bool NEGATE = true;

    float xmid = 0.4f;

    Eigen::VectorXf coeffs;
    Eigen::Vector4f coefficientBlock(float tVal, bool deriv, bool negate = false);
    Eigen::VectorXf createCoefficientRow(float tVal, bool deriv, bool append);
    Eigen::VectorXf createJoinedRow(float leftTval, float rightTval, bool leftDeriv, bool rightDeriv);
    void solveCustomConstraints();
};
