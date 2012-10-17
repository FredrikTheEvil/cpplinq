// ----------------------------------------------------------------------------------------------
// Copyright (c) M�rten R�nge.
// ----------------------------------------------------------------------------------------------
// This source code is subject to terms and conditions of the Microsoft Public License. A 
// copy of the license can be found in the License.html file at the root of this distribution. 
// If you cannot locate the  Microsoft Public License, please send an email to 
// dlr@microsoft.com. By using this source code in any fashion, you are agreeing to be bound 
//  by the terms of the Microsoft Public License.
// ----------------------------------------------------------------------------------------------
// You must not remove this notice, or any other, from this software.
// ----------------------------------------------------------------------------------------------
#include "stdafx.h"
// ----------------------------------------------------------------------------------------------
#ifdef _MSC_VER 
#   pragma warning (disable:4100)
#   pragma warning (disable:4996)
#endif
// ----------------------------------------------------------------------------------------------
#undef min
#undef max
// ----------------------------------------------------------------------------------------------
#include <algorithm>
#include <chrono>
#include <map>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>
// ----------------------------------------------------------------------------------------------
#include <limits.h>
#include <stdio.h>
// ----------------------------------------------------------------------------------------------
#include "../CppLinq/cpplinq.hpp"
// ----------------------------------------------------------------------------------------------
#define TEST_PRELUDE()                  test_prelude(__FILE__, __LINE__, __FUNCTION__)
#define TEST_ASSERT(expected, found)    test_assert(__FILE__, __LINE__, expected, #expected, found, #found, (expected == found))
// ---------------------------------------------------------------------------------------------- 
namespace
{

    struct  customer
    {
        std::size_t     id          ;
        std::string     first_name  ;
        std::string     last_name   ;

        customer (std::size_t id = 0, std::string first_name = "", std::string last_name = "")
            :   id          (std::move (id))
            ,   first_name  (std::move (first_name))
            ,   last_name   (std::move (last_name))
        {
        }

        customer & operator= (customer const & c)
        {
            if (std::addressof (c) == this)
            {
                return *this;
            }

            id          = c.id          ;
            first_name  = c.first_name  ;
            last_name   = c.last_name   ;

            return *this;
        }

        customer (customer const & c)
            :   id          (c.id)
            ,   first_name  (c.first_name)
            ,   last_name   (c.last_name)
        {
        }

        customer (customer && c)
            :   id          (std::move (c.id))
            ,   first_name  (std::move (c.first_name))
            ,   last_name   (std::move (c.last_name))
        {
        }

        bool operator==(customer const & c) const
        {
            return id == c.id && first_name == c.first_name && last_name == c.last_name;
        }

        bool operator !=(customer const & c) const
        {
            return !(*this == c);
        }

        bool operator<(customer const & c) const 
        {
            return id < c.id;
        }
    };

    struct  customer_address
    {
        std::size_t     id          ;
        std::size_t     customer_id ;
        std::string     country     ;

        customer_address (std::size_t id, std::size_t customer_id, std::string country)
            :   id          (std::move (id))
            ,   customer_id (std::move (customer_id))
            ,   country     (std::move (country))
        {
        }

    };

    template<typename TValueArray>
    int get_array_size (TValueArray & a)
    {
        return cpplinq::detail::get_array_properties<TValueArray>::size;
    }

    int get_even_counts (int* is, int count)
    {
        auto c = 0;
        for (auto index = 0; index < count; ++index)
        {
            auto i = is[index];
            if (i%2 == 0)
            {
                ++c;
            }
        }
        return c;
    }

    std::size_t             errors          = 0;
    
    std::vector<int>        empty_vector    ;

    std::vector<customer>   empty_customers ;

    customer                customers[] = 
        {
            customer (1 , "Bill"    , "Gates"   ),
            customer (2 , "Steve"   , "Jobs"    ),
            customer (3 , "Richard" , "Stallman"),
            customer (4 , "Linus"   , "Torvalds"),

            customer (11, "Steve"   , "Ballmer" ),
            customer (12, "Tim"     , "Cook"    ),

            customer (21, "Melinda" , "Gates"   ),
        };
    int const           count_of_customers  = get_array_size (customers); 

    customer_address        customer_addresses[] =
        {
            customer_address (1, 1, "USA"       ),
            customer_address (2, 4, "Finland"   ),
            customer_address (3, 4, "USA"       ),
        };

    customer                customers_set1[] = 
        {
            customer (1 , "Bill"    , "Gates"   ),
            customer (2 , "Steve"   , "Jobs"    ),
            customer (3 , "Richard" , "Stallman"),
            customer (4 , "Linus"   , "Torvalds"),
            customer (3 , "Richard" , "Stallman"),
            customer (2 , "Steve"   , "Jobs"    ),
            customer (1 , "Bill"    , "Gates"   ),
        };

    customer                customers_set2[] = 
        {
            customer (1 , "Bill"    , "Gates"   ),
            customer (11, "Steve"   , "Ballmer" ),
            customer (12, "Tim"     , "Cook"    ),
        };

    int                 ints[]                  = {3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5};
    int const           count_of_ints           = get_array_size (ints);

    int const           even_count_of_ints      = get_even_counts (ints, count_of_ints);

