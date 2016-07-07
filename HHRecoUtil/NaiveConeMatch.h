#ifndef NAIVECONEMATCH_H
#define NAIVECONEMATCH_H

#include <iostream>
#include <vector>
#include <algorithm>

#include <TLorentzVector.h>

#include "MCTruthClassifier/MCTruthClassifier.h"

class NaiveConeMatch {
private:
        static constexpr Double_t MeVtoGeV { 0.001 };
        std::vector<const xAOD::Jet*> m_conematchedjets;
        std::vector<const xAOD::Jet*> m_mjjbuilderjets;
        std::vector<const xAOD::Jet*> m_Bclassifiedjets;
        std::vector<const xAOD::Jet*> m_Hclassifiedjets;
        Double_t m_partnerConeR;
        Double_t m_mPole;
        Double_t m_mWidth;
        Double_t m_firstHiggsCone;
        Double_t m_m_j1j2_cone { -999999. };
        Double_t m_m_j1j2_cone_massCut { -999999. };
        Double_t m_m_j3j4_cone { -999999. };
        Double_t m_m_j3j4_cone_massCut { -999999. };

        Int_t m_BclassifiedNumber { -1 };
        Int_t m_HclassifiedNumber { -1 };

        std::vector<std::pair<const xAOD::Jet*, Int_t> > m_jheritage;

        static bool jetpTCompare (const xAOD::Jet *a, const xAOD::Jet *b) {
                return a->pt() > b->pt();
        }

        void SetPartnerConeR (const Double_t &R) {
                m_partnerConeR = R;
        }
        void SetMPole (const Double_t &m) {
                m_mPole = m;
        }
        void SetMWidth (const Double_t &m) {
                m_mWidth = m;
        }
        void SetFirstHiggsCone (const Double_t &R) {
                m_firstHiggsCone = R;
        }
        void SetMj1j2Cone (const Double_t &m) {
                m_m_j1j2_cone = m;
        }
        void SetMj1j2Cut (const Double_t &m) {
                m_m_j1j2_cone_massCut = m;
        }
        void SetMj3j4Cone (const Double_t &m) {
                m_m_j3j4_cone = m;
        }
        void SetMj3j4Cut (const Double_t &m) {
                m_m_j3j4_cone_massCut = m;
        }

public:
        NaiveConeMatch ();
        NaiveConeMatch (Double_t partnerConeR);
        NaiveConeMatch (Double_t mPole, Double_t mWidth);
        NaiveConeMatch (Double_t partnerConeR, Double_t mPole, Double_t mWidth,
                        Double_t firstHiggsCone);
        ~NaiveConeMatch ();

        Double_t GetPartnerConeR () {
                return m_partnerConeR;
        }
        Double_t GetMPole() {
                return m_mPole;
        }
        Double_t GetMWidth (){
                return m_mWidth;
        }
        Double_t GetFirstHiggsCone () {
                return m_firstHiggsCone;
        }
        Double_t GetMj1j2Cone (){
                return m_m_j1j2_cone;
        }
        Double_t GetMj1j2Cut (){
                return m_m_j1j2_cone_massCut;
        }
        Double_t GetMj3j4Cone (){
                return m_m_j3j4_cone;
        }
        Double_t GetMj3j4Cut (){
                return m_m_j3j4_cone_massCut;
        }
        std::vector<const xAOD::Jet*> GetBClassJets() {
                return m_Bclassifiedjets;
        }
        std::vector<const xAOD::Jet*> GetHClassJets() {
                return m_Hclassifiedjets;
        }
        Int_t GetNumberBClass() {
                return m_BclassifiedNumber;
        }
        Int_t GetNumberHClass() {
                return m_HclassifiedNumber;
        }
        Int_t GetNumberBHClassBuilderJets (MCTruthClassifier *mcclasstool = nullptr);
        Int_t GetNumberJetPairs ();

        std::vector<const xAOD::Jet*> GetNCMJets (const std::vector<const xAOD::Jet*> &v);

        void ClassifyNCMJets (const std::vector<const xAOD::Jet*> &v,
                              MCTruthClassifier *mcclasstool = nullptr);

        std::vector<const xAOD::Jet*> GetMjjBuilderJets (){
                return m_mjjbuilderjets;
        }
};

#endif
