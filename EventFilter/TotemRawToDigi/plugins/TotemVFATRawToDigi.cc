/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/TotemDigi/interface/TotemRPDigi.h"
#include "DataFormats/TotemDigi/interface/TotemVFATStatus.h"

#include "CondFormats/DataRecord/interface/TotemReadoutRcd.h"
#include "CondFormats/TotemReadoutObjects/interface/TotemDAQMapping.h"
#include "CondFormats/TotemReadoutObjects/interface/TotemAnalysisMask.h"

#include "EventFilter/TotemRawToDigi/interface/SimpleVFATFrameCollection.h"
#include "EventFilter/TotemRawToDigi/interface/RawDataUnpacker.h"
#include "EventFilter/TotemRawToDigi/interface/RawToDigiConverter.h"

#include <string>

//----------------------------------------------------------------------------------------------------

class TotemRawToDigi : public edm::one::EDProducer<>
{
  public:
    explicit TotemRawToDigi(const edm::ParameterSet&);
    ~TotemRawToDigi();

    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endJob();

  private:
    // TODO: for testing with TOTEM-standalone data only,
    //  eventually to be removed, see comment in TotemRawToDigi::produce
    std::vector<unsigned int> fedIds;

    edm::EDGetTokenT<FEDRawDataCollection> fedDataToken;

    /// product labels
    std::string rpDataProductLabel;
    std::string conversionStatusLabel;

    RawDataUnpacker rawDataUnpacker;
    RawToDigiConverter rawToDigiConverter;
};

//----------------------------------------------------------------------------------------------------

using namespace edm;
using namespace std;

//----------------------------------------------------------------------------------------------------

TotemRawToDigi::TotemRawToDigi(const edm::ParameterSet &conf):
  fedIds(conf.getParameter< vector<unsigned int> >("fedIds")),
  rawDataUnpacker(conf.getParameterSet("RawUnpacking")),
  rawToDigiConverter(conf.getParameterSet("RawToDigi"))
{
  fedDataToken = consumes<FEDRawDataCollection>(conf.getParameter<edm::InputTag>("rawDataTag"));

  // RP data
  rpDataProductLabel = conf.getUntrackedParameter<std::string>("rpDataProductLabel", "");
  produces< DetSetVector<TotemRPDigi> >(rpDataProductLabel);

  // status
  conversionStatusLabel = conf.getUntrackedParameter<std::string>("conversionStatusLabel", "");
  produces< DetSetVector<TotemVFATStatus> >(conversionStatusLabel);
}

//----------------------------------------------------------------------------------------------------

TotemRawToDigi::~TotemRawToDigi()
{
}

//----------------------------------------------------------------------------------------------------

void TotemRawToDigi::produce(edm::Event& event, const edm::EventSetup &es)
{
  // get DAQ mapping
  ESHandle<TotemDAQMapping> mapping;
  es.get<TotemReadoutRcd>().get(mapping);

  // get analysis mask to mask channels
  ESHandle<TotemAnalysisMask> analysisMask;
  es.get<TotemReadoutRcd>().get(analysisMask);

  // raw data handle
  edm::Handle<FEDRawDataCollection> rawData;
  event.getByToken(fedDataToken, rawData);

  // book output products
  auto_ptr< DetSetVector<TotemRPDigi> > rpDataOutput(new DetSetVector<TotemRPDigi>);  
  auto_ptr< DetSetVector<TotemVFATStatus> > conversionStatus(new DetSetVector<TotemVFATStatus>);

  // step 1: raw-data unpacking
  SimpleVFATFrameCollection vfatCollection;

  // TODO: replace fedIds with real FED Ids read from
  //  DataFormats/FEDRawData/interface/FEDNumbering.h
  /* Hints from Michele
      DEVICE      ID     DETECTOR          OLD ID
      Trigger     577    LONEG             0x29c
      RX 1        578    5-6 210m FAR      0x1a1
      RX 2        579    5-6 210m NEAR     0x1a2
      RX 3        580    4-5 210m FAR      0x1a9
      RX 4        581    4-5 210m NEAR     0x1aa  
  */
  for (const auto &fedId : fedIds)
  {
    rawDataUnpacker.Run(fedId, rawData->FEDData(fedId), vfatCollection);
  }

  // step 2: raw to digi
  rawToDigiConverter.Run(vfatCollection, *mapping, *analysisMask,
    *rpDataOutput, *conversionStatus);

  // commit products to event
  event.put(rpDataOutput, rpDataProductLabel);
  event.put(conversionStatus, conversionStatusLabel);
}

//----------------------------------------------------------------------------------------------------

void TotemRawToDigi::endJob()
{
  rawToDigiConverter.PrintSummaries();
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(TotemRawToDigi);