/*******************************************************************************
 * Copyright 2021-2024 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/
#ifndef BACKEND_ELYZOR_COMPILER_PARTITION_IMPL_HPP
#define BACKEND_ELYZOR_COMPILER_PARTITION_IMPL_HPP

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include "graph/interface/backend.hpp"
#include "graph/interface/partition.hpp"

#include "elyzor_backend.hpp"
#include "include/dnnl_graph_compiler.h"
#include "utils.hpp"

namespace dnnl {
namespace impl {
namespace graph {
namespace elyzor {

class elyzor_partition_impl_t : public partition_impl_t {
    friend class elyzor_backend_t;

public:
    elyzor_partition_impl_t(graph::engine_kind_t engine_kind,
            graph::fpmath_mode_t fpmath_mode, graph::partition_kind_t pkind,
            std::string pname)
        : graph::partition_impl_t(engine_kind, fpmath_mode, pkind)
        , is_init_(true)
        , pname_(pname) {
        assertm(fpmath_mode == fpmath_mode::strict
                        || fpmath_mode == fpmath_mode::bf16,
                "Compiler backend only allows fpmath mode: strict, bf16.");
    }

    virtual ~elyzor_partition_impl_t() = default;

    bool is_initialized() const override;
    std::shared_ptr<graph::partition_impl_t> clone() const override;
    graph::status_t infer_shape(
            std::vector<const graph::logical_tensor_t *> &inputs,
            std::vector<graph::logical_tensor_t *> &outputs) const override;
    graph::status_t compile(graph::compiled_partition_t *compiled_partition,
            const std::vector<graph::logical_tensor_t> &inputs,
            const std::vector<graph::logical_tensor_t> &outputs,
            const graph::engine_t *aengine) const override;

    const graph::backend_t *get_assigned_backend() const override {
        return &elyzor_backend_t::get_singleton();
    }

    // add op to backend partition
    void add_op(const std::shared_ptr<op_t> &op) { ops_.emplace_back(op); }

    // add ops to backend partition
    void add_op(const std::vector<std::shared_ptr<op_t>> &ops) {
        for (auto &op : ops) {
            add_op(op);
        }
    }

    // add backend partition's input tensor
    void add_input_tensor(const std::shared_ptr<value_t> &v) {
        auto in_pos = std::find_if(inputs_.begin(), inputs_.end(),
                [&](const graph::logical_tensor_t &alt) -> bool {
                    return alt.id == v->get_logical_tensor().id;
                });
        if (in_pos == inputs_.end()) {
            inputs_.push_back(v->get_logical_tensor());
        }
    }

    // add backend partition's output tensor
    void add_output_tensor(const std::shared_ptr<value_t> &v) {
        auto out_pos = std::find_if(outputs_.begin(), outputs_.end(),
                [&](const graph::logical_tensor_t &alt) -> bool {
                    return alt.id == v->get_logical_tensor().id;
                });
        if (out_pos == outputs_.end()) {
            outputs_.push_back(v->get_logical_tensor());
        }
    }

    bool is_op_exist(const op_t *aop) {
        auto pos = std::find_if(ops_.begin(), ops_.end(),
                [&](const std::shared_ptr<graph::op_t> &cur) -> bool {
                    return cur->get_id() == aop->get_id();
                });
        return pos != ops_.end();
    }

    std::string get_name() const { return pname_; }

#ifndef NDEBUG
    // get protected 'copied_ops_' field, should only be used in testing
    std::vector<std::shared_ptr<graph::op_t>> get_copied_ops() {
        return copied_ops_;
    }
#endif

protected:
    bool is_init_ = false;
    mutable std::vector<std::shared_ptr<graph::op_t>> copied_ops_;
    mutable std::mutex mtx_;
    std::string pname_;
};

class elyzor_compiled_partition_impl_t : public compiled_partition_impl_t {
public:
    elyzor_compiled_partition_impl_t(const graph::engine_t &engine,
            const std::vector<graph::logical_tensor_t> &inputs,
            const std::vector<graph::logical_tensor_t> &outputs,
            const std::vector<graph::inplace_pair_t> &inplace_pairs,
            const std::shared_ptr<const dnnl_graph_compiler_executable> exe,
            const std::shared_ptr<const dnnl_graph_compiler> gc);
    virtual ~elyzor_compiled_partition_impl_t();
    graph::status_t execute(const graph::stream_t *astream,
            const std::vector<graph::tensor_t> &inputs,
            const std::vector<graph::tensor_t> &outputs) override;

#ifdef DNNL_WITH_SYCL
    status_t execute_sycl(const stream_t *astream,
            const std::vector<tensor_t> &inputs,
            const std::vector<tensor_t> &outputs,
            const std::vector<::sycl::event> &sycl_deps,
            ::sycl::event *sycl_event) override {
        return execute(astream, inputs, outputs);
    }
#endif

private:
    const std::shared_ptr<const dnnl_graph_compiler_executable> exe_;
    const std::shared_ptr<const dnnl_graph_compiler> gc_;
};

} // namespace elyzor
} // namespace graph
} // namespace impl
} // namespace dnnl
#endif
