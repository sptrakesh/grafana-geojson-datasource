//
// Created by Rakesh on 02/06/2020.
//

#pragma once

#include "model/defs.h"
#include "model/response.h"

namespace spt::client::akumuli
{
  model::Response query( const model::Query& query );
  model::Response annotations( const model::AnnotationsReq& request );
  model::Response search( const model::Target& target );
}
