#include <catch2/catch_test_macros.hpp>
#include <compiler/ast_format.h>
#include <compiler/if_expr.h>
#include <compiler/parsers.h>

namespace xi
{

// NOLINTBEGIN(cppcoreguidelines-*, readability*)
TEST_CASE("Parse digit", "[string]")
{
    auto [result1, result2] = s_digit("123").value();
    REQUIRE(result1 == '1');
    REQUIRE(result2 == "23");

    auto result3 = s_digit("abc");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse space", "[string]")
{
    auto [result1, result2] = s_space(" 123").value();
    REQUIRE(result1 == ' ');
    REQUIRE(result2 == "123");

    auto result3 = s_space("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse letter", "[string]")
{
    auto [result1, result2] = s_alpha("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto result3 = s_alpha("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse alphanum", "[string]")
{
    auto [result1, result2] = s_alphanum("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto [result3, result4] = s_alphanum("123").value();
    REQUIRE(result3 == '1');
    REQUIRE(result4 == "23");
}

TEST_CASE("Parse symbol", "[string]")
{
    auto [result1, result2] = symbol('a')("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");
}

TEST_CASE("Parse token", "[string]")
{
    auto parser             = token(str("foo"));
    auto [result1, result2] = parser("  foo bar").value();
    REQUIRE(result1 == "foo");
    REQUIRE(result2 == " bar");
}

TEST_CASE("Parse String", "[Xi_String]")
{
    auto [result1, result2] = Xi_string("\"abcccb\"").value();
    REQUIRE(result1 == Xi_String{"abcccb"});
    REQUIRE(result2 == "");
}

TEST_CASE("Parse natural", "[string]")
{
    auto [result1, result2] = s_natural("123abc").value();
    REQUIRE(result1 == "123");
    REQUIRE(result2 == "abc");

    auto [result3, result4] = s_natural("123 123").value();
    REQUIRE(result3 == "123");
    REQUIRE(result4 == " 123");
}

TEST_CASE("Parse integer", "[Xi_Integer]")
{
    auto [result1, result2] = Xi_integer("123abc").value();
    REQUIRE(result1 == Xi_Integer{123});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_integer("-123abc").value();
    REQUIRE(result3 == Xi_Integer{-123});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Parse boolean", "[Xi_Boolean]")
{
    auto [result1, result2] = Xi_boolean("trueabc").value();
    REQUIRE(result1 == Xi_Boolean{true});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_boolean("falseabc").value();
    REQUIRE(result3 == Xi_Boolean{false});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Parse real", "[Xi_Real]")
{
    auto [result1, result2] = Xi_real("123.456abc").value();
    REQUIRE(result1 == Xi_Real{123.456});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_real("-123.456abc").value();
    REQUIRE(result3 == Xi_Real{-123.456});
    REQUIRE(result4 == "abc");

    auto result5 = Xi_real("123abc");
    REQUIRE(result5 == std::nullopt);
}

TEST_CASE("Parse expr", "[Xi_Expr]")
{
    auto [integer1, integer2] = Xi_expr("123abc").value();
    REQUIRE(integer1 == Xi_Integer{123});
    REQUIRE(integer2 == "abc");

    auto [real1, real2] = Xi_expr("123.456abc").value();
    REQUIRE(real1 == Xi_Real{123.456});
    REQUIRE(real2 == "abc");

    auto [boolean1, boolean2] = Xi_expr("trueabc").value();
    REQUIRE(boolean1 == Xi_Boolean{true});
    REQUIRE(boolean2 == "abc");

    auto [string1, string2] = Xi_expr("\"abc\"abc").value();
    REQUIRE(string1 == Xi_String{"abc"});
}

TEST_CASE("Parse Xi_Op", "[Xi_Op]")
{
    auto [op5, op6] = Xi_mul(" *abc").value();
    REQUIRE(op5 == Xi_Op::Mul);
    REQUIRE(op6 == "abc");
}

TEST_CASE("Parse mathexpr", "[Xi_Expr]")
{
    auto [integer1, integer2] = Xi_mathexpr("1 + 2").value();
    REQUIRE(
        integer1 == Xi_Binop{
                        Xi_Integer{1},
                        Xi_Integer{2},
                        Xi_Op::Add,
                    });

    // test multiply
    auto [integer3, integer4] = Xi_mathexpr(" 1*2").value();
    REQUIRE(
        integer3 == Xi_Binop{
                        Xi_Integer{1},
                        Xi_Integer{2},
                        Xi_Op::Mul,
                    });

    // test add and multiply
    auto [integer5, integer6] = Xi_mathexpr(" 1 + 2 * 3").value();
    REQUIRE(
        integer5 == Xi_Binop{
                        Xi_Integer{1},
                        Xi_Binop{
                            Xi_Integer{2},
                            Xi_Integer{3},
                            Xi_Op::Mul,
                        },
                        Xi_Op::Add,
                    });

    auto [integer7, integer8] = Xi_mathexpr(" 1 + 2 + 3").value();
    REQUIRE(
        integer7 == Xi_Binop{
                        Xi_Integer{1},
                        Xi_Binop{
                            Xi_Integer{2},
                            Xi_Integer{3},
                            Xi_Op::Add,
                        },
                        Xi_Op::Add,
                    });

    // test paren
    auto [integer9, integer10] = Xi_mathexpr(" (1 + 2) * 3").value();
    REQUIRE(
        integer9 == Xi_Binop{
                        Xi_Binop{
                            Xi_Integer{1},
                            Xi_Integer{2},
                            Xi_Op::Add,
                        },
                        Xi_Integer{3},
                        Xi_Op::Mul,
                    });

    auto [integer11, integer12] = Xi_mathexpr(" (1 * 2) * 3 + 4").value();
    REQUIRE(
        integer11 == Xi_Binop{
                         Xi_Binop{
                             Xi_Binop{
                                 Xi_Integer{1},
                                 Xi_Integer{2},
                                 Xi_Op::Mul,
                             },
                             Xi_Integer{3},
                             Xi_Op::Mul},
                         Xi_Integer{4},
                         Xi_Op::Add,
                     });
}

TEST_CASE("Parse mathexpr with identify", "[Xi_Expr]")
{
    auto [integer1, integer2] = Xi_mathexpr("a + 2").value();
    REQUIRE(
        integer1 == Xi_Binop{
                        Xi_Iden{"a"},
                        Xi_Integer{2},
                        Xi_Op::Add,
                    });

    auto [integer3, integer4] = Xi_mathexpr("a + b").value();
    REQUIRE(
        integer3 == Xi_Binop{
                        Xi_Iden{"a"},
                        Xi_Iden{"b"},
                        Xi_Op::Add,
                    });

    auto [integer5, integer6] = Xi_mathexpr("(a_1 + b) * 3").value();
    REQUIRE(
        integer5 == Xi_Binop{
                        Xi_Binop{
                            Xi_Iden{"a_1"},
                            Xi_Iden{"b"},
                            Xi_Op::Add,
                        },
                        Xi_Integer{3},
                        Xi_Op::Mul,
                    });
}

TEST_CASE("Parse boolexpr", "[Xi_Expr]")
{
    auto [boolean1, boolean2] = Xi_boolexpr("true && false").value();
    REQUIRE(
        boolean1 == Xi_Binop{
                        Xi_Boolean{true},
                        Xi_Boolean{false},
                        Xi_Op::And,
                    });

    auto [boolean3, boolean4] = Xi_boolexpr("true || false").value();
    REQUIRE(
        boolean3 == Xi_Binop{
                        Xi_Boolean{true},
                        Xi_Boolean{false},
                        Xi_Op::Or,
                    });

    auto [boolean5, boolean6] = Xi_boolexpr("true && false || true").value();
    REQUIRE(
        boolean5 == Xi_Binop{
                        Xi_Binop{
                            Xi_Boolean{true},
                            Xi_Boolean{false},
                            Xi_Op::And,
                        },
                        Xi_Boolean{true},
                        Xi_Op::Or,
                    });

    auto [boolean7, boolean8] = Xi_boolexpr("true || false && true").value();
    REQUIRE(
        boolean7 == Xi_Binop{
                        Xi_Boolean{true},
                        Xi_Binop{
                            Xi_Boolean{false},
                            Xi_Boolean{true},
                            Xi_Op::And,
                        },
                        Xi_Op::Or,
                    });

    auto [boolean9, boolean10] =
        Xi_boolexpr("true && false || true && false").value();
    REQUIRE(
        boolean9 == Xi_Binop{
                        Xi_Binop{
                            Xi_Boolean{true},
                            Xi_Boolean{false},
                            Xi_Op::And,
                        },
                        Xi_Binop{
                            Xi_Boolean{true},
                            Xi_Boolean{false},
                            Xi_Op::And,
                        },
                        Xi_Op::Or,
                    });

    // test paren
    auto [boolean11, boolean12] =
        Xi_boolexpr("(true || false) && true").value();
    REQUIRE(
        boolean11 == Xi_Binop{
                         Xi_Binop{
                             Xi_Boolean{true},
                             Xi_Boolean{false},
                             Xi_Op::Or,
                         },
                         Xi_Boolean{true},
                         Xi_Op::And,
                     });

    auto [boolean13, boolean14] =
        Xi_boolexpr("true && (false || true)").value();
    REQUIRE(
        boolean13 == Xi_Binop{
                         Xi_Boolean{true},
                         Xi_Binop{
                             Xi_Boolean{false},
                             Xi_Boolean{true},
                             Xi_Op::Or,
                         },
                         Xi_Op::And});

    // test mathbool
    auto [boolean15, boolean16] = Xi_boolexpr("1 + 2 > 3 * 4").value();
    REQUIRE(
        boolean15 == Xi_Binop{
                         Xi_Binop{
                             Xi_Integer{1},
                             Xi_Integer{2},
                             Xi_Op::Add,
                         },
                         Xi_Binop{
                             Xi_Integer{3},
                             Xi_Integer{4},
                             Xi_Op::Mul,
                         },
                         Xi_Op::Gt,
                     });

    auto [boolean17, boolean18] =
        Xi_boolexpr("(1 + (2 + 3) < 3 * 4) || true").value();
    REQUIRE(
        boolean17 == Xi_Binop{
                         Xi_Binop{
                             Xi_Binop{
                                 Xi_Integer{1},
                                 Xi_Binop{
                                     Xi_Integer{2},
                                     Xi_Integer{3},
                                     Xi_Op::Add,
                                 },
                                 Xi_Op::Add,
                             },
                             Xi_Binop{
                                 Xi_Integer{3},
                                 Xi_Integer{4},
                                 Xi_Op::Mul,
                             },
                             Xi_Op::Lt},
                         Xi_Boolean{true},
                         Xi_Op::Or,
                     });
}

TEST_CASE("Parse Xi_If", "[Xi_If]")
{
    auto [if1, if2] = Xi_if("if true then 1 else 2").value();
    REQUIRE(
        if1 == Xi_If{
                   Xi_Boolean{true},
                   Xi_Integer{1},
                   Xi_Integer{2},
               });
    REQUIRE(if2 == "");

    auto [if3, if4] = Xi_if("if true || false then 1 + 2 else 3 * 4 ").value();
    REQUIRE(
        if3 == Xi_If{
                   Xi_Binop{
                       Xi_Boolean{true},
                       Xi_Boolean{false},
                       Xi_Op::Or,
                   },
                   Xi_Binop{
                       Xi_Integer{1},
                       Xi_Integer{2},
                       Xi_Op::Add,
                   },
                   Xi_Binop{
                       Xi_Integer{3},
                       Xi_Integer{4},
                       Xi_Op::Mul,
                   },
               });
}

TEST_CASE("Parse Xi_Iden", "[Xi_Iden]")
{
    auto [iden1, iden2] = Xi_iden("abc").value();
    REQUIRE(iden1 == Xi_Iden{"abc"});
    REQUIRE(iden2 == "");

    auto [iden3, iden4] = Xi_iden("abc123").value();
    REQUIRE(iden3 == Xi_Iden{"abc123"});
    REQUIRE(iden4 == "");

    auto [iden5, iden6] = Xi_iden("abc_123").value();
    REQUIRE(iden5 == Xi_Iden{"abc_123"});
    REQUIRE(iden6 == "");

    auto [iden7, iden8] = Xi_iden("abc 123").value();
    REQUIRE(iden7 == Xi_Iden{"abc"});
    REQUIRE(iden8 == " 123");
}

TEST_CASE("Parse Xi_Lam", "[Xi_Expr]")
{
    {
        auto [lam, left] = Xi_lam("? x y -> (x + y + 1)").value();
        REQUIRE(
            lam == Xi_Lam{
                       {
                           Xi_Iden{"x"},
                           Xi_Iden{"y"},
                       },
                       Xi_Binop{
                           Xi_Iden{"x"},
                           Xi_Binop{
                               Xi_Iden{"y"},
                               Xi_Integer{1},
                               Xi_Op::Add,
                           },
                       },
                   });
    }

    {
        auto [lam, left] = Xi_lam("? x -> (x + y + 1) - (5 * 2)").value();
        REQUIRE(
            lam == Xi_Lam{
                       {
                           Xi_Iden{"x"},
                       },
                       Xi_Binop{
                           Xi_Binop{
                               Xi_Iden{"x"},
                               Xi_Binop{
                                   Xi_Iden{"y"},
                                   Xi_Integer{1},
                                   Xi_Op::Add,
                               },
                           },
                           Xi_Binop{
                               Xi_Integer{5},
                               Xi_Integer{2},
                               Xi_Op::Mul,
                           },
                           Xi_Op::Sub,
                       },
                   });
    }

    {
        auto [lam, left] = Xi_lam("? x y -> ?z -> x + y + z").value();
        REQUIRE(
            lam == Xi_Lam{
                       {
                           Xi_Iden{"x"},
                           Xi_Iden{"y"},
                       },
                       Xi_Lam{
                           {Xi_Iden{"z"}},
                           Xi_Binop{
                               Xi_Iden{"x"},
                               Xi_Binop{
                                   Xi_Iden{"y"},
                                   Xi_Iden{"z"},
                                   Xi_Op::Add,
                               },
                               Xi_Op::Add,
                           },
                       },
                   });
    }
}

// NOLINTEND(cppcoreguidelines-*, readability*)

} // namespace xi
