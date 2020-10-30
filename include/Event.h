// Martin Duy Tat 30th October 2020
/**
 * Event is a class for storing the four-momenta of daugher particles in a D->KKpipi decay.
 */

#ifndef EVENT_H
#define EVENT_H

#include<vector>
#include"TLorentzVector.h"

class Event {
  public:
    /**
     * Default constructor with zero particles
     */
    Event();
    /**
     * Constructor that takes a vector of four-momenta
     * @param p Four-momenta in the form (E, px, py, pz), in the order K+ K- pi+ pi-
     */
    Event(const std::vector<double> &p);
    /**
     * Copy constructor
     */
    Event(const Event &event);
    /**
     * Constructor that takes a vector of four-momenta
     * @param p Vector of TLorentzVector objects, in the order K+ K- pi+ pi-
     */
    Event(const std::vector<TLorentzVector> &p);
    /**
     * Returns the four-momenta of daughter particles as a vector
     * @return Four-momenta of daughter particles in the form (E, px, py, pz), in the order K+ K- pi+ pi-
     */
    std::vector<double> GetEventVector();
    /**
     * Returns the four-momenta of daughter particles as a vector of TLorentzVector objects
     * @return Four-momenta of daughter particles in the order K+ K- pi+ pi-
     */
    std::vector<TLorentzVector> GetEventLorentzVector();
    /**
     * Function for getting invariant mass of two particles
     * @param particle1 Particle 0(K+), 1(K-), 2(pi+), 3(pi-)
     * @param particle2 Particle 0(K+), 1(K-), 2(pi+), 3(pi-)
     * @return Returns invariant mass of given particles
     */
    double GetInvMass(int particle1, int particle2);
    /**
     * Function for getting invariant mass of three particles
     * @param particle1 Particle 0(K+), 1(K-), 2(pi+), 3(pi-)
     * @param particle2 Particle 0(K+), 1(K-), 2(pi+), 3(pi-)
     * @param particle3 Particle 0(K+), 1(K-), 2(pi+), 3(pi-)
     * @return Returns mass of given particles
     */
    double GetInvMass(int particle1, int particle2, int particle3);
  private:
    /**
     * Four-momenta of daughter particles
     */
    std::vector<double> m_momenta;
};

#endif
