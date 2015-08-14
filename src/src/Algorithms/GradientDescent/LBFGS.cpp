/*!
 * 
 *
 * \brief       LLBFGS
 * 
 * The Limited-Memory Broyden, Fletcher, Goldfarb, Shannon (LBFGS) algorithm
 * is a quasi-Newton method for unconstrained real-valued optimization.
 * See: http://en.wikipedia.org/wiki/LLBFGS for details.
 * 
 * 
 *
 * \author      S. Dahlgaard, O.Krause
 * \date        2013
 *
 *
 * \par Copyright 1995-2015 Shark Development Team
 * 
 * <BR><HR>
 * This file is part of Shark.
 * <http://image.diku.dk/shark/>
 * 
 * Shark is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Shark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Shark.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 #define SHARK_COMPILE_DLL
#include <shark/Algorithms/GradientDescent/LBFGS.h>

using namespace shark;

void LBFGS::initModel(){
        m_hdiag = 1.0;         // Start with the identity
        m_updThres = 1e-10;       // Reasonable threshold
        
        m_gradientDifferences.clear();
        m_steps.clear();
}
void LBFGS::computeSearchDirection(){
        // Update the history if necessary
        RealVector y = m_derivative - m_lastDerivative;
        RealVector s = m_best.point - m_lastPoint;
        updateHist(y, s);
        
        getDirection(m_searchDirection);
}

//from ISerializable
void LBFGS::updateHist(RealVector& y, RealVector &step) {
        //Only update if <y,s> is above some reasonable threshold.
        double ys = inner_prod(y, step);
        if (ys > m_updThres) {
                // Only store m_numHist steps, so possibly pop the oldest.
                if (m_steps.size() >= m_numHist) {
                        m_steps.pop_front();
                        m_gradientDifferences.pop_front();
                }
                m_steps.push_back(step);
                m_gradientDifferences.push_back(y);
                // Update the hessian approximation.
                m_hdiag = ys / inner_prod(y,y);
        }
}

void LBFGS::getDirection(RealVector& searchDirection) {
        
        RealVector rho(m_numHist);
        RealVector alpha(m_numHist);
        RealVector beta(m_numHist);

        for (size_t i = 0; i < m_steps.size(); ++i)
                rho(i) = 1.0 / inner_prod(m_gradientDifferences[i], m_steps[i]);

        searchDirection = -m_derivative;
        for (int i = m_steps.size() - 1; i >= 0; --i) {
                alpha(i) = rho(i) * inner_prod(m_steps[i], searchDirection);
                searchDirection -= alpha(i) * m_gradientDifferences[i];
        }
        searchDirection *= m_hdiag;
        for (size_t i = 0; i < m_steps.size(); ++i) {
                beta(i) = rho(i) * inner_prod(m_gradientDifferences[i], searchDirection);
                searchDirection += m_steps[i] * (alpha(i) - beta(i));
        }
}