# HHRecoUtil
An ATLAS analysis package built around the `NaiveConeMatch` class

## Class Reference

### Public Member Functions
* `NaiveConeMatch ()`
   * Default constructor
* `NaiveConeMatch (Double_t partnerConeR)`
   * Constructor
* `NaiveConeMatch (Double_t mPole, Double_t mWidth)`
   * Constructor
* `NaiveConeMatch (Double_t partnerConeR, Double_t mPole, Double_t mWidth, Double_t gammagammaCone)`
   * Constructor
* `~NaiveConeMatch ()`
   * Destructor
* `Double_t GetPartnerConeR ()`
  * Return the R of the cone searched in to find the partner jet of the leading pT jet
* `Double_t GetMPole ()`
  * Return the mean of the mass window for the m_j1j2 mass cut
* `Double_t GetMWidth ()`
  * Return the half width of the mass window for the m_j1j2 mass cut (FWMH/2)
* `Double_t GetGammaGammaCone ()`
  * Return the R of the cone built around the inverse direction of the object formed from j1 and j2
* `Double_t GetMj1j2Cone ()`
  * Return the invariant mass of the object formed from j1 and j2
* `Double_t GetMj1j2Cut ()`
  * Return the invariant mass of the object formed from j1 and j2 if the mass lies in the mass window
* `Double_t GetMj3j4Cone ()`
  * Return the invariant mass of the object formed from j3 and j4
* `Double_t GetMj3j4Cut ()`
  * Return the invariant mass of the object formed from j3 and j4 if the mass lies in the mass window
* `std::vector<const xAOD::Jet*> GetBClassJets ()`
  * Return a vector of pointers to NCM passing jets that have been classified as BJEt
* `std::vector<const xAOD::Jet*> GetHClassJets ()`
  * Return a vector of pointers to NCM passing jets that have been classified as having origin on Higgs
* `Int_t GetNumberBClass ()`
  * Return the number of NCM passing jets that have been classified as BJEt
* `Int_t GetNumberHClass ()`
  * Return the number of NCM passing jets that have been classified as having origin on Higgs
* `Int_t GetNumberBHClassBuilderJets (MCTruthClassifier *mcclasstool = nullptr)`
  * Return the number of NCM BJet and Higgs classified jets that were used to build the objects j1j2 and j3j4
* `Int_t GetNumberPairedJets ()`
  * Return the number of jets (not pairs) whose parent is the same as another selected jet (the parents barcodes match)
* `std::vector<const xAOD::Jet*> GetNCMJets (const std::vector<const xAOD::Jet*> &v)`
  * Return a vector of pointers to jets that pass the NCM selection
* `void ClassifyNCMJets (const std::vector<const xAOD::Jet*> &v, MCTruthClassifier *mcclasstool = nullptr)`
  * Apply `GetNCMJets` to a vector of pointers to jets (if not already done) and apply the `MCTruthClassifier` to the jets that pass the NCM
* `std::vector<const xAOD::Jet*> GetMjjBuilderJets ()`
  * Return a vector of pointers to jets that were used to build the objects j1j2 and j3j4

## Useage
`HHRecotUtil` is meant to be used inside of the ATLAS analysis framework and be managed by RootCore<sup id="ref1">[1](#footnote1)</sup> with the rest of the standard analysis base release packages used for an analysis. The pacakge is built to deliver utilies of use for di-Higgs analysis, specifically, a class for a "naive cone matching" algorithm (NCM): `NaiveConeMatch`.

### Example:
**In the header of your `EventLoop` based analysis package:**

As `NaiveConeMatch` is dependant on `MCTruthClassifier` (along with the relevant `#inlcude`statements) instantiate an instance of `MCTruthClassifier` in the body of your analysis package class:
```C++
// ...
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "HHRecoUtil/NaiveConeMatch.h"
// ...
class HH4bCorrelationsAnalysis : public EL::Algorithm{
  // ...
  MCTruthClassifier *mcclasstool = nullptr; //!
  // ...
};
```
**In the source of your `EventLoop` based analysis package:**

Initialize the instance of `MCTruthClassifier` in the `histInitialize` function of the `EventLoop` algorithm.
```C++
EL::StatusCode MyAnalysis::histInitialize () {

  // ...

  mcclasstool = new MCTruthClassifier("mcclasstool");

  // ...
  return EL::StatusCode::SUCCESS;
}
```
In the body of your analysis use it to fill branches for your output TTree:
```C++
  std::vector<const xAOD::Jet*> fiducialjets;

  // ... fill fiducialjets ...

  NaiveConeMatch MyNCM;
  MyNCM.ClassifyNCMJets(fiducialjets, mcclasstool);
  m_j1j2_cone         = MyNCM.GetMj1j2Cone();
  m_j1j2_cone_massCut = MyNCM.GetMj1j2Cut();
  m_j3j4_cone         = MyNCM.GetMj3j4Cone();
  m_j3j4_cone_massCut = MyNCM.GetMj3j4Cut();

  if (m_j1j2_cone_massCut > 0) {
    conematchedNumber      = MyNCM.GetNumberBClass();
    conematchedHiggsNumber = MyNCM.GetNumberHClass();
    massbuildNumber        = MyNCM.GetNumberBHClassBuilderJets(m_mcclasstool);
    pairedjetNumber        = MyNCM.GetNumberPairedJets(); // number of jets with pair, not number of pairs
  }
```
**In the steering file in your `util` dir:**
```C++
// ...
#include "HHRecoUtil/NaiveConeMatch.h"
// ...
```
**In the RootCore Makefile for your `EventLoop` based analysis package:**

Add `NaiveConeMatch` to the list of package dependencies:
```C++
// ...
PACKAGE_DEP = ... MCTruthClassifier NaiveConeMatch
// ...
```
## References
<a name="footnote1">1</a>: [RootCore Twiki](https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RootCore "RootCore Twiki") [↩](#ref1)
