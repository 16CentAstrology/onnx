// Copyright (c) ONNX Project Contributors

/*
 * SPDX-License-Identifier: Apache-2.0
 */

// ATTENTION: The code in this file is highly EXPERIMENTAL.
// Adventurous users should note that the APIs will probably change.

#pragma once

#include <cmath>
#include <functional>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "onnx/common/assertions.h"
#include "onnx/onnx_pb.h"

namespace ONNX_NAMESPACE {

struct Tensor final {
 private:
  bool is_segment_{false};
  int64_t segment_begin_{0};
  int64_t segment_end_{0};
  bool has_name_{false};
  std::string name_;
  int32_t elem_type_{ONNX_NAMESPACE::TensorProto_DataType_UNDEFINED};
  std::vector<int64_t> sizes_;

  std::vector<float> float_data_;
  std::vector<double> double_data_;
  std::vector<int32_t> int32_data_;
  std::vector<int64_t> int64_data_;
  std::vector<uint64_t> uint64_data_;
  std::vector<std::string> string_data_;

  bool is_raw_data_{false};
  std::string raw_data_;

  std::vector<std::pair<std::string, std::string>> external_data_;
  ONNX_NAMESPACE::TensorProto_DataLocation data_location_{ONNX_NAMESPACE::TensorProto_DataLocation_DEFAULT};

 public:
  const std::vector<int64_t>& sizes() const {
    return sizes_;
  }
  std::vector<int64_t>& sizes() {
    return sizes_;
  }
  /// if tensor is a scalar, the sizes is empty, but the element number is actually 1.
  /// size_from_dim() cannot handle this case, while elem_num() handles it correctly
  int64_t elem_num() const {
    return std::accumulate(sizes_.begin(), sizes_.end(), (int64_t)1, std::multiplies<int64_t>{});
  }
  int64_t size_from_dim(int dim) const {
    if (dim < 0) {
      dim += (int)sizes_.size();
    }
    ONNX_ASSERT(dim >= 0 && (size_t)dim < sizes_.size())
    return std::accumulate(sizes_.begin() + dim, sizes_.end(), (int64_t)1, std::multiplies<int64_t>{});
  }

  int32_t elem_type() const {
    return elem_type_;
  }

  int32_t& elem_type() {
    return elem_type_;
  }

  std::vector<std::string>& strings() {
    return string_data_;
  }

  const std::vector<std::string>& strings() const {
    return string_data_;
  }

  std::vector<float>& floats() {
    return float_data_;
  }

  const std::vector<float>& floats() const {
    return float_data_;
  }

  std::vector<double>& doubles() {
    return double_data_;
  }

  const std::vector<double>& doubles() const {
    return double_data_;
  }

  std::vector<int32_t>& int32s() {
    return int32_data_;
  }

  const std::vector<int32_t>& int32s() const {
    return int32_data_;
  }

  std::vector<int64_t>& int64s() {
    return int64_data_;
  }

  const std::vector<int64_t>& int64s() const {
    return int64_data_;
  }

  std::vector<uint64_t>& uint64s() {
    return uint64_data_;
  }

  const std::vector<uint64_t>& uint64s() const {
    return uint64_data_;
  }

  const std::string& raw() const {
    return raw_data_;
  }

  void set_raw_data(std::string raw_data) {
    is_raw_data_ = true;
    raw_data_ = std::move(raw_data);
  }

  template <typename T>
  T* data();

  template <typename T>
  const T* data() const;

  bool is_segment() const {
    return is_segment_;
  }

  int64_t segment_begin() const {
    return segment_begin_;
  }

  int64_t segment_end() const {
    return segment_end_;
  }

  void set_segment_begin_and_end(int64_t begin, int64_t end) {
    is_segment_ = true;
    segment_begin_ = begin;
    segment_end_ = end;
  }

  bool hasName() const {
    return has_name_;
  }

  const std::string& name() const {
    return name_;
  }

  void setName(std::string name) {
    has_name_ = true;
    name_ = std::move(name);
  }

  bool is_raw_data() const {
    return is_raw_data_;
  }

  const std::vector<std::pair<std::string, std::string>>& external_data() const {
    return external_data_;
  }

  std::vector<std::pair<std::string, std::string>>& external_data() {
    return external_data_;
  }

  bool has_data_location() const {
    return data_location_ != ONNX_NAMESPACE::TensorProto_DataLocation_DEFAULT;
  }

  const ONNX_NAMESPACE::TensorProto_DataLocation& data_location() const {
    return data_location_;
  }

  ONNX_NAMESPACE::TensorProto_DataLocation& data_location() {
    return data_location_;
  }
};

template <>
inline std::string* Tensor::data<std::string>() {
  ONNX_ASSERTM(
      !is_raw_data(),
      "data type is string. string content is required to be stored in repeated bytes string_data field."
      "raw_data type cannot be string.")
  return string_data_.data();
}
template <>
inline const std::string* Tensor::data<std::string>() const {
  ONNX_ASSERTM(
      !is_raw_data(),
      "data type is string. string content is required to be stored in repeated bytes string_data field."
      "raw_data type cannot be string.")
  return string_data_.data();
}

#define define_data(type, field)                              \
  template <>                                                 \
  inline type* Tensor::data<type>() {                         \
    if (is_raw_data_) {                                       \
      return reinterpret_cast<type*>(raw_data_.data());       \
    } else {                                                  \
      return field.data();                                    \
    }                                                         \
  }                                                           \
                                                              \
  template <>                                                 \
  inline const type* Tensor::data<type>() const {             \
    if (is_raw_data_) {                                       \
      return reinterpret_cast<const type*>(raw_data_.data()); \
    } else {                                                  \
      return field.data();                                    \
    }                                                         \
  }

define_data(float, float_data_)
define_data(double, double_data_)
define_data(int32_t, int32_data_)
define_data(int64_t, int64_data_)
define_data(uint64_t, uint64_data_)
#undef define_data

} // namespace ONNX_NAMESPACE
