/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_MLIR_TENSORFLOW_UTILS_XLA_SHARDING_UTIL_H_
#define TENSORFLOW_COMPILER_MLIR_TENSORFLOW_UTILS_XLA_SHARDING_UTIL_H_

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "mlir/IR/Builders.h"  // from @llvm-project
#include "mlir/IR/Operation.h"  // from @llvm-project
#include "mlir/IR/Types.h"  // from @llvm-project
#include "mlir/IR/Value.h"  // from @llvm-project
#include "mlir/Support/LogicalResult.h"  // from @llvm-project
#include "tensorflow/compiler/mlir/tensorflow/ir/tf_device.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"

namespace tensorflow {

extern const char* const kXlaShardingAttrName;
extern const char* const kInputShardingAttr;
extern const char* const kOutputShardingAttr;

// Parses "_XlaSharding" attribute from operation, if it exists.
llvm::Optional<mlir::StringRef> ParseShardingAttribute(
    mlir::Operation* operation);

// Parses "input_sharding_configuration" attribute and returns a list where
// i-th element is a list of mlir::Value's which represent inputs for the
// TPU computation correponding to i-th logical device. If the attribute
// does not exist, the all inputs are placed on logical core 0.
mlir::LogicalResult ExtractInputsForLogicalDevices(
    const int num_cores_per_replica, mlir::tf_device::LaunchFuncOp launch_func,
    mlir::OpBuilder* builder,
    llvm::SmallVectorImpl<llvm::SmallVector<mlir::Value, 4>>* input_list);

// Extracts a list of OpSharding that represent output sharding configuration
// of `tf_device.launch`.
mlir::LogicalResult ParseAndValidateOutputSharding(
    const int num_cores_per_replica, mlir::tf_device::LaunchFuncOp launch_func,
    mlir::SmallVector<xla::OpSharding, 4>* output_sharding_list);

// Retrieves output types for TPUExecute op representing execution for provided
// logical device id. TPUExecute op for different logical device may have
// different outputs depending on the output sharding configuration.
mlir::LogicalResult GetOutputTypesForLogicalDeviceComputation(
    const int core_id, llvm::ArrayRef<xla::OpSharding> output_sharding_config,
    mlir::tf_device::LaunchFuncOp launch_func,
    llvm::SmallVectorImpl<mlir::Type>* output_types);

// Remaps outputs of `tf_device.parallel_execute` op that represent concurrent
// execution of the `tf_device.launch_func` with its users.
void RemapOutputsFromLogicalDevices(
    const mlir::Location& location,
    llvm::ArrayRef<xla::OpSharding> output_sharding_config,
    mlir::tf_device::LaunchFuncOp launch_func,
    mlir::tf_device::ParallelExecuteOp parallel_execute,
    mlir::OpBuilder* builder);

}  // namespace tensorflow

#endif  // TENSORFLOW_COMPILER_MLIR_TENSORFLOW_UTILS_XLA_SHARDING_UTIL_H_
