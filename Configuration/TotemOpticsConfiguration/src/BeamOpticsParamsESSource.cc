#include "Configuration/TotemOpticsConfiguration/interface/BeamOpticsParamsESSource.h"

#include <iostream>
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/SourceFactory.h"


BeamOpticsParamsESSource::BeamOpticsParamsESSource(const edm::ParameterSet& conf) 
: conf_(conf)
{
  edm::LogInfo("BeamOpticsParamsESSource::BeamOpticsParamsESSource");

  setWhatProduced(this);
  findingRecord<BeamOpticsParamsRcd>();
}

BeamOpticsParamsESSource::~BeamOpticsParamsESSource()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}

std::unique_ptr<BeamOpticsParams> BeamOpticsParamsESSource::produce(const BeamOpticsParamsRcd &)
{
  std::unique_ptr<BeamOpticsParams> obj( new BeamOpticsParams(conf_) );

  std::cout << ">> BeamOpticsParamsESSource::produce : Loaded optics with energy " << obj->GetBeamEnergy()
	  << " GeV and beta* " << obj->GetBetaStarX() << "/" << obj->GetBetaStarY() << " m" << std::endl;

  return obj;
}

void BeamOpticsParamsESSource::setIntervalFor( const edm::eventsetup::EventSetupRecordKey&, 
    const edm::IOVSyncValue& iosv, 
    edm::ValidityInterval& oValidity ) 
{
  edm::ValidityInterval infinity( iosv.beginOfTime(), iosv.endOfTime() );
  oValidity = infinity;  
}

DEFINE_FWK_EVENTSETUP_SOURCE(BeamOpticsParamsESSource);
