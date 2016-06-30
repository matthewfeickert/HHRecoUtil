#include "HHRecoUtil/NaiveConeMatch.h"

// constructor
NaiveConeMatch::NaiveConeMatch () {
  SetPartnerConeR(3.0);
  SetMPole(120.);
  SetMWidth(20.);
  SetGammaGammaCone(3.0);
}

NaiveConeMatch::NaiveConeMatch (Double_t partnerConeR) : NaiveConeMatch() {
  SetPartnerConeR(partnerConeR);
}

NaiveConeMatch::NaiveConeMatch (Double_t mPole,
                                Double_t mWidth) : NaiveConeMatch() {
  SetMPole(mPole);
  SetMWidth(mWidth);
}

NaiveConeMatch::NaiveConeMatch (Double_t partnerConeR,
                                Double_t mPole,
                                Double_t mWidth,
                                Double_t gammagammaCone) {
  SetPartnerConeR(partnerConeR);
  SetMPole(mPole);
  SetMWidth(mWidth);
  SetGammaGammaCone(gammagammaCone);
}

// destructor
NaiveConeMatch::~NaiveConeMatch () {}

/* Apply a Naive Cone Matching algorithm to a vector of jet pointers and return the jets
   that passed the algorithm as a vector of jet pointers. In doing so also find the
   invariant mass of j1j2 and j3j4 pre and post mass window cut.*/
std::vector<const xAOD::Jet*> NaiveConeMatch::GetNCMJets (
  const std::vector<const xAOD::Jet*> &v) {
  std::vector<const xAOD::Jet*> searchjets(v);

  // sort the vector of jets by their pT from greatest to least
  std::sort(searchjets.begin(), searchjets.end(), jetpTCompare);

  m_mjjbuilderjets.clear();

  if (searchjets.size() > 3) {
    // store the highest pT jet
    m_conematchedjets.push_back(searchjets[0]);
    searchjets.erase(std::remove(searchjets.begin(), searchjets.end(),
                                 searchjets[0]), searchjets.end());

    if (m_conematchedjets.size() > 0) {
      // find the second highest pT

      for (const auto &jet_itr : searchjets) {
        if ((m_conematchedjets[0]->p4().DeltaR(jet_itr->p4()) < m_partnerConeR)
            && (jet_itr->pt() < m_conematchedjets[0]->pt())) {
          m_conematchedjets.push_back(jet_itr);
          searchjets.erase(std::remove(searchjets.begin(), searchjets.end(),
                                       jet_itr), searchjets.end());

          if (m_conematchedjets.size() > 1) break;
        }
      }

      if (m_conematchedjets.size() > 1) {
        TLorentzVector higgsDirection { m_conematchedjets[0]->p4() +
                                        m_conematchedjets[1]->p4() };
        SetMj1j2Cone(higgsDirection.M() * MeVtoGeV);

        if ((higgsDirection.M() * MeVtoGeV >= (m_mPole - m_mWidth)) &&
            (higgsDirection.M() * MeVtoGeV <= (m_mPole + m_mWidth))) {
          SetMj1j2Cut(GetMj1j2Cone());
          higgsDirection.SetVect(-1. * higgsDirection.Vect());

          for (const auto &jet_itr : searchjets) {
            if (higgsDirection.DeltaR(
                  jet_itr->p4()) < m_gammagammaCone) m_conematchedjets.push_back(jet_itr);
          }

          if (m_conematchedjets.size() > 3) {
            TLorentzVector h34Cone { m_conematchedjets[2]->p4() +
                                     m_conematchedjets[3]->p4() };
            SetMj3j4Cone(h34Cone.M() * MeVtoGeV);
            std::copy(m_conematchedjets.begin(), m_conematchedjets.begin() + 4,
                      std::back_inserter(m_mjjbuilderjets));

            if ((m_m_j3j4_cone >= (m_mPole - m_mWidth))
                && (m_m_j3j4_cone <= (m_mPole + m_mWidth))) SetMj3j4Cut(GetMj3j4Cone());
          }
        }
      } // m_conematchedjets > 1
    }   // m_conematchedjets > 0
  }     // searchjets > 3

  return m_conematchedjets;
} // GetNCMJets