    int                 simple_ints[]           = {1,2,3,4,5,6,7,8,9};
    int const           count_of_simple_ints    = get_array_size (simple_ints);

    int                 set1[]                  = {5,4,3,2,1,2,3,4,5};
    int const           count_of_set1           = get_array_size (set1);
    int                 set2[]                  = {9,8,4,5,6,7,1,8,9};
    int const           count_of_set2           = get_array_size (set2);

    auto                is_even                 = [](int i) {return i%2==0;};
    auto                is_odd                  = [](int i) {return i%2==1;};
    auto                smaller_than_five       = [](int i) {return i < 5;};
    auto                greater_than_five       = [](int i) {return i > 5;};
    auto                double_it               = [](int i) {return i+i;};
    auto                sum_aggregator          = [](int s, int i) {return s+i;};
    auto                mul_aggregator          = [](int s, int i) {return s*i;};
    auto                to_string               = [](int i) {std::stringstream sstr; sstr<<i; return sstr.str ();};


    void test_prelude (
            char const *    file
        ,   int             line_no
        ,   char const *    test
        )
    {
        printf (
                "%s(%d): RUNNING: %s\r\n"
            ,   file
            ,   line_no
            ,   test
            );
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   bool            expected
        ,   char const *    expected_name
        ,   bool            found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %s(%s), FOUND: %s(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected    ? "true" : "false"
                ,   expected_name
                ,   found       ? "true" : "false"
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   std::string     expected
        ,   char const *    expected_name
        ,   std::string     found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %s(%s), FOUND: %s(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected.c_str ()
                ,   expected_name
                ,   found.c_str ()
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   int             expected
        ,   char const *    expected_name
        ,   int             found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %d(%s), FOUND: %d(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected
                ,   expected_name
                ,   found
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   customer        expected
        ,   char const *    expected_name
        ,   customer        found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: (%d,%s,%s)(%s), FOUND: (%d,%s,%s)(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected.id, expected.first_name.c_str (), expected.last_name.c_str ()
                ,   expected_name
                ,   found.id, found.first_name.c_str (), found.last_name.c_str ()
                ,   found_name
                );
        }

        return result;
    }

    void test_int_at (int index, int v)
    {
        if (
                TEST_ASSERT (true, (index > -1))
            &&  TEST_ASSERT (true, (index < count_of_ints))
            &&  TEST_ASSERT (ints[index], v)
            )
        {
        }
        else
        {
            printf ("    @index:%d\r\n", index);
        }
    }

    void test_opt ()
    {
        using namespace cpplinq::detail;

        TEST_PRELUDE ();

        {
            opt<int> o;
            TEST_ASSERT (false, o.has_value ());
            TEST_ASSERT (false, o);

        }
        {
            opt<int> o1;
            opt<int> o2 (10);
            TEST_ASSERT (false, o1.has_value ());
            TEST_ASSERT (false, o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT (10, *o2);

            opt<int> o3 (o2);
            opt<int> o4 (o1);
            o3.swap (o4);
            TEST_ASSERT (false, o3.has_value ());
            TEST_ASSERT (false, o3);
            TEST_ASSERT (true, o4.has_value ());
            TEST_ASSERT (true, (bool)o4);
            TEST_ASSERT (10, *o4);

            o1.swap (o2);
            TEST_ASSERT (true, o1.has_value ());
            TEST_ASSERT (true, (bool)o1);
            TEST_ASSERT (10, *o1);
            TEST_ASSERT (false, o2.has_value ());
            TEST_ASSERT (false, o2);

            o2 = o1;
            TEST_ASSERT (true, o1.has_value ());
            TEST_ASSERT (true, (bool)o1);
            TEST_ASSERT (10, *o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT (10, *o2);

            o1 = 11;
            o2 = 12;
            TEST_ASSERT (true, o1.has_value ());
            TEST_ASSERT (true, (bool)o1);
            TEST_ASSERT (11, *o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT (12, *o2);

        }
        {
            opt<std::string> o1;
            opt<std::string> o2 ("Test");

            TEST_ASSERT (false, o1.has_value ());
            TEST_ASSERT (false, o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT ("Test", *o2);
            TEST_ASSERT (4, (int)o2->size ());
        
            o1 = "Test2";
            o2 = "Test3";
            TEST_ASSERT (true, o1.has_value ());
            TEST_ASSERT (true, (bool)o1);
            TEST_ASSERT ("Test2", *o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT ("Test3", *o2);

            o1.swap (o2);
            TEST_ASSERT (true, o1.has_value ());
            TEST_ASSERT (true, (bool)o1);
            TEST_ASSERT ("Test3", *o1);
            TEST_ASSERT (true, o2.has_value ());
            TEST_ASSERT (true, (bool)o2);
            TEST_ASSERT ("Test2", *o2);
        }
    }

    void test_from ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto q = from (empty_vector);

            typedef decltype (q.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }
        {
            auto q = from_array (ints);

            typedef decltype (q.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (count_of_ints, index);
        }
        {
            auto q = from_copy (empty_vector);

            typedef decltype (q.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }
        {
            std::vector<int> is (ints, ints + count_of_ints);
            auto q = from_copy (is);

            typedef decltype (q.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (count_of_ints, index);
        }
        {
            auto q = from_array (customers);
            typedef decltype (q.front ())   return_type;
            static_assert (
                    std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = customer"
                );
        }
        {
            std::vector<customer> cs;
            auto q = from_copy (cs);
            typedef decltype (q.front ())   return_type;
            static_assert (
                    std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = customer"
                );
        }
    }

    void test_range ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto start = 12;
            auto count = 10;
            auto index = start;

            auto q = range (start, count);

            typedef decltype (q.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            while (q.next ())
            {
                if (!TEST_ASSERT (index, q.front ()))
                {
                        printf ("    @index:%d\r\n", index);
                }
                ++index;
            }
            TEST_ASSERT (start + count , index);
        }
    }

    void test_repeat ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto r = repeat (42, 0);

            typedef decltype (r.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            bool isempty = !r.next ();

            TEST_ASSERT (true, isempty);
        }

        {
            auto value = 42;
            int count = 10;
            int total = 0;

            auto r = repeat (value, count);

            typedef decltype (r.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            while (r.next ())
            {
                if (!TEST_ASSERT (value, r.front ()))
                {
                        printf ("    @index:%d\r\n", total);
                }
                ++total;
            }
            TEST_ASSERT (total , count);
        }

        {
            auto value = customers[0];
            int count = 10;
            int total = 0;

            auto r = repeat (value, count);

            while (r.next ())
            {
                if (!TEST_ASSERT (value, r.front ()))
                {
                        printf ("    @index:%d\r\n", total);
                }
                ++total;
            }
            TEST_ASSERT (total , count);
        }
    }

    void test_empty ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto r = empty<int>();

            typedef decltype (r.front ())   return_type;
            static_assert (
                    !std::is_reference<return_type>::value 
                ,   "front () must return non-reference when value_type = int"
                );

            bool isempty = !r.next ();

            TEST_ASSERT (true, isempty);
        }

        {
            auto r = empty<customer>();

            bool isempty = !r.next ();

            TEST_ASSERT (true, isempty);
        }

        {
            auto customers = empty<customer>() >> to_list ();
            TEST_ASSERT (0, (int)customers.size ());
        }

    }

    void test_count ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto count_result = from (empty_vector) >> count ();
            TEST_ASSERT (0, (int)count_result);
        }

        {
            auto count_result = from_array (ints) >> count ();
            TEST_ASSERT (count_of_ints, (int)count_result);
        }

        {
            auto count_result = from (empty_vector) >> count (is_even);
            TEST_ASSERT (0, (int)count_result);
        }

        {
            auto count_result = from_array (ints) >> count (is_even);
            TEST_ASSERT (even_count_of_ints, (int)count_result);
        }

    }

    void test_any ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto any_result = from (empty_vector) >> any ();
            TEST_ASSERT (false, any_result);
        }

        {
            auto any_result = from_array (ints) >> any ();
            TEST_ASSERT (true, any_result);
        }

        {
            auto any_result = from (empty_vector) >> any (is_even);
            TEST_ASSERT (false, any_result);
        }

        {
            auto any_result = from_array (ints) >> any (is_even);
            TEST_ASSERT (true, any_result);
        }
    }

    void test_first_or_default ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int first_result = from (empty_vector) >> first_or_default ();
            TEST_ASSERT (0, first_result);
        }

        {
            int first_result = from_array (ints) >> first_or_default ();
            TEST_ASSERT (3, first_result);
        }

        {
            int first_result = from (empty_vector) >> first_or_default (is_even);
            TEST_ASSERT (0, first_result);
        }

        {
            int first_result = from_array (ints) >> first_or_default (is_even);
            TEST_ASSERT (4, first_result);
        }

    }

