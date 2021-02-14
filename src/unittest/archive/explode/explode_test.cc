#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <doctest.h>

#include "formats/explode/mz/exe_file.hh"
#include "formats/explode/mz/unpklite.hh"
#include "formats/explode/mz/unlzexe.hh"
#include "formats/explode/mz/knowledge_dynamics.hh"

#include "unittest/md5.h"

#include "pklite_112.cc"
#include "pklite_120.cc"
#include "pklite_201.cc"
#include "pklite_E_115.cc"
#include "pklite_100.cc"
#include "pklite_115.cc"
#include "pklite_150.cc"
#include "pklite_E_112.cc"
#include "pklite_E_120.cc"

#include "z90.cc"
#include "z91.cc"
#include "z91e.cc"

#include "dot.cc"
#include "lex.cc"
#include "tntega.cc"



static const char* digest_pklite_112   = "e1f98f301ef8bb8710ae14469bcb2cd0";
static const char* digest_pklite_115   = "13482d37794b1106a85712b5e7a1227a";
static const char* digest_pklite_120   = "e1f98f301ef8bb8710ae14469bcb2cd0";
static const char* digest_pklite_150   = "36ce063f2a979acc3ba887f4f3b9f735";
static const char* digest_pklite_201   = "e6cf27d7818c320ce64bcb4caba7f5a4";
static const char* digest_pklite_E_112 = "8a4b841106bae1f32c7ca45e9d41c016";
static const char* digest_pklite_E_115 = "56dccb4b55bdd7c57f09dbb584050a51";
static const char* digest_pklite_E_120 = "8a4b841106bae1f32c7ca45e9d41c016";

static const char* digest_lzexe_91   = "f38e4c688fcd8f3d4f102dc5e2b8bb0f"; 
static const char* digest_lzexe_91_E = "f38e4c688fcd8f3d4f102dc5e2b8bb0f";
static const char* digest_lzexe_90   = "620d7dce66a13ec7be84b9f390078aa6";


static const char* digest_knowledge_dynamics_LEX = "03703e056977944b007eb2ecccf3f1c4";
static const char* digest_knowledge_dynamics_DOT = "3b1429a7224c868b4725228b1a4ffb66";
static const char* digest_knowledge_dynamics_TNT = "d813b5ac3095c24c3eba559bac22a32d";

typedef unsigned char md5_digest[MD5_DIGEST_LENGTH];

template <class DECODER>
struct tester;

template <>
struct tester < formats::explode::mz::unpklite >
{
	static void test(formats::explode::mz::input_exe_file& iexe)
	{
		if (!formats::explode::mz::unpklite::accept (iexe))
		{
			throw std::runtime_error("not a PKLITE");
		}
	}
};

template <>
struct tester < formats::explode::mz::unlzexe >
{
	static void test(formats::explode::mz::input_exe_file& iexe)
	{
		if (!formats::explode::mz::unlzexe::accept (iexe))
		{
			throw std::runtime_error("not a LZEXE");
		}
	}
};

template <>
struct tester < formats::explode::mz::knowledge_dynamics >
{
	static void test(formats::explode::mz::input_exe_file& iexe)
	{
		if (!formats::explode::mz::knowledge_dynamics::accept(iexe))
		{
			throw std::runtime_error("not a Knowledge Dynaimcs");
		}
	}
};


template <typename DECODER>
static void eval_digest(const unsigned char* data, std::size_t length, md5_digest& digest, std::vector<char>& out_buff)
{
	formats::io::inmem_input input((unsigned char*)data, length);
	formats::explode::mz::input_exe_file iexe(input);

	tester <DECODER>::test(iexe);

	DECODER decoder(iexe);
	formats::explode::mz::full_exe_file fo(decoder.decomp_size());
	decoder.unpack(fo);

	formats::io::inmem_output out(out_buff);
	fo.write(out);


	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, &out_buff[0], static_cast <unsigned long> (out_buff.size ()));
	MD5_Final(digest, &c);
}
template <typename DECODER>
static bool do_test(const unsigned char* data, std::size_t length, const char* expected)
{
	md5_digest dgst = {0};
	bool ok = true;

		std::vector <char> out_buff;
		eval_digest <DECODER>(data, length, dgst, out_buff);

		for (int n = 0; n < MD5_DIGEST_LENGTH; n++)
		{
			std::string h(expected + 2 * n, expected + 2 * (n + 1));
			std::istringstream is(h);
			int x;
			is >> std::hex >> x;
			if (x != dgst[n])
			{
				ok = false;
				break;
			}
		}
	return ok;
}

#define CONCATENATE_DIRECT(s1,s2) s1##s2
#define CONCATENATE(s1,s2) CONCATENATE_DIRECT(s1,s2)

#define STRINGIZE_HELPER(exp) #exp
#define STRINGIZE(exp) STRINGIZE_HELPER(exp)

#define PKLITE_TEST(NAME) do_test <formats::explode::mz::unpklite>(CONCATENATE (data::pklite_, NAME), CONCATENATE(CONCATENATE (data::pklite_, NAME), _len), CONCATENATE(digest_, CONCATENATE (pklite_, NAME)))
#define LZEXE_TEST(NAME) do_test <formats::explode::mz::unlzexe>(CONCATENATE (data::z, NAME), CONCATENATE(CONCATENATE (data::z, NAME), _len), CONCATENATE(digest_, CONCATENATE (lzexe_, NAME)))
#define KD_TEST(NAME) do_test <formats::explode::mz::knowledge_dynamics>(CONCATENATE (data::knowledge_dynamics_, NAME), CONCATENATE(CONCATENATE (data::knowledge_dynamics_, NAME), _len), CONCATENATE(digest_, CONCATENATE (knowledge_dynamics_, NAME)))

TEST_CASE("mz-explode PKLITE tests") {
	REQUIRE(PKLITE_TEST(112));
	REQUIRE(PKLITE_TEST(E_112));

	REQUIRE(PKLITE_TEST(115));
	REQUIRE(PKLITE_TEST(E_115));

	REQUIRE(PKLITE_TEST(150));
	//REQUIRE(PKLITE_TEST(201));
}

TEST_CASE("mz-explode LZEXE tests") {
	REQUIRE(LZEXE_TEST(90));
	REQUIRE(LZEXE_TEST(91));
	REQUIRE(LZEXE_TEST(91_E));
}

TEST_CASE("mz-explode KnowledgeDynamics tests") {
	REQUIRE(KD_TEST(DOT));
	REQUIRE(KD_TEST(TNT));
	REQUIRE(KD_TEST(LEX));
}