/* Apply a Naive Cone Matching algorithm if needed and then use the MCTruthClassifier
   class to classify the selected jets, placing those classified as BJets in a vector of
   jet pointers and those classified as BJets with a Higgs as an origin into another
   vector of jet pointers.*/
void NaiveConeMatch::ClassifyNCMJets (
  const std::vector<const xAOD::Jet*> &v, MCTruthClassifier *mcclasstool = nullptr) {
  std::vector<const xAOD::Jet*> conematchedjets;

  // if passed m_conematchedjets don't redo NCM
  if (&v == &m_conematchedjets) conematchedjets = v;
  else conematchedjets = GetNCMJets(v);

  if (mcclasstool != nullptr) {
    m_Bclassifiedjets.clear();
    m_Hclassifiedjets.clear();
    m_jheritage.clear();

    m_BclassifiedNumber = 0;
    m_HclassifiedNumber = 0;

    std::pair<MCTruthPartClassifier::ParticleType,
              MCTruthPartClassifier::ParticleOrigin> classifyResult;

    for (const auto &jet_itr : conematchedjets) {
      classifyResult = mcclasstool->particleTruthClassifier(jet_itr, true);

      if (classifyResult.first == MCTruthPartClassifier::BJet) {
        ++m_BclassifiedNumber;
        m_Bclassifiedjets.push_back(jet_itr);

        if (classifyResult.second == MCTruthPartClassifier::Higgs) {
          ++m_HclassifiedNumber;
          m_Hclassifiedjets.push_back(jet_itr);

          if (mcclasstool->getMotherPDG() == 25) m_jheritage.emplace_back(
              jet_itr, mcclasstool->getMotherBarcode());
        }
      }
    }
  }
} // ClassifyNCMJets

/* Return the number of jets that were used to make m_j1j2 and m_j3j4 that also were
   classified by MCTruthClassifier as BJets with Higgs origin.*/
Int_t NaiveConeMatch::GetNumberBHClassBuilderJets (
  MCTruthClassifier *mcclasstool = nullptr) {
  Int_t BHclassifiedbuilderNumber { 0 };

  if (mcclasstool != nullptr) {
    std::pair<MCTruthPartClassifier::ParticleType,
              MCTruthPartClassifier::ParticleOrigin> classifyResult;

    if (m_mjjbuilderjets.size() > 0) {
      for (const auto &jet_itr : m_mjjbuilderjets) {
        classifyResult = mcclasstool->particleTruthClassifier(jet_itr, true);

        if ((classifyResult.first == MCTruthPartClassifier::BJet) &&
            (classifyResult.second ==
             MCTruthPartClassifier::Higgs)) ++BHclassifiedbuilderNumber;
      }
    }
  }

  return BHclassifiedbuilderNumber;
} // GetNumberBHClassBuilderJets

/* Return the number of jets that were classified as BJets with Higgs origin that were
   matched with a jet that comes from the same parent Higgs (and so has formed a
   correct pair).*/
Int_t NaiveConeMatch::GetNumberPairedJets () {
  Int_t jetswithpairsNumber { 0 }; // number of jets with pair, not number of pairs

  if (m_jheritage.size() > 0) {
    for (UInt_t i = 0; i < m_jheritage.size(); ++i) {
      for (UInt_t j = 0; j < m_jheritage.size(); ++j) {
        if ((m_jheritage[i] != m_jheritage[j]) &&
            (m_jheritage[i].second == m_jheritage[j].second)) ++jetswithpairsNumber;
      }
    }
  }

  return jetswithpairsNumber;
} // GetNumberPairedJets
