//
// Created by Rakesh on 01/06/2020.
//

#pragma once

#include "lithium_json.h"

LI_SYMBOL(from)
LI_SYMBOL(to)
LI_SYMBOL(datasource)
LI_SYMBOL(iconColor)
LI_SYMBOL(enable)
LI_SYMBOL(showLine)
LI_SYMBOL(query)
LI_SYMBOL(range)
LI_SYMBOL(annotation)
LI_SYMBOL(key)
LI_SYMBOL(value)
LI_SYMBOL(time)
LI_SYMBOL(title)
LI_SYMBOL(tags)
LI_SYMBOL(text)
LI_SYMBOL(target)
LI_SYMBOL(adhocFilters)
LI_SYMBOL(refId)
LI_SYMBOL(panelId)
LI_SYMBOL(interval)
LI_SYMBOL(intervalMs)
LI_SYMBOL(targets);
LI_SYMBOL(format);
LI_SYMBOL(maxDataPoints);
LI_SYMBOL(coordinates);
LI_SYMBOL(timestamp);
LI_SYMBOL(metadata);
LI_SYMBOL(columns);
LI_SYMBOL(rows);
LI_SYMBOL(code);
LI_SYMBOL(cause);

LI_SYMBOL(raw);
LI_SYMBOL(rangeRaw);

namespace spt::model
{
  struct RangeRaw
  {
    std::string from;
    std::string to;
  };

  struct Range
  {
    std::string from;
    std::string to;
    RangeRaw raw;
  };

  struct Annotation
  {
    std::string name;
    std::string datasource;
    std::string iconColor;
    bool enable;
    bool showLine;
    std::string query;
  };

  struct AnnotationsReq
  {
    Range range;
    Annotation annotation;
  };

  struct Tag
  {
    std::string key;
    std::string value;
  };

  struct AnnotationResponse
  {
    // The original annotation sent from Grafana.
    Annotation annotation;
    // Time since UNIX Epoch in milliseconds. (required)
    int64_t time;
    // The title for the annotation tooltip. (required)
    std::string title;
    // Tags for the annotation. (optional)
    std::vector<Tag> tags;
    // Text for the annotation. (optional)
    std::string text;
  };

  struct Target
  {
    std::string target;
    std::string refId;
    std::string type;
  };

  struct Query
  {
    int panelId;
    Range range;
    RangeRaw rangeRaw;
    std::string interval;
    int intervalMs;
    std::vector<Target> targets;
    std::unordered_map<std::string, std::string> adhocFilters;
    std::string format;
    int maxDataPoints;
  };

  struct LocationValue
  {
    std::string type;
    std::vector<double> coordinates;
  };

  struct Timestamp
  {
    std::string type;
    std::string value;
  };

  struct Metadata
  {
    Timestamp timestamp;
  };

  struct Column
  {
    std::string text;
    std::string type;
  };

  struct Row
  {
    std::string type;
    LocationValue value;
    Metadata metadata;
  };

  struct LocationResponse
  {
    std::vector<Column> columns;
    std::vector<std::vector<Row>> rows;
    std::string type;
  };

  struct TagKey
  {
    std::string text;
    std::string type;
  };

  struct TagValue
  {
    std::string key;
  };

  struct Error
  {
    // Usually a HTTP status code associated with the error.
    int code;

    // A context specific error string describing the cause of the error.
    std::string cause;
  };
}
