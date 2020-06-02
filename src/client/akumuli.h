//
// Created by Rakesh on 02/06/2020.
//

#pragma once

#include "model/defs.h"
#include "model/config.h"

namespace spt::client::akumuli
{
  std::string query( const model::Configuration* configuration, const model::Query& query );
  std::string annotations( const model::Configuration* configuration, const model::AnnotationsReq& request );
  std::string search( const model::Configuration* configuration, const model::Query& query );
}
