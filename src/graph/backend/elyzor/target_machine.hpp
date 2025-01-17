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
#ifndef BACKEND_ELYZOR_TARGET_MACHINE_HPP
#define BACKEND_ELYZOR_TARGET_MACHINE_HPP

#include "cpu/x64/cpu_isa_traits.hpp"

#define REQUIRE_AVX512_BEGIN \
    if (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512F)) {
#define REQUIRE_VNNI_AMXINT8_BEGIN \
    if (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512_VNNI) \
            || (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512F) \
                    && dnnl::impl::cpu::x64::cpu().has( \
                            Xbyak::util::Cpu::tAMX_INT8))) {
#define REQUIRE_BF16_AMXBF16_BEGIN \
    if (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512_BF16) \
            || (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512F) \
                    && dnnl::impl::cpu::x64::cpu().has( \
                            Xbyak::util::Cpu::tAMX_BF16))) {
#define REQUIRE_AMX_BEGIN \
    if (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512F) \
            && dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAMX_TILE)) {
#define REQUIRE_AMXBF16_BEGIN \
    if (dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAVX512F) \
            && dnnl::impl::cpu::x64::cpu().has(Xbyak::util::Cpu::tAMX_BF16)) {
#define REQUIRE_AVX512_END }
#define REQUIRE_VNNI_AMXINT8_END }
#define REQUIRE_BF16_AMXBF16_END }
#define REQUIRE_AMX_END }
#define REQUIRE_AMXBF16_END }

inline bool support_vnni() {
    REQUIRE_VNNI_AMXINT8_BEGIN
    return true;
    REQUIRE_VNNI_AMXINT8_END
    return false;
}

inline bool support_bf16() {
    REQUIRE_BF16_AMXBF16_BEGIN
    return true;
    REQUIRE_BF16_AMXBF16_END
    return false;
}
#endif
