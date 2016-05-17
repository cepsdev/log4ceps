/*
Copyright 2014, cpsdev (cepsdev@hotmail.com).
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of Google Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef INC_AST_COMPUTATIONS_HPP
#define INC_AST_COMPUTATIONS_HPP

#include <string>
#include <vector>
#include <map>

#include "ceps_all.hh"

#include "cmdline_utils.hpp"



using namespace std;
using namespace ceps;
using namespace ceps::ast;

extern map<string, int> global_counter;

void expand_global_sequencer(Nodeset& attr_value);
bool has_struct_defs(ceps::ast::Nodeset const & ns);
string get_name_of_first_struct_def(ceps::ast::Nodeset const & ns);
ceps::ast::Nodeset eval_path_expr(ceps::ast::Nodeset const & pattern, ceps::ast::Nodeset const & ns, std::string & path);
vector<string> get_attribute_names(ceps::ast::Nodeset const & ns);
set<string>  get_attribute_names_as_set(ceps::ast::Nodeset const & ns);
inline bool is_a_struct_def(Nodebase_ptr p);
inline bool is_a_struct_def(ceps::ast::Nodeset const & ns);
bool empty_struct(ceps::ast::Nodeset const & struct_ns);
ceps::ast::Int* copy_Int(Nodebase_ptr p);
ceps::ast::Double* copy_Double(Nodebase_ptr p);
ceps::ast::String* copy_String(Nodebase_ptr p);
ceps::ast::Symbol* copy_Symbol(Nodebase_ptr p);
ceps::ast::Nodeset compute_instance_using_inheritance_relation(ceps::ast::Nodeset const & defaults,
																ceps::ast::Nodeset const & sub_ns,
																ceps::ast::Nodeset const & universe);
Nodebase_ptr deep_copy_struct(Struct* root,
		                 ceps::ast::Nodeset const & universe);
bool contains_struct_defs(Struct* root);
Struct* contains_exactly_one_struct_def_and_nothing_else(Struct* root);

bool is_struct(ceps::ast::Nodebase*);
bool is_test_precondition(ceps::ast::Struct  &);
Struct* contains_exactly_one_child(Struct* root);

ceps::ast::Nodeset instantiate_ast_fragment(ceps::ast::Nodeset const & to_instantiate,ceps::ast::Nodeset const & universe);

template<typename F> void for_each_struct_do(ceps::ast::Nodeset const & ns, F const & f)
		{
			for(auto p : ns.nodes())
			{
				if (p->kind() != Ast_node_kind::structdef)
				continue;
				f(as_struct_ref(p));
			}
		}


struct library_descriptor
{
	ceps::ast::Nodebase_ptr parse_tree_ = nullptr;
	ceps::ast::Nodebase_ptr& parse_tree() { return parse_tree_; }
	ceps::parser_env::Symboltable symbol_table_;
	ceps::parser_env::Symboltable& symbol_table() { return symbol_table_; }
	library_descriptor() = default;
	library_descriptor(ceps::ast::Nodebase_ptr p) :parse_tree_(p){}
};

extern map<string, library_descriptor> libraries;
extern ceps::Ceps_Environment * ceps_env_current;
extern ceps::ast::Nodeset*	current_universe;



#endif

