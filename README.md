# HHRecoUtil
An ATLAS analysis package built around the `NaiveConeMatch` class

## Useage
`HHRecotUtil` is meant to be used inside of the ATLAS analysis framework and be managed by RootCore with the rest of the standard analysis base release packages used for an analysis. The pacakge is built to deliver utilies of use for di-Higgs analysis, specifically, a class for a "naive cone matching" algorithm (NCM): `NaiveConeMatch`.

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
    massbuildNumber        = MyNCM.GetNumberBHClassBuiderJets(m_mcclasstool);
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
