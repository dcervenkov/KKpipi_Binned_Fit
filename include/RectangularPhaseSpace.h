// Martin Duy Tat 12th November 2020
/**
 * RectangularPhaseSpace is a class that contains the information about how phase space is divided into bins using a rectangular parameterisation
 */

#ifndef RECTANGULARPHASESPACE
#define RECTANGULARPHASESPACE

#include<vector>
#include<map>
#include"Event.h"
#include"PhaseSpaceParameterisation.h"

class RectangularPhaseSpace: public PhaseSpaceParameterisation {
  public:
    /**
     * Default constructor
     * @param bins A vector with the number of bins in each direction, such that the total number of bins is the product
     * @param masses An array of the \f$(D^0, K^+, K^-, \pi^+, \pi^-)\f$ masses, with default the same as Ampgen's values
     */
    RectangularPhaseSpace(std::vector<int> bins, double *masses = nullptr);
    /**
     * Function that converts an event into a vector of TLorentzVector objects
     */
    std::vector<TLorentzVector> ConvertTo4Vectors(const Event &event) const;
    /**
     * Function that determines the rectangular coordinates in phase space of an event
     */
    std::vector<double> RectCoordinates(const Event &event) const;
    /**
     * Function that determines which bin an event belongs to
     * @param event The event we want to determine the bin of
     * @return Bin number
     */
    int WhichBin(const Event &event) const;
    /**
     * Function that returns the number of bins in the binning scheme
     * @return Number of bins
     */
    int NumberOfBins() const;
  private:
    /** 
     * The lower boundary of phase space
     */
    std::vector<double> m_xlow;
    /**
     * The upper boundary of phase space
     */
    std::vector<double> m_xhigh;
    /**
     * Vector with the number of bins in each direction
     */
    std::vector<int> m_Binning;
    /**
     * A vector of maps that connect the upper bin edges with the bin numbers
     */
    std::vector<std::map<double, int>> m_BinMap;
    /**
     * A vector of masses of the \f$(D, K^+, K^-, \pi^+, \pi^-)\f$
     */
    std::vector<double> m_Masses;
};

#endif