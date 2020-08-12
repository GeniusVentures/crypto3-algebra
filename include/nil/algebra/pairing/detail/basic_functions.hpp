//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef ALGEBRA_PAIRING_BASIC_FUNCTIONS_HPP
#define ALGEBRA_PAIRING_BASIC_FUNCTIONS_HPP

#include <stdexcept>
#include <vector>

namespace nil{
    namespace algebra{
        namespace pairing{
            namespace detail{

                /*
                    square over Fp4
                    Operation Count:

                    3 * Fp2Dbl::square
                    2 * Fp2Dbl::mod
                    1 * Fp2Dbl::mul_xi == 1 * (2 * Fp2::add/sub) == 2 * Fp2::add/sub
                    3 * Fp2Dbl::add/sub == 3 * (2 * Fp2::add/sub) == 6 * Fp2::add/sub
                    1 * Fp2::add/sub

                    Total:

                    3 * Fp2Dbl::square
                    2 * Fp2Dbl::mod
                    9 * Fp2::add/sub
                 */
                element <fp4> sq_Fp4UseDbl(const element<fp4> &B) {
                    double_element<fp2> T0, T1, T2;
                    element<fp2> z0, z1;
                    T0 = x0.square();
                    T1 = x1.square();
                    T2 = T1.mul_xi();
                    T2 += T0;
                    z1 = x0 + x1;
                    z0 = T2.mod();
                    // overwrite z[0] (position 0).
                    T2 = z1.square();
                    T2 -= T0;
                    T2 -= T1;
                    z1 = T2.mod();
                    return {z0, z1};
                }




                /*
                    Final exponentiation based on:
                    - Laura Fuentes-Casta{\~n}eda, Edward Knapp, and Francisco
                    Rodr\'{\i}guez-Henr\'{\i}quez.
                    Faster hashing to $\mathbb{G}_2$.
                    SAC 2011, pp. 412--430. doi:10.1007/978-3-642-28496-0_25.

                    *this = final_exp(*this)
                */

                element<fp12_2over3over2> pow_neg_t(const element<fp12_2over3over2> &A) {
                    element <fp12_2over3over2> out = A;
                    element <fp12_2over3over2> inConj;
                    inConj.a_ = A.a_;
                    inConj.b_ = -A.b_;    // in^-1 == in^(p^6)

                    for (size_t i = 1; i < Param::zReplTbl.size(); i++) {
                        out.sqru();
                        if (Param::zReplTbl[i] > 0) {
                            out = out * in;
                        } else if (Param::zReplTbl[i] < 0) {
                            out = out * inConj;
                        }
                    }
                    // invert by conjugation
                    out.b_ = -out.b_;
                }


                /*
                    @note destory *this
                */
                element<fp12_2over3over2> mapToCyclo(element<fp12_2over3over2> A){
                    // (a + b*i) -> ((a - b*i) * (a + b*i)^(-1))^(q^2+1)
                    //
                    // See Beuchat page 9: raising to 6-th power is the same as
                    // conjugation, so this entire function computes
                    // z^((p^6-1) * (p^2+1))
                    z.a_ = A.a_;
                    z.b_ - A.b_;
                    data = A.data.inverse();
                    z *= A;
                    z.Frobenius2(A);
                    z *= A;
                }

                element<fp12_2over3over2> final_exp(element<fp12_2over3over2> A) {
                    element <fp12_2over3over2> f, f2z, f6z, f6z2, f12z3;
                    element <fp12_2over3over2> a, b;
                    element <fp12_2over3over2> &z = A;
                    f = mapToCyclo(f);

                    f2z = pow_neg_t(f);
                    f2z = f2z.sqru();    // f2z = f^(-2*z)
                    f6z = f2z.sqru();
                    f6z *= f2z;    // f6z = f^(-6*z)
                    f6z2 = pow_neg_t(f6z);
                    // A variable a is unnecessary only here.
                    a = f6z2.sqru();
                    // Compress::fixed_power(f12z3, a); // f12z3 = f^(-12*z^3)
                    f12z3 = pow_neg_t(a);
                    // It will compute inversion of f2z, thus, conjugation free.
                    f6z.b_ = -f6z.b_;        // f6z = f^(6z)
                    f12z3.b_ = -f12z3.b_;    // f12z3 = f^(12*z^3)
                    // Computes a and b.
                    a = f12z3 * f6z2;    // a = f^(12*z^3 + 6z^2)
                    a *= f6z;                      // a = f^(12*z^3 + 6z^2 + 6z)
                    b = a * f2z;         // b = f^(12*z^3 + 6z^2 + 4z)w
                    // @note f2z, f6z, and f12z are unnecessary from here.
                    // Last part.
                    z = a * f6z2;    // z = f^(12*z^3 + 12z^2 + 6z)
                    z *= f;                    // z = f^(12*z^3 + 12z^2 + 6z + 1)
                    f2z = b.Frobenius();          // f2z = f^(q(12*z^3 + 6z^2 + 4z))
                    z *= f2z;                  // z = f^(q(12*z^3 + 6z^2 + 4z) + (12*z^3 + 12z^2 + 6z + 1))
                    f2z = a.Frobenius2();         // f2z = f^(q^2(12*z^3 + 6z^2 + 6z))
                    z *= f2z;    // z = f^(q^2(12*z^3 + 6z^2 + 6z) + q(12*z^3 + 6z^2 + 4z) + (12*z^3 + 12z^2 + 6z + 1))
                    f.b_ = -f.b_;    // f = -f
                    b *= f;                  // b = f^(12*z^3 + 6z^2 + 4z - 1)
                    f2z = b.Frobenius3();       // f2z = f^(q^3(12*z^3 + 6z^2 + 4z - 1))
                    z *= f2z;
                    // z = f^(q^3(12*z^3 + 6z^2 + 4z - 1) +
                    // q^2(12*z^3 + 6z^2 + 6z) +
                    // q(12*z^3 + 6z^2 + 4z) +
                    // (12*z^3 + 12z^2 + 6z + 1))
                    // see page 6 in the "Faster hashing to G2" paper

                }

            }       // namespace detail
        }       // namespace pairing
    }       // namespace algebra
}    // namespace nil

#endif    // ALGEBRA_PAIRING_BASIC_FUNCTIONS_HPP