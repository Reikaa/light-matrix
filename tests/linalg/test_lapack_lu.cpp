/**
 * @file test_lapack_lu.cpp
 *
 * @brief Unit testing of LU factorization
 *
 * @author Dahua Lin
 */

#include "linalg_test_base.h"
#include <light_mat/linalg/blas_l3.h>
#include <light_mat/linalg/lapack_lu.h>

using namespace lmat;
using namespace lmat::test;

using lmat::lapack::lu_fac;

template<typename T>
void test_lu_solve( char trans )
{
	typedef mat_host<bloc, T, 0, 0> host_t;
	typedef typename host_t::cmat_t cmat_t;
	typedef typename host_t::mat_t mat_t;

	index_t m = DM;
	index_t n = DN;

	host_t a_host(m, m);
	host_t b_host(m, n);
	host_t x_host(m, n);

	mat_t amat = a_host.get_mat();
	fill_prand(amat, T(2));
	b_host.fill_rand();

	cmat_t a = a_host.get_cmat();
	cmat_t b = b_host.get_cmat();
	mat_t x = x_host.get_mat();

	lu_fac<T> lu;
	lu.set(a);

	lu.solve(b, x, trans);

	dense_matrix<T> r(m, n);
	blas::gemm(a, x, r, trans, 'n');

	T tol = (T)(sizeof(T) == 4 ? 2.0e-5 : 1.0e-10);

	ASSERT_MAT_APPROX(m, n, b, r, tol);
}

template<typename T>
void test_lu_inv()
{
	typedef mat_host<bloc, T, 0, 0> host_t;
	typedef typename host_t::cmat_t cmat_t;
	typedef typename host_t::mat_t mat_t;

	index_t m = DM;

	host_t a_host(m, m);
	mat_t amat = a_host.get_mat();
	fill_prand(amat, T(2));

	cmat_t a = a_host.get_cmat();

	dense_matrix<T> b = lapack::inv(a);

	ASSERT_EQ( b.nrows(), m );
	ASSERT_EQ( b.ncolumns(), m );

	dense_matrix<T> r(m, m);
	dense_matrix<T> e(m, m);

	blas::gemm(a, b, r);
	fill_eye(e);

	T tol = (T)(sizeof(T) == 4 ? 2.0e-5 : 1.0e-10);
	ASSERT_MAT_APPROX(m, m, r, e, tol);
}


template<typename T>
void test_gesv()
{
	typedef mat_host<bloc, T, 0, 0> host_t;
	typedef typename host_t::cmat_t cmat_t;
	typedef typename host_t::mat_t mat_t;

	index_t m = DM;
	index_t n = DN;

	host_t a_host(m, m);
	host_t b_host(m, n);

	mat_t amat = a_host.get_mat();
	fill_prand(amat, T(2));
	b_host.fill_rand();

	cmat_t a = a_host.get_cmat();
	cmat_t b = b_host.get_cmat();

	dense_matrix<T> x = lapack::solve(a, x);

	dense_matrix<T> r(m, n);
	blas::gemm(a, x, r);

	T tol = (T)(sizeof(T) == 4 ? 2.0e-5 : 1.0e-10);

	ASSERT_MAT_APPROX(m, n, b, r, tol);
}



T_CASE( mat_lu, solve_n )
{
	test_lu_solve<T>('n');
}

T_CASE( mat_lu, solve_t )
{
	test_lu_solve<T>('t');
}

T_CASE( mat_lu, inv )
{
	test_lu_inv<T>();
}

T_CASE( mat_lu, gesv )
{
	test_lu_inv<T>();
}


BEGIN_TPACK( mat_lu_solve )
	ADD_T_CASE( mat_lu, solve_n, float )
	ADD_T_CASE( mat_lu, solve_n, double )
	ADD_T_CASE( mat_lu, solve_t, float )
	ADD_T_CASE( mat_lu, solve_t, double )
END_TPACK

BEGIN_TPACK( mat_lu_inv )
	ADD_T_CASE( mat_lu, inv, float )
	ADD_T_CASE( mat_lu, inv, double )
END_TPACK

BEGIN_TPACK( mat_gesv )
	ADD_T_CASE( mat_lu, gesv, float )
	ADD_T_CASE( mat_lu, gesv, double )
END_TPACK

BEGIN_MAIN_SUITE
	ADD_TPACK( mat_lu_solve )
	ADD_TPACK( mat_lu_inv )
	ADD_TPACK( mat_gesv )
END_MAIN_SUITE

