//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ALGEBRA_PAIRING_ALT_BN128_POLICY_HPP
#define CRYPTO3_ALGEBRA_PAIRING_ALT_BN128_POLICY_HPP

namespace nil {
    namespace crypto3 {
        namespace algebra {
            namespace curves {

                template<std::size_t ModulusBits>
                struct alt_bn128;

            }    // namespace curves
            namespace pairings {

                template<typename PairingCurveType, typename PairingFunctions>
                struct pairing_policy;

                template<std::size_t ModulusBits, typename PairingFunctions>
                class pairing_policy<curves::alt_bn128<ModulusBits>, PairingFunctions> {

                    using policy_type = PairingFunctions;

                public:
                    typedef typename policy_type::number_type number_type;

                    constexpr static const typename policy_type::number_type pairing_loop_count =
                        policy_type::ate_loop_count;

                    using Fp_type = typename policy_type::Fp_type;
                    using Fq_type = typename policy_type::Fq_type ;
                    using Fqe_type = typename policy_type::Fqe_type;
                    using Fqk_type = typename policy_type::Fqk_type;

                    using g1_type = typename policy_type::g1_type;
                    using g2_type = typename policy_type::g2_type;
                    using gt_type = typename policy_type::gt_type;

                    using G1_precomp = typename policy_type::g1_precomp;
                    using G2_precomp = typename policy_type::g2_precomp;

                    /*constexpr static*/ const typename g2_type::underlying_field_type::value_type twist =
                        policy_type().twist;

                    using G1_precomp = typename policy_type::g1_precomp;
                    using G2_precomp = typename policy_type::g2_precomp;

                    static inline G1_precomp precompute_g1(const typename g1_type::value_type &P) {
                        return policy_type::precompute_g1(P);
                    }

                    static inline G2_precomp precompute_g2(const typename g2_type::value_type &Q) {
                        return policy_type::precompute_g2(Q);
                    }

                    static inline typename gt_type::value_type pairing(const typename g1_type::value_type &P,
                                                                   const typename g2_type::value_type &Q) {
                        return policy_type::pairing(P, Q);
                    }

                    static inline typename gt_type::value_type reduced_pairing(const typename g1_type::value_type &P,
                                                                           const typename g2_type::value_type &Q) {
                        return policy_type::reduced_pairing(P, Q);
                    }

                    static inline typename policy_type::gt
                        double_miller_loop(const G1_precomp &prec_P1,
                                           const G2_precomp &prec_Q1,
                                           const G1_precomp &prec_P2,
                                           const G2_precomp &prec_Q2) {
                        return policy_type::double_miller_loop(prec_P1, prec_Q1, prec_P2, prec_Q2);
                    }

                    static inline typename gt_type::value_type final_exponentiation(const typename gt_type::value_type &elt) {
                        return policy_type::final_exponentiation(elt);
                    }

                    static inline typename gt_type::value_type miller_loop(const G1_precomp &prec_P,
                                                                       const G2_precomp &prec_Q) {
                        return policy_type::miller_loop(prec_P, prec_Q);
                    }
                };

                template<std::size_t ModulusBits, typename PairingFunctions>
                constexpr typename pairing_policy<curves::alt_bn128<ModulusBits>, PairingFunctions>::number_type const
                    pairing_policy<curves::alt_bn128<ModulusBits>, PairingFunctions>::pairing_loop_count;
            }    // namespace pairings
        }        // namespace algebra
    }            // namespace crypto3
}    // namespace nil
#endif    // CRYPTO3_ALGEBRA_PAIRING_ALT_BN128_POLICY_HPP