    void test_last_or_default ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int first_result = from (empty_vector) >> last_or_default ();
            TEST_ASSERT (0, first_result);
        }

        {
            int first_result = from_array (ints) >> last_or_default ();
            TEST_ASSERT (5, first_result);
        }

        {
            int first_result = from (empty_vector) >> last_or_default (is_even);
            TEST_ASSERT (0, first_result);
        }

        {
            int first_result = from_array (ints) >> last_or_default (is_even);
            TEST_ASSERT (2, first_result);
        }
    }

    void test_sum ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int sum_result = from (empty_vector) >> sum ();
            TEST_ASSERT (0, sum_result);
        }

        {
            int sum_of_ints = std::accumulate (ints, ints + count_of_ints, 0);
            int sum_result = from_array (ints) >> sum ();
            TEST_ASSERT (sum_of_ints, sum_result);
        }

        {
            int sum_result = from (empty_vector) >> sum (double_it);
            TEST_ASSERT (0, sum_result);
        }

        {
            int sum_of_ints = std::accumulate (ints, ints + count_of_ints, 0);
            int sum_result = from_array (ints) >> sum (double_it);
            TEST_ASSERT (2*sum_of_ints, sum_result);
        }
    }

    void test_min ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int min_result = from (empty_vector) >> min ();
            TEST_ASSERT (INT_MAX, min_result);
        }

        {
            int min_result = from_array (ints) >> min ();
            TEST_ASSERT (1, min_result);
        }

        {
            int min_result = from (empty_vector) >> min (double_it);
            TEST_ASSERT (INT_MAX, min_result);
        }

        {
            int min_result = from_array (ints) >> min (double_it);
            TEST_ASSERT (2, min_result);
        }
    }

    void test_avg ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int avg_result = from (empty_vector) >> avg ();
            TEST_ASSERT (0, avg_result);
        }

        {
            int avg_result = from_array (ints) >> avg ();
            TEST_ASSERT (4, avg_result);
        }

        {
            int avg_result = from (empty_vector) >> avg (double_it);
            TEST_ASSERT (0, avg_result);
        }

        {
            int avg_result = from_array (ints) >> avg (double_it);
            TEST_ASSERT (9, avg_result);
        }
    }

    void test_max ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int max_result = from (empty_vector) >> max ();
            TEST_ASSERT (INT_MIN, max_result);
        }

        {
            int max_result = from_array (ints) >> max ();
            TEST_ASSERT (9, max_result);
        }

        {
            int max_result = from (empty_vector) >> max (double_it);
            TEST_ASSERT (INT_MIN, max_result);
        }

        {
            int max_result = from_array (ints) >> max (double_it);
            TEST_ASSERT (18, max_result);
        }

    }

    void test_concatenate ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::wstring concatenate_result = 
                    from (empty_vector) 
                >>  select ([] (int i){return std::wstring ();})
                >>  concatenate (L"")
                ;
            TEST_ASSERT (true, concatenate_result.empty ());
        }

        {
            std::string concatenate_result = 
                    from_array (customers) 
                >>  select ([](customer const & c){return c.last_name;})
                >>  concatenate (", ")
                ;

            TEST_ASSERT ("Gates, Jobs, Stallman, Torvalds, Ballmer, Cook, Gates", concatenate_result);
        }
    }

    void test_for_each ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int index = 0;
            from (empty_vector) >> for_each ([&](int i){test_int_at (index, i); ++index;});
            TEST_ASSERT (0, index);
        }

        {
            int index = 0;
            from_array (ints) >> for_each ([&](int i){test_int_at (index, i); ++index;});
            TEST_ASSERT (count_of_ints, index);

        }
    }

    void test_all ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int index = 0;
            auto all_result = from (empty_vector) >> all ([&](int i){test_int_at (index, i); ++index; return true;});
            TEST_ASSERT (true, all_result);
            TEST_ASSERT (0, index);
        }

        {
            int index = 0;
            auto all_result = from_array (ints) >> all ([&](int i){test_int_at (index, i); ++index; return true;});
            TEST_ASSERT (true, all_result);
            TEST_ASSERT (count_of_ints, index);

        }

        {
            int index = 0;
            auto all_result = from_array (ints) >> all ([&](int i){test_int_at (index, i); ++index; return index < 10;});
            TEST_ASSERT (false, all_result);
            TEST_ASSERT (10, index);

        }
    }

    void test_to_vector ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::vector<int> to_vector_result = from (empty_vector) >> to_vector ();
            TEST_ASSERT (0, (int)to_vector_result.size ());
        }

        {
            std::vector<int> to_vector_result = from_array (ints) >> to_vector ();
            TEST_ASSERT (count_of_ints, (int)to_vector_result.size ());
            for (auto index = 0U; index < to_vector_result.size (); ++index)
            {
                test_int_at (index, to_vector_result[index]);
            }
        }
    }

    void test_to_map ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::map<int,int> to_map_result = from (empty_vector) >> to_map ([](int i){return i;});
            TEST_ASSERT (0, (int)to_map_result.size ());
        }

        {
            auto to_map_result = from_array (customers) >> to_map ([](customer const & c){return c.id;});
            TEST_ASSERT (count_of_customers, (int)to_map_result.size ());

            for (auto index = 0; index < count_of_customers; ++index)
            {
                auto c1 = customers[index];
                auto find_c2 = to_map_result.find (c1.id);
                if (TEST_ASSERT (true, (find_c2 != to_map_result.end ())))
                {
                    auto c2 = find_c2->second;

                    if (
                            TEST_ASSERT ((int)c1.id, (int)c2.id)
                        &&  TEST_ASSERT (c1.first_name, c2.first_name)
                        &&  TEST_ASSERT (c1.last_name, c2.last_name)
                        )
                    {
                    }
                    else
                    {
                        printf ("    @index:%d\r\n", index);
                    }
                }
            }
        }
    }

    void test_to_list ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::list<int> to_list_result = from (empty_vector) >> to_list ();
            TEST_ASSERT (0, (int)to_list_result.size ());
        }

        {
            std::list<int> to_list_result = from_array (ints) >> to_list ();
            TEST_ASSERT (count_of_ints, (int)to_list_result.size ());

            auto pos = to_list_result.begin ();
            for (auto index = 0U; index < to_list_result.size (); ++index)
            {
                test_int_at (index, *pos++);
            }
        }
    }

    void test_container ()
    {
        using namespace cpplinq;
        using namespace cpplinq::experimental;

        TEST_PRELUDE ();

        {
            auto container_result = from (empty_vector) >> container ();
            std::vector<int> v (container_result.begin (), container_result.end ());
            TEST_ASSERT (0, (int)v.size ());
        }

        {
            auto container_result = from_iterators (ints, ints + count_of_ints) >> container ();
            std::vector<int> v (container_result.begin (), container_result.end ());
            if (TEST_ASSERT (count_of_ints, (int)v.size ()))
            {
                for (auto index = 0; index < count_of_ints; ++index)
                {
                    test_int_at (index, v[index]);
                }
            }
        }
        {
            auto container_result   = from_array (customers) >> container ();
            auto begin              = container_result.begin ();
            auto end                = container_result.end ();

            TEST_ASSERT (true, (begin != end));
            TEST_ASSERT (false, (begin == end));

            auto c = *begin;

            TEST_ASSERT (1, (int)c.id);
            TEST_ASSERT (1, (int)begin->id);
            TEST_ASSERT (1, (int)(*begin).id);


        }
    }

    void test_where ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto c = from (empty_vector) >> where (is_even) >> count ();
            TEST_ASSERT (0, (int)c);
        }

        {
            auto c = from_array (ints) >> where (is_even) >> count ();
            TEST_ASSERT (even_count_of_ints, (int)c);
        }
    }

    void test_select ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::vector<double> select_result = from (empty_vector) >> select ([](int i){return 1.0*i;}) >> to_vector ();
            TEST_ASSERT (0, (int)select_result.size ());
        }

        {
            std::vector<std::size_t> select_result = 
                    from_array (customers) 
                >>  select ([](customer const & c){return c.id;}) 
                >>  to_vector ()
                ;

            auto index = 0U;
            for (auto sz : select_result)
            {
                if (!TEST_ASSERT ((int)customers[index].id, (int)sz))
                {
                    printf ("    @index:%d\r\n", index);
                }

                ++index;
            }

            TEST_ASSERT (count_of_customers, (int)select_result.size ());
        }

    }

    void test_join ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto cs     = empty<customer> ();
            auto cas    = empty<customer_address> ();

            auto join_result = cs
                >> join (
                        cas
                    ,   [](customer const & c) {return c.id;}
                    ,   [](customer_address const & ca) {return ca.customer_id;}
                    ,   [](customer const & c, customer_address const & ca) {return std::make_pair (c, ca);}
                    )
                >> to_vector ()
                ;

            TEST_ASSERT (0, (int)join_result.size ());
        }
        {
            auto cas    = empty<customer_address> ();

            auto join_result = from_array (customers)
                >> join (
                        cas
                    ,   [](customer const & c) {return c.id;}
                    ,   [](customer_address const & ca) {return ca.customer_id;}
                    ,   [](customer const & c, customer_address const & ca) {return std::make_pair (c, ca);}
                    )
                >> to_vector ()
                ;

            TEST_ASSERT (0, (int)join_result.size ());
        }
        {
            auto cs     = empty<customer> ();

            auto join_result = cs
                >> join (
                        from_array (customer_addresses)
                    ,   [](customer const & c) {return c.id;}
                    ,   [](customer_address const & ca) {return ca.customer_id;}
                    ,   [](customer const & c, customer_address const & ca) {return std::make_pair (c, ca);}
                    )
                >> to_vector ()
                ;

            TEST_ASSERT (0, (int)join_result.size ());
        }
        {
            auto join_result = from_array (customers)
                >> join (
                        from_array (customer_addresses)
                    ,   [](customer const & c) {return c.id;}
                    ,   [](customer_address const & ca) {return ca.customer_id;}
                    ,   [](customer const & c, customer_address const & ca) {return std::make_pair (c, ca);}
                    )
                >> to_vector ()
                ;

            if (TEST_ASSERT (3, (int)join_result.size ()))
            {
                {
                    auto result = join_result[0];
                    TEST_ASSERT (1, (int)result.first.id);
                    TEST_ASSERT (1, (int)result.second.id);
                }
                {
                    auto result = join_result[1];
                    TEST_ASSERT (4, (int)result.first.id);
                    TEST_ASSERT (2, (int)result.second.id);
                }
                {
                    auto result = join_result[2];
                    TEST_ASSERT (4, (int)result.first.id);
                    TEST_ASSERT (3, (int)result.second.id);
                }
            }
        }
    }

    void test_select_many ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            std::vector<char> select_many_result = 
                    from_iterators (customers, customers) 
                >>  select_many ([](customer const & c){return from (c.last_name);}) 
                >>  to_vector ()
                ;

            TEST_ASSERT (0, (int)select_many_result.size ());
        }
        {
            std::vector<char> expected; 
            for (auto customer : customers)
            {
                expected.insert (
                        expected.end ()
                    ,   customer.last_name.begin ()
                    ,   customer.last_name.end ()
                    );
            }

            std::vector<char> select_many_result = 
                    from_array (customers) 
                >>  select_many ([](customer const & c){return from (c.last_name);}) 
                >>  to_vector ()
                ;

            if (TEST_ASSERT ((int)expected.size (), (int)select_many_result.size ()))
            {
                for (auto index = 0U; index < expected.size (); ++index)
                {
                    if (!TEST_ASSERT (expected[index], select_many_result[index]))
                    {
                        printf ("    @index:%d\r\n", index);
                    }
                }
            }
        }

    }

    void test_orderby ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto c = from (empty_vector) >> orderby_ascending ([](int i){return i;}) >> count ();
            TEST_ASSERT (0, (int)c);
        }

        const int test_set_size = 7;

        auto verify = [=](
                int expected[test_set_size]
            ,   std::vector<customer> const & sequence
            )
        {
            auto sz = sequence.size ();
            if (TEST_ASSERT (test_set_size, (int)sz))
            {
                auto index = 0U;
                for (auto c : sequence)
                {
                    if (!TEST_ASSERT (expected[index], (int)c.id))
                    {
                        printf ("    @index:%d\r\n", index);
                    }

                    ++index;
                }

            }
        };

        {
            int expected[] = 
                {
                    11,
                    12,
                    1,
                    21,
                    2,
                    3,
                    4,
                };

            auto sequence = 
                    from_array (customers) 
                >>  orderby_ascending ([] (customer const & c) {return c.last_name;})
                >>  thenby_ascending ([] (customer const & c) {return c.first_name;})
                >>  to_vector ()
                ;

            verify (expected, sequence);
        }
        {
            int expected[] = 
                {
                    4,
                    3,
                    2,
                    21,
                    1,
                    12,
                    11,
                };

            auto sequence = 
                    from_array (customers) 
                >>  orderby_descending ([] (customer const & c) {return c.last_name;})
                >>  thenby_descending ([] (customer const & c) {return c.first_name;})
                >>  to_vector ()
                ;

            verify (expected, sequence);
        }
        {
            int expected[] = 
                {
                    11,
                    12,
                    21,
                    1,
                    2,
                    3,
                    4,
                };

            auto sequence = 
                    from_array (customers) 
                >>  orderby ([] (customer const & c) {return c.last_name;}, true)
                >>  thenby ([] (customer const & c) {return c.first_name;}, false)
                >>  to_vector ()
                ;

            verify (expected, sequence);
        }
    }

    void test_reverse()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        // reverse an empty range
        {
            auto result = empty<int>() >> reverse() >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // reverse an empty range
        {
            auto result = from(empty_vector) >> reverse() >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // reverse a non-empty range
        {
            int expected[] = {9,8,7,6,5,4,3,2,1,0};
            int expected_size = get_array_size(expected);

            auto result = range(0, 10) >> reverse() >> to_vector();
            TEST_ASSERT(expected_size, (int)result.size());

            for(int i = 0; i < expected_size && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }
    }

    void test_skip ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto q = from (empty_vector) >> skip (5);
            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }
        {
            auto q = from_array (ints) >> skip (5);

            auto index = 5;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (count_of_ints, index);
        }
    }

    void test_skip_while ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto q = from (empty_vector) >> skip_while (smaller_than_five);
            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }
        {
            auto q = from_array (ints) >> skip_while (smaller_than_five);
            auto index = 4;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (count_of_ints, index);
        }
    }

    void test_take ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto q = from (empty_vector) >> take (5);
            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }
        {
            auto q = from_array (ints) >> take (5);

            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (5, index);
        }
    }

    void test_take_while ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto q = from (empty_vector) >> take_while (smaller_than_five);
            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (0, index);
        }

        {
            auto q = from_array (ints) >> take_while (smaller_than_five);
            auto index = 0;

            while (q.next ())
            {
                test_int_at (index, q.front ());
                ++index;
            }
            TEST_ASSERT (4, index);
        }
    }

    void test_contains ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            bool result = from (empty_vector) >> contains (1);
            TEST_ASSERT (false, result);
        }

        {
            bool result = from_array (ints) >> contains (1);
            TEST_ASSERT (true, result);
        }

        {
            bool result = 
                from (empty_customers) 
                >> contains (
                    customer (1, "Bill", "Gates"),
                    [](customer const& c1, customer const& c2) {return c1.id == c2.id;});

            TEST_ASSERT (false, result);
        }

        {
            bool result = 
                from_array (customers) 
                >> contains (
                    customer (1, "Bill", "Gates"),
                    [](customer const& c1, customer const& c2) {return c1.id == c2.id;});

            TEST_ASSERT (true, result);
        }

        {
            bool result = 
                from_array (customers) 
                >> contains (
                    customer (42, "Bill", "Gates"),
                    [](customer const& c1, customer const& c2) {return c1.id == c2.id;});

            TEST_ASSERT (false, result);
        }
    }

    void test_element_at_or_default ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            auto result = from (empty_vector) >> element_at_or_default (0);
            TEST_ASSERT (0, result);
        }

        {
            auto result = from (empty_vector) >> element_at_or_default (1);
            TEST_ASSERT (0, result);
        }

        {
            auto result = from_array (ints) >> element_at_or_default (0);
            TEST_ASSERT (3, result);
        }

        {
            auto result = from_array (ints) >> element_at_or_default (1);
            TEST_ASSERT (1, result);
        }

        {
            auto result = from_array (ints) >> element_at_or_default (count_of_ints-1);
            TEST_ASSERT (5, result);
        }

        {
            auto result = from_array (ints) >> element_at_or_default (count_of_ints);
            TEST_ASSERT (0, result);
        }

    }

    void test_aggregate ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        {
            int sum_result = from (empty_vector) >> aggregate (0, sum_aggregator);
            TEST_ASSERT (0, sum_result);
        }

        {
            int sum_of_simple_ints = std::accumulate (simple_ints, simple_ints + count_of_simple_ints, 0);
            int sum_result = from_array (simple_ints) >> aggregate (0, sum_aggregator);
            TEST_ASSERT (sum_of_simple_ints, sum_result);
        }

        {
            int prod_of_simple_ints = std::accumulate (simple_ints, simple_ints + count_of_simple_ints, 1, mul_aggregator);
            int sum_result = from_array (simple_ints) >> aggregate (1, mul_aggregator);
            TEST_ASSERT (prod_of_simple_ints, sum_result);
        }

        {
            auto sum_result = from (empty_vector) >> aggregate (0, sum_aggregator, to_string);
            TEST_ASSERT ("0", sum_result);
        }

        {
            auto sum_of_simple_ints = to_string (std::accumulate (simple_ints, simple_ints + count_of_simple_ints, 0));
            auto sum_result = from_array (simple_ints) >> aggregate (0, sum_aggregator, to_string);
            TEST_ASSERT (sum_of_simple_ints, sum_result);
        }

        {
            auto prod_of_simple_ints = to_string (std::accumulate (simple_ints, simple_ints + count_of_simple_ints, 1, mul_aggregator));
            auto sum_result = from_array (simple_ints) >> aggregate (1, mul_aggregator, to_string);
            TEST_ASSERT (prod_of_simple_ints, sum_result);
        }
    }

    void test_distinct()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();
        
        {
            auto d = from(empty_vector) >> distinct() >> to_vector();
            TEST_ASSERT(0, (int)d.size());
        }
        
        {
            int expected[] = {5,4,3,2,1};
            const int expected_size = (int)(sizeof(expected)/sizeof(int));

            auto result = from_array(set1) >> distinct() >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        {
            auto d = from_array(customers_set1) >> distinct() >> to_vector();
            TEST_ASSERT(4, (int)d.size());
        }

    }

    void test_union_with()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();
        
        // union of two empty ranges
        {
            auto result = from(empty_vector) >> union_with( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // union of an empty range with a non-empty range
        {
            auto result = empty<int>() >> union_with( range(0, 10) ) >> to_vector();
            TEST_ASSERT(10, (int)result.size());
            for(int i = 0; i < 10 && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(i, result[i]);
            }
        }

        // union of an empty range with a non-empty range
        {
            int expected[] = {5,4,3,2,1};
            int expected_size = get_array_size(expected);
            auto result = from(empty_vector) >> union_with( from_array(set1) ) >> to_vector();
            TEST_ASSERT(expected_size, (int)result.size());

            for(int i = 0; i < (int)result.size() && i < expected_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        // union of a non-empty range with an empty range
        {
            auto result = range(0, 10) >> union_with( empty<int>() ) >> to_vector();
            TEST_ASSERT(10, (int)result.size());
            for(int i = 0; i < 10 && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(i, result[i]);
            }
        }

        // union of a non-empty range with an empty range
        {
            int expected[] = {5,4,3,2,1};
            int expected_size = get_array_size(expected);
            auto result = from_array(set1) >> union_with( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(expected_size, (int)result.size());
            for(int i = 0; i < expected_size && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        // union of two non-empty ranges
        {
            int expected[] = {5,4,3,2,1,9,8,6,7};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set1) >> union_with(from_array(set2)) >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }
    }

    void test_intersect_with()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();
        
        // intersection of two empty ranges
        {
            auto result = from(empty_vector) >> intersect_with( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // intersection of an empty range with a non-empty range
        {
            auto result = empty<int>() >> intersect_with( range(0, 10) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // intersection of an empty range with a non-empty range
        {
            auto result = from(empty_vector) >> intersect_with( from_array(set1) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // intersection of a non-empty range with an empty range
        {
            auto result = range(0, 10) >> intersect_with( empty<int>() ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // intersection of a non-empty range with an empty range
        {
            auto result = from_array(set1) >> intersect_with( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // intersection of two non-empty ranges
        {
            int expected[] = {5,4,1};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set1) >> intersect_with(from_array(set2)) >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        // intersection of two non-empty ranges
        {
            int expected[] = {4,5,1};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set2) >> intersect_with(from_array(set1)) >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

    }

    void test_except()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();
        
        // difference of two empty ranges
        {
            auto result = from(empty_vector) >> except( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // difference of an empty range with a non-empty range
        {
            auto result = empty<int>() >> except( range(0, 10) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // difference of an empty range with a non-empty range
        {
            auto result = from(empty_vector) >> except( from_array(set1) ) >> to_vector();
            TEST_ASSERT(0, (int)result.size());
        }

        // difference of a non-empty range with an empty range
        {
            auto result = range(0, 10) >> except( empty<int>() ) >> to_vector();
            TEST_ASSERT(10, (int)result.size());
            for(int i = 0; i < 10 && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(i, result[i]);
            }
        }

        // difference of a non-empty range with an empty range
        {
            int expected[] = {5,4,3,2,1};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set1) >> except( from(empty_vector) ) >> to_vector();
            TEST_ASSERT(expected_size, (int)result.size());
            for(int i = 0; i < expected_size && i < (int)result.size(); ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        // difference of two non-empty ranges
        {
            int expected[] = {3,2};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set1) >> except(from_array(set2)) >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }

        // difference of two non-empty ranges
        {
            int expected[] = {9,8,6,7};
            const int expected_size = sizeof(expected)/sizeof(int);

            auto result = from_array(set2) >> except(from_array(set1)) >> to_vector();
            auto result_size = (int)result.size();

            TEST_ASSERT(expected_size, result_size);
            for(int i =0; i < expected_size && i < result_size; ++i)
            {
                TEST_ASSERT(expected[i], result[i]);
            }
        }
    }

    template<typename TPredicate>
    long long execute_testruns (
            std::size_t test_runs
        ,   TPredicate predicate
        )
    {
        auto then = std::chrono::high_resolution_clock::now ();

        for (auto test_run = 0U; test_run < test_runs; ++test_run)
        {
            predicate ();
        }

        auto now = std::chrono::high_resolution_clock::now ();

        auto diff = now - then;

        auto diff_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count ();

        return diff_in_ms;
    }

    void test_performance_sum ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

        int         const test_repeat   = 80000     ;
        int         const test_size     = 20000     ;

        srand (19740531);

        auto test_set = 
                range (0, test_size)
            >>  select ([] (int i){return rand ();})
            >>  to_vector (test_size)
            ;

        auto expected_complete_sum = 0;
        auto expected = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    auto set_sum = 0;
                    for (auto v : test_set)
                    {
                        set_sum += v;
                    }
                    expected_complete_sum += set_sum;
                }
            );

        auto result_complete_sum = 0;
        auto result = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    auto set_sum =
                            from (test_set)
                        >>  sum ()
                        ;
                    result_complete_sum += set_sum;
                }                 
            );

        printf ("Expected sum: %d\r\n", expected_complete_sum);
        printf ("Result sum: %d\r\n", result_complete_sum);

        auto ratio_limit    = 3.0; 
        auto ratio          = ((double)expected)/result;
        TEST_ASSERT (true, (ratio > 1/ratio_limit && ratio < ratio_limit));
        printf (
                "Performance numbers for simple sum over numbers, expected:%d, result:%d, ratio_limit:%f, ratio:%f\r\n"
            ,   (int)expected
            ,   (int)result
            ,   ratio_limit
            ,   ratio
            );
    }

   bool is_prime (int i)
    {
        if (i < 2)
        {
            return false;
        }
        else if (i == 2)
        {
            return true;
        }
        else
        {
            auto r = (int)std::ceil (std::sqrt (i));

            for (auto iter = 2; iter <= r; ++iter)
            {
                if (i % iter == 0)
                {
                    return false;
                }
            }

            return true;
        }
    }

    void test_performance_is_prime ()
    {
        using namespace cpplinq;

        TEST_PRELUDE ();

#if _DEBUG
        int         const test_repeat   = 25        ;
#else
        int         const test_repeat   = 100       ;
#endif
        int         const test_size     = 10000     ;

        auto expected = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    auto        iter       = 2;
                    auto        count      = 0;
                    auto        prime_sum  = 0;
                    while (count < test_size)
                    {
                        if (is_prime (iter))
                        {
                            prime_sum += iter;
                            ++count;
                        }
                        ++iter;
                    }
                }
            );

        auto result = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    auto prime_sum =
                            range (2, INT_MAX)
                        >>  where (is_prime)
                        >>  take (test_size)
                        >>  sum ()
                        ;

                    // To suppress not used warnings
                    prime_sum = prime_sum;
                }                 
            );

        auto ratio_limit    = 1.25; 
        auto ratio          = ((double)expected)/result;
        TEST_ASSERT (true, (ratio > 1/ratio_limit && ratio < ratio_limit));
        printf (
                "Performance numbers for computing primes, expected:%d, result:%d, ratio_limit:%f, ratio:%f\r\n"
            ,   (int)expected
            ,   (int)result
            ,   ratio_limit
            ,   ratio
            );
    }

    bool run_all_tests (bool run_perfomance_tests)
    {
        // -------------------------------------------------------------------------
        test_opt                    ();
        test_from                   ();
        test_range                  ();
        test_repeat                 ();
        test_empty                  ();
        test_count                  ();
        test_any                    ();
        test_first_or_default       ();
        test_last_or_default        ();
        test_sum                    ();
        test_avg                    ();
        test_max                    ();
        test_min                    ();
        test_concatenate            ();
        test_all                    ();
        test_for_each               ();
        test_to_vector              ();
        test_to_map                 ();
        test_to_list                ();
        test_container              ();
        test_where                  ();
        test_select                 ();
        test_select_many            ();
        test_join                   ();
        test_orderby                ();
        test_reverse                ();
        test_take                   ();
        test_skip                   ();
        test_take_while             ();
        test_skip_while             ();
        test_contains               ();
        test_element_at_or_default  ();
        test_aggregate              ();
        test_distinct               ();
        test_union_with             ();
        test_intersect_with         ();
        test_except                 ();
        // -------------------------------------------------------------------------
        if (run_perfomance_tests)
        {
            test_performance_sum ();
            test_performance_is_prime ();
        }
        // -------------------------------------------------------------------------
        if (errors == 0)
        {
            printf ("PASS\r\n");
        }
        else
        {
            printf ("FAIL\r\n");
        }
        // -------------------------------------------------------------------------
        return errors > 0;
        // -------------------------------------------------------------------------
    }
} 



int main ()
{
    //for (auto iter = 0; iter < 100; ++iter)
    //{
    //    run_all_tests (false);
    //}

    auto r = run_all_tests (
#if _DEBUG
            false
#else
            true
#endif
        );

    //_CrtDumpMemoryLeaks ();

    return r ? 101 : 0;
}
// ---------------------------------------------------------------------------- -

