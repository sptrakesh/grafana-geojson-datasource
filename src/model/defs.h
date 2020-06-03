//
// Created by Rakesh on 01/06/2020.
//

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace spt::model
{
  struct Range
  {
    Range() = default;
    ~Range() = default;
    Range( Range&& ) = default;
    Range& operator=( Range&& ) = default;
    Range( const Range& ) = delete;
    Range& operator=( const Range& ) = delete;

    int64_t fromNs() const;
    int64_t toNs() const;

    std::string from;
    std::string to;
  };

  struct Annotation
  {
    Annotation() = default;
    ~Annotation() = default;
    Annotation( Annotation&& ) = default;
    Annotation& operator=( Annotation&& ) = default;
    Annotation( const Annotation& ) = delete;
    Annotation& operator=( const Annotation& ) = delete;

    std::string name;
    std::string datasource;
    std::string iconColor;
    bool enable;
    bool showLine;
    std::string query;
  };

  struct AnnotationsReq
  {
    AnnotationsReq() = default;
    ~AnnotationsReq() = default;
    AnnotationsReq( AnnotationsReq&& ) = default;
    AnnotationsReq& operator=( AnnotationsReq&& ) = default;
    AnnotationsReq( const AnnotationsReq& ) = delete;
    AnnotationsReq& operator=( const AnnotationsReq& ) = delete;

    Range range;
    Annotation annotation;
  };

  struct Tag
  {
    Tag() = default;
    ~Tag() = default;
    Tag( Tag&& ) = default;
    Tag& operator=( Tag&& ) = default;
    Tag( const Tag& ) = delete;
    Tag& operator=( const Tag& ) = delete;

    std::string key;
    std::string value;
  };

  struct AnnotationResponse
  {
    AnnotationResponse() = default;
    ~AnnotationResponse() = default;
    AnnotationResponse( AnnotationResponse&& ) = default;
    AnnotationResponse& operator=( AnnotationResponse&& ) = default;
    AnnotationResponse( const AnnotationResponse& ) = delete;
    AnnotationResponse& operator=( const AnnotationResponse& ) = delete;

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
    Target() = default;
    ~Target() = default;
    Target( Target&& ) = default;
    Target& operator=( Target&& ) = default;
    Target( const Target& ) = delete;
    Target& operator=( const Target& ) = delete;

    std::string target;
    std::string refId;
    std::string type;
  };

  struct Filter
  {
    std::string key;
    std::string oper;
    std::string value;
  };

  struct Query
  {
    Query() = default;
    explicit Query( std::string_view json );
    ~Query() = default;
    Query( Query&& ) = default;
    Query& operator=( Query&& ) = default;
    Query( const Query& ) = delete;
    Query& operator=( const Query& ) = delete;

    int panelId;
    Range range;
    std::string interval;
    int intervalMs;
    std::vector<Target> targets;
    std::vector<Filter> adhocFilters;
    std::string format;
    int maxDataPoints;
  };

  struct LocationValue
  {
    LocationValue() = default;
    ~LocationValue() = default;
    LocationValue( LocationValue&& ) = default;
    LocationValue& operator=( LocationValue&& ) = default;
    LocationValue( const LocationValue& ) = delete;
    LocationValue& operator=( const LocationValue& ) = delete;

    std::string type;
    std::vector<double> coordinates;
  };

  struct Timestamp
  {
    Timestamp() = default;
    ~Timestamp() = default;
    Timestamp( Timestamp&& ) = default;
    Timestamp& operator=( Timestamp&& ) = default;
    Timestamp( const Timestamp& ) = delete;
    Timestamp& operator=( const Timestamp& ) = delete;

    int64_t valueNs() const;

    std::string type;
    std::string value;
  };

  struct Metadata
  {
    Metadata() = default;
    ~Metadata() = default;
    Metadata( Metadata&& ) = default;
    Metadata& operator=( Metadata&& ) = default;
    Metadata( const Metadata& ) = delete;
    Metadata& operator=( const Metadata& ) = delete;

    Timestamp timestamp;
  };

  struct Column
  {
    Column() = default;
    ~Column() = default;
    Column( Column&& ) = default;
    Column& operator=( Column&& ) = default;
    Column( const Column& ) = delete;
    Column& operator=( const Column& ) = delete;

    std::string text;
    std::string type;
  };

  struct Row
  {
    Row() = default;
    explicit Row( std::string_view json );
    ~Row() = default;
    Row( Row&& ) = default;
    Row& operator=( Row&& ) = default;
    Row( const Row& ) = delete;
    Row& operator=( const Row& ) = delete;

    std::string type;
    LocationValue value;
    Metadata metadata;
  };

  struct LocationResponse
  {
    LocationResponse() = default;
    ~LocationResponse() = default;
    LocationResponse( LocationResponse&& ) = default;
    LocationResponse& operator=( LocationResponse&& ) = default;
    LocationResponse( const LocationResponse& ) = delete;
    LocationResponse& operator=( const LocationResponse& ) = delete;

    std::vector<Column> columns;
    std::vector<std::vector<Row>> rows;
    std::string type;
  };

  struct TagKey
  {
    TagKey() = default;
    ~TagKey() = default;
    TagKey( TagKey&& ) = default;
    TagKey& operator=( TagKey&& ) = default;
    TagKey( const TagKey& ) = delete;
    TagKey& operator=( const TagKey& ) = delete;

    std::string text;
    std::string type;
  };

  struct TagValue
  {
    TagValue() = default;
    ~TagValue() = default;
    TagValue( TagValue&& ) = default;
    TagValue& operator=( TagValue&& ) = default;
    TagValue( const TagValue& ) = delete;
    TagValue& operator=( const TagValue& ) = delete;

    std::string key;
  };

  struct Error
  {
    Error() = default;
    ~Error() = default;
    Error( Error&& ) = default;
    Error& operator=( Error&& ) = default;
    Error( const Error& ) = delete;
    Error& operator=( const Error& ) = delete;

    // Usually a HTTP status code associated with the error.
    int code;

    // A context specific error string describing the cause of the error.
    std::string cause;
  };
}